#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_GENERICS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_GENERICS_H

#include "simulation/game_interface/simulation_game_object_entity_definition.h"

struct s_entity_update_data;
struct s_simulation_entity;
struct s_simulation_generic_state_data;
struct s_simulation_view_telemetry_data;
class c_bitstream;

class c_simulation_generic_entity_definition : public c_simulation_object_entity_definition
{
public:
    ~c_simulation_generic_entity_definition() {}
    virtual const char *entity_type_name();
    virtual long state_data_size();
    virtual bool object_required_to_join_game(long object_index);
    virtual void build_object_creation_data(long object_index, long creation_data_size, void *creation_data);
    virtual bool entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool quantize);
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update);
    virtual void rotate_entity_indices(const s_simulation_entity *entity);
    virtual void write_creation_description_to_string(
        const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long size, char *buffer);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *updated_mask, long state_data_size,
        const void *state_data, const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long maximum_bits, bool final_update);
    virtual bool entity_update_decode(
        bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool is_replicated);
    bool build_baseline_state_data(long creation_data_size, const void *creation_data, long state_data_size, void *state_data);
    virtual unsigned long handle_object_update(long object_index, unsigned long update_mask, long state_data_size, void *state_data);
    virtual void apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data);
    virtual long create_object(
        long creation_data_size, const void *creation_data, unsigned long *flags, long state_data_size, const void *state_data);
    virtual e_simulation_entity_type entity_type();
    virtual long creation_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_type_is_gameworld_object();
    virtual bool entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual void entity_creation_encode(long creation_data_size, const void *creation_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update);
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
    virtual bool entity_creation_lossy_compare(void *first, void *second, long size);
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state);
    virtual bool handle_delete_object(long object_index);
    virtual bool promote_object_to_authority(long object_index);
    virtual bool ensure_object_position_update_quantization_inside_bsp();
    void write_update_description_to_string(const s_simulation_entity *entity, const s_entity_update_data *update, long size, char *buffer);

};

class c_simulation_generic_garbage_entity_definition : public c_simulation_generic_entity_definition
{
public:
    virtual e_simulation_entity_type entity_type();
    virtual const char *entity_type_name();
    virtual long create_object(
        long creation_data_size, const void *creation_data, unsigned long *flags, long state_data_size, const void *state_data);
};

extern "C" unsigned long simulation_generic_object_get_rigid_body_mask(long object_index);

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_GENERICS_H
