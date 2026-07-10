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

    this->verticalScroll->applyOffset(scrollX, scrollY);
    for (auto& child : children) {
        al_set_clipping_rectangle(
            srcScrollX + this->computedX,
            srcScrollY + this->computedY,
            this->computedWidth,
            this->computedHeight
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
