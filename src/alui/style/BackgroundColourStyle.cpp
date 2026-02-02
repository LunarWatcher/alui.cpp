#include "BackgroundColourStyle.hpp"
#include "allegro5/allegro_primitives.h"
#include "alui/Component.hpp"

namespace alui {

void BackgroundColourStyle::render(Component* component) {
    al_draw_filled_rectangle(
        component->getComputedX(),
        component->getComputedY(),
        component->getComputedX() + component->getComputedWidth(),
        component->getComputedY() + component->getComputedHeight(),
        this->backgroundColour
    );
}

}
