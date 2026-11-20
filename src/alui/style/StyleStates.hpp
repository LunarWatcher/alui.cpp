#pragma once

#include <alui/component/Component.hpp>
#include <memory>

namespace alui {

struct AbstractStyle {
    virtual ~AbstractStyle() = default;

    virtual void render(Component*, float scrollX, float scrollY) = 0;
    virtual void resize(
        float /* width */,
        float /* height */
    ) {}
};

struct StyleStates {
    std::shared_ptr<AbstractStyle> defaultState = nullptr;
    std::shared_ptr<AbstractStyle> disabledState = nullptr;
    std::shared_ptr<AbstractStyle> focusedState = nullptr;
    std::shared_ptr<AbstractStyle> activeState = nullptr;

    void render(Component* component, float scrollX, float scrollY) {
        // Pointer to avoid copies
        AbstractStyle* state = defaultState.get();
        if (component->isFocused() && focusedState) {
            state = focusedState.get();
        } // The rest of the states are not implemented in the Component yet

        // There are null states, so if !has_value(), there's nothing to render.
        // The optional will always be inited to something though, so *state is safe
        if (state != nullptr) {
            state->render(component, scrollX, scrollY);
        }
    }

    void resize(
        float width,
        float height
    ) {
        if (defaultState) { defaultState->resize(width, height); }
        if (disabledState) { disabledState->resize(width, height); }
        if (focusedState) { focusedState->resize(width, height); }
        if (activeState) { activeState->resize(width, height); }
    }
};

}
