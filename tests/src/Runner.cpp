#define CATCH_CONFIG_RUNNER

#include "catch2/catch_session.hpp"

#include <allegro5/allegro5.h>
#include <iostream>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include "Conf.hpp"

int main(int argc, const char* argv[]) {
    if(!al_init()) {
        std::cout << "couldn't initialize allegro" << std::endl;
        return 1;
    }

    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();

    al_install_keyboard();
    al_install_mouse();

    auto sess = Catch::Session();

    auto cli = sess.cli() 
        | Catch::Clara::Opt(alui::tests::captureDebugRenders, "debugCapture")
            ["--debugCapture"]
            ("Whether or not to output debug renders from test cases to a bitmap");

    sess.cli(cli);

    if (auto result = sess.applyCommandLine(argc, argv); result != 0) {
        return result;
    }

    std::cout << "Running with custom configuration:\n"
        << std::boolalpha
        << "\tdebugCapture = " << alui::tests::captureDebugRenders << "\n"
        << std::endl;

    return sess.run();
}
