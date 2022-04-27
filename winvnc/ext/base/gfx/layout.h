#pragma once

#include "base/gfx/size.h"
#include "base/gfx/rect.h"

namespace base {
namespace gfx {

enum VideoFillMode {
    STRETCH,
    FIT,
    CROP
};

void adjustSrcDstRect(const Size &scene_resolution, const RectF &dst_rect,
                      const Size &source_resolution, const RectF &src_rect,
                      VideoFillMode mode, RectF &out_dst_rect, RectF &out_src_rect);

} // End of namespace gfx
} // End of namespace base
