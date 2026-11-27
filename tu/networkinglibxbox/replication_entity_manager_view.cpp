// networkinglibxbox/replication_entity_manager_view.obj: from the records 0x41a690 0x41a780 0x41a808 0x41a858 0x41aab8 0x41abe0 0x41ad88 0x41aeb0 0x41b030 0x41b070 0x41b1d8 0x41b448 0x41b5f8 0x41b6e8 0x41b9c0 0x41bac8
// @flags /TP /O2 /Oc
#include "networking/network_time_access.h"
#include "networking/simulation/simulation_entity_definition_types.h"
#include "networking/delivery/network_delivery_types.h"
#include "networking/replication/replication_entity_view_state_types.h"
#include "networking/replication/replication_entity_manager.h"
#include "networking/replication/replication_entity_manager_view.h"
#include "networking/simulation/simulation_entity_database.h"
#include "simulation/simulation_globals.h"
#include <string.h>
#include <stddef.h>
#include "networking/simulation/simulation_view_telemetry.h"
#include "cseries/cseries.h"
#include "networking/network_memory.h"
#include "networking/replication/replication_encoding.h"
#include "simulation/simulation_priority.h"
#include "networking/replication/replication_types.h"
#include "memory/bitstream.h"
#include "cseries/crt_xdk.h"
#include "memory/tls_globals.h"
#include "game/allocator_types.h"
#include "simulation/simulation_queue_types.h"
#include "networking/network_time_types.h"
#include "networking/replication/replication_motion.h"
#include "simulation/simulation_world_types.h"
#include "networking/network_memory_types.h"
#include "networking/simulation/simulation_entity_database_operations.h"
#include "networking/simulation/simulation_accessors.h"
struct s_replication_entity_manager_view_outgoing_database;
// @address 0x40a2c8 ?notify_mark_entity_for_deletion@c_simulation_entity_database@@QAAXJ_N@Z
// @address 0x40a448 ?entity_delete_internal@c_simulation_entity_database@@AAAXJ@Z
// @address 0x4167f0 ?acknowledge_entity_deletion@c_replication_entity_manager@@AAAXJJ@Z
// @address 0x41a690 ?reset@c_replication_entity_manager_view@@QAAXXZ
// @address 0x41bac8 ?set_state@c_replication_entity_manager_view@@AAAXJW4e_replication_entity_view_state@@@Z
// @address 0x57ead8 ?allocate@c_rockall_heap@@QAAPAXJ@Z
// @address 0x58e5c0 memset
// @address 0x80608 GetTickCount



class c_replication_entity_update_record {
public:
    long entity_index;
    unsigned long update_mask;
    c_replication_entity_update_record *entity_next;
    c_replication_entity_update_record *packet_next;
    __forceinline c_replication_entity_update_record() : entity_next(0), packet_next(0) {}
    __forceinline static void *operator new(size_t size) {
        return network_shared_memory_globals.heap->allocate(size);
    }
};
struct s_replication_entity_view_packet {
    long sequence;
    void *status;
    c_replication_entity_update_record *updates;
    s_replication_entity_view_packet *next;
    __forceinline s_replication_entity_view_packet() : sequence(-1), status(0), updates(0), next(0) {}
    __forceinline static void *operator new(size_t size) { return network_shared_memory_globals.heap->allocate(size); }
};
struct s_replication_entity_view_status_record {
    long entity_index;
    long type;
    s_replication_entity_view_status_record *next;
    __forceinline s_replication_entity_view_status_record() : type(0), next(0) {}
    __forceinline static void *operator new(size_t size) { return network_shared_memory_globals.heap->allocate(size); }
};
enum e_replication_entity_request_type
{
    _replication_entity_request_none,
    _replication_entity_request_creation,
    _replication_entity_request_deletion,
    _replication_entity_request_update
};
static __forceinline void unlink_entity_update(s_replication_entity_view_packet *packets,
    s_replication_entity_view_packet *packet, long entity_index, c_replication_entity_update_record *update)
{
    for (s_replication_entity_view_packet *p = packets; p && p != packet; ) {
        c_replication_entity_update_record *u = p->updates;
        p = p->next;
        for (; u; u = u->packet_next) {
            if (u->entity_index == entity_index && u->entity_next == update) {
                u->entity_next = update->entity_next;
                return;
            }
        }
    }
}

