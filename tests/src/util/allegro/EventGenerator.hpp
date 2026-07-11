#pragma once

#include "allegro5/events.h"
#include "fixtures/SetupGUI.hpp"
#include "ScrollHelper.hpp"

namespace test {

template <int EventType>
struct EventGenerator {
    ALLEGRO_EVENT ev;
    constexpr EventGenerator(test::SetupGUI& setup) {
        if constexpr (
            EventType == (int) ALLEGRO_EVENT_MOUSE_AXES
            || EventType == (int) ALLEGRO_EVENT_MOUSE_WARPED
            || EventType == (int) ALLEGRO_EVENT_MOUSE_BUTTON_DOWN
            || EventType == (int) ALLEGRO_EVENT_MOUSE_BUTTON_UP
        ) {
            ev.mouse = ALLEGRO_MOUSE_EVENT {
                .type = EventType,
                .source = nullptr,
                .timestamp = 0,
                .display = *setup.display,
                .x = 0, .y = 0, .z = 0, .w = 0,
                .dx = 0, .dy = 0, .dz = 0, .dw = 0,
                .button = 0,
                .pressure = 0,
            };
        } else {
            static_assert(false, "Not sure how to handle this event");
        }
    }

    ALLEGRO_EVENT asScrollEvent(
        int mx,
        int my,
        ScrollDirection dir,
        int magnitude = 1
    ) {
        ev.mouse.x = mx;
        ev.mouse.y = my;
        ev.mouse.dz = test::scroll(dir, magnitude);

        return ev;
    }
};

}
