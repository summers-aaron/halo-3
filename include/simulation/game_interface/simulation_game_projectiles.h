#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_PROJECTILES_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_PROJECTILES_H

#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "simulation/game_interface/simulation_game_object_entity_definition.h"
#include "networking/simulation/simulation_event_definition.h"

class c_bitstream;
class c_replication_outgoing_event;
struct s_simulation_entity;
struct s_entity_update_data;
struct s_simulation_view_telemetry_data;

struct s_projectile_state_data { s_simulation_object_state_data object; bool at_rest; };

struct s_projectile_targeting_data
{
    long object_index;
    short model_target_index;
    short pad6;
};

struct s_simulation_projectile_effect_event_data
{
    long definition_index;
    real_point3d position;
    real_vector3d forward;
    unsigned long flags;
    real_vector3d material_normal;
    long material_index;
    bool has_material_effect;
    long material_effect_index;
};

struct s_projectile_entity_update_state { s_simulation_object_state_data object; union { bool at_rest; unsigned char at_rest_byte; }; };

struct s_simulation_projectile_impact_effect_event_data
{
    long definition_index;
    float scale;
    float error;
    real_vector3d direction;
    real_point3d position;
    real_vector3d normal;
    long material_index;
    bool extra;
};

struct s_simulation_projectile_object_impact_effect_event_data
{
    long definition_index;
    float scale;
    float error;
    real_vector3d direction;
    real_point3d position;
    real_vector3d normal;
    long material_index;
    bool extra;
    char extra_padding[3];
    bool effect;
    long node_index;
    long position_encoding_type;
};

struct s_simulation_projectile_creation_data
{
    s_simulation_object_creation_data object_creation;
    long owner_player_index, target_entity_index, target_model_target_index;
    bool tracer, disable_deceleration;
    long target_gamestate_index;
};

struct s_simulation_projectile_attached_event_data
{
    bool attached_to_object;
    long node_index;
    long position_encoding_type;
    real_point3d position;
    unsigned char location[4];
};

struct s_simulation_projectile_supercombine_request_event_data { long definition_index; };

class c_simulation_projectile_entity_definition : public c_simulation_object_entity_definition
{
public:
    ~c_simulation_projectile_entity_definition() {}
    virtual void build_object_creation_data(long object_index, long size, void *creation_data);
    virtual bool entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool final_update);
    virtual const char *entity_type_name();
    virtual long creation_data_size();
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data);
    virtual bool handle_delete_object(long object_index);
    virtual bool entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state);
    virtual void rotate_entity_indices(const s_simulation_entity *entity);
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update);
    virtual long create_object(long p1, const void *p2, unsigned long *p3, long p4, const void *p5);
    virtual unsigned long handle_object_update(long p1, unsigned long p2, long p3, void *p4);
    virtual bool entity_update_encode(bool p1, unsigned long p2, unsigned long *p3, long p4, const void *p5,
     const s_simulation_view_telemetry_data *telemetry, c_bitstream *bitstream, long p8, bool p9);
    virtual bool entity_update_decode(bool p1, unsigned long *p2, long p3, void *p4, c_bitstream *bitstream, bool p6);
    virtual void apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data);
    virtual void write_creation_description_to_string(
        const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer);
    virtual void entity_creation_encode(long creation_data_size, const void *creation_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update);
    virtual e_simulation_entity_type entity_type();
    virtual long state_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_type_is_gameworld_object();
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
    virtual bool entity_creation_lossy_compare(void *first, void *second, long size);
    virtual bool build_baseline_state_data(long creation_data_size, const void *creation_data, long state_data_size, void *state_data);
    virtual bool object_required_to_join_game(long object_index);
    virtual bool promote_object_to_authority(long object_index);
    virtual bool ensure_object_position_update_quantization_inside_bsp();

};

class c_simulation_projectile_effect_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual const char *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance);
    virtual void write_description_to_string(
        const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_gamestate_count, const long *gamestate_indices, long payload_size, const void *payload);
};

class c_simulation_projectile_supercombine_request_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual const char *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual long minimum_required_bits(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual float calculate_relevance(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance);
    virtual void write_description_to_string(
        const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_gamestate_count, const long *gamestate_indices, long payload_size, const void *payload);
};

class c_simulation_projectile_attached_event_definition : public c_simulation_event_definition
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
    virtual void write_description_to_string(
        const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_gamestate_count, const long *gamestate_indices, long payload_size, const void *payload);
};

class c_simulation_projectile_impact_effect_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual const char *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance);
    virtual void write_description_to_string(
        const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float maximum_distance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_gamestate_count, const long *gamestate_indices, long payload_size, const void *payload);
};

class c_simulation_projectile_object_impact_effect_event_definition : public c_simulation_event_definition
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
    virtual bool apply_game_event(long reference_gamestate_count, const long *gamestate_indices, long payload_size, const void *payload);
};

typedef char projectile_creation_packet_size[(sizeof(s_simulation_projectile_creation_data) == 0x24) ? 1 : -1];
typedef char projectile_creation_target_offset[(offsetof(s_simulation_projectile_creation_data, target_entity_index) == 0x14) ? 1 : -1];
typedef char projectile_creation_gamestate_offset[(offsetof(s_simulation_projectile_creation_data, target_gamestate_index) == 0x20) ? 1 : -1];
typedef char projectile_object_impact_payload_size[(sizeof(s_simulation_projectile_object_impact_effect_event_data) == 0x44) ? 1 : -1];

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_PROJECTILES_H
