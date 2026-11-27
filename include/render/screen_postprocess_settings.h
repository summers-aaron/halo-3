#ifndef RENDER_SCREEN_POSTPROCESS_SETTINGS_H
#define RENDER_SCREEN_POSTPROCESS_SETTINGS_H

#include <stddef.h>

// Kept for tu/graphicslibxbox/rasterizer_dx9_dynamic_geometry.cpp, the one TU still on the old
// spelling (held by another pod); identical layout to c_screen_postprocess::s_settings, which is
// the real type. Remove once that TU renames to x_editable_settings.
struct s_apply_trinary_settings
{
    union { bool resolve_hdr; bool enabled; };
    long final_surface_mode;
    long composite_mask;
    long final_flags;
    long primary;
    long bloom_mode;
    long bling;
    bool persist;
    long display;
    float final_scale;
    bool debug;
    bool distortion;
    float exposure;
    unsigned char exposure_adapt_instantly;
    long final_blend;
};

typedef char screen_postprocess_composite_mask_offset[(offsetof(s_apply_trinary_settings, composite_mask) == 8) ? 1 : -1];
typedef char screen_postprocess_bling_offset[(offsetof(s_apply_trinary_settings, bling) == 0x18) ? 1 : -1];
typedef char screen_postprocess_persist_offset[(offsetof(s_apply_trinary_settings, persist) == 0x1c) ? 1 : -1];
typedef char screen_postprocess_display_offset[(offsetof(s_apply_trinary_settings, display) == 0x20) ? 1 : -1];
typedef char screen_postprocess_distortion_offset[(offsetof(s_apply_trinary_settings, distortion) == 0x29) ? 1 : -1];
typedef char screen_postprocess_exposure_offset[(offsetof(s_apply_trinary_settings, exposure) == 0x2c) ? 1 : -1];
typedef char screen_postprocess_exposure_adapt_instantly_offset[(offsetof(s_apply_trinary_settings, exposure_adapt_instantly) == 0x30) ? 1 : -1];
typedef char screen_postprocess_settings_size[(sizeof(s_apply_trinary_settings) == 0x38) ? 1 : -1];

#endif
