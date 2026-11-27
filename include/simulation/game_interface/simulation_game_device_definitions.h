#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_DEVICE_DEFINITIONS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_DEVICE_DEFINITIONS_H

#include "simulation/game_interface/simulation_game_object_entity_definition.h"
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "simulation/simulation_entity_types.h"
#include "networking/simulation/simulation_event_definition.h"

struct s_simulation_device_state_data
{
    s_simulation_object_state_data object_state;
    float position;
    float position_group;
    float power;
    float power_group;
};
typedef char simulation_device_state_size[(sizeof(s_simulation_device_state_data) == 0xb0) ? 1 : -1];
typedef char simulation_device_position_offset[(offsetof(s_simulation_device_state_data, position) == 0xa0) ? 1 : -1];

class c_simulation_device_entity_definition : public c_simulation_object_entity_definition
{
public:
    ~c_simulation_device_entity_definition() {}
    virtual e_simulation_entity_type entity_type();
    virtual const char *entity_type_name();
    virtual long state_data_size();
    virtual bool build_baseline_state_data(long creation_data_size, const void *creation_data, long state_data_size, void *state_data);
    virtual unsigned long handle_object_update(long object_index, unsigned long update_mask, long state_data_size, void *state_data);
    virtual long create_object(long creation_data_size, const void *creation_data, unsigned long *update_mask,
        long state_data_size, const void *state_data);
    virtual void apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data);
    virtual void write_creation_description_to_string(const s_simulation_entity *entity,
        const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer);
    virtual void entity_creation_encode(long creation_data_size, const void *creation_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *update_mask_written,
        long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry,
        c_bitstream *packet, long minimum_bits, bool force);
    virtual bool entity_update_decode(bool initial_update, unsigned long *update_mask, long state_data_size,
        void *state_data, c_bitstream *packet, bool final_update);
    virtual bool entity_state_lossy_compare(void *first, void *second, long state_data_size);
    static void apply_device_position_update(long object_index, bool force, bool position_changed, float position,
        bool position_group_changed, float position_group_position, bool power_changed, float power,
        bool power_group_changed, float power_group_power);
    virtual long creation_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_type_is_gameworld_object();
    virtual bool entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update);
    virtual bool entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool final_update);
    virtual bool entity_creation_lossy_compare(void *first, void *second, long size);
    virtual void rotate_entity_indices(const s_simulation_entity *entity);
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state);
    virtual void build_object_creation_data(long object_index, long creation_data_size, void *creation_data);
    virtual bool object_required_to_join_game(long object_index);
    virtual bool handle_delete_object(long object_index);
    virtual bool promote_object_to_authority(long object_index);
    virtual bool ensure_object_position_update_quantization_inside_bsp();
};

class c_simulation_device_touch_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual const char *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float relevance);
    virtual void write_description_to_string(const c_replication_outgoing_event *event,
        const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_count, const long *references, long payload_size, const void *payload);
};

#endif
