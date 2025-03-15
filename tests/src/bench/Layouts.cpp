#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "allegro5/color.h"
#include "alui/Component.hpp"
#include "alui/GUI.hpp"
#include "alui/components/Text.hpp"
#include "alui/layouts/FlexBox.hpp"
#include <util/Display.hpp>

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro5.h>

using namespace alui;

TEST_CASE("Benchmark flexbox", "[Benchmark]") {
    BENCHMARK_ADVANCED("Layout time, small nested layout")(Catch::Benchmark::Chronometer m) {
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

        m.measure([&] {
            g.resize(1000, 1500);
        });
    };
}
