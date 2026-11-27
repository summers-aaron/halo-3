// networkinglibxbox/network_memory.obj: from the records 0x126a50 0x219b30 0x3b5f38 0x3b5f70 0x3b5fa8 0x3b6140 0x3b61b8 0x3b6290 0x3b6318 0x3b63e0 0x3b66c0 0x3b67a0 0x3b6a30 0x3b6b48 0x3b6cb0 0x3b6cf0 0x3b6d48 0x3b6e28 0x42d298
// @flags /TP /O2 /Oc
// @address 0x448b8 ??_7c_network_connection@@6B@
// @address 0x448d8 ??_7c_network_message_queue@@6B@
// @address 0x448f8 ??_7c_network_channel_simulation_gatekeeper@@6B@
// @address 0x44890 ??_7c_network_session@@6B@
// @address 0x4488c ??_7c_network_message_gateway@B@
// @address 0x448a4 ??_7c_simulation_watcher@@6B@
// @address 0x42d298 ?get_client_name@c_network_connection@@UBAPBDXZ
// @address 0x42d298 ?get_client_name@c_network_message_queue@@UBAPBDXZ
// @address 0x42d298 ?get_client_name@c_network_channel_simulation_gatekeeper@@UBAPBDXZ
// @address 0x1b65a8 ?notify_packet_acknowledged@c_network_connection@@UAAXJ@Z
// @address 0x1b65a8 ?notify_packet_retired@c_network_connection@@UAAXJ_N@Z
// @address 0x1b65a8 ?notify_packet_acknowledged@c_network_message_queue@@UAAXJ@Z
// @address 0x1b65a8 ?notify_packet_acknowledged@c_network_channel_simulation_gatekeeper@@UAAXJ@Z
// @address 0x1b65a8 ?notify_packet_retired@c_network_channel_simulation_gatekeeper@@UAAXJ_N@Z
// @address 0x42d298 ?connection_lost@c_network_channel_simulation_gatekeeper@@UBA_NPAW4e_network_channel_closure_reason@@@Z
// @address 0x42d298 ?has_data_to_transmit@c_network_channel_simulation_gatekeeper@@UBA_NPA_N@Z
// @address 0x42d380 ?space_required_bits@c_network_channel_simulation_gatekeeper@@UAAJJJ@Z
// @address 0x3fc598 ?space_required_bits@c_network_message_queue@@UAAJJJ@Z
#include "memory/data.h"
#include "networking/network_memory_types.h"
#include "networking/delivery/network_link.h"
#include "networking/messages/network_message_gateway.h"
#include "networking/delivery/network_channel_closure_reason_types.h"
// @address 0x3b6290 ??0s_network_session_shared_membership@@QAA@XZ
// @address 0x3b6318 ??0s_network_distributed_view@@QAA@XZ
// @address 0x3b5fa8 ??0c_network_channel@@QAA@XZ
// @address 0x3b6a30 ?network_memory_base_dispose@@YAXXZ
// @address 0x3b6cb0 ?network_memory_shared_dispose@@YAXXZ
// @address 0x126a50 ?network_channel_iterate@@YAPAVc_network_channel@@PAV1@@Z
// @address 0x3b5f38 ?connection_lost@c_network_connection@@UBA_NPAW4e_network_channel_closure_reason@@@Z
// @address 0x3b5f70 ?connection_lost@c_network_message_queue@@UBA_NPAW4e_network_channel_closure_reason@@@Z
// @address 0x42d298 ?get_client_name@c_replication_scheduler@@UBAPBDXZ
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0x3b6140 ??1c_network_channel@@QAA@XZ
// @address 0x4153c0 ?reset@c_network_message_queue@@QAAXXZ
#include <string.h>
#include "networking/logic/network_life_cycle.h"
#include "networking/network_globals.h"
#include "networking/network_globals_types.h"
#include "networking/network_memory.h"
#include "networking/network_configuration.h"
#include "networking/transport/transport_security.h"
#include "networking/session/network_session_parameters.h"
#include "memory/bitstream.h"
#include "networking/session/network_session_membership_types.h"
#include "memory/sliding_window.h"
#include "networking/network_statistics.h"
#include "cseries/crt_xdk.h"
#include "memory/physical_memory_map.h"
#include "memory/physical_memory_map_types.h"
#include "networking/session/network_session_membership.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/session/network_session_manager.h"

