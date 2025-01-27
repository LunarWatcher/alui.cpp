#include "allegro5/allegro_font.h"
#include "alui/components/Text.hpp"
#include <catch2/catch_test_macros.hpp>

#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro5.h>

TEST_CASE("Verify text measuring and wrapping", "[Unit][Text]") {
    std::unique_ptr<ALLEGRO_FONT, decltype(&al_destroy_font)> font(
        al_load_ttf_font("./dejavu.ttf", 36, 0),
        &al_destroy_font
    );
    REQUIRE(font != nullptr);
    SECTION("Soft wrap") {
        alui::Text a("Long wordsssssssssssssssss", {});

        a.setFont(font.get());
        std::vector<std::string> computedLines;
        a.processText([&](const auto& line){
            computedLines.push_back(line);
        }, 100);

        REQUIRE(computedLines.size() == 6);
        REQUIRE(computedLines.at(0) == "Long ");
        REQUIRE(computedLines.at(1) == "word-");
        REQUIRE(computedLines.at(2) == "sssss-");
        REQUIRE(computedLines.at(3) == "sssss-");
        REQUIRE(computedLines.at(4) == "sssss-");
        REQUIRE(computedLines.at(5) == "ss");
    }

    SECTION("Hard wrap") {
        alui::Text a("Long\nword", {});

        a.setFont(font.get());
        std::vector<std::string> computedLines;
        a.processText([&](const auto& line){
            computedLines.push_back(line);
        }, 100);

        REQUIRE(computedLines.size() == 2);
        REQUIRE(computedLines.at(0) == "Long");
        REQUIRE(computedLines.at(1) == "word");
    }

    SECTION("Soft wrap, non-english multi-byte chars") {
        alui::Text a("여우 여우 여우", {});
        a.setFont(font.get());
        std::vector<std::string> computedLines;
        a.processText([&](const auto& line){
            computedLines.push_back(line);
        }, 100);

        REQUIRE(computedLines.size() == 2);
        REQUIRE(computedLines.at(0) == "여우 여우");
        REQUIRE(computedLines.at(1) == " 여우");

    }
}
