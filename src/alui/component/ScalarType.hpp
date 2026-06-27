#pragma once

/** \file */

namespace alui {

/** @enum alui::ScalarType
 *
 * @brief determines how sizes are interpreted. 
 */
enum class ScalarType {
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

}
