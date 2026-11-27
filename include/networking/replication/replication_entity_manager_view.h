#ifndef NETWORKING_REPLICATION_REPLICATION_ENTITY_MANAGER_VIEW_H
#define NETWORKING_REPLICATION_REPLICATION_ENTITY_MANAGER_VIEW_H
#include <stddef.h>
#include "cseries/cseries_types.h"
#include "cseries/cseries_wrapped.h"
#include "networking/replication/replication_scheduler_client.h"
#include "networking/replication/replication_entity_view_state_types.h"
enum e_replication_entity_view_data_entity_flags { _creation_sent_bit = 0 };
struct s_replication_entity_view_entry
{
    c_flags<e_replication_entity_view_data_entity_flags, short, 1> flags;
    c_enum<e_replication_entity_view_state, short, 0, 6> state;
    long id;
    unsigned long update_mask;
    long last_update_time;
    __forceinline bool creation_sent() const { return flags.test(_creation_sent_bit); }
};
struct s_replication_entity_manager_view_statistics
{
    long creations_completed;
    long creations_pending;
    long updates_completed;
    long updates_pending;
    long deletions_completed;
    long deletions_pending;
};
class c_replication_entity_manager;
struct s_replication_entity_view_packet;
struct s_replication_allocation_block;
struct s_simulation_event_manager;
class c_replication_entity_manager_view : public c_replication_scheduler_client
{
public:
    c_replication_entity_manager_view() { initialized = false; }
    ~c_replication_entity_manager_view() {}
    virtual bool has_data_to_transmit();
    virtual long build_outgoing_requests(const void *, long, s_replication_outgoing_request *);
    virtual long terminator_required_bits() const { return 3; }
    virtual void write_to_packet(LONG_PTR, long, const void *, long, c_bitstream *, long);
    virtual void write_terminator_to_packet(long, c_bitstream *);
    virtual e_network_read_result read_from_packet(long, c_bitstream *, long, s_replication_incoming_request *, long *);
    virtual void process_incoming_request(s_replication_incoming_request *);
    virtual void notify_packet_acknowledged(long);
    virtual void notify_packet_retired(long, bool);
    bool initialized;
    bool transmitting;
    bool fatal_error;
    long local_index;
    long local_mask;
    unsigned long replication_start_time;
    c_replication_entity_manager *definitions;
    s_replication_entity_view_packet *packets;
    union { long packet_entity_count; s_replication_entity_view_packet *outgoing_packet; };
    unsigned long : 32;
    s_replication_entity_view_entry entries[1024];
    unsigned __int64 pending_bits[16];
    s_replication_entity_manager_view_statistics statistics;
    __forceinline void create_entity(long id) { set_state(id, _replication_entity_view_state_active); }
    void mark_entity_for_deletion(long entity_index);
    void reset();
    void set_entity_dirty(long entity_index, unsigned long update_mask);
private:
    s_replication_entity_view_entry *get_entity(long id);
    void set_pending_update(unsigned long index, bool enabled);
    void push_status_record(long entity_index, long type);
    bool handle_incoming_creation(long entity_index, long entity_type, const s_replication_allocation_block *blocks);
    void record_entity_update(long entity_index, unsigned long update_mask);
    bool write_creation_to_packet(long, const void *, c_bitstream *, long);
    bool write_deletion_to_packet(long, const void *, c_bitstream *, long);
    bool write_update_to_packet(long, const void *, c_bitstream *, long);
    void set_state(long id, e_replication_entity_view_state state);
    friend class c_simulation_view;
    friend struct s_simulation_entity_manager;
};
typedef char replication_entity_start_time_offset[(sizeof(void *) != 4
    || offsetof(c_replication_entity_manager_view, replication_start_time) == 0x14) ? 1 : -1];
typedef char replication_entity_statistics_offset[(sizeof(void *) != 4
    || offsetof(c_replication_entity_manager_view, statistics) == 0x40a8) ? 1 : -1];
#endif
