#ifndef RASTERIZER_DX9_RASTERIZER_SCREENSHOT_TYPES_H
#define RASTERIZER_DX9_RASTERIZER_SCREENSHOT_TYPES_H
#include <stddef.h>
class c_view;
class c_camera_fx_values;

struct s_rasterizer_screenshot_context
{
    bool active;
    c_view *view;
};
typedef char rasterizer_screenshot_context_view_offset[(sizeof(void *) != 4 || ((offsetof(s_rasterizer_screenshot_context, view) == 4))) ? 1 : -1];

struct rasterizer_dx9_dynamic_geometry_screenshot_parameters : s_rasterizer_screenshot_context
{
    c_camera_fx_values *camera_fx;
    char screen_effects[56];
    long dimensions[6];
    bool depth_of_field;
    long flags;
};
typedef char rasterizer_layout_assert_1[
    (sizeof(void *) != 4 || ((offsetof(rasterizer_dx9_dynamic_geometry_screenshot_parameters, camera_fx) == 0x8))) ? 1 : -1];
typedef char rasterizer_layout_assert_2[
    (sizeof(void *) != 4 || ((offsetof(rasterizer_dx9_dynamic_geometry_screenshot_parameters, dimensions) == 0x44))) ? 1 : -1];
typedef char rasterizer_layout_assert_3[(sizeof(void *) != 4 || ((sizeof(rasterizer_dx9_dynamic_geometry_screenshot_parameters) == 0x64))) ? 1 : -1];

#endif
