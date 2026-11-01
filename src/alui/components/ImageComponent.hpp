#pragma once

#include "allegro5/bitmap.h"
#include "alui/component/Component.hpp"
#include "alui/component/FlexDirection.hpp"
#include "alui/images/Image.hpp"
#include "alui/images/ImageFactory.hpp"

namespace alui {

class ImageComponent : public Component {
protected:
    std::shared_ptr<ImageFactory> imageFactory;
    std::shared_ptr<ImageSlice> imageSlice;
public:
    ImageComponent(
        const std::shared_ptr<ImageFactory>& imageFactory,
        const ComponentConfig& cfg
    );
    virtual void render(GUI& ctx, float scrollX, float scrollY) override;
    virtual void tick() override {}

    float computeCrossSize(FlexDirection dir, float virtualMainSize, float) override;

    void updateComputedSizes(float width, float height) override {
        Component::updateComputedSizes(width, height);

        if (imageSlice == nullptr || imageFactory->shouldRecompute()) {
            imageSlice = imageFactory->resize(width, height);
        }
    }
};

}

