#include "alui/component/SizeUnit.hpp"
#include "fixtures/Font.hpp"
#include "util/Display.hpp"
#include <catch2/catch_test_macros.hpp>

#include "alui/GUI.hpp"
#include "alui/component/Size.hpp"

namespace {
/**
 * For most applications, the GUI is going to take 100% of the screen, but not actually consume all of it on GUI
 * components. Notable examples include most game HUDs, that are going to take advantage of the full size of the screen
 * for alignment, but that obviously aren't going to cover the entire screen.
 */
TEST_CASE("100% GUI with partial screen coverage") {
    test::Font font;
    test::Display d(1280, 720);

    alui::GUI g(
        {
            .font = *font,
            .width = alui::Size {
                alui::SizeUnit::RELATIVE,
                1.f
            },
            .height = alui::Size {
                alui::SizeUnit::RELATIVE,
                1.f
            }
        }
    );

    SECTION("GUI with no content should scale to 1280x720") {
        g.resize(1280, 720);

        REQUIRE(
            g.getComputedWidth() == 1280
        );
        REQUIRE(
            g.getComputedHeight() == 720
        );
    }
}

}
