#pragma once

#include "allegro5/bitmap.h"
#include "allegro5/bitmap_draw.h"
namespace alui {

// TODO: this entire class and all use of it will need to be redesigned for 9-patch support. 9-patches are just so
// special that I don't entirely know how to approach it right now. It all depends on the interface to the components
// that accept 9-patch images, since there's nothing wrong with taking an Image as-is, with an additional 9-patch
// parameter for doing the actual slicing.
// I also want to keep the Images reusable separately, which that approach would support. But it is mildly more annoying
// downstream, since I don't think we can give a raw image in at that point. At some point, we probably need a pointer
// if we want to abstract the actual rendering away to a generic `Image` class (not to be confused with the current
// implementation style) or whatever

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
