#include "alui/component/Sizing.hpp"
#include <catch2/catch_test_macros.hpp>


TEST_CASE("Sizing", "[Initialisers]") {
    SECTION("All sides equal") {
        alui::Sizing s{10};
        REQUIRE(s.bot == 10);
        REQUIRE(s.top == 10);
        REQUIRE(s.left == 10);
        REQUIRE(s.right == 10);
    }
} 
