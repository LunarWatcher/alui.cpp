#pragma once

/** \file */

#include "allegro5/allegro_font.h"
#include <allegro5/color.h>
#include <functional>
#include <optional>

namespace alui {

class GUI;

/** @enum alui::SizingMethod
 *
 * @brief determines how a component is sized
 */
enum class SizingMethod {
    /**
     * Used for percentage-based layouts that can shrink or grow depending on how much space is free. Note that if no
     * space is free, the elements will be hidden - try to avoid this :)
     *
     * Width and height must be [0, 100]. If a dimension equals 0, it'll be determined by the containing layout.
     */
    RELATIVE,
    /**
     * Used for sizing in absolute pixels. Width and/or height must be >= 0, depending on the containing layout.
     * The width or height can be 0 for it to be automatically determined by the layout.
     */
    ABSOLUTE
};

/** @enum alui::FlexDirection
 *
 * @brief Describes the flexbox container direction
 *
 */
enum class FlexDirection {
    HORIZONTAL,
    VERTICAL
};


struct Sizing {
    float top, bot, left, right;

    /**
     * \brief Initialises the size with all sides set to 0
     */
    Sizing() : Sizing(0.f, 0.f, 0.f, 0.f) {}

    /**
     * \brief Initialises the size with all sides set to the same value
     */
    Sizing(float size) : Sizing(size, size, size, size) {}

    /**
     * \brief Initializes the size with vertical and horizontal dimensions set
     */
    Sizing(float vertical, float horizontal) : Sizing(vertical, vertical, horizontal, horizontal) {}

    /**
     * \brief Initialises the size with each side set separately
     */
    Sizing(float top, float bot, float left, float right) : top(top), bot(bot), left(left), right(right) {}

    float getSizeForDimension(FlexDirection dir) {
        return dir == FlexDirection::HORIZONTAL ? left + right : top + bot;
    }
};

/**
 * \brief Meta struct defining how layouts flex.
 *
 * All layouts are secretly flex in disguise :)
 *
 * \see https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_flexible_box_layout/Basic_concepts_of_flexbox for attribute
 *      descriptions
 * \see layouts for various specific-purpose implementations
 *
 */
struct Flex {

    // TODO: Rename to something that isn't fucking shit
    struct Size {
        SizingMethod type;
        float value;

        float compute(float parentSize) {
            switch(type) {
            case SizingMethod::RELATIVE:
                return parentSize * value / 100.0f;
            case SizingMethod::ABSOLUTE:
                return value;
            }
        }
    };

    float flexGrow = 0;
    float flexShrink = 0;

    Sizing padding, margin;

    std::optional<Size> minWidth;
    std::optional<Size> minHeight;
    std::optional<Size> maxWidth;
    std::optional<Size> maxHeight;

    std::optional<Size> getMinAxialSize(FlexDirection dir) {
        return dir == FlexDirection::HORIZONTAL ? minWidth : minHeight;
    }

    std::optional<Size> getMaxAxialSize(FlexDirection dir) {
        return dir == FlexDirection::HORIZONTAL ? maxWidth : maxHeight;
    }
};

class Component {
public:
    using ClickListener = std::function<bool(Component* c, float x, float y)>;

protected:
    /**
     * Computed dimensions of the component, i.e. where the component will be rendered on the screen.
     * Relative in screen pixels.
     */
    float computedX, computedY, computedWidth, computedHeight;

    bool dirty = true;

    std::vector<ClickListener> clickListeners;

    ALLEGRO_FONT* font = nullptr;

    Component* parent;
    Flex f;

    virtual float unwrap(std::optional<Flex::Size> orig, float def) {
        if (orig) {
            return orig->value;
        }
        return def;
    }

    /**
     * \brief Utility function that returns the computed width minus padding, i.e. the usable internal width
     */
    virtual float getInternalWidth() { return computedWidth - f.padding.getSizeForDimension(FlexDirection::HORIZONTAL); }
    /**
     * \brief Utility function that returns the computed height minus padding, i.e. the usable internal height
     */
    virtual float getInternalHeight() { return computedHeight - f.padding.getSizeForDimension(FlexDirection::VERTICAL); }

    /**
     * \brief Utility function that returns the component's x position offset by padding
     */
    virtual float getContentX() { return computedX + f.padding.left; }
    /**
     * \brief Utility function that returns the component's y position offset by padding
     */
    virtual float getContentY() { return computedY + f.padding.top; }

public:
    virtual ~Component() = default;

    virtual void render(GUI& ctx);
    virtual void tick() = 0;


    /**
     * @return Whether or not the component consumed the delay. If true, the event will be considered consumed.
     */
    virtual bool onClick(float x, float y);

    virtual void setFlex(float flexGrow, float flexShrink);

    virtual float computeSizeRequirements(FlexDirection dir);
    virtual float computeCrossSize(FlexDirection dir, float virtualMainSize);

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

    virtual bool isDirty() { return dirty; }
    virtual void clearDirty() { dirty = false; }
    const Flex& getFlex() { return f; }

    virtual void setMinDimensions(Flex::Size width, Flex::Size height) {
        f.minWidth = width;
        f.minHeight = height;
    }

    virtual void setMaxDimensions(Flex::Size width, Flex::Size height) {
        f.maxWidth = width;
        f.maxHeight = height;
    }

    virtual void setDimensions(Flex::Size width, Flex::Size height) {
        f.minWidth = width;
        f.maxWidth = width;

        f.minHeight = height;
        f.maxHeight = height;
    }

    virtual void setPadding(Sizing padding) { this->f.padding = padding; }
    virtual void setMargin(Sizing margin) { this->f.margin = margin; }

    virtual void setFont(ALLEGRO_FONT* font) {
        this->font = font;
    }
    virtual ALLEGRO_FONT* getFont() { return font; }
};

}
