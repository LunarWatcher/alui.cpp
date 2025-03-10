#include <catch2/catch_test_macros.hpp>

#include "Conf.hpp"
#include "allegro5/color.h"
#include "alui/Component.hpp"
#include "alui/GUI.hpp"
#include "alui/components/Text.hpp"
#include "alui/layouts/FlexBox.hpp"
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
        }
    );

    std::shared_ptr<FlexBox> inner = std::make_shared<FlexBox>(
        FlexDirection::VERTICAL,
        ComponentConfig {
            .flex = 1,
            .id = 1,
            .padding{10},
            .minHeight = Size {230.f},
        }
    );

    std::shared_ptr<Text> text = std::make_shared<Text>(
        "Choo choo", 
        ComponentConfig {
            .flex = 0,
            .id = 2,
            .padding{10},
            .minWidth = Size {SizeUnit::ABSOLUTE, 500.0f},
            .minHeight = Size {200.f}
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
        Display disp(600, 900);
        g.resize(600, 900);
        disp.captureRender(
            "FlexBoxTests::VerifyNestedSizingLogic.bmp",
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
        REQUIRE(inner->getComputedSize().second == 280.f);
        REQUIRE(text->getComputedSize().first == 560.0f);
        REQUIRE(text->getComputedSize().second == 200.f);
    }

    SECTION("Validate initial state") {
        REQUIRE(text->getConfig().getMinAxialSize(FlexDirection::VERTICAL).has_value());
        REQUIRE(text->getConfig().getMinAxialSize(FlexDirection::VERTICAL).value().value == 200);
    }

    SECTION("Validate component size calculation functions") {
        REQUIRE(text->computeSizeRequirements(FlexDirection::VERTICAL) == 200);
        REQUIRE(text->computeCrossSize(FlexDirection::VERTICAL, 280, 560) == 560);
    }
}
