#ifndef NETWORKING_NETWORK_CONFIGURATION_TYPES_H
#define NETWORKING_NETWORK_CONFIGURATION_TYPES_H

#include "math/real_math_types.h"
#include "networking/delivery/network_channel.h"
#include <stddef.h>
#include "networking/session/network_observer_configuration.h"
#include "networking/simulation/simulation_event_configuration.h"
#include "simulation/simulation_queue_event_types.h"

typedef s_simulation_event_configuration s_simulation_event_relevance_entry;

enum
{
    k_network_configuration_skill_level_count = 50
};

struct s_simulation_entity_update_configuration
{
    float base_priority;
    float relevance_radius;
    real_bounds relevance;
    long minimum_period;
    long maximum_period;
    float initial_minimum_priority;
    float initial_maximum_priority;
    long maximum_delay;
    float late_minimum_priority;
    float late_maximum_priority;
    float maximum_priority;
    float special_priority;
    float flagged_priority;
    float occupied_vehicle_bonus;
};
struct s_simulation_entity_creation_configuration
{
    float base_priority;
    float relevance_radius;
    float minimum_priority;
    float maximum_priority;
};
struct s_simulation_entity_configuration
{
    s_simulation_entity_creation_configuration creation;
    s_simulation_entity_update_configuration update;
};
struct s_matchmaking_desirability_configuration
{
    long base_desirability;
    long desirability_per_player;
    long minimum_player_count_reached_desirability;
    long preferred_party_size_desirability;
    long gather_seconds_threshold;
    long gather_seconds_desirability;
    long search_seconds_threshold;
    long search_seconds_desirability;
    long skill_difference_maximum;
    long skill_difference_desirability;
    float experience_difference_maximum;
    long experience_difference_desirability;
    long rank_difference_maximum;
    long rank_difference_desirability;
    long single_guest_desirability;
    long region_desirability;
    long country_desirability;
    long language_desirability;
    long composition_flag_desirability;
    long unused_desirability;
    long latency_maximum;
    long latency_divisor;
    long latency_scale;
};
struct s_simulation_range { long first; long second; };
struct s_entity_relevance_configuration
{
    float zoom_cosine[2];
    float zoom_relevance[2];
};
struct s_position_relevance_configuration
{
    float minimum_distance;
    float maximum_cosine;
    float minimum_cosine;
    float minimum_view_distance;
};
struct s_bandwidth_host_preference_weights
{
    unsigned long connectivity;
    unsigned long local_host;
    unsigned long peer_capacity;
    unsigned long machine_capacity;
    unsigned long player_capacity;
    unsigned long latency;
    unsigned long gamestate;
    unsigned long open_nat;
    unsigned long flagged;
    unsigned long player_count;
    unsigned long quality_tier;
    unsigned long latency_tier;
    unsigned long bandwidth_capacity;
};
struct s_bandwidth_configuration
{
    float trim_fraction;
    long minimum_measurements;
    long safety_margin;
    long minimum_bandwidth;
    long interval;
    float ratio;
    long dispute_threshold;
    long dispute_margin;
    long dispute_increment;
    long host_migration_interval_campaign;
    long host_migration_interval_matchmaking;
    long host_migration_interval_group;
    long host_migration_minimum_interval;
    long host_speculative_migration_required_lobby_peer_connectivity_difference;
    long latency_tier_threshold;
    long capacity_threshold;
    long other_latency_tier_threshold;
    long other_capacity_threshold;
    unsigned char latency_scores[20];
    unsigned char player_count_scores[8];
    s_bandwidth_host_preference_weights host_preference_weights[4];
    long latency_tier_delta;
    long quality_tier_delta;
};

struct s_network_configuration_map_entry
{
    long map_id;
    long map_status;
};

struct s_online_lsp_configuration
{
    long port;
    long port_count;
    long minimum_score;
    long search_timeout;
    long idle_timeout;
};

