#pragma once

#include "allegro5/bitmap.h"
#include <alui/data/Rect.hpp>
#include "Image.hpp"
#include <memory>

namespace alui {

/**
 * ImageFactory turns a slice of a bitmap or a full bitmap into an `ImageSlice` that's consumable by components.
 */
class ImageFactory {
protected:
    ALLEGRO_BITMAP* source;
    Rect sourceSliceSize;

public:
    ImageFactory(
        ALLEGRO_BITMAP* bmp,
        Rect sourceSliceSize
    ) : source(bmp), sourceSliceSize(sourceSliceSize) {}
    virtual ~ImageFactory() = default;

    virtual std::shared_ptr<ImageSlice> resize(
        float width, float height
    ) = 0;

    /**
     * Computes the height given a specific container width. This is used during size calculation, and should be defined
     * in a way that makes sense for the image's resize strategy.
     */
    virtual float computeHeight(float containerWidth) = 0;

    /**
     * Computes the width given a specific container height. This is used during size calculation, and should be defined
     * in a way that makes sense for the image's resize strategy.
     */
    virtual float computeWidth(float containerHeight) = 0;

    /**
     * Whether or not a given factory requires recomputing the underlying ImageSlice. This should be false for images
     * that, for example, use standard resize methods, but true if there's another internal bitmap that has to be
     * created for the bitmap (notably 9-slices)  
     */
    virtual bool shouldRecompute() { return false; }
};

}
