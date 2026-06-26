#pragma once

#include "allegro5/bitmap.h"
#include "allegro5/bitmap_draw.h"
namespace alui {

/*
 * Redesign notes:
 * * I don't know if keeping slices reusable makes sense. It does for standard slices, but it doesn't for 9-slices,
 *   since 9-slices may need to be cached as a bitmap.
 *   It _could_ be drawn on-demand, but that's 9 draw calls per component
 * * I think it might be worth hooking 9-slices into the resize cycle, so a cached ALLEGRO_BITMAP can be made.
 */

/**
 * Contains raw information about an image. Applications providing images are required to keep the ALLEGRO_BITMAP valid
 * for the duration of the alui::Image. alui::ImageSlice does not take ownership, and does not do cleanup of the image
 * assets.
 */
struct ImageSlice {
    ALLEGRO_BITMAP* bmp;
    float x, y, w, h;

    ImageSlice(
        ALLEGRO_BITMAP* bmp,
        float x,
        float y,
        float w,
        float h
    ) : bmp(bmp), x(x), y(y), w(w), h(h) {}

    /**
     * Renders the image at the given location, with the parameters dx, dy, dw, and dh representing the target
     * location's position.
     */
    void render(
        float dx,
        float dy,
        float dw,
        float dh
    ) {
        al_draw_scaled_bitmap(
            bmp,
            x, y,
            w, h,
            dx, dy,
            dw, dh,
            0
        );
    }
};

}
