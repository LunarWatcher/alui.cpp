#pragma once

#include <utility>

namespace alui {

/**
 * Represents a position or a size. This magniture may not necessarily be an Absolute magniture, meaning a magniture
 * whose value is interpreted literally. Magnitudes may be Relative, and these have values that are computed relative to
 * their parent at layout time.
 */
struct Magnitude {
    /**
     * \enum alui::Scalar::Type
     * \brief determines how sizes are interpreted.
     */
    enum class Type {
        /**
         * Used for percentage-based layouts that can shrink or grow depending on how much space is free. Note that if no
         * space is free, the elements will be hidden - try to avoid this :)
         *
         * Width and height must be [0, 1]. If a dimension equals 0, it'll be determined by the containing layout. If the
         * percentage exceeds the size of the screen, behaviour, overflow, and rendering behaviour is undefined[^1].
         *
         * [^1]: At the time of writing c:
         */
        Relative,
        /**
         * Used for sizing in absolute pixels. Width and/or height must be >= 0, depending on the containing layout.
         * The width or height can be 0 for it to be automatically determined by the layout.
         */
        Absolute,
    };
    Type type;
    float value;

    Magnitude() = delete;
    Magnitude(float v) : type(Type::Absolute), value(v) {}
    Magnitude(Type type, float value) : type(type), value(value) {}

    float compute(float parentSize) {
        switch(type) {
        case Type::Relative:
            return parentSize * value;
        case Type::Absolute:
            return value;
        }
        // TODO: Why the fuck is this required?
        // GCC whines if this isn't here because ConTRoL FlOW REacHES eND OF FunCtion - no it fucking doesn't
        // Maybe if the type is serialized via an int and ends up being set to an invalid  type, but that's always UB,
        // isn't it? Why do I need to bump to C++23 and `std::unreachable()` this function? Just segfault if someone
        // does something that dumb, idfc
        std::unreachable();
    }

    /**
     * Utility method for creating a relative magnitude. Can be used in place of the constructors for slightly more
     * readable code
     */
    static Magnitude Relative(float value) {
        return Magnitude(
            Type::Relative,
            value
        );
    }

    /**
     * Utility method for creating an absolute magnitude. Can be used in place of the constructors for slightly more
     * readable code
     */
    static Magnitude Absolute(float value) {
        return Magnitude(
            Type::Absolute,
            value
        );
    }
};

}
