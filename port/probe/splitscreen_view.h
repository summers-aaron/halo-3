#pragma once

#include "interface/view_config_types.h"

const view_config &splitscreen_view_config(bool widescreen, long count, long screen);
void rasterizer_get_view_title_safe_pixel_bounds(rectangle2d *bounds, bool full_width_split);
