#pragma once

#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"
#include "catch2/catch_test_macros.hpp"

#include <memory>

namespace test {

struct Font {
    std::unique_ptr<ALLEGRO_FONT, decltype(&al_destroy_font)> font {
        al_load_ttf_font("./dejavu.ttf", 36, 0),
        &al_destroy_font
    };

    Font() {
        INFO("font == nullptr means the file failed to load");
        REQUIRE(font != nullptr);
    }

    ALLEGRO_FONT* operator*() const { return font.get(); }
};

}
