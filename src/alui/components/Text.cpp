#include "Text.hpp"
#include "allegro5/allegro_font.h"
#include "allegro5/display.h"
#include "allegro5/utf8.h"
#include "alui/Component.hpp"
#include "alui/GUI.hpp"
#include <limits>
#include <cmath>
#include <stdexcept>
#include <string>
#if defined ALUI_TEST || 1
#include <iostream>
#endif

namespace alui {

Text::Text(const std::string& str, const ComponentConfig& cfg) : Component(cfg), content(str) {

}

void Text::render(GUI& ctx) {
    Component::render(ctx);
    //al_set_clipping_rectangle(
        //std::floor(getContentX()),
        //std::floor(getContentY()),
        //std::floor(getInternalWidth()),
        //std::floor(getInternalHeight())
    //);

    int lineNum = 0;
    processText([&](const std::string& line) {
        al_draw_text(
            font,
            textColour,
            getContentX(),
            getContentY() + (float) (al_get_font_line_height(font) * lineNum++),
            0, line.c_str()
        );
    }, getInternalWidth());


    //al_reset_clipping_rectangle();
}

float Text::computeCrossSize(FlexDirection dir, float virtualMainSize) {
    // TODO: figure out a better solution for when FlexDirection::VERTICAL
    auto maxWidth = dir == FlexDirection::HORIZONTAL 
        ? virtualMainSize - this->f.padding.getSizeForDimension(dir) 
        : unwrap(f.maxWidth, std::numeric_limits<float>::max());
    processText([&](const auto&) {}, maxWidth, true);
    //std::cout << "used maxWidth = " << maxWidth << ", vms: " << virtualMainSize << std::endl;

    int lineCount = (int) computedLines.size();

    //std::cout << lineCount << std::endl;

    if (dir == FlexDirection::HORIZONTAL) {
        // Horizontal rows: cross size is height

        // TODO: cache somewhere?
        auto lineHeight = al_get_font_line_height(font);

        return (float) (lineCount * lineHeight) + f.padding.getCrossSizeForDimension(dir);
    } else {
        // Vertical "rows": Cross size is display width
        auto* display = al_get_current_display();
        if (display == nullptr) {
            // ... except in test cases, where cross size is display width or 600px
            // TODO: This probably wouldn't be hard-coded. Might be easier to cache the width somewhere (GUI?)
#ifdef ALUI_TEST
            std::cout << "Warning: running in test mode, display is null, returning 600px" << std::endl;
            return 600;
#else
            throw std::runtime_error("Looks like you've misconfigured your setup.");
#endif
        }
        return (float) al_get_display_width(display);
        //return unwrap(f.minWidth, 0) + f.padding.getCrossSizeForDimension(dir);
    }
}

void Text::processText(const LineCallback& func, float maxWidth, bool bustCache) {
    if (bustCache || computedLines.empty()) {
        computedLines.clear();

        int currRangeStart = 0;
        float accumulatedSize = 0;

        std::unique_ptr<ALLEGRO_USTR, decltype(&al_ustr_free)> codepoints(
            al_ustr_new(content.c_str()), 
            &al_ustr_free
        );
        assert(codepoints != nullptr);
        //
        // The current codepoint
        int32_t codepoint;
        // The previous codepoint, cached for utility purposes
        int32_t lastCodepoint = 0;

        // Functionally a pointer to the next codepoint, indicates where al_ustr_get_next should start its search
        int nextCodepointPtr = 0;
        // The start of the current byte. nextCodepoint - currBytePtr = codepoint size
        int currBytePtr = 0;
        // The start of the previous byte. currBytePtr - prevBytePtr = lastCodepoint size
        // (Currently not in use)
        //int prevBytePtr = 0;

        while ((codepoint = al_ustr_get_next(codepoints.get(), &nextCodepointPtr)) != -1) {
            if (codepoint == -2) {
                // TODO: handle spacing for invalid codepoint boxes
                // (assuming that's how allegro handles rendering them)
                continue;
            } else if (codepoint < 0) {
                [[unlikely]]
                throw std::runtime_error("Unknown ustr_get_next return code");
            }
            auto glyphSize = (float) al_get_glyph_advance(font, codepoint, ALLEGRO_NO_KERNING);
            accumulatedSize += glyphSize;

            if (codepoint == (int) '\n') {
                if (accumulatedSize == 0) {
                    computedLines.push_back("");
                    currRangeStart = nextCodepointPtr + 1;
                } else {
                    computedLines.push_back(
                        content.substr(currRangeStart, currBytePtr - currRangeStart)
                    );
                    // Consume the current byte
                    accumulatedSize = 0;
                    currRangeStart = nextCodepointPtr;
                }
            } else if ((maxWidth > 0 && accumulatedSize > maxWidth)) {
                // TODO: adding a `-` can result in an overflow. This probably needs to be done in a few passes

                computedLines.push_back(content.substr(
                    currRangeStart, currBytePtr - currRangeStart
                ) + (lastCodepoint != (int) ' ' && codepoint != (int) ' ' ? "-" : ""));

                // Glob the current byte into the next line
                accumulatedSize = glyphSize;
                currRangeStart = currBytePtr;
            }

            lastCodepoint = codepoint;
            //prevBytePtr = currBytePtr;
            currBytePtr = nextCodepointPtr;
        }

        // TODO: Check if this checks out
        if ((size_t) currRangeStart < content.size()) {
            computedLines.push_back(content.substr(currRangeStart, content.size() - currRangeStart));
        }
    }

    for (auto& line : computedLines) {
        func(line);
    }

    
}

}
