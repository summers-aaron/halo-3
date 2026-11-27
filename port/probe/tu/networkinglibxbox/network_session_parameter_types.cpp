// networkinglibxbox/network_session_parameter_types.obj: from the records 0x3f5958
// @flags /TP /O2 /Oc

#include "networking/logic/storage/network_storage_files.h"
#include "networking/session/network_session_parameter_types.h"
#include "networking/session/network_session_parameters_game.h"
#include "networking/session/network_session_parameters_matchmaking.h"
#include "networking/session/network_session_parameters_session.h"
#include "networking/session/network_session_parameters_base.h"
#include "networking/session/network_session_parameter_type_collection.h"
#include "memory/bitstream.h"
static __forceinline void network_session_parameter_types_register_all(c_network_session_parameter_type_collection *collection)
{

    collection->register_session_parameter_type(_network_session_parameter_type_session_size, "session-size", session_size_encode_function,
        session_size_decode_function, 8, session_size_encode_function, session_size_decode_function, 8);
    collection->register_session_parameter_type(_network_session_parameter_type_session_mode, "session-mode", session_mode_encode_function,
        session_mode_decode_function, 12, session_mode_encode_function, session_mode_decode_function, 12);
    collection->register_session_parameter_type(_network_session_parameter_type_privacy_mode, "privacy-mode", privacy_mode_encode_function,
        privacy_mode_extended_decode_function, 16, privacy_mode_encode_function, privacy_mode_extended_decode_function, 16);
    collection->register_session_parameter_type(_network_session_parameter_type_ui_game_mode, "ui-game-mode",
        campaign_difficulty_encode_function, ui_game_mode_param_decode_function, 4, campaign_difficulty_encode_function,
        ui_game_mode_param_decode_function, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_map, "map", map_encode_function, map_decode_function, 256,
        map_encode_function, map_decode_function, 256);
    collection->register_session_parameter_type(_network_session_parameter_type_initial_participants, "initial-participants",
        initial_participants_encode_function, initial_participants_decode_function, 4848, initial_participants_encode_function,
        initial_participants_decode_function, 4848);
    collection->register_session_parameter_type(_network_session_parameter_type_game_instance, "game-instance",
        game_instance_encode_function, game_instance_decode_function, 8, game_instance_encode_function, game_instance_decode_function, 8);
    collection->register_session_parameter_type(_network_session_parameter_type_random_seed, "random-seed", random_seed_encode_function,
        random_seed_decode_function, 4, random_seed_encode_function, random_seed_decode_function, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_language, "language", language_encode_function,
        language_decode_function, 4, language_encode_function, language_decode_function, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_determinism_version, "determinism-version",
        determinism_version_encode_function, determinism_version_decode_function, 4, determinism_version_encode_function,
        determinism_version_decode_function, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_simulation_protocol, "simulation-protocol",
        simulation_protocol_encode_function, simulation_protocol_decode_function, 4, simulation_protocol_encode_function,
        simulation_protocol_decode_function, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_campaign_difficulty, "campaign-difficulty",
        campaign_difficulty_encode_function, campaign_difficulty_decode_function, 4, campaign_difficulty_encode_function,
        campaign_difficulty_decode_function, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_campaign_active_primary_skulls,
        "campaign-active-primary-skulls", random_seed_encode_function, random_seed_decode_function, 4, random_seed_encode_function,
        random_seed_decode_function, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_campaign_active_secondary_skulls,
        "campaign-active-secondary-skulls", random_seed_encode_function, random_seed_decode_function, 4, random_seed_encode_function,
        random_seed_decode_function, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_campaign_metagame_scoring, "campaign-metagame-scoring",
        campaign_metagame_scoring_encode_function, campaign_metagame_scoring_decode_function, 2, campaign_metagame_scoring_encode_function,
        campaign_metagame_scoring_decode_function, 2);
    collection->register_session_parameter_type(_network_session_parameter_type_campaign_insertion_point, "campaign-insertion-point",
        campaign_metagame_scoring_encode_function, campaign_metagame_scoring_decode_function, 2, campaign_metagame_scoring_encode_function,
        campaign_metagame_scoring_decode_function, 2);
    collection->register_session_parameter_type(_network_session_parameter_type_ui_state, "ui-state", end_game_encode_function,
        end_game_decode_function, 1, end_game_encode_function, end_game_decode_function, 1);
    collection->register_session_parameter_type(_network_session_parameter_type_end_game, "end-game", end_game_encode_function,
        end_game_decode_function, 1, end_game_encode_function, end_game_decode_function, 1);
    collection->register_session_parameter_type(_network_session_parameter_type_start_mode, "start-mode",
        simulation_protocol_encode_function, privacy_mode_decode_function, 4, simulation_protocol_encode_function,
        privacy_mode_decode_function, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_game_variant, "game-variant",
        network_session_encode_chunked, network_session_decode_chunked, 144, network_session_encode_chunked, network_session_decode_chunked,
        144);
    collection->register_session_parameter_type(_network_session_parameter_type_map_variant, "map-variant", network_session_encode_chunked,
        network_session_decode_chunked, 144, network_session_encode_chunked, network_session_decode_chunked, 144);
    collection->register_session_parameter_type(_network_session_parameter_type_saved_film, "saved-film", saved_film_encode,
        saved_film_decode, 536, saved_film_encode, saved_film_decode, 536);
    collection->register_session_parameter_type(_network_session_parameter_type_saved_film_game_options, "saved-film-game-options",
        network_session_encode_chunked, network_session_decode_chunked, 144, network_session_encode_chunked, network_session_decode_chunked,
        144);
    collection->register_session_parameter_type(_network_session_parameter_type_game_start_status, "game-start-status",
        game_start_status_encode_function, game_start_status_decode_function, 12, game_start_status_encode_function,
        game_start_status_decode_function, 12);
    collection->register_session_parameter_type(_network_session_parameter_type_countdown_timer, "countdown-timer",
        countdown_timer_encode_function, countdown_timer_decode_function, 24, countdown_timer_encode_function,
        countdown_timer_decode_function, 24);
    collection->register_session_parameter_type(_network_session_parameter_type_voice_repeater, "voice-repeater",
        voice_repeater_encode_function, voice_repeater_decode_function, 4, voice_repeater_encode_function, voice_repeater_decode_function,
        4);
    collection->register_session_parameter_type(_network_session_parameter_type_requested_remote_join_data, "requested-remote-join-data",
        remote_join_data_encode_function, remote_join_data_decode_function, 104, remote_join_data_encode_function,
        remote_join_data_decode_function, 104);
    collection->register_session_parameter_type(_network_session_parameter_type_remote_join_data, "remote-join-data",
        remote_join_data_encode_function, remote_join_data_decode_function, 104, remote_join_data_encode_function,
        remote_join_data_decode_function, 104);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_arbitration_nonce,
        "matchmaking-arbitration-nonce", game_instance_encode_function, game_instance_decode_function, 8, game_instance_encode_function,
        game_instance_decode_function, 8);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_hopper_list, "matchmaking-hopper-list",
        matchmaking_hopper_list_encode_function, matchmaking_hopper_list_decode_function, 1700, matchmaking_hopper_list_encode_function,
        matchmaking_hopper_list_decode_function, 1700);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_game_list, "matchmaking-game-list",
        matchmaking_game_list_encode_function, matchmaking_game_list_decode_function, 432, matchmaking_game_list_encode_function,
        matchmaking_game_list_decode_function, 432);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_requested_hopper,
        "matchmaking-requested-hopper", matchmaking_requested_hopper_encode_function, matchmaking_requested_hopper_decode_function, 2,
        matchmaking_requested_hopper_encode_function, matchmaking_requested_hopper_decode_function, 2);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_hopper, "matchmaking-hopper",
        matchmaking_hopper_encode_function, matchmaking_hopper_decode, 76, matchmaking_hopper_encode_function, matchmaking_hopper_decode,
        76);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_abort_requested, "matchmaking-abort",
        end_game_encode_function, end_game_decode_function, 1, end_game_encode_function, end_game_decode_function, 1);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_search_preference,
        "matchmaking-search-preference", simulation_protocol_encode_function, matchmaking_search_preference_decode, 4,
        simulation_protocol_encode_function, matchmaking_search_preference_decode, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_progress, "matchmaking-progress",
        matchmaking_progress_encode, matchmaking_progress_decode_function, 32, matchmaking_progress_encode,
        matchmaking_progress_decode_function, 32);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_progress_search_criteria,
        "matchmaking-progress-search-criteria", matchmaking_progress_search_criteria_encode, matchmaking_progress_search_criteria_decode,
        36, matchmaking_progress_search_criteria_encode, matchmaking_progress_search_criteria_decode, 36);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_assembly_peer_evict_mask,
        "matchmaking-assembly-peer-evict-mask", matchmaking_assembly_peer_evict_mask_encode, matchmaking_assembly_peer_evict_mask_decode, 4,
        matchmaking_assembly_peer_evict_mask_encode, matchmaking_assembly_peer_evict_mask_decode, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_rematch_data, "matchmaking-rematch-data",
        matchmaking_rematch_encode_function, matchmaking_rematch_decode_function, 1156, matchmaking_rematch_encode_function,
        matchmaking_rematch_decode_function, 1156);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_hopper_statistics, "matchmaking-statistics",
        matchmaking_hopper_statistics_encode_function, matchmaking_hopper_statistics_decode_function, 8,
        matchmaking_hopper_statistics_encode_function, matchmaking_hopper_statistics_decode_function, 8);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_tip, "matchmaking-tip",
        matchmaking_tip_encode_function, matchmaking_tip_decode_function, 4, matchmaking_tip_encode_function,
        matchmaking_tip_decode_function, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_suppress_progress,
        "matchmaking-suppress-progress", end_game_encode_function, end_game_decode_function, 1, end_game_encode_function,
        end_game_decode_function, 1);
    collection->register_session_parameter_type(_network_session_parameter_type_synchronous_out_of_sync, "synchronous-out-of-sync",
        synchronous_out_of_sync_encode_function, synchronous_out_of_sync_decode_function, 2, synchronous_out_of_sync_encode_function,
        synchronous_out_of_sync_decode_function, 2);
    collection->register_session_parameter_type(_network_session_parameter_type_matchmaking_messaging, "matchmaking-messaging",
        matchmaking_messaging_encode, matchmaking_messaging_decode, 12, matchmaking_messaging_encode, matchmaking_messaging_decode, 12);
    collection->register_session_parameter_type(_network_session_parameter_type_host_frame_quality, "host-frame-quality",
        simulation_protocol_encode_function, (t_decode_network_session_parameter)host_frame_quality_decode, 4,
        simulation_protocol_encode_function, (t_decode_network_session_parameter)host_frame_quality_decode, 4);
    collection->register_session_parameter_type(_network_session_parameter_type_request_campaign_quit, "request-campaign-quit",
        request_campaign_quit_encode_function, request_campaign_quit_decode_function, 8, request_campaign_quit_encode_function,
        request_campaign_quit_decode_function, 8);
    collection->register_session_parameter_type(_network_session_parameter_type_leader_properties, "leader-properties",
        leader_properties_encode_function, leader_properties_decode_function, 64, leader_properties_encode_function,
        leader_properties_decode_function, 64);
}

// @address 0x3f5958 ?network_session_parameter_types_register@@YAXPAVc_network_session_parameter_type_collection@@@Z
void network_session_parameter_types_register(c_network_session_parameter_type_collection *parameter_types)
{
    c_network_session_parameter_type_collection **home = &parameter_types;
    (void)home;
    network_session_parameter_types_register_all(parameter_types);
}
