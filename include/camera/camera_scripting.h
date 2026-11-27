#ifndef CAMERA_CAMERA_SCRIPTING_H
#define CAMERA_CAMERA_SCRIPTING_H

#include <stddef.h>
#include "math/real_math_types.h"

// Functions defined in tu/blamlibxbox/camera_scripting.cpp.

long scripted_camera_get_first_person_camera_object(void);
void __tls_set_g_camera_script_globals_allocator(void *value);
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_camera_script_globals_allocator> g_camera_script_globals_allocator;
void scripted_camera_set_animation_relative(long graph_index, long animation_name, long object_index,
    short flag_index, float playback_scale, bool looping, float start_fraction);
void scripted_camera_set(short point_index, short ticks, long object_index);
void scripted_camera_set_pan(short point_index, short ticks);
void scripted_camera_pan(short set_index, short point_index, short duration_ticks,
    short acceleration_ticks, float initial_scale, short deceleration_ticks, float final_scale);
void cinematic_camera_set_easing_in(float seconds, float scale, float field_of_view, float i, float j, float k);
void cinematic_camera_set_easing_out(float seconds);
void scripted_camera_set_animation_relative(long bsp_index, long camera_index, long object_index, short name_index);
void scripted_camera_set_field_of_view(float field_of_view, short ticks);
void scripted_camera_game_tick(void);
extern "C" void scripted_camera_set_cinematic_lighting(long item_index);
extern "C" void scripted_camera_start_object_animation(long cinematic_index, long scene_index, long scene_object_index, long object_index, long animation_name);
extern "C" void scripted_camera_show_object(long cinematic_index, long scene_index,
    long scene_object_index, short name_index, long object_index, long animation_name, bool flag);
extern "C" void scripted_camera_hide_object(long cinematic_index, long scene_index, long scene_object_index, long object_index);
extern "C" void scripted_camera_start_shot_sound(long cinematic_index, long scene_index, long shot_index, long sound_index);

struct real_matrix4x3;
struct s_camera_frame
{
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    int : 32;
    int : 32;
    float field_of_view;
    long depth_of_field;
    float depth_values[4];
};
typedef char camera_frame_size[(sizeof(s_camera_frame) == 0x44) ? 1 : -1];
typedef char camera_frame_field_of_view_offset[(offsetof(s_camera_frame, field_of_view) == 0x2c) ? 1 : -1];
struct s_cinematic_scene;

void transform_into_flag_space(struct real_matrix4x3 * matrix, long cutscene_flag_index);
extern "C" s_cinematic_scene *scripted_camera_get_scene(long cinematic_index, long scene_index);
extern "C" bool scripted_camera_is_object_valid_for_players(long cinematic_index, long scene_index, long object_index);
bool scripted_camera_get_deterministic_animated_camera_matrix(real_matrix4x3 *matrix, float *time);
void scripted_camera_get_deterministic_cinematic_camera_matrix(const s_camera_frame *frame, real_matrix4x3 *matrix);

enum e_scenario_camera_type;
union real_point3d;
union real_euler_angles3d;
struct s_camera_control;
struct s_observer_command;

void scripted_camera_set_internal(e_scenario_camera_type type, const real_point3d *position, const real_euler_angles3d *orientation,
    float duration, short point_index, long object_index);
void scripted_camera_update(void *camera, const s_camera_control *control, s_observer_command *result);

#endif // CAMERA_CAMERA_SCRIPTING_H
