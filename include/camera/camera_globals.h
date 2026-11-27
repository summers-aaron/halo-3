#ifndef CAMERA_CAMERA_GLOBALS_H
#define CAMERA_CAMERA_GLOBALS_H

#include <stddef.h>

// The camera settings of the globals tag (March: g_camera_globals, camera_globals.obj).
// camera_globals_initialize_from_tags (director.cpp) copies the tag's camera block over the defaults; the
// cameras read it every update. Names follow what each reader does with the value.
struct s_camera_globals
{
    float field_of_view_scale;                  // flying camera
    float yaw_scale;                            // flying camera look input
    float pitch_scale;
    float forward_scale;                        // flying and orbiting camera movement input
    float side_scale;
    float up_scale;
    float dead_camera_transition_time;          // dead camera: transition timer = this - time dead
    float falling_death_transition_time;
    float dead_camera_initial_focus_distance;   // dead camera focus distance on its first update
    float dead_camera_focus_distance;           // dead camera focus distance; grows to five times this
    float dead_camera_height;                   // dead camera height above the body's root node
    float dead_camera_maximum_pitch_down;       // the killer-tracking pitch stays above -this
    float dead_camera_minimum_transition_time;
    float director_idle_time;                   // observer director: seconds below director_idle_speed
    float director_idle_speed;
    float boost_scale;                          // flying and orbiting camera speed boost
    float boost_time;                           // flying camera: seconds to reach boost_scale
    short boost_function;                       // flying camera: transition function over boost_time
    float flying_camera_field_of_view;          // degrees, while locked in the xy plane
    float flying_camera_look_scale;             // look scale while locked in the xy plane
    float collision_radius;                     // flying camera collision and soft ceiling sphere
    float flying_camera_transition_time;
    float flying_camera_parameter_time;         // timer of observer command parameter 4
    float vertical_boost_time;
    short vertical_boost_function;
    float orbiting_camera_minimum_distance;
    float orbiting_camera_maximum_distance;
    float orbiting_camera_transition_time;
    float orbiting_camera_height;
    float orbiting_camera_maximum_pitch;        // the pitch stays between -minimum and -maximum
    float orbiting_camera_minimum_pitch;
    float saved_film_maximum_playback_speed;
    float saved_film_secondary_playback_speed;  // unused
    float saved_film_revert_transition_time;    // directors' transition timer = -this after a revert
    float following_camera_transition_time;     // observer director: switching to following or first person
    float orbiting_camera_switch_transition_time;
};

typedef char camera_globals_size[(sizeof(s_camera_globals) == 0x90) ? 1 : -1];
typedef char camera_globals_boost_function_offset[(offsetof(s_camera_globals, boost_function) == 0x44) ? 1 : -1];
typedef char camera_globals_flying_fov_offset[(offsetof(s_camera_globals, flying_camera_field_of_view) == 0x48) ? 1 : -1];
typedef char camera_globals_vertical_boost_function_offset[(offsetof(s_camera_globals, vertical_boost_function) == 0x60) ? 1 : -1];
typedef char camera_globals_orbiting_minimum_offset[(offsetof(s_camera_globals, orbiting_camera_minimum_distance) == 0x64) ? 1 : -1];
typedef char camera_globals_saved_film_speed_offset[(offsetof(s_camera_globals, saved_film_maximum_playback_speed) == 0x7c) ? 1 : -1];

// March: g_camera_globals (camera_globals.obj).
extern s_camera_globals g_camera_globals;


#endif // CAMERA_CAMERA_GLOBALS_H
