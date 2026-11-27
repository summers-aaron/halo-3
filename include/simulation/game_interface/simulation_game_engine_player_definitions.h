#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ENGINE_PLAYER_DEFINITIONS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ENGINE_PLAYER_DEFINITIONS_H

#include "networking/simulation/simulation_entity_definition_types.h"
#include "networking/simulation/simulation_event_definition.h"
#include "game/player_traits.h"
#include "math/real_math_types.h"

struct s_simulation_game_engine_player_state_data
{
    signed char waypoint_action;
    unsigned long respawn_timer_ticks;
    real_vector3d dead_location;
    c_player_traits traits;
    bool blocking_teleporter;
    short lives_remaining;
    long last_betrayer;
    unsigned short respawn_timer;
    bool vehicle_entrance_ban;
    bool active_in_game;
    bool sitting_out;
    short desired_respawn_player;
    real_vector3d player_control_aiming;
    bool is_host;
    unsigned short host_estimated_bps;
    unsigned short host_transmission_rate;
    unsigned short host_transmission_bps;
    unsigned short client_rtt_msec;
    unsigned short client_packet_rate;
    unsigned short client_bandwidth_bps;
    unsigned short client_packet_loss;
};
struct s_simulation_game_engine_player_creation_data
{
    short absolute_player_index;
};
struct s_player_respawn_target_request_event_data
{
    short request_player_index;
    short desired_respawn_player;
};
class c_simulation_game_engine_player_entity_definition : public c_simulation_entity_definition
{
public:
    ~c_simulation_game_engine_player_entity_definition() {}

    virtual const char *entity_type_name();
    virtual bool gameworld_attachment_valid(long gamestate_index);
    virtual bool promote_game_entity_to_authority(long gamestate_index);
    virtual void build_creation_data(long gamestate_index, long creation_data_size, void *creation_data);
    virtual void entity_creation_encode(long creation_data_size, const void *creation_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool encoding_is_for_debug);
    virtual bool entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool decoding_is_for_debug);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *written_update_mask,
        long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry,
        c_bitstream *packet, long maximum_bits, bool final_update);
    virtual bool entity_update_decode(
        bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool final_update);
    virtual bool create_game_entity(long gamestate_index, long creation_data_size, const void *creation_data,
        unsigned long initial_update_mask, long state_data_size, const void *state_data);
    virtual bool delete_game_entity(long gamestate_index);
    virtual bool build_baseline_state_data(long creation_data_size, const void *creation_data, long state_data_size, void *state_data);
    virtual bool build_updated_state_data(const s_simulation_entity *entity, unsigned long *update_mask, long state_data_size, void *state_data);
    virtual bool update_game_entity(long gamestate_index, unsigned long update_mask, long state_data_size, const void *state_data);
    virtual void write_update_description_to_string(const s_simulation_entity *entity,
        const s_entity_update_data *update_data, long buffer_size, char *buffer);
    virtual void write_creation_description_to_string(
        const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer);
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
    virtual void rotate_entity_indices(const s_simulation_entity *entity);
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state);
};
class c_simulation_player_respawn_request_event_definition : public c_simulation_event_definition
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
    virtual void write_description_to_string(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry,
        float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool encoding_is_for_debug);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool decoding_is_for_debug);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_gamestate_count, const long *gamestate_indices, long payload_size, const void *payload);
};
class c_simulation_player_editor_request_event_definition : public c_simulation_event_definition
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
    virtual void write_description_to_string(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry,
        float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long, const void *, c_bitstream *, bool);
    virtual bool event_payload_decode(long, void *, c_bitstream *, bool);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_gamestate_count, const long *reference_gamestate_indices, long payload_size, const void *payload);
};

typedef char simulation_game_engine_player_state_size[(sizeof(s_simulation_game_engine_player_state_data) == 92) ? 1 : -1];
typedef char simulation_game_engine_player_creation_size[(sizeof(s_simulation_game_engine_player_creation_data) == 2) ? 1 : -1];
typedef char simulation_player_respawn_request_size[(sizeof(s_player_respawn_target_request_event_data) == 4) ? 1 : -1];

#endif
