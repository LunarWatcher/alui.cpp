#pragma once

#include <functional>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "ComponentConfig.hpp"
#include <optional>


namespace alui {

class GUI;

class Component {
public:
    using ClickListener = std::function<bool(Component* c, float x, float y)>;
    using FocusListener = std::function<bool(Component* c)>;

protected:
    /**
     * Computed dimensions of the component, i.e. where the component will be rendered on the screen.
     * Relative in screen pixels.
     */
    float computedX, computedY, computedWidth, computedHeight;

    /**
     * Scroll offset. Only respected if overflow is enabled and happening
     */
    float scrollX, scrollY;

    /**
     * Equivalent to computedWidth and computedHeight if and only if:
     *
     * 1. Overflow is disabled, or
     * 2. Overflow is enabled and the container is small enough that no overflow is happening
     *
     * These are used to keep track of the inner dimensions
     */
    float computedInnerWidth, computedInnerHeight;

    bool dirty = true;
    bool focused = false;

    std::vector<ClickListener> clickListeners;
    std::vector<FocusListener> focusListeners;

    ALLEGRO_FONT* font = nullptr;

    Component* parent;
    ComponentConfig f;

    Component(const ComponentConfig& cfg) : f(cfg) {}

    virtual inline float unwrap(std::optional<Magnitude> orig, float def) {
        // TODO: This does not respect relative sizes 
        if (orig) {
            return orig->value;
        }
        return def;
    }

    /**
     * \brief Utility function that returns the computed width minus padding, i.e. the usable internal width
     */
    virtual float getInternalWidth() { return computedWidth - f.padding.getSizeForDimension(FlexDirection::Horizontal); }
    /**
     * \brief Utility function that returns the computed height minus padding, i.e. the usable internal height
     */
    virtual float getInternalHeight() { return computedHeight - f.padding.getSizeForDimension(FlexDirection::Vertical); }

    /**
     * \brief Utility function that returns the component's x position offset by padding
     */
    virtual float getContentX() { return computedX + f.padding.left; }
    /**
     * \brief Utility function that returns the component's y position offset by padding
     */
    virtual float getContentY() { return computedY + f.padding.top; }

private:
    void clearFocus() { focused = false; }
    void focus();
    

    friend class GUI;
public:
    virtual ~Component() = default;

    virtual void render(GUI& ctx);
    virtual void tick() = 0;


    /**
     * @return Whether or not the component consumed the delay. If true, the event will be considered consumed.
     */
    virtual bool onClick(float x, float y);

    virtual float computeSizeRequirements(FlexDirection dir);
    virtual float computeCrossSize(FlexDirection dir, float virtualMainSize, float maxCrossSize);

    virtual void updateComputedSizes(
        float width, float height
    ) {

        this->computedWidth = width;
        this->computedHeight = height;
    }

    virtual void updateComputedPos(float x, float y) {
        dirty = false;

        this->computedX = x;
        this->computedY = y;
    }

    virtual void setParent(Component* newParent) {
        this->parent = newParent;
    }

    virtual bool isDirty() const { return dirty; }
    virtual void clearDirty() { dirty = false; }
    ComponentConfig& getConfig() { return f; }

    virtual void setMinDimensions(Magnitude width, Magnitude height) {
        f.minWidth = width;
        f.minHeight = height;
    }

    virtual void setMaxDimensions(Magnitude width, Magnitude height) {
        f.maxWidth = width;
        f.maxHeight = height;
    }

    virtual void setDimensions(Magnitude width, Magnitude height) {
        f.minWidth = width;
        f.maxWidth = width;

        f.minHeight = height;
        f.maxHeight = height;
    }

    virtual void setPosition(float x, float y) {
        f.x = x;
        f.y = y;
        this->dirty = true;
    }

    virtual void setPadding(Sizing padding) { this->f.padding = padding; }
    virtual void setMargin(Sizing margin) { this->f.margin = margin; }

    virtual void setFont(ALLEGRO_FONT* font) {
        this->font = font;
    }
    virtual ALLEGRO_FONT* getFont() const { return font; }
    virtual bool contains(float x, float y);

    virtual void addClickListener(ClickListener l) {
        this->clickListeners.push_back(std::move(l));
    }

    virtual bool isFocused() { return focused; }

    float getComputedX() { return computedX; }
    float getComputedY() { return computedY; }
    float getComputedWidth() { return computedWidth; }
    float getComputedHeight() { return computedHeight; }

    std::pair<float, float> getComputedPositions() {
        return {computedX, computedY};
    }

    std::pair<float, float> getComputedSize() {
        return {computedWidth, computedHeight};
    }
};

}
