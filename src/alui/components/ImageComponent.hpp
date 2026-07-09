#pragma once

#include "allegro5/bitmap.h"
#include "alui/component/Component.hpp"
#include "alui/component/FlexDirection.hpp"
#include "alui/images/Image.hpp"

namespace alui {

class ImageComponent : public Component {
protected:
    std::shared_ptr<ImageSlice> imageSlice;
public:
    ImageComponent(
        const std::shared_ptr<ImageSlice>& imageSlice,
        const ComponentConfig& cfg
    );
    virtual void render(GUI& ctx, float scrollX, float scrollY) override;
    virtual void tick() override {}

    float computeCrossSize(FlexDirection dir, float virtualMainSize, float) override;
};

}