static __forceinline bool encode_entity_update(c_simulation_entity_database *database, long index, unsigned long mask, const
    s_simulation_view_telemetry_data *context, c_bitstream *packet, long leave, unsigned long *written) {
        c_simulation_entity_database *data = database;
        s_simulation_entity *entity = &data->channels[index & 1023];
        bool force = true;
        c_simulation_entity_definition *definition = data->listeners[entity->entity_type + 1];
        *written = 0;
        return definition->entity_update_encode(false, mask, written, entity->state_data_size, entity->state_data, context, packet, leave, force);
    }

class c_replication_entity_manager;
class c_replication_entity_manager_view;
__forceinline void c_replication_entity_manager::replication_entity_manager_view_notify_delete_remote_entity(
    c_replication_entity_manager *manager, long entity_index, bool force_cleanup)
{
    s_replication_entity_data *entity = manager->get_entity(entity_index);
    entity->flags |= 2;
    entity->views = 0;
    manager->m_client->c_simulation_entity_database::notify_mark_entity_for_deletion(entity_index, force_cleanup);
    manager->m_client->entity_delete_internal(entity_index);
    entity->flags &= ~1;
}
__forceinline void c_replication_entity_manager::replication_entity_manager_view_process_creation(c_replication_entity_manager *manager, long entity_index,
    long entity_type, unsigned long update_mask, long block_count, s_replication_allocation_block *blocks)
{
    s_replication_entity_data *entity = manager->get_entity(entity_index);
    entity->flags = 1;
    entity->seed = (unsigned char)((unsigned long)entity_index >> 28);
    entity->views = 0;
    manager->m_client->c_simulation_entity_database::process_creation(entity_index, entity_type, update_mask, block_count, blocks);
}

__forceinline s_replication_entity_view_entry *c_replication_entity_manager_view::get_entity(long id) { return &entries[id & 0x3ff]; }
__forceinline void c_replication_entity_manager_view::set_pending_update(unsigned long index, bool enabled) {
    unsigned long word_index = index >> 6;
    unsigned long bit_index = index & 63;
    if (enabled) pending_bits[word_index] |= (unsigned __int64)1 << bit_index;
    else pending_bits[word_index] &= ~((unsigned __int64)1 << bit_index);
}
__forceinline void c_replication_entity_manager_view::push_status_record(long entity_index, long type) {
    s_replication_entity_view_status_record *record = new s_replication_entity_view_status_record;
    if (record) {
        record->next = (s_replication_entity_view_status_record *)outgoing_packet->status;
        outgoing_packet->status = record;
        record->entity_index = entity_index;
        record->type = type;
    } else {
        fatal_error = true;
    }
}

// @address 0x41a690 ?reset@c_replication_entity_manager_view@@QAAXXZ
void c_replication_entity_manager_view::reset()
{
    while (packets != 0) {
        notify_packet_retired(packets->sequence, false);
    }
    for (long index = 0; index < 1024; index++) {
        if (entries[index].state != _replication_entity_view_state_none) {
            long entity_index = entries[index].id;
            s_replication_entity_data *entity = definitions->get_entity(entity_index);
            if (entity->views & (1 << local_index)) {
                entity->views &= ~(1 << local_index);
                definitions->acknowledge_entity_deletion(local_index, entity_index);
            }
            entries[index].state = _replication_entity_view_state_none;
            entries[index].id = -1;
            entries[index].flags.clear();
        }
    }
    memset(pending_bits, 0, sizeof(pending_bits));
    statistics.creations_pending = 0;
    statistics.creations_completed = 0;
    statistics.updates_pending = 0;
    statistics.updates_completed = 0;
}

// @address 0x41a780 ?set_entity_dirty@c_replication_entity_manager_view@@QAAXJK@Z
void c_replication_entity_manager_view::set_entity_dirty(long entity_index, unsigned long update_mask)
{
    s_replication_entity_view_entry *view_data = get_entity(entity_index);
    s_replication_entity_data *entity_data = definitions->get_entity(entity_index);
    if (view_data->state == _replication_entity_view_state_replicated && !(entity_data->views & (1 << local_index)) && view_data->update_mask == 0) {
        ++statistics.updates_pending;
        unsigned long absolute_index = entity_index & 0x3ff;
        set_pending_update(absolute_index, true);
    }
    view_data->update_mask |= update_mask;
}

