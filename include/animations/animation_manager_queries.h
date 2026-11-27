#ifndef ANIMATIONS_ANIMATION_MANAGER_QUERIES_H
#define ANIMATIONS_ANIMATION_MANAGER_QUERIES_H

class c_aiming_channel;
class c_facial_animation_channel;
class c_animation_id;
class c_animation_manager;
struct s_channel_limits;
struct animation_aiming_screen_bounds;
struct flags256;
struct real_orientation;

extern "C" const s_channel_limits *mixing_board_get_blend_screen_bounds(const c_aiming_channel *channel);
#ifdef HALO3_64BIT
extern "C" void animation_data_channel_update_flags(c_facial_animation_channel *channel, void *argument);
#else
extern "C" void animation_data_channel_update_flags(c_facial_animation_channel *channel, int argument);
#endif
extern "C" void compute_node_orientations_from_blend_screen(long graph_index, c_animation_id animation_id, long node_count, float yaw,
    float pitch, const animation_aiming_screen_bounds *bounds, float weight, const flags256 *node_flags, real_orientation *orientations);

#endif // ANIMATIONS_ANIMATION_MANAGER_QUERIES_H
