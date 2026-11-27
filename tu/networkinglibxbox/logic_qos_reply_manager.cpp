// networkinglibxbox/logic_qos_reply_manager.obj: from the records 0x941f0 0x94470 0x946b8 0x94768 0x3ecc50 0x3ecd28 0x3ecdd8 0x3ece18 0x3ece50 0x3ecf18 0x3ecf28 0x3ecf90
#include "networking/logic/network_search_types.h"
#include "networking/session/network_managed_session.h"
#include "cseries/crt_xdk.h"
#include "networking/online/online_nat_types.h"
#include <string.h>
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_bandwidth.h"
#include "networking/network_game_definitions.h"
#include "networking/online/online_xenon.h"
#include "networking/transport/transport_qos.h"
#include "networking/session/network_session.h"
#include "memory/bitstream.h"
#include "memory/data_types.h"
#include "networking/online/online_guide_xenon.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/transport/transport_security_types.h"
#include "networking/network_utilities.h"
#include "networking/transport/transport_platform_types.h"
#include "cseries/data_mining_event_types.h"
#include "networking/online/online_presence_accessors.h"
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
#include "networking/logic/logic_qos_reply_manager.h"
#include "networking/network_time_types.h"
#include "xbox/xnet_services.h"
#include "networking/logic/logic_qos_reply_globals.h"
#include "networking/transport/transport_qos_types.h"
#include "networking/session/network_session_interface_user_externs.h"
#include "networking/logic/network_bandwidth_globals.h"
#include "memory/data.h"
#include "networking/network_time_access.h"

// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x3ece50 ?desires_qos_reply_block_set@c_group_session_qos_reply_manager@@MAA_NXZ

namespace {
static __forceinline e_life_cycle_state logic_qos_reply_manager_life_cycle_get_state()
{
    e_life_cycle_state state = _life_cycle_state_none;
    if (online_presence_life_cycle_globals().initialized)
        state = online_presence_life_cycle_globals().manager.m_current_state;
    return state;
}
static __forceinline bool logic_qos_reply_manager_in_squad_session(c_network_session **session)
{
    bool result = false;
    if (online_presence_life_cycle_globals().initialized)
    {
        c_network_session *squad_session = online_presence_life_cycle_globals().manager.m_active_squad_session;
        if (!squad_session->disconnected())
        {
            if (session)
                *session = squad_session;
            result = true;
        }
    }
    return result;
}
static __forceinline bool logic_qos_reply_manager_in_group_session(bool initialized, c_network_session **session)
{
    bool result = false;
    if (initialized)
    {
        c_network_session *group_session = online_presence_life_cycle_globals().manager.m_group_session;
        if (!group_session->disconnected())
        {
            if (session)
                *session = group_session;
            result = true;
        }
    }
    return result;
}
static __forceinline bool network_life_cycle_desires_qos_reply_block_for_group(bool initialized)
{
    bool result = false;
    e_life_cycle_state state = _life_cycle_state_none;
    if (initialized)
        state = online_presence_life_cycle_globals().manager.m_current_state;
    if (state == _life_cycle_state_matchmaking_find_and_assemble_match || state == _life_cycle_state_matchmaking_assemble_match)
        result = true;
    return result;
}
static __forceinline void network_life_cycle_notify_qos_reply_block_set_for_group()
{
    e_life_cycle_state state = logic_qos_reply_manager_life_cycle_get_state();
    if (state == _life_cycle_state_matchmaking_find_and_assemble_match)
        online_presence_life_cycle_globals().find_and_gather.notify_qos_reply_block_set();
    else if (state == _life_cycle_state_matchmaking_assemble_match)
        online_presence_life_cycle_globals().assemble.notify_qos_reply_block_set();
}
}

// @address 0x5bec98 ?notify_qos_reply_block_set@c_session_qos_reply_manager@@MAAXXZ
// @address 0xfd4 ??_7c_squad_session_qos_reply_manager@@6B@
// @address 0x1c94 ??_7c_group_session_qos_reply_manager@@6B@

