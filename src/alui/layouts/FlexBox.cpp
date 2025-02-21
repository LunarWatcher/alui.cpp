#include "FlexBox.hpp"
#include "alui/Component.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

namespace alui {

FlexBox::FlexBox(FlexDirection layoutDirection, const ComponentConfig& cfg) : Layout(cfg), dir(layoutDirection) {

}

void FlexBox::recomputeBounds(
    Layout*,
    float parentWidth, float parentHeight
) {
    std::cout << "Invoked with w,h=" << parentWidth << "," << parentHeight << std::endl;
    // TODO: find a better starting size
    // (This will also crash if minHeight is undefined, which is just stupid)
    //
    // §9b2: determine available space (modified)
    auto mainSize = f.getMinAxialSize(dir) ? this->f.getMinAxialSize(dir)->compute(
        dir == FlexDirection::HORIZONTAL ? parentWidth : parentHeight
    )
        // Padding is internal, and also an offset on the position (with the right padding, but all of that is a future
        // me problem)
        - this->f.padding.getSizeForDimension(this->dir) : 0;

    auto innerWidth = f.minWidth ? f.minWidth->compute(parentWidth) - f.padding.getSizeForDimension(FlexDirection::HORIZONTAL)
        : parentWidth - f.padding.getSizeForDimension(dir);

    // Nested layouts strategy:
    // 1. Recursively determine the size of the layouts
    // 2. Through the flexbox algorithm, determine the position of the elements
    // 3. Recursively update the position of the children of the nested layouts. All computations assume x = 0, so
    // recomputing down the tree is fairly easy.

    // TODO: Layouts should not take the parent position, nor should positions be computed here. Positions should be
    // determined relative to the parent layout, and they should all be processed in a single go in the GUI class

    std::vector<FlexLine> lines;

    lines.reserve(children.size());


    // TODO: For 9b5, this needs to be per-line
    {
        FlexLine currLine;
        // §9b3: min size computation + 9.3b5 (flex lines): {{{
        for (auto& child : children) {
            FlexAlgoData dataWrapper { child };
            auto& [component, flexBaseSize, hypotheticalMainSize, flexCrossSize, frozen] = dataWrapper;

            auto conf = component->getConfig();
            if (auto* layout = dynamic_cast<Layout*>(component.get()); layout != nullptr) {
                layout->recomputeBounds(this,
                    innerWidth,
                    parentHeight
                );
            }

            auto minAxialSize = component->computeSizeRequirements(this->dir);
            flexBaseSize = minAxialSize;

            hypotheticalMainSize = std::clamp(
                flexBaseSize,
                unwrap(conf.getMinAxialSize(dir), 0),
                unwrap(conf.getMaxAxialSize(dir), mainSize)
            );
            // Negative size is not an option
            if (hypotheticalMainSize < 0) { hypotheticalMainSize = 0; }

            //std::cout << "Flex base, hypothetical main: " << flexBaseSize << ", " << hypotheticalMainSize << std::endl;

            if (
                // Vertical lines do not support multiple lines in HTML, so this doesn't either
                dir == FlexDirection::VERTICAL
                // Otherwise, check if the main size so far plus the size of the next elements exceeds the total size
                // for the row
                || currLine.runningMainSize + hypotheticalMainSize <= mainSize) {
                currLine.runningMainSize += hypotheticalMainSize;
            } else {
                lines.push_back(currLine);
                currLine = {
                    { },
                    hypotheticalMainSize,
                    flexCrossSize
                };
            }

            flexCrossSize = component->computeCrossSize(dir, hypotheticalMainSize);
            currLine.maxCrossSize = std::max(
                flexCrossSize,
                currLine.maxCrossSize
            );

            // inflexible elements will not change sizes
            if (conf.flex.grow == 0 && conf.flex.shrink == 0) {
                //std::cout << "Flex preemptively frozen" << std::endl;
               frozen = true;
            } else {
                currLine.factorPool += component->getConfig().flex.grow;
            }

            currLine.components.push_back(dataWrapper);
        }

        if (!currLine.components.empty()) {
            lines.push_back(currLine);
        }
    }
    // }}}
    // § 9.4: cross size {{{
    //for (auto& it : components) {
        //// TODO(fix): this assumes height: 100% (or whatever the flex equivalent is)
        //it.flexOpposingSize = lineMaxOpposingSize;
    //}
    // }}}

    // § 9.7 {{{
    // 1. Sum hypothetical main sizes [Done separately]
    // 2. Not actionable, done separately
    // 3. Size inflexible items: mostly done already for 9b3
    // 4. same as  runningMainSize at this time, not sure if that's right
    // This loop only needs to happen if caring about min-max violations which, for now, I don't
    float y = this->f.y + this->f.padding.top;
    float maxX = 0;
    for (auto& [components, runningMainSize, maxCrossSize, factorPool] : lines) {
        float x = this->f.x + f.padding.left;
        auto freeSpace = mainSize - runningMainSize;


        for (auto& item : components) {
            if (item.frozen) {
                continue;
            }
            // Allocate remaining free space and compute cross size
            item.flexAxialSize += freeSpace * (item.c->getConfig().flex.grow / factorPool);
            item.flexCrossSize = std::clamp(
                item.c->computeCrossSize(dir, item.flexAxialSize),
                (dir == FlexDirection::HORIZONTAL ? f.minHeight : f.minWidth).value_or(Size {0.f}).compute(maxCrossSize),
                (dir == FlexDirection::HORIZONTAL ? f.maxHeight : f.maxWidth).value_or(Size {
                    dir == FlexDirection::HORIZONTAL ? parentHeight : parentWidth
                }).compute(maxCrossSize)
            );
        }


        for (auto& item : components) {
            item.c->updateComputedPos(x, y);
            //std::cout << "Set item pos to " << x << "," << y << std::endl;
            auto width = dir == FlexDirection::HORIZONTAL ? item.flexAxialSize : item.flexCrossSize;
            auto height = dir == FlexDirection::HORIZONTAL ? item.flexCrossSize : item.flexAxialSize;
            //std::cout << width << "," << height << std::endl;

            item.c->updateComputedSizes(width, height);

            // TODO: + gap
            (dir == FlexDirection::HORIZONTAL ? x : y) += item.flexAxialSize;
            std::cout << "item size in axis is " << item.flexAxialSize << std::endl;
            //std::cout << "Next x, y = " << x << ", " << y << std::endl;
        }
        // TODO: + gap
        (dir == FlexDirection::HORIZONTAL ? y : x) += maxCrossSize;

        maxX = std::max(x, maxX);
    }
    // }}}

    dirty = false;
    // TODO: sort out computedX/Y
    this->computedX = 0;
    this->computedY = 0;
    // TODO: The compute params are probably wrong
    this->computedWidth = std::clamp(
        maxX,
        f.minWidth.value_or(Size {0.0f}).compute(parentWidth),
        f.maxWidth.value_or(Size {parentWidth}).compute(parentWidth)
    );
    this->computedHeight = std::clamp(
        y,
        f.minHeight.value_or(Size {0.0f}).compute(parentHeight),
        f.maxHeight.value_or(Size {parentHeight}).compute(parentHeight)
    );
    std::cout << "Resized flexbox to " << computedWidth << ", " << computedHeight << std::endl;

}

}
