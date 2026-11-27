#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERNAL_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERNAL_H

// Definition table indices use 12 bits.
__forceinline long simulation_definition_table_index_bits() { return 12; }


class c_bitstream;
union real_point3d;
union real_vector3d;
struct s_simulation_view_telemetry_data;
enum e_simulation_game_position_update_result;
enum e_simulation_game_position_update_context;

e_simulation_game_position_update_result simulation_evaluate_position_update(long object_index,
    e_simulation_game_position_update_context context, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up,
    const real_vector3d *linear_velocity, const real_vector3d *angular_velocity);

float simulation_calculate_position_relevance(const real_point3d *position, float maximum_distance, const s_simulation_view_telemetry_data *telemetry_data);
void simulation_write_definition_index(const char *description, c_bitstream *packet, long definition_index);
long simulation_read_definition_index(const char *description, c_bitstream *packet);
float simulation_calculate_entity_relevance(long count, const long *indices, float distance, const s_simulation_view_telemetry_data *telemetry, bool *out);
void simulation_apply_position_update(long object_index, long context, long result, const real_point3d *position,
    const real_vector3d *forward, const real_vector3d *up, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity);

class c_simulation_event_definition;
class c_replication_outgoing_event;

extern "C" float simulation_calculate_event_relevance(c_simulation_event_definition *definition,
    const c_replication_outgoing_event *event, float relevance, const s_simulation_view_telemetry_data *telemetry, bool *out);

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERNAL_H
