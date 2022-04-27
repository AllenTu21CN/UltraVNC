#pragma once

namespace base {
namespace gfx {

enum Alignment {
    // Horizontal
    ALIGN_LEFT       = 0x0001,   // Aligns with the left edge.
    ALIGN_RIGHT      = 0x0002,   // Aligns with the right edge.
    ALIGN_HCENTER    = 0x0004,   // Centers horizontally in the available space.
    ALIGN_JUSTIFY    = 0x0008,   // Justifies the text in the available space.

    // Vertical
    ALIGN_TOP        = 0x0020,   // Aligns with the top.
    ALIGN_BOTTOM     = 0x0040,   // Aligns with the bottom.
    ALIGN_VCENTER    = 0x0080,   // Centers vertically in the available space.
    ALIGN_BASELINE   = 0x0100,   // Aligns with the baseline.
};

} // End of namespace gfx
} // End of namespace base
