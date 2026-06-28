#pragma once

#include "alui/images/Image.hpp"
#include "fixtures/AlImage.hpp"

namespace test {

struct ImageAssets {
    AlImage eepyfox { "./assets/eepyfox.png" };

    AlImage d256x128 { "./assets/256x128.png" };
    AlImage d128x256 { "./assets/128x256.png" };

    std::shared_ptr<alui::ImageSlice> fullSlice(
        AlImage& img
    ) {
        return std::make_shared<alui::ImageSlice>(
            img.bmp.get(),
            0, 0,
            al_get_bitmap_width(img.bmp.get()),
            al_get_bitmap_height(img.bmp.get())
        );
    }
};

}