struct s_transport_secure_address;
struct s_channel_configuration;
class c_network_link;
class c_network_observer;
class c_network_message_gateway;
class c_network_message_handler;
class c_network_message_type_collection;

#include "networking/delivery/network_channel.h"
#include "networking/network_configuration_types.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/network_memory_base_types.h"

#include "networking/replication/replication_scheduler.h"
#include "game/player_types.h"
#include "game/game_results_player_data.h"
#include "networking/replication/replication_entity_manager_view.h"
#include "networking/replication/replication_entity_manager.h"
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_watcher_types.h"
#include "simulation/simulation_distributed_view.h"
#include "simulation/simulation_distributed_world_types.h"
#include "cseries/placement_new.h"

struct s_network_distributed_view {
    s_network_distributed_view();
    // Element header of the distributed_views data array, written by datum_new_at_index.
    s_datum_header m_header;
    c_replication_scheduler scheduler;
    __declspec(align(8)) c_replication_entity_manager_view m_table;
    c_replication_event_manager_view m_events;
    c_replication_control_view m_control;
    c_simulation_view_telemetry_provider m_telemetry_provider;
    long m_telemetry_alignment;
    __declspec(align(8)) c_game_results_replicator m_game_results;
};

struct s_network_memory_database {
    bool initialized;
    c_simulation_world *world;
    c_replication_entity_manager *entity_manager;
    c_simulation_type_collection *types;
    s_simulation_entity entities[1024];
};
struct s_network_memory_event_handler {
    bool initialized;
    c_simulation_world *world;
    c_replication_event_manager *event_manager;
    c_simulation_type_collection *types;
    c_simulation_entity_database *entity_database;
};

// match: 0x3b6b48: construction initializes the database and handler
// without vptrs; the canonical virtual interfaces cannot express these stores.
class c_network_memory_distributed_world {
public:
    c_replication_entity_manager entity_manager;
    c_replication_event_manager event_manager;
    s_network_memory_database database;
    s_network_memory_event_handler handler;
    __forceinline c_network_memory_distributed_world() {
        s_replication_entity_data *entry = entity_manager.m_entity_data;
        for (long remaining = 1023; remaining >= 0; --remaining, ++entry)
            entry->flags = 0;
        database.initialized = false; database.world = 0; database.entity_manager = 0; database.types = 0;
        // match: 0x3b6b48: retain construction through the handler subobject;
        // flattened high-offset stores generate separate indexed addresses.
        s_network_memory_event_handler *h = &handler;
        h->initialized = false; h->world = 0; h->event_manager = 0; h->types = 0; h->entity_database = 0;
    }
};

#include "memory/no_allocation.h"
#include "memory/data_array_functions.h"

#include "simulation/simulation_view.h"

struct view {
    c_simulation_view simulation_view;
    __forceinline void construct() {
        new (&simulation_view) c_simulation_view;
    }
};

#include "networking/session/network_session.h"

class c_fixed_memory_rockall_heap : public c_rockall_heap {
public:
    __forceinline c_fixed_memory_rockall_heap(void *memory, long size) {
        m_rockall_front_end = new c_fixed_memory_rockall_frontend(memory, size, &free_page_count, true, true);
        m_allocation_count = 0;
        m_locked = false;
    }
    static __forceinline void *operator new(size_t size) {
        return RtlAllocateHeap(GetProcessHeap(), 0, size);
    }
    long free_page_count;
};

