#pragma once

#include "alui/component/Component.hpp"
#include "alui/component/FlexDirection.hpp"
#include "alui/style/BackgroundColourStyle.hpp"
#include "alui/style/StyleStates.hpp"
#include "alui/style/Style.hpp"
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
    float parentX = 0;
    float parentY = 0;
public:
    Scrollbar(FlexDirection scrollDirection)
        : Component(ComponentConfig {
                .style = std::make_shared<StyleSpec>(
                    alui::StyleStates {
                        .defaultState = std::make_shared<BackgroundColourStyle>(
                            al_map_rgb(221, 88, 209)
                        ),
                        .activeState = std::make_shared<BackgroundColourStyle>(
                            al_map_rgb(241, 108, 229)
                        ),
                    }
                )
            }),
          scrollDirection(scrollDirection)
    {
        this->computedX = 0;
        this->computedY = 0;
    }
    virtual ~Scrollbar() = default;

    virtual void tick() override {}
    void render(GUI &ctx, float scrollX, float scrollY) override;

    virtual float getOffset() {
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
            updateScrollbarPosition();
        } else {
            dim = containerSize * containerSize / overflowSize;
            thickness = getThickness();

            scroll(0);
        }
    }

    void updateScrollbarPosition() {
        // Reset
        this->computedX = parentX + parentWidth - getThickness();
        this->computedY = parentY;

        float availableSpace = scrollDirection == FlexDirection::Horizontal ? parentWidth : parentHeight;
        auto progress = this->offset / overflowHeight;

        auto& scrollPos = scrollDirection == FlexDirection::Horizontal ? computedX : computedY;
        scrollPos += availableSpace * progress;
    }

    virtual void moveParentContainer(float x, float y) {
        this->parentX = x;
        this->parentY = y;

        updateScrollbarPosition();
    }

    virtual bool scroll(float delta) {
        auto oldOffset = offset;
        // We always reclamp
        offset = computedWidth == 0 ? 0 : std::clamp<float>(
            offset - delta,
            0,
            overflowHeight - this->parentHeight
        );
        updateScrollbarPosition();
        // Return whether or not we actually did a scroll
        // TODO: this doesn't allow for horizontal scroll
        return
            delta != 0
            && !(oldOffset == 0 && delta > 0)
            && !(oldOffset == overflowHeight - this->parentHeight && delta < 0);
    }

    virtual float getThickness() {
        // TODO: make customizable
        return 10;
    }
};

}
