#include "BackgroundColourStyle.hpp"
#include "allegro5/allegro_primitives.h"
#include "alui/component/Component.hpp"

namespace alui {

void BackgroundColourStyle::render(Component* component, float scrollX, float scrollY) {
    al_draw_filled_rectangle(
        scrollX + component->getComputedX(),
        scrollY + component->getComputedY(),
        scrollX + component->getComputedX() + component->getComputedWidth(),
        scrollY + component->getComputedY() + component->getComputedHeight(),
        this->backgroundColour
    );
}

}
