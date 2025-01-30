#include "allegro5/allegro_primitives.h"
#include <iostream>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/display.h>

#include "allegro5/events.h"
#include "allegro5/timer.h"

#include "alui/Component.hpp"
#include "alui/GUI.hpp"
#include "alui/components/Text.hpp"
#include "alui/layouts/FlexBox.hpp"

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

    auto rootLayout = std::make_shared<alui::FlexBox>(alui::FlexDirection::HORIZONTAL, alui::ComponentConfig {
        .x = 0, .y = 0,
        .minWidth = alui::Size { alui::SizeUnit::ABSOLUTE, 640.f },
        .minHeight = alui::Size { alui::SizeUnit::ABSOLUTE, 480.f },
    });

    auto text = std::make_shared<alui::Text>("Hewwo x3", alui::ComponentConfig {
        .flex{1},
        .minWidth = alui::Size { alui::SizeUnit::ABSOLUTE, 300.f },
    });
    //text->setDimensions(50, 100);
    text->setTextColour(al_map_rgb(255, 255, 255));

    rootLayout->push(text);

    auto text2 = std::make_shared<alui::Text>("x3 Hewwo x3 aaaaaaaaaaaaaaaaaaaaaaaaaaaaa", alui::ComponentConfig {
        .flex{1},
        .padding = 15.0f,
        .minWidth = alui::Size { alui::SizeUnit::ABSOLUTE, 300.f },
    });
    //text->setDimensions(50, 100);
    text2->setTextColour(al_map_rgb(255, 255, 255));

    rootLayout->push(text2);

    auto testText = std::make_shared<alui::Text>("1\n2\n3\n4\n5555555555555555555555555555555", alui::ComponentConfig {
        .flex{1},
        .minWidth = alui::Size { alui::SizeUnit::ABSOLUTE, 300.f },
    });
    testText->setFont(font);
    testText->setTextColour(al_map_rgb(255, 255, 255));

    rootLayout->push(testText);

    gui.pushFront(rootLayout);

    // }}}

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
        }

        if(done) {
            break;
        }

        gui.handleEvent(event);

        if(redraw && al_is_event_queue_empty(queue)) {

            al_clear_to_color(al_map_rgb(0, 0, 0));
            gui.tick();

            gui.render();

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
