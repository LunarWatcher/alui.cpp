#include "NineSliceImageFactory.hpp"

namespace alui {

NineSliceImageFactory::NineSliceImageFactory(ALLEGRO_BITMAP* bmp, Rect sourceSliceSize, Rect center)
    : ImageFactory(bmp, sourceSliceSize), localCenter(center), center(center + sourceSliceSize)
{}

std::shared_ptr<ImageSlice> NineSliceImageFactory::resize(float width, float height)
{
    auto dest = al_create_bitmap(width, height);

    if (dest == nullptr) {
        [[unlikely]]
        throw std::runtime_error("Bitmap init failed");
    }

    int x, y, w, h;
    al_get_clipping_rectangle(&x, &y, &w, &h);
    al_set_clipping_rectangle(0, 0, width, height);

    auto centerWidth = width - (sourceSliceSize.w - localCenter.w);
    auto centerHeight = height - (sourceSliceSize.h - localCenter.h);
    auto bottomHeight = sourceSliceSize.h - localCenter.endY();
    auto leftWidth = localCenter.x;
    auto topHeight = localCenter.y;
    auto rightWidth = sourceSliceSize.h - localCenter.endX();

    auto old = al_get_target_bitmap();

    al_set_target_bitmap(dest);
    // Top-left corner
    al_draw_scaled_bitmap(
        this->source, sourceSliceSize.x, sourceSliceSize.y, leftWidth, topHeight, 0, 0, leftWidth, topHeight, 0
    );
    // Top edge
    al_draw_scaled_bitmap(
        this->source, center.x, sourceSliceSize.y, localCenter.w, topHeight, localCenter.x, 0, centerWidth, topHeight, 0
    );
    // Top-right corner
    al_draw_scaled_bitmap(
        this->source,
        center.endX(),
        sourceSliceSize.y,
        rightWidth,
        topHeight,
        localCenter.x + centerWidth,
        0,
        rightWidth,
        topHeight,
        0
    );

    float bottomStartY = localCenter.y + centerHeight;
    // Bottom-left corner
    al_draw_scaled_bitmap(
        this->source,
        sourceSliceSize.x,
        sourceSliceSize.y + localCenter.endY(),
        leftWidth,
        bottomHeight,
        0,
        bottomStartY,
        leftWidth,
        bottomHeight,
        0
    );
    // Bottom edge
    al_draw_scaled_bitmap(
        this->source,
        center.x,
        sourceSliceSize.y + localCenter.endY(),
        localCenter.w,
        sourceSliceSize.h - localCenter.endY(),
        localCenter.x,
        bottomStartY,
        centerWidth,
        bottomHeight,
        0
    );
    // Bottom-right corner
    al_draw_scaled_bitmap(
        this->source,
        center.endX(),
        center.endY(),
        rightWidth,
        bottomHeight,
        localCenter.x + centerWidth,
        localCenter.y + centerHeight,
        rightWidth,
        bottomHeight,
        0
    );
    // Left edge
    al_draw_scaled_bitmap(
        this->source,
        sourceSliceSize.x,
        sourceSliceSize.y + topHeight,
        leftWidth,
        localCenter.h,
        0,
        topHeight,
        leftWidth,
        centerHeight,
        0
    );
    // Right edge
    al_draw_scaled_bitmap(
        this->source,
        center.endX(),
        sourceSliceSize.y + topHeight,
        rightWidth,
        localCenter.h,
        localCenter.x + centerWidth,
        topHeight,
        rightWidth,
        centerHeight,
        0
    );
    // Center
    al_draw_scaled_bitmap(
        this->source, center.x, center.y, center.w, center.h, localCenter.x, localCenter.y, centerWidth, centerHeight, 0
    );

    al_set_target_bitmap(old);
    al_set_clipping_rectangle(x, y, w, h);

    static bool rendered = false;

    if (!rendered) {
        rendered = true;
        al_save_bitmap("debug.bmp", dest);
    }

    return std::make_shared<ImageSlice>(dest, Rect(0, 0, width, height), true);
}
}
