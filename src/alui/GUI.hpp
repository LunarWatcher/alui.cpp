#pragma once

#include "Component.hpp"
#include "allegro5/allegro_font.h"
#include "alui/Layout.hpp"

#include <allegro5/events.h>
#include <deque>
#include <memory>

namespace alui {

/**
 * \brief Root GUI class.
 *
 * Note that unlike Components, a GUI can have multiple components as direct children. Also note that just because you
 * can doesn't mean you should; this class has no layout mechanics, meaning that if the Component elements aren't set,
 * they'll all default to (0, 0). You _should_ only add layouts to the GUI.
 * You also can't style the GUI class directly. You _can_ set 
 *
 * This also serves as a utility class to wrap UIs that are placed in separate areas, as well as a unified input adapter
 * system.
 */
class GUI {
private:
    std::deque<std::shared_ptr<Component>> rootComponents;
    ALLEGRO_FONT* font;

    float x, y, width, height;

public:
    GUI(ALLEGRO_FONT* font);

    // Handles updates and animations, if I ever implement any. We'll see how deep this rabbit hole goes
    void tick();
    void render();

    void handleEvent(const ALLEGRO_EVENT& ev);

    /**
     * Pushes a component in front of the other current components.
     *
     * This is the funciton you should normally be using, unless you want to push a component that appears behind
     * another already-visible component for some reason.
     */
    void pushFront(std::shared_ptr<Layout> component);

    /**
     * Pushes a component behind the other current elements.
     *
     * This is not the function you should normally be using. You normally want to use pushFront, as this makes new
     * components render on top of the old components. You should only use this if you _need_ the element to appear
     * behind existing elements. If you don't, or if there's no overlap, use pushFront instead.
     *
     */
    void pushBack(std::shared_ptr<Layout> component);

    void resize(int screenWidth, int screenHeight);

    void setPosition(float x, float y) {
        this->x = x;
        this->y = y;
    }

    ALLEGRO_FONT* getFont() { return font; }

};

}