// @address 0x41a808 ?has_data_to_transmit@c_replication_entity_manager_view@@UAA_NXZ
bool c_replication_entity_manager_view::has_data_to_transmit()
{
    bool has_data = false;
    if (transmitting) {
        has_data |= statistics.updates_pending != 0;
        has_data |= statistics.creations_pending != 0;
        has_data |= statistics.deletions_pending != 0;
    }
    return has_data;
}

// @address 0x41b030 ?write_terminator_to_packet@c_replication_entity_manager_view@@UAAXJPAVc_bitstream@@@Z
void c_replication_entity_manager_view::write_terminator_to_packet(long packet_index, c_bitstream *packet)
{
    packet->write_integer("code", 0, 3);
    packet_entity_count = 0;
}

// @address 0x41aab8 ?write_to_packet@c_replication_entity_manager_view@@UAAXJJPBXJPAVc_bitstream@@J@Z
void c_replication_entity_manager_view::write_to_packet(
    LONG_PTR absolute_index, long request_type, const void *data, long request_size, c_bitstream *packet, long bits_reserved)
{
    if (!outgoing_packet) {
        s_replication_entity_view_packet *new_packet = new s_replication_entity_view_packet;
        outgoing_packet = new_packet;
        if (new_packet) {
            outgoing_packet->sequence = request_size;
            outgoing_packet->status = 0;
            outgoing_packet->updates = 0;
            outgoing_packet->next = 0;
            s_replication_entity_view_packet **tail = &packets;
            if (*tail) {
                do {
                    tail = &(*tail)->next;
                } while (*tail);
            }
            *tail = outgoing_packet;
        } else {
            fatal_error = true;
        }
    }
    if (!fatal_error) {
        if (request_type != 1) {
            if (request_type != 2) {
                write_update_to_packet((long)absolute_index, data, packet, bits_reserved);
                return;
            }
            write_deletion_to_packet((long)absolute_index, data, packet, bits_reserved);
            return;
        }
        write_creation_to_packet((long)absolute_index, data, packet, bits_reserved);
    }
}


// @address 0x41abe0 ?write_creation_to_packet@c_replication_entity_manager_view@@AAA_NJPBXPAVc_bitstream@@J@Z
bool c_replication_entity_manager_view::write_creation_to_packet(
    // match: 0x41abe0: pointer __restrict qualifier prevents alias reload
    long absolute_index, const void * data, c_bitstream *__restrict packet, long bits_reserved)
{
    bool success = false;
    unsigned long written_mask = 0;
    long entity_index = entries[absolute_index].id;
    packet->push_position();
    packet->write_integer("code", 1, 3);
    packet->write_integer("entity-index", entity_index & 0x3ff, 10);
    packet->write_value_internal((unsigned long)entity_index >> 28, 4);
    if (definitions->get_client()->entity_write_update(entity_index, entries[absolute_index].update_mask, (const s_simulation_view_telemetry_data
        *)data, packet, bits_reserved, &written_mask)
        && packet->get_space_left_in_bits() >= bits_reserved) {
        packet->pop_position(false);
        s_replication_entity_view_status_record *record = new s_replication_entity_view_status_record;
        unsigned long entity_slot = entity_index & 0x3ff;
        if (record) {
            record->next = (s_replication_entity_view_status_record *)outgoing_packet->status;
            outgoing_packet->status = record;
            record->entity_index = entity_index;
            record->type = 1;
        } else {
            fatal_error = true;
        }
        entries[entity_slot].flags.set(_creation_sent_bit, true);
        set_state(entity_index, _replication_entity_view_state_creating);
        if (written_mask)
            record_entity_update(entity_index, written_mask);
        success = true;
    } else {
        packet->pop_position(true);
    }
    return success;
}