const char *c_network_connection::get_client_name() const { return 0; }
const char *c_network_message_queue::get_client_name() const { return 0; }
const char *c_network_channel_simulation_gatekeeper::get_client_name() const { return 0; }
void c_network_connection::notify_packet_acknowledged(long) {}
void c_network_connection::notify_packet_retired(long, bool) {}
void c_network_message_queue::notify_packet_acknowledged(long) {}
void c_network_channel_simulation_gatekeeper::notify_packet_acknowledged(long) {}
void c_network_channel_simulation_gatekeeper::notify_packet_retired(long, bool) {}
bool c_network_channel_simulation_gatekeeper::connection_lost(e_network_channel_closure_reason *) const { return false; }
bool c_network_channel_simulation_gatekeeper::has_data_to_transmit(bool *) const { return false; }
long c_network_channel_simulation_gatekeeper::space_required_bits(long, long) { return 1; }
long c_network_message_queue::space_required_bits(long, long) { return 2; }
inline c_network_connection::c_network_connection() : m_incoming_window(128), m_outgoing_window(128) {
    m_allocated = false; m_name = NULL;
}
inline c_network_connection::~c_network_connection() {
}
inline c_network_message_queue::c_network_message_queue() : m_outgoing_window(512), m_incoming_window(512) {
    m_initialized = false; m_name = NULL; m_message_types = 0;
}
inline c_network_message_queue::~c_network_message_queue() {
    if (m_initialized) { reset(); m_initialized = false; m_name = NULL; }
}
inline c_network_channel_simulation_gatekeeper::c_network_channel_simulation_gatekeeper() {
    m_initialized = false; m_active = false;
}
inline c_network_channel_simulation_gatekeeper::~c_network_channel_simulation_gatekeeper() {
}
    __forceinline void c_network_observer::destroy_channels() {
        for (long observer_index = 0; observer_index < 32; observer_index++) {
            s_channel_observer *observer = &m_channel_observers[observer_index];
            if (observer->state != 0) {
                c_network_channel *channel = &observer->channel;
                if (!channel->closed()) {
                    channel->close(_network_channel_closure_reason_channel_deleted);
                    update_observer_connection_state(observer_index);
                }
                channel->deallocate();
            }
        }
    }
    __forceinline c_network_channel *c_network_observer::channel_iterate(const c_network_channel *channel) {
        c_network_channel *result = 0;
        s_channel_observer *observer = channel == 0 ? m_channel_observers : find_observer_by_channel(channel) + 1;
        while (observer < m_channel_observers + 32) {
            if (observer->state != 0) {
                result = &observer->channel;
                break;
            } else {
                ++observer;
                continue;
            }
        }
        return result;
    }
s_network_base_memory_globals network_base_memory_globals;
struct s_network_globals;

static __forceinline bool network_initialized() {
    return network_globals.initialized;
}

c_network_channel *network_channel_iterate(c_network_channel *channel) {
    c_network_channel *result = 0;
    if (network_initialized()) {
        result = network_base_memory_globals.observer.channel_iterate(channel);
    }
    return result;
}

class c_simulation_distributed_world;
class c_rockall_heap;

// @address 0x219b30 network_shared_memory_get_bounds
extern "C" bool network_shared_memory_get_bounds(buffer *bounds)
{
    void *low = network_shared_memory_globals.low;
    void *high = network_shared_memory_globals.high;
    bool result = false;
    if (high > low) {
        bounds->address = low;
        bounds->size = (char *)high - (char *)low;
        result = true;
    }
    return result;
}

