#pragma once

#include "alui/GUI.hpp"
#include "fixtures/Font.hpp"
#include "fixtures/ImageAssets.hpp"
#include "util/Display.hpp"
namespace test {

struct SetupConfig {
    int displayWidth = 1280;
    int displayHeight = 720;
    alui::GUIConfig conf {
        // Populated later
        .font = nullptr,
        .width = alui::Magnitude::Relative(1.f),
        .height = alui::Magnitude::Relative(1.f),
    };

    alui::GUIConfig& imbueFont(test::Font& font) {
        conf.font = *font;
        return conf;
    }
};

struct SetupGUI {
    test::Font font;
    test::Display display;
    alui::GUI gui;

    test::ImageAssets imageAssets;

    SetupConfig conf;
    
    SetupGUI(
        SetupConfig&& conf = {}
    ) : display(conf.displayWidth, conf.displayHeight),
        gui(
            conf.imbueFont(font)
        ),
        conf(conf)
    {}

    int width() { return conf.displayWidth; }
    int height() { return conf.displayHeight; }

    void autoCapture(
        const std::string& filename
    ) {
        gui.tick();
        display.captureRender(
            filename.c_str(), [this]() {
                gui.render();
            });
    }

    alui::GUI* operator->() { return &gui; }
};

}
