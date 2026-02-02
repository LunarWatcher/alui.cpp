#include <catch2/catch_test_macros.hpp>

#include "Conf.hpp"
#include "allegro5/color.h"
#include "alui/Component.hpp"
#include "alui/GUI.hpp"
#include "alui/components/Text.hpp"
#include "alui/layouts/FlexBox.hpp"
#include "util/Style.hpp"
#include <iostream>
#include <util/Display.hpp>

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro5.h>

using namespace alui;

TEST_CASE("Verify nested sizing logic", "[FlexBox][Layout]") {
    std::unique_ptr<ALLEGRO_FONT, decltype(&al_destroy_font)> font(
        al_load_ttf_font("./dejavu.ttf", 36, 0),
        &al_destroy_font
    );
    REQUIRE(font != nullptr);
    std::shared_ptr<FlexBox> fb = std::make_shared<FlexBox>(
        FlexDirection::HORIZONTAL,
        ComponentConfig {
            .flex = 1,
            .id = 0,
            .padding{10},
            .minHeight = Size {300.0f},
            .style = getDebugStyleSpec(),
        }
    );

    std::shared_ptr<FlexBox> inner = std::make_shared<FlexBox>(
        FlexDirection::VERTICAL,
        ComponentConfig {
            .flex = 1,
            .id = 1,
            .padding{10},
            .minHeight = Size {230.f},
            .style = getDebugStyleSpec(),
        }
    );

    std::shared_ptr<Text> text = std::make_shared<Text>(
        "Choo choo", 
        ComponentConfig {
            .flex = 0,
            .id = 2,
            .padding{10},
            .minWidth = Size {SizeUnit::ABSOLUTE, 500.0f},
            .minHeight = Size {200.f},
            .style = getDebugStyleSpec(),
        }
    );
    text->setTextColour(al_map_rgb_f(1.0, 1.0, 1.0));

    fb->push(inner);
    inner->push(text);

    GUI g({
        .font = font.get(),
        .width = 600,
        .height = 900,
    });
    g.push(fb);

    REQUIRE(text->getFont() != nullptr);

    SECTION("Validate sizing and resizing") {
        Display disp(1000, 1500);
        g.resize(1000, 1500);
        disp.captureRender(
            "FlexBoxTests-VerifyNestedSizingLogic.bmp",
            [&]() {
                g.render();
            }
        );

        // Verify positions
        REQUIRE(fb->getComputedPositions().first == 0.0f);
        REQUIRE(fb->getComputedPositions().second == 0.0f);
        // Inner is first + first.padding
        REQUIRE(inner->getComputedPositions().first == 10.0f);
        REQUIRE(inner->getComputedPositions().second == 10.0f);
        // Text is inner + inner.padding
        REQUIRE(text->getComputedPositions().first == 20.0f);
        REQUIRE(text->getComputedPositions().second == 20.0f);


        REQUIRE(fb->getComputedSize().first == 600.0f);
        REQUIRE(fb->getComputedSize().second == 300.0f);
        REQUIRE(inner->getComputedSize().first == 580.0f);
        REQUIRE(inner->getComputedSize().second == 230.f);
        REQUIRE(text->getComputedSize().first == 560.0f);
        REQUIRE(text->getComputedSize().second == 200.f);
    }

    SECTION("Validate sizes in non-zero root element positions") {

        fb->getConfig().x = 10;
        fb->getConfig().y = 10;

        Display disp(1000, 1500);
        g.resize(1000, 1500);
        disp.captureRender(
            "FlexBoxTests-VerifySizingLogicWithOffsetXY.bmp",
            [&]() {
                g.render();
            }
        );

#define POSITION(elementPos, rootPos) elementPos + rootPos

        // Verify positions
        REQUIRE(fb->getComputedPositions().first == POSITION(0.0f, fb->getConfig().x));
        REQUIRE(fb->getComputedPositions().second == POSITION(0.0f, fb->getConfig().y));
        // Inner is first + first.padding
        REQUIRE(inner->getComputedPositions().first == POSITION(10.0f, fb->getConfig().x));
        REQUIRE(inner->getComputedPositions().second == POSITION(10.0f, fb->getConfig().y));

        // Text is inner + inner.padding
        REQUIRE(text->getComputedPositions().first == POSITION(20.0f, fb->getConfig().x));
        REQUIRE(text->getComputedPositions().second == POSITION(20.0f, fb->getConfig().y));


        REQUIRE(fb->getComputedSize().first == 600.0f);
        REQUIRE(fb->getComputedSize().second == 300.0f);
        REQUIRE(inner->getComputedSize().first == 580.0f);
        REQUIRE(inner->getComputedSize().second == 230.f);
        REQUIRE(text->getComputedSize().first == 560.0f);
        REQUIRE(text->getComputedSize().second == 200.f);
    }

    SECTION("Validate initial state") {
        REQUIRE(text->getConfig().getMinAxialSize(FlexDirection::VERTICAL).has_value());
        REQUIRE(text->getConfig().getMinAxialSize(FlexDirection::VERTICAL).value().value == 200);
    }

    SECTION("Validate component size calculation functions") {
        REQUIRE(text->computeSizeRequirements(FlexDirection::VERTICAL) == 62); // text height (ish)
        REQUIRE(text->computeCrossSize(FlexDirection::VERTICAL, 280, 560) == 560);
    }
}

