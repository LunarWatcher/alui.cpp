#pragma once

namespace alui {

/**
 * Represents a rectangle at point (x,y) with dimensions (w,h), used to form the other three points of the rect.  
 */
struct Rect {
    float x, y, w, h;

    float endX() {
        return x + w;
    }
    float endY() {
        return y + h;
    }

    Rect& operator+(const Rect& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};

}
