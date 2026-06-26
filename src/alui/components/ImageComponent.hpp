#pragma once

#include "allegro5/bitmap.h"
#include "alui/Component.hpp"
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
    virtual void render(GUI& ctx) override;
    virtual void tick() override {}
};

}