struct s_network_configuration
{
    long request_cache_clear_interval;
    long required_file_invalidation_check_interval_msec;
    long required_file_download_retry_interval_msec;
    s_bandwidth_configuration bandwidth;
    long upstream_bandwidth_absolute_minimum_bps[17];
    long machine_capacity_bandwidth_thresholds[17];
    long player_capacity_bandwidth_thresholds[17];
    long matchmaking_host_minimum_machine_capacity;
    long matchmaking_host_minimum_downstream_bandwidth;
    long matchmaking_host_minimum_upstream_bandwidth;
    long matchmaking_host_downstream_bandwidth_improvement;
    long matchmaking_good_host_downstream_bandwidth;
    long matchmaking_good_host_upstream_bandwidth;
    long minimum_voice_repeater_downstream_bandwidth;
    long minimum_voice_repeater_upstream_bandwidth;
    long minimum_bandwidth_bps;
    long join_host_timeout;                           // network_join.cpp
    long join_disband_timeout;
    unsigned long squad_join_timeout;
    long request_migration_start_rety_interval_msec;
    long request_migration_abort_rety_interval_msec;
    long joining_search_qos_bps;
    unsigned long group_join_timeout;
    long desperation_wait_time_seconds;
    bool matchmaking_disallow_restricted_nat_hosts;
    float matchmaking_strict_nat_host_percentage;
    long squad_start_failure_give_up_time_msec;
    long minimum_joinable_sessions;
    long join_delay;
    long search_stale_timeout;
    long search_join_delay;
    long stream_period;
    long group_join_delay;
    char prepare_map_display_map_during_loading;
    long matchmaking_veto_countdown;
    long matchmaking_loading_timeout;
    long matchmaking_verify_countdown;
    long matchmaking_ready_countdown;
    long matchmaking_loading_countdown;
    long end_game_write_stats_timeout_seconds;
    long matchmaking_arbitration_registration_timeout_seconds;
    long matchmaking_arbitration_completion_timeout_seconds;
    bool return_to_pre_game;
    long post_match_leaderboard_refresh_countdown;
    long nat_warning_interval_seconds;
    long simulation_abort_host_timeout;
    long simulation_abort_peer_timeout;
    long unsuitable_count;
    long qos_bandwidth;
    long qos_refresh_interval;
    long qos_timeout;
    long maximum_qos_target_count;
    long session_qos_target_limit;
    s_matchmaking_desirability_configuration matchmaking_desirability;
    long search_start_delay;
    long search_stage_timeout;
    long search_final_delay;
    long strict_skill_steps;
    float strict_mu_delta_start;
    float strict_mu_delta_end;
    long strict_skill_range_start;
    long strict_skill_range_end;
    long strict_skill_level_delta_start;
    long strict_skill_level_delta_end;
    long strict_experience_delta_start;
    long maximum_latency;
    long skill_steps;
    long skill_limit;
    long matchmaking_search_state_timeout;
    long any_maximum_latency;
    float any_mu_delta;
    long any_skill_level_delta;
    long search_stage_any_final_desired_skill_range;
    long leaderboard_query_failure_count;
    long leaderboard_query_retry_delay_msec;
    long leaderboard_query_maximum_retry_delay_msec;
    long leaderboard_refresh_msec;
    long peer_properties_update_interval;
    long player_add_interval;
    long player_remove_interval;
    long user_properties_update_interval;
    long film_reset_interval;
    long post_match_recover_time;
    long post_match_settle_time;
    long qos_reply_update_interval;
    long qos_reply_bandwidth_limit;
    long squad_qos_reply_bandwidth_limit;
    long squad_pregame_qos_reply_bandwidth_limit;
    long group_qos_reply_bandwidth_limit;
    long group_in_game_qos_reply_bandwidth_bps;
    long maximum_player_count;
    long maximum_map_editor_player_count;
    long machine_file_timeout_seconds;
    long machine_file_refresh_seconds;
    long user_file_timeout_msec;                      // network_banhammer.cpp
    long user_file_request_timeout_msec;
    unsigned long qos_quality_scale;
    long controller_idle_timeout_seconds;             // network_banhammer_controller_idle_for_too_long (seconds)
    s_entity_relevance_configuration entity_relevance;
    float control_growth_base;
    float control_growth_shift;
    float control_maximum_growth_shift;
    long control_probe_time;
    long control_recover_time;
    s_position_relevance_configuration position_relevance;
    long maximum_value;                               // connection rating
    s_simulation_range ranges[4];
    long game_results_update_interval;
    long synchronous_client_block_duration_msec;
    float simulation_actor_control_timeout;
    float projectile_supercombine_minimum_fraction;
    long maximum_catchup_views;
    long simulation_join_timeout;
    long simulation_join_partial_timeout;
    long simulation_join_complete_timeout;
    long simulation_join_total_timeout;
    float simulation_join_active_fraction;
    float simulation_join_completion_threshold;
    long simulation_maximum_catchup_attempts;
    long simulation_catchup_retry_delay;
    long simulation_maximum_join_attempts;
    long simulation_join_failure_timeout;
    float simulation_queue_allocated_bytes_limit;
    float simulation_queue_allocated_elements_limit;
    s_simulation_event_configuration event_relevance[k_simulation_event_type_count];
    s_simulation_entity_configuration records[22];
    float position_update_distance;
    float position_update_distance_remote;
    float vehicle_position_update_distance;
    float vehicle_position_update_distance_remote;
    float supercombine_minimum_time;
    float supercombine_minimum_distance;
    float trigger_mispredict_time;
    float weapon_ticks_seconds;
    float weapon_ticks_threshold;
    float weapon_ticks_minimum;
    long event_timeout;
    float default_priority;
    long stale_timeout;                               // replication control
    float stale_priority;
    float stale_offset;
    float stale_scale;
    float current_offset;
    float current_scale;
    long replication_request_limit;
    long managed_session_retry_interval;              // network_managed_session.cpp
    long peer_join_request_check_interval;
    long peer_join_secure_connection_timeout;
    long peer_join_initial_update_timeout;
    long join_queue_timeout;                          // network_join.cpp
    long join_queue_acknowledge_period;
    long peer_join_abort_check_interval;
    long peer_join_abort_timeout;
    long host_rejoin_accept_timeout_msec;
    long peer_leave_timeout;
    long peer_leave_request_check_interval;
    long host_handoff_acknowledge_timeout;
    long host_handoff_stabilize_timeout;
    long host_handoff_timeout;
    long host_handoff_transition_timeout;
    long host_reestablish_acknowledge_timeout;
    long host_reestablish_message_interval;             // network_session::check_to_send_host_reestablish
    long election_timeout;
    long election_peer_timeout;
    long election_ignore_dissension_msec;
    long election_message_interval;
    long time_synchronization_interval_a;
    long time_synchronization_interval_b;
    long minimum_election_send_interval_msec;
    long host_migration_on_channel_death;
    s_observer_configuration observer;
    s_channel_configuration channel_configuration;
    long packet_statistics_interval;
    long connection_sequence_rate;
    long connection_ancient_packet_threshold;
    long connection_drop_threshold;
    long connection_packet_retire_time;
    long latency_average_gain_bits;
    long latency_deviation_shift;
    long stream_timeout_deviation_scale;
    long stream_timeout_minimum_margin;
    long stream_timeout_minimum;
    long timeout_deviation_scale;
    long timeout_minimum_margin;
    long timeout_minimum;
    long initial_latency_minimum_msec;
    long initial_latency_average_msec;
    long initial_latency_deviation;
    long initial_timeout_msec;
    long backoff_increment_msec;
    long backoff_max_msec;
    long backoff_decrement_msec;
    float bandwidth_warning_latency_multiplier;
    long bandwidth_warning_latency_minimum_increase;
    long address_resolution_timeout_msec;
    long transport_qos_probe_count[2];
    long transport_qos_bandwidth[2];
    bool cap_live_service_upstream;
    long upstream_cap_multiplier;
    long upstream_cap_divisor;
    float push_to_talk_idle_timeout;
    float push_to_talk_timeout;
    bool voice_repeater_enabled;
    long maximum_voice_repeater_players;
    long ui_upload_record_threshold;
    long ui_upload_time_threshold;
    char record_uploads_prevent_game_from_starting;
    unsigned long betrayal_decay_interval;
    unsigned long quit_decay_interval;
    float betrayal_threshold;
    long ejection_cutoff;
    float friendly_assist_amount;
    long network_heap_large_size;
    long network_heap_small_size;
    long allow_matchmaking_abort_interval_msec;
    long maximum_theater_player_count;
    long basic_training_completion_minimum_games_completed;
    long basic_training_completion_minimum_experience;
    long skill_level_experience_thresholds[k_network_configuration_skill_level_count];
    long rank_grade_thresholds[56];
    long rank_experience_thresholds[14];
    char disable_game;
    char custom_games_disabled;
    long alpha_locked_down;
    long maximum_players_per_peer;
    bool crash_reporting_enabled;
    long crash_minidump_mode;
    s_online_lsp_configuration online_lsp;
    s_network_configuration_map_entry map_table[32];
    bool allow_mismatched_peer_properties;
    bool require_matching_peer_properties;
    char disallow_cross_language_coop;
};

