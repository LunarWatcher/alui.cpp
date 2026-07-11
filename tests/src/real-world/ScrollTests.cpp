#include "alui/components/Text.hpp"
#include "alui/layouts/FlexBox.hpp"
#include "fixtures/SetupGUI.hpp"
#include "util/Style.hpp"
#include "util/WordSoup.hpp"
#include "util/allegro/EventGenerator.hpp"
#include <catch2/catch_test_macros.hpp>

namespace {

TEST_CASE("Test basic scroll functionality") {
    test::SetupGUI setup;
    auto layout = std::make_shared<alui::FlexBox>(
        alui::FlexDirection::Horizontal,
        alui::ComponentConfig {
            // aggressively constrain size to provoke scrolling
            .minWidth = alui::Magnitude::Absolute(500.f),
            .minHeight = alui::Magnitude::Absolute(500.f),
            .maxWidth = alui::Magnitude::Absolute(500.f),
            .maxHeight = alui::Magnitude::Absolute(500.f),
            .style = test::getDebugStyleSpec(),
        }
    );
    auto text = std::make_shared<alui::Text>(
        test::getWordSoup(300),
        alui::ComponentConfig {}
    );
    text->setTextColour(al_map_rgb(255, 255, 255));
    layout->push(text);

    setup->push(layout);
    setup.autoCapture(
        "BasicScrollPreScroll.bmp"
    );

    REQUIRE(text->getComputedHeight() > 500);
    REQUIRE(text->getComputedWidth() == 500);
    REQUIRE(layout->getScrollY() == 0);

    SECTION("Scroll down") {
        setup->handleEvent(
            test::EventGenerator<ALLEGRO_EVENT_MOUSE_AXES>(setup)
                .asScrollEvent(
                    250,
                    250,
                    test::ScrollDirection::DOWN,
                    10
                )
        );

        setup.autoCapture(
            "BasicScrollPostScroll.bmp"
        );
        // Each scroll tick is 10% of the screen, so we expect the scroll offset to be 720px (since that's the height of
        // the screen).
        // This will be subject to change in the future, since this behaves awkwardly with smaller containers no matter
        // how you go about it.
        REQUIRE(
            layout->getScrollY() == 720
        );

        // Reversing the previous scroll should always return to the same point (in this case, to scrollY == 0)
        setup->handleEvent(
            test::EventGenerator<ALLEGRO_EVENT_MOUSE_AXES>(setup)
                .asScrollEvent(
                    250,
                    250,
                    test::ScrollDirection::UP,
                    10
                )
        );

        REQUIRE(
            layout->getScrollY() == 0
        );
    }

    SECTION("Scroll up with no scroll done") {
        // We also want to verify that it returns false at this point
        REQUIRE_FALSE(layout->scrollY(test::scroll(test::ScrollDirection::UP)));
    }

    SECTION("Scroll at bottom should be noop") {
        setup->handleEvent(
            test::EventGenerator<ALLEGRO_EVENT_MOUSE_AXES>(setup)
                .asScrollEvent(
                    250,
                    250,
                    test::ScrollDirection::DOWN,
                    10000
                )
        );
        setup.autoCapture(
            "BasicScrollPostScrollToBottom.bmp"
        );
        // - layout->getComputedHeight (500px) accounts for the height of the viewport, so we don't scroll past the end
        // of the content.
        REQUIRE(
            layout->getScrollY() == text->getComputedHeight() - layout->getComputedHeight()
        );
        // Since we're at the bottom, further scrolling should be noop
        REQUIRE_FALSE(layout->scrollY(test::scroll(test::ScrollDirection::DOWN)));
    }
}

}
