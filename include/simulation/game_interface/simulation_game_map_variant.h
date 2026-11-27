#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_MAP_VARIANT_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_MAP_VARIANT_H

#include "networking/simulation/simulation_entity_definition_types.h"

class c_simulation_map_variant_group_entity_definition : public c_simulation_entity_definition
{
public:
    ~c_simulation_map_variant_group_entity_definition() {}

    virtual bool gameworld_attachment_valid(long gamestate_index);
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update);
    virtual bool create_game_entity(long gamestate_index, long creation_size, const void *creation_data,
        unsigned long update_mask, long state_size, const void *state_data);
    virtual bool build_updated_state_data(const s_simulation_entity *entity, unsigned long *update_mask, long size, void *state);
    virtual void rotate_entity_indices(const s_simulation_entity *entity);
    virtual bool delete_game_entity(long gamestate_index);
    virtual bool build_baseline_state_data(long creation_size, const void *creation_data, long state_size, void *state_data);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *updated_mask,
        long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry,
        c_bitstream *packet, long maximum_bits, bool is_replicated);
    virtual bool entity_update_decode(bool initial_update, unsigned long *update_mask, long size,
        void *state, c_bitstream *packet, bool final_update);
    virtual void write_creation_description_to_string(const s_simulation_entity *entity,
        const s_simulation_view_telemetry_data *telemetry, long size, char *buffer);
    virtual void write_update_description_to_string(const s_simulation_entity *entity,
        const s_entity_update_data *update, long size, char *buffer);
    virtual e_simulation_entity_type entity_type();
    virtual const char *entity_type_name();
    virtual long state_data_size();
    virtual long creation_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_replication_required_for_view_activation(const s_simulation_entity *entity);
    virtual bool entity_type_is_gameworld_object();
    virtual bool entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual void entity_creation_encode(long size, const void *creation_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update);
    virtual bool entity_creation_decode(long size, void *creation_data, c_bitstream *packet, bool final_update);
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
    virtual bool entity_creation_lossy_compare(void *first, void *second, long size);
    virtual void build_creation_data(long gamestate_index, long size, void *creation_data);
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state);
    virtual bool update_game_entity(long gamestate_index, unsigned long update_mask, long state_size, const void *state_data);
    virtual bool promote_game_entity_to_authority(long gamestate_index);
};

#endif
