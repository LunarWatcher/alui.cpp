#pragma once

namespace test {

enum ScrollDirection {
    DOWN = -1,
    UP = 1,
};

inline constexpr int scroll(ScrollDirection dir, int magnitude = 1) {
    return static_cast<int>(dir) * magnitude;
}

}
