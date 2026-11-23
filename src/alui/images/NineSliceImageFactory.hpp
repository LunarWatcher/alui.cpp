#pragma once

#include "allegro5/allegro_primitives.h"
#include "allegro5/bitmap.h"
#include "allegro5/bitmap_draw.h"
#include "allegro5/display.h"
#include "alui/images/ImageFactory.hpp"
#include <iostream>
namespace alui {

class NineSliceImageFactory : public ImageFactory {
private:
    Rect localCenter;
    Rect center;
public:
    NineSliceImageFactory(ALLEGRO_BITMAP* bmp, Rect sourceSliceSize, Rect center);

    std::shared_ptr<ImageSlice> resize(float width, float height) override;

    float computeHeight(float) override {
        return 0;
    }

    float computeWidth(float) override {
        return 0;
    }

    bool shouldRecompute() override {
        return true;
    }
};

}
