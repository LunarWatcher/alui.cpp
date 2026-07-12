#include "Layout.hpp"
#include "allegro5/bitmap.h"
#include <algorithm>

#include <allegro5/allegro.h>

namespace alui {

void Layout::tick() {

}

void Layout::render(GUI& ctx, float scrollX, float scrollY) {
    Component::render(ctx, scrollX, scrollY);

    float srcScrollX = scrollX;
    float srcScrollY = scrollY;

    int px, py, pw, ph;
    al_get_clipping_rectangle(&px, &py, &pw, &ph);

    int renderX = srcScrollX + this->computedX;
    int renderY = srcScrollY + this->computedY;
    int clippingWidth = px + pw - renderX;
    int clippingHeight = py + ph - renderY;

    // Since we're more aggressively constraining the container, we don't need to render this tree or its subtrees if it
    // exceeds its permitted size.
    if (
        // The hitbox is after the parent
        clippingHeight <= 0 || clippingWidth <= 0
        // The hitbox is before the parent
        || renderX + computedWidth < px
        || renderY + computedHeight < py
    ) {
        return;
    }

    this->verticalScroll->applyOffset(scrollX, scrollY);
    for (auto& child : children) {
        // The clipHeight represents the maximum allowed clipping at the scroll position = (renderX, renderY), and does
        // not currently take the actual computedHeight into consideration.
        // This means that the actual render is the min(computedHeight, clipHeight).
        //
        // As far as I can tell, clippingHeight > 0 \forall realistic scroll scenarios
        //
        // Given (px, py) = (0, 0), and (x_n, y_n) = (0, -100), ph = 100, and innerHeight_n > 100
        // where x_n and y_n are the positions of the nested layout, clippingHeight = 0 + 100 - (-100) = 200, which is
        // way more than the actual size. Assuming h_n = 100 as well (note: h_n <= ph = 100 for all realistic
        // containers), we can indeed treat h_n as the size, like what was the case prior to this change.
        //
        // Given (x_n, y_n) = (0, 50), and the other constraints as before, we get 0 + 100 - 50 = 50, which checks out.
        // Given (x_n, y_n) = (0, 110), we get 0 + 100 - 110 = -10 <= 0, which is filtered out.
        //
        // The extra pre-loop checks that check renderY + computedHeight < py (and equivalent for x) should completely
        // filter out
        //
        // All of the scenarios above also apply to x scroll offsets, and follow the same math.
        //
        // The geometry math used here is likely suboptimal, and it may or may not respond weirdly to out of bounds
        // scrolling. I eyeballed the math with some drawings and consulted precisely 0 sources that have already done
        // this better.
        al_set_clipping_rectangle(
            renderX, renderY,
            std::min<int>(computedWidth, clippingWidth),
            std::min<int>(computedHeight, clippingHeight)
        );
        child->render(ctx, scrollX, scrollY);
    }
    al_reset_clipping_rectangle();
}

bool Layout::onClick(float x, float y) {
    // Click does not intersect the layout; skip
    if (!Component::onClick(x, y)) {
        return false;
    }

    return std::any_of(
        children.begin(), children.end(),
        [&](const auto& child) {
            return child->onClick(x, y);
        });
}

void Layout::push(std::shared_ptr<Component> c) { // NOLINT
    dirty = true;
    // If the font is not null, there's probably an existing component hierarchy
    if (font != nullptr && c->getFont() == nullptr) {
        c->setFont(font);
    }
    c->setParent(this);
    this->children.push_back(c);
}

void Layout::setFont(ALLEGRO_FONT* font) {
    Component::setFont(font);

    for (auto& child : children) {
        // Don't override child fonts if they're set already; this is _probably_ only going to apply during
        // initialisation anyway.
        if (child->getFont() == nullptr) {
            child->setFont(font);
        }
    }
}

void Layout::updateComputedPos(float parentX, float parentY) {
    this->computedX = parentX;
    this->computedY = parentY;

    for (auto& child : children) {
        auto [cx, cy] = child->getComputedPositions();
        child->updateComputedPos(cx + parentX, cy + parentY);
    }
}

bool Layout::scrollY(float delta) {
    return verticalScroll->scroll(delta);
}

float Layout::getScrollX() {
    // TODO: not implemented yet
    return 0.f;
}
float Layout::getScrollY() {
    return verticalScroll->getOffset();
}
}
