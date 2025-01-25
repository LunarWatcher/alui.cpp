#include "FlexBox.hpp"
#include "alui/Component.hpp"
#include <algorithm>
#include <cassert>

namespace alui {

FlexBox::FlexBox(FlexDirection layoutDirection, const ComponentConfig& cfg) : dir(layoutDirection), Layout(cfg) {

}

void FlexBox::resizeChildren(
    Layout* parent,
    float parentX, float parentY,
    float parentWidth, float parentHeight
) {
    std::vector<FlexAlgoData> components;

    components.reserve(children.size());
    for (auto& child : children) {
        components.push_back({
            child
        });
    }

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

    // Not technically valid under the flexbox spec, but I don't care :)
    auto maxSize = mainSize;

    // TODO: For 9b5, this needs to be per-line
    float runningMainSize = 0;
    float lineMaxOpposingSize = 0;

    // §9b3: min size computation
    for (auto& [component, flexBaseSize, hypotheticalMainSize, flexOpposingSize, frozen] : components) {
        auto conf = component->getConfig();

        auto minAxialSize = component->computeSizeRequirements(this->dir);
        // TODO
        auto minOpposingSize = 0.f;
        flexBaseSize = minAxialSize;

        hypotheticalMainSize = std::clamp(
            flexBaseSize,
            unwrap(conf.getMinAxialSize(dir), 0),
            unwrap(conf.getMaxAxialSize(dir), maxSize)
        );
        if (hypotheticalMainSize < 0) { hypotheticalMainSize = 0; }

        flexOpposingSize = component->computeCrossSize(dir, hypotheticalMainSize);
        lineMaxOpposingSize = std::max(
            flexOpposingSize,
            lineMaxOpposingSize
        );

        //std::cout << "Flex base, hypothetical main: " << flexBaseSize << ", " << hypotheticalMainSize << std::endl;

        // §9b3, closing paragraph
        runningMainSize += hypotheticalMainSize;

        // This currently means that the non-axial dimension is maxed out relative to the other elements, so not "true"
        // flexbox, but it's a start
        // TODO: This is ignored due to later code, sort out your shit
        flexOpposingSize = lineMaxOpposingSize = std::max(
            lineMaxOpposingSize,
            minOpposingSize
        );

        // inflexible elements will not change sizes
        if (conf.flex.grow == 0 && conf.flex.shrink == 0) {
            //std::cout << "Flex preemptively frozen" << std::endl;
            frozen = true;
        }
    }
    // §9.3 (flex lines): TODO
    // Temporary hack; one flex line. Not sure how flex lines are going to be stored, but a .size() can be shoved in
    // instead later
    auto flexLines = 1;
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
    std::vector<FlexAlgoData*> toProcess;

    // First pass; find the factor pool. This could technically be done when building lines, but I'm not going for
    // efficient yet
    // TODO: figure out if
    // TODO: Figure out what past me meant with that unfinished todo
    float factorPool = 0;
    for (auto& it : components) {
        if (!it.frozen) {
            toProcess.push_back(&it);
            factorPool += it.c->getConfig().flex.grow;
        }
    }

    //std::cout << "Factor pool: " << factorPool << std::endl;

    auto freeSpace = mainSize - runningMainSize;
    //std::cout << "Free space: " << freeSpace << std::endl;

    for (auto& item : toProcess) {
        // Allocate remaining free space and compute cross size
        item->flexAxialSize += freeSpace * (item->c->getConfig().flex.grow / ((float) factorPool));
        item->flexCrossSize = item->c->computeCrossSize(dir, item->flexAxialSize);
    }

    float x = this->f.x + this->f.padding.left;
    float y = this->f.y + this->f.padding.top;

    for (auto& item : components) {
        item.c->updateComputedPos(x, y);
        auto width = dir == FlexDirection::HORIZONTAL ? item.flexAxialSize : item.flexCrossSize;
        auto height = dir == FlexDirection::HORIZONTAL ? item.flexCrossSize : item.flexAxialSize;
        //std::cout << width << "," << height << std::endl;

        item.c->updateComputedSizes(width, height);

        (dir == FlexDirection::HORIZONTAL ? x : y) += item.flexAxialSize;
        //std::cout << "Next x, y = " << x << ", " << y << std::endl;
    }
    // }}}

    dirty = false;

}

}
