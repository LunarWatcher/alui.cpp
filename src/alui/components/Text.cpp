#include "Text.hpp"
#include "allegro5/bitmap.h"
#include "alui/Component.hpp"
#include "alui/GUI.hpp"
#include <cmath>

namespace alui {

Text::Text(const std::string& str) : content(str) {

}

void Text::render(GUI& ctx) {
    Component::render(ctx);
    //al_set_clipping_rectangle(
        //std::floor(x),
        //std::floor(y), 
        //std::floor(width),
        //std::floor(height)
    //);

    al_draw_text(
        font == nullptr ? ctx.getFont() : font,
        textColour,
        this->computedX, this->computedY,
        0,
        content.c_str()
    );
    al_reset_clipping_rectangle();
}

}
