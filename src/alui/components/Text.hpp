#pragma once

#include "allegro5/color.h"
#include "allegro5/allegro_font.h"
#include "alui/Component.hpp"
#include <string>
namespace alui {

class Text : public Component {
protected:
    std::string content;
    /**
     * Internal variable; contains the same string as content, but modified for text wrapping. This should not be
     * modified outside the reflowing process.
     */
    std::string reflowedContent;
    ALLEGRO_COLOR textColour;
public:
    Text(const std::string& str);

    virtual void render(GUI& ctx) override;
    virtual void tick() override {}

    virtual float computeCrossSize(FlexDirection dir, float virtualMainSize) override;

    virtual void setColour(ALLEGRO_COLOR colour) {
        this->textColour = colour;
    }

    virtual void setContent(const std::string& text) {
        content = text;
        dirty = true;
    }

};

}
