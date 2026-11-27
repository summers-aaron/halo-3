#ifndef CAMERA_CAMERA_SCRIPT_GLOBALS_TYPES_H
#define CAMERA_CAMERA_SCRIPT_GLOBALS_TYPES_H

#include "math/real_math_types.h"
#include "cseries/cseries.h"
#include "camera/camera_pan_navigator.h"

struct s_camera_scripting_pan {
  real_point3d position;
  real_quaternion orientation;
  long time;
  c_camera_pan_navigator navigator;
};

enum e_scenario_camera_type {
  _scenario_camera_type_0,
  _scenario_camera_type_1,
  _scenario_camera_type_2,
  _scenario_camera_type_3,
};
struct s_scripted_camera_target {
  real_point3d position;
  real_vector3d side;
};
struct s_scripted_camera_state {
  short point_index;
  c_enum_no_init<e_scenario_camera_type, short, 0, 4> camera_type;
  union {
    struct {
      real_point3d position;
      real_vector3d side;
    };
    s_scripted_camera_target target;
  };
};
enum e_easing_type { _easing_type_0, _easing_type_1, _easing_type_2 };

enum e_camera_script_mode {
  _camera_script_mode_none,
  _camera_script_mode_point,
};
struct s_camera_script_globals {
  bool active;
  bool animation_changed;
  union {
    unsigned short mode;
    c_enum_no_init<e_camera_script_mode, short, 0, 6> scripted_mode;
  };
  float field_of_view_time;
  float field_of_view_duration;
  float field_of_view_start;
  float field_of_view_target;
  long animation_duration;
  union {
    real_point3d position;
    real_vector3d position_offset;
  };
  real_vector3d forward;
  real_vector3d up;
  long object_index;
  struct s_animation_state {
    long start_tick;
    long graph_index;
    long animation_name;
    short flag_index;
    float playback_scale;
    bool looping;
  };
  struct s_camera_track {
    union { long tag; long cinematic_tag_index; long animation_start_tick; };
    union { long shot_index; long cinematic_scene_index; };
    union { long frame; long cinematic_shot_index; };
    union { long previous_frame; long cinematic_frame; };
    union { long cutscene_flag_index; long cinematic_flag_space; };
    union { bool texture; bool render_main_camera_to_texture; };
    union { long target_name; long active_camera; };
    union { bool previous_texture; bool cinematic_transition; };
    union { long total_ticks; long cinematic_time; };
    // March: scripted_camera_set_cinematic stamps game_time_get() here on entry (82a1bad4),
    // and scripted_camera_game_tick passes (game_time_get() - this) to cinematic_set_frame_status (82a1b3c4).
    union { long start_tick; long cinematic_start_tick; };
    c_enum_no_init<e_easing_type, short, 0, 3> easing;
    float duration;
    real_quaternion orientation;
    union {
      real_quaternion previous_orientation;
      struct { real_vector3d easing_velocity; float field_of_view; };
    };
    real_point3d position;
    real_point3d previous_position;
    float scale;
    union { real_vector3d velocity; real_point3d cinematic_offset; };
  };
  union {
    s_animation_state animation;
    s_camera_scripting_pan pan;
    s_camera_track camera_track;
    long cinematic_flag_index;
    s_scripted_camera_state scripted;
  };
};

typedef char camera_script_globals_pan_offset[(offsetof(s_camera_script_globals, pan) == 0x40) ? 1 : -1];
typedef char camera_scripting_pan_size[(sizeof(s_camera_scripting_pan) == 0x38) ? 1 : -1];

typedef char camera_script_globals_size[(sizeof(s_camera_script_globals) == 0xb8) ? 1 : -1];
typedef char camera_script_globals_object_offset[(offsetof(s_camera_script_globals, object_index) == 0x3c) ? 1 : -1];
typedef char camera_script_globals_texture_offset[(offsetof(s_camera_script_globals, camera_track.render_main_camera_to_texture) == 0x54) ? 1 : -1];
typedef char camera_script_globals_time_offset[(offsetof(s_camera_script_globals, camera_track.cinematic_time) == 0x60) ? 1 : -1];
typedef char camera_script_globals_cinematic_offset[(offsetof(s_camera_script_globals, camera_track.cinematic_offset) == 0xac) ? 1 : -1];

typedef char camera_animation_playback_scale_offset[(offsetof(s_camera_script_globals::s_animation_state, playback_scale) == 0x10) ? 1 : -1];
typedef char camera_track_flag_offset[(offsetof(s_camera_script_globals::s_camera_track, cutscene_flag_index) == 0x10) ? 1 : -1];
typedef char camera_track_target_offset[(offsetof(s_camera_script_globals::s_camera_track, target_name) == 0x18) ? 1 : -1];
typedef char camera_track_total_ticks_offset[(offsetof(s_camera_script_globals::s_camera_track, total_ticks) == 0x20) ? 1 : -1];
typedef char camera_track_easing_offset[(offsetof(s_camera_script_globals::s_camera_track, easing) == 0x28) ? 1 : -1];
typedef char camera_track_duration_offset[(offsetof(s_camera_script_globals::s_camera_track, duration) == 0x2c) ? 1 : -1];
typedef char camera_track_size[(sizeof(s_camera_script_globals::s_camera_track) == 0x78) ? 1 : -1];

typedef char camera_track_easing_velocity_offset[(offsetof(s_camera_script_globals::s_camera_track, easing_velocity) == 0x40) ? 1 : -1];
typedef char camera_track_field_of_view_offset[(offsetof(s_camera_script_globals::s_camera_track, field_of_view) == 0x4c) ? 1 : -1];

extern "C" {
extern __declspec(thread) s_camera_script_globals *g_camera_script_globals;
}

#endif // CAMERA_CAMERA_SCRIPT_GLOBALS_TYPES_H
