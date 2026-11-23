#pragma once

#include "alui/style/StyleStates.hpp"
#include "allegro5/color.h"
#include "alui/component/Component.hpp"

namespace alui {

struct BorderStyle : public AbstractStyle {
    ALLEGRO_COLOR colour;
    float thickness;

    BorderStyle(ALLEGRO_COLOR colour, float thickness = 2)
        : colour(colour), thickness(thickness) {}

    virtual void render(Component*, float scrollX, float scrollY) override;
};

}
