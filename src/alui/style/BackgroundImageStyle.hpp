#pragma once

#include "alui/images/ImageFactory.hpp"
#include "alui/style/StyleStates.hpp"
#include "alui/component/Component.hpp"

namespace alui {

struct BackgroundImageStyle : public AbstractStyle {
    std::shared_ptr<ImageFactory> imageFactory;

    BackgroundImageStyle(const std::shared_ptr<ImageFactory>& imageFactory)
        : imageFactory(imageFactory) {}

    void render(Component* component, float scrollX, float scrollY) override {
        auto image = imageFactory->resize(
            component->getComputedWidth(),
            component->getComputedHeight()
        );

        image->render(
            scrollX + component->getComputedX(),
            scrollY + component->getComputedY(),
            scrollX + component->getComputedWidth(),
            scrollY + component->getComputedHeight()
        );
    }
};

}
