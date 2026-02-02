#pragma once

#include "alui/style/StyleStates.hpp"
#include "allegro5/color.h"

namespace alui {

struct BorderStyle : public AbstractStyle {
    ALLEGRO_COLOR colour;
    float thickness;
    
    BorderStyle(ALLEGRO_COLOR colour, float thickness = 2) 
        : colour(colour), thickness(thickness) {}

    virtual void render(Component*) override;
};

}
