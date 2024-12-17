#include "GUI.hpp"
#include "allegro5/allegro_font.h"
#include "alui/Component.hpp"
#include "alui/Layout.hpp"
#include <memory>

namespace alui {

GUI::GUI(ALLEGRO_FONT* font) : font(font) {

}

void GUI::tick() {
    bool dirty = false;
    for (auto& component : this->rootComponents) {
        component->tick();

        if (component->isDirty()) {
            dirty = true;
        }
    }

    if (dirty) {
        // GUI is not a container, so defer resizing to the child components.
        for (auto& component : this->rootComponents) {

            if (auto l = std::dynamic_pointer_cast<Layout>(component)) {
                l->resizeChildren(
                    nullptr,
                    x, y, width, height
                );
            }
        }
    }

}

void GUI::render() {
    for (auto& component : this->rootComponents) {
        component->render(*this);
    }
}

void GUI::handleEvent(const ALLEGRO_EVENT& ev) {

}

void GUI::pushFront(std::shared_ptr<Layout> component) {
    // No, this is not a mistake.
    // push_back means this is rendered last, which means it's rendered on top of everything else (assuming there's
    // overlap anyway)
    this->rootComponents.push_back(component);
    component->setFont(this->font);
}  

void GUI::pushBack(std::shared_ptr<Layout> component) {
    this->rootComponents.push_front(component);
    component->setFont(this->font);
}  

void GUI::resize(int screenWidth, int screenHeight) {
    // TODO
}

}