typedef char network_configuration_betrayal_decay_offset[(offsetof(s_network_configuration, betrayal_decay_interval) == 0x1310) ? 1 : -1];
typedef char network_configuration_quit_decay_offset[(offsetof(s_network_configuration, quit_decay_interval) == 0x1314) ? 1 : -1];
typedef char network_configuration_betrayal_threshold_offset[(offsetof(s_network_configuration, betrayal_threshold) == 0x1318) ? 1 : -1];
typedef char network_configuration_player_add_interval_offset[(offsetof(s_network_configuration, player_add_interval) == 0x388) ? 1 : -1];
typedef char network_configuration_host_reestablish_interval_offset[(offsetof(s_network_configuration, host_reestablish_message_interval) == 0xe30) ? 1 : -1];
typedef char network_configuration_peer_properties_interval_offset[(offsetof(s_network_configuration, peer_properties_update_interval) == 0x384) ? 1 : -1];
typedef char network_configuration_player_remove_interval_offset[(offsetof(s_network_configuration, player_remove_interval) == 0x38c) ? 1 : -1];
typedef char network_configuration_user_properties_interval_offset[(offsetof(s_network_configuration, user_properties_update_interval) == 0x390) ? 1 : -1];
typedef char network_configuration_film_reset_interval_offset[(offsetof(s_network_configuration, film_reset_interval) == 0x394) ? 1 : -1];
typedef char network_configuration_qos_reply_interval_offset[(offsetof(s_network_configuration, qos_reply_update_interval) == 0x3a0) ? 1 : -1];
typedef char network_configuration_squad_qos_reply_offset[(offsetof(s_network_configuration, squad_qos_reply_bandwidth_limit) == 0x3a8) ? 1 : -1];
typedef char network_configuration_group_qos_reply_offset[(offsetof(s_network_configuration, group_qos_reply_bandwidth_limit) == 0x3b0) ? 1 : -1];
typedef char network_configuration_entity_relevance_offset[(offsetof(s_network_configuration, entity_relevance) == 0x3d8) ? 1 : -1];
typedef char network_configuration_machine_bandwidth_offset[(offsetof(s_network_configuration, machine_capacity_bandwidth_thresholds) == 0x18c) ? 1 : -1];
typedef char network_configuration_connection_sequence_offset[(offsetof(s_network_configuration, connection_sequence_rate) == 0x1280) ? 1 : -1];
typedef char network_configuration_connection_retire_offset[(offsetof(s_network_configuration, connection_packet_retire_time) == 0x128c) ? 1 : -1];
typedef char network_configuration_qos_probes_offset[(offsetof(s_network_configuration, transport_qos_probe_count) == 0x12d8) ? 1 : -1];
typedef char network_configuration_qos_bandwidth_offset[(offsetof(s_network_configuration, transport_qos_bandwidth) == 0x12e0) ? 1 : -1];
typedef char network_configuration_upstream_cap_offset[(offsetof(s_network_configuration, cap_live_service_upstream) == 0x12e8) ? 1 : -1];
typedef char network_configuration_upstream_multiplier_offset[(offsetof(s_network_configuration, upstream_cap_multiplier) == 0x12ec) ? 1 : -1];
typedef char network_configuration_upstream_divisor_offset[(offsetof(s_network_configuration, upstream_cap_divisor) == 0x12f0) ? 1 : -1];
typedef char network_configuration_simulation_join_offset[(offsetof(s_network_configuration, simulation_join_timeout) == 0x444) ? 1 : -1];
typedef char network_configuration_simulation_queue_offset[(offsetof(s_network_configuration, simulation_queue_allocated_bytes_limit) == 0x46c) ? 1 : -1];
typedef char network_configuration_friendly_assist_amount_offset[
    (offsetof(s_network_configuration, friendly_assist_amount) == 0x1320) ? 1 : -1];
typedef char network_configuration_size[(sizeof(s_network_configuration) == 0x1648) ? 1 : -1];
typedef char network_configuration_bandwidth_size[(sizeof(s_bandwidth_configuration) == 0x13c) ? 1 : -1];
typedef char network_configuration_bandwidth_offset[(offsetof(s_network_configuration, bandwidth) == 0x0c) ? 1 : -1];
typedef char network_configuration_flip_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_vehicle_flip * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x608) ? 1 : -1];
typedef char network_configuration_device_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_device_touch * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x630) ? 1 : -1];
typedef char network_configuration_enter_vehicle_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_unit_enter_vehicle * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x680) ? 1 : -1];
typedef char network_configuration_pickup_equipment_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_unit_equipment_pickup * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x6d0) ? 1 : -1];
typedef char network_configuration_capacity_offset[(offsetof(s_network_configuration, player_capacity_bandwidth_thresholds) == 0x1d0) ? 1 : -1];
typedef char network_configuration_join_offset[(offsetof(s_network_configuration, squad_join_timeout) == 0x240) ? 1 : -1];
typedef char network_configuration_qos_scale_offset[(offsetof(s_network_configuration, qos_quality_scale) == 0x3d0) ? 1 : -1];
typedef char network_configuration_controller_idle_timeout_offset[(offsetof(s_network_configuration, controller_idle_timeout_seconds) == 0x3d4) ? 1 : -1];
typedef char network_configuration_peer_abort_offset[(offsetof(s_network_configuration, peer_join_abort_check_interval) == 0xe08) ? 1 : -1];
typedef char network_configuration_timesync_offset[(offsetof(s_network_configuration, time_synchronization_interval_a) == 0xe44) ? 1 : -1];
typedef char network_configuration_anticheating_offset[(offsetof(s_network_configuration, observer.disable_bad_client_anticheating) == 0x10c0) ? 1 : -1];
typedef char network_configuration_player_bandwidth_offset[(offsetof(s_network_configuration, observer.bandwidth_per_player) == 0x1018) ? 1 : -1];
typedef char network_configuration_entity_size[(sizeof(s_simulation_entity_configuration) == 0x4c) ? 1 : -1];
typedef char network_configuration_leaderboard_retry_offset[(offsetof(s_network_configuration, leaderboard_query_failure_count) == 0x374) ? 1 : -1];
typedef char network_configuration_leaderboard_refresh_offset[(offsetof(s_network_configuration, leaderboard_refresh_msec) == 0x380) ? 1 : -1];
typedef char network_configuration_voice_offset[(offsetof(s_network_configuration, minimum_voice_repeater_downstream_bandwidth) == 0x22c) ? 1 : -1];
typedef char network_configuration_desirability_offset[(offsetof(s_network_configuration, matchmaking_desirability) == 0x2cc) ? 1 : -1];
typedef char network_configuration_latency_offset[(offsetof(s_network_configuration, maximum_latency) == 0x354) ? 1 : -1];
typedef char network_configuration_player_count_offset[(offsetof(s_network_configuration, maximum_player_count) == 0x3b8) ? 1 : -1];
typedef char network_configuration_ranges_offset[(offsetof(s_network_configuration, ranges) == 0x410) ? 1 : -1];
typedef char network_configuration_actor_timeout_offset[(offsetof(s_network_configuration, simulation_actor_control_timeout) == 0x438) ? 1 : -1];
typedef char network_configuration_records_offset[(offsetof(s_network_configuration, records) == 0x71c) ? 1 : -1];
typedef char network_configuration_supercombine_offset[(offsetof(s_network_configuration, supercombine_minimum_time) == 0xdb4) ? 1 : -1];
typedef char network_configuration_replication_limit_offset[(offsetof(s_network_configuration, replication_request_limit) == 0xdec) ? 1 : -1];
typedef char network_configuration_theater_offset[(offsetof(s_network_configuration, maximum_theater_player_count) == 0x1330) ? 1 : -1];
typedef char network_configuration_alpha_offset[(offsetof(s_network_configuration, alpha_locked_down) == 0x1520) ? 1 : -1];
typedef char network_configuration_join_host_offset[(offsetof(s_network_configuration, join_host_timeout) == 0x238) ? 1 : -1];
typedef char network_configuration_user_file_offset[(offsetof(s_network_configuration, user_file_timeout_msec) == 0x3c8) ? 1 : -1];
typedef char network_configuration_position_relevance_offset[(offsetof(s_network_configuration, position_relevance) == 0x3fc) ? 1 : -1];
typedef char network_configuration_managed_session_retry_offset[(offsetof(s_network_configuration, managed_session_retry_interval) == 0xdf0) ? 1 : -1];
typedef char network_configuration_join_queue_offset[(offsetof(s_network_configuration, join_queue_timeout) == 0xe00) ? 1 : -1];
typedef char network_configuration_veto_countdown_offset[(offsetof(s_network_configuration, matchmaking_veto_countdown) == 0x280) ? 1 : -1];
typedef char network_configuration_loading_countdown_offset[(offsetof(s_network_configuration, matchmaking_loading_countdown) == 0x290) ? 1 : -1];

