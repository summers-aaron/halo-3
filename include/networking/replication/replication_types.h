#ifndef NETWORKING_REPLICATION_REPLICATION_TYPES_H
#define NETWORKING_REPLICATION_REPLICATION_TYPES_H

#include <basetsd.h>
#include <stddef.h>

class c_replication_outgoing_event;

// A client supplies either a control/entity identifier or an outgoing event.
// The scheduler forwards this payload together with the client's request type.
// Pointer-sized: the event view passes an event pointer through this slot.
struct s_replication_outgoing_request
{
    long priority;
    long size_in_bits;
    union
    {
        LONG_PTR identifier;
        c_replication_outgoing_event *event;
    };
    short client_index;
    short request_type;
};

typedef char replication_outgoing_request_size[(sizeof(void *) != 4 || ((sizeof(s_replication_outgoing_request) == 0x10) )) ? 1 : -1];
typedef char replication_outgoing_request_client_offset[(sizeof(void *) != 4 || ((offsetof(s_replication_outgoing_request, client_index) == 0xc) )) ? 1 : -1];
typedef char replication_outgoing_request_type_offset[(sizeof(void *) != 4 || ((offsetof(s_replication_outgoing_request, request_type) == 0xe) )) ? 1 : -1];

// Blocks passed between the replication scheduler and its clients.
struct s_replication_allocation_block
{
    short block_size;
    short block_type;
    void *block_data;
};

typedef char replication_allocation_block_size[(sizeof(void *) != 4 || ((sizeof(s_replication_allocation_block) == 8) )) ? 1 : -1];
typedef char replication_allocation_block_data_offset[(sizeof(void *) != 4 || ((offsetof(s_replication_allocation_block, block_data) == 4) )) ? 1 : -1];

// Known s_replication_allocation_block::block_type values.
enum e_replication_entity_allocation_type
{
    _replication_entity_allocation_simulation_queue = 11,
    _replication_entity_allocation_gamestate_entity = 12
};

// Entity creation/update metadata carried in the incoming request's 16-byte payload.
struct s_replication_request_data
{
    long entity_count;
    long entity_index;
    long entity_type;
    unsigned long entity_update_mask;
};

// The scheduler reserves eight allocation blocks per request. Control requests use
// the first two; event requests carry their entity references in the payload union.
struct s_replication_incoming_request
{
    long request_type;
    long entity_index;
    union
    {
        s_replication_request_data request_data;
        long entity_references[2];
    };
    long block_count;
    s_replication_allocation_block blocks[8];
};

typedef char replication_request_data_size[(sizeof(s_replication_request_data) == 0x10) ? 1 : -1];
typedef char replication_incoming_request_size[(sizeof(void *) != 4 || ((sizeof(s_replication_incoming_request) == 0x5c) )) ? 1 : -1];
typedef char replication_incoming_request_payload_offset[(offsetof(s_replication_incoming_request, request_data) == 8) ? 1 : -1];
typedef char replication_incoming_request_blocks_offset[(sizeof(void *) != 4 || ((offsetof(s_replication_incoming_request, blocks) == 0x1c) )) ? 1 : -1];

#endif // NETWORKING_REPLICATION_REPLICATION_TYPES_H
