#ifndef MAIN_MAIN_RENDER_EXTERNS_H
#define MAIN_MAIN_RENDER_EXTERNS_H

struct s_data_array;
struct levels_map_datum;
#include "rasterizer/dx9/rasterizer_screenshot_types.h"
class c_camera_fx_values;
class c_player_view;

#include "cache/predicted_resources.h"
extern "C" { bool levels_try_and_get_datum(s_data_array *, int, levels_map_datum *); }
extern "C" { void chud_loading_draw_screen(void *loading_screen, float field_of_view, float near_plane,
	float far_plane, float progress, void *position, void *direction); }
extern "C" { unsigned char rasterizer_render_screenshot(rasterizer_dx9_dynamic_geometry_screenshot_parameters *, c_player_view *, c_camera_fx_values *, long); }

#endif // MAIN_MAIN_RENDER_EXTERNS_H
