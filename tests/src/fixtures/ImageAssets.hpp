#pragma once

#include "alui/data/Rect.hpp"
#include "alui/images/ImageFactory.hpp"
#include "alui/images/ImageSliceFactory.hpp"
#include "fixtures/AlImage.hpp"

namespace test {

struct ImageAssets {
    AlImage eepyfox { "./assets/eepyfox.png" };

    AlImage d256x128 { "./assets/256x128.png" };
    AlImage d128x256 { "./assets/128x256.png" };

    std::shared_ptr<alui::ImageFactory> fullSlice(
        AlImage& img
    ) {
        return std::make_shared<alui::ImageSliceFactory>(
            img.bmp.get(),
            alui::Rect(
                0, 0,
                al_get_bitmap_width(img.bmp.get()),
                al_get_bitmap_height(img.bmp.get())
            )
        );
    }
};

}
