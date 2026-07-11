#include "FlexBox.hpp"
#include "alui/component/Component.hpp"
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
    auto parentDimension = dir == FlexDirection::Horizontal ? parentWidth : parentHeight;
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access): The fuck are you on about clang-tidy, it is very clearly checked. There's theoretically a race condition here, but like, that applies everywhere
    auto mainSize = f.getMinAxialSize(dir).has_value() ? this->f.getMinAxialSize(dir)->compute(parentDimension)
        // Padding is internal, and also an offset on the position (with the right padding, but all of that is a future
        // me problem)
        - this->f.padding.getSizeForDimension(this->dir)
        : parentDimension - this->f.padding.getSizeForDimension(dir);

    auto internalWidth = f.minWidth ? f.minWidth->compute(parentWidth) : parentWidth;
    internalWidth -= f.padding.getSizeForDimension(FlexDirection::Horizontal);
    auto internalHeight = f.minHeight ? f.minHeight->compute(parentHeight) : parentHeight;
    internalHeight -= f.padding.getSizeForDimension(FlexDirection::Vertical);

    std::vector<FlexLine> lines;

    lines.reserve(children.size());

    {
        FlexLine currLine;
        // §9b3: min size computation + 9.3b5 (flex lines): {{{
        for (auto& child : children) {
            FlexAlgoData dataWrapper { child };
            auto& [component, flexBaseSize, hypotheticalMainSize, flexCrossSize, frozen] = dataWrapper;

            auto conf = component->getConfig();
            auto lo = conf.getMinAxialSize(dir).value_or(
                Magnitude{0.f}
            ).compute(parentDimension);
            auto hi = conf.getMaxAxialSize(dir).value_or(
                Magnitude{ parentDimension }
            ).compute(parentDimension);
            auto minAxialSize = std::clamp(
                component->computeSizeRequirements(this->dir),
                std::min(lo, hi),
                std::max(lo, hi)
            );
            flexBaseSize = minAxialSize;

            {
                auto lo = unwrap(conf.getMinAxialSize(dir), minAxialSize);
                auto hi = unwrap(conf.getMaxAxialSize(dir), mainSize);
                hypotheticalMainSize = std::clamp(
                    flexBaseSize,
                    std::min(lo, hi),
                    std::max(lo, hi)
                );
            }
            // Negative size is not an option
            if (hypotheticalMainSize < 0) { hypotheticalMainSize = 0; }

            if (
                // Vertical lines do not support multiple lines in HTML, so this doesn't either
                // TODO: The above comment is wrong, so we need to deal with it
                // It's only a thing when the height is contrained though, which it normally isn't
                dir == FlexDirection::Vertical
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
                dir == FlexDirection::Horizontal ? internalHeight : internalWidth
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
    auto computedX = this->f.x.compute(parentWidth);
    auto computedY = this->f.y.compute(parentHeight);
    float y = computedY + this->f.padding.top;
    float maxX = 0;
    // This currently isn't fully used, since the current implementation doesn't support vertical flex lines
    // (i.e. lines that go vertically rather than horizontally). This means that y will never backtrack, so after the
    // iteration, $maxY == y \forall \text{FlexDirections}$ - but max(x, y) is so cheap to compute that it might as well
    // be added here for when vertical lines are supported.
    float maxY = y;
    for (auto& [components, runningMainSize, maxCrossSize, factorPool] : lines) {
        float x = computedX + f.padding.left;
        auto freeSpace = mainSize - runningMainSize;

        float localMaxCrossSize = 0;
        for (auto& item : components) {
            if (!item.frozen) {
                // Allocate remaining free space and compute cross size
                item.flexAxialSize += freeSpace * (item.c->getConfig().flex.grow / factorPool);
            }

            // Note to self: clamping the cross size here makes for weird shit with overflow
            // To be more specific, clamping to the min and max constraints of the container means we do not have a
            // concept of internal content size, since it cannot differ from the container.
            // This, interestingly enough, doesn't actually seem to affect scroll, aside the clamping being incorrect.
            // Basically, our flexCrossSizes need to be where overflow goes for any of this to make sense. This way, the
            // `y` value can also properly respect the size of overflow content
            //
            // Since the layouts do clipping now, there's technically no consequence to this anyway.
            // In theory, if overflow is disabled, we can then clamp it, since this also disables the scroll mechanics.
            //
            // Also TODO: can we make overflowSize - containerSize > 0 the metric of when to enable scrolling?
            // 
            // auto lo = (dir == FlexDirection::Horizontal ? item.c->getConfig().minHeight : item.c->getConfig().minWidth)
            //     .value_or(Magnitude {
            //             0.f
            //         }).compute(maxCrossSize);
            // auto hi = (dir == FlexDirection::Horizontal ? item.c->getConfig().maxHeight : item.c->getConfig().maxWidth)
            //     .value_or(Magnitude {
            //             dir == FlexDirection::Horizontal ? internalHeight : internalWidth
            //         }).compute(maxCrossSize);
            item.flexCrossSize = item.c->computeCrossSize(
                dir,
                item.flexAxialSize,
                maxCrossSize
            );
            localMaxCrossSize = std::max(localMaxCrossSize, item.flexCrossSize);
        }

        for (auto& item : components) {
            item.c->updateComputedPos(x, y);
            // TODO: allow for either item.crossSize or maxCrossSize
            auto width = dir == FlexDirection::Horizontal ? item.flexAxialSize : internalWidth;
            auto height = dir == FlexDirection::Horizontal ? (
                item.c->getConfig().flex.expandCrossSize ? localMaxCrossSize : item.flexCrossSize
            ) : item.flexAxialSize;

            item.c->updateComputedSizes(width, height);

            // TODO: + gap
            (dir == FlexDirection::Horizontal ? x : y) += item.flexAxialSize;
        }
        // TODO: + gap
        (dir == FlexDirection::Horizontal ? y : x) += localMaxCrossSize;

        maxX = std::max(x + f.padding.right, maxX);
        maxY = std::max(y, maxY);
    }
    // }}}
    
    dirty = false;

    this->computedX = computedX;
    this->computedY = computedY;

    // TODO: The compute params are probably wrong
    this->computedWidth = std::clamp(
        maxX + f.padding.right - computedX,
        f.minWidth.value_or(Magnitude {0.0f}).compute(parentWidth),
        f.maxWidth.value_or(Magnitude {parentWidth}).compute(parentWidth)
    );
    this->computedHeight = std::clamp(
        maxY + f.padding.bot - computedY,
        f.minHeight.value_or(Magnitude {0.0f}).compute(parentHeight),
        f.maxHeight.value_or(Magnitude {parentHeight}).compute(parentHeight)
    );

    this->verticalScroll->resizeParentContainer(
        this->computedWidth,
        this->computedHeight,
        maxX,
        maxY
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
        dir == FlexDirection::Horizontal ? virtualMainSize : maxCrossSize,
        dir == FlexDirection::Horizontal ? std::numeric_limits<float>::max() : virtualMainSize
    );

    return dir == FlexDirection::Horizontal ? computedHeight : computedWidth;
}

}
