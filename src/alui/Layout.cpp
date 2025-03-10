#include "Layout.hpp"
#include <algorithm>
#include <iostream>

namespace alui {

void Layout::tick() {

}


void Layout::render(GUI& ctx) {
    Component::render(ctx);

    for (auto& child : children) {
        child->render(ctx);
    }
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
    std::cout << "Parent is " << parentX << "," << parentY << " in id=" << f.id << std::endl;
    this->computedX = parentX;
    this->computedY = parentY;

    for (auto& child : children) {
        auto [cx, cy] = child->getComputedPositions();
        child->updateComputedPos(cx + parentX, cy + parentY);
    }
}

float Layout::computeSizeRequirements(FlexDirection dir) {
    return (dir == FlexDirection::HORIZONTAL ? computedWidth : computedHeight);
}

float Layout::computeCrossSize(FlexDirection dir, float, float) {
    return (dir == FlexDirection::HORIZONTAL ? computedHeight : computedWidth);
}

}
