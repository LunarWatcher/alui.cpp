#include "Component.hpp"
#include "allegro5/allegro_primitives.h"
#include "allegro5/color.h"
#include "alui/style/Style.hpp"

namespace alui {

bool Component::onClick(float x, float y) {
    bool intersects = this->contains(x, y);

    if (intersects) {
        for (auto& listener : clickListeners) {
            listener(this, x, y);
        }
    }

    return intersects;
}

void Component::render(GUI&) {
    if (f.style != nullptr) {
        // TODO: We'll likely need more control over the rendering in other components, so not sure if it even makes
        // sense to use it here. The border should probably be on top anyway? Not sure if it matters.
        // If it doesn't, we'll probably want to merge the background and border styles.
        f.style->backgroundStyle.render(this);
        f.style->borderStyle.render(this);
    }
}

float Component::computeSizeRequirements(FlexDirection dir) {
    return
        std::max(
            dir == FlexDirection::HORIZONTAL ? unwrap(f.minWidth, 0) : unwrap(f.minHeight, 0),
            f.padding.getSizeForDimension(dir)
        );
}

float Component::computeCrossSize(FlexDirection dir, float, float) {
    // Primitive base implementation: the cross size is the min size for that dimension
    return
        std::max(
            dir == FlexDirection::VERTICAL ? unwrap(f.minWidth, 0) : unwrap(f.minHeight, 0),
            f.padding.getSizeForDimension(dir)
        );
}

bool Component::contains(float x, float y) {
    return x >= this->computedX && x <= this->computedX + this->computedWidth
        && y >= this->computedY && y <= this->computedY + this->computedHeight;
}

void Component::focus() {
    focused = true;
    for (auto& listener : focusListeners) {
        listener(this);
    }
}

} /* alui */
