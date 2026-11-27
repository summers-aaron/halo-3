#ifndef SIMULATION_SIMULATION_ENCODING_H
#define SIMULATION_SIMULATION_ENCODING_H

// Functions defined in tu/networkinglibxbox/simulation_encoding.cpp.

class c_bitstream;
struct player_action;
union real_point3d;
union real_rectangle3d;
struct s_aim_assist_targeting_result;
struct simulation_player_update;
struct simulation_update;
enum e_simulation_node_space_encoding_type;

void simulation_write_quantized_node_space_position(
    c_bitstream *packet, const real_point3d *position, e_simulation_node_space_encoding_type encoding_type, long bits);

bool aim_assist_targeting_decode(c_bitstream *packet, s_aim_assist_targeting_result *result);
void simulation_write_quantized_position(c_bitstream *stream, const real_point3d *point, long bits, bool within_world, const real_rectangle3d *source);
void simulation_read_quantized_position(class c_bitstream * stream, union real_point3d * out, long bit_count, union real_rectangle3d const * source);
bool simulation_compare_quantized_positions(const real_point3d *first, const real_point3d *second, long bits, const real_rectangle3d *source);
void player_action_encode(c_bitstream *bitstream, const player_action *action);
bool player_action_decode(c_bitstream *bitstream, player_action *action);
bool player_action_compare(const player_action *action1, player_action *action2);
void simulation_player_update_encode(c_bitstream *packet, const simulation_player_update *player_update);
bool simulation_player_update_decode(c_bitstream *packet, simulation_player_update *player_update);
void simulation_encoding_get_world_bounds(real_rectangle3d *bounds);
bool simulation_update_compare(const simulation_update *update1, simulation_update *update2);
void simulation_update_encode(c_bitstream *packet, const simulation_update *update);
bool simulation_update_decode(c_bitstream *packet, simulation_update *update);

struct s_simulation_camera_update;
bool simulation_camera_update_decode(c_bitstream *stream, s_simulation_camera_update *update);

extern "C" void simulation_machine_update_entry_encode(c_bitstream *stream, const s_simulation_camera_update *update);
bool simulation_encoding_point_within_world(const real_point3d *point);

struct real_bounds;
extern "C" const real_bounds k_simulation_node_space_bounds[][3];

union long_point3d;
extern "C" const long_point3d k_quantized_point_neighbor_offsets[14];

#endif // SIMULATION_SIMULATION_ENCODING_H