typedef char network_configuration_weapon_fire_delay_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_weapon_fire * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x57c) ? 1 : -1];
typedef char network_configuration_pickup_weapon_delay_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_weapon_pickup * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x5cc) ? 1 : -1];
typedef char network_configuration_drop_weapon_delay_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_weapon_drop * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x5f4) ? 1 : -1];
typedef char network_configuration_grenade_release_delay_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_unit_grenade_release * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x644) ? 1 : -1];
typedef char network_configuration_melee_damage_delay_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_unit_melee_damage * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x658) ? 1 : -1];
typedef char network_configuration_melee_clang_delay_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_unit_melee_clang * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x66c) ? 1 : -1];
typedef char network_configuration_player_respawn_delay_offset[(offsetof(s_network_configuration, event_relevance)
    + _simulation_event_type_player_respawn_request * sizeof(s_simulation_event_configuration)
    + offsetof(s_simulation_event_configuration, delay_msec) == 0x6a8) ? 1 : -1];

typedef char network_configuration_skill_thresholds_offset[(offsetof(s_network_configuration, skill_level_experience_thresholds) == 0x133c) ? 1 : -1];
typedef char network_configuration_rank_grades_offset[(offsetof(s_network_configuration, rank_grade_thresholds) == 0x1404) ? 1 : -1];
typedef char network_configuration_rank_thresholds_offset[(offsetof(s_network_configuration, rank_experience_thresholds) == 0x14e4) ? 1 : -1];
typedef char network_configuration_map_table_offset[(offsetof(s_network_configuration, map_table) == 0x1544) ? 1 : -1];

typedef char network_configuration_control_growth_offset[(offsetof(s_network_configuration, control_growth_base) == 0x3e8) ? 1 : -1];
typedef char network_configuration_control_recover_offset[(offsetof(s_network_configuration, control_recover_time) == 0x3f8) ? 1 : -1];

typedef char network_configuration_host_minimum_offset[(offsetof(s_network_configuration, matchmaking_host_minimum_machine_capacity) == 0x214) ? 1 : -1];
typedef char network_configuration_host_good_upstream_offset[(offsetof(s_network_configuration, matchmaking_good_host_upstream_bandwidth) == 0x228) ? 1 : -1];
typedef char network_configuration_host_nat_offset[(offsetof(s_network_configuration, matchmaking_disallow_restricted_nat_hosts) == 0x258) ? 1 : -1];

typedef char network_configuration_observer_integer_offset[(offsetof(s_network_configuration, observer.packet_window_minimum_bytes) == 0xf60) ? 1 : -1];
typedef char network_configuration_synchronous_client_block_duration_msec_offset[
    (offsetof(s_network_configuration, synchronous_client_block_duration_msec) == 0x434) ? 1 : -1];
typedef char network_configuration_projectile_supercombine_minimum_fraction_offset[
    (offsetof(s_network_configuration, projectile_supercombine_minimum_fraction) == 0x43c) ? 1 : -1];
typedef char network_configuration_maximum_catchup_views_offset[(offsetof(s_network_configuration, maximum_catchup_views) == 0x440) ? 1 : -1];
typedef char network_configuration_simulation_join_total_timeout_offset[
    (offsetof(s_network_configuration, simulation_join_total_timeout) == 0x450) ? 1 : -1];
typedef char network_configuration_simulation_join_active_fraction_offset[
    (offsetof(s_network_configuration, simulation_join_active_fraction) == 0x454) ? 1 : -1];
typedef char network_configuration_observer_undesired_connection_drained_timeout_offset[
    (offsetof(s_network_configuration, observer.undesired_connection_drained_timeout) == 0xf08) ? 1 : -1];
typedef char network_configuration_observer_undesired_connection_timeout_offset[
    (offsetof(s_network_configuration, observer.undesired_connection_timeout) == 0xf0c) ? 1 : -1];
typedef char network_configuration_observer_simulation_packet_rate_offset[
    (offsetof(s_network_configuration, observer.simulation_packet_rate) == 0xf70) ? 1 : -1];
typedef char network_configuration_voice_receive_fast_acknowledge_time_offset[
    (offsetof(s_network_configuration, observer.voice_receive_fast_acknowledge_time) == 0xf74) ? 1 : -1];