TEST_CASE("Layout wrapping, single horizontal layout", "[FlexBox][Layout]") {
    std::unique_ptr<ALLEGRO_FONT, decltype(&al_destroy_font)> font(
        al_load_ttf_font("./dejavu.ttf", 36, 0),
        &al_destroy_font
    );

    auto rootLayout = std::make_shared<alui::FlexBox>(alui::FlexDirection::HORIZONTAL, alui::ComponentConfig {
        .x = 0, .y = 0,
        .minWidth = alui::Size { alui::SizeUnit::ABSOLUTE, 640.f },
        .minHeight = alui::Size { alui::SizeUnit::ABSOLUTE, 480.f },
        .style = getDebugStyleSpec(),
    });

    auto text = std::make_shared<alui::Text>("Hewwo x3", alui::ComponentConfig {
        .flex{1},
        .minWidth = alui::Size { alui::SizeUnit::ABSOLUTE, 300.f },
        .maxWidth = alui::Size { alui::SizeUnit::ABSOLUTE, 300.f },
        .style = getDebugStyleSpec(),
    });
    //text->setDimensions(50, 100);
    text->setTextColour(al_map_rgb(255, 255, 255));

    rootLayout->push(text);

    auto text2 = std::make_shared<alui::Text>("x3 Hewwo x3 aaaaaaaaaaaaaaaaaaaaaaaaaaaaa", alui::ComponentConfig {
        .flex{1},
        .padding = 15.0f,
        .maxWidth = alui::Size { alui::SizeUnit::ABSOLUTE, 340.f },
        .style = getDebugStyleSpec(),
    });
    //text->setDimensions(50, 100);
    text2->setTextColour(al_map_rgb(255, 255, 255));

    rootLayout->push(text2);

    auto forcedSoftWrap = std::make_shared<alui::Text>("1\n2\n3\n4\n5555555555555555555555555555555", alui::ComponentConfig {
        .flex{1},
        .minWidth = alui::Size { alui::SizeUnit::ABSOLUTE, 300.f },
        .style = getDebugStyleSpec(),
    });
    forcedSoftWrap->setFont(font.get());
    forcedSoftWrap->setTextColour(al_map_rgb(255, 255, 255));

    rootLayout->push(forcedSoftWrap);
    GUI g({
        .font = font.get(),
        .width = 640,
        .height = 480,
    });
    g.push(rootLayout);

    SECTION("Validate sizing and resizing") {
        Display disp(1000, 1500);
        g.resize(640, 480);
        disp.captureRender(
            "FlexBoxTests-VerifyTextSizingInHorizontal.bmp",
            [&]() {
                g.render();
            }
        );

        REQUIRE(rootLayout->getComputedSize().first == 640.0f);
        REQUIRE(rootLayout->getComputedSize().second == 480.0f);

        REQUIRE(text->getComputedSize().first == 300.0f);
        REQUIRE(text->getComputedSize().second == 156.0f);
        REQUIRE(text2->getComputedSize().first == 340.0f);
        REQUIRE(text2->getComputedSize().second == 156.0f);

        REQUIRE(forcedSoftWrap->getComputedSize().first == 640.0f);
        REQUIRE(forcedSoftWrap->getComputedSize().second == 252.0f);

        REQUIRE(text->getComputedPositions().first == 0.f);
        REQUIRE(text->getComputedPositions().second == 0.f);

        REQUIRE(text2->getComputedPositions().first == 300.f);
        REQUIRE(text2->getComputedPositions().second == 0.f);

        REQUIRE(forcedSoftWrap->getComputedPositions().first == 0.f);
        REQUIRE(forcedSoftWrap->getComputedPositions().second == 156.f);
    }
}
