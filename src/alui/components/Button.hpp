#pragma once

#include "alui/components/Text.hpp"
namespace alui {

class Button : public Text {
public:
    Button(const std::string& str, const ComponentConfig& cfg) 
        : Text(str, cfg) {}
};

}
