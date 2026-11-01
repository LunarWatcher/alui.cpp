#include "ImageComponent.hpp"

namespace alui {

ImageComponent::ImageComponent(
    const std::shared_ptr<ImageFactory>& imageFactory,
    const ComponentConfig& cfg
) : Component(cfg), imageFactory(imageFactory) {}

void ImageComponent::render(GUI&, float scrollX, float scrollY) {
    imageSlice->render(
        this->computedX + scrollX,
        this->computedY + scrollY,
        this->computedWidth,
        this->computedHeight
    );
}

float ImageComponent::computeCrossSize(
    FlexDirection dir,
    float virtualMainSize,
    float
) {
    if (dir == FlexDirection::Horizontal) {
        return imageFactory->computeHeight(virtualMainSize);
    } else {
        return imageFactory->computeWidth(virtualMainSize);
    }
}

}
