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
    float, float,
    float parentWidth, float parentHeight
) {
    for (auto& component : this->children) {
        if (auto* layout = dynamic_cast<Layout*>(component.get()); layout != nullptr) {
            layout->recomputeBounds(this,
                0, 0,
                // TODO: This isn't going to work properly I think, at least not with the variables as they're currently
                // defined in the superclass function
                parentWidth,
                parentHeight
            );
        }
    }

    std::cout << "End nested loops" << std::endl;
    std::vector<FlexLine> lines;

    lines.reserve(children.size());

    // TODO: find a better starting size
    // (This will also crash if minHeight is undefined, which is just stupid)
    //
    // §9b2: determine available space (modified)
    auto mainSize = this->f.getMinAxialSize(dir)->compute(
        dir == FlexDirection::HORIZONTAL ? parentWidth : parentHeight
    )
        // Padding is internal, and also an offset on the position (with the right padding, but all of that is a future
        // me problem)
        - this->f.padding.getSizeForDimension(this->dir);

    // TODO: For 9b5, this needs to be per-line
    {
        FlexLine currLine;
        // §9b3: min size computation + 9.3b5 (flex lines): {{{
        for (auto& child : children) {
            FlexAlgoData dataWrapper { child };
            auto& [component, flexBaseSize, hypotheticalMainSize, flexCrossSize, frozen] = dataWrapper;

            auto conf = component->getConfig();

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

            if (currLine.runningMainSize + hypotheticalMainSize <= mainSize) {
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

            // This currently means that the non-axial dimension is maxed out relative to the other elements, so not "true"
            // flexbox, but it's a start
            // TODO: This is ignored due to later code, sort out your shit
            //flexOpposingSize = currLine.maxCrossSize = std::max(
                //currLine.maxCrossSize,
                //flexOpposingSize
            //);

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
    for (auto& [components, runningMainSize, maxCrossSize, factorPool] : lines) {
        float x = this->f.x + this->f.padding.left;
        auto freeSpace = mainSize - runningMainSize;

        for (auto& item : components) {
            if (item.frozen) {
                continue;
            }
            // Allocate remaining free space and compute cross size
            item.flexAxialSize += freeSpace * (item.c->getConfig().flex.grow / factorPool);
            item.flexCrossSize = item.c->computeCrossSize(dir, item.flexAxialSize);
        }


        for (auto& item : components) {
            item.c->updateComputedPos(x, y);
            std::cout << "Set item pos to " << x << "," << y << std::endl;
            auto width = dir == FlexDirection::HORIZONTAL ? item.flexAxialSize : item.flexCrossSize;
            auto height = dir == FlexDirection::HORIZONTAL ? item.flexCrossSize : item.flexAxialSize;
            std::cout << width << "," << height << std::endl;

            item.c->updateComputedSizes(width, height);

            // TODO: + gap
            (dir == FlexDirection::HORIZONTAL ? x : y) += item.flexAxialSize;
            //std::cout << "Next x, y = " << x << ", " << y << std::endl;
        }
        // TODO: + gap
        (dir == FlexDirection::HORIZONTAL ? y : x) += maxCrossSize;
    }
    // }}}

    dirty = false;
    // TODO: sort out computedX/Y
    this->computedX = 0;
    this->computedY = 0;
    this->computedWidth = parentWidth;
    this->computedHeight = std::min(
        parentHeight,
        // TODO: This will not work if gui.y != 0
        y
    );
    std::cout << "Resized flexbox to " << computedWidth << ", " << computedHeight << std::endl;

}

}
