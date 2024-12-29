#include "Text.hpp"
#include "allegro5/allegro_font.h"
#include "allegro5/bitmap.h"
#include "alui/Component.hpp"
#include "alui/GUI.hpp"
#include <limits>
#include <cmath>
#include <stdexcept>
#include <string>

namespace alui {

Text::Text(const std::string& str, const ComponentConfig& cfg) : content(str), Component(cfg) {

}

void Text::render(GUI& ctx) {
    Component::render(ctx);
    //al_set_clipping_rectangle(
        //std::floor(x),
        //std::floor(y),
        //std::floor(width),
        //std::floor(height)
    //);

    al_draw_multiline_text(
        font,
        textColour,
        getContentX(), getContentY(),
        getInternalWidth(),
        (float) al_get_font_line_height(font), 0, this->content.c_str()
    );

    al_reset_clipping_rectangle();
}

float Text::computeCrossSize(FlexDirection dir, float virtualMainSize) {
    auto computeCallback = [](int, const char*, int, void* lineCount) -> bool {
        // This feels wrong
        *((float*) lineCount) += 1;
        return true;
    };

    float lineCount = 0;
    al_do_multiline_text(
        font,
        // TODO: figure out a better solution for when FlexDirection::VERTICAL
        dir == FlexDirection::HORIZONTAL ? virtualMainSize : std::numeric_limits<float>::max(),
        content.c_str(),
        computeCallback,
        &lineCount
    );

    //std::cout << lineCount << std::endl;

    if (dir == FlexDirection::HORIZONTAL) {
        // TODO: cache somewhere?
        // TODO: check if the height includes spacing
        auto lineHeight = al_get_font_line_height(font);

        return lineCount * (float) lineHeight;
    } else {
        [[unlikely]]
        throw std::runtime_error("Not implemented");
    }
}

}
