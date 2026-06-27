#pragma once

#include "FlexDirection.hpp"

namespace alui {

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
        return dir == FlexDirection::Horizontal ? left + right : top + bot;
    }

    float getCrossSizeForDimension(FlexDirection dir) const {

        return dir == FlexDirection::Horizontal ? top + bot : left + right;
    }
};


}
