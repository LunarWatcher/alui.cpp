#include "Component.hpp"
#include "allegro5/allegro_primitives.h"

namespace alui {

bool Component::onClick(float x, float y) {
    bool intersects = x >= this->computedX && x <= this->computedX + computedWidth
        && x >= this->computedY && y <= this->computedX + computedWidth;

    if (intersects) {
        for (auto& listener : clickListeners) {
            listener(this, x, y);
        }
    }

    return intersects;
}

void Component::render(GUI&) {
#ifdef ALUI_DEBUG_SHAPES
    static ALLEGRO_COLOR colour = al_map_rgb(255, 135, 255);
    al_draw_rectangle(
        this->computedX,
        this->computedY,
        this->computedX + this->computedWidth,
        this->computedY + this->computedHeight,
        colour,
        1
    );

    if (this->f.padding.bot != 0) {
        al_draw_rectangle(
            this->getContentX(),
            this->getContentY(),
            this->getContentX() + this->getInternalWidth(),
            this->getContentY() + this->getInternalHeight(),
            al_map_rgb(255, 158, 0), // orange
            1
        );
    }
#endif
}

float Component::computeSizeRequirements(FlexDirection dir) {
    // TODO: This does not account for padding
    return
        std::max(
            dir == FlexDirection::HORIZONTAL ? unwrap(f.minWidth, 0) : unwrap(f.minHeight, 0),
            f.padding.getSizeForDimension(dir)
        );
}

float Component::computeCrossSize(FlexDirection dir, float) {
    // Primitive base implementation: the cross size is the min size for that dimension
    return
        std::max(
            dir == FlexDirection::VERTICAL ? unwrap(f.minWidth, 0) : unwrap(f.minHeight, 0),
            f.padding.getSizeForDimension(dir)
        );
}

void Component::setFlex(float flexGrow, float flexShrink) {
    f.flexGrow = flexGrow;
    f.flexShrink = flexShrink;
}

} /* alui */
