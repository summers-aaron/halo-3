#ifndef INTERFACE_FIRST_PERSON_WEAPON_RUNTIME_TYPES_H
#define INTERFACE_FIRST_PERSON_WEAPON_RUNTIME_TYPES_H

#include "cseries/cseries.h"
#include "animations/animation_manager.h"
#include "animations/animation_id.h"
#include "animations/animation_search_path_iterator.h"
#include "math/matrix_math_types.h"
#include <stddef.h>

typedef c_animation_id fpw_animation_id;
class c_model_animation;
class c_model_animation_graph;
template <long k_maximum_count> class c_static_flags;
class ik_point_iterator;
#include "animations/animation_search_path_iterator.h"
struct fpw_manager {
 __forceinline bool paused() const { return playback_flags.test(0); }
 __forceinline void pause_playback(bool paused) { playback_flags.set(0, paused); }
 c_animation_channel channels[2];
 float elapsed, duration;
 short type, control_flags;
 long board, model, graph;
 union
 {
     unsigned short flags;
     c_flags_no_init<long, unsigned short, 16> playback_flags;
 };
 short counter;
 s_animation_goal_settings current, previous;
 // Same bytes as c_animation_manager::m_transition_translation: first_person_weapons.cpp
 // calls c_animation_manager methods on this struct, and they read these 12 bytes as that field.
 real_vector3d transition_translation;
 __forceinline void reset_state_channels(){channels[0].reset(); channels[1].reset(); flags&=~0x40;}
 __forceinline void initialize(long model_index,long board_index){
  model=model_index;
  (&channels[0])->initialize(); (&channels[1])->initialize();
  c_interpolator_control *control=(c_interpolator_control *)&elapsed; flags=0;counter=0;graph=-1;control->disable();
  current.mode=-1;current.state=-1;current.weapon_class=-1;current.weapon_type=-1;
  board=board_index;previous=current;
 }
};
struct fpw_weapon {
 __forceinline const long *get_object_indices() const { return object_indices; }
 __forceinline long get_weapon_class() const { const long *indices = get_object_indices(); BLAM_ASSUME(indices != NULL); return indices[3]; }
 unsigned long flags; long object_indices[10];fpw_manager animations;c_animation_channel extra_channels[2];fpw_animation_id ids[3];
 float fire_overlay_weight;
 float firing_time;
 short idle_timer;
 short idle_delay;
 short sound_timer;
 short state_timer;
 long weapon_node_remap_count, hands_node_remap_count;
 long weapon_node_remapping[64], hands_node_remapping[64];
 long hands_node_secondary_remapping[64];
 short weapon_root_node_index, opposite_hand_node_index;
 long node_count;
 long node_matrix_count;
 real_matrix4x3 node_matrices[64];
 long hands_node_count;
 real_matrix4x3 camera_adjustment;
 long weapon_index, unknown_index, sound_index;
 bool pending_suppress;
 char end[3];
};
struct fpw_transition {unsigned short current,previous,count,elapsed;char pad;unsigned char flags;char end[2];};
struct fpw_first_person {
long flags;long unit_index;long object_indices[10];fpw_weapon weapons[2];float elapsed,duration;short type,control_flags;char camera_transition[12];
 float movement_yaw, movement_pitch;
 float movement_yaw_velocity, movement_pitch_velocity;
 float turn_yaw, turn_pitch;
 float turn_yaw_velocity, turn_pitch_velocity;
 union
 {
     real_euler_angles2d facing_angles;
     struct
     {
         float facing_yaw, aim_pitch;
     };
 };
 real_euler_angles2d facing_delta;
 real_matrix4x3 camera_matrix;long unknown_index,unknown_index2;fpw_animation_id id;
};

STATIC_ASSERT(sizeof(fpw_weapon) == 0x11bc, first_person_weapon_runtime_size);
STATIC_ASSERT(sizeof(fpw_first_person) == 0x2430, first_person_runtime_size);
STATIC_ASSERT(offsetof(fpw_weapon, object_indices) == 4, first_person_weapon_objects_offset);
STATIC_ASSERT(offsetof(fpw_first_person, weapons) == 0x30, first_person_runtime_weapons_offset);

#endif
