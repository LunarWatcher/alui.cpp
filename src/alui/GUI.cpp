#include "GUI.hpp"
#include "allegro5/display.h"
#include "allegro5/events.h"
#include "allegro5/transformations.h"
#include "alui/component/Component.hpp"
#include "alui/Layout.hpp"
#include <algorithm>
#include <iostream>
#include <memory>

namespace alui {

GUI::GUI(const GUIConfig& cfg) : cfg(cfg) {
}

void GUI::tick() {
    bool dirty = this->resized;
    for (auto& component : this->rootComponents) {

        component->tick();
        dirty = dirty || component->isDirty();
    }

    if (dirty) {
        auto* disp = al_get_current_display();
        this->computedWidth = (float) al_get_display_width(disp);
        this->computedHeight = (float) al_get_display_height(disp);

        for (auto& component : this->rootComponents) {
            if (auto* l = static_cast<Layout*>(component.get())) {
                l->recomputeBounds(
                    nullptr,
                    computedWidth,
                    computedHeight
                );
            }
        }
        this->resized = false;
    }
}

void GUI::render() {
    for (auto& component : this->rootComponents) {
        component->render(*this, 0.f, 0.f);
    }
}

bool GUI::handleEvent(const ALLEGRO_EVENT& ev) {
    // TODO: There's no reason _not_ to hook the resize event in here
    switch (ev.type) { // NOLINT
    case ALLEGRO_EVENT_KEY_CHAR:
    case ALLEGRO_EVENT_KEY_UP:
        if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            if (focused) {
                focused->clearFocus();
                focused = nullptr;
                return true;
            }
        }
        // TODO
        break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
    } break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
        const auto* tf = al_get_current_transform();
        float x = (float) ev.mouse.x;
        float y = (float) ev.mouse.y;
        if (tf != nullptr) {
            al_transform_coordinates(tf, &x, &y);
        }

        auto clickedElement = getInterceptedComponent(x, y);
        if (clickedElement != nullptr) {
            focused = clickedElement;
            focused->onClick(x, y);
            return true;
        }

    } break;
    case ALLEGRO_EVENT_MOUSE_AXES:
    case ALLEGRO_EVENT_MOUSE_WARPED: {
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


        if (ev.mouse.dz != 0) {
            // ev.mouse.dz is a small integer (seems to be the number of like mouse steps), so the app needs to
            // implement actual scrolling mechanics
            // Can't be bothered doing smooth scroll, but 10% scrolled of the screen height per wheel tick feels good
            recursiveScrollLayout(
                ev.mouse.x,
                ev.mouse.y,
                ev.mouse.dz * 0.1f * this->computedHeight
            );
        }

        auto focusedElement = getInterceptedComponent(x, y);
        if (focusedElement != nullptr) {
            focused = focusedElement;
            focused->focus();

            return true;
        }

    } break;
    case ALLEGRO_EVENT_DISPLAY_RESIZE: {
        signalResize();
    } break;
    }

    return false;
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

void GUI::signalResize() {
    this->resized = true;
}

std::shared_ptr<Component> GUI::getInterceptedComponent(float x, float y) {
    // In the GUI, it's possible for two components to overlap
    // When this happens, if begin()->end(), the bottom component will be given the event
    // The render is begin()->end(), but this means the components rendered first will be in the back. 
    //
    // The intercepting check therefore has to iterate backwards, since the last component is the top component.
    auto it = std::find_if(
        rootComponents.rbegin(),
        rootComponents.rend(),
        [&x, &y](const auto& ptr) {
            return ptr->contains(x, y);
        }
    );

    if (it == rootComponents.rend()) {
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

bool GUI::recursiveScrollLayout(
    float x,
    float y,
    float delta,
    Layout* currLayout,
    float sx,
    float sy
) {
    // In the GUI, it's possible for two components to overlap
    // When this happens, if begin()->end(), the bottom component will be given the event
    // The render is begin()->end(), but this means the components rendered first will be in the back. 
    //
    // The intercepting check therefore has to iterate backwards, since the last component is the top component.
    if (currLayout == nullptr) {
        auto it = std::find_if(
            rootComponents.rbegin(),
            rootComponents.rend(),
            [&x, &y](const auto& ptr) {
                return ptr->contains(x, y);
            }
        );

        if (it == rootComponents.rend()) {
            return false;
        }
        auto& currIntersect = *it;
        Layout* layoutPtr = dynamic_cast<Layout*>(currIntersect.get());

        // If not a layout, there are no root-level layouts in the position
        if (layoutPtr == nullptr) {
            return false;
        }
        return recursiveScrollLayout(
            x,
            y,
            delta,
            layoutPtr
        );
    }

    sx += currLayout->getScrollX();
    sy += currLayout->getScrollY();

    for (const auto& item : currLayout->getChildren()) {
        // This method only returns layouts, so we don't care about layouts. We're trying to find the deepest nested
        // layout
        auto nextLayout = dynamic_cast<Layout*>(item.get());
        if (nextLayout == nullptr) {
            continue;
        }
        if (item->contains(
                x + sx,
                y + sy
            )
        ) {
            if(!recursiveScrollLayout(
                x, y, delta,
                nextLayout,
                sx, sy
            )) {
                if (!nextLayout->scrollY(delta)) {
                    break;
                }
            }
            return true;
        }
    }
    return currLayout->scrollY(delta);
}

}
