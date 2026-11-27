#ifndef RENDER_RENDER_PLAYER_VIEW_CAMERA_SETTINGS_H
#define RENDER_RENDER_PLAYER_VIEW_CAMERA_SETTINGS_H

#include "render/camera_fx_settings_types.h"
#include "render/camera_fx_values.h"

struct render_player_view_camera_settings
{
    c_camera_fx_settings settings;
    c_camera_fx_values values;
};

typedef char player_camera_values_offset[(offsetof(render_player_view_camera_settings, values) == 0xe4) ? 1 : -1];
typedef char player_camera_settings_size[(sizeof(render_player_view_camera_settings) == 0x244) ? 1 : -1];

#endif
