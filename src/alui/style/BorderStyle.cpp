#include "BorderStyle.hpp"
#include "allegro5/allegro_primitives.h"
#include "alui/component/Component.hpp"

namespace alui {

void BorderStyle::render(Component* component, float scrollX, float scrollY) {
    // The thickness is relative to the points, so the border will grow outside the bounding box without the adjustment
    // for the thickness / 2
    // This may be slightly jank if thickness == 1, but I don't care for now.
    al_draw_rectangle(
        scrollX + component->getComputedX() + this->thickness / 2,
        scrollY + component->getComputedY() + this->thickness / 2,
        scrollX + component->getComputedX() + component->getComputedWidth() - this->thickness / 2,
        scrollY + component->getComputedY() + component->getComputedHeight() - this->thickness / 2,
        this->colour,
        this->thickness
    );
}

}
