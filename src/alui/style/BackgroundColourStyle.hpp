#pragma once

#include "allegro5/color.h"
#include "alui/Component.hpp"
#include "alui/style/Style.hpp"
#include "alui/style/StyleStates.hpp"

namespace alui {

struct BackgroundColourStyle : public AbstractStyle {
    ALLEGRO_COLOR backgroundColour;

    BackgroundColourStyle(const ALLEGRO_COLOR& colour)
        : backgroundColour(colour) {}

    virtual void render(Component*) override;
};

}
