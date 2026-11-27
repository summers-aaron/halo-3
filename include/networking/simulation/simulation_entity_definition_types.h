#ifndef NETWORKING_SIMULATION_SIMULATION_ENTITY_DEFINITION_TYPES_H
#define NETWORKING_SIMULATION_SIMULATION_ENTITY_DEFINITION_TYPES_H

struct s_simulation_entity;
struct s_entity_update_data;
struct s_simulation_view_telemetry_data;
class c_bitstream;
enum e_simulation_entity_type;


// Entity definitions supply all slots of this abstract interface.
// Derived engine definitions extend it with additional methods.
class c_simulation_entity_definition
{
public:
    virtual e_simulation_entity_type entity_type() = 0;
    virtual const char *entity_type_name() = 0;
    virtual long state_data_size() = 0;
    virtual long creation_data_size() = 0;
    virtual long update_flag_count() = 0;
    virtual unsigned long initial_update_mask() = 0;
    virtual bool entity_replication_required_for_view_activation(const s_simulation_entity *entity) = 0;
    virtual bool entity_type_is_gameworld_object() = 0;
    virtual bool gameworld_attachment_valid(long gamestate_index) = 0;
    virtual bool entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry)
        = 0;
    virtual long creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry)
        = 0;
    virtual void write_creation_description_to_string(const s_simulation_entity *entity,
        const s_simulation_view_telemetry_data *telemetry, long size, char *buffer) = 0;
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update)
        = 0;
    virtual void entity_creation_encode(long size, const void *creation_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update) = 0;
    virtual bool entity_creation_decode(long size, void *creation_data, c_bitstream *packet, bool final_update)
        = 0;
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *written_update_mask,
        long size, const void *state, const s_simulation_view_telemetry_data *telemetry,
        c_bitstream *packet, long maximum_bits, bool final_update) = 0;
    virtual bool entity_update_decode(bool initial_update, unsigned long *update_mask, long size,
        void *state, c_bitstream *packet, bool final_update) = 0;
    virtual bool entity_state_lossy_compare(void *first, void *second, long size) = 0;
    virtual bool entity_creation_lossy_compare(void *first, void *second, long size) = 0;
    virtual void build_creation_data(long gamestate_index, long size, void *creation_data) = 0;
    virtual bool build_baseline_state_data(long creation_size, const void *creation_data, long state_size, void *state_data)
        = 0;
    virtual bool build_updated_state_data(const s_simulation_entity *entity, unsigned long *update_mask, long size, void *state)
        = 0;
    virtual void rotate_entity_indices(const s_simulation_entity *entity) = 0;
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data) = 0;
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state) = 0;
    virtual bool create_game_entity(long gamestate_index, long creation_size, const void *creation_data,
        unsigned long update_mask, long state_size, const void *state_data) = 0;
    virtual bool update_game_entity(long gamestate_index, unsigned long update_mask, long state_size, const void *state_data)
        = 0;
    virtual bool delete_game_entity(long gamestate_index) = 0;
    virtual bool promote_game_entity_to_authority(long gamestate_index) = 0;
};

typedef char simulation_entity_definition_size[(sizeof(c_simulation_entity_definition) == sizeof(void *)) ? 1 : -1];

#endif
