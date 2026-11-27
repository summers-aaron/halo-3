#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_VEHICLES_INTERNALS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_VEHICLES_INTERNALS_H

#include <stddef.h>
#include "memory/bitstream.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_entity_definition.h"
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "simulation/game_interface/simulation_game_object_entity_definition.h"
#include "units/vehicle_datum.h"

extern "C" const real_vector3d k_real_math_zero_vector3d_storage;

const s_damage_owner *damage_owner_unknown();

struct s_simulation_vehicle_creation_data : s_simulation_object_creation_data
{
    char variant_name[32];
};

struct s_simulation_vehicle_state_data
{
    s_simulation_object_state_data object_state;
    bool auto_turret_enabled;
    bool auto_primary_trigger;
    bool auto_secondary_trigger;
    real_vector3d auto_aiming_vector;
    long emp_timer;
};

typedef char simulation_vehicle_state_size[(sizeof(void *) != 4 || (sizeof(s_simulation_vehicle_state_data) == 0xb4)) ? 1 : -1];
typedef char simulation_vehicle_aiming_offset[(sizeof(void *) != 4 || (offsetof(s_simulation_vehicle_state_data, auto_aiming_vector) == 0xa4)) ? 1 : -1];
typedef char simulation_vehicle_emp_offset[(sizeof(void *) != 4 || (offsetof(vehicle_datum, unit.stun_ticks) == 0x36a)) ? 1 : -1];
typedef char simulation_vehicle_forward_offset[(sizeof(void *) != 4 || (offsetof(vehicle_datum, vehicle.forward) == 0x768)) ? 1 : -1];

class c_simulation_vehicle_entity_definition : public c_simulation_object_entity_definition
{
public:
    ~c_simulation_vehicle_entity_definition() {}
    virtual bool entity_update_decode(
        bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool is_replicated);
    virtual const char *entity_type_name();
    virtual long state_data_size();
    virtual unsigned long initial_update_mask();
    virtual bool object_required_to_join_game(long object_index);
    virtual bool build_baseline_state_data(long creation_data_size, const void *creation_data, long state_data_size, void *state_data);
    virtual unsigned long handle_object_update(long object_index, unsigned long update_mask, long state_data_size, void *state_data);
    virtual void apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data);
    virtual long update_minimum_required_bits(const s_simulation_entity *, const s_entity_update_data *);
    virtual void entity_creation_encode(long creation_data_size, const void *creation_data, const s_simulation_view_telemetry_data *telemetry,
        c_bitstream *packet, bool quantize);
    virtual bool entity_state_lossy_compare(void *first, void *second, long state_data_size);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *update_mask_written, long state_data_size,
        const void *state_data, const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long minimum_bits, bool lossy);
    virtual void write_creation_description_to_string(
        const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer);
    virtual e_simulation_entity_type entity_type();
    virtual long creation_data_size();
    virtual long update_flag_count();
    virtual bool entity_type_is_gameworld_object();
    virtual bool entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual bool entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool final_update);
    virtual bool entity_creation_lossy_compare(void *first, void *second, long size);
    virtual void rotate_entity_indices(const s_simulation_entity *entity);
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state);
    virtual void build_object_creation_data(long object_index, long creation_data_size, void *creation_data);
    virtual long create_object(long creation_data_size, const void *creation_data, unsigned long *update_mask, long state_data_size, const void *state_data);
    virtual bool handle_delete_object(long object_index);
    virtual bool promote_object_to_authority(long object_index);
    virtual bool ensure_object_position_update_quantization_inside_bsp();
    void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update, long size, char *buffer);

};

struct s_vehicle_type_definition
{
    char unit_definition_prefix[0x288];
    long count;
};

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_VEHICLES_INTERNALS_H