bool c_network_connection::connection_lost(e_network_channel_closure_reason *reason) const {
    bool result = false;
    if (m_allocated && m_overflowed) {
        result = true;
        if (reason) {
            *reason = _network_channel_closure_reason_connection_lost;
        }
    }
    return result;
}
bool c_network_message_queue::connection_lost(e_network_channel_closure_reason *reason) const {
    bool result = false;
    if (m_initialized && m_connection_lost) {
        result = true;
        if (reason) {
            *reason = _network_channel_closure_reason_message_queue_lost;
        }
    }
    return result;
}
#if defined(_XBOX) && !defined(__clang__)
inline c_network_channel::c_network_channel()
#else
c_network_channel::c_network_channel()
#endif
{
    m_link = 0;
    m_message_gateway = 0;
    m_message_handler = 0;
    m_client_count = 0;
    memset(m_clients, 0, sizeof(m_clients));
    m_simulation_interface = 0;
    m_connect_flags = 0;
    m_identifier = -1;
    m_remote_identifier = -1;
    m_state = _network_channel_state_none;
    m_closure_reason = _network_channel_closure_reason_none;
    m_destination_unreachable = false;
    memset(&m_remote_address, 0, sizeof(m_remote_address));
    m_next_connect_packet_time = 0;
}
c_network_channel::~c_network_channel()
{
}
s_network_session_shared_membership::s_network_session_shared_membership()
{
}

// @address 0x44918 ??_7c_replication_scheduler@@6B@
// @address 0x44938 ??_7c_replication_event_manager_view@@6B@
// @address 0x4495c ??_7c_replication_entity_manager_view@B@
// @address 0x45fc28 ?terminator_required_bits@c_replication_entity_manager_view@@UBAJXZ
// @address 0x44980 ??_7c_replication_control_view@@6B@
typedef char network_memory_view_table_offset[(sizeof(void *) != 4 || ((offsetof(s_network_distributed_view, m_table) == 48))) ? 1 : -1];
typedef char network_memory_view_events_offset[(sizeof(void *) != 4 || ((offsetof(s_network_distributed_view, m_events) == 16624))) ? 1 : -1];
typedef char network_memory_view_control_offset[(sizeof(void *) != 4 || ((offsetof(s_network_distributed_view, m_control) == 16664))) ? 1 : -1];
typedef char network_memory_view_results_offset[(sizeof(void *) != 4 || ((offsetof(s_network_distributed_view, m_game_results) == 21728))) ? 1 : -1];
typedef char network_memory_view_size[(sizeof(void *) != 4 || ((sizeof(s_network_distributed_view) == 46472))) ? 1 : -1];
s_network_distributed_view::s_network_distributed_view()
{
}


class c_allocation_base;

typedef char network_memory_world_database_offset[(sizeof(void *) != 4 || ((offsetof(c_network_memory_distributed_world, database) == 0x1098))) ? 1 : -1];
typedef char network_memory_world_handler_offset[(sizeof(void *) != 4 || ((offsetof(c_network_memory_distributed_world, handler) == 0xb0a8))) ? 1 : -1];
typedef char network_memory_world_size[(sizeof(void *) != 4 || ((sizeof(c_network_memory_distributed_world) == 0xb0bc))) ? 1 : -1];

s_network_shared_memory_globals network_shared_memory_globals;

static __forceinline bool map_memory_configuration_is_campaign(e_map_memory_configuration configuration) {
    return configuration == 4 || configuration == 5 || configuration == 6;
}
// 0x3b6a30
// @address 0x3b6a30 ?network_memory_base_dispose@@YAXXZ
void network_memory_base_dispose() {
    network_base_memory_globals.observer.destroy_channels();
    if (network_shared_memory_globals.views) {
        network_shared_memory_globals.views->valid = false;
        network_shared_memory_globals.views = 0;
    }
    if (network_shared_memory_globals.heap) {
        delete network_shared_memory_globals.heap;
        network_shared_memory_globals.heap = 0;
    }
    if (network_shared_memory_globals.heap_memory) {
        network_shared_memory_globals.heap_memory = 0;
    }
    network_shared_memory_globals.total = 0;
    network_shared_memory_globals.low = 0;
    network_shared_memory_globals.high = 0;
}

