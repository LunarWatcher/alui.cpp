#pragma once

#include "allegro5/color.h"
#include "allegro5/allegro_font.h"
#include "alui/Component.hpp"
#include <string>
namespace alui {

class Text : public Component {
protected:
    std::string content;

    ALLEGRO_COLOR textColour;
public:
    Text(const std::string& str, const ComponentConfig& cfg);

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
