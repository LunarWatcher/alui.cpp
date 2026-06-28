#pragma once

#include "allegro5/bitmap.h"
#include "allegro5/bitmap_io.h"
#include "catch2/catch_test_macros.hpp"
#include <memory>
#include <string>
namespace test {

struct AlImage {
    std::unique_ptr<ALLEGRO_BITMAP, decltype(&al_destroy_bitmap)> bmp;

    AlImage(const std::string &name) : bmp(
        al_load_bitmap(name.c_str()),
        &al_destroy_bitmap
    ) {
        INFO("Attempting to load "
            << name
            << ". If this assertion fails, the asset does not exist or otherwise failed to load"
        );
        REQUIRE(bmp != nullptr);
    }
};

}