typedef char network_configuration_flood_max_aperture_msec_offset[(offsetof(s_network_configuration, observer.flood_max_aperture_msec) == 0xf80) ? 1 : -1];
typedef char network_configuration_flood_packet_fraction_offset[(offsetof(s_network_configuration, observer.flood_packet_fraction) == 0xf84) ? 1 : -1];
typedef char network_configuration_bandwidth_adjust_interval_offset[(offsetof(s_network_configuration, observer.bandwidth_adjust_interval) == 0xf98) ? 1 : -1];
typedef char network_configuration_bandwidth_flood_channel_fraction_offset[
    (offsetof(s_network_configuration, observer.bandwidth_flood_channel_fraction) == 0xf9c) ? 1 : -1];
typedef char network_configuration_bandwidth_flood_check_threshold_offset[
    (offsetof(s_network_configuration, observer.bandwidth_flood_check_threshold) == 0xfa0) ? 1 : -1];
typedef char network_configuration_bandwidth_flood_backoff_kbps_offset[
    (offsetof(s_network_configuration, observer.bandwidth_flood_backoff_kbps) == 0xfa4) ? 1 : -1];
typedef char network_configuration_bandwidth_flood_backoff_repeat_interval_offset[
    (offsetof(s_network_configuration, observer.bandwidth_flood_backoff_repeat_interval) == 0xfa8) ? 1 : -1];
typedef char network_configuration_bandwidth_clear_check_threshold_offset[
    (offsetof(s_network_configuration, observer.bandwidth_clear_check_threshold) == 0xfac) ? 1 : -1];
typedef char network_configuration_bandwidth_check_threshold_maximum_offset[
    (offsetof(s_network_configuration, observer.bandwidth_check_threshold_maximum) == 0xfb0) ? 1 : -1];
typedef char network_configuration_bandwidth_increment_known_bad_threshold_kbps_offset[
    (offsetof(s_network_configuration, observer.bandwidth_increment_known_bad_threshold_kbps) == 0xfbc) ? 1 : -1];
typedef char network_configuration_bandwidth_increment_kbps_offset[(offsetof(s_network_configuration, observer.bandwidth_increment_kbps) == 0xfc0) ? 1 : -1];
typedef char network_configuration_observer_stream_expansion_interval_msec_offset[
    (offsetof(s_network_configuration, observer.observer_stream_expansion_interval_msec) == 0xfc8) ? 1 : -1];
typedef char network_configuration_stream_initial_startup_msec_offset[
    (offsetof(s_network_configuration, observer.stream_initial_startup_msec) == 0xfd4) ? 1 : -1];
typedef char network_configuration_stream_rtt_noise_msec_offset[(offsetof(s_network_configuration, observer.stream_rtt_noise_msec) == 0x101c) ? 1 : -1];
typedef char network_configuration_stream_minimum_rtt_msec_offset[(offsetof(s_network_configuration, observer.stream_minimum_rtt_msec) == 0x1020) ? 1 : -1];
typedef char network_configuration_stream_loss_window_size_offset[(offsetof(s_network_configuration, observer.stream_loss_window_size) == 0x1028) ? 1 : -1];
typedef char network_configuration_stream_loss_throttle_fraction_offset[
    (offsetof(s_network_configuration, observer.stream_loss_throttle_fraction) == 0x102c) ? 1 : -1];
typedef char network_configuration_stream_throttle_rtt_multiplier_offset[
    (offsetof(s_network_configuration, observer.stream_throttle_rtt_multiplier) == 0x1030) ? 1 : -1];
typedef char network_configuration_stream_throttle_reduce_multiplier_offset[
    (offsetof(s_network_configuration, observer.stream_throttle_reduce_multiplier) == 0x1034) ? 1 : -1];
typedef char network_configuration_stream_throttle_cookie_event_count_offset[
    (offsetof(s_network_configuration, observer.stream_throttle_cookie_event_count) == 0x1038) ? 1 : -1];
typedef char network_configuration_stream_throttle_cookie_backoff_threshold_offset[
    (offsetof(s_network_configuration, observer.stream_throttle_cookie_backoff_threshold) == 0x103c) ? 1 : -1];
typedef char network_configuration_stream_bandwidth_step_bps_offset[(offsetof(s_network_configuration, observer.stream_bandwidth_step_bps) == 0x1040) ? 1 : -1];
typedef char network_configuration_stream_bandwidth_step_max_fraction_offset[
    (offsetof(s_network_configuration, observer.stream_bandwidth_step_max_fraction) == 0x1044) ? 1 : -1];
typedef char network_configuration_stream_bandwidth_backoff_bps_offset[
    (offsetof(s_network_configuration, observer.stream_bandwidth_backoff_bps) == 0x1048) ? 1 : -1];
typedef char network_configuration_stream_bandwidth_backoff_max_fraction_offset[
    (offsetof(s_network_configuration, observer.stream_bandwidth_backoff_max_fraction) == 0x104c) ? 1 : -1];
typedef char network_configuration_stream_growth_period_minimum_msec_offset[
    (offsetof(s_network_configuration, observer.stream_growth_period_minimum_msec) == 0x1054) ? 1 : -1];
typedef char network_configuration_stream_throughput_recording_period_minimum_msec_offset[
    (offsetof(s_network_configuration, observer.stream_throughput_recording_period_minimum_msec) == 0x1058) ? 1 : -1];
typedef char network_configuration_stream_congestion_offender_bandwidth_increment_bps_offset[
    (offsetof(s_network_configuration, observer.stream_congestion_offender_bandwidth_increment_bps) == 0x106c) ? 1 : -1];
typedef char network_configuration_stream_congestion_offender_timeout_msec_offset[
    (offsetof(s_network_configuration, observer.stream_congestion_offender_timeout_msec) == 0x1070) ? 1 : -1];
typedef char network_configuration_stream_growth_maximum_count_offset[
    (offsetof(s_network_configuration, observer.stream_growth_maximum_count) == 0x1074) ? 1 : -1];
typedef char network_configuration_stream_growth_maximum_fraction_offset[
    (offsetof(s_network_configuration, observer.stream_growth_maximum_fraction) == 0x1078) ? 1 : -1];
typedef char network_configuration_stream_growth_desire_maximum_offset[
    (offsetof(s_network_configuration, observer.stream_growth_desire_maximum) == 0x107c) ? 1 : -1];
typedef char network_configuration_stream_growth_desire_delay_multiplier_offset[
    (offsetof(s_network_configuration, observer.stream_growth_desire_delay_multiplier) == 0x1080) ? 1 : -1];
