#pragma once

#include <optional>
#include <memory>

#include "FlexDirection.hpp"
#include "Magnitude.hpp"
#include "Sizing.hpp"

namespace alui {
struct StyleSpec;
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

        bool expandCrossSize = false;

        Flex() : grow(1), shrink(1), basis(0) {}
        Flex(float f) : grow(f), shrink(f) {}
        explicit Flex(float f, bool expandCrossSize) : grow(f), shrink(f), expandCrossSize(expandCrossSize) {}
        Flex(float g, float s) : grow(g), shrink(s) {}
        Flex(float g, float s, float basis) : grow(g), shrink(s), basis(basis) {}
        Flex(float g, float s, float basis, bool expandCrossSize)
            : grow(g), shrink(s), basis(basis), expandCrossSize(expandCrossSize) {}
    } flex { 1 };

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
    Magnitude x = 0;

    /**
     * \brief Y position; only respected for layouts
     *
     * \see [Positioning](docs/Positioning.md)
     */
    Magnitude y = 0;

    Sizing padding{0}, margin{0};

    std::optional<Magnitude> minWidth = std::nullopt;
    std::optional<Magnitude> minHeight = std::nullopt;
    std::optional<Magnitude> maxWidth = std::nullopt;
    std::optional<Magnitude> maxHeight = std::nullopt;

    /**
     * Defines the general style of the component, such as the background style and border style. This does not include
     * styling specific to each component, such as the text colour in the Text component, which is a per-component
     * customisation.
     */
    std::shared_ptr<StyleSpec> style = nullptr;

    std::optional<Magnitude> getMinAxialSize(FlexDirection dir) const {
        return dir == FlexDirection::Horizontal ? minWidth : minHeight;
    }

    std::optional<Magnitude> getMaxAxialSize(FlexDirection dir) const {
        return dir == FlexDirection::Horizontal ? maxWidth : maxHeight;
    }
};

}