// @address 0x3b6b48 network_memory_shared_initialize
extern "C" void network_memory_shared_initialize(e_map_memory_configuration configuration) {
    network_shared_memory_globals.initialized = !map_memory_configuration_is_campaign(configuration);
    if (network_shared_memory_globals.initialized) {
        long size = sizeof(c_network_memory_distributed_world);
        network_shared_memory_globals.distributed_world = (c_simulation_distributed_world *)::_physical_memory_malloc(
            (memory_stage)4, (char const *)(0), size, 0, (char const *)(0), -1);
        network_memory_track_physical_allocation(size, network_shared_memory_globals.distributed_world);
        new (network_shared_memory_globals.distributed_world) c_network_memory_distributed_world;
        long view_size = data_allocation_size(15, sizeof(s_network_distributed_view), 0);
        network_shared_memory_globals.distributed_views = (s_data_array *)::_physical_memory_malloc(
            (memory_stage)4, (char const *)(0), view_size, 0, (char const *)(0), -1);
        network_memory_track_physical_allocation(view_size, network_shared_memory_globals.distributed_views);
        data_initialize(network_shared_memory_globals.distributed_views, "simulation distributed view", 15, sizeof(s_network_distributed_view), 0,
            &g_no_allocation_storage);
        data_make_valid(network_shared_memory_globals.distributed_views);
    }
}

void network_memory_shared_dispose() {
    if (network_shared_memory_globals.distributed_world) network_shared_memory_globals.distributed_world = 0;
    if (network_shared_memory_globals.distributed_views) {
        network_shared_memory_globals.distributed_views->valid = false;
        network_shared_memory_globals.distributed_views = 0;
    }
    network_shared_memory_globals.initialized = false;
}

// @address 0x3b6cf0 ?network_memory_track_physical_allocation@@YAXJPAX@Z
void network_memory_track_physical_allocation(long size, void *address)
{
    char *end = (char *)address + size;
    if (network_shared_memory_globals.total == 0) {
        network_shared_memory_globals.low = address;
        network_shared_memory_globals.high = end;
    } else {
        if (network_shared_memory_globals.low > address)
            network_shared_memory_globals.low = address;
        if (network_shared_memory_globals.high <= end)
            network_shared_memory_globals.high = end;
    }
    network_shared_memory_globals.total += size;
}

// @address 0x3b6d48 network_shared_memory_new_view
// match: 0x3b6d48: pointer __restrict qualifier prevents alias reload
extern "C" long network_shared_memory_new_view(view **out_view, s_network_distributed_view **__restrict out_distributed) {
    view *result = 0;
    s_network_distributed_view *distributed = 0;
    s_data_array *view_data = network_shared_memory_globals.views;
    long index = datum_new(view_data);
    if (index != -1) {
        result = (view *)((char *)view_data->data + (unsigned short)index * sizeof(view));
        if (result) result->construct();
        if (network_shared_memory_globals.initialized) {
            s_data_array *distributed_data = network_shared_memory_globals.distributed_views;
            long distributed_index = datum_new_at_index(distributed_data, index);
            distributed = (s_network_distributed_view *)((char *)distributed_data->data
                + (unsigned short)distributed_index * sizeof(s_network_distributed_view));
            if (distributed) new (distributed) s_network_distributed_view;
        }
    }
    *out_view = result;
    *out_distributed = distributed;
    return index;
}

// 0x3b6e28
// @address 0x3b6e28 ?network_heap_free_block@@YAXPAX@Z
void network_heap_free_block(void * block)
{
    char block_space[8];
    network_shared_memory_globals.heap->m_rockall_front_end->Details(block, (int *)block_space);
    network_shared_memory_globals.heap->deallocate(block);
}

const char *c_replication_scheduler::get_client_name() const {
    const char *name = 0;
    return name;
}