typedef char network_configuration_stream_growth_desire_penalty_bandwidth_multiplier_offset[
    (offsetof(s_network_configuration, observer.stream_growth_desire_penalty_bandwidth_multiplier) == 0x1084) ? 1 : -1];
typedef char network_configuration_stream_growth_backoff_rtt_event_threshold_offset[
    (offsetof(s_network_configuration, observer.stream_growth_backoff_rtt_event_threshold) == 0x1094) ? 1 : -1];
typedef char network_configuration_stream_growth_incremental_rtt_event_threshold_offset[
    (offsetof(s_network_configuration, observer.stream_growth_incremental_rtt_event_threshold) == 0x1098) ? 1 : -1];
typedef char network_configuration_stream_growth_incremental_rtt_increase_msec_offset[
    (offsetof(s_network_configuration, observer.stream_growth_incremental_rtt_increase_msec) == 0x109c) ? 1 : -1];
typedef char network_configuration_stream_cautious_expansion_limited_periods_count_offset[
    (offsetof(s_network_configuration, observer.stream_cautious_expansion_limited_periods_count) == 0x10a0) ? 1 : -1];
typedef char network_configuration_stream_cautious_expansion_instability_timeout_msec_offset[
    (offsetof(s_network_configuration, observer.stream_cautious_expansion_instability_timeout_msec) == 0x10a4) ? 1 : -1];
typedef char network_configuration_bandwidth_monitor_constriction_threshold_bps_offset[
    (offsetof(s_network_configuration, observer.bandwidth_monitor_constriction_threshold_bps) == 0x10b4) ? 1 : -1];
typedef char network_configuration_observer_time_offset[(offsetof(s_network_configuration, observer.time) == 0x10b8) ? 1 : -1];
typedef char network_configuration_bad_bandwidth_throughput_threshold_offset[
    (offsetof(s_network_configuration, observer.bad_bandwidth_throughput_threshold) == 0x10bc) ? 1 : -1];
typedef char network_configuration_observer_disable_disconnection_badness_offset[
    (offsetof(s_network_configuration, observer.disable_disconnection_badness) == 0x10c1) ? 1 : -1];
typedef char network_configuration_observer_ignore_badness_offset[(offsetof(s_network_configuration, observer.ignore_badness) == 0x10c2) ? 1 : -1];
typedef char network_configuration_stream_wants_more_allocation_fudge_factor_offset[
    (offsetof(s_network_configuration, observer.stream_wants_more_allocation_fudge_factor) == 0x10d8) ? 1 : -1];
typedef char network_configuration_stream_wants_more_allocation_fudge_factor_small_offset[
    (offsetof(s_network_configuration, observer.stream_wants_more_allocation_fudge_factor_small) == 0x10dc) ? 1 : -1];
typedef char network_configuration_stream_maximum_instability_value_offset[
    (offsetof(s_network_configuration, observer.stream_maximum_instability_value) == 0x10e0) ? 1 : -1];
typedef char network_configuration_qos_to_live_interval_msec_offset[(offsetof(s_network_configuration, observer.qos_to_live_interval_msec) == 0x1160) ? 1 : -1];
typedef char network_configuration_request_cache_clear_interval_offset[(offsetof(s_network_configuration, request_cache_clear_interval) == 0x0) ? 1 : -1];
typedef char network_configuration_required_file_invalidation_check_interval_msec_offset[
    (offsetof(s_network_configuration, required_file_invalidation_check_interval_msec) == 0x4) ? 1 : -1];
typedef char network_configuration_required_file_download_retry_interval_msec_offset[
    (offsetof(s_network_configuration, required_file_download_retry_interval_msec) == 0x8) ? 1 : -1];
typedef char network_configuration_trim_fraction_offset[(offsetof(s_network_configuration, bandwidth.trim_fraction) == 0xc) ? 1 : -1];
typedef char network_configuration_minimum_measurements_offset[(offsetof(s_network_configuration, bandwidth.minimum_measurements) == 0x10) ? 1 : -1];
typedef char network_configuration_safety_margin_offset[(offsetof(s_network_configuration, bandwidth.safety_margin) == 0x14) ? 1 : -1];
typedef char network_configuration_minimum_bandwidth_offset[(offsetof(s_network_configuration, bandwidth.minimum_bandwidth) == 0x18) ? 1 : -1];
typedef char network_configuration_interval_offset[(offsetof(s_network_configuration, bandwidth.interval) == 0x1c) ? 1 : -1];
typedef char network_configuration_ratio_offset[(offsetof(s_network_configuration, bandwidth.ratio) == 0x20) ? 1 : -1];
typedef char network_configuration_dispute_threshold_offset[(offsetof(s_network_configuration, bandwidth.dispute_threshold) == 0x24) ? 1 : -1];
typedef char network_configuration_dispute_margin_offset[(offsetof(s_network_configuration, bandwidth.dispute_margin) == 0x28) ? 1 : -1];
typedef char network_configuration_dispute_increment_offset[(offsetof(s_network_configuration, bandwidth.dispute_increment) == 0x2c) ? 1 : -1];
typedef char network_configuration_host_speculative_migration_required_lobby_peer_connectivity_difference_offset[
    (offsetof(s_network_configuration, bandwidth.host_speculative_migration_required_lobby_peer_connectivity_difference) == 0x40) ? 1 : -1];
typedef char network_configuration_host_preference_connectivity_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, connectivity) == 0x10c) ? 1 : -1];
typedef char network_configuration_host_preference_local_host_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, local_host) == 0x110) ? 1 : -1];
typedef char network_configuration_host_preference_peer_capacity_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, peer_capacity) == 0x114) ? 1 : -1];
typedef char network_configuration_host_preference_machine_capacity_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, machine_capacity) == 0x118) ? 1 : -1];
typedef char network_configuration_host_preference_player_capacity_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, player_capacity) == 0x11c) ? 1 : -1];
typedef char network_configuration_host_preference_latency_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, latency) == 0x120) ? 1 : -1];
typedef char network_configuration_host_preference_gamestate_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, gamestate) == 0x124) ? 1 : -1];
typedef char network_configuration_host_preference_open_nat_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, open_nat) == 0x128) ? 1 : -1];
typedef char network_configuration_host_preference_flagged_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, flagged) == 0x12c) ? 1 : -1];
typedef char network_configuration_host_preference_player_count_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, player_count) == 0x130) ? 1 : -1];
typedef char network_configuration_host_preference_quality_tier_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, quality_tier) == 0x134) ? 1 : -1];
typedef char network_configuration_host_preference_latency_tier_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, latency_tier) == 0x138) ? 1 : -1];
typedef char network_configuration_host_preference_bandwidth_capacity_offset[(offsetof(s_network_configuration, bandwidth)
    + offsetof(s_bandwidth_configuration, host_preference_weights)
    + 3 * sizeof(s_bandwidth_host_preference_weights)
    + offsetof(s_bandwidth_host_preference_weights, bandwidth_capacity) == 0x13c) ? 1 : -1];
