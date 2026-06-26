#pragma once

#include <memory>
#include "StyleStates.hpp"

namespace alui {

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
};

}
