#include "Layout.hpp"
#include <algorithm>

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
    c->setParent(this);
    this->children.push_back(c);
}

}