// @address 0x946b8 ?get_session_id@c_squad_session_qos_reply_manager@@MAA_NPAUs_transport_secure_identifier@@@Z
bool c_squad_session_qos_reply_manager::get_session_id(s_transport_secure_identifier *session_id)
{
    c_network_session *session = 0;
    bool result = false;
    if (logic_qos_reply_manager_in_squad_session(&session) && session->session_class() != 0)
    {
        s_transport_session_description description;
        result = ::managed_session_get_security_information(
            session->managed_session_index(), true, &description, 0);
        if (result)
            memcpy(session_id, &description, sizeof(*session_id));
    }
    return result;
}
// @address 0x94768 ?get_session_id@c_group_session_qos_reply_manager@@MAA_NPAUs_transport_secure_identifier@@@Z
bool c_group_session_qos_reply_manager::get_session_id(s_transport_secure_identifier *session_id)
{
    c_network_session *session = 0;
    bool result = false;
    if (logic_qos_reply_manager_in_group_session(online_presence_life_cycle_globals().initialized, &session) && session->session_class() != 0)
    {
        s_transport_session_description description;
        result = ::managed_session_get_security_information(
            session->managed_session_index(), true, &description, 0);
        if (result)
            memcpy(session_id, &description, sizeof(*session_id));
    }
    return result;
}
long c_squad_session_qos_reply_manager::get_qos_reply_bandwidth_limit()
{
    long bandwidth_limit = 0;
    e_life_cycle_state state = logic_qos_reply_manager_life_cycle_get_state();
    if (state == _life_cycle_state_in_game)
        bandwidth_limit = g_network_configuration.squad_pregame_qos_reply_bandwidth_limit;
    else
        bandwidth_limit = g_network_configuration.squad_qos_reply_bandwidth_limit;
    return bandwidth_limit;
}
// @address 0x3ecd28 ?desires_qos_reply_block_set@c_squad_session_qos_reply_manager@@MAA_NXZ
bool c_squad_session_qos_reply_manager::desires_qos_reply_block_set()
{
    c_network_session *session = 0;
    bool result = false;
    if (logic_qos_reply_manager_in_squad_session(&session) && session->established() && session->is_host())
        result = true;
    return result;
}
bool c_group_session_qos_reply_manager::desires_qos_reply_block_set()
{
    c_network_session *session = 0;
    bool initialized = online_presence_life_cycle_globals().initialized;
    bool result = false;
    if (logic_qos_reply_manager_in_group_session(initialized, &session) && session->established() && session->is_host())
        result = network_life_cycle_desires_qos_reply_block_for_group(initialized);
    return result;
}
long c_group_session_qos_reply_manager::get_qos_reply_bandwidth_limit()
{
    return g_network_configuration.group_qos_reply_bandwidth_limit;
}
void c_group_session_qos_reply_manager::notify_qos_reply_block_set()
{
    network_life_cycle_notify_qos_reply_block_set_for_group();
}

// @address 0x3ecc50 qos_reply_encode_status
extern "C" long qos_reply_encode_status(const s_network_squad_status_data *status_data, long buffer_size, unsigned char *buffer)
{
    long result = 0;
    c_bitstream packet;
    packet.m_data_size_alignment = 1;
    packet.set_data(buffer, 1020);
    packet.begin_writing();
    squad_status_encode(&packet, status_data);
    packet.finish_writing(0);
    long current_bit_position = packet.m_current.current_stream_bit_position;
    bool overflowed;
    if (current_bit_position > packet.m_data_size_bytes * 8)
        overflowed = true;
    else
        overflowed = false;
    if (!overflowed)
        result = (current_bit_position + packet.m_current.window_bits_used + 7) / 8;
    return result;
}

// @address 0x8c447c g_network_time_globals

// @address 0x94470 ?update@c_session_qos_reply_manager@@QAAXXZ
void c_session_qos_reply_manager::update()
{
    c_session_qos_reply_manager *self = this;
    s_transport_secure_identifier identifier;
    if (self->get_session_id(&identifier)) {
        s_network_configuration *config = &g_network_configuration;
        long last_update = self->last_update;
        if (!last_update || (long)(network_time_get() - last_update) >= config->qos_reply_update_interval) {
            bool reset = true;
            if (self->desires_qos_reply_block_set()) {
                s_network_squad_status_data status;
                unsigned char reply[1020];
                memset(&status, 0, sizeof(status));
                if (self->get_qos_status(&status)) {
                    if (memcmp(&status, &self->status, sizeof(status))) {
                        long size = qos_reply_encode_status(&status, sizeof(reply), reply);
                        if (size > 0) {
                            if (transport_qos_globals.initialized)
                                XNetQosListen(&identifier, reply, size, 0, 5);
                            long bandwidth_limit = self->get_qos_reply_bandwidth_limit();
                            if (transport_qos_globals.initialized)
                                XNetQosListen(&identifier, 0, 0, bandwidth_limit, 9);
                            self->notify_qos_reply_block_set();
                            memcpy(&self->status, &status, sizeof(status));
                            reset = false;
                        }
                    } else {
                        reset = false;
                    }
                }
            }
            if (reset) {
                if (transport_qos_globals.initialized)
                    XNetQosListen(&identifier, 0, 0, 0, 6);
                long bandwidth_limit = config->qos_reply_bandwidth_limit;
                if (transport_qos_globals.initialized)
                    XNetQosListen(&identifier, 0, 0, bandwidth_limit, 10);
                memset(&self->status, 0, sizeof(self->status));
            }
            self->last_update = network_time_get();
        }
    }
}

