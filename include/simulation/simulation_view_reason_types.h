#ifndef SIMULATION_SIMULATION_VIEW_REASON_TYPES_H
#define SIMULATION_SIMULATION_VIEW_REASON_TYPES_H

enum e_simulation_view_reason
{
    _simulation_view_reason_none = 0,
    _simulation_view_reason_channel_closed = 1,
    _simulation_view_reason_join_timeout = 3,
    _simulation_view_reason_synchronous_catchup_failed = 5,
    _simulation_view_reason_remote_establishment_lost = 6,
    _simulation_view_reason_invalid_establishment = 7,
    _simulation_view_reason_player_acknowledgement_failed = 8,
    _simulation_view_reason_entity_replication_error = 9,
    _simulation_view_reason_event_replication_error = 10,
    _simulation_view_reason_game_results_replication_error = 11,
    _simulation_view_reason_no_longer_authority = 12,
    _simulation_view_reason_map_digest_mismatch = 13,
};

#endif // SIMULATION_SIMULATION_VIEW_REASON_TYPES_H
