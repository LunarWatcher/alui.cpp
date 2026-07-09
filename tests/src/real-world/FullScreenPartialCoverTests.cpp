#include "alui/component/FlexDirection.hpp"

#include "alui/components/Button.hpp"
#include "alui/layouts/FlexBox.hpp"
#include "fixtures/Font.hpp"
#include "util/Display.hpp"
#include <catch2/catch_test_macros.hpp>

#include "alui/GUI.hpp"
#include "alui/component/Magnitude.hpp"
#include "util/Style.hpp"

namespace {
/**
 * For most applications, the GUI is going to take 100% of the screen, but not actually consume all of it on GUI
 * components. Notable examples include most game HUDs, that are going to take advantage of the full size of the screen
 * for alignment, but that obviously aren't going to cover the entire screen.
 */
TEST_CASE("100% GUI with partial screen coverage") {
    const auto width = 1280;
    const auto height = 720;
    test::Font font;
    test::Display d(width, height);

    alui::GUI g(
        {
            .font = *font,
            .width = alui::Magnitude::Relative(1.f),
            .height = alui::Magnitude::Relative(1.f)
        }
    );

    SECTION("Relative GUI with no content should scale to 1280x720") {
        g.tick();

        REQUIRE(
            g.getComputedWidth() == width
        );
        REQUIRE(
            g.getComputedHeight() == height
        );
    }

    SECTION("With menu in the middle of the screen") {
        auto layout = std::make_shared<alui::FlexBox>(
            alui::FlexDirection::Horizontal,
            alui::ComponentConfig {
                .flex = 1,
                .id = 0,
                .x = alui::Magnitude::Relative(0.3),
                .y = alui::Magnitude::Absolute(100),
                .padding {
                    10
                },
                .minWidth = alui::Magnitude::Relative(0.4),
                .maxWidth = alui::Magnitude::Relative(0.4),
                .style = test::getDebugStyleSpec(),
            }
        );

        auto btn = std::make_shared<alui::Button>(
            "Click for headpats",
            alui::ComponentConfig {
                .flex = 1,
                .style = test::getDebugStyleSpec(),
            }
        );
        layout->push(
            btn
        );
        g.push(layout);

        g.tick();

        d.captureRender("FullScreenPartialCover-40PercentCentered.bmp", [&]() {
            g.render();
        });

        REQUIRE(layout->getComputedX() == width * 0.3f);
        REQUIRE(layout->getComputedY() == 100);
        // Same math as for the layout, but with +10.f for the layout's padding of 10px
        REQUIRE(btn->getComputedX() == width * 0.3f + 10.f);
        REQUIRE(btn->getComputedY() == 100 + 10);

        REQUIRE(layout->getComputedWidth() == width * 0.4);
        // -20.f: padding, and we expect the button to be smaller as a result.
        REQUIRE(btn->getComputedWidth() == width * 0.4 - 20.f);
        // Magic number (42): this is just the correct height given the font size
        REQUIRE(btn->getComputedHeight() == 42.f);
        // +20f: 10px padding before and 10px padding after
        REQUIRE(layout->getComputedHeight() == 42.f + 20.f);
    }
}

}
