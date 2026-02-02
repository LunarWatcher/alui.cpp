#include "FlexBox.hpp"
#include "alui/Component.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>

namespace alui {

FlexBox::FlexBox(FlexDirection layoutDirection, const ComponentConfig& cfg) : Layout(cfg), dir(layoutDirection) {

}

void FlexBox::recomputeBounds(
    Layout*,
    float parentWidth, float parentHeight
) {
    // TODO: find a better starting size
    // (This will also crash if minHeight is undefined, which is just stupid)
    //
    // §9b2: determine available space (modified)
    auto parentDimension = dir == FlexDirection::HORIZONTAL ? parentWidth : parentHeight;
    auto mainSize = f.getMinAxialSize(dir) ? this->f.getMinAxialSize(dir)->compute(parentDimension)
        // Padding is internal, and also an offset on the position (with the right padding, but all of that is a future
        // me problem)
        - this->f.padding.getSizeForDimension(this->dir)
        : parentDimension - this->f.padding.getSizeForDimension(dir);

    auto internalWidth = f.minWidth ? f.minWidth->compute(parentWidth) : parentWidth;
    internalWidth -= f.padding.getSizeForDimension(FlexDirection::HORIZONTAL);
    auto internalHeight = f.minHeight ? f.minHeight->compute(parentHeight) : parentHeight;
    internalHeight -= f.padding.getSizeForDimension(FlexDirection::VERTICAL);

    std::vector<FlexLine> lines;

    lines.reserve(children.size());

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
                unwrap(conf.getMinAxialSize(dir), minAxialSize),
                unwrap(conf.getMaxAxialSize(dir), mainSize)
            );
            // Negative size is not an option
            if (hypotheticalMainSize < 0) { hypotheticalMainSize = 0; }

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

            flexCrossSize = component->computeCrossSize(
                dir,
                hypotheticalMainSize,
                dir == FlexDirection::HORIZONTAL ? internalHeight : internalWidth
            );
            currLine.maxCrossSize = std::max(
                flexCrossSize,
                currLine.maxCrossSize
            );

            if (
                // inflexible elements will not change sizes
                (conf.flex.grow == 0 && conf.flex.shrink == 0)
                // Elements already at max size cannot grow further
                || unwrap(
                    conf.getMaxAxialSize(dir),
                    std::numeric_limits<float>::max()
                ) <= hypotheticalMainSize
            ) {
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

        float localMaxCrossSize = 0;
        for (auto& item : components) {
            if (!item.frozen) {
                // Allocate remaining free space and compute cross size
                item.flexAxialSize += freeSpace * (item.c->getConfig().flex.grow / factorPool);
            }
                
            item.flexCrossSize = std::clamp(
                item.c->computeCrossSize(
                    dir,
                    item.flexAxialSize,
                    maxCrossSize
                ),
                (dir == FlexDirection::HORIZONTAL ? item.c->getConfig().minHeight : item.c->getConfig().minWidth).value_or(Size {0.f}).compute(maxCrossSize),
                (dir == FlexDirection::HORIZONTAL ? item.c->getConfig().maxHeight : item.c->getConfig().maxWidth).value_or(Size {
                    dir == FlexDirection::HORIZONTAL ? internalHeight : internalWidth
                }).compute(maxCrossSize)
            );

            localMaxCrossSize = std::max(localMaxCrossSize, item.flexCrossSize);
        }

        for (auto& item : components) {
            item.c->updateComputedPos(x, y);
            // TODO: allow for either item.crossSize or maxCrossSize
            auto width = dir == FlexDirection::HORIZONTAL ? item.flexAxialSize : internalWidth;
            auto height = dir == FlexDirection::HORIZONTAL ? localMaxCrossSize : item.flexAxialSize;

            item.c->updateComputedSizes(width, height);

            // TODO: + gap
            (dir == FlexDirection::HORIZONTAL ? x : y) += item.flexAxialSize;
        }
        // TODO: + gap
        (dir == FlexDirection::HORIZONTAL ? y : x) += maxCrossSize;

        maxX = std::max(x + f.padding.right, maxX);
    }
    // }}}
    
    dirty = false;
    // This will not work as expected if this isn't the root-level layout, but I think I'll just... ignore that c:
    this->computedX = this->f.x;
    this->computedY = this->f.y;

    // TODO: The compute params are probably wrong
    this->computedWidth = std::clamp(
        maxX + f.padding.right - computedX,
        f.minWidth.value_or(Size {0.0f}).compute(parentWidth),
        f.maxWidth.value_or(Size {parentWidth}).compute(parentWidth)
    );
    this->computedHeight = std::clamp(
        y - computedY,
        f.minHeight.value_or(Size {0.0f}).compute(parentHeight),
        f.maxHeight.value_or(Size {parentHeight}).compute(parentHeight)
    );
}

float FlexBox::computeSizeRequirements(FlexDirection dir) {
    // The size is the minimum size of the padding of this component, and the minimum size of the biggest inner
    // component.
    // This basically treats the minimum size as being whatever the smallest column/row could be, depending on the
    // direction, by treating it as if there are multiple rows/columns instead.
    return Component::computeSizeRequirements(dir) // NOLINT
        + (*std::max_element(children.begin(), children.end(), [this](const auto& a, const auto& b) {
            return a->computeSizeRequirements(this->dir) < b->computeSizeRequirements(this->dir);
        }))->computeSizeRequirements(this->dir);
}

float FlexBox::computeCrossSize(FlexDirection dir, float virtualMainSize, float maxCrossSize) {
    recomputeBounds(
        nullptr,
        dir == FlexDirection::HORIZONTAL ? virtualMainSize : maxCrossSize, 
        dir == FlexDirection::HORIZONTAL ? std::numeric_limits<float>::max() : virtualMainSize
    );

    return dir == FlexDirection::HORIZONTAL ? computedHeight : computedWidth;
}

}
