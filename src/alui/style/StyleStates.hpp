#pragma once

#include <alui/Component.hpp>

#include <optional>
namespace alui {

struct AbstractStyle {
    virtual ~AbstractStyle() = default;

    virtual void render(Component*) = 0;
};

struct StyleStates {
    std::optional<std::shared_ptr<AbstractStyle>> defaultState = std::nullopt;
    std::optional<std::shared_ptr<AbstractStyle>> disabledState = std::nullopt;
    std::optional<std::shared_ptr<AbstractStyle>> focusedState = std::nullopt;
    std::optional<std::shared_ptr<AbstractStyle>> activeState = std::nullopt;

    void render(Component* component) {
        // Pointer to avoid copies
        auto* state = &defaultState;
        if (component->isFocused() && focusedState.has_value()) {
            state = &focusedState;
        } // The rest of the states are not implemented in the Component yet

        // There are null states, so if !has_value(), there's nothing to render. 
        // The optional will always be inited to something though, so *state is safe
        if ((*state).has_value()) {
            (**state)->render(component);
        }
    }
};

}
