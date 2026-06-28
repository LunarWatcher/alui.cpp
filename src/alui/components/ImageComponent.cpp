#include "ImageComponent.hpp"
#include <iostream>

namespace alui {

ImageComponent::ImageComponent(
    const std::shared_ptr<ImageSlice>& imageSlice,
    const ComponentConfig& cfg
) : Component(cfg), imageSlice(imageSlice) {}

void ImageComponent::render(GUI&) {
    imageSlice->render(
        this->computedX,
        this->computedY,
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
        return imageSlice->computeHeight(virtualMainSize);
    } else {
        return imageSlice->computeWidth(virtualMainSize);
    }
}

}
