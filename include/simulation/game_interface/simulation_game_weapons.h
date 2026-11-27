#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_WEAPONS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_WEAPONS_H

#include "simulation/game_interface/simulation_game_items.h"
#include "simulation/simulation_queue_event_types.h"
#include "networking/simulation/simulation_event_definition.h"
#include "networking/replication/replication_motion.h"
#include "units/unit_control.h"
#include "cseries/cseries.h"
class c_replication_outgoing_event;

struct s_simulation_weapon_reload_event_data {
    bool primary;
    bool secondary;
    bool tertiary;
};
struct s_simulation_weapon_pickup_event_data {
    s_unit_weapon_set weapon_set;
    long unit_add_weapon_mode;
};
struct s_simulation_weapon_put_away_event_data {
    short unit_weapon_set_index;
    short unit_inventory_index;
    long weapon_definition_index;
};
struct s_simulation_weapon_drop_event_data {
    short unit_weapon_set_index;
    short unit_inventory_index;
    long weapon_definition_index;
};
struct s_simulation_weapon_empty_click_event_data {
    long unit_inventory_index;
    long weapon_definition_index;
};

class c_simulation_weapon_entity_definition : public c_simulation_item_entity_definition {
public:
    ~c_simulation_weapon_entity_definition() {}
    virtual void build_object_creation_data(long object_index, long creation_data_size, void *creation_data);
    virtual unsigned long handle_object_update(long object_index, unsigned long update_mask, long state_data_size, void *state_data);
    virtual char const *entity_type_name();
    virtual long state_data_size();
    virtual bool entity_update_decode(
        bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool final_update);
    virtual long update_flag_count();
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long state_data_size, void *state_data);
    virtual unsigned long initial_update_mask();
    virtual void entity_creation_encode(long, const void *, const s_simulation_view_telemetry_data *, c_bitstream *, bool);
    virtual void write_creation_description_to_string(
        const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long size, char *buffer);
    virtual bool entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool quantize);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *written_update_mask, long size,
        const void *state, const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long maximum_bits, bool final_update);
    virtual bool entity_state_lossy_compare(void *first, void *second, long state_data_size);
    virtual void apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data);
    virtual long create_object(long creation_data_size, const void *creation_data, unsigned long *flags, long state_data_size, const void *state_data);
    virtual bool build_baseline_state_data(long, void const *, long, void *);
    virtual e_simulation_entity_type entity_type();
    virtual long creation_data_size();
};
class c_simulation_weapon_fire_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual char const *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry, float relevance);
    virtual void write_description_to_string(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry,
        float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, void const *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_gamestate_count, long const *gamestate_indices, long payload_size, void const *payload);
};
class c_simulation_weapon_reload_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual char const *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry, float relevance);
    virtual void write_description_to_string(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry,
        float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, void const *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long event_index, long const *entity_reference, long payload_size, void const *payload);
};
class c_simulation_weapon_pickup_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual char const *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry, float relevance);
    virtual void write_description_to_string(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry,
        float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, void const *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long event_index, long const *gamestate_indices, long payload_size, void const *payload);
};
class c_simulation_weapon_put_away_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual char const *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry, float relevance);
    virtual void write_description_to_string(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry,
        float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, void const *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long event_index, long const *entity_reference, long payload_size, void const *payload);
};
class c_simulation_weapon_drop_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual char const *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry, float relevance);
    virtual void write_description_to_string(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry,
        float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long event_index, long const *entity_reference, long payload_size, void const *payload);
};
class c_simulation_weapon_effect_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual char const *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry, float relevance);
    virtual void write_description_to_string(const c_replication_outgoing_event *event,
        const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *stream, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long event_index, long const *gamestate_indices, long payload_size, void const *payload);
};
class c_simulation_weapon_empty_click_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual char const *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry, float relevance);
    virtual void write_description_to_string(c_replication_outgoing_event const *event, s_simulation_view_telemetry_data const *telemetry,
        float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, void const *payload, c_bitstream *packet, bool quantize);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long event_index, long const *entity_reference, long payload_size, void const *payload);
};


struct s_simulation_weapon_fire_impact_target
{
    long encoding_type;
    long entity_index;
    long gamestate_index;
};
struct s_simulation_weapon_fire_projectile
{
    long projectile_id;
    bool impact_valid;
    real_point3d impact_position;
    long object_node_index;
    long target_index;
    bool conical_valid;
    unsigned short conical_random_seed;
};
struct s_simulation_weapon_fire_event_data
{
    long unit_inventory_index;
    long weapon_definition_index;
    long barrel_index;
    union { bool primary_trigger; unsigned char primary_trigger_byte; };
    long impact_target_count;
    s_simulation_weapon_fire_impact_target impact_targets[16];
    long projectile_count;
    s_simulation_weapon_fire_projectile projectiles[16];
    real_vector3d aiming_vector;
    s_player_predicted_aim_assist predicted_aim_assist;
    s_aim_assist_targeting_result aim_assist;
    bool tracked_data_valid;
    long tracked_entity_index;
    long tracked_gamestate_index;
    long tracked_model_target_index;
    bool generated_from_host_live_fire;
};


struct s_simulation_weapon_creation_data
{
    s_simulation_object_creation_data object;
    unsigned short simulation_weapon_identifier;
};

struct s_simulation_weapon_update_state
{
    s_simulation_item_state_data item_state;
    unsigned char multiplayer_flags;
    char multiplayer_flags_padding[3];
    short multiplayer_weapon_identifier;
    unsigned short rounds_loaded;
    unsigned short rounds_inventory;
    float age;
};


struct s_simulation_weapon_effect_event_data
{
    long effect_type;
    real_point3d position;
};


typedef char simulation_weapon_layout_1[(sizeof(s_simulation_weapon_update_state) == 0xb8) ? 1 : -1];
typedef char simulation_weapon_layout_2[(offsetof(s_simulation_weapon_update_state, multiplayer_weapon_identifier) == 0xac) ? 1 : -1];
typedef char simulation_weapon_layout_3[(offsetof(s_simulation_weapon_update_state, age) == 0xb4) ? 1 : -1];
typedef char simulation_weapon_layout_4[(sizeof(s_simulation_weapon_fire_event_data) == 0x33c) ? 1 : -1];
typedef char simulation_weapon_layout_5[(offsetof(s_simulation_weapon_fire_event_data, projectiles) == 0xd8) ? 1 : -1];
typedef char simulation_weapon_layout_6[(offsetof(s_simulation_weapon_fire_event_data, aim_assist) == 0x304) ? 1 : -1];

extern "C" {
extern const char k_multiplayer_flags_exist[];
extern const char k_multiplayer_weapon_identifier_exists[];
extern const char k_rounds_exist[];
}

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_WEAPONS_H
