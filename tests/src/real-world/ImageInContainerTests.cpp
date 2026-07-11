#include "alui/component/FlexDirection.hpp"
#include "alui/components/ImageComponent.hpp"
#include "alui/layouts/FlexBox.hpp"
#include "fixtures/Font.hpp"
#include "fixtures/SetupGUI.hpp"
#include "util/Display.hpp"
#include "util/Style.hpp"
#include <catch2/catch_test_macros.hpp>

namespace {

// The dimensions in this test are likely subject to change
TEST_CASE("Test layout sizing of dynamic landscape image with no expanding elements") {
    test::SetupGUI setup;

    auto testGenerator = [&](float mult) {
        SECTION("Horizontal layout " + std::to_string(mult)) {
            auto layout = std::make_shared<alui::FlexBox>(
                alui::FlexDirection::Horizontal,
                alui::ComponentConfig {
                    .minWidth = alui::Magnitude::Absolute(256 * mult),
                    .maxWidth = alui::Magnitude::Absolute(256 * mult),
                    .style = test::getDebugStyleSpec(),
                }
            );
            auto img = std::make_shared<alui::ImageComponent>(
                setup.imageAssets.fullSlice(
                    setup.imageAssets.d256x128
                ),
                alui::ComponentConfig {
                    .flex = 1,
                    .style = test::getDebugStyleSpec(),
                }
            );
            layout->push(img);

            setup.gui.push(layout);

            setup->tick();
            setup.autoCapture(
                std::format("LandscapeImageHorizontalLayout-ScaleFactor{}.bmp", mult)
            );

            REQUIRE(img->getComputedX() == 0);
            REQUIRE(img->getComputedY() == 0);

            REQUIRE(img->getComputedWidth() == 256 * mult);
            REQUIRE(img->getComputedHeight() == 128 * mult);

            REQUIRE(layout->getComputedWidth() == img->getComputedWidth());
            REQUIRE(layout->getComputedHeight() == img->getComputedHeight());
        }
    };

    testGenerator(0.5);
    testGenerator(1);
    testGenerator(2);
}

TEST_CASE("Test layout sizing of landscape and portrait images") {
    test::SetupGUI setup;

    auto testGenerator = [&](float mult) {
        SECTION("Horizontal layout " + std::to_string(mult)) {
            auto layout = std::make_shared<alui::FlexBox>(
                alui::FlexDirection::Horizontal,
                alui::ComponentConfig {
                    .minWidth = alui::Magnitude::Absolute(256 * mult),
                    .maxWidth = alui::Magnitude::Absolute(256 * mult),
                    .style = test::getDebugStyleSpec(),
                }
            );
            auto landscapeImg = std::make_shared<alui::ImageComponent>(
                setup.imageAssets.fullSlice(
                    setup.imageAssets.d256x128
                ),
                alui::ComponentConfig {
                    .flex = 1,
                    .style = test::getDebugStyleSpec(),
                }
            );
            auto portraitImg = std::make_shared<alui::ImageComponent>(
                setup.imageAssets.fullSlice(
                    setup.imageAssets.d128x256
                ),
                alui::ComponentConfig {
                    .flex = 1,
                    .style = test::getDebugStyleSpec(),
                }
            );
            layout->push(landscapeImg);
            layout->push(portraitImg);

            setup.gui.push(layout);

            setup->tick();
            setup.autoCapture(
                std::format("BothImages-ScaleFactor{}.bmp", mult)
            );

            REQUIRE(landscapeImg->getComputedX() == 0);
            REQUIRE(landscapeImg->getComputedY() == 0);

            // * 0.5: the images compete for the available space with the current flex settings
            REQUIRE(landscapeImg->getComputedWidth() == 256 * mult * 0.5);
            REQUIRE(landscapeImg->getComputedHeight() == 128 * mult * 0.5);

            REQUIRE(portraitImg->getComputedWidth() == 256 * mult * 0.5);
            // for the portraitImg, the computed height = 2 * computedWidth, so the 0.5 cancels out
            // Technically, the above simplifies to 128 * mult, I'm keeping this form to stay consistent with the width
            // of the container. That that width happens to line up with the width of this particular image is
            // coincidental
            REQUIRE(portraitImg->getComputedHeight() == 256 * mult);

            REQUIRE(layout->getComputedWidth() == (landscapeImg->getComputedWidth() + portraitImg->getComputedWidth()));
            // portraitImg will always be the biggest image, so we can check exclusively against its height
            // The layout is horizontal, so only the width needs to care about the combined size of the two child
            // components
            REQUIRE(layout->getComputedHeight() == portraitImg->getComputedHeight());
        }
    };

    testGenerator(0.5);
    testGenerator(1);
    testGenerator(2);
}

}
