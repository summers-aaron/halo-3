#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ITEMS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ITEMS_H

#include <stddef.h>
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "simulation/game_interface/simulation_game_object_entity_definition.h"

struct s_entity_update_data;
struct s_simulation_entity;
struct s_simulation_view_telemetry_data;
class c_bitstream;

struct s_simulation_item_state_data
{
    s_simulation_object_state_data object;
    union { bool at_rest; unsigned char at_rest_byte; };
    union { bool equipment_deployed; unsigned char equipment_deployed_byte; };
    union { bool equipment_activated; unsigned char equipment_activated_byte; };
    short equipment_creator_player_index;
};
typedef char simulation_item_state_size[(sizeof(s_simulation_item_state_data) == 168) ? 1 : -1];

class c_simulation_item_entity_definition : public c_simulation_object_entity_definition
{
public:
    ~c_simulation_item_entity_definition() {}
    virtual const char *entity_type_name(void);
    virtual unsigned long handle_object_update(long object_index, unsigned long update_mask, long state_data_size, void *state_data);
    virtual void build_object_creation_data(long object_index, long creation_data_size, void *creation_data);
    virtual bool build_baseline_state_data(long creation_size, const void *creation_data, long state_size, void *state_data);
    virtual bool entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *written_update_mask, long size,
        const void *state, const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long maximum_bits, bool final_update);
    virtual bool entity_update_decode(
        bool initial_update, unsigned long *update_mask, long size, void *state, c_bitstream *packet, bool final_update);
    virtual bool entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool quantize);
    virtual long create_object(
        long creation_data_size, const void *creation_data, unsigned long *flags, long state_data_size, const void *state_data);
    virtual void write_creation_description_to_string(
        const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer);

    virtual e_simulation_entity_type entity_type();
    virtual long state_data_size();
    virtual long creation_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_type_is_gameworld_object();
    virtual long creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual void entity_creation_encode(long creation_data_size, const void *creation_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update);
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
    virtual bool entity_creation_lossy_compare(void *first, void *second, long size);
    virtual void rotate_entity_indices(const s_simulation_entity *entity);
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state);
    virtual bool object_required_to_join_game(long object_index);
    virtual bool handle_delete_object(long object_index);
    virtual void apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data);
    virtual bool promote_object_to_authority(long object_index);
    virtual bool ensure_object_position_update_quantization_inside_bsp();

protected:
    bool item_update_encode(bool initial_update, unsigned long update_mask, unsigned long *updated_mask,
        const s_simulation_view_telemetry_data *telemetry, const s_simulation_item_state_data *state_data, c_bitstream *packet,
        long maximum_bits, bool final_update);
};

extern "C" bool simulation_item_decode_update(c_simulation_item_entity_definition *entity_definition, bool initial_update,
    unsigned long *update_mask, void *state_data, c_bitstream *packet, bool final_update);

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ITEMS_H
