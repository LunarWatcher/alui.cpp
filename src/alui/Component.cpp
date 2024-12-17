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
        this->computedWidth, 
        this->computedHeight, 
        colour, 
        1
    );
#endif
}

float Component::computeSizeRequirements(FlexDirection dir) {
    return
        std::max(
            dir == FlexDirection::HORIZONTAL ? unwrap(f.minWidth, 0) : unwrap(f.minHeight, 0),
            f.padding.getSizeForDimension(dir)
        );
}

void Component::setFlex(float flexGrow, float flexShrink) {
    f.flexGrow = flexGrow;
    f.flexShrink = flexShrink;
}

} /* alui */ 
