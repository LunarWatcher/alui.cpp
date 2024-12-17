#pragma once

#include "alui/components/Text.hpp"
namespace alui {

class Button : public Text {
public:
    virtual void render(GUI& ctx) override;
};

}
