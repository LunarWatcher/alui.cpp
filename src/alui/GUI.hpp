#pragma once

#include "Component.hpp"
#include "allegro5/allegro_font.h"
#include "alui/Layout.hpp"

#include <allegro5/events.h>
#include <deque>
#include <memory>

namespace alui {

struct GUIConfig {
    ALLEGRO_FONT* font;

    Size width, height;
    float x = 0;
    float y = 0;
};

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
 *
 * ## General use
 * ```cpp
 * std::shared_ptr<GUI> gui(someFont);
 * // Add elements
 * ...
 *
 * while (true) {
 *     al_wait_for_event(queue, &event);
 *     ...
 *     gui.handleEvent(event);
 *
 *     if (redraw && al_is_event_queue_empty(queue)) {
 *         gui.tick();
 *         // ... other updates
 *         gui.render();
 *
 *         al_flip_display();
 *         ...
 *     }
 * }
 * ```
 */
class GUI {
protected:
    std::deque<std::shared_ptr<Component>> rootComponents;

    GUIConfig cfg;
    std::shared_ptr<Component> focused;

    std::shared_ptr<Component> getClickedComponent(float x, float y);

public:
    /**
     * \param font          The default font to use for all child components. The font is set automatically when new
     *                      components are added to the hierarchy
     */
    GUI(const GUIConfig& cfg);

    // Handles updates and animations, if I ever implement any. We'll see how deep this rabbit hole goes
    void tick();
    void render();

    /**
     * \brief Processes input events.
     * \param ev    The event to process. The event does not need to be type-checked before being passed to this
     *              function, as function only handles certain specific events.
     * \returns     whether or not the event was handled by the GUI. Can be treated as a consume flag in certain cases.
     * \see         [Allegro's event documentation](https://liballeg.org/a5docs/trunk/events.html)
     *
     * ## Handled events
     *
     * ### Mouse events
     *
     *
     *
     * ### Keyboard events
     *
     * #### Tab
     *
     * Tab is handled similarly to mouse down events, meaning they set a focused component. The exact handling of
     * focused components is determined by the components themselves. Tab (and shift-tab) are handled as an
     * accessibility/keyboard-only feature.
     *
     * #### Typing (input fields only)
     *
     */
    bool handleEvent(const ALLEGRO_EVENT& ev);

    /**
     * Pushes a component in front of the other current components.
     *
     * This is the funciton you should normally be using, unless you want to push a component that appears behind
     * another already-visible component for some reason.
     */
    void push(const std::shared_ptr<Layout>& component);

    /**
     * Pushes a component behind the other current elements.
     *
     * This is not the function you should normally be using. You normally want to use pushFront, as this makes new
     * components render on top of the old components. You should only use this if you _need_ the element to appear
     * behind existing elements. If you don't, or if there's no overlap, use pushFront instead.
     *
     */
    void pushBack(const std::shared_ptr<Layout>& component);

    void resize(float screenWidth, float screenHeight);

    void setPosition(float x, float y) {
        this->cfg.x = x;
        this->cfg.y = y;
    }

    ALLEGRO_FONT* getFont() { return cfg.font; }

};

}
