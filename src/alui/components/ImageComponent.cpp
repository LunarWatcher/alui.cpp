#include "ImageComponent.hpp"
#include <iostream>

namespace alui {

ImageComponent::ImageComponent(
    const std::shared_ptr<ImageSlice>& imageSlice,
    const ComponentConfig& cfg
) : Component(cfg), imageSlice(imageSlice) {}

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
        return imageSlice->computeHeight(virtualMainSize);
    } else {
        return imageSlice->computeWidth(virtualMainSize);
    }
}

}
