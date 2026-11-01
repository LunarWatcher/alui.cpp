#pragma once

#include "allegro5/bitmap.h"
#include "allegro5/bitmap_draw.h"
#include "alui/data/Rect.hpp"

namespace alui {

/**
 * Contains raw information about an image. Applications providing images are required to keep the ALLEGRO_BITMAP valid
 * for the duration of the alui::Image. alui::ImageSlice does not take ownership, and does not do cleanup of the image
 * assets.
 */
class ImageSlice {
private:
    ALLEGRO_BITMAP* bmp;
    Rect sliceSize;

    /**
     * Whether or not the bmp is owned by this class.
     * If true, bmp is freed when this class is destroyed.
     */
    bool takeOwnership;

public:
    ImageSlice(
        ALLEGRO_BITMAP* bmp,
        Rect sliceSize,
        bool takeOwnership = false
    ) : bmp(bmp), sliceSize(sliceSize), takeOwnership(takeOwnership) {}
    virtual ~ImageSlice() {
        if (takeOwnership && bmp != nullptr) {
            al_destroy_bitmap(bmp);
        }
    }

    /**
     * Renders the image at the given location, with the parameters dx, dy, dw, and dh representing the target
     * location's position.
     */
    virtual void render(
        float dx,
        float dy,
        float dw,
        float dh
    ) {
        al_draw_scaled_bitmap(
            bmp,
            sliceSize.x, sliceSize.y,
            sliceSize.w, sliceSize.h,
            dx, dy,
            dw, dh,
            0
        );
    }
};

}
