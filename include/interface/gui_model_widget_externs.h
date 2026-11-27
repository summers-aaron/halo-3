#ifndef INTERFACE_GUI_MODEL_WIDGET_EXTERNS_H
#define INTERFACE_GUI_MODEL_WIDGET_EXTERNS_H

#include "cseries/cseries.h"
#include "game/player_color_types.h"
#include "game/game_types.h"
#include "math/real_math_types.h"


extern "C" {
void network_get_player_colors(e_game_team team_index, bool unused, const c_enum<e_player_color_index, char, -1, 32> *color_indices, real_rgb_color *colors);
void render_texture_camera_set_position_and_direction(bool enable, const real_point3d *position, const real_vector3d *forward,
    const real_vector3d *up, float field_of_view);
}

#endif // INTERFACE_GUI_MODEL_WIDGET_EXTERNS_H
