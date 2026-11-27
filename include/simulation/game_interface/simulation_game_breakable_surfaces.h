#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_BREAKABLE_SURFACES_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_BREAKABLE_SURFACES_H

#include "networking/simulation/simulation_entity_definition_types.h"
#include "game/game_engine_scoring_globals.h"
#include "memory/tls_globals.h"

class c_simulation_breakable_surface_group_entity_definition : public c_simulation_entity_definition
{
public:
    ~c_simulation_breakable_surface_group_entity_definition() {}
    virtual const char *entity_type_name();
    virtual bool gameworld_attachment_valid(long gamestate_index);
    virtual void write_creation_description_to_string(const s_simulation_entity *, const s_simulation_view_telemetry_data *, long, char *);
    virtual void entity_creation_encode(long, const void *, const s_simulation_view_telemetry_data *, c_bitstream *, bool);
    virtual bool entity_creation_decode(long, void *, c_bitstream *, bool);
    virtual bool entity_update_encode(bool, unsigned long, unsigned long *, long,
        const void *, const s_simulation_view_telemetry_data *, c_bitstream *, long, bool);
    virtual bool entity_update_decode(bool, unsigned long *, long, void *, c_bitstream *, bool);
    virtual void build_creation_data(long, long, void *);
    virtual bool build_baseline_state_data(long, const void *, long, void *);
    virtual bool create_game_entity(long, long, const void *, unsigned long, long, const void *);
    virtual bool update_game_entity(long, unsigned long, long, const void *);
    virtual bool delete_game_entity(long);
    virtual bool promote_game_entity_to_authority(long);
    virtual void write_update_description_to_string(const s_simulation_entity *, const s_entity_update_data *, long, char *);
    virtual e_simulation_entity_type entity_type();
    virtual long state_data_size();
    virtual long creation_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_replication_required_for_view_activation(const s_simulation_entity *entity);
    virtual bool entity_type_is_gameworld_object();
    virtual bool entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update);
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
    virtual bool entity_creation_lossy_compare(void *first, void *second, long size);
    virtual bool build_updated_state_data(const s_simulation_entity *entity, unsigned long *update_mask, long size, void *state);
    virtual void rotate_entity_indices(const s_simulation_entity *entity);
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state);
};

// map_info (+0x78) contains m_simulation_indices at +0xdf4c.
typedef char breakable_surface_gamestate_indices_offset[
    (offsetof(s_game_engine_globals, map_info) + offsetof(c_map_variant, m_simulation_indices) == 0xdfc4) ? 1 : -1];

static __forceinline long breakable_surface_group_get_gamestate_index(long group_index)
{
    return tls_game_engine_globals()->map_info.m_simulation_indices[group_index];
}

static __forceinline void breakable_surface_group_set_gamestate_index(long group_index, long gamestate_index)
{
    tls_game_engine_globals()->map_info.m_simulation_indices[group_index] = gamestate_index;
}

#endif