// @address 0x41ad88 ?write_deletion_to_packet@c_replication_entity_manager_view@@AAA_NJPBXPAVc_bitstream@@J@Z
// match: 0x41ad88: pointer __restrict qualifier prevents alias reload
bool c_replication_entity_manager_view::write_deletion_to_packet(long absolute_index, const void *data, c_bitstream * __restrict packet, long bits_reserved)
{
    bool success = false;
    long entity_index = entries[absolute_index].id;
    packet->push_position();
    packet->write_integer("code", 2, 3);
    packet->write_integer("entity-index", entity_index & 0x3ff, 10);
    packet->write_value_internal((unsigned long)entity_index >> 28, 4);
    if (packet->get_space_left_in_bits() >= bits_reserved) {
        packet->pop_position(false);
        push_status_record(entity_index, 2);
        set_state(entity_index, _replication_entity_view_state_pending_deletion);
        success = true;
    } else packet->pop_position(true);
    return success;
}

// @address 0x41aeb0 ?write_update_to_packet@c_replication_entity_manager_view@@AAA_NJPBXPAVc_bitstream@@J@Z
bool c_replication_entity_manager_view::write_update_to_packet(long absolute_index, const void *data, c_bitstream *packet, long bits_reserved)
{
    bool success = false;
    bool wrote = false;
    unsigned long written_mask = 0;
    long entity_index = entries[absolute_index].id;
    packet->push_position();
    packet->write_integer("code", 3, 3);
    packet->write_integer("entity-index", entity_index & 0x3ff, 10);
    packet->write_value_internal((unsigned long)entity_index >> 28, 4);
    bool update = encode_entity_update(definitions->get_client(), 
                          entity_index, entries[absolute_index].update_mask, (const s_simulation_view_telemetry_data *)data, packet, bits_reserved,
                              &written_mask);
    wrote = update && written_mask != 0;
    if (wrote && packet->get_space_left_in_bits() >= bits_reserved) {
        packet->pop_position(false);
        record_entity_update(entity_index, written_mask);
        success = true;
    } else packet->pop_position(true);
    return success;
}


// @address 0x41b070 ?record_entity_update@c_replication_entity_manager_view@@AAAXJK@Z
void c_replication_entity_manager_view::record_entity_update(long entity_index, unsigned long update_mask)
{
    c_replication_entity_update_record *record = new c_replication_entity_update_record;
    unsigned long absolute_index = entity_index & 0x3ff;
    if (record) {
        record->packet_next = outgoing_packet->updates;
        outgoing_packet->updates = record;
        c_replication_entity_update_record *previous_record = 0;
        s_replication_entity_view_packet *packet = packets;
        while (packet) {
            c_replication_entity_update_record *update = packet->updates;
            while (update) {
                if (update->entity_index == entity_index) {
                    previous_record = update;
                    break;
                } else {
                    update = update->packet_next;
                }
            }
            packet = packet->next;
        }
        if (previous_record)
            previous_record->entity_next = record;
        record->entity_index = entity_index;
        record->update_mask = update_mask;
        record->entity_next = 0;
    } else {
        fatal_error = true;
    }
    entries[absolute_index].update_mask &= ~update_mask;
    entries[absolute_index].last_update_time = network_time_get();
    if (entries[absolute_index].state == _replication_entity_view_state_replicated && entries[absolute_index].update_mask == 0) {
        --statistics.updates_pending;
        set_pending_update(absolute_index, false);
    }
}

struct s_game_state;

