#pragma once

#include "alui/component/Component.hpp"
#include "alui/component/ComponentConfig.hpp"
#include "alui/component/FlexDirection.hpp"
#include <algorithm>

namespace alui {

/**
 * Rootclass for the scrollbar pseudo-component, a component that may be rendered and used by containers to deal with
 * scroll offset.
 */
class Scrollbar : public Component {
private:
    float offset = 0;
    FlexDirection scrollDirection;

    float parentWidth = 0;
    float parentHeight = 0;
    float overflowWidth = 0;
    float overflowHeight = 0;
public:
    Scrollbar(FlexDirection scrollDirection)
        : Component({}),
          scrollDirection(scrollDirection)
    {}
    virtual ~Scrollbar() = default;

    virtual void tick() override {}

    virtual int getOffset() {
        return offset;
    }

    virtual void applyOffset(float& x, float& y) {
        switch (scrollDirection) {
        case FlexDirection::Horizontal:
            x -= offset;
            break;
        case FlexDirection::Vertical:
            y -= offset;
            break;
        }
    }

    virtual void resizeParentContainer(
        float width,
        float height,
        float overflowWidth,
        float overflowHeight
    ) {
        this->parentWidth = width;
        this->parentHeight = height;
        this->overflowWidth = overflowWidth;
        this->overflowHeight = overflowHeight;

        float containerSize = scrollDirection == FlexDirection::Horizontal ? width : height;
        float overflowSize = scrollDirection == FlexDirection::Horizontal ? overflowWidth : overflowHeight;

        float& dim = scrollDirection == FlexDirection::Horizontal ? computedWidth : computedHeight;
        float& thickness = scrollDirection == FlexDirection::Horizontal ? computedHeight : computedWidth;

        if (containerSize >= overflowSize) {
            // Container content fits within container. Set offset and scrollbar dims to 0
            offset = 0;
            dim = 0;
            thickness = 0;
        } else {
            dim = containerSize * containerSize / overflowSize;
            // TODO: make customizable
            thickness = 10;

            if (offset > overflowSize) {
                offset = overflowSize;
            }
        }
    }

    virtual void scroll(float delta) {
        offset = std::clamp<float>(
            offset - delta,
            0,
            overflowHeight
        );
    }
};

}
