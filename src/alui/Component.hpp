#pragma once

/** \file */

#include "allegro5/allegro_font.h"
#include <allegro5/color.h>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>

namespace alui {

class GUI;

/** @enum alui::SizeUnit
 *
 * @brief determines how sizes are interpreted. 
 */
enum class SizeUnit {
    /**
     * Used for percentage-based layouts that can shrink or grow depending on how much space is free. Note that if no
     * space is free, the elements will be hidden - try to avoid this :)
     *
     * Width and height must be [0, 1]. If a dimension equals 0, it'll be determined by the containing layout. If the
     * percentage exceeds the size of the screen, behaviour, overflow, and rendering behaviour is undefined[^1].
     *
     * [^1]: At the time of writing c:
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


// TODO: better name
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

    float getSizeForDimension(FlexDirection dir) const {
        return dir == FlexDirection::HORIZONTAL ? left + right : top + bot;
    }

    float getCrossSizeForDimension(FlexDirection dir) const {

        return dir == FlexDirection::HORIZONTAL ? top + bot : left + right;
    }
};

struct Size {
    SizeUnit type;
    float value;

    Size() = delete;
    Size(float v) : type(SizeUnit::ABSOLUTE), value(v) {}
    Size(SizeUnit type, float value) : type(type), value(value) {}

    float compute(float parentSize) {
        switch(type) {
        case SizeUnit::RELATIVE:
            return parentSize * value;
        case SizeUnit::ABSOLUTE:
            return value;
        }
        [[unlikely]]
        // .c_str(): Fuck you windows
        throw std::runtime_error((std::string("Fatal: unknown size type: ") + std::to_string(static_cast<int>(type))).c_str());
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
struct ComponentConfig {

    // TODO: Rename to something that isn't fucking shit

    struct Flex {
        float grow;
        float shrink; 

        /**
         * \brief Defines the flex basis. 
         *
         * Note that unlike CSS flex, this value can only be numeric, and 0 and negative numbers have a special meaning.
         * If this value is 0 or negative, it's computed based on the minimum size of the elements. There's no
         * equivalent for many of the special CSS values as this implementation is not a complete 1:1 with CSS.
         */
        float basis = 0;

        Flex() : grow(1), shrink(1), basis(0) {}
        Flex(float f) : grow(f), shrink(f) {}
        Flex(float g, float s) : grow(g), shrink(s) {}
        Flex(float g, float s, float basis) : grow(g), shrink(s), basis(basis) {}
    } flex{1};

    /**
     * \brief Optional component ID
     * 
     * The ID can be used to quickly identify elements in common callbacks and similar.
     *
     * \note ID = 0 is reserved, and means the element has no ID. It's also up to end-user code to set IDs, and to keep
     *      track of which IDs refer to which component.
     *
     * \warning Non-zero ID uniqueness is not systemically enforced. It's up to the invoking code to use IDs correctly.
     *      However, aside error in your code, the only ID duplicated by the library is ID = 0. If you use ID = 0 in your
     *      code, you will run into duplicate IDs. There may also be cases where the ID being duplicated across a UI is
     *      desirable, for example in lists, where the ID just reflects the position of the element. However, such use
     *      of IDs should be 1-indexed to avoid errors with undefined or incorrectly defined IDs that end up defaulting
     *      to 0. There may be cases where duplicate IDs are desired, but this is left as an exercise to end-users.
     */
    int id = 0;


    /**
     * \brief X position; only respected for layouts
     *
     * \see [Positioning](docs/Positioning.md)
     */
    float x = 0;

    /**
     * \brief Y position; only respected for layouts
     *
     * \see [Positioning](docs/Positioning.md)
     */
    float y = 0;

    Sizing padding{0}, margin{0};

    std::optional<Size> minWidth = std::nullopt;
    std::optional<Size> minHeight = std::nullopt;
    std::optional<Size> maxWidth = std::nullopt;
    std::optional<Size> maxHeight = std::nullopt;

    std::optional<Size> getMinAxialSize(FlexDirection dir) const {
        return dir == FlexDirection::HORIZONTAL ? minWidth : minHeight;
    }

    std::optional<Size> getMaxAxialSize(FlexDirection dir) const {
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

    /**
     * Scroll offset. Only respected if overflow is enabled and happening
     */
    float scrollX, scrollY;

    /**
     * Equivalent to computedWidth and computedHeight if and only if 
     * 1. Overflow is disabled, or
     * 2. Overflow is enabled and the container is small enough that no overflow is happening
     *
     * These are used to keep track of the inner dimensions
     */
    float computedInnerWidth, computedInnerHeight;

    bool dirty = true;
    bool focused = false;

    std::vector<ClickListener> clickListeners;


    ALLEGRO_FONT* font = nullptr;

    Component* parent;
    ComponentConfig f;

    Component(const ComponentConfig& cfg) : f(cfg) {}

    virtual float unwrap(std::optional<Size> orig, float def) {
        // TODO: This does not respect relative sizes 
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

private:
    void clearFocus() { focused = false; }
    void focus() { focused = true; }
    

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

    virtual bool isDirty() { return dirty; }
    virtual void clearDirty() { dirty = false; }
    ComponentConfig& getConfig() { return f; }

    virtual void setMinDimensions(Size width, Size height) {
        f.minWidth = width;
        f.minHeight = height;
    }

    virtual void setMaxDimensions(Size width, Size height) {
        f.maxWidth = width;
        f.maxHeight = height;
    }

    virtual void setDimensions(Size width, Size height) {
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
    virtual ALLEGRO_FONT* getFont() { return font; }
    virtual bool contains(float x, float y);

    std::pair<float, float> getComputedPositions() {
        return {computedX, computedY};
    }

    std::pair<float, float> getComputedSize() {
        return {computedWidth, computedHeight};
    }
};

}
