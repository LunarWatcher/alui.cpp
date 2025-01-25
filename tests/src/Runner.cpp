#define CATCH_CONFIG_RUNNER

#include "catch2/catch_session.hpp"

#include <allegro5/allegro5.h>
#include <iostream>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

int main(int argc, const char* argv[]) {
    if(!al_init()) {
        std::cout << "couldn't initialize allegro" << std::endl;
        return 1;
    }

    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    al_install_keyboard();
    al_install_mouse();

    return Catch::Session().run(argc, argv);
}
