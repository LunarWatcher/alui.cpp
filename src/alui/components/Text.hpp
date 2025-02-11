#pragma once

#include "allegro5/color.h"
#include "alui/Component.hpp"
#include <string>

namespace alui {

class Text : public Component {
protected:
    std::string content;

    ALLEGRO_COLOR textColour;

    struct AlMultilineData {

        int lineCount;
        float maxWidth;

        ALLEGRO_FONT* font;
    };

    std::vector<std::string> computedLines;
 
public:
    using LineCallback = std::function<void(const std::string& line)>;
    Text(const std::string& str, const ComponentConfig& cfg);

    virtual void render(GUI& ctx) override;
    virtual void tick() override {}

    virtual float computeCrossSize(FlexDirection dir, float virtualMainSize) override;
    virtual void processText(
        const LineCallback& func,
        float maxWidth,
        bool bustCache = false
    );

    virtual void setTextColour(ALLEGRO_COLOR colour) {
        this->textColour = colour;
    }

    virtual const std::string& getContent() { return content; }

    virtual void setContent(const std::string& text) {
        content = text;
        dirty = true;
    }


};

}
