#pragma once

#include "alui/Component.hpp"
#include "alui/Layout.hpp"
#include <vector>

namespace alui {


/**
 * \brief Primitive flexbox implementation used for the core layout, as well as several other layouts
 * \ingroup layouts
 *
 * ## Limitations
 *
 * For obvious reasons, this is not an attempt to reimplement CSS, though there are a lot of commonalities between CSS
 * and this particular layout due to how the flexbox spec is defined. However, due to the lack of CSS support (and a
 * related lack of support for various other specific CSS features), some part sof teh flexbox standard have not been
 * implemented because the related computations would not have any data to run on.
 *
 * The following paragraphs have been implemented from
 * [the flexbox standard](https://drafts.csswg.org/css-flexbox-1/#layout-algorithm):
 * * § 9.2
 * * § 9.3
 * * § 9.4 [TODO]
 * * § 9.6, bullet 16 [TODO]
 * * § 9.7
 *
 * The rest have either been omitted in their entirety (due to a lack of need or lack of relevant config options), or
 * been implemented in some other ✨ Special Way ✨. Several of the implemented paragraphs have somewhat non-standard
 * implementations, as the goal isn't a 100% compliant flexbox implementation.
 *
 */
class FlexBox : public Layout  {
protected:
    struct FlexAlgoData {
        std::shared_ptr<Component> c;

        float flexBaseSize = 0;
        float flexAxialSize = 0;
        float flexCrossSize = 0;

        bool frozen = false;
    };

    struct FlexLine {
        std::vector<FlexAlgoData> components;
        float runningMainSize = 0;
        float maxCrossSize = 0;
        float factorPool = 0;
    };

    float flexGap;
    FlexDirection dir;

    virtual float computeSizeRequirements(FlexDirection dir) override;
    virtual float computeCrossSize(FlexDirection dir, float virtualMainSize, float maxCrossSize) override;

public:
    FlexBox(FlexDirection layoutDirection, const ComponentConfig& cfg);

    virtual void recomputeBounds(
        Layout*,
        float parentMaxWidth, float parentMaxHeight
    ) override;
    virtual void setFlexGap(float flexGap) { this->flexGap = flexGap; }

    //virtual Flex computeSizeRequirements(FlexDirection dir) override;
};

}
