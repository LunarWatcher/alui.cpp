#include "GUI.hpp"
#include "allegro5/display.h"
#include "allegro5/events.h"
#include "allegro5/transformations.h"
#include "alui/Component.hpp"
#include "alui/Layout.hpp"
#include <algorithm>
#include <memory>

namespace alui {

GUI::GUI(const GUIConfig& cfg) : cfg(cfg) {
}

void GUI::tick() {
    bool dirty = false;


    for (auto& component : rootComponents) {
        if (component->kill) {
            continue;
        }

        component->tick();
        dirty = dirty || component->isDirty();
    }

    if (dirty) {
        pruneTree();

        auto* disp = al_get_current_display();
        auto displayWidth = (float) al_get_display_width(disp);
        auto displayHeight = (float) al_get_display_height(disp);
        resize(displayWidth, displayHeight);
    }

}

void GUI::render() {
    for (auto& component : this->rootComponents) {
        component->render(*this);
    }
}

bool GUI::handleEvent(const ALLEGRO_EVENT& ev) {
    switch (ev.type) { // NOLINT
        case ALLEGRO_EVENT_KEY_CHAR:
        case ALLEGRO_EVENT_KEY_UP:
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                if (focused) {
                    focused->clearFocus();
                    focused = nullptr;
                }
            }
            // TODO
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
            const auto* tf = al_get_current_transform();
            float x = (float) ev.mouse.x;
            float y = (float) ev.mouse.y;
            if (tf != nullptr) {
                al_transform_coordinates(tf, &x, &y);
            }
            // Force clear the focused element
            if (focused != nullptr) {
                focused->clearFocus();
                focused = nullptr;
            }

            auto clickedElement = getClickedComponent(x, y);
            if (clickedElement != nullptr) {
                focused = clickedElement;
                focused->focus();
            } else {
            }
        } break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP: // NOLINT
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
        case ALLEGRO_EVENT_MOUSE_WARPED:
            break;
    }

    return false;
}

void GUI::pruneTree() {
    // TODO: This can be optimised by making sure each child contains a pointer to its root GUI, and adding a separate
    // variable checking if there are any elements in the tree to kill. Not sure how much computation this saves, but
    // it's a microsecond or two (at release optimisation)
    std::erase_if(rootComponents, [](auto& component) {
        return component->shouldRemove();
    });
    std::for_each(rootComponents.begin(), rootComponents.end(), [](auto& it) {
        // TODO: should this be dynamic_cast? `rootComponents` should be guaranteed not to contain Components (limited
        // by the construction methods), but an overriding implementation _could_ change this
        if (auto* layout = static_cast<Layout*>(it.get())) {
            layout->pruneTree();
        }
    });
}

void GUI::push(const std::shared_ptr<Layout>& component) {
    // No, this is not a mistake.
    // push_back means this is rendered last, which means it's rendered on top of everything else (assuming there's
    // overlap anyway)
    this->rootComponents.push_back(component);
    component->setFont(this->cfg.font);
}  

void GUI::pushBack(const std::shared_ptr<Layout>& component) {
    this->rootComponents.push_front(component);
    component->setFont(this->cfg.font);
}  

void GUI::resize(float displayWidth, float displayHeight) {
    // GUI is not a container, so defer resizing to the child components.
    for (auto& component : this->rootComponents) {

        if (auto* l = static_cast<Layout*>(component.get())) {
            l->recomputeBounds(
                nullptr,
                cfg.width.compute(displayWidth), cfg.height.compute(displayHeight)
            );
        }

    }
}

std::shared_ptr<Component> GUI::getClickedComponent(float x, float y) {
    auto it = std::find_if(
        rootComponents.begin(),
        rootComponents.end(),
        [&x, &y](const auto& ptr) {
            return ptr->contains(x, y);
        }
    );

    if (it == rootComponents.end()) {
        return nullptr;
    }
    auto currIntersect = *it;

    while (true) {
        Layout* layoutPtr = dynamic_cast<Layout*>(currIntersect.get());

        // If the component isn't a layout, the bottom of the stack has been reached; break
        if (layoutPtr == nullptr) {
            break;
        }

        // If the component is a layout, iterate further to try 
        for (const auto& item : layoutPtr->getChildren()) {
            if (item->contains(x, y)) {
                currIntersect = item;
                goto continueOuter;
            }
        }
    
        // No inner matches found; break
        break;
continueOuter:;
    }

    // Whatever component was last intersected is considered clicked
    return currIntersect;
}

}
