#ifndef NETWORKING_REPLICATION_REPLICATION_ENTITY_VIEW_STATE_TYPES_H
#define NETWORKING_REPLICATION_REPLICATION_ENTITY_VIEW_STATE_TYPES_H

enum e_replication_entity_view_state
{
    _replication_entity_view_state_none = 0,
    _replication_entity_view_state_active = 1,
    _replication_entity_view_state_creating = 2,
    _replication_entity_view_state_replicated = 3,
    _replication_entity_view_state_pending_deletion = 4,
    _replication_entity_view_state_deleting = 5,
    k_replication_entity_view_state_count = 6
};

#endif // NETWORKING_REPLICATION_REPLICATION_ENTITY_VIEW_STATE_TYPES_H