static __forceinline void logic_qos_reply_manager_status_initialize(s_network_squad_status_data *status)
{
    long compatible_version = k_network_compatible_version;
    status->build = compatible_version;
    status->protocol = 7;
    status->type = 5;
    status->flags = 0;
    status->version = 11855;
    status->state = 0;
}
// @address 0x3ece18 ?get_qos_status@c_squad_session_qos_reply_manager@@MAA_NPAUs_network_squad_status_data@@@Z
bool c_squad_session_qos_reply_manager::get_qos_status(s_network_squad_status_data *status)
{
    logic_qos_reply_manager_status_initialize(status);
    return network_squad_session_build_status(status);
}
static __forceinline bool logic_qos_reply_manager_gather_properties(s_matchmaking_gather_party_properties *properties)
{
    bool result = false;
    e_life_cycle_state state = logic_qos_reply_manager_life_cycle_get_state();
    switch (state)
    {
    case _life_cycle_state_matchmaking_find_and_assemble_match:
        result = online_presence_life_cycle_globals().find_and_gather.get_gather_party_properties(properties);
        break;
    case _life_cycle_state_matchmaking_assemble_match:
        result = online_presence_life_cycle_globals().assemble.get_gather_party_properties(properties);
        break;
    }
    return result;
}
// @address 0x3ecf90 ?get_qos_status@c_group_session_qos_reply_manager@@MAA_NPAUs_network_squad_status_data@@@Z
bool c_group_session_qos_reply_manager::get_qos_status(s_network_squad_status_data *status)
{
    bool success = false;
    c_network_session *session = 0;
    logic_qos_reply_manager_status_initialize(status);
    status->state = 2;
    if (logic_qos_reply_manager_in_group_session(online_presence_life_cycle_globals().initialized, &session))
    {
        if (logic_qos_reply_manager_gather_properties(&status->details.gather_party_properties))
            success = true;
    }
    return success;
}

static __forceinline s_transport_qos_globals *transport_qos_globals_get()
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x941f0: retain the shared bandwidth base; direct transport_qos_globals access grows update by 8 bytes.
    return reinterpret_cast<s_transport_qos_globals *>(&network_bandwidth_globals) - 1;
#else
    return &transport_qos_globals;
#endif
}

static __forceinline s_bandwidth_globals *bandwidth_globals_get()
{
    return &network_bandwidth_globals;
}
static __forceinline long transport_qos_service_new(void **qos)
{
    long result = -1;
    if (transport_qos_globals_get()->initialized)
    {
        if (XNetQosServiceLookup(0, 0, qos) == 0)
        {
            result = datum_new(transport_qos_globals_get()->data);
            if (result != -1)
            {
                s_transport_qos_attempt *datum = datum_get<s_transport_qos_attempt>(transport_qos_globals_get()->data, result);
                datum->active = 1;
                datum->qos = (s_transport_qos *)*qos;
            }
            else
            {
                XNetQosRelease(*qos);
            }
        }
    }
    return result;
}
// @address 0x941f0 ?update@c_live_service_qos_manager@@QAAXXZ
void c_live_service_qos_manager::update()
{
    c_live_service_qos_manager *self = this;
    void *qos;

    if (!self->flags.test(1) && !self->flags.test(0) && online_local_xuid_is_online_enabled())
    {
        unsigned long last_attempt = self->last_attempt;
        if (!last_attempt || (long)(network_time_get() - last_attempt) >= 2000)
        {
            self->qos_index = transport_qos_service_new(&qos);
            self->last_attempt = network_time_get();
            if (self->qos_index == -1)
                self->flags.set(2, true);
            else
                self->flags.set(0, true);
        }
    }
    if (self->flags.test(0))
    {
        long qos_index = self->qos_index;
        if (transport_qos_is_complete(qos_index))
        {
            if (transport_qos_get_result(qos_index, 0, &self->result))
            {
                s_network_configuration *config = &g_network_configuration;
                if (config->cap_live_service_upstream &&
                    self->result.bandwidth_upstream_bps * config->upstream_cap_multiplier / 100 > self->result.bandwidth_downstream_bps)
                    self->result.bandwidth_upstream_bps = self->result.bandwidth_downstream_bps / config->upstream_cap_divisor;
                online_get_nat_type();
                c_datamine event(0, "qos", 1, "networking");
                network_bandwidth_notify_live_service_qos_measurement(&self->result);
                online_presence_user_slot(0).quality_valid = true;
                memcpy(&online_presence_user_slot(0).quality, &self->result, sizeof(self->result));
                online_presence_user_slot(0).nat_flags_a = bandwidth_globals_get()->bandwidth;
                online_presence_user_slot(0).nat_flags_b = bandwidth_globals_get()->capacity;
                self->flags.set(1, true);
                self->flags.set(2, false);
            }
            else
            {
                self->flags.set(2, true);
            }
            transport_qos_delete(self->qos_index);
            self->qos_index = -1;
            self->flags.set(0, false);
        }
    }
}

typedef char qos_reply_manager_globals_size_check[(sizeof(void *) != 4 || sizeof(s_qos_reply_manager_globals) == 0x27e0) ? 1 : -1];

s_qos_reply_manager_globals g_qos_reply_manager_globals;

