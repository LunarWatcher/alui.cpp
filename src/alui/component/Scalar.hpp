#pragma once

#include <stdexcept>
#include "ScalarType.hpp"

namespace alui {

/**
 * Represents a position or a size. This scalar may not necessarily be an Absolute scalar, meaning a scalar whose value
 * is interpreted literally. Scalars may be Relative, and these have values that are computed relative to their parent
 * at layout time.
 *
 * The name "Scalar" may technically be abusing math terminology, but I couldn't think of anything better that wasn't
 * just "Dimension", and that's just shit. Scalar at least makes some quantity of sense, since this is both a scalar
 * value used for sizes, as well as scalar components used in vectors (XY positions).
 */
struct Scalar {
    ScalarType type;
    float value;

    Scalar() = delete;
    Scalar(float v) : type(ScalarType::Absolute), value(v) {}
    Scalar(ScalarType type, float value) : type(type), value(value) {}

    float compute(float parentScalar) {
        switch(type) {
        case ScalarType::Relative:
            return parentScalar * value;
        case ScalarType::Absolute:
            return value;
        }
    }
};

}
