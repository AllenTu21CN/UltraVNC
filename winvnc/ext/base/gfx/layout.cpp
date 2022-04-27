#include "base/gfx/layout.h"

namespace base {
namespace gfx {

void adjustSrcDstRect(const Size &scene_resolution, const RectF &dst_rect,
                      const Size &source_resolution, const RectF &src_rect,
                      VideoFillMode mode, RectF &out_dst_rect, RectF &out_src_rect)
{
    if (STRETCH == mode) {
        out_dst_rect = dst_rect;
        out_src_rect = src_rect;
        return;
    }

    float src_width = (float)source_resolution.width() * src_rect.width();
    float src_height = (float)source_resolution.height() * src_rect.height();
    float dst_width = (float)scene_resolution.width() * dst_rect.width();
    float dst_height = (float)scene_resolution.height() * dst_rect.height();
    float src_aspect_ratio = src_width / src_height;
    float dst_aspect_ratio = dst_width / dst_height;

    if (FIT == mode) {
        if (dst_aspect_ratio > src_aspect_ratio) {
            float dst_rect_width = dst_height * src_aspect_ratio / (float)scene_resolution.width();
            float dst_rect_left = dst_rect.left() + (dst_rect.width() - dst_rect_width) / 2.0f;
            out_dst_rect = RectF(dst_rect_left, dst_rect.top(), dst_rect_width, dst_rect.height());
        } else if (dst_aspect_ratio < src_aspect_ratio) {
            float dst_rect_height = dst_width / src_aspect_ratio / (float)scene_resolution.height();
            float dst_rect_top = dst_rect.top() + (dst_rect.height() - dst_rect_height) / 2.0f;
            out_dst_rect = RectF(dst_rect.left(), dst_rect_top, dst_rect.width(), dst_rect_height);
        } else {
            out_dst_rect = dst_rect;
        }
        out_src_rect = src_rect;
    } else if (CROP == mode) {
        if (dst_aspect_ratio > src_aspect_ratio) {
            float src_rect_height = src_width / dst_aspect_ratio / (float)source_resolution.height();
            float src_rect_top = src_rect.top() + (src_rect.height() - src_rect_height) / 2.0f;
            out_src_rect = RectF(src_rect.left(), src_rect_top, src_rect.width(), src_rect_height);
        } else if (dst_aspect_ratio < src_aspect_ratio) {
            float src_rect_width = src_height * dst_aspect_ratio / (float)source_resolution.width();
            float src_rect_left = src_rect.left() + (src_rect.width() - src_rect_width) / 2.0f;
            out_src_rect = RectF(src_rect_left, src_rect.top(), src_rect_width, src_rect.height());
        } else {
            out_src_rect = src_rect;
        }
        out_dst_rect = dst_rect;
    }
}

} // End of namespace gfx
} // End of namespace base
