#pragma once

/**
 * \defgroup layouts
 * \brief Layouts are special components that can contain other components, and that do various types of alignments and sizing
 */

#include "alui/component/Component.hpp"
#include "alui/components/sub/Scrollbar.hpp"
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
    bool showOverflow = true;

    std::vector<std::shared_ptr<Component>> children;

    // TODO: make customizable
    std::shared_ptr<Scrollbar> verticalScroll = std::make_shared<Scrollbar>(
        FlexDirection::Vertical
    );
public:
    Layout(const ComponentConfig& cfg) : Component(cfg) {}

    virtual void tick() override;
    virtual void render(GUI& ctx, float scrollX, float scrollY) override;

    virtual bool onClick(float x, float y) override;

    virtual void recomputeBounds(
        Layout* parent,
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

    virtual void updateComputedPos(float x, float y) override;
    virtual float computeSizeRequirements(FlexDirection dir) override = 0;
    virtual float computeCrossSize(FlexDirection dir, float virtualMainSize, float maxCrossSize) override = 0;

    virtual bool scrollY(float delta);

    virtual float getScrollX();
    virtual float getScrollY();

    friend class GUI;

};

}
