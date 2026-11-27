#ifndef INTERFACE_VIEW_CONFIG_TYPES_H
#define INTERFACE_VIEW_CONFIG_TYPES_H

#include "math/real_math_types.h"

struct view_config { float x0, y0, x1, y1; long extra; };

// Index 0 selects standard aspect, 1 widescreen; then player count - 1; then window.
extern "C" const view_config k_interface_splitscreen_view_configs[2][4][4];

#endif // INTERFACE_VIEW_CONFIG_TYPES_H
