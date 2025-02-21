#include "Display.hpp"
#include "Conf.hpp"
#include "allegro5/bitmap.h"
#include "allegro5/display.h"
#include "allegro5/drawing.h"
#include "allegro5/events.h"

namespace alui {

Display::Display(int width, int height) 
    : display(al_create_display(width, height)),
        queue(al_create_event_queue()),
        width(width), height(height)
{

    al_set_window_title(display, "alui test window");
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
}

Display::~Display() {
    if (display != nullptr) {
        al_destroy_display(display);
    }
    if (queue != nullptr) {
        al_destroy_event_queue(queue);
    }
}

Display::Display(Display&& src) noexcept : display(src.display), queue(src.queue) {
    src.display = nullptr;
    src.queue = nullptr;
}

void Display::captureRender(const char* name, const std::function<void()>& renderFunc) {
    if (renderFunc == nullptr) {
        throw std::runtime_error("renderFunc in Display::render cannot be nullptr");
    } else if (!alui::tests::captureDebugRenders) {
        return;
    }

    std::unique_ptr<ALLEGRO_BITMAP, decltype(&al_destroy_bitmap)> target(
        al_create_bitmap(width, height),
        al_destroy_bitmap
    );

    al_set_target_bitmap(target.get());

    al_clear_to_color(al_map_rgb(0, 0, 0));
    renderFunc();
    al_flip_display();

    // Reset the render buffer
    al_set_target_bitmap(nullptr);
    // Output to a file
    al_save_bitmap(name, target.get());

}

}
