#pragma once

#include <stdexcept>
#include "SizeUnit.hpp"

namespace alui {

struct Size {
    SizeUnit type;
    float value;

    Size() = delete;
    Size(float v) : type(SizeUnit::ABSOLUTE), value(v) {}
    Size(SizeUnit type, float value) : type(type), value(value) {}

    float compute(float parentSize) {
        switch(type) {
        case SizeUnit::RELATIVE:
            return parentSize * value;
        case SizeUnit::ABSOLUTE:
            return value;
        }
        [[unlikely]]
        // .c_str(): Fuck you windows
        throw std::runtime_error((std::string("Fatal: unknown size type: ") + std::to_string(static_cast<int>(type))).c_str());
    }
};

}
