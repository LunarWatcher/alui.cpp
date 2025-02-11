#pragma once

/**
 * \defgroup layouts
 * \brief Layouts are special components that can contain other components, and that do various types of alignments and sizing
 */

#include "alui/Component.hpp"
#include <vector>
#include <memory>

namespace alui {

/**
 * \brief Root class for all layouts
 *
 * \ingroup layouts
 */
class Layout : public Component {
protected:
    /**
     * \brief Whether or not to show a scrollbar
     */
    bool showOverflow;

    std::vector<std::shared_ptr<Component>> children;

    virtual void updateComputedPos(float x, float y) override {
        dirty = false;

        this->computedX = x;
        this->computedY = y;

        for (auto& child : children) {
            auto [sx, sy] = child->getComputedPositions();

            child->updateComputedPos(
                sx + x,
                sy + y
            );
        }
    }

public:
    Layout(const ComponentConfig& cfg) : Component(cfg) {}

    virtual void tick() override;
    virtual void render(GUI& ctx) override;

    virtual bool onClick(float x, float y) override;

    virtual void recomputeBounds(
        Layout* parent,
        float parentX, float parentY,
        float parentWidth, float parentHeight
    ) = 0;

    virtual void push(std::shared_ptr<Component> c);
    virtual void setShowOverflow(bool showOverflow) {
        this->showOverflow = showOverflow;
    }
    virtual void setFont(ALLEGRO_FONT* font) override;

    const decltype(children)& getChildren() {
        return children;
    }

};

}
