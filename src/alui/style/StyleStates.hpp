#pragma once

#include <memory>

namespace alui {

class Component;

struct AbstractStyle {
    virtual ~AbstractStyle() = default;

    virtual void render(Component*, float scrollX, float scrollY) = 0;
};

struct StyleStates {
    std::shared_ptr<AbstractStyle> defaultState = nullptr;
    std::shared_ptr<AbstractStyle> disabledState = nullptr;
    std::shared_ptr<AbstractStyle> focusedState = nullptr;
    std::shared_ptr<AbstractStyle> activeState = nullptr;

    void render(Component* component, float scrollX, float scrollY);
};

}