static __forceinline void replication_entity_manager_view_process_update(c_replication_entity_manager *manager, long entity_index,
    unsigned long update_mask, long block_count, s_replication_allocation_block *blocks)
{
    s_simulation_entity *entity = &manager->get_client()->channels[entity_index & 1023];
    memcpy(entity->state_data, blocks[0].block_data, entity->state_data_size);
    simulation_get_world()->simulation_queue_enqueue((s_simulation_queue_element *)*(void **)blocks[1].block_data);
    ::network_heap_free_block(blocks[1].block_data);
    memset(&blocks[1], 0, sizeof(s_replication_allocation_block));
}
// @address 0x41b1d8 ?read_from_packet@c_replication_entity_manager_view@@UAA?AW4e_network_read_result@@JPAVc_bitstream@@JPAUs_replication_incoming_request@@PAJ@Z
e_network_read_result c_replication_entity_manager_view::read_from_packet(
    long packet_index, c_bitstream *packet, long maximum_requests, s_replication_incoming_request *requests, long *request_count)
{
    e_network_read_result result = _network_read_result_success;
    long count = 0;
    if (fatal_error) result = _network_read_result_discard;
    while (result == _network_read_result_success) {
        unsigned long code = packet->read_integer("code", 3);
        long entity_count;
        if (code < 4) entity_count = code != 0;
        else result = _network_read_result_error;
        if (result == _network_read_result_success && count >= maximum_requests && entity_count > 0)
            result = _network_read_result_error;
        if (result != _network_read_result_success || !entity_count) break;
        s_replication_request_data data;
        memset(&data, 0, sizeof(data));
        long block_count = 0;
        s_replication_allocation_block blocks[8];
        data.entity_count = entity_count;
        unsigned long low = packet->read_integer("absolute-index", 10);
        unsigned long high = packet->read_integer("seed", 4);
        long entity_index = (high << 28) | low;
        data.entity_index = entity_index;
        long absolute_index = entity_index & 1023;
        switch (code) {
        case 1:
            result = definitions->get_client()->c_simulation_entity_database::read_creation_from_packet(
                entity_index, &data.entity_type, &data.entity_update_mask, 8, &block_count, blocks, packet);
            break;
        case 2:
            break;
        case 3: {
            if (entries[absolute_index].id == entity_index && (unsigned short)entries[absolute_index].state == _replication_entity_view_state_replicated) {
                result = definitions->get_client()->c_simulation_entity_database::read_update_from_packet(
                    entity_index, &data.entity_update_mask, 8, &block_count, blocks, packet);
            } else result = _network_read_result_discard;
            break;
        }
        default:
            result = _network_read_result_error;
            break;
        }
        if (result == _network_read_result_success) {
            s_replication_incoming_request *request = &requests[count++];
            request->request_type = code;
            request->entity_index = data.entity_index;
            memcpy(&request->request_data, &data, sizeof(data));
            request->block_count = block_count;
            memcpy(request->blocks, blocks, sizeof(blocks));

        } else {
            for (long i = 0; i < block_count; ++i) {
                s_replication_allocation_block *block = &blocks[i];
                if (block->block_data) {
                    if (!packet->read_only_for_consistency()) {
                        switch (block->block_type) {
                        case _replication_entity_allocation_simulation_queue:
                            simulation_get_world()->simulation_queue_free(*(s_simulation_queue_element **)block->block_data);
                            break;
                        case _replication_entity_allocation_gamestate_entity:
                            datum_delete(tls_simulation_gamestate_entity_data(), *(long *)block->block_data);
                            break;
                        }
                    }
                    ::network_heap_free_block(block->block_data);
                    block->block_data = 0;
                }
            }
        }
    }
    *request_count = count;
    return result;
}

// @address 0x41b448 ?process_incoming_request@c_replication_entity_manager_view@@UAAXPAUs_replication_incoming_request@@@Z
void c_replication_entity_manager_view::process_incoming_request(s_replication_incoming_request *replication_incoming_request)
{
    s_replication_incoming_request *r = replication_incoming_request;
    s_replication_request_data *data = &r->request_data;
    long index = r->entity_index;
    long absolute = index & 1023;
    switch (r->request_type) {
    case 1: {
        bool success = true;
        if (!handle_incoming_creation(data->entity_index, data->entity_type, r->blocks)) {
            success = false;
        }
        if (success) {
            c_replication_entity_manager::replication_entity_manager_view_process_creation(
                definitions, index, data->entity_type, data->entity_update_mask, r->block_count, r->blocks);
            set_state(data->entity_index, _replication_entity_view_state_replicated);
        }
        break;
    }
    case 2:
        if (entries[absolute].state != _replication_entity_view_state_none
            && entries[absolute].id == index
            && entries[absolute].state == _replication_entity_view_state_replicated) {
            set_state(index, _replication_entity_view_state_none);
            c_replication_entity_manager::replication_entity_manager_view_notify_delete_remote_entity(definitions, index, false);
        }
        break;
    case 3: {
        replication_entity_manager_view_process_update(definitions, index, data->entity_update_mask, r->block_count, r->blocks);
        break;
    }
    // match: 0x41b448: unreachable hint omits dead branch check
    default: __assume(0);
    }
}

