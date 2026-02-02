#pragma once

#include "alui/style/BorderStyle.hpp"
#include "alui/style/Style.hpp"

inline std::shared_ptr<alui::StyleSpec> getDebugStyleSpec() {
    return std::make_shared<alui::StyleSpec>(
        alui::StyleStates {},
        alui::StyleStates {
            .defaultState = std::make_shared<alui::BorderStyle>(
                al_map_rgb(255, 0, 0),
                3
            )
        }

    );
}
