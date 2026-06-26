#include "ImageComponent.hpp"

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

}
