#pragma once

#include "alui/images/Image.hpp"
#include "alui/images/ImageFactory.hpp"
#include <memory>

namespace alui {

/**
 * Factory for a standard ImageSlice, i.e. one that resizes "normally".
 * Only one instance of an ImageSlice is made.
 */
class ImageSliceFactory : public ImageFactory {
private:
    std::shared_ptr<ImageSlice> instance;
public:
    ImageSliceFactory(
        ALLEGRO_BITMAP* bmp,
        Rect sourceSliceSize
    ) : ImageFactory(bmp, sourceSliceSize),
        instance(
            std::make_shared<ImageSlice>(
                bmp,
                sourceSliceSize
            )
        )
    {}

    virtual float computeHeight(float containerWidth) override {
        return sourceSliceSize.h / sourceSliceSize.w * containerWidth;
    }

    virtual float computeWidth(float containerHeight) override {
        return sourceSliceSize.w / sourceSliceSize.h * containerHeight;
    }

    std::shared_ptr<ImageSlice> resize(float, float) override {
        return instance;
    }
    
};

}