// @address 0x41b5f8 ?handle_incoming_creation@c_replication_entity_manager_view@@AAA_NJJPBUs_replication_allocation_block@@@Z
bool c_replication_entity_manager_view::handle_incoming_creation(long entity_index, long entity_type, const s_replication_allocation_block *blocks)
{
    bool result = false;
    unsigned long absolute_index = entity_index & 0x3ff;
    if (entries[absolute_index].state != _replication_entity_view_state_none) {
        long existing_entity_index = entries[absolute_index].id;
        bool replace = true;
        unsigned char seed = (unsigned char)((unsigned long)entity_index >> 28);
        unsigned char existing_seed = (unsigned char)((unsigned long)existing_entity_index >> 28);
        long seed_delta = 16 + seed - existing_seed;
        if (seed_delta > 16 / 2)
            seed_delta -= 16;
        if (seed_delta < 0 || seed_delta >= 16 / 4) {
            if (seed_delta > -16 / 4)
                replace = false;
        }
        if (replace) {
            set_state(existing_entity_index, _replication_entity_view_state_none);
            c_replication_entity_manager::replication_entity_manager_view_notify_delete_remote_entity(definitions, existing_entity_index, true);
            result = true;
        }
    } else {
        result = true;
    }
    return result;
}

// @address 0x41b9c0 ?mark_entity_for_deletion@c_replication_entity_manager_view@@QAAXJ@Z
void c_replication_entity_manager_view::mark_entity_for_deletion(long entity_index)
{
    s_replication_entity_data *entity_data = definitions->get_entity(entity_index);
    unsigned long absolute_index = entity_index & 0x3ff;
    s_replication_entity_view_entry *view_data = &entries[absolute_index];
    if (view_data->state == _replication_entity_view_state_active && !view_data->creation_sent()) {
        set_state(entity_index, _replication_entity_view_state_none);
        ++statistics.deletions_completed;
    } else {
        if (view_data->state == _replication_entity_view_state_active)
            set_state(entity_index, _replication_entity_view_state_replicated);
        entity_data->views |= 1 << local_index;
        if (view_data->state == _replication_entity_view_state_replicated) {
            if (view_data->update_mask != 0) {
                --statistics.updates_pending;
                set_pending_update(absolute_index, false);
            }
            ++statistics.deletions_pending;
            set_pending_update(absolute_index, true);
        }
    }
}

void c_replication_entity_manager_view::set_state(long id, e_replication_entity_view_state state)
{
    c_replication_entity_manager_view *manager = this;
    unsigned long index = id & 0x3ff;
    s_replication_entity_view_entry *entry = &manager->entries[index];
    long old_state = manager->entries[index].state;
    if (state == old_state)
        return;
    s_replication_entity_data *element = &manager->definitions->m_entity_data[index];
    if (old_state == 1) {
        manager->statistics.creations_pending--;
        { unsigned long word_index = index >> 6; unsigned long bit_index = index & 63; manager->pending_bits[word_index] &= ~((unsigned __int64)1 <<
            bit_index); }
    } else if (old_state == 3) {
        if (element->views & (1 << manager->local_index)) {
            manager->statistics.deletions_pending--;
            { unsigned long word_index = index >> 6; unsigned long bit_index = index & 63; manager->pending_bits[word_index] &= ~((unsigned __int64)1
                << bit_index); }
        } else if (manager->entries[index].update_mask) {
            manager->statistics.updates_pending--;
            { unsigned long word_index = index >> 6; unsigned long bit_index = index & 63; manager->pending_bits[word_index] &= ~((unsigned __int64)1
                << bit_index); }
        }
    }
    if (state == 1) {
        manager->statistics.creations_pending++;
        { unsigned long word_index = index >> 6; unsigned long bit_index = index & 63; manager->pending_bits[word_index] |= (unsigned __int64)1 <<
            bit_index; }
    } else if (state == 3) {
        if (element->views & (1 << manager->local_index)) {
            manager->statistics.deletions_pending++;
            { unsigned long word_index = index >> 6; unsigned long bit_index = index & 63; manager->pending_bits[word_index] |= (unsigned __int64)1 <<
                bit_index; }
        } else if (manager->entries[index].update_mask) {
            manager->statistics.updates_pending++;
            { unsigned long word_index = index >> 6; unsigned long bit_index = index & 63; manager->pending_bits[word_index] |= (unsigned __int64)1 <<
                bit_index; }
        }
    }
    if (state == 0) {
        entry->id = -1;
        entry->flags.clear();
    } else if (manager->entries[index].state == 0) {
        manager->entries[index].id = id;
        manager->entries[index].update_mask = 0;
        manager->entries[index].last_update_time = 0;
    }
    manager->entries[index].state = state;
}