typedef char network_configuration_minimum_bandwidth_bps_offset[(offsetof(s_network_configuration, minimum_bandwidth_bps) == 0x234) ? 1 : -1];
typedef char network_configuration_request_migration_start_rety_interval_msec_offset[
    (offsetof(s_network_configuration, request_migration_start_rety_interval_msec) == 0x244) ? 1 : -1];
typedef char network_configuration_request_migration_abort_rety_interval_msec_offset[
    (offsetof(s_network_configuration, request_migration_abort_rety_interval_msec) == 0x248) ? 1 : -1];
typedef char network_configuration_joining_search_qos_bps_offset[(offsetof(s_network_configuration, joining_search_qos_bps) == 0x24c) ? 1 : -1];
typedef char network_configuration_desperation_wait_time_seconds_offset[(offsetof(s_network_configuration, desperation_wait_time_seconds) == 0x254) ? 1 : -1];
typedef char network_configuration_matchmaking_strict_nat_host_percentage_offset[
    (offsetof(s_network_configuration, matchmaking_strict_nat_host_percentage) == 0x25c) ? 1 : -1];
typedef char network_configuration_squad_start_failure_give_up_time_msec_offset[
    (offsetof(s_network_configuration, squad_start_failure_give_up_time_msec) == 0x260) ? 1 : -1];
typedef char network_configuration_prepare_map_display_map_during_loading_offset[
    (offsetof(s_network_configuration, prepare_map_display_map_during_loading) == 0x27c) ? 1 : -1];
typedef char network_configuration_matchmaking_arbitration_registration_timeout_seconds_offset[
    (offsetof(s_network_configuration, matchmaking_arbitration_registration_timeout_seconds) == 0x298) ? 1 : -1];
typedef char network_configuration_matchmaking_arbitration_completion_timeout_seconds_offset[
    (offsetof(s_network_configuration, matchmaking_arbitration_completion_timeout_seconds) == 0x29c) ? 1 : -1];
typedef char network_configuration_return_to_pre_game_offset[(offsetof(s_network_configuration, return_to_pre_game) == 0x2a0) ? 1 : -1];
typedef char network_configuration_post_match_leaderboard_refresh_countdown_offset[
    (offsetof(s_network_configuration, post_match_leaderboard_refresh_countdown) == 0x2a4) ? 1 : -1];
typedef char network_configuration_nat_warning_interval_seconds_offset[(offsetof(s_network_configuration, nat_warning_interval_seconds) == 0x2a8) ? 1 : -1];
typedef char network_configuration_simulation_abort_host_timeout_offset[(offsetof(s_network_configuration, simulation_abort_host_timeout) == 0x2ac) ? 1 : -1];
typedef char network_configuration_simulation_abort_peer_timeout_offset[(offsetof(s_network_configuration, simulation_abort_peer_timeout) == 0x2b0) ? 1 : -1];
typedef char network_configuration_search_start_delay_offset[(offsetof(s_network_configuration, search_start_delay) == 0x328) ? 1 : -1];
typedef char network_configuration_search_stage_timeout_offset[(offsetof(s_network_configuration, search_stage_timeout) == 0x32c) ? 1 : -1];
typedef char network_configuration_search_final_delay_offset[(offsetof(s_network_configuration, search_final_delay) == 0x330) ? 1 : -1];
typedef char network_configuration_any_maximum_latency_offset[(offsetof(s_network_configuration, any_maximum_latency) == 0x364) ? 1 : -1];
typedef char network_configuration_any_mu_delta_offset[(offsetof(s_network_configuration, any_mu_delta) == 0x368) ? 1 : -1];
typedef char network_configuration_any_skill_level_delta_offset[(offsetof(s_network_configuration, any_skill_level_delta) == 0x36c) ? 1 : -1];
typedef char network_configuration_search_stage_any_final_desired_skill_range_offset[
    (offsetof(s_network_configuration, search_stage_any_final_desired_skill_range) == 0x370) ? 1 : -1];
typedef char network_configuration_post_match_recover_time_offset[(offsetof(s_network_configuration, post_match_recover_time) == 0x398) ? 1 : -1];
typedef char network_configuration_post_match_settle_time_offset[(offsetof(s_network_configuration, post_match_settle_time) == 0x39c) ? 1 : -1];
typedef char network_configuration_group_in_game_qos_reply_bandwidth_bps_offset[
    (offsetof(s_network_configuration, group_in_game_qos_reply_bandwidth_bps) == 0x3b4) ? 1 : -1];
typedef char network_configuration_machine_file_timeout_seconds_offset[(offsetof(s_network_configuration, machine_file_timeout_seconds) == 0x3c0) ? 1 : -1];
typedef char network_configuration_machine_file_refresh_seconds_offset[(offsetof(s_network_configuration, machine_file_refresh_seconds) == 0x3c4) ? 1 : -1];
typedef char network_configuration_peer_join_secure_connection_timeout_offset[
    (offsetof(s_network_configuration, peer_join_secure_connection_timeout) == 0xdf8) ? 1 : -1];
typedef char network_configuration_peer_join_initial_update_timeout_offset[
    (offsetof(s_network_configuration, peer_join_initial_update_timeout) == 0xdfc) ? 1 : -1];
typedef char network_configuration_host_rejoin_accept_timeout_msec_offset[
    (offsetof(s_network_configuration, host_rejoin_accept_timeout_msec) == 0xe10) ? 1 : -1];
typedef char network_configuration_host_handoff_acknowledge_timeout_offset[
    (offsetof(s_network_configuration, host_handoff_acknowledge_timeout) == 0xe1c) ? 1 : -1];
typedef char network_configuration_host_handoff_stabilize_timeout_offset[(offsetof(s_network_configuration, host_handoff_stabilize_timeout) == 0xe20) ? 1 : -1];
typedef char network_configuration_host_handoff_timeout_offset[(offsetof(s_network_configuration, host_handoff_timeout) == 0xe24) ? 1 : -1];
typedef char network_configuration_host_handoff_transition_timeout_offset[
    (offsetof(s_network_configuration, host_handoff_transition_timeout) == 0xe28) ? 1 : -1];
