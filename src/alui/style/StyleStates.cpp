#include "StyleStates.hpp"

#include <alui/component/Component.hpp>

namespace alui {

void StyleStates::render(Component* component, float scrollX, float scrollY)
{
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

}