void c_replication_entity_manager_view::notify_packet_acknowledged(long sequence_number)
{
}

// @address 0x41b6e8 ?notify_packet_retired@c_replication_entity_manager_view@@UAAXJ_N@Z
void c_replication_entity_manager_view::notify_packet_retired(long packet_index, bool delivered)
{
    s_replication_entity_view_packet **prev = &packets;
    s_replication_entity_view_packet *packet = *prev;
    while (packet) {
        if (packet->sequence == packet_index)
            break;
        prev = &packet->next;
        packet = *prev;
    }
    if (!packet)
        return;
    for (c_replication_entity_update_record *update = packet->updates; update; ) {
        c_replication_entity_update_record *next_update = update->packet_next;
        long entity_index = update->entity_index;
        unsigned long absolute_index = entity_index & 0x3ff;
        s_replication_entity_data *entity = definitions->get_entity(entity_index);
        if (!delivered) {
            unsigned long remaining_mask = update->update_mask;
            for (s_replication_entity_view_packet *later = packet->next; later; later = later->next) {
                for (c_replication_entity_update_record *u = later->updates; u; u = u->packet_next) {
                    if (u->entity_index == entity_index)
                        remaining_mask &= ~u->update_mask;
                }
            }
            if (remaining_mask != 0
                && entries[absolute_index].update_mask == 0
                && entries[absolute_index].state == _replication_entity_view_state_replicated
                && ((1 << local_index) & entity->views) == 0) {
                ++statistics.updates_pending;
                set_pending_update(absolute_index, true);
            }
            entries[absolute_index].update_mask = remaining_mask | entries[absolute_index].update_mask;
        } else {
            ++statistics.updates_completed;
        }
        unlink_entity_update(packets, packet, entity_index, update);
        ::network_heap_free_block(update);
        update = next_update;
    }
    for (s_replication_entity_view_status_record *status = (s_replication_entity_view_status_record *)packet->status; status; ) {
        s_replication_entity_view_status_record *next_status = status->next;
        long status_type = status->type;
        long status_entity_index = status->entity_index;
        s_replication_entity_data *status_entity = definitions->get_entity(status_entity_index);
        if (status_type == 1) {
            if (!delivered) {
                if (((1 << local_index) & status_entity->views) != 0)
                    set_state(status_entity_index, _replication_entity_view_state_replicated);
                else
                    set_state(status_entity_index, _replication_entity_view_state_active);
            } else {
                set_state(status_entity_index, _replication_entity_view_state_replicated);
                ++statistics.creations_completed;
            }
        } else if (!delivered) {
            set_state(status_entity_index, _replication_entity_view_state_replicated);
        } else {
            set_state(status_entity_index, _replication_entity_view_state_none);
            status_entity->views &= ~(1 << local_index);
            definitions->acknowledge_entity_deletion(local_index, status_entity_index);
            ++statistics.deletions_completed;
        }
        ::network_heap_free_block(status);
        status = next_status;
    }
    *prev = packet->next;
    ::network_heap_free_block(packet);
}

