#pragma once

#include "StyleStates.hpp"

namespace alui {

// TODO: this is a shit name
struct StyleSpec {
    StyleStates backgroundStyle;
    StyleStates borderStyle;

    StyleSpec(
        StyleStates&& backgroundStyle = {},
        StyleStates&& borderStyle = {}
    ) : backgroundStyle(
        std::move(backgroundStyle)
    ), borderStyle(
        std::move(borderStyle)
    ) {}

    void resize(
        float width, float height
    ) {
        backgroundStyle.resize(width, height);
        borderStyle.resize(width, height);
    }
};

}
