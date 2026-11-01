#include "Scrollbar.hpp"

namespace alui {

void Scrollbar::render(GUI &ctx, float scrollX, float scrollY) {
    if (this->computedWidth == 0 || this->computedHeight == 0) {
        // No scrollbar visible
        return;
    }
    Component::render(ctx, scrollX, scrollY);
}

}