static __forceinline bool replication_entity_manager_view_deletion_requirements(const s_simulation_entity *entity, long *priority)
{
    bool result = false;
    if (entity->event_reference_count > 0) *priority = 0;
    else { result = true; *priority = 0xd999; }
    return result;
}
static __forceinline long replication_entity_manager_view_update_requirements(const s_simulation_entity *entity,
    c_simulation_entity_definition *definition, unsigned long update_mask, unsigned long last_update_time,
    const s_simulation_view_telemetry_data *telemetry, long *priority)
{
    s_entity_update_data update;
    long size = -1;
    if (telemetry->view_inactive) {
        *priority = 0;
    } else {
        update.update_mask = update_mask;
        update.last_update_time = last_update_time;
        update.telemetry = telemetry;
        size = definition->update_minimum_required_bits(entity, &update);
        *priority = (long)(simulation_calculate_entity_update_priority(entity, &update, 0, 0) * 65536.0f);
    }
    return size;
}
static __forceinline void replication_entity_manager_view_initialize_request(
    s_replication_outgoing_request *request, long absolute_index, long request_type, long view_index)
{
    request->identifier = absolute_index;
    request->request_type = (e_replication_entity_request_type)request_type;
    request->client_index = (short)view_index;
}
static __forceinline unsigned __int64 replication_entity_manager_view_pending_bit(unsigned __int64 pending)
{
    return pending & 1;
}
// @address 0x41a858 ?build_outgoing_requests@c_replication_entity_manager_view@@UAAJPBXJPAUs_replication_outgoing_request@@@Z
long c_replication_entity_manager_view::build_outgoing_requests(const void *data, long maximum_requests, s_replication_outgoing_request *requests)
{
    long view_mask = 1 << local_index;
    long count = 0;
    if (transmitting) {
        long absolute_index = 0;
        do {
            unsigned __int64 pending = pending_bits[(unsigned long)absolute_index >> 6];
            if (pending == 0) absolute_index += 64;
            else {
                unsigned long bit = 0;
                s_replication_outgoing_request *request = &requests[count];
                do {
                    const unsigned __int64 &pending_bit = pending & 1;
                    if (pending_bit != 0) {
                        if (count >= maximum_requests) return count;
                        const long &state_word = entries[absolute_index].state;
                        // match: 0x41a858: const reference binding preserves temporary lifetime
                        const unsigned long &state_value = (unsigned short)state_word;
                        if (state_value == 3) {
                            long entity_index = entries[absolute_index].id;
                            long entity_slot = entity_index & 1023;
                            if (definitions->m_entity_data[entity_slot].views & view_mask) {
                                replication_entity_manager_view_initialize_request(
                                    request, absolute_index, 2, m_client_index);
                                c_simulation_entity_database *database =
                                    (c_simulation_entity_database *)definitions->get_client();
                                if (replication_entity_manager_view_deletion_requirements(
                                        &database->channels[entity_slot], &request->priority)) {
                                    request->size_in_bits = 17;
                                    ++count; ++request;
                                }
                            } else {
                                replication_entity_manager_view_initialize_request(
                                    request, absolute_index, 3, m_client_index);
                                c_simulation_entity_database *update_database =
                                    (c_simulation_entity_database *)definitions->get_client();
                                const s_simulation_entity *update_entity = &update_database->channels[entity_slot];
                                c_simulation_entity_definition *const update_definition = update_database->listeners[update_entity->entity_type + 1];
                                long size = replication_entity_manager_view_update_requirements(update_entity, update_definition,
                                    entries[absolute_index].update_mask, entries[absolute_index].last_update_time,
                                    (const s_simulation_view_telemetry_data *)data, &request->priority);
                                if (size != -1) {
                                    request->size_in_bits = size + 17;
                                    ++count; ++request;
                                }
                            }
                        } else {
                            long creation_entity_index = entries[absolute_index].id;
                            replication_entity_manager_view_initialize_request(
                                request, absolute_index, 1, m_client_index);
                            long size = definitions->get_client()->c_simulation_entity_database::calculate_creation_requirements(
                                creation_entity_index, entries[absolute_index].update_mask, data, &request->priority);
                            if (size != -1) {
                                request->size_in_bits = size + 17;
                                ++count; ++request;
                            }
                        }
                    }
                    ++bit; ++absolute_index;
                    pending >>= 1;
                } while (bit < 64);
            }
        } while (absolute_index < 1024);
    }
    return count;
}