inline c_network_link::c_network_link()
{
    m_initialized = false;
    m_channel_identifier = 0;
    m_first_channel_index = 0;
    m_endpoints_created = false;
    memset(m_endpoints, 0, sizeof(m_endpoints));
    m_out_of_band_consumer = 0;
}

inline c_network_link::~c_network_link()
{
    destroy_endpoints();
    m_out_of_band_consumer = 0;
    m_initialized = false;
}

inline c_network_message_gateway::c_network_message_gateway()
{
    bitstream_construct(&m_outgoing_packet, m_outgoing_packet_storage, sizeof(m_outgoing_packet_storage));
    m_link = 0;
    m_message_types = 0;
    m_message_handler = 0;
    m_initialized = false;
}

inline c_network_observer::c_network_observer()
{
    m_link = 0;
    m_message_types = 0;
    for (unsigned long i = 0; i < 4; ++i) {
        m_owners[i].owner = 0;
        m_owners[i].session_index = -1;
    }
}

inline c_network_message_gateway::~c_network_message_gateway()
{
    if (m_initialized) {
        if (m_link) {
            m_link->m_out_of_band_consumer = 0;
            m_link = 0;
        }
        m_message_handler = 0;
        m_message_types = 0;
        m_initialized = false;
    }
}

// @address 0x3b61b8 ??0c_network_session@@QAA@XZ
c_network_session::c_network_session()
{
    c_network_session_parameters *parameters = get_session_parameters();
    parameters->m_parameters.c_network_session_parameters::s_network_session_parameters_internal::s_network_session_parameters_internal();
    parameters->m_flags = 0;
    parameters->m_peers_reset_mask = 0;
    memset(parameters->m_parameter_interfaces, 0, sizeof(parameters->m_parameter_interfaces));
    parameters->m_session = 0;
    parameters->m_observer = 0;
    m_session_type = _network_session_type_none;
    m_message_gateway = 0;
    m_session_class = _network_session_class_none;
    m_local_state = (e_network_session_state)NONE;
}

// @address 0x3b66c0 ??1s_network_base_memory_globals@@QAA@XZ
s_network_base_memory_globals::~s_network_base_memory_globals()
{
    // match: 0x3b66c0: teardown loads the global into r31 before the watcher vptr store; a generic this stays in r29.
    __assume(this == &network_base_memory_globals);
}

typedef char network_memory_globals_observer_offset[(sizeof(void *) != 4 || ((offsetof(s_network_base_memory_globals, observer) == 4320))) ? 1 : -1];
typedef char network_memory_globals_sessions_offset[(sizeof(void *) != 4 || ((offsetof(s_network_base_memory_globals, sessions) == 143456))) ? 1 : -1];
typedef char network_memory_globals_world_offset[(sizeof(void *) != 4 || ((offsetof(s_network_base_memory_globals, world) == 1425576))) ? 1 : -1];
typedef char network_memory_globals_watcher_offset[(sizeof(void *) != 4 || ((offsetof(s_network_base_memory_globals, watcher) == 1430680))) ? 1 : -1];
typedef char network_memory_globals_types_offset[(sizeof(void *) != 4 || ((offsetof(s_network_base_memory_globals, types) == 1435792))) ? 1 : -1];

static __forceinline c_network_link network_memory_create_link(s_network_base_memory_globals *globals)
{
    // match: 0x3b63e0: specialize the receiver before the first statistics stores; using the incoming this shortens the constructor by 84 bytes.
    __assume(globals == &network_base_memory_globals);
    return c_network_link();
}

// @address 0x3b63e0 ??0s_network_base_memory_globals@@QAA@XZ
s_network_base_memory_globals::s_network_base_memory_globals()
    : link(network_memory_create_link(this))
{
    types.initialize();
}

