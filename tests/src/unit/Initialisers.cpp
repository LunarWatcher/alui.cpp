#include "alui/Component.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace alui;

TEST_CASE("Sizing", "[Initialisers]") {
    SECTION("All sides equal") {
        Sizing s{10};
        REQUIRE(s.bot == 10);
        REQUIRE(s.top == 10);
        REQUIRE(s.left == 10);
        REQUIRE(s.right == 10);
    }
} 
