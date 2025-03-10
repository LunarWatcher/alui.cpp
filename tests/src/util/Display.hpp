#pragma once

#include "alui/GUI.hpp"
#include <allegro5/allegro5.h>
#include <functional>
#include <thread>

namespace alui {

class Display {
private:
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* queue;

    std::thread runner;
    int width, height;

public:
    bool running = true;

    Display(int width = 600, int height = 900);
    Display(GUI& g);
    ~Display();

    Display(Display&& src) noexcept;

    void captureRender(const char* name, const std::function<void()>& renderFunc);

    ALLEGRO_DISPLAY* operator*() { return display; }

};

}