static __forceinline void network_memory_track_allocation(long size, void *address) {
    char *end = (char *)address + size;
    if (network_shared_memory_globals.total == 0) {
        network_shared_memory_globals.low = address;
        network_shared_memory_globals.high = end;
    } else {
        if (network_shared_memory_globals.low > address)
            network_shared_memory_globals.low = address;
        if (network_shared_memory_globals.high <= end)
            network_shared_memory_globals.high = end;
    }
    network_shared_memory_globals.total += size;
}

__forceinline void network_memory_recreate_channels() {
    c_network_observer *observer = &network_base_memory_globals.observer;
    for (long observer_index = 0; observer_index < 32; ++observer_index) {
        c_network_observer::s_channel_observer *channel = &observer->m_channel_observers[observer_index];
        if (channel->state) {
            char name[256];
            transport_secure_address_to_string(&channel->secure_identifier, name, sizeof(name), false, false);
            channel->channel.allocate(8, name,
                observer->m_link, observer,
                static_cast<c_network_message_gateway *>(observer->m_message_gateway),
                static_cast<c_network_message_handler *>(observer->m_message_handler),
                static_cast<c_network_message_type_collection *>(observer->m_message_types),
                &g_network_configuration.channel_configuration);
            network_base_memory_globals.observer.update_observer_secure_state(observer_index);
            network_base_memory_globals.observer.update_observer_connection_state(observer_index);
            network_base_memory_globals.observer.update_observer_connection_initiation(observer_index);
        }
    }
}

static __forceinline void network_memory_publish_interfaces(
    c_network_session_parameter_type_collection **session_parameter_types, c_network_link **link,
    c_network_message_type_collection **message_types, c_network_message_gateway **gateway,
    c_network_message_handler **handler, c_network_observer **observer,
    c_network_session **sessions, c_network_session_manager **session_manager) {
    *link = &network_base_memory_globals.link;
    *message_types = &network_base_memory_globals.message_types;
    *gateway = &network_base_memory_globals.gateway;
    *handler = &network_base_memory_globals.handler;
    *observer = &network_base_memory_globals.observer;
    *sessions = network_base_memory_globals.sessions;
    *session_manager = &network_base_memory_globals.session_manager;
    *session_parameter_types = &network_base_memory_globals.session_parameter_types;
}

// @address 0x3b67a0 network_memory_initialize
extern "C" bool network_memory_initialize(c_network_link **, c_network_message_type_collection **, c_network_message_gateway **,
    c_network_message_handler **, c_network_observer **, c_network_session **, c_network_session_manager **,
    c_network_session_parameter_type_collection **) {
    network_memory_publish_interfaces(&network_life_cycle_parameter_storage(), &network_globals_link_storage(),
        &network_globals_message_types_storage(),
        &network_globals_gateway_storage(), &network_globals_handler_storage(), &network_life_cycle_observer_storage(),
        &network_life_cycle_session_storage(), &network_life_cycle_manager_storage());
    network_shared_memory_globals.heap_size = g_network_configuration.network_heap_large_size;
    long view_size = data_allocation_size(15, sizeof(view), 0);
    network_shared_memory_globals.views = static_cast<s_data_array *>(
        _physical_memory_malloc((memory_stage)1, 0, view_size, 0, 0, -1));
    network_memory_track_allocation(view_size, network_shared_memory_globals.views);
    data_initialize(network_shared_memory_globals.views, "simulation view", 15, sizeof(view), 0,
        &g_no_allocation_storage);
    data_make_valid(network_shared_memory_globals.views);
    network_shared_memory_globals.heap_memory = _physical_memory_malloc((memory_stage)1, 0,
        network_shared_memory_globals.heap_size, 0, 0, -1);
    network_memory_track_allocation(network_shared_memory_globals.heap_size,
        network_shared_memory_globals.heap_memory);
    network_shared_memory_globals.heap = new c_fixed_memory_rockall_heap(
        network_shared_memory_globals.heap_memory, network_shared_memory_globals.heap_size);
    network_memory_recreate_channels();
    return true;
}

