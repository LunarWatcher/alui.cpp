#include "allegro5/allegro_primitives.h"
#include <iostream>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/display.h>

#include "allegro5/color.h"
#include "allegro5/events.h"
#include "allegro5/timer.h"

#include "alui/Component.hpp"
#include "alui/GUI.hpp"
#include "alui/components/Button.hpp"
#include "alui/components/Text.hpp"
#include "alui/layouts/FlexBox.hpp"
#include "alui/style/BackgroundColourStyle.hpp"
#include "alui/style/BorderStyle.hpp"
#include "alui/style/StyleStates.hpp"
#include <alui/style/Style.hpp>

int main() {
    // General allegro boilerplate {{{
    constexpr auto FONT = "./dejavu.ttf";

    if(!al_init()) {
        std::cout << "couldn't initialize allegro" << std::endl;
        return 1;
    }

    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    al_install_keyboard();
    al_install_mouse();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

    al_set_new_display_flags(
        ALLEGRO_RESIZABLE
    );
    ALLEGRO_DISPLAY* disp = al_create_display(640, 480);
    al_set_window_title(disp, "Choo choo");
    

    ALLEGRO_FONT* font = al_load_ttf_font(FONT, 36, 0);

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_display_event_source(disp));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;
    // }}}
    // alui {{{
    alui::GUI gui({
        .font = font,
        .width = alui::Size { alui::SizeUnit::ABSOLUTE, 640.f },
        .height = alui::Size { alui::SizeUnit::ABSOLUTE, 480.f },
    });
    std::shared_ptr<alui::StyleSpec> buttonStyle = std::make_shared<alui::StyleSpec>(
        alui::StyleStates {
            .defaultState = std::make_shared<alui::BackgroundColourStyle>(
                al_map_rgb(80, 80, 80)
            ),
            .focusedState = std::make_shared<alui::BackgroundColourStyle>(
                al_map_rgb(135, 113, 46)
            )
        },
        alui::StyleStates {
            .defaultState = std::make_shared<alui::BorderStyle>(
                al_map_rgb(83, 27, 70),
                2
            ),
        }
    );

    auto rootLayout = std::make_shared<alui::FlexBox>(alui::FlexDirection::VERTICAL, alui::ComponentConfig {
        .x = 150, .y = 90,
        .minWidth = alui::Size { alui::SizeUnit::ABSOLUTE, 300.f },
        .minHeight = alui::Size { alui::SizeUnit::ABSOLUTE, 300.f },
    });

    auto playBtn = std::make_shared<alui::Button>("Play", alui::ComponentConfig {
        .flex = { 0, 0 },
        .style = buttonStyle
    });
    playBtn->setTextColour(al_map_rgb_f(1, 1, 1));
    playBtn->addClickListener([](auto*, auto, auto) {
        std::cout << "Play owo" << std::endl;
        return true;
    });
    rootLayout->push(playBtn);

    // Currently borked because relative is handled poorly
    // auto veryLongBtn = std::make_shared<alui::Button>(
    //     "A button with an unreasonably long text label that may or may not need to wrap",
    //     alui::ComponentConfig {
    //         .flex = { 1, 1 },
    //         .maxWidth = alui::Size { alui::SizeUnit::RELATIVE, 1.f },
    //         .style = buttonStyle
    //     }
    // );
    // veryLongBtn->setTextColour(al_map_rgb_f(1, 1, 1));
    // rootLayout->push(veryLongBtn);

    auto settingsBtn = std::make_shared<alui::Button>("Settings", alui::ComponentConfig {
        .flex = { 0, 0 },
        .style = buttonStyle
    });
    settingsBtn->setTextColour(al_map_rgb_f(1, 1, 1));
    settingsBtn->addClickListener([](auto*, auto, auto) {
        std::cout << "Settings" << std::endl;
        return true;
    });
    rootLayout->push(settingsBtn);

    gui.push(rootLayout);

    // }}}

    int mx, my;
    al_start_timer(timer);
    while(true) {
        al_wait_for_event(queue, &event);

        switch(event.type) { // NOLINT
        case ALLEGRO_EVENT_TIMER:
            redraw = true;
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        case ALLEGRO_EVENT_MOUSE_WARPED:
        case ALLEGRO_EVENT_MOUSE_AXES: {
            mx = event.mouse.x;
            my = event.mouse.y;
        } break;
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            al_acknowledge_resize(disp);
            gui.resize(
                (float) event.display.width,
                (float) event.display.height
            );
            break;
        }

        if(done) {
            break;
        }

        gui.handleEvent(event);

        if(redraw && al_is_event_queue_empty(queue)) {

            al_clear_to_color(al_map_rgb(0, 0, 0));
            gui.tick();

            gui.render();

            al_draw_filled_circle(
                (float) mx, (float) my, 10,
                al_map_rgb(255, 50, 255)
            );

            al_flip_display();


            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);


    return 0;
}