typedef char network_configuration_host_reestablish_acknowledge_timeout_offset[
    (offsetof(s_network_configuration, host_reestablish_acknowledge_timeout) == 0xe2c) ? 1 : -1];
typedef char network_configuration_election_timeout_offset[(offsetof(s_network_configuration, election_timeout) == 0xe34) ? 1 : -1];
typedef char network_configuration_election_peer_timeout_offset[(offsetof(s_network_configuration, election_peer_timeout) == 0xe38) ? 1 : -1];
typedef char network_configuration_election_ignore_dissension_msec_offset[
    (offsetof(s_network_configuration, election_ignore_dissension_msec) == 0xe3c) ? 1 : -1];
typedef char network_configuration_election_message_interval_offset[(offsetof(s_network_configuration, election_message_interval) == 0xe40) ? 1 : -1];
typedef char network_configuration_minimum_election_send_interval_msec_offset[
    (offsetof(s_network_configuration, minimum_election_send_interval_msec) == 0xe4c) ? 1 : -1];
typedef char network_configuration_channel_connect_packet_interval_offset[
    (offsetof(s_network_configuration, channel_configuration.connect_packet_interval) == 0x126c) ? 1 : -1];
typedef char network_configuration_channel_connect_packet_count_offset[
    (offsetof(s_network_configuration, channel_configuration.connect_packet_count) == 0x1270) ? 1 : -1];
typedef char network_configuration_channel_connect_timeout_offset[
    (offsetof(s_network_configuration, channel_configuration.connect_timeout) == 0x1274) ? 1 : -1];
typedef char network_configuration_channel_establishment_timeout_offset[
    (offsetof(s_network_configuration, channel_configuration.establishment_timeout) == 0x1278) ? 1 : -1];
typedef char network_configuration_packet_statistics_interval_offset[(offsetof(s_network_configuration, packet_statistics_interval) == 0x127c) ? 1 : -1];
typedef char network_configuration_latency_average_gain_bits_offset[(offsetof(s_network_configuration, latency_average_gain_bits) == 0x1290) ? 1 : -1];
typedef char network_configuration_latency_deviation_shift_offset[(offsetof(s_network_configuration, latency_deviation_shift) == 0x1294) ? 1 : -1];
typedef char network_configuration_stream_timeout_deviation_scale_offset[
    (offsetof(s_network_configuration, stream_timeout_deviation_scale) == 0x1298) ? 1 : -1];
typedef char network_configuration_stream_timeout_minimum_margin_offset[(offsetof(s_network_configuration, stream_timeout_minimum_margin) == 0x129c) ? 1 : -1];
typedef char network_configuration_stream_timeout_minimum_offset[(offsetof(s_network_configuration, stream_timeout_minimum) == 0x12a0) ? 1 : -1];
typedef char network_configuration_timeout_deviation_scale_offset[(offsetof(s_network_configuration, timeout_deviation_scale) == 0x12a4) ? 1 : -1];
typedef char network_configuration_timeout_minimum_margin_offset[(offsetof(s_network_configuration, timeout_minimum_margin) == 0x12a8) ? 1 : -1];
typedef char network_configuration_timeout_minimum_offset[(offsetof(s_network_configuration, timeout_minimum) == 0x12ac) ? 1 : -1];
typedef char network_configuration_initial_latency_minimum_msec_offset[(offsetof(s_network_configuration, initial_latency_minimum_msec) == 0x12b0) ? 1 : -1];
typedef char network_configuration_initial_latency_average_msec_offset[(offsetof(s_network_configuration, initial_latency_average_msec) == 0x12b4) ? 1 : -1];
typedef char network_configuration_initial_latency_deviation_offset[(offsetof(s_network_configuration, initial_latency_deviation) == 0x12b8) ? 1 : -1];
typedef char network_configuration_initial_timeout_msec_offset[(offsetof(s_network_configuration, initial_timeout_msec) == 0x12bc) ? 1 : -1];
typedef char network_configuration_backoff_increment_msec_offset[(offsetof(s_network_configuration, backoff_increment_msec) == 0x12c0) ? 1 : -1];
typedef char network_configuration_backoff_max_msec_offset[(offsetof(s_network_configuration, backoff_max_msec) == 0x12c4) ? 1 : -1];
typedef char network_configuration_backoff_decrement_msec_offset[(offsetof(s_network_configuration, backoff_decrement_msec) == 0x12c8) ? 1 : -1];
typedef char network_configuration_bandwidth_warning_latency_multiplier_offset[
    (offsetof(s_network_configuration, bandwidth_warning_latency_multiplier) == 0x12cc) ? 1 : -1];
typedef char network_configuration_bandwidth_warning_latency_minimum_increase_offset[
    (offsetof(s_network_configuration, bandwidth_warning_latency_minimum_increase) == 0x12d0) ? 1 : -1];
typedef char network_configuration_address_resolution_timeout_msec_offset[
    (offsetof(s_network_configuration, address_resolution_timeout_msec) == 0x12d4) ? 1 : -1];
typedef char network_configuration_ui_upload_record_threshold_offset[(offsetof(s_network_configuration, ui_upload_record_threshold) == 0x1304) ? 1 : -1];
typedef char network_configuration_ui_upload_time_threshold_offset[(offsetof(s_network_configuration, ui_upload_time_threshold) == 0x1308) ? 1 : -1];
typedef char network_configuration_record_uploads_prevent_game_from_starting_offset[
    (offsetof(s_network_configuration, record_uploads_prevent_game_from_starting) == 0x130c) ? 1 : -1];
typedef char network_configuration_ejection_cutoff_offset[(offsetof(s_network_configuration, ejection_cutoff) == 0x131c) ? 1 : -1];
typedef char network_configuration_network_heap_large_size_offset[(offsetof(s_network_configuration, network_heap_large_size) == 0x1324) ? 1 : -1];
typedef char network_configuration_network_heap_small_size_offset[(offsetof(s_network_configuration, network_heap_small_size) == 0x1328) ? 1 : -1];
typedef char network_configuration_allow_matchmaking_abort_interval_msec_offset[
    (offsetof(s_network_configuration, allow_matchmaking_abort_interval_msec) == 0x132c) ? 1 : -1];
typedef char network_configuration_disable_game_offset[(offsetof(s_network_configuration, disable_game) == 0x151c) ? 1 : -1];
typedef char network_configuration_custom_games_disabled_offset[(offsetof(s_network_configuration, custom_games_disabled) == 0x151d) ? 1 : -1];
typedef char network_configuration_disallow_cross_language_coop_offset[(offsetof(s_network_configuration, disallow_cross_language_coop) == 0x1646) ? 1 : -1];

#endif
