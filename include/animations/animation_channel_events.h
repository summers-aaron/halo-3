#ifndef ANIMATIONS_ANIMATION_CHANNEL_EVENTS_H
#define ANIMATIONS_ANIMATION_CHANNEL_EVENTS_H

#include "cseries/cseries.h"
#include "animations/animation_id.h"
#include "animations/mixing_board/channels/anim_computation_flags_types.h"
#include "math/real_math_types.h"

class c_animation_manager;
class c_model_animation_graph;
class c_model_animation;
struct s_animation_event_data;
struct s_animation_compute_orientations_aiming_screen_bounds;
enum e_animation_playback_flag_bits;

extern "C" void compute_node_orientations_from_blend_screen(long graph_index, c_animation_id animation_id, long node_count, float yaw, float pitch,
 const s_animation_compute_orientations_aiming_screen_bounds *bounds, float weight, const c_static_flags_no_init<255> *node_flags,
 real_orientation *orientations);

#include "animations/animation_node_orientations.h"

extern "C" c_animation_id animation_manager_resolve_animation_id(const c_animation_manager *manager, c_animation_id id);

extern "C" unsigned long check_animation_frame_range_for_events(const c_model_animation_graph *graph, const c_model_animation *animation,
    c_flags<e_animation_playback_flag_bits, unsigned short, 16> *playback_flags, float start_frame, float end_frame,
    void (*callback)(const s_animation_event_data *, long), long user_data);

#endif
