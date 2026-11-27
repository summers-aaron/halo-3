// networkinglibxbox/network_observer.obj: from the records 0x1238a8 0x123978 0x125e10 0x126490 0x1264e8 0x1265d8 0x126758 0x3ca598 0x3d8dc0 0x3d8f98 0x3d91d0 0x3d9318 0x3d9388 0x3d9518 0x3d9568 0x3d9688 0x3d9bf8 0x3d9cd0 0x3d9d58 0x3d9e28 0x3d9ea0 0x3da2e8 0x3da388 0x3da470 0x3da878 0x3da928 0x3daab0 0x3dada0 0x3daf28 0x3db288 0x3db360 0x3db438 0x3db510 0x3db638 0x3db698 0x3db9b0 0x3dbbe0 0x3dbd08 0x3dc780 0x3dc918 0x3dc9f8
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
#include "xbox/xnet_services.h"
#include "networking/session/network_session_runtime_bindings.h"
#include "networking/logic/life_cycle/life_cycle_globals_access.h"
// @address 0xf11308 g_network_life_cycle_globals
#include "networking/delivery/network_channel_activity_types.h"
#include "networking/delivery/network_channel_state_types.h"
#include "networking/delivery/network_channel_closure_reason_types.h"
#include "networking/network_observer_owner_types.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "cseries/bit_operations.h"
#include "networking/messages/network_messages_connect.h"
#include "memory/data_types.h"
#include <stddef.h>
#include "networking/transport/transport_qos.h"
#include "networking/transport/transport_security.h"
#include "networking/session/network_managed_session.h"
#include "networking/logic/network_join.h"
#include "networking/transport/transport_address.h"
#include "networking/online/voice_xenon.h"
#include "networking/online/voice_lifecycle.h"
#include "cseries/pointer_utilities.h"
#include "networking/messages/network_message_type_collection.h"
#include "memory/sliding_window.h"
#include "networking/delivery/network_delivery_types.h"
#include "networking/transport/transport_security_types.h"
#include "networking/network_statistics.h"
#include "networking/observer_state_types.h"
#include "networking/in_addr_types.h"
#include "networking/online/online_files.h"
#include "networking/session/network_observer.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/online/voice_queue_types.h"
#include "networking/delivery/network_link.h"
struct s_transport_session_description;
enum e_transport_platform;
// @address 0x80608 GetTickCount
// @address 0x3a1190 ?transport_secure_connection_status@@YA?AW4e_transport_secure_connection@@PBUtransport_address@@@Z
// @address 0x80a6b8 ?g_network_join_data@@3Us_networking_join_data@@A
// @address 0x108eee8 ?g_voice_queue@@3Vc_voice_queue@@A
// @address 0x126490 network_observer_is_join_in_progress
// @address 0x1264e8 network_observer_is_peer_joining
// @address 0x1265d8 ?record_throughput_bandwidth@c_network_observer@@AAAXJJJJ@Z
// @address 0x3cfc80 ?is_peer_joining_this_session@c_network_session@@QBA_NXZ
// @address 0x43c960 ?add_event@c_network_time_statistics@@QAAXJ@Z
// @address 0x43cc78 ?add_event@c_network_window_statistics@@QAAXJ@Z
// @address 0x125e10 ?stream_check_stream_channel_connections@c_network_observer@@AAA_NPA_N@Z
// @address 0x126758 ?update_voice_repeater_use@c_network_observer@@AAAXXZ
// @address 0x3ca598 network_owner_entry_is_pending
// @address 0x3d91d0 ?quality_statistics_get_ratings@c_network_observer@@QAAXPAJ00@Z
// @address 0x3d9318 ?quality_statistics_notify_established_connectivity@c_network_observer@@AAAXJ_N@Z
// @address 0x3d9388 ?quality_statistics_report_badness@c_network_observer@@AAAXJ_NPBD@Z
// @address 0x3d9518 ?observer_channel_find_by_network_channel@c_network_observer@@QBAJW4e_network_observer_owner@@PAVc_network_channel@@@Z
// @address 0x3d9bf8 ?voice_data_available@c_network_observer@@SA_NJ_NPAJ@Z
// @address 0x3d9cd0 ?observer_channel_backlogged@c_network_observer@@QBA_NW4e_network_observer_owner@@JW4e_network_message_type@@@Z
// @address 0x3d9d58 ?observer_channel_currently_alive@c_network_observer@@QBA_NW4e_network_observer_owner@@J@Z
// @address 0x3da878 ?change_observer_state@c_network_observer@@AAAXJW4e_observer_state@@@Z
// @address 0x3da928 ?update_observer_timeout@c_network_observer@@AAAXJ@Z
// @address 0x3db510 ?discard_secure_connection@c_network_observer@@AAAXJ_NW4e_network_channel_closure_reason@@@Z
// @address 0x418588 ?close@c_network_channel@@QAAXW4e_network_channel_closure_reason@@@Z
// @address 0x3b5fa8 ??0c_network_channel@@QAA@XZ
// @address 0x126ac8 ?send_all_pending_messages@c_network_message_gateway@@QAAXXZ
// @address 0x3a15a0 ?transport_address_get_insecure_xbox_address@@YA_NPBUtransport_address@@PAUin_addr@@@Z
// @address 0x609a60 XNetUnregisterInAddr
// @address 0x3db288 ?packet_rate_get_maximum_given_bandwidth_bps@c_network_observer@@ABAMJ_N0@Z
// @address 0x3db438 ?build_secure_connection@c_network_observer@@ABA_NW4e_network_observer_owner@@_NPBUs_transport_secure_address@@PAUtransport_address@@@Z
// @address 0x3db698 ?handle_connect_request@c_network_observer@@QAAXPBUtransport_address@@PBUs_network_message_connect_request@@@Z
// @address 0x418148 ?establish@c_network_channel@@QAAXK@Z
// @address 0x417fc0 ?open@c_network_channel@@QAAXPBUtransport_address@@_NK@Z
// @address 0x3d9ea0 ?allocate_observer@c_network_observer@@AAAJW4e_network_observer_owner@@PBUs_transport_secure_address@@@Z
// @address 0x3a0fd8 ?transport_secure_address_compare@@YA_NPBUs_transport_secure_address@@0@Z
// @address 0x3db9b0 ?handle_packet_event@c_network_observer@@QAAXPAVc_network_channel@@W4e_network_packet_event@@JJJ@Z
// @address 0x3dbbe0 network_observer_initialize_stream_bandwidth
// @address 0x3dbd08 network_observer_update_stream_bandwidth
// @address 0x3dc780 ?stream_initialize@c_network_observer@@AAAXJ@Z
// @address 0x3dc918 ?stream_set_bandwidth@c_network_observer@@AAAXJJ@Z
// @address 0x3dc9f8 ?bandwidth_monitor_events@c_network_observer@@AAAXJJJ@Z
// @address 0x3d9568 ?observer_channel_send_message@c_network_observer@@QAAXW4e_network_observer_owner@@J_NW4e_network_message_type@@JPBX@Z
// @address 0x3ee8a8 ?send_message_directed@c_network_message_gateway@@QAA_NPBUtransport_address@@W4e_network_message_type@@JPBX@Z
// @address 0x415e18 ?send_message@c_network_message_queue@@QAAXW4e_network_message_type@@JPBX@Z
// @address 0x41ee88 ?encode_peer_voice_stream@c_voice_queue@@QAAJJPAEJ@Z
// @address 0x3d8da0 ?established@c_network_channel@@QBA_NXZ
// @address 0x41ebe8 ?get_peer_voice_stream_size@c_voice_queue@@QAAJJ@Z
// @address 0x41eae0 ?has_packets_to_transmit@c_voice_queue@@QAA_NJ@Z
// @address 0x3a0e60 ?transport_secure_address_decode@@YA_NPBUs_transport_session_description@@PBUs_transport_secure_address@@GPAUtransport_address@@@Z
// @address 0x3a1138 ?transport_secure_connection_initiate@@YA_NPBUtransport_address@@@Z
#include "cseries/cseries_types.h"
#include <stdlib.h>
#include <string.h>
#include <ppcintrinsics.h>
#include "networking/session/network_session.h"
// @address 0x3d8f98 ?reset_bandwidth@c_network_observer@@QAAXXZ

#include "networking/delivery/network_channel.h"

#include "networking/network_configuration_types.h"
typedef char observer_owner_offset_1[(offsetof(s_network_session_peer, properties.host_badness) == 0xcc) ? 1 : -1];
typedef char observer_owner_offset_2[(offsetof(s_network_session_peer, user_player_indices) == 0x104) ? 1 : -1];
typedef char observer_owner_offset_3[
    (offsetof(c_network_session_membership, m_current_membership.players) +
        offsetof(s_network_session_player, configuration.client.player_flags) == 0x1234) ? 1 : -1];
#include "networking/online/voice_interface_types.h"
#include "networking/network_time_types.h"
extern "C" {
// @address 0x8c447c g_network_time_globals
}
struct s_voice_interface_globals;
struct s_network_configuration;
#include "networking/network_configuration.h"

#include "networking/logic/life_cycle/life_cycle_globals_types.h"

#include "networking/logic/network_join_queue_types.h"

enum e_voice_repeater_usage { _voice_repeater_usage_none, _voice_repeater_usage_one, _voice_repeater_usage_two };


class c_network_observer_owner
{
public:
    virtual bool desire_channel_heartbeat(long observer_channel_index) const = 0;
    virtual long get_stream_priority(long observer_index) = 0;
    virtual bool should_initiate_connection(long observer_index, bool immediate) = 0;
    virtual void notify_observer_connection(long observer_index, unsigned long connection_identifier, bool connected) = 0;
    virtual void observer_state_dead(long observer_index) = 0;
};
#include "networking/session/network_observer_configuration.h"

#include "networking/transport/transport_qos_target_types.h"

#include "networking/transport/transport_qos_types.h"
#include "networking/messages/network_message_gateway.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/online/online_managed_session_types.h"
#include "networking/transport/transport_qos_globals.h"
#include "networking/session/network_quality.h"
#include "networking/logic/network_session_interface.h"
#include "networking/online/online_file_manager.h"
#include "networking/delivery/network_observer_globals.h"
enum
{
    _observer_qos_attempted_bit = 3,
    _observer_qos_succeeded_bit = 4,
};

static __forceinline c_network_session *network_observer_life_cycle_get_squad_session(void)
{
  c_network_session *result = 0;
  if (g_network_life_cycle_globals.initialized)
    result = g_network_life_cycle_globals.manager.m_active_squad_session;
  return result;
}

static __forceinline c_network_session *network_observer_life_cycle_get_group_session(void)
{
  c_network_session *result = 0;
  if (g_network_life_cycle_globals.initialized)
    result = g_network_life_cycle_globals.manager.m_group_session;
  return result;
}

static __forceinline c_network_session *network_observer_life_cycle_get_session_5c(void)
{
  c_network_session *result = 0;
  if (g_network_life_cycle_globals.initialized)
    result = g_network_life_cycle_globals.manager.m_target_squad_session;
  return result;
}

static __forceinline bool network_observer_session_established(const c_network_session *session)
{
  return (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
}

static __forceinline e_network_join_queue_mode network_observer_join_get_join_queue_mode(void)
{
  return g_network_join_data.join_queue_mode;
}

static __forceinline long network_observer_join_number_of_joins_in_queue(void)
{
  return g_network_join_data.join_queue_entry_count;
}

// 0x126490
bool network_observer_is_join_in_progress(void)
{
  bool join_in_progress = network_observer_is_peer_joining();
  if (!join_in_progress)
  {
    e_network_join_queue_mode join_queue_mode = network_observer_join_get_join_queue_mode();
    if ((join_queue_mode == _network_join_open_to_join_squad || join_queue_mode == _network_join_process_queued_group_joins) &&
        network_observer_join_number_of_joins_in_queue() != 0)
    {
      join_in_progress = true;
    }
  }
  return join_in_progress;
}

// 0x1264e8
bool network_observer_is_peer_joining(void)
{
  c_network_session *squad_session = network_observer_life_cycle_get_squad_session();
  c_network_session *group_session = network_observer_life_cycle_get_group_session();
  c_network_session *session_5c = network_observer_life_cycle_get_session_5c();
  return !session_5c->disconnected()
      || (!group_session->disconnected() && (!network_observer_session_established(group_session) || group_session->is_peer_joining_this_session()))
      || (!squad_session->disconnected() && squad_session->is_peer_joining_this_session());
}

class c_network_link;
class c_network_observer;
class c_network_message_gateway;
class c_network_message_handler;
class c_network_message_type_collection;
class c_network_channel;
struct transport_address;
inline float c_network_time_statistics::average_values_per_second()
{
    check_interval_rollover();
    return (float)m_stored_total.total_values * m_period_reciprocal;
}
static __forceinline void network_observer_statistics_reset(c_network_time_statistics *statistics)
{
    statistics->m_total_events = 0;
    statistics->m_total_values = 0;
    statistics->m_interval_start = 0;
    memset(&statistics->m_current, 0, sizeof(statistics->m_current));
    statistics->m_next_interval_index = 0;
    memset(statistics->m_intervals, 0, sizeof(statistics->m_intervals));
    memset(&statistics->m_stored_total, 0, sizeof(statistics->m_stored_total));
}
static __forceinline void network_observer_statistics_initialize(c_network_time_statistics *statistics, long period_duration_msec)
{
    statistics->m_period_duration_msec = period_duration_msec;
    statistics->m_interval_duration_msec = statistics->m_period_duration_msec / 20;
    statistics->m_period_reciprocal = 1000.0f / (float)statistics->m_period_duration_msec;
    network_observer_statistics_reset(statistics);
}

namespace
{
    static __forceinline unsigned long network_observer_time_get()
    {
        if (g_network_time_globals.enabled)
            return g_network_time_globals.time;
        return GetTickCount();
    }
    static __forceinline long network_observer_time_since(unsigned long time)
    {
        return (long)(network_observer_time_get() - time);
    }
    static __forceinline void network_observer_record_badness(unsigned __int64 *history, bool bad)
    {
        for (long i = 1; i > 0; --i)
            history[i] = (history[i] << 1) | (history[i - 1] >> 63);
        history[0] <<= 1;
        if (bad)
            history[0] |= 1;
    }
    static __forceinline void network_observer_voice_set_repeater_use(e_voice_repeater_usage usage)
    {
        if (g_voice_interface_globals.initialized)
            g_voice_interface_globals.repeater_usage = usage;
    }
}

struct s_network_message_connect_request;

extern "C" {
// match: 0x609a60: SDK header adoption is blocked by this unit's current types/API arguments: 'in_addr' : 'struct' type redefinition.

}

static __forceinline void network_observer_secure_connection_terminate(const transport_address *address)
{
    in_addr insecure_address;
    if (transport_address_get_insecure_xbox_address(address, &insecure_address))
    {
        e_transport_secure_connection status = transport_secure_connection_status(address);
        if (status == _transport_secure_connection_pending || status == _transport_secure_connection_connected)
            XNetUnregisterInAddr(insecure_address.s_addr);
    }
}

class c_network_observer;
class c_network_link;
class c_network_message_type_collection;
class c_network_message_handler;

__forceinline void c_network_observer::stream_dispose(long observer_index, bool disconnected, const char *reason)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        if (disconnected && !g_network_configuration.observer.disable_disconnection_badness &&
            observer->voice_active && observer->voice_talker)
            quality_statistics_report_badness(observer_index, true, reason);
        if (observer->voice_talker)
            network_observer_bandwidth_monitor_stop(observer_index);
        observer->voice_active = false;
    }


static __forceinline const s_online_managed_session *network_observer_managed_session_get(long session_index)
{
  return &online_session_manager_globals.sessions[session_index];
}

namespace
{
  static inline bool network_observer_session_class_is_squad_established(long session_index)
  {
    bool established = false;
    if (network_observer_managed_session_get(session_index)->session_class == 2)
      established = true;
    return established;
  }
  static inline bool network_observer_session_is_squad(long session_index)
  {
    bool squad = false;
    bool squad_bit = (network_observer_managed_session_get(session_index)->flags >> 1) & 1;
    if (squad_bit)
      squad = true;
    return squad;
  }
  static inline void network_observer_voice_set_outgoing_channel_count(long channel_count, long voice_channel_count)
  {
    if (g_voice_interface_globals.initialized)
    {
      g_voice_interface_globals.game_channel_count = channel_count;
      g_voice_interface_globals.lobby_channel_count = voice_channel_count;
    }
  }
}

// global random seed

inline long c_network_observer::stream_get_current_latency_threshold(long observer_index, bool probing) const
{
    const s_channel_observer *observer = &m_channel_observers[observer_index];
    long overhead;
    if (!observer->voice_talker)
        overhead = m_configuration->stream_latency_overhead;
    else
        overhead = probing ? m_configuration->stream_latency_overhead_probe : m_configuration->stream_latency_overhead_host;
    return overhead;
}

inline void c_network_observer::stream_backoff_update(long observer_index, long backoff_tolerance_bps, long maximum_backoff_level, long backoff_time)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (abs(observer->stream_bandwidth_bps - observer->stream_maximum_bandwidth_bps) > backoff_tolerance_bps)
        observer->backoff_level = 1;
    else
        observer->backoff_level = observer->backoff_level + 2 > maximum_backoff_level ? maximum_backoff_level : observer->backoff_level + 2;
    observer->backoff_time = network_observer_time_get();
    observer->backoff_duration = backoff_time * observer->backoff_level;
    observer->stream_maximum_bandwidth_bps = observer->stream_bandwidth_bps;
    observer->failed_probes = 1;
    observer->stream_minimum_bandwidth_bps = observer->stream_bandwidth_bps;
    stream_set_bandwidth(observer_index, observer->stream_minimum_bandwidth_bps);
}

static inline long network_observer_overprobe_pin(long n, long floor, long ceiling)
{
    n = MAX(floor, n);
    n = ceiling > n ? n : ceiling;
    return n;
}

static inline long network_observer_stream_clamp_growth_shift(long probes, long shift, long maximum)
{
    long result = probes >> shift;
    if (result > maximum)
        result = maximum;
    return result;
}

inline void c_network_observer::stream_probe_overhead(long observer_index, long *overhead) const
{
    if (!m_channel_observers[observer_index].voice_talker)
        *overhead = m_configuration->stream_latency_overhead;
    else
        *overhead = m_configuration->stream_latency_overhead_probe;
}

// 0x123978
void c_network_observer::stream_update()
{
    bool channels_changed = false;
    bool thresholds_unknown = false;
    bool reset_successful_probes = false;
    bool voice_active = false;
    bool any_high_priority = false;
    bool any_low_priority = false;
    unsigned long active_streams = 0;
    unsigned long congested_streams = 0;
    unsigned long streams_wants_to_grow = 0;
    unsigned long streams_requiring_bandwidth_mask = 0;
    unsigned long streams_probeable = 0;
    unsigned long streams_backed_off = 0;
    unsigned long streams_unstable = 0;
    unsigned long streams_failed = 0;
    long observer_index;
    unsigned long observer_mask = 1;
    long stream_count = 0;
    long saturated_count = 0;
    long host_count = 0;
    long client_count = 0;
    long requiring_count = 0;
    long probeable_count = 0;
    long total_bandwidth_bps = 0;
    long stable_bandwidth_bps = 0;
    long total_sent_bps = 0;
    unsigned long streams_to_probe = 0;
    bool probe_all = false;

    long backoff_failures = m_configuration->stream_backoff_failures;
    long stream_period = m_configuration->stream_period;
    long minimum_deviation = m_configuration->stream_minimum_deviation;
    long growth_threshold_bps = m_configuration->stream_growth_threshold_bps;
    long growth_minimum_bps = m_configuration->stream_growth_minimum_bps;
    long backoff_time = m_configuration->stream_backoff_time;
    long loss_average_shift = m_configuration->stream_loss_average_shift;
    long loss_deviation_shift = m_configuration->stream_loss_deviation_shift;
    long latency_threshold_base = m_configuration->stream_latency_threshold_base;
    long loss_threshold_base = m_configuration->stream_loss_threshold_base;
    long loss_minimum_deviation = m_configuration->stream_loss_minimum_deviation;
    long loss_minimum_spread = m_configuration->stream_loss_minimum_spread;
    long backoff_tolerance_bps = m_configuration->stream_backoff_tolerance_bps;
    long maximum_backoff_level = m_configuration->stream_maximum_backoff_level;

    long growth_base_bps = m_configuration->stream_parameters[0].growth_base_bps;
    long growth_shift = m_configuration->stream_parameters[0].growth_shift;
    long maximum_growth_shift = m_configuration->stream_parameters[0].maximum_growth_shift;
    long probe_time = m_configuration->stream_parameters[0].probe_time;
    long recover_time = m_configuration->stream_parameters[0].recover_time;
    long settle_time = m_configuration->stream_parameters[0].settle_time;
    float settle_scale = m_configuration->stream_parameters[0].settle_scale;
    long backoff_bps = m_configuration->stream_parameters[0].backoff_bps;
    long stable_time = m_configuration->stream_parameters[0].stable_time;
    long overprobe_minimum_bps = m_configuration->stream_parameters[0].overprobe_minimum_bps;
    long overprobe_maximum_bps = m_configuration->stream_parameters[0].overprobe_maximum_bps;
    long overprobe_percentage = m_configuration->stream_parameters[0].overprobe_percentage;

    long measurement_elapsed = network_observer_time_since(m_time_of_bandwidth_measurement);
    bool measure_bandwidth = measurement_elapsed >= m_configuration->stream_bandwidth_interval;
    bool bandwidth_changed = m_stream_bandwidth_changed;
    m_stream_bandwidth_changed = false;
    if (measure_bandwidth)
        m_time_of_bandwidth_measurement = network_observer_time_get();

    bool period_elapsed = network_observer_time_since(m_time_of_period) >= stream_period;
    if (period_elapsed)
        m_time_of_period = network_observer_time_get();

    if (stream_check_stream_channel_connections(&channels_changed))
    {
        measure_bandwidth = true;
        bandwidth_changed = true;
        m_stream_settle = true;
        m_stream_rebalance = true;
    }

    if (network_observer_is_join_in_progress())
        m_stream_settle = true;

    if (period_elapsed)
        m_stream_stable = true;

    for (observer_index = 0; observer_index < 32; observer_mask <<= 1, observer_index++)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        if (observer->state != _observer_state_none && observer->voice_active)
        {
            if ((observer->voice_talker || observer->stream_packet_rate_limited) &&
                (observer->backoff_time == 0 || network_observer_time_since(observer->backoff_time) >= observer->backoff_duration))
            {
                long desired_bps;
                if (observer->stream_packet_rate_halved)
                    desired_bps = m_configuration->stream_desired_high_priority_bps;
                else if (observer->voice_listener)
                    desired_bps = m_configuration->stream_desired_low_priority_bps;
                else
                    desired_bps = m_configuration->stream_desired_bps;

                if (observer->stream_minimum_bandwidth_bps < desired_bps)
                {
                    observer->stream_minimum_bandwidth_bps = desired_bps;
                    observer->backoff_time = network_observer_time_get();
                    if (observer->backoff_duration == 0)
                        observer->backoff_duration = backoff_time;
                    m_stream_rebalance = true;
                }
            }

            if (observer->stream_timeout_disabled && observer->stream_voice_active)
                voice_active = true;
            if (observer->voice_listener)
                any_low_priority = true;
            if (observer->stream_packet_rate_halved)
                any_high_priority = true;

            stream_count++;
            active_streams |= observer_mask;

            if (stable_bandwidth_bps != -1)
            {
                if (observer->stream_maximum_bandwidth_bps > m_configuration->stream_maximum_bandwidth_bps)
                    stable_bandwidth_bps = -1;
                else
                    stable_bandwidth_bps += observer->stream_maximum_bandwidth_bps;
            }

            observer->stream_requires_more_bandwidth = observer->voice_talker || observer->stream_packet_rate_limited && m_stream_state != 0;
            if (observer->voice_talker)
                host_count++;
            else if (observer->stream_packet_rate_limited)
                client_count++;

            long throughput_bps = (long)(observer->throughput_statistics.average_values_per_second() * 8.0f);
            total_bandwidth_bps += throughput_bps;

            bool wants_to_grow;
            if (observer->stream_requires_more_bandwidth)
                wants_to_grow = true;
            else if (observer->stream_packet_rate_limited)
                wants_to_grow = observer->stream_bandwidth_bps < observer->stream_minimum_bandwidth_bps;
            else if (throughput_bps > growth_threshold_bps)
                wants_to_grow = throughput_bps >= observer->stream_bandwidth_bps - growth_threshold_bps;
            else
                wants_to_grow = throughput_bps >= observer->stream_bandwidth_bps - growth_minimum_bps;

            if (!observer->stream_probed &&
                observer->stream_bandwidth_bps < observer->stream_minimum_bandwidth_bps &&
                observer->stream_requires_more_bandwidth)
            {
                streams_requiring_bandwidth_mask |= observer_mask;
                requiring_count++;
            }

            if (wants_to_grow && observer->stream_bandwidth_bps < observer->stream_minimum_bandwidth_bps)
            {
                streams_wants_to_grow |= observer_mask;
                observer->stream_wants_to_grow = true;
                if (observer->stream_probed == 1 && (observer->failed_probes == 0 || m_configuration->stream_ignore_failures))
                {
                    streams_probeable |= observer_mask;
                    probeable_count++;
                }
                if (observer->latency_threshold == -1 || observer->loss_threshold == -1)
                    thresholds_unknown = true;
            }
            else
            {
                observer->stream_wants_to_grow = false;
            }

            if (observer->failed_probes != 0)
                streams_failed |= observer_mask;

            if (observer->stream_minimum_bandwidth_bps != m_configuration->stream_minimum_bandwidth_bps &&
                observer->stream_maximum_bandwidth_bps > m_configuration->stream_maximum_bandwidth_bps)
            {
                streams_unstable |= observer_mask;
            }

            if (period_elapsed)
            {
                if (observer->previous_interval_latency != 0 &&
                    observer->latency_average_maximum <= observer->previous_interval_latency)
                {
                    m_stream_unstable_mask &= ~observer_mask;
                }

                if (observer->latency_deviation_maximum != -1)
                    observer->latency_idle_deviation += (observer->latency_deviation_maximum - observer->latency_idle_deviation) >>
                        m_configuration->stream_latency_average_shift;

                do
                {

                    long packets = observer->packets_received;
                    packets += observer->packets_lost;
                    long packet_loss = -1;
                    if (packets > 0)
                        packet_loss = observer->packets_lost * 100 / packets;

                    if (packet_loss == -1)
                        break;

                    if (m_configuration->stream_packet_loss_percentage != 0 && packet_loss >= m_configuration->stream_packet_loss_percentage)
                    {
                        if (packets >= m_configuration->stream_packet_loss_minimum_packets)
                            observer->packet_loss_events++;
                        if (packets >= m_configuration->stream_packet_loss_minimum_packets || m_configuration->stream_packet_loss_ignore != 0)
                            break;
                    }

                    long loss_delta = packet_loss - observer->loss_average;
                    observer->loss_accumulator += loss_delta;
                    observer->loss_average = observer->loss_accumulator >> loss_average_shift;
                    observer->loss_deviation += (abs(loss_delta) - observer->loss_deviation) >> loss_deviation_shift;
                    observer->loss_deviation = MAX(loss_minimum_spread, observer->loss_deviation);

                    if (observer->loss_average < observer->period_loss_minimum)
                        observer->period_loss_minimum = observer->loss_average;
                    if (observer->loss_average > observer->period_loss_maximum)
                        observer->period_loss_maximum = observer->loss_average;

                    if (observer->baseline_loss != -1 && abs(observer->loss_average - observer->baseline_loss) > observer->loss_deviation)
                        m_stream_stable = false;
                } while (false);

                long interval_total = observer->interval_counts[0] + observer->interval_counts[1] + observer->interval_counts[2];
                if (interval_total > 0)
                {
                    observer->interval_percentages[0] = observer->interval_counts[0] * 100 / interval_total;
                    observer->interval_percentages[1] = observer->interval_counts[1] * 100 / interval_total;
                    observer->interval_percentages[2] = observer->interval_counts[2] * 100 / interval_total;
                }
                else
                {
                    observer->interval_percentages[0] = 0;
                    observer->interval_percentages[1] = 0;
                    observer->interval_percentages[2] = 0;
                }

                observer->packets_received = 0;
                observer->packets_lost = 0;
                observer->previous_interval_latency = observer->latency_average_maximum;
                observer->latency_deviation_maximum = observer->latency_deviation;
                observer->baseline_loss = observer->loss_average;
                observer->interval_counts[0] = 0;
                observer->interval_counts[1] = 0;
                observer->interval_counts[2] = 0;
                observer->baseline_latency = observer->latency_average;
                observer->latency_average_maximum = observer->latency_average;
            }

            if (observer->baseline_latency != -1)
            {
                long latency_change = abs(observer->latency_average - observer->baseline_latency);
                if (latency_change > m_configuration->stream_latency_spike_percentage * MAX(minimum_deviation, observer->latency_idle_deviation) / 100)
                    m_stream_stable = false;
            }

            if (measure_bandwidth)
            {
                if (!bandwidth_changed)
                {
                    long received_bps = 0;
                    long sent_bps = 0;
                    if (measurement_elapsed > 0)
                    {
                        sent_bps = observer->period_bytes_sent * 8000 / measurement_elapsed;
                        received_bps = observer->period_bytes_received * 8000 / measurement_elapsed;
                        total_sent_bps += sent_bps;
                        if (observer->voice_talker)
                            bandwidth_monitor_events(observer_index, received_bps, sent_bps);
                        if (sent_bps > observer->stream_bandwidth_bps)
                            sent_bps = observer->stream_bandwidth_bps;
                    }

                    bool saturated = !observer->stream_saturated || sent_bps >= m_configuration->stream_saturated_bps;
                    if (saturated && !observer->stream_packet_rate_limiting)
                        saturated_count++;

                    if (observer->latency_threshold == -1 ||
                        observer->period_latency_maximum - observer->period_latency_minimum <= MAX(observer->latency_idle_deviation, observer->latency_drift))
                    {
                        long latency_overhead = stream_get_current_latency_threshold(observer_index, false);
                        long latency_threshold = (observer->latency_average + latency_overhead +
                            MAX(observer->latency_idle_deviation, observer->latency_drift) + latency_threshold_base) *
                            m_configuration->stream_latency_threshold_percentage / 100;
                        if (observer->latency_threshold == -1 || !observer->latency_threshold_valid ||
                            latency_threshold < observer->latency_threshold)
                        {
                            observer->latency_threshold_valid = true;
                            observer->latency_threshold = latency_threshold;
                        }
                    }

                    observer->stream_received_bps = received_bps;
                    observer->stream_sent_bps = sent_bps;
                }

                if (observer->loss_threshold == -1 || observer->best_loss_bandwidth_bps == -1 ||
                    observer->period_loss_maximum - observer->period_loss_minimum <= observer->loss_deviation)
                {
                    long previous_loss_threshold = observer->loss_threshold;
                    long loss_deviation = MAX(loss_minimum_deviation, observer->loss_deviation);
                    long current_loss_average = observer->loss_average;
                    long loss_threshold = current_loss_average +
                        current_loss_average * m_configuration->stream_loss_average_percentage / 100 +
                        loss_deviation * m_configuration->stream_loss_deviation_percentage / 100 + loss_threshold_base;

                    if (observer->loss_threshold == -1 || observer->best_loss_bandwidth_bps == -1 ||
                        loss_threshold - observer->best_loss_deviation < observer->best_loss_threshold)
                    {
                        observer->loss_threshold = loss_threshold;
                    }

                    if (observer->best_loss_bandwidth_bps != -1 && loss_threshold >= observer->best_loss_threshold)
                    {
                        if (loss_threshold - observer->best_loss_deviation <= previous_loss_threshold &&
                            observer->best_loss_bandwidth_bps < observer->stream_bandwidth_bps)
                        {
                            observer->best_loss_bandwidth_bps = observer->stream_bandwidth_bps;
                        }
                    }
                    else
                    {
                        observer->best_loss_threshold = loss_threshold;
                        observer->best_loss_deviation = loss_deviation;
                        observer->best_loss_bandwidth_bps = observer->stream_bandwidth_bps;
                    }
                }

                observer->period_bytes_received = 0;
                observer->period_bytes_sent = 0;
                observer->period_latency_minimum = 9999;
                observer->period_latency_maximum = 0;
                observer->period_start_latency = observer->latency_average;
                observer->period_loss_minimum = 100;
                observer->period_loss_maximum = 0;
            }

            if (observer->voice_talker || observer->stream_packet_rate_limited)
            {
                if (observer->latency_threshold != -1 && observer->latency_average > observer->latency_threshold)
                {
                    congested_streams |= observer_mask;
                }
                else if (observer->voice_talker && m_configuration->stream_congestion_maximum_latency != 0 &&
                    observer->latency_average >= m_configuration->stream_congestion_maximum_latency)
                {
                    long packet_rate = (long)observer->stream_packet_rate;
                    if (packet_rate >= m_configuration->stream_congestion_minimum_rate)
                        congested_streams |= observer_mask;
                }

                if (observer->loss_threshold != -1 && observer->loss_average > observer->loss_threshold)
                    congested_streams |= observer_mask;
            }
        }
    }

    m_stream_has_host = host_count != 0;

    if (measure_bandwidth && !bandwidth_changed)
        record_throughput_bandwidth(total_sent_bps, stream_count, saturated_count, measurement_elapsed);

    if (m_stream_state == 0 && total_bandwidth_bps > m_stream_peak_bandwidth_bps)
        m_stream_peak_bandwidth_bps = total_bandwidth_bps;

    bool throttle_expired = true;
    if (m_time_of_throttle != -1)
    {
        if (!any_high_priority)
        {
            m_time_of_throttle = -1;
        }
        else if (m_throttle_duration == -1)
        {
            throttle_expired = false;
            long throttle_duration = m_configuration->stream_throttle_time_per_stream * (stream_count - 1) + m_configuration->stream_throttle_base_time;
            m_throttle_duration = throttle_duration +
                random_range(get_local_random_seed_address(), -m_configuration->stream_throttle_random_percentage,
                    m_configuration->stream_throttle_random_percentage) *
                    throttle_duration / 100;
        }
        else if (network_observer_time_since(m_time_of_throttle) < m_throttle_duration)
        {
            throttle_expired = false;
        }
        else
        {
            m_time_of_throttle = -1;
            m_throttle_duration = -1;
        }
    }

    if (m_configuration->stream_exclude_probed)
    {
        unsigned long unprobed_streams = streams_wants_to_grow & ~m_stream_probed_mask;
        if (unprobed_streams == 0)
        {
            m_stream_probed_mask = 0;
        }
        else
        {
            streams_wants_to_grow = unprobed_streams & ~streams_probeable;
            if (streams_wants_to_grow == 0)
                streams_wants_to_grow = unprobed_streams;
        }
    }

    long expansion_state = m_stream_expansion_state;
    if (m_stream_state == 0)
    {
        m_stream_settle = false;
        if (congested_streams != 0)
        {
            m_stream_state = 5;
            m_time_of_probe = -1;
        }
        else if (!m_stream_rebalance && network_observer_time_since(m_time_of_expansion) < backoff_time)
        {
            if (thresholds_unknown)
            {
                expansion_state = 0;
            }
            else if (network_observer_is_join_in_progress())
            {
                expansion_state = 0;
                m_stream_settle = true;
            }
            else if (!m_stream_stable)
            {
                expansion_state = 0;
            }
            else if (streams_wants_to_grow == 0)
            {
                expansion_state = 0;
            }
            else if (!throttle_expired)
            {
                expansion_state = 0;
            }
            else if (streams_requiring_bandwidth_mask != 0)
            {
                expansion_state = 1;
            }
            else if (streams_probeable != 0 && probeable_count > 1 && (streams_wants_to_grow & ~streams_probeable) == 0)
            {
                expansion_state = 2;
                if (m_stream_expansion_state != 2)
                    reset_successful_probes = true;
            }
            else
            {
                expansion_state = 3;
            }
        }
        else
        {
            m_stream_rebalance = false;
            expansion_state = 4;
            m_time_of_expansion = network_observer_time_get();
        }
    }

    switch (m_stream_expansion_state)
    {
    case 0:
        if (m_stream_state == 0 && expansion_state != 0)
            m_stream_expansion_state = expansion_state;
        break;
    case 1:
        if (m_stream_state == 0)
        {
            if (expansion_state != 1)
            {
                m_stream_expansion_state = expansion_state;
            }
            else
            {
                m_stream_state = 1;
                m_time_of_probe = -1;
                streams_to_probe = streams_requiring_bandwidth_mask;
                streams_probeable = streams_requiring_bandwidth_mask;
                probe_all = (streams_failed & streams_requiring_bandwidth_mask) == 0;
            }
        }
        growth_base_bps = m_configuration->stream_parameters[1].growth_base_bps;
        growth_shift = m_configuration->stream_parameters[1].growth_shift;
        maximum_growth_shift = m_configuration->stream_parameters[1].maximum_growth_shift;
        probe_time = m_configuration->stream_parameters[1].probe_time;
        recover_time = m_configuration->stream_parameters[1].recover_time;
        settle_time = m_configuration->stream_parameters[1].settle_time;
        settle_scale = m_configuration->stream_parameters[1].settle_scale;
        backoff_bps = m_configuration->stream_parameters[1].backoff_bps;
        stable_time = m_configuration->stream_parameters[1].stable_time;
        overprobe_minimum_bps = m_configuration->stream_parameters[1].overprobe_minimum_bps;
        overprobe_maximum_bps = m_configuration->stream_parameters[1].overprobe_maximum_bps;
        overprobe_percentage = m_configuration->stream_parameters[1].overprobe_percentage;
        break;
    case 2:
        if (m_stream_state == 5)
        {
            m_stream_expansion_state = 3;
        }
        else if (m_stream_state == 0)
        {
            if (expansion_state != 2)
            {
                m_stream_expansion_state = expansion_state;
            }
            else
            {
                m_time_of_probe = -1;
                streams_to_probe = streams_wants_to_grow;
                probe_all = true;
                m_stream_state = 1;
            }
        }
        growth_base_bps = m_configuration->stream_parameters[2].growth_base_bps;
        growth_shift = m_configuration->stream_parameters[2].growth_shift;
        maximum_growth_shift = m_configuration->stream_parameters[2].maximum_growth_shift;
        probe_time = m_configuration->stream_parameters[2].probe_time;
        recover_time = m_configuration->stream_parameters[2].recover_time;
        settle_time = m_configuration->stream_parameters[2].settle_time;
        settle_scale = m_configuration->stream_parameters[2].settle_scale;
        backoff_bps = m_configuration->stream_parameters[2].backoff_bps;
        stable_time = m_configuration->stream_parameters[2].stable_time;
        overprobe_minimum_bps = m_configuration->stream_parameters[2].overprobe_minimum_bps;
        overprobe_maximum_bps = m_configuration->stream_parameters[2].overprobe_maximum_bps;
        overprobe_percentage = m_configuration->stream_parameters[2].overprobe_percentage;
        break;
    case 3:
        if (m_stream_state == 0)
        {
            if (expansion_state != 3)
            {
                m_stream_expansion_state = expansion_state;
            }
            else
            {
                m_time_of_probe = -1;
                streams_to_probe = streams_wants_to_grow;
                probe_all = false;
                m_stream_state = 1;
            }
        }
        growth_base_bps = m_configuration->stream_parameters[3].growth_base_bps;
        growth_shift = m_configuration->stream_parameters[3].growth_shift;
        maximum_growth_shift = m_configuration->stream_parameters[3].maximum_growth_shift;
        probe_time = m_configuration->stream_parameters[3].probe_time;
        recover_time = m_configuration->stream_parameters[3].recover_time;
        settle_time = m_configuration->stream_parameters[3].settle_time;
        settle_scale = m_configuration->stream_parameters[3].settle_scale;
        backoff_bps = m_configuration->stream_parameters[3].backoff_bps;
        stable_time = m_configuration->stream_parameters[3].stable_time;
        overprobe_minimum_bps = m_configuration->stream_parameters[3].overprobe_minimum_bps;
        overprobe_maximum_bps = m_configuration->stream_parameters[3].overprobe_maximum_bps;
        overprobe_percentage = m_configuration->stream_parameters[3].overprobe_percentage;
        break;
    case 4:
        if (m_stream_state == 0)
        {
            m_stream_expansion_state = network_observer_update_stream_bandwidth(this, total_bandwidth_bps, stable_bandwidth_bps,
                streams_unstable == 0 && stream_count > 0, voice_active, stream_count, host_count, client_count,
                streams_wants_to_grow, requiring_count);
            m_stream_probed_mask = 0;
        }
        break;
    }

    switch (m_stream_state)
    {
    case 1:
        if (m_stream_settle)
        {
            m_stream_state = 0;
            m_time_of_probe = -1;
        }
        else
        {
            unsigned long probe_mask = 0;
            long lowest_target_bps = 0x7FFFFFFF;
            long lowest_bandwidth_bps = 0;
            long probe_class = -1;
            long probe_count = 0;
            long maximum_growth_bps;
            if (total_bandwidth_bps < m_stream_bandwidth_cap_bps && m_stream_low_bandwidth)
                maximum_growth_bps = m_configuration->stream_maximum_growth_low_bps;
            else
                maximum_growth_bps = m_configuration->stream_maximum_growth_bps;

            observer_mask = 1;
            for (observer_index = 0; observer_index < 32; observer_mask <<= 1, observer_index++)
            {
                s_channel_observer *observer = &m_channel_observers[observer_index];
                if (streams_to_probe & observer_mask)
                {
                    long current_bandwidth_bps = observer->stream_bandwidth_bps;
                    long successful_probes = observer->successful_probes;
                    long maximum_shift = maximum_growth_shift;
                    if (observer->stream_timeout_disabled && observer->stream_voice_active)
                        maximum_shift = m_configuration->stream_maximum_growth_shift_voice;

                    long stream_growth_shift = network_observer_stream_clamp_growth_shift(successful_probes, growth_shift, maximum_shift);
                    long total_growth_shift = total_bandwidth_bps >> m_configuration->stream_total_growth_shift;
                    if (total_growth_shift > m_configuration->stream_maximum_total_growth_shift)
                        total_growth_shift = m_configuration->stream_maximum_total_growth_shift;
                    stream_growth_shift = MAX(total_growth_shift, stream_growth_shift);

                    long growth_bps = growth_base_bps << stream_growth_shift;
                    if (growth_bps > maximum_growth_bps)
                        growth_bps = maximum_growth_bps;
                    long target_bps = current_bandwidth_bps + growth_bps;
                    if (target_bps < lowest_target_bps)
                    {
                        lowest_bandwidth_bps = current_bandwidth_bps;
                        lowest_target_bps = target_bps;
                        probe_class = observer->successful_probes > m_configuration->stream_maximum_probe_class
                            ? m_configuration->stream_maximum_probe_class : observer->successful_probes;
                        probe_mask = observer_mask;
                        probe_count = 1;
                    }
                }
            }

            if (probe_all)
            {
                unsigned long candidates = streams_to_probe;
                if (m_configuration->stream_exclude_probed)
                    candidates |= streams_probeable;
                else
                    probe_class = -1;

                while (lowest_target_bps > lowest_bandwidth_bps)
                {
                    unsigned long grow_mask = probe_mask;
                    long requiring_count = probe_count;
                    long total_available_bandwidth = lowest_target_bps - lowest_bandwidth_bps;

                    observer_mask = 1;
                    for (observer_index = 0; observer_index < 32; observer_mask <<= 1, observer_index++)
                    {
                        s_channel_observer *candidate = &m_channel_observers[observer_index];
                        if ((candidates & observer_mask) && !(grow_mask & observer_mask))
                        {
                            const long &candidate_bps = candidate->stream_bandwidth_bps;
                            long candidate_class = candidate->successful_probes > m_configuration->stream_maximum_probe_class
                                ? m_configuration->stream_maximum_probe_class : candidate->successful_probes;
                            if (candidate_bps < lowest_target_bps && candidate_bps >= lowest_bandwidth_bps &&
                                candidate_bps < candidate->stream_minimum_bandwidth_bps &&
                                candidate_bps < m_configuration->stream_maximum_probe_bps &&
                                (probe_class == -1 || candidate_class == probe_class))
                            {
                                grow_mask |= observer_mask;
                                total_available_bandwidth += lowest_target_bps - candidate_bps;
                                requiring_count++;
                            }
                        }
                    }

                    if (total_available_bandwidth <= maximum_growth_bps)
                    {
                        probe_mask = grow_mask;
                        probe_count = requiring_count;
                        break;
                    }
                    lowest_target_bps -= (total_available_bandwidth - maximum_growth_bps + requiring_count - 1) / requiring_count;
                }
            }

            m_stream_probed_mask |= probe_mask;

            long overprobe_bps = network_observer_overprobe_pin(
                                     total_bandwidth_bps * overprobe_percentage / 100, overprobe_minimum_bps, overprobe_maximum_bps) /
                probe_count;
            if (m_configuration->stream_limit_overprobe)
            {
                observer_mask = 1;
                for (observer_index = 0; observer_index < 32; observer_mask <<= 1, observer_index++)
                {
                    if (probe_mask & observer_mask)
                    {
                        s_channel_observer *observer = &m_channel_observers[observer_index];
                        long limit_bps = network_observer_overprobe_pin(
                            observer->stream_bandwidth_bps * overprobe_percentage * host_count / 100, overprobe_minimum_bps,
                            overprobe_maximum_bps);
                        if (limit_bps / probe_count < overprobe_bps)
                            overprobe_bps = limit_bps / probe_count;
                    }
                }
            }

            observer_mask = 1;
            for (observer_index = 0; observer_index < 32; observer_mask <<= 1, observer_index++)
            {
                if (probe_mask & observer_mask)
                {
                    s_channel_observer *observer = &m_channel_observers[observer_index];
                    if (reset_successful_probes)
                        observer->successful_probes = 0;

                    long bandwidth_bps = lowest_target_bps;
                    if (bandwidth_bps > observer->stream_minimum_bandwidth_bps)
                        bandwidth_bps = observer->stream_minimum_bandwidth_bps;
                    bandwidth_bps = MAX(bandwidth_bps, m_configuration->stream_minimum_bandwidth_bps);
                    observer->probe_bandwidth_bps = observer->stream_bandwidth_bps;
                    long latency_overhead;
                    stream_probe_overhead(observer_index, &latency_overhead);
                    long latency_deviation = MAX(observer->latency_idle_deviation, observer->latency_drift);
                    observer->probe_latency_threshold = observer->latency_average + latency_deviation + latency_overhead + latency_threshold_base;
                    observer->overprobe_bps = overprobe_bps;
                    stream_set_bandwidth(observer_index, bandwidth_bps + overprobe_bps);
                }
            }

            for (observer_index = 0; observer_index < 32; observer_index++)
            {
                s_channel_observer *observer = &m_channel_observers[observer_index];
                if (observer->state != _observer_state_none && observer->voice_active)
                {
                    if (m_configuration->stream_latency_spike_scaled)
                        observer->spike_latency_threshold = observer->latency_average +
                            m_configuration->stream_latency_spike_percentage * observer->latency_idle_deviation / 100;
                    else
                        observer->spike_latency_threshold = observer->latency_average + MAX(observer->latency_idle_deviation, observer->latency_drift);
                }
            }

            m_time_of_probe = network_observer_time_get();
            m_stream_unstable_duration = 0;
            m_stream_state = 2;
            m_stream_bandwidth_changed = true;
        }
        break;

    case 2:
    {
        long probe_elapsed = network_observer_time_since(m_time_of_probe);
        unsigned long probe_congested = congested_streams;
        bool congested = probe_congested != 0;
        bool all_spiking = false;
        if (congested || probe_elapsed >= recover_time)
        {
            if (m_stream_unstable_mask != 0)
                m_stream_unstable_duration = probe_elapsed;

            observer_mask = 1;
            for (observer_index = 0; observer_index < 32; observer_mask <<= 1, observer_index++)
            {
                s_channel_observer *observer = &m_channel_observers[observer_index];
                if (observer->state != _observer_state_none && observer->voice_active &&
                    observer->probe_bandwidth_bps != -1 && observer->probe_latency_threshold != -1 &&
                    observer->latency_average > observer->probe_latency_threshold)
                {
                    congested = true;
                    probe_congested |= observer_mask;
                }
            }
        }
        else
        {
            m_stream_unstable_mask = active_streams;
        }

        if (congested)
        {
            bool any_spiking = false;
            all_spiking = true;
            observer_mask = 1;
            for (observer_index = 0; observer_index < 32; observer_mask <<= 1, observer_index++)
            {
                s_channel_observer *observer = &m_channel_observers[observer_index];
                if (observer->state != _observer_state_none && observer->voice_active)
                {
                    if (observer->spike_latency_threshold == -1 || observer->latency_average <= observer->spike_latency_threshold)
                    {
                        all_spiking = false;
                    }
                    else
                    {
                        any_spiking = true;
                        probe_congested |= observer_mask;
                    }
                }
            }
            if (!any_spiking)
                all_spiking = false;
        }

        if (!congested && !m_stream_stable)
            m_stream_unstable_duration = probe_elapsed;

        if (congested || m_stream_settle)
        {
            observer_mask = 1;
            for (observer_index = 0; observer_index < 32; observer_mask <<= 1, observer_index++)
            {
                s_channel_observer *observer = &m_channel_observers[observer_index];
                if (observer->state != _observer_state_none && observer->voice_active)
                {
                    if (observer->probe_bandwidth_bps != -1)
                    {
                        if (!m_stream_settle && (m_configuration->stream_always_count_failure || (probe_congested & observer_mask)))
                        {
                            observer->successful_probes = 0;
                            observer->failed_probes++;
                        }
                        observer->overprobe_bps = 0;
                        stream_set_bandwidth(observer_index, observer->probe_bandwidth_bps);
                        observer->probe_bandwidth_bps = -1;
                        streams_backed_off |= observer_mask;
                    }
                    else
                    {
                        if (!all_spiking && (m_configuration->stream_backoff_on_spike == 0 || observer->spike_latency_threshold == -1 ||
                            observer->latency_average <= observer->spike_latency_threshold))
                        {
                            continue;
                        }
                        observer->successful_probes = 0;
                        observer->failed_probes++;
                        streams_backed_off |= observer_mask;
                    }
                }
            }

            if (!congested)
            {
                m_stream_state = 0;
                m_time_of_probe = -1;
                break;
            }
            else
            {
                if (all_spiking && any_low_priority)
                {
                    if (m_stream_total_bandwidth_average == -1)
                        m_stream_total_bandwidth_average = m_stream_bandwidth_cap_bps;
                    m_stream_total_bandwidth_average += (total_bandwidth_bps - m_stream_total_bandwidth_average) >> m_configuration->stream_total_average_shift;
                }
                m_stream_state = 3;
            }

            m_time_of_probe = -1;
        }
        else if ((unsigned long)network_observer_time_since(m_time_of_probe) >= probe_time + m_stream_unstable_duration)
        {
            for (observer_index = 0; observer_index < 32; observer_index++)
            {
                s_channel_observer *observer = &m_channel_observers[observer_index];
                if (observer->state != _observer_state_none && observer->voice_active)
                {
                    if (observer->probe_bandwidth_bps != -1)
                    {
                        observer->stream_probed = 1;
                        observer->failed_probes = 0;
                        observer->successful_probes++;
                        if (observer->overprobe_bps != 0)
                        {
                            stream_set_bandwidth(observer_index, observer->stream_bandwidth_bps - observer->overprobe_bps);
                            observer->overprobe_bps = 0;
                        }
                        observer->stream_maximum_bandwidth_bps = MAX(observer->stream_maximum_bandwidth_bps, observer->stream_bandwidth_bps);
                        observer->probe_bandwidth_bps = -1;
                        observer->probe_latency_threshold = -1;
                    }
                    observer->spike_latency_threshold = -1;
                }
            }
            m_stream_state = 0;
            m_time_of_probe = -1;
            m_time_of_throttle = network_observer_time_get();
        }
        break;
    }

    case 3:
    {
        bool settled = false;
        if (m_time_of_probe == -1)
            m_time_of_probe = network_observer_time_get();
        else if (network_observer_time_since(m_time_of_probe) >= settle_time)
            settled = true;

        if (settled)
        {
            for (observer_index = 0; observer_index < 32; observer_index++)
            {
                s_channel_observer *observer = &m_channel_observers[observer_index];
                if (observer->state != _observer_state_none && observer->voice_active)
                {
                    observer->latency_time = network_observer_time_get();
                    if (observer->probe_bandwidth_bps != -1)
                    {
                        stream_set_bandwidth(observer_index, observer->probe_bandwidth_bps);
                        observer->probe_bandwidth_bps = -1;
                    }
                    observer->probe_latency_threshold = -1;
                    observer->spike_latency_threshold = -1;
                }
            }
            m_stream_state = 4;
            m_time_of_probe = -1;
        }
        else
        {
            for (observer_index = 0; observer_index < 32; observer_index++)
            {
                s_channel_observer *observer = &m_channel_observers[observer_index];
                if (observer->state != _observer_state_none && observer->voice_active &&
                    (observer->voice_talker || observer->stream_packet_rate_limited) && observer->probe_bandwidth_bps == -1)
                {
                    long current_bps = observer->stream_bandwidth_bps;
                    long bandwidth_bps = (long)(current_bps * settle_scale + 0.5f);
                    bandwidth_bps = MAX(bandwidth_bps, m_configuration->stream_minimum_bandwidth_bps);
                    bandwidth_bps = bandwidth_bps > observer->stream_minimum_bandwidth_bps ? observer->stream_minimum_bandwidth_bps : bandwidth_bps;
                    observer->probe_bandwidth_bps = current_bps;
                    if (bandwidth_bps < current_bps)
                        observer->stream_sent_bps = -1;
                    stream_set_bandwidth(observer_index, bandwidth_bps);
                }
            }
        }
        break;
    }

    case 4:
    {
        bool recovered = false;
        if (congested_streams != 0)
        {
            m_stream_state = 5;
            m_time_of_probe = -1;
        }
        else
        {
            if (!m_stream_settle)
            {
                if (m_time_of_probe == -1)
                {
                    m_time_of_probe = network_observer_time_get();
                    break;
                }
                if (network_observer_time_since(m_time_of_probe) < stable_time / 2)
                {
                    m_stream_unstable_mask = active_streams;
                    if (!m_stream_stable)
                        m_stream_unstable_duration = network_observer_time_since(m_time_of_probe);
                    break;
                }
                if (!m_stream_stable || m_stream_unstable_mask != 0)
                {
                    m_stream_unstable_duration = network_observer_time_since(m_time_of_probe);
                    break;
                }
                if ((unsigned long)network_observer_time_since(m_time_of_probe) < m_stream_unstable_duration + stable_time)
                    break;
                recovered = congested_streams == 0;
            }
            else
                recovered = congested_streams == 0;
        }

        if (recovered)
        {
            for (observer_index = 0; observer_index < 32; observer_index++)
            {
                s_channel_observer *observer = &m_channel_observers[observer_index];
                if (observer->state != _observer_state_none && observer->voice_active)
                    observer->throttle_count = 0;
            }
            m_stream_state = 0;
            m_time_of_probe = -1;
            m_time_of_throttle = network_observer_time_get();
            break;
        }
    }
        // fall through

    case 5:
        if (any_low_priority && congested_streams == active_streams)
        {
            if (m_stream_total_bandwidth_average == -1)
                m_stream_total_bandwidth_average = m_stream_bandwidth_cap_bps;
            m_stream_total_bandwidth_average += (total_bandwidth_bps - m_stream_total_bandwidth_average) >> m_configuration->stream_total_average_shift;
        }

        while (congested_streams != 0)
        {
            bool bandwidth_reduced = false;
            observer_mask = 1;
            for (observer_index = 0; observer_index < 32; observer_mask <<= 1, observer_index++)
            {
                s_channel_observer *observer = &m_channel_observers[observer_index];
                if (observer->state != _observer_state_none && observer->voice_active && (observer->voice_talker || observer->stream_packet_rate_limited))
                {
                    long acknowledged_bps = (long)(observer->acknowledged_statistics.average_values_per_second() * 8.0f);
                    long bandwidth_bps;
                    if (congested_streams & observer_mask)
                    {
                        bandwidth_bps = observer->stream_bandwidth_bps * m_configuration->stream_congested_percentage / 100;
                        bandwidth_bps = bandwidth_bps > acknowledged_bps ? acknowledged_bps : bandwidth_bps;
                        bandwidth_bps = bandwidth_bps > observer->stream_bandwidth_bps - backoff_bps
                            ? observer->stream_bandwidth_bps - backoff_bps : bandwidth_bps;
                        bandwidth_bps = MAX(bandwidth_bps, m_configuration->stream_minimum_bandwidth_bps);
                        bandwidth_bps = bandwidth_bps > observer->stream_minimum_bandwidth_bps ? observer->stream_minimum_bandwidth_bps : bandwidth_bps;
                        if (m_configuration->stream_clear_threshold_on_congestion)
                            observer->latency_threshold_valid = false;
                        observer->successful_probes = 0;
                        observer->throttle_count++;
                        observer->failed_probes = m_configuration->stream_count_throttle_as_failure != false;
                    }
                    else
                    {
                        bandwidth_bps = observer->stream_bandwidth_bps * m_configuration->stream_uncongested_percentage / 100;
                        bandwidth_bps = MAX(bandwidth_bps, m_configuration->stream_minimum_bandwidth_bps);
                        bandwidth_bps = bandwidth_bps > observer->stream_minimum_bandwidth_bps ? observer->stream_minimum_bandwidth_bps : bandwidth_bps;
                    }

                    if (observer->best_loss_bandwidth_bps != -1 &&
                        (bandwidth_bps <= observer->best_loss_bandwidth_bps || bandwidth_bps <= m_configuration->stream_minimum_bandwidth_bps))
                    {
                        observer->best_loss_bandwidth_bps = -1;
                    }

                    m_stream_bandwidth_changed = true;
                    observer->stream_sent_bps = -1;
                    if (bandwidth_bps != observer->stream_bandwidth_bps)
                    {
                        if (m_configuration->stream_debug_bandwidth)
                            observer->throughput_statistics.average_values_per_second();
                        stream_set_bandwidth(observer_index, bandwidth_bps);
                        bandwidth_reduced = true;
                    }
                    else if (congested_streams & observer_mask)
                    {
                        observer->latency_threshold_valid = false;
                    }
                }
            }

            if (bandwidth_reduced || congested_streams == 0xFFFFFFFF || !m_configuration->stream_retry_congestion)
                break;
            congested_streams = 0xFFFFFFFF;
        }

        m_stream_rebalance = true;
        m_stream_state = 3;
        m_time_of_probe = -1;
        break;
    }

    if (streams_backed_off != 0)
    {
        observer_mask = 1;
        for (observer_index = 0; observer_index < 32; observer_mask *= 2, observer_index++)
        {
            s_channel_observer *observer = &m_channel_observers[observer_index];
            if ((streams_backed_off & observer_mask) && observer->state != _observer_state_none && observer->voice_active &&
                observer->failed_probes >= backoff_failures)
            {
                stream_backoff_update(observer_index, backoff_tolerance_bps, maximum_backoff_level, backoff_time);
            }
        }
    }
}

// 0x125e10
bool c_network_observer::stream_check_stream_channel_connections(bool *all_streams_changed)
{
    bool any_stream_changed = false;
    bool use_stream_control = m_stream_control;
    bool all_changed = true;
    bool simulation_attached = false;
    bool voice_changed = false;
    bool voice_disabled = !use_stream_control;

    for (long observer_index = 0; observer_index < 32; observer_index++)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        if (observer->state != _observer_state_none)
        {
            bool changed = false;
            bool was_attached = false;
            bool stream_control = use_stream_control;
            long priority = 0;
            for (long owner_index = 0; owner_index < 4; owner_index++)
            {
                if (observer->owner_flags & (1 << owner_index))
                {
                    c_network_observer_owner *owner = m_owners[owner_index].owner;
                    long owner_priority = owner->get_stream_priority(observer_index);
                    if (owner_priority > priority)
                        priority = owner_priority;
                }
            }

            bool client = priority >= 1;
            bool voice = priority >= 2;
            bool voice_active = priority >= 3;
            if (voice && !voice_active)
                stream_control = false;
            if (!stream_control)
                voice_disabled = true;

            c_network_channel *channel = &observer->channel;
            if (observer->voice_active)
            {
                was_attached = observer->voice_talker;
                if (!stream_control || !channel->connected() ||
                    channel->simulation_attached() != observer->voice_talker ||
                    channel->simulation_is_authority() != observer->voice_listener)
                {
                    stream_dispose(observer_index, !channel->connected(), "connection disconnected");
                    changed = true;
                }
            }
            if (channel->connected() && stream_control && !observer->voice_active)
            {
                stream_initialize(observer_index);
                changed = true;
            }

            if (observer->voice_active)
            {
                if (!was_attached && observer->voice_talker)
                    simulation_attached = true;
                if (client != observer->stream_packet_rate_limited)
                {
                    observer->stream_packet_rate_limited = client;
                    if (!observer->voice_talker)
                        changed = true;
                }
                if (voice != observer->stream_timeout_disabled || voice_active != observer->stream_voice_active)
                {
                    if (voice_active != observer->stream_voice_active)
                        voice_changed = true;
                    changed = true;
                    observer->stream_timeout_disabled = voice;
                    observer->stream_voice_active = voice_active;
                    observer->stream_initialized = false;
                }
            }

            if (changed)
                any_stream_changed = true;
            else
                all_changed = false;
        }
    }

    if (voice_disabled)
        network_observer_voice_set_outgoing_channel_count(15, 15);

    if (simulation_attached)
    {
        bool group_session = false;
        for (long owner_index = 0; owner_index <= 2; owner_index++)
        {
            long session_index = m_owners[owner_index].session_index;
            if (session_index != -1 && network_observer_session_class_is_squad_established(session_index) && !network_observer_session_is_squad(session_index))
            {
                group_session = true;
                break;
            }
        }
        for (long observer_index = 0; observer_index < 32; observer_index++)
        {
            s_channel_observer *observer = &m_channel_observers[observer_index];
            if (observer->state != _observer_state_none && observer->voice_active && observer->voice_talker)
            {
                if (!group_session)
                {
                    if (observer->voice_listener)
                        observer->quality_statistics_active[0] = true;
                    else
                        observer->quality_statistics_active[1] = true;
                }
                else
                {
                    if (observer->voice_listener)
                        observer->quality_statistics_active[2] = true;
                    else
                        observer->quality_statistics_active[3] = true;
                }
            }
        }
    }

    if (voice_changed)
    {
        for (long observer_index = 0; observer_index < 32; observer_index++)
        {
            s_channel_observer *observer = &m_channel_observers[observer_index];
            if (observer->state != _observer_state_none && observer->voice_active &&
                observer->voice_talker && observer->stream_timeout_disabled)
            {
                observer->stream_initialized = false;
            }
        }
    }

    *all_streams_changed = all_changed;
    return any_stream_changed;
}

// 0x1265d8
void c_network_observer::record_throughput_bandwidth(long sent_bps, long stream_count, long saturated_count, long elapsed_msec)
{
  if (stream_count > 0) {
    bool saturated = (float)saturated_count > m_configuration->stream_saturated_ratio * (float)stream_count;
    if (m_time_of_bandwidth_probe == -1 || m_bandwidth_probing != saturated) {
      m_bandwidth_probing = saturated;
      m_time_of_bandwidth_probe = network_observer_time_get();
    }
    if (network_observer_time_since(m_time_of_bandwidth_probe) >= m_configuration->stream_saturated_interval) {
      m_bandwidth_probe_counts[m_bandwidth_probing]++;
      m_time_of_bandwidth_probe = network_observer_time_get();
    }
  }
  if (sent_bps > m_stream_peak_bandwidth_bps)
    m_stream_peak_bandwidth_bps = sent_bps;
  if (m_stream_total_bandwidth_average != -1 && sent_bps > m_stream_total_bandwidth_average)
    m_stream_total_bandwidth_average += (sent_bps - m_stream_total_bandwidth_average) >>
        m_configuration->stream_total_average_shift;
}

// 0x126758
void c_network_observer::update_voice_repeater_use()
{
    bool forced = false;
    bool host_use = false;
    bool client_use = false;
    long usage = _voice_repeater_usage_none;
    for (long observer_index = 0; observer_index < 32; observer_index++)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        if (observer->state != _observer_state_none && observer->voice_active)
        {
            host_use = host_use | (observer->voice_talker && observer->voice_listener);
            client_use = client_use | (observer->voice_talker && observer->voice_repeater);
        }
    }
    if (host_use)
    {
        if (forced)
            usage = _voice_repeater_usage_two;
        else
            usage = _voice_repeater_usage_one;
    }
    else if (g_network_configuration.voice_repeater_enabled && (forced || client_use))
    {
        usage = _voice_repeater_usage_one;
    }
    network_observer_voice_set_repeater_use((e_voice_repeater_usage)usage);
}

// 0x3ca598
// Whether the owner's entry is past the game's time, or any of its four items is flagged 0x20. The
bool network_owner_entry_is_pending(c_network_session_membership *membership, long index)
{
  bool result = false;
  s_network_session_peer *entry = &membership->m_current_membership.peers[index];
  if (entry->properties.host_badness > g_network_configuration.observer.time)
    result = true;
  else {
    for (unsigned long i = 0; i < 4; i++) {
      long item_index = entry->user_player_indices[i];
      if (item_index != -1 && (membership->m_current_membership.players[item_index].configuration.client.player_flags & 0x20)) {
        result = true;
        break;
      }
    }
  }
  return result;
}

// Initialization uses the networking globals shared by the link and its message services.

// @address 0x3d8dc0 ?initialize_observer@c_network_observer@@SA_NPAV1@PAVc_network_link@@PAVc_network_message_type_collection@@PAVc_network_message_gateway@@PAVc_network_message_handler@@PBUs_observer_configuration@@@Z
bool c_network_observer::initialize_observer(c_network_observer *observer, c_network_link *link,
    c_network_message_type_collection *message_types, c_network_message_gateway *message_gateway,
    c_network_message_handler *message_handler, const s_observer_configuration *configuration)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x3d8dc0: the five interfaces load off the
    // network_globals_message_handler base, not the argument registers
    c_network_message_handler **globals = (c_network_message_handler **)network_globals_message_handler;
    message_handler = globals[0];
    message_gateway = ((c_network_message_gateway **)globals)[-1];
    message_types = ((c_network_message_type_collection **)globals)[-2];
    observer = ((c_network_observer **)globals)[1];
    link = ((c_network_link **)globals)[-3];
#endif
    observer->m_link = link;
    observer->m_message_types = message_types;
    observer->m_message_gateway = message_gateway;
    observer->m_message_handler = message_handler;
    observer->m_configuration = configuration;
    for (unsigned long owner_index = 0; owner_index < 4; owner_index++)
    {
        observer->m_owners[owner_index].owner = 0;
        observer->m_owners[owner_index].session_index = -1;
    }
    observer->m_bandwidth_statistics.initialize(observer->m_configuration->time_statistics_interval);
    observer->m_stream_bandwidth_changed = true;
    observer->m_stream_expansion_pending = false;
    observer->m_stream_expansion_blocked = false;
    observer->m_time_of_bandwidth_measurement = network_observer_time_get();
    observer->m_time_of_period = network_observer_time_get();
    for (long observer_index = 0; observer_index < 32; observer_index++)
        observer->clear_observer(observer_index);
    observer->m_stream_expansion_state = 0;
    observer->m_time_of_expansion = network_observer_time_get();
    observer->m_stream_probed_mask = 0;
    observer->m_stream_state = 0;
    observer->m_time_of_probe = -1;
    observer->m_stream_bandwidth_cap_bps = -1;
    observer->m_stream_total_bandwidth_average = -1;
    observer->set_online_network_environment(false);
    return true;
}

// 0x3d8f98
void c_network_observer::reset_bandwidth()
{
    // match: 0x3d8f98: without it g_network_time_globals is reloaded (lbz/cmplwi) after self-> stores and the branch flips bne->beq
    c_network_observer *__restrict self = this;
    self->m_bandwidth_minimum_bps = self->m_configuration->bandwidth_minimum_bps;
    self->m_bandwidth_growth_bps = 1024;
    self->m_stream_peak_bandwidth_bps = -1;
    self->m_time_of_bandwidth_probe = -1;
    self->m_bandwidth_probing = false;
    memset(self->m_bandwidth_probe_counts, 0, sizeof(self->m_bandwidth_probe_counts));
    self->m_bandwidth_statistics.reset();
    self->m_bandwidth_safe_bps = self->m_bandwidth_bps / 1024 - self->m_configuration->bandwidth_safety_margin_kbps;
    self->m_bandwidth_safe_bps = self->m_bandwidth_safe_bps > self->m_bandwidth_minimum_bps ? self->m_bandwidth_safe_bps : self->m_bandwidth_minimum_bps;
    self->m_bandwidth_successful_bps = 0;
    self->m_bandwidth_unsafe_bps = 0;
    self->m_bandwidth_satiation_fraction = 0.0f;
    self->m_bandwidth_satiation_average = 0.0f;
    self->m_time_of_bandwidth_measurement = network_observer_time_get();
    self->m_time_of_period = network_observer_time_get();
    self->m_stream_bandwidth_changed = true;
    self->m_time_of_bandwidth_reset = network_observer_time_get();
    for (long observer_index = 0; observer_index < 32; observer_index++)
    {
        s_channel_observer *observer = &self->m_channel_observers[observer_index];
        if (observer->state != _observer_state_none)
        {
            if (observer->voice_active)
                network_observer_stream_dispose(observer_index);
            memset(observer->bandwidth_events, 0, sizeof(observer->bandwidth_events));
        }
    }
}

// 0x3d91d0
void c_network_observer::quality_statistics_get_ratings(long *connectivity, long *host_badness, long *client_badness)
{
    long maximum_host_badness = 0;
    long maximum_client_badness = 0;
    long connectivity_count = 0;
    for (long word = 0; word < 2; ++word)
    {
        unsigned __int64 bits = m_network_quality_statistics.quality_statistics.connectivity[word];
        for (long bit = 0; bit < 64; ++bit)
        {
            if (bits & 1)
                ++connectivity_count;
            bits >>= 1;
        }
    }
    long connectivity_rating = connectivity_count * 100 / 128;
    for (long type = 0; type < 2; ++type)
    {
        s_network_observer_quality_statistics *statistics = &m_network_quality_statistics.quality_statistics.type[type];
        long client_count = 0;
        for (long word = 0; word < 2; ++word)
        {
            unsigned __int64 bits = statistics->client_badness[word];
            for (long bit = 0; bit < 64; ++bit)
            {
                if (bits & 1)
                    ++client_count;
                bits >>= 1;
            }
        }
        long client_rating = client_count * 100 / 128;
        if (client_rating > maximum_client_badness)
            maximum_client_badness = client_rating;
        long host_count = 0;
        for (long word = 0; word < 2; ++word)
        {
            unsigned __int64 bits = statistics->host_badness[word];
            for (long bit = 0; bit < 64; ++bit)
            {
                if (bits & 1)
                    ++host_count;
                bits >>= 1;
            }
        }
        long host_rating = host_count * 100 / 128;
        if (host_rating > maximum_host_badness)
            maximum_host_badness = host_rating;
    }
    *connectivity = connectivity_rating;
    *host_badness = maximum_host_badness;
    *client_badness = maximum_client_badness;
}

// 0x3d9318
static __forceinline void network_observer_quality_connectivity_add(s_network_observer_quality_statistics_set *statistics, bool bad)
{
    for (long index = 1; index > 0; index--)
        statistics->connectivity[index] = (statistics->connectivity[index] << 1) | (statistics->connectivity[index - 1] >> 63);
    statistics->connectivity[0] = (statistics->connectivity[0] << 1) | (unsigned int)bad;
}

void c_network_observer::quality_statistics_notify_established_connectivity(long observer_index, bool good)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (!observer->quality_statistics_ignored && !observer->quality_statistics_connectivity_reported)
    {
        network_observer_quality_connectivity_add(&m_network_quality_statistics.quality_statistics, !good);
        observer->quality_statistics_connectivity_reported = true;
    }
}

// 0x3d9388
void c_network_observer::quality_statistics_report_badness(long observer_index, bool bad, const char *reason)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (observer->quality_statistics_ignored)
        return;
    if (!observer->quality_statistics_reported[0] && observer->quality_statistics_active[0])
    {
        network_observer_record_badness(m_network_quality_statistics.quality_statistics.type[0].host_badness, bad);
        observer->quality_statistics_reported[0] = true;
    }
    if (!observer->quality_statistics_reported[1] && observer->quality_statistics_active[1])
    {
        network_observer_record_badness(m_network_quality_statistics.quality_statistics.type[0].client_badness, bad);
        observer->quality_statistics_reported[1] = true;
    }
    if (!observer->quality_statistics_reported[2] && observer->quality_statistics_active[2])
    {
        network_observer_record_badness(m_network_quality_statistics.quality_statistics.type[1].host_badness, bad);
        observer->quality_statistics_reported[2] = true;
    }
    if (!observer->quality_statistics_reported[3] && observer->quality_statistics_active[3])
    {
        network_observer_record_badness(m_network_quality_statistics.quality_statistics.type[1].client_badness, bad);
        observer->quality_statistics_reported[3] = true;
    }
}

// 0x3d9518
long c_network_observer::observer_channel_find_by_network_channel(e_network_observer_owner owner_type, c_network_channel *channel) const
{
    long observer_index = -1;
    const s_channel_observer *observer = find_observer_by_channel(channel);
    if (observer->state != _observer_state_none && (observer->owner_flags & (1 << owner_type)) != 0)
        observer_index = observer - m_channel_observers;
    return observer_index;
}


// 0x3d9568
void c_network_observer::observer_channel_send_message(e_network_observer_owner owner, long observer_index, bool direct,
    e_network_message_type message_type, long message_size, const void *message)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (direct)
    {
        transport_address address;
        bool ready = false;
        if (transport_address_valid(&observer->secure_address))
        {
            address = observer->secure_address;
            ready = true;
        }
        else
        {
            ready = build_secure_connection(owner, (bool)(_rotl(observer->flags, 27) & 1), &observer->secure_identifier, &address);
        }
        if (ready)
            m_message_gateway->send_message_directed(&address, message_type, message_size, message);
    }
    else if (!observer->channel.closed())
    {
        if ((observer->message_type_flags & (1i64 << message_type)) != 0)
            observer->message_type_flags &= ~(1i64 << message_type);
        observer->channel.m_message_queue.send_message(message_type, message_size, message);
    }
}


static __forceinline long network_observer_voice_get_observer_stream_index(long observer_index)
{
    return g_voice_interface_globals.observer_channel_peer_indices[observer_index];
}

static __forceinline bool network_voice_data_available(long observer_index)
{
    bool available = false;
    if (g_voice_interface_globals.initialized && voice_lifecycle_valid())
    {
        long stream_index = network_observer_voice_get_observer_stream_index(observer_index);
        if (stream_index != -1)
            available = g_voice_queue.has_packets_to_transmit(stream_index);
    }
    return available;
}

static __forceinline long network_voice_get_data_size(long observer_index)
{
    long data_size = 0;
    if (g_voice_interface_globals.initialized && voice_lifecycle_valid())
    {
        long stream_index = network_observer_voice_get_observer_stream_index(observer_index);
        if (stream_index != -1)
            data_size = g_voice_queue.get_peer_voice_stream_size(stream_index);
    }
    return data_size;
}

static __forceinline long network_voice_data_get(long observer_index, void *buffer, long buffer_size)
{
    long voice_data_bytes = 0;
    if (g_voice_interface_globals.initialized && voice_lifecycle_valid())
    {
        long stream_index = network_observer_voice_get_observer_stream_index(observer_index);
        if (stream_index != -1)
            voice_data_bytes = g_voice_queue.encode_peer_voice_stream(stream_index, (unsigned char *)buffer, buffer_size);
    }
    return voice_data_bytes;
}

inline bool c_network_observer::observer_desires_heartbeat(long observer_index) const
{
    bool desires_heartbeat = false;
    const s_channel_observer *observer = &m_channel_observers[observer_index];
    const c_network_channel *channel = &observer->channel;
    if (channel->established())
    {
        long time_since_receive = network_observer_time_since(channel->last_activity_time(_network_channel_activity_send_sequenced_packet));
        if (time_since_receive >= m_configuration->heartbeat_interval_msec)
            desires_heartbeat = true;
    }
    return desires_heartbeat;
}

inline long c_network_observer::voice_data_build(long observer_index, long voice_data_bytes, void *voice_data_buffer) const
{
    return network_voice_data_get(observer_index, voice_data_buffer, voice_data_bytes);
}

// 0x3d9688
bool c_network_observer::channel_should_send_packet(c_network_channel *channel, bool game_data_pending, bool force_heartbeat,
    bool send_sequenced, bool *out_send_sequenced_packet, bool *out_force_fill_packet,
    long *out_game_data_size_bytes, long *out_voice_data_size_bytes, long voice_packet_bytes, void *voice_data_buffer)
{
    s_channel_observer *observer = find_observer_by_channel(channel);
    long observer_index = find_observer_index_by_observer(observer);
    bool send_packet = false;
    bool force_fill_packet = false;
    long safe_packet_bytes = -1;
    bool heartbeat = false;
    if (observer_desires_heartbeat(observer_index))
    {
        force_heartbeat = true;
        send_sequenced = true;
        heartbeat = true;
    }

    long voice_data_available_bytes;
    bool voice_available = voice_data_available(observer_index, false, &voice_data_available_bytes);
    unsigned long current_time = network_observer_time_get();
    long safe_packet_interval = -1;
    bool simulation = (channel->simulation_attached() || voice_available) && !heartbeat;
    bool stream_fill = false;
    float packet_rate = -1.0f;
    long stream_bandwidth_bps = -1;
    if (observer->voice_active && !heartbeat && observer->stream_requires_more_bandwidth)
    {
        stream_fill = true;
        simulation = true;
    }

    if (!heartbeat)
    {
        if (simulation)
        {
            if (observer->voice_active)
                packet_rate = observer->stream_packet_rate;
            else if (channel->simulation_attached())
                packet_rate = -1.0f;
            else
                packet_rate = g_network_configuration.observer.simulation_packet_rate;
        }
        else if (!game_data_pending)
        {
            if (force_heartbeat && observer->voice_active && (observer->stream_packet_rate_limited || observer->voice_talker))
                packet_rate = 4.0f;
            else
                packet_rate = 0.0f;
        }
    }

    if (channel->connected() && !heartbeat)
    {
        long safety_range = channel->get_connection_safe_sequence_range();
        long safety_remaining = channel->get_connection_safe_sequence_range_remaining();
        float safety_fraction = (float)safety_remaining / ((float)safety_range * m_configuration->safe_sequence_threshold);
        if (safety_fraction < 1.0f)
        {
            float safe_packet_rate = m_configuration->safe_packet_rate_minimum +
                (1.0f - safety_fraction) * (m_configuration->safe_packet_rate_maximum - m_configuration->safe_packet_rate_minimum);
            if (packet_rate <= 0.0f || packet_rate >= 1.0f / safe_packet_rate)
                packet_rate = 1.0f / safe_packet_rate;
        }
    }

    if (simulation)
    {
        if (observer->voice_active)
            stream_bandwidth_bps = observer->stream_bandwidth_bps;
        else
            stream_bandwidth_bps = -1;
    }
    if (simulation)
        send_sequenced = true;

    long elapsed_msec = current_time - observer->stream_last_packet_time;
    if (elapsed_msec < 0)
    {
        elapsed_msec = 0;
        observer->stream_last_packet_time = current_time;
    }

    bool send;
    if (packet_rate > 0.0f)
    {
        safe_packet_interval = (long)(1000.0f / packet_rate + 0.5f);
        send = elapsed_msec >= safe_packet_interval;
    }
    else if (packet_rate == 0.0f)
    {
        send = false;
    }
    else
    {
        send = true;
    }

    if (send && stream_bandwidth_bps >= 0)
    {
        if (elapsed_msec > 1000)
            elapsed_msec = 1000;
        long bits = elapsed_msec * stream_bandwidth_bps / 1000;
        safe_packet_bytes = bits / 8 - 45;
        if (safe_packet_bytes < 48)
            send = false;
    }

    if (send && safe_packet_interval >= 0)
        observer->stream_last_packet_time = current_time;
    if (send)
    {
        send_packet = true;
        if (stream_fill)
            force_fill_packet = true;
    }

    if (send_packet)
    {
        long voice_data_bytes;
        if (voice_available)
        {
            float voice_fraction = 1.0f;
            if (game_data_pending)
            {
                c_network_channel_simulation_interface *const *simulation_storage = channel->simulation_interface_storage();
                if (*simulation_storage)
                {
                    bool authority = *simulation_storage && (*simulation_storage)->m_authority;
                    if (authority)
                        voice_fraction = m_configuration->voice_fraction_authority;
                    else
                        voice_fraction = m_configuration->voice_fraction_client;
                }
                else
                {
                    voice_fraction = m_configuration->voice_fraction_unattached;
                }
            }
            long voice_packet_size;
            if (safe_packet_bytes >= 0)
            {
                voice_packet_size = (long)((float)safe_packet_bytes * voice_fraction + 0.5f);
                if (voice_packet_size > 512)
                    voice_packet_size = 512;
            }
            else
            {
                voice_packet_size = 512;
            }
            if (observer->voice_active && voice_packet_size < voice_data_available_bytes)
                stream_packet_bandwidth_restricted(observer_index);
            voice_data_bytes = voice_data_build(observer_index, voice_packet_size, voice_data_buffer);
        }
        else
        {
            voice_data_bytes = 0;
        }

        long game_data_size_bytes;
        if (safe_packet_bytes >= 0)
        {
            game_data_size_bytes = safe_packet_bytes - voice_data_bytes;
            game_data_size_bytes = game_data_size_bytes > 48 ? game_data_size_bytes : 48;
        }
        else
            game_data_size_bytes = 1536;

        if (c_network_link::adjust_packet_size(false, voice_data_bytes, &game_data_size_bytes))
        {
            *out_send_sequenced_packet = send_sequenced;
            *out_force_fill_packet = force_fill_packet;
            *out_game_data_size_bytes = game_data_size_bytes;
            *out_voice_data_size_bytes = voice_data_bytes;
        }
        else
        {
            send_packet = false;
        }
    }
    return send_packet;
}

// 0x3d9bf8
bool c_network_observer::voice_data_available(long observer_index, bool potentially_available, long *voice_bytes_available)
{
    bool result = false;
    if (network_voice_data_available(observer_index))
    {
        result = true;
        *voice_bytes_available = network_voice_get_data_size(observer_index);
    }
    return result;
}

// 0x3d9cd0
bool c_network_observer::observer_channel_backlogged(e_network_observer_owner owner, long observer_index, e_network_message_type message_type) const
{
    const c_network_observer::s_channel_observer *observer = get_observer(owner, observer_index);
    const c_network_channel *channel = &observer->channel;
    bool backlogged = false;
    if (channel->connected())
    {
        if (!(observer->message_type_flags & (1i64 << message_type)))
            backlogged = channel->get_message_space_available() * 4 < channel->get_message_space_maximum_size_bytes();
        else
            backlogged = channel->get_message_space_available() * 4 < channel->get_message_space_maximum_size_bytes() * 3;
    }
    return backlogged;
}

// 0x3d9d58
bool c_network_observer::observer_channel_currently_alive(e_network_observer_owner owner, long observer_index) const
{
    const s_channel_observer *observer = get_observer(owner, observer_index);
    const c_network_channel *channel = &observer->channel;
    bool alive = false;
    if (channel->connected())
    {
        long first_age = network_observer_time_since(channel->last_activity_time(_network_channel_activity_send_sequenced_packet));
        long second_age = network_observer_time_since(channel->last_activity_time(_network_channel_activity_packet_delivery));
        if (first_age < m_configuration->channel_activity_first_timeout && second_age >= m_configuration->channel_activity_second_timeout)
            alive = false;
        else
            alive = true;
    }
    return alive;
}

long c_network_observer::allocate_observer(e_network_observer_owner owner, const s_transport_secure_address *address)
{
    long result = -1;
    for (long existing_index = 0; existing_index < 32; existing_index++)
    {
        s_channel_observer *observer = &m_channel_observers[existing_index];
        if (observer->state != _observer_state_none && transport_secure_address_compare(address, &observer->secure_identifier))
        {
            result = existing_index;
            break;
        }
    }
    if (result == -1)
    {
        long index = -1;
        for (long unused_index = 0; unused_index < 32; unused_index++)
        {
            s_channel_observer *observer = &m_channel_observers[unused_index];
            if (observer->state == _observer_state_none)
            {
                index = unused_index;
                break;
            }
        }
        if (index == -1)
        {
            for (long reusable_index = 0; reusable_index < 32; reusable_index++)
            {
                s_channel_observer *observer = &m_channel_observers[reusable_index];
                if (observer->state != _observer_state_none && observer->owner_flags == 0)
                {
                    index = reusable_index;
                    release_observer(index);
                    break;
                }
            }
        }
        if (index != -1)
        {
        s_channel_observer *observer = &m_channel_observers[index];
        char name[256];
        const s_channel_configuration *channel_configuration = &g_network_configuration.channel_configuration;
        transport_secure_address_to_string(address, name, 256, false, false);
        observer->channel.allocate(8, name, m_link, this, m_message_gateway, m_message_handler, m_message_types, channel_configuration);
        observer->channel.set_connection_packet_timeout(m_configuration->stream_initial_packet_timeout);
        observer->state = _observer_state_idle;
        observer->state_change_time = network_observer_time_get();
        observer->owner_connection_identifier = -1;
        observer->secure_identifier = *address;
        observer->last_inactive_time = network_observer_time_get();
        observer->undesired_time = 0;
        observer->drained_time = 0;
        observer->latency_time = -1;
        observer->latency_average = -1;
        observer->latency_drift = 0;
        observer->latency_idle_deviation = g_network_configuration.initial_latency_deviation;
        observer->latency_deviation = g_network_configuration.initial_latency_deviation;
        long packet_timeout = m_configuration->stream_initial_packet_timeout;
        observer->packet_timeout = packet_timeout;
        observer->channel.set_connection_packet_timeout(packet_timeout);
        network_observer_statistics_initialize(&observer->packet_statistics, m_configuration->time_statistics_interval);
        network_observer_statistics_initialize(&observer->throughput_statistics, m_configuration->time_statistics_interval);
        network_observer_statistics_initialize(&observer->acknowledged_statistics, m_configuration->time_statistics_interval);
        observer->receive_window.initialize(m_configuration->window_statistics_count);
        observer->send_window.initialize(m_configuration->window_statistics_count);
        observer->reconnection_attempts = 0;
        observer->secure_connection_failure_flags = 0;
        observer->secure_connection_owner = -1;
        discard_secure_connection(index, false, _network_channel_closure_reason_none);
        observer->quality_statistics_connectivity_reported = false;
        observer->quality_statistics_reported[0] = false;
        result = index;
        observer->quality_statistics_reported[1] = false;
        observer->quality_statistics_reported[2] = false;
        observer->quality_statistics_reported[3] = false;
        observer->quality_statistics_active[0] = false;
        observer->quality_statistics_active[1] = false;
        observer->quality_statistics_active[2] = false;
        observer->quality_statistics_active[3] = false;
        }
    }
    if (result != -1)
    {
        s_channel_observer *found = &m_channel_observers[result];
        found->owner_flags |= (1 << owner);
        if ((found->flags & 2) != 0)
            m_owners[owner].owner->notify_observer_connection(result, found->owner_connection_identifier, true);
    }
    return result;
}

// @address 0x3da2e8 ?release_observer@c_network_observer@@AAAXJ@Z
void c_network_observer::release_observer(long observer_index)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    quality_statistics_report_badness(observer_index, false, "connection closed");
    discard_secure_connection(observer_index, false, _network_channel_closure_reason_observer_released);
    if (observer->channel.allocated())
        observer->channel.deallocate();
    if (observer->remote_machine_index != -1)
    {
        transport_qos_delete(observer->remote_machine_index);
        observer->remote_machine_index = -1;
    }
    clear_observer(observer_index);
}

// @address 0x3da388 ?update_observer_secure_state@c_network_observer@@AAAXJ@Z
void c_network_observer::update_observer_secure_state(long observer_index)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (observer->state != _observer_state_none)
    {
        switch (observer->state)
        {
        case _observer_state_dead:
        case _observer_state_idle:
            break;
        case _observer_state_securing:
        {
            e_transport_secure_connection status = get_secure_connection_status(observer_index);
            if (status == _transport_secure_connection_connected)
                change_observer_state(observer_index, _observer_state_waiting);
            else if (status != _transport_secure_connection_pending)
            {
                discard_secure_connection(observer_index, true, _network_channel_closure_reason_none);
                change_observer_state(observer_index, _observer_state_idle);
                observer->reconnection_attempts++;
            }
            break;
        }
        default:
            if (get_secure_connection_status(observer_index) != _transport_secure_connection_connected)
            {
                discard_secure_connection(observer_index, false, _network_channel_closure_reason_security_lost);
                change_observer_state(observer_index, _observer_state_idle);
                observer->reconnection_attempts = 0;
            }
            break;
        }
    }
}

// @address 0x3da470 ?update_observer_connection_state@c_network_observer@@AAAXJ@Z
void c_network_observer::update_observer_connection_state(long observer_index)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (observer->state != _observer_state_none)
    {
        c_network_channel *channel = &observer->channel;
        switch (observer->state)
        {
        case _observer_state_dead:
        case _observer_state_idle:
        case _observer_state_securing:
        case _observer_state_waiting:
        case _observer_state_ready:
        case _observer_state_disconnected:
            break;
        case _observer_state_connecting:
        case _observer_state_reconnecting:
            if (channel->connected())
                change_observer_state(observer_index, _observer_state_connected);
            else if (channel->closed())
            {
                bool connection_failed = false;
                switch (channel->closure_reason())
                {
                case 1:
                case 2:
                case 5:
                    connection_failed = true;
                    break;
                case 9:
                    discard_secure_connection(observer_index, true, _network_channel_closure_reason_none);
                    change_observer_state(observer_index, _observer_state_idle);
                    observer->reconnection_attempts = 0;
                    break;
                default:
                    break;
                }
                if (connection_failed)
                {
                    discard_secure_connection(observer_index, true, _network_channel_closure_reason_observer_refused);
                    change_observer_state(observer_index, _observer_state_dead);
                }
                else if (observer->state == _observer_state_connecting)
                {
                    change_observer_state(observer_index, _observer_state_ready);
                    observer->reconnection_attempts++;
                }
                else
                {
                    change_observer_state(observer_index, _observer_state_disconnected);
                    observer->reconnection_attempts++;
                }
            }
            break;
        case _observer_state_connected:
            if (!channel->connected())
            {
                if (!channel->closed())
                    change_observer_state(observer_index, _observer_state_reconnecting);
                else
                {
                    change_observer_state(observer_index, _observer_state_disconnected);
                    observer->reconnection_attempts = 0;
                }
            }
            break;
        default:
            // match: 0x3da470: unreachable hint omits dead branch check
            __assume(0);
        }
        if ((observer->flags & 2) != 0 &&
            (!channel->connected() || channel->get_identifier() != observer->owner_connection_identifier))
        {
            for (long owner_index = 0; owner_index < 4; owner_index++)
            {
                if (observer->owner_flags & (1 << owner_index))
                {
                    c_network_observer_owner *owner = m_owners[owner_index].owner;
                    owner->notify_observer_connection(observer_index, observer->owner_connection_identifier, false);
                }
            }
            observer->flags &= ~2;
            observer->owner_connection_identifier = -1;
        }
        if ((observer->flags & 2) == 0 && observer->state == _observer_state_connected)
        {
            observer->last_inactive_time = network_observer_time_get();
            observer->packet_statistics.reset();
            observer->throughput_statistics.reset();
            observer->acknowledged_statistics.reset();
            observer->receive_window.reset();
            observer->send_window.reset();
            observer->stream_last_packet_time = network_observer_time_get() - 1000;
            observer->flags |= 4;
            observer->flags |= 2;
            observer->owner_connection_identifier = channel->get_identifier();
            for (long owner_index = 0; owner_index < 4; owner_index++)
            {
                if (observer->owner_flags & (1 << owner_index))
                {
                    c_network_observer_owner *owner = m_owners[owner_index].owner;
                    owner->notify_observer_connection(observer_index, observer->owner_connection_identifier, true);
                }
            }
        }
    }
}

// 0x3da878
void c_network_observer::change_observer_state(long observer_index, e_observer_state state)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (observer->state != state)
    {
        observer->state = state;
        observer->state_change_time = network_observer_time_get();
        if (observer->state == _observer_state_dead)
        {
            for (long owner = 0; owner < 4; owner++)
            {
                if (observer->owner_flags & (1 << owner))
                    m_owners[owner].owner->observer_state_dead(observer_index);
            }
        }
    }
}

// 0x3da928
void c_network_observer::update_observer_timeout(long observer_index)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (observer->state != _observer_state_none)
    {
        bool active = false;
        c_network_channel *channel = &observer->channel;
        if (channel->connected())
        {
            long time_since_activity = network_observer_time_since(channel->last_activity_time(_network_channel_activity_send_packet));
            if (time_since_activity < m_configuration->channel_activity_timeout)
            {
                active = true;
                long time_since_inactivity = network_observer_time_since(observer->last_inactive_time);
                long time_since_receive = network_observer_time_since(channel->last_activity_time(_network_channel_activity_receive_packet));
                bool timed_out = false;
                if (observer->voice_active && (!observer->voice_talker || !observer->stream_timeout_disabled))
                {
                    if (time_since_inactivity >= m_configuration->stream_inactivity_timeout &&
                        time_since_receive >= m_configuration->stream_receive_timeout)
                        timed_out = true;
                }
                else
                {
                    if (time_since_inactivity >= m_configuration->inactivity_timeout &&
                        time_since_receive >= m_configuration->receive_timeout)
                        timed_out = true;
                }
                if (timed_out)
                    channel->close(_network_channel_closure_reason_timeout);
            }
        }
        if (!active)
            observer->last_inactive_time = network_observer_time_get();
    }
}

#define DATUM_INDEX_TO_IDENTIFIER(index) ((index) >> 16)
#define assert(expression) ((void)(expression))

static __forceinline void *observer_datum_try_and_get(const s_data_array *data, long index)
{
    void *result = 0;
    assert(data);
    assert(data->valid);
    if (index != -1)
    {
        long absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(index);
        long identifier = DATUM_INDEX_TO_IDENTIFIER(index);
        if (absolute_index < data->first_unallocated)
        {
            s_datum_header *header = (s_datum_header *)(data->data + absolute_index * data->size);
            if (header->identifier != 0 && header->identifier == identifier)
                result = header;
        }
    }
    assert(result == align_pointer(result, data->alignment_bits));
    return result;
}

static __forceinline bool observer_qos_attempt_is_complete(long qos_index)
{
    bool complete = true;
    if (transport_qos_globals.initialized)
    {
        s_transport_qos_attempt *qos = (s_transport_qos_attempt *)observer_datum_try_and_get(transport_qos_globals.data, qos_index);
        assert(qos->qos);
        if (qos)
            complete = qos->qos->unresolved == 0;
    }
    return complete;
}

// @address 0x3daab0 ?update_observer_qos@c_network_observer@@AAAXJ@Z
void c_network_observer::update_observer_qos(long observer_index)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    bool qos_in_progress = false;
    for (unsigned long index = 0; index < NUMBEROF(m_channel_observers); index++)
    {
        if (m_channel_observers[index].state == _observer_state_connected && m_channel_observers[index].remote_machine_index != -1)
        {
            qos_in_progress = true;
            break;
        }
    }

    if (TEST_BIT(observer->flags, _observer_qos_attempted_bit) && !TEST_BIT(observer->flags, _observer_qos_succeeded_bit) &&
        observer->state == _observer_state_connected)
    {
        SET_BIT(observer->flags, _observer_qos_attempted_bit, false);
    }
    if (TEST_BIT(observer->flags, _observer_qos_attempted_bit) && TEST_BIT(observer->flags, _observer_qos_succeeded_bit) &&
        observer->state == _observer_state_dead)
    {
        SET_BIT(observer->flags, _observer_qos_succeeded_bit, false);
    }

    if (!qos_in_progress && observer->remote_machine_index == -1 && !TEST_BIT(observer->flags, _observer_qos_attempted_bit))
    {
        if (observer->state == _observer_state_connected)
        {
            bool target_found = false;
            s_transport_qos_target target;
            for (long owner_index = 0; owner_index < k_network_observer_owner_count; owner_index++)
            {
                s_transport_session_description description;
                if (TEST_BIT(observer->owner_flags, owner_index) && m_owners[owner_index].session_index != -1 &&
                    ::managed_session_get_security_information(m_owners[owner_index].session_index, false, &description, 0))
                {
                    target.session_id = description.secure_identifier;
                    memcpy(&target.key, &description.secure_key, sizeof(target.key));
                    memcpy(&target.address, &observer->secure_identifier, sizeof(target.address));
                    target_found = true;
                    break;
                }
            }
            if (target_found)
                observer->remote_machine_index = transport_qos_target_new(_transport_qos_type_default, 1, &target, -1);
        }
        else if (observer->state == _observer_state_dead)
        {
            quality_statistics_notify_established_connectivity(observer_index, false);
            SET_BIT(observer->flags, _observer_qos_succeeded_bit, false);
            SET_BIT(observer->flags, _observer_qos_attempted_bit, true);
        }
    }

    if (observer->remote_machine_index != -1 && observer_qos_attempt_is_complete(observer->remote_machine_index))
    {
        bool success = transport_qos_get_result(observer->remote_machine_index, 0, &observer->qos_result);
        if (success)
        {
            observer->qos_result.data_block = 0;
            observer->qos_result.data_block_size = 0;
            SET_BIT(observer->flags, _observer_qos_succeeded_bit, true);
        }
        if (!observer->quality_statistics_ignored && !observer->quality_statistics_connectivity_reported)
        {
            network_observer_quality_connectivity_add(&m_network_quality_statistics.quality_statistics, !success);
            observer->quality_statistics_connectivity_reported = true;
        }
        transport_qos_delete(observer->remote_machine_index);
        observer->remote_machine_index = -1;
        SET_BIT(observer->flags, _observer_qos_attempted_bit, true);
    }
}

// @address 0x3dada0 ?update_undesired_connection_timers@c_network_observer@@AAA_NJW4e_network_channel_closure_reason@@@Z
bool c_network_observer::update_undesired_connection_timers(long observer_index, e_network_channel_closure_reason closure_reason)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    bool drained = true;
    c_network_channel *channel = &observer->channel;
    if (channel->connected())
    {
        if (channel->get_message_space_available() < channel->get_message_space_maximum_size_bytes())
            drained = false;
        else
            channel->close(closure_reason);
    }
    if (observer->undesired_time == 0)
        observer->undesired_time = network_observer_time_get();
    long time_since_undesired = network_observer_time_since(observer->undesired_time);
    bool timed_out = time_since_undesired > g_network_configuration.observer.undesired_connection_timeout;
    if (drained)
    {
        if (observer->drained_time == 0)
            observer->drained_time = network_observer_time_get();
        long time_since_drained = network_observer_time_since(observer->drained_time);
        timed_out = timed_out || time_since_drained > g_network_configuration.observer.undesired_connection_drained_timeout;
    }
    else
    {
        observer->drained_time = 0;
    }
    timed_out = timed_out || !transport_address_valid(&observer->secure_address);
    return timed_out;
}

// 0x3db288
float c_network_observer::packet_rate_get_maximum_given_bandwidth_bps(long bandwidth_bps, bool high_overhead, bool limit) const
{
    float maximum_packet_rate = packet_rate_maximum();
    float packet_rate = 0.0f;
    long overhead = 45 + (high_overhead ? m_configuration->stream_packet_overhead_high : m_configuration->stream_packet_overhead_low);
    float ratio = (float)bandwidth_bps / (float)(overhead << 3);
    long count = m_configuration->stream_packet_rate_count;
    long index = 0;
    if (index < count - 1)
    {
        do
        {
            float threshold = maximum_packet_rate * m_configuration->stream_packet_rate_thresholds[index];
            if (ratio >= threshold)
            {
                packet_rate = threshold;
                break;
            }
            index++;
        } while (index < count - 1);
    }
    if (packet_rate == 0.0f)
        packet_rate = maximum_packet_rate * m_configuration->stream_packet_rate_thresholds[count - 1];
    if (limit)
        packet_rate = packet_rate > maximum_packet_rate / 2.0f ? maximum_packet_rate / 2.0f : packet_rate;
    return packet_rate;
}

// @address 0x3db360 ?acquire_secure_connection@c_network_observer@@AAA_NJ@Z
bool c_network_observer::acquire_secure_connection(long observer_index)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    bool acquired = false;
    e_transport_secure_connection status = get_secure_connection_status(observer_index);
    switch (status)
    {
    case _transport_secure_connection_pending:
    case _transport_secure_connection_connected:
        acquired = true;
        break;
    default:
        discard_secure_connection(observer_index, false, _network_channel_closure_reason_none);
        break;
    }
    if (!acquired)
    {
        for (long owner_index = 0; owner_index < 4; owner_index++)
        {
            if ((observer->owner_flags & (1 << owner_index)) != 0 &&
                (observer->secure_connection_failure_flags & (1 << owner_index)) == 0)
            {
                if (build_secure_connection((e_network_observer_owner)owner_index,
                    (bool)(_rotl(observer->flags, 27) & 1), &observer->secure_identifier, &observer->secure_address))
                {
                    observer->secure_connection_owner = owner_index;
                    acquired = true;
                    break;
                }
            }
        }
    }
    return acquired;
}

// 0x3db438
bool c_network_observer::build_secure_connection(e_network_observer_owner owner, bool include_private,
    const s_transport_secure_address *secure_address, transport_address *address) const
{
    bool result = false;
    long session_index = m_owners[owner].session_index;
    s_transport_session_description description;
    transport_address remote_address;
    if (session_index != -1 &&
        ::managed_session_get_security_information(
            session_index, include_private, &description, 0) &&
        transport_secure_address_decode(&description, secure_address, 1000, &remote_address) &&
        transport_secure_connection_initiate(&remote_address))
    {
        memcpy(address, &remote_address, sizeof(remote_address));
        result = true;
    }
    if (!result)
        memset(address, 0, sizeof(*address));
    return result;
}

// 0x3db510
void c_network_observer::discard_secure_connection(long observer_index, bool failed, e_network_channel_closure_reason closure_reason)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    c_network_channel *channel = &observer->channel;
    if (!channel->closed())
        channel->close(closure_reason);
    if (transport_address_valid(&observer->secure_address))
    {
        m_message_gateway->send_pending_messages_to_address(&observer->secure_address);
        if (failed && observer->secure_connection_owner >= 0 && observer->secure_connection_owner < 4)
            observer->secure_connection_failure_flags |= 1 << observer->secure_connection_owner;
        network_observer_secure_connection_terminate(&observer->secure_address);
        memset(&observer->secure_address, 0, sizeof(observer->secure_address));
    }
}

// 0x3db698
void c_network_observer::handle_connect_request(const transport_address *incoming_address, const s_network_message_connect_request *message)
{
    long refuse_reason;
    if (transport_secure_connection_status(incoming_address) != _transport_secure_connection_connected)
    {
        refuse_reason = 4;
    }
    else
    {
        unsigned long request_flags = message->flags;
        unsigned long new_channel_flags = (request_flags >> 1) & 1;
        if ((request_flags & 1) != 0)
            new_channel_flags |= 2;
        else
            new_channel_flags &= ~2;
        if ((request_flags & 4) != 0)
            new_channel_flags |= 4;
        else
            new_channel_flags &= ~4;
        if ((request_flags & 8) != 0)
            new_channel_flags |= 8;
        else
            new_channel_flags &= ~8;
        bool known_flags = (_CountLeadingZeros(new_channel_flags & ~15) >> 5) & 1;
        bool flags_valid = known_flags && ((new_channel_flags & 1) == 0 || (new_channel_flags & 2) == 0);
        if (!flags_valid)
        {
            refuse_reason = 1;
        }
        else
        {
            bool success = false;
            s_transport_secure_address incoming_secure_address;
            long session_index;
            {
                s_transport_session_description incoming_session_description;
                session_index = managed_session_retrieve_security_information(incoming_address, _transport_platform_xenon,
                    &incoming_session_description, &incoming_secure_address);
            }
            if (session_index != -1)
            {
                for (long observer_index = 0; observer_index < 32; observer_index++)
                {
                    s_channel_observer *observer = &m_channel_observers[observer_index];
                    if (observer->state == _observer_state_none ||
                        !transport_secure_address_compare(&incoming_secure_address, &observer->secure_identifier) ||
                        observer->owner_flags == 0)
                    {
                        continue;
                    }

                    transport_address channel_address;
                    e_network_channel_state channel_state = observer->channel.get_state();
                    if (channel_state != _network_channel_state_none && channel_state != _network_channel_state_empty)
                        channel_address = observer->channel.m_remote_address;
                    unsigned long channel_remote_identifier = observer->channel.get_remote_identifier();
                    unsigned long identifier_delta = message->identifier - channel_remote_identifier;
                    bool channel_closed = channel_state <= _network_channel_state_closed;
                    if (!channel_closed)
                    {
                        if (!transport_address_equivalent(&channel_address, incoming_address, false))
                        {
                            s_transport_secure_address existing_secure_address;
                            s_transport_session_description existing_session_description;
                            managed_session_retrieve_security_information(&channel_address, _transport_platform_xenon,
                                &existing_session_description, &existing_secure_address);
                            transport_secure_connection_status(&channel_address);
                            observer->channel.close(_network_channel_closure_reason_connect_reinitiate);
                        }
                        else if (channel_remote_identifier != -1 && identifier_delta != 0)
                        {
                            observer->channel.close(_network_channel_closure_reason_connect_reinitiate);
                        }
                    }
                    if (observer->channel.closed())
                    {
                        transport_secure_connection_initiate(incoming_address);
                        observer->secure_address = *incoming_address;
                        observer->secure_connection_owner = -1;
                        for (long owner_index = 0; owner_index < k_network_observer_owner_count; owner_index++)
                        {
                            if (m_owners[owner_index].session_index == session_index)
                            {
                                observer->secure_connection_owner = owner_index;
                                break;
                            }
                        }
                        observer->channel.open(incoming_address, false, 0xffffffff);
                        change_observer_state(observer_index, _observer_state_connecting);
                        observer->reconnection_attempts = 0;
                        update_observer_connection_state(observer_index);
                    }
                    observer->channel.establish(message->identifier);
                    update_observer_secure_state(observer_index);
                    update_observer_connection_state(observer_index);
                    success = true;
                    break;
                }
                if (success)
                    return;
            }
            refuse_reason = 3;
        }
    }
    s_network_message_connect_refuse refuse;
    refuse.remote_identifier = message->identifier;
    refuse.reason = refuse_reason;
    m_message_gateway->send_message_directed(incoming_address, _network_message_type_connect_refuse, sizeof(refuse), &refuse);
}
// 0x3db9b0
void c_network_observer::handle_packet_event(c_network_channel *channel, e_network_packet_event packet_event, long sequence_number,
    long packet_size_bytes, long round_trip_msec)
{
    s_channel_observer *observer = find_observer_by_channel(channel);
    long size = packet_size_bytes;

    if (round_trip_msec != -1 && (packet_event == _network_packet_event_acknowledged ||
        packet_event == _network_packet_event_delivered || packet_event == _network_packet_event_acknowledged_late))
    {
        observer->acknowledged_statistics.add_event(size);
        if (observer->voice_active)
            observer->period_bytes_sent += size;

        if (observer->latency_time != -1)
        {
            long time_since_latency = network_observer_time_since(observer->latency_time);
            long latency_change = round_trip_msec - observer->latency_average;
            long drift_change = time_since_latency - observer->latency_drift;
            observer->latency_average += latency_change >> m_configuration->stream_latency_average_shift;
            observer->latency_deviation += (abs(latency_change) - observer->latency_deviation) >>
                g_network_configuration.latency_deviation_shift;
            observer->latency_drift += drift_change >> m_configuration->stream_latency_average_shift;

            long total_timeout;
            if (observer->voice_active)
            {
                long timeout_margin = g_network_configuration.stream_timeout_deviation_scale * observer->latency_deviation;
                timeout_margin = timeout_margin > g_network_configuration.stream_timeout_minimum_margin
                    ? timeout_margin
                    : g_network_configuration.stream_timeout_minimum_margin;
                total_timeout = observer->latency_average + timeout_margin;
                total_timeout = total_timeout > g_network_configuration.stream_timeout_minimum
                    ? total_timeout
                    : g_network_configuration.stream_timeout_minimum;
            }
            else
            {
                long timeout_margin = g_network_configuration.timeout_deviation_scale * observer->latency_deviation;
                timeout_margin = timeout_margin > g_network_configuration.timeout_minimum_margin
                    ? timeout_margin
                    : g_network_configuration.timeout_minimum_margin;
                total_timeout = observer->latency_average + timeout_margin;
                total_timeout = total_timeout > g_network_configuration.timeout_minimum
                    ? total_timeout
                    : g_network_configuration.timeout_minimum;
            }
            long packet_timeout = total_timeout + observer->latency_drift;
            observer->packet_timeout = packet_timeout;
            observer->channel.set_connection_packet_timeout(packet_timeout);

            if (!observer->voice_active)
            {
                observer->latency_idle_deviation = observer->latency_deviation;
            }
            else
            {
                if (observer->latency_average > observer->latency_average_maximum)
                    observer->latency_average_maximum = observer->latency_average;
                if (observer->latency_deviation > observer->latency_deviation_maximum)
                    observer->latency_deviation_maximum = observer->latency_deviation;
                if (observer->latency_average < observer->period_latency_minimum)
                    observer->period_latency_minimum = observer->latency_average;
                if (observer->latency_average > observer->period_latency_maximum)
                    observer->period_latency_maximum = observer->latency_average;
            }
        }
        observer->latency_time = network_observer_time_get();
    }

    if (packet_event == _network_packet_event_lost)
    {
        observer->packets_lost++;
        observer->receive_window.add_event(1);
    }
    if (packet_event == _network_packet_event_delivered || packet_event == _network_packet_event_delivered_late)
    {
        observer->packets_received++;
        observer->receive_window.add_event(0);
    }
}

// 0x3dbbe0
long network_observer_initialize_stream_bandwidth(c_network_observer *observer, long observer_index, long client_count, long maximum_bandwidth_bps,
    long initial_bandwidth_bps, long minimum_bandwidth_bps, const char *unused)
{
    c_network_observer::s_channel_observer *channel_observer = &observer->m_channel_observers[observer_index];
    long bandwidth_bps = channel_observer->stream_maximum_bandwidth_bps;
    if (bandwidth_bps > maximum_bandwidth_bps)
        bandwidth_bps = maximum_bandwidth_bps;
    if (minimum_bandwidth_bps > channel_observer->stream_minimum_bandwidth_bps)
        minimum_bandwidth_bps = channel_observer->stream_minimum_bandwidth_bps;
    if (bandwidth_bps > minimum_bandwidth_bps)
        minimum_bandwidth_bps = bandwidth_bps;
    channel_observer->stream_minimum_bandwidth_bps = minimum_bandwidth_bps;
    if (!channel_observer->stream_initialized)
    {
        channel_observer->stream_initialized = true;
        if (channel_observer->stream_packet_rate_halved && channel_observer->estimated_bandwidth_bps && client_count)
        {
            long estimated_bandwidth_bps = observer->m_configuration->stream_initial_client_bandwidth_percentage *
                channel_observer->estimated_bandwidth_bps / 100 / client_count;
            if (bandwidth_bps > estimated_bandwidth_bps)
                bandwidth_bps = estimated_bandwidth_bps;
        }
        else if (channel_observer->estimated_bandwidth_bps)
        {
            long estimated_bandwidth_bps = observer->m_configuration->stream_initial_bandwidth_percentage * channel_observer->estimated_bandwidth_bps / 100;
            if (bandwidth_bps > estimated_bandwidth_bps)
                bandwidth_bps = estimated_bandwidth_bps;
        }
        if (bandwidth_bps > initial_bandwidth_bps)
            bandwidth_bps = initial_bandwidth_bps;
        observer->stream_set_bandwidth(observer_index, bandwidth_bps);
    }
    else if (channel_observer->stream_bandwidth_bps > bandwidth_bps)
        observer->stream_set_bandwidth(observer_index, bandwidth_bps);
    return bandwidth_bps;
}

class c_online_file_manager;


static __forceinline void network_observer_set_upload_quota(long bandwidth_bps)
{
    c_online_file_manager::get()->set_upstream_quota(bandwidth_bps);
}

// 0x3dbd08
long network_observer_update_stream_bandwidth(c_network_observer *observer, long total_bandwidth_bps, long stable_bandwidth_bps, bool stable,
    bool voice, long stream_count, long host_count, long client_count, unsigned long growing_mask, long requiring_count)
{
    long host_and_client_count = host_count + client_count;
    long bandwidth_cap;
    if (observer->m_stream_low_bandwidth)
    {
        long initial_bps = observer->m_bandwidth_bps * observer->m_configuration->stream_initial_low_percentage / 100;
        long maximum_bps = observer->m_configuration->stream_initial_low_maximum_bps;
        bandwidth_cap = initial_bps > maximum_bps ? maximum_bps : initial_bps;
    }
    else
    {
        long initial_bps = observer->m_bandwidth_bps * observer->m_configuration->stream_initial_percentage / 100;
        long maximum_bps = observer->m_configuration->stream_initial_maximum_bps;
        bandwidth_cap = initial_bps > maximum_bps ? maximum_bps : initial_bps;
    }

    s_network_quality_data quality;
    long latency, loss, flags;
    if (network_session_interface_get_network_quality(&quality, &latency, &loss, &flags))
    {
        quality.upstream_bandwidth *= observer->m_configuration->stream_quality_percentage;
        bandwidth_cap = bandwidth_cap > quality.upstream_bandwidth / 100 ? quality.upstream_bandwidth / 100 : bandwidth_cap;
    }
    if (observer->m_stream_total_bandwidth_average != -1)
        bandwidth_cap = observer->m_stream_total_bandwidth_average;
    else if (observer->m_stream_peak_bandwidth_bps > bandwidth_cap)
        bandwidth_cap = observer->m_stream_peak_bandwidth_bps;
    if (total_bandwidth_bps > bandwidth_cap)
        bandwidth_cap = total_bandwidth_bps;

    long observer_index;
    if (stream_count > 0)
    {
        long voice_bandwidth_bps = 0;
        bool low_priority_voice = false;
        for (observer_index = 0; observer_index < 32; ++observer_index)
        {
            c_network_observer::s_channel_observer *channel_observer = &observer->m_channel_observers[observer_index];
            if (channel_observer->state != _observer_state_none && channel_observer->voice_active && channel_observer->voice_talker &&
                channel_observer->stream_timeout_disabled)
            {
                if (channel_observer->voice_listener)
                {
                    voice_bandwidth_bps += observer->m_configuration->stream_voice_low_host_minimum_bps;
                    low_priority_voice = true;
                }
                else
                    voice_bandwidth_bps += observer->m_configuration->stream_voice_host_minimum_bps;
            }
        }
        if (voice_bandwidth_bps != 0 && !low_priority_voice)
            voice_bandwidth_bps += g_network_configuration.minimum_bandwidth_bps;
        if (voice_bandwidth_bps > bandwidth_cap)
            bandwidth_cap = voice_bandwidth_bps;
    }
    observer->m_stream_bandwidth_cap_bps = bandwidth_cap;
    if (stream_count > 0)
    {
        long reserved_voice_channels = 0;
        long allocated_stream_count = 0;
        long remaining_bandwidth_bps = bandwidth_cap;
        long minimum_primary_bandwidth = host_count * observer->m_configuration->stream_minimum_host_bps;
        if (host_count <= 0)
            minimum_primary_bandwidth = host_and_client_count * observer->m_configuration->stream_minimum_host_bps;
        long voice_channel_count = stream_count - host_and_client_count;
        if (host_count > 0)
            voice_channel_count += client_count;
        if (voice_channel_count > 0 && remaining_bandwidth_bps >= minimum_primary_bandwidth + g_network_configuration.minimum_bandwidth_bps)
        {
            ++reserved_voice_channels;
            for (observer_index = 0; observer_index < 32; ++observer_index)
            {
                c_network_observer::s_channel_observer *channel_observer = &observer->m_channel_observers[observer_index];
                if (channel_observer->state != _observer_state_none && channel_observer->voice_active &&
                    (!channel_observer->stream_packet_rate_limited || (channel_observer->stream_packet_rate_limited && host_count > 0)) &&
                    !channel_observer->voice_talker)
                {
                    long bandwidth = channel_observer->stream_maximum_bandwidth_bps > g_network_configuration.minimum_bandwidth_bps
                        ? g_network_configuration.minimum_bandwidth_bps : channel_observer->stream_maximum_bandwidth_bps;
                    channel_observer->stream_minimum_bandwidth_bps = bandwidth;
                    observer->stream_set_bandwidth(observer_index, bandwidth);
                    if (allocated_stream_count == 0)
                        remaining_bandwidth_bps -= g_network_configuration.minimum_bandwidth_bps;
                    ++allocated_stream_count;
                }
            }
        }
        long remaining_stream_count = stream_count - allocated_stream_count;
        if (remaining_stream_count != 0)
        {
            if (host_count == 0 && client_count > 0)
            {
                long bandwidth = remaining_bandwidth_bps / host_and_client_count;
                if (bandwidth > observer->m_configuration->stream_desired_bps)
                    bandwidth = observer->m_configuration->stream_desired_bps;
                for (observer_index = 0; observer_index < 32; ++observer_index)
                {
                    c_network_observer::s_channel_observer *channel_observer = &observer->m_channel_observers[observer_index];
                    if (channel_observer->state != _observer_state_none && channel_observer->voice_active && channel_observer->stream_packet_rate_limited)
                    {
                        remaining_bandwidth_bps -= network_observer_initialize_stream_bandwidth(observer, observer_index, stream_count, bandwidth,
                            observer->m_configuration->stream_minimum_game_bps, observer->m_configuration->stream_desired_bps, "load-bearing");
                        --remaining_stream_count;
                        ++voice_channel_count;
                    }
                }
            }
            if (remaining_stream_count != 0)
            {
                if (!g_network_configuration.observer.ignore_badness)
                {
                    long available_bandwidth_bps = remaining_bandwidth_bps / remaining_stream_count;
                    if (available_bandwidth_bps > observer->m_configuration->stream_desired_high_priority_bps)
                        available_bandwidth_bps = observer->m_configuration->stream_desired_high_priority_bps;
                    for (observer_index = 0; observer_index < 32; ++observer_index)
                    {
                        c_network_observer::s_channel_observer *channel_observer = &observer->m_channel_observers[observer_index];
                        if (channel_observer->state != _observer_state_none && channel_observer->voice_active && channel_observer->voice_talker &&
                            channel_observer->latency_threshold != -1)
                        {
                            long minimum_bandwidth, minimum_budget, minimum_throughput;
                            if (channel_observer->voice_listener)
                            {
                                minimum_bandwidth = observer->m_configuration->stream_low_bandwidth_bps;
                                minimum_budget = observer->m_configuration->stream_low_bandwidth_budget_bps;
                                minimum_throughput = observer->m_configuration->stream_high_bandwidth_bps;
                            }
                            else
                            {
                                minimum_bandwidth = observer->m_configuration->stream_high_bandwidth_bps;
                                minimum_budget = observer->m_configuration->stream_high_bandwidth_budget_bps;
                                minimum_throughput = observer->m_configuration->stream_low_bandwidth_bps;
                            }
                            if (channel_observer->stream_bandwidth_bps < minimum_bandwidth && available_bandwidth_bps > minimum_budget)
                            {
                                if (++channel_observer->bad_bandwidth_intervals == observer->m_configuration->stream_bad_bandwidth_intervals)
                                    observer->stream_report_bad(observer_index, "throttled upstream");
                            }
                            else
                                channel_observer->bad_bandwidth_intervals = 0;
                            if ((long)(channel_observer->packet_statistics.average_values_per_second() * 8.0f) < minimum_throughput)
                            {
                                if (++channel_observer->bad_throughput_intervals == observer->m_configuration->stream_bad_bandwidth_intervals)
                                    observer->stream_report_bad(observer_index, "throttled downstream");
                            }
                            else
                                channel_observer->bad_throughput_intervals = 0;
                            if (channel_observer->latency_average > observer->m_configuration->stream_bad_latency)
                            {
                                if (++channel_observer->bad_latency_intervals == observer->m_configuration->stream_bad_latency_intervals)
                                    observer->stream_report_bad(observer_index, "high latency");
                            }
                            else
                                channel_observer->bad_latency_intervals = 0;
                            if (channel_observer->loss_average > observer->m_configuration->stream_bad_loss_average)
                            {
                                if (++channel_observer->bad_loss_intervals == observer->m_configuration->stream_bad_loss_intervals)
                                    observer->stream_report_bad(observer_index, "packet loss");
                            }
                            else
                                channel_observer->bad_loss_intervals = 0;
                            if (channel_observer->packet_loss_events >= observer->m_configuration->stream_bad_packet_loss_events)
                            {
                                if (++channel_observer->bad_packet_loss_intervals == observer->m_configuration->stream_bad_packet_loss_intervals)
                                    observer->stream_report_bad(observer_index, "packet loss spike");
                            }
                            else
                                channel_observer->bad_packet_loss_intervals = 0;
                            channel_observer->packet_loss_events = 0;
                        }
                    }
                }
                long bandwidth = remaining_bandwidth_bps / remaining_stream_count;
                for (observer_index = 0; observer_index < 32; ++observer_index)
                {
                    c_network_observer::s_channel_observer *channel_observer = &observer->m_channel_observers[observer_index];
                    if (channel_observer->state != _observer_state_none && channel_observer->voice_active && channel_observer->stream_packet_rate_halved)
                    {
                        long minimum, desired;
                        if (channel_observer->stream_timeout_disabled)
                        {
                            minimum = observer->m_configuration->stream_voice_high_minimum_bps;
                            desired = observer->m_configuration->stream_voice_high_desired_bps;
                        }
                        else
                        {
                            minimum = observer->m_configuration->stream_minimum_game_bps;
                            desired = observer->m_configuration->stream_desired_high_priority_bps;
                        }
                        remaining_bandwidth_bps -= network_observer_initialize_stream_bandwidth(
                            observer, observer_index, stream_count, (bandwidth > desired ? desired : bandwidth), minimum, desired,
                            "client->host");
                        --remaining_stream_count;
                        ++voice_channel_count;
                    }
                }
                if (remaining_stream_count != 0)
                {
                    bandwidth = remaining_bandwidth_bps / remaining_stream_count;
                    for (observer_index = 0; observer_index < 32; ++observer_index)
                    {
                        c_network_observer::s_channel_observer *channel_observer = &observer->m_channel_observers[observer_index];
                        if (channel_observer->state != _observer_state_none && channel_observer->voice_active && channel_observer->voice_listener &&
                            (!channel_observer->stream_timeout_disabled || !channel_observer->stream_voice_active))
                        {
                            long minimum, desired;
                            if (channel_observer->stream_timeout_disabled)
                            {
                                if (voice && !channel_observer->stream_voice_active)
                                {
                                    desired = observer->m_configuration->stream_voice_shared_desired_bps;
                                    minimum = observer->m_configuration->stream_minimum_game_bps;
                                }
                                else
                                {
                                    minimum = observer->m_configuration->stream_voice_low_minimum_bps;
                                    desired = observer->m_configuration->stream_voice_low_desired_bps;
                                }
                            }
                            else
                            {
                                desired = observer->m_configuration->stream_desired_low_priority_bps;
                                minimum = observer->m_configuration->stream_minimum_game_bps;
                            }
                            remaining_bandwidth_bps -= network_observer_initialize_stream_bandwidth(
                                observer, observer_index, stream_count, (bandwidth > desired ? desired : bandwidth), minimum, desired,
                                "host->client");
                            --remaining_stream_count;
                            ++voice_channel_count;
                        }
                    }
                    if (remaining_stream_count != 0)
                    {
                        bandwidth = remaining_bandwidth_bps / remaining_stream_count;
                        for (observer_index = 0; observer_index < 32; ++observer_index)
                        {
                            c_network_observer::s_channel_observer *channel_observer = &observer->m_channel_observers[observer_index];
                            if (channel_observer->state != _observer_state_none && channel_observer->voice_active && channel_observer->voice_listener &&
                                channel_observer->stream_timeout_disabled && channel_observer->stream_voice_active)
                            {
                                long desired = observer->m_configuration->stream_voice_active_desired_bps;
                                long minimum = observer->m_configuration->stream_voice_active_minimum_bps;
                                remaining_bandwidth_bps -= network_observer_initialize_stream_bandwidth(
                                    observer, observer_index, stream_count, (bandwidth > desired ? desired : bandwidth),
                                    minimum, desired, "host->client (joining)");
                                ++voice_channel_count;
                            }
                        }
                    }
                }
            }
        }
        if (remaining_bandwidth_bps >= g_network_configuration.minimum_bandwidth_bps &&
            voice_channel_count > reserved_voice_channels && !observer->m_stream_disable_expansion)
        {
            long affordable_channels = remaining_bandwidth_bps / g_network_configuration.minimum_bandwidth_bps;
            long extra_channels = voice_channel_count - reserved_voice_channels;
            if (extra_channels > affordable_channels)
                extra_channels = affordable_channels;
            remaining_bandwidth_bps -= extra_channels * g_network_configuration.minimum_bandwidth_bps;
            voice_channel_count += extra_channels;
        }
        if (g_voice_interface_globals.initialized)
        {
            g_voice_interface_globals.game_channel_count = voice_channel_count;
            g_voice_interface_globals.lobby_channel_count = voice_channel_count;
        }
        if (growing_mask != 0 && !observer->m_stream_disable_expansion)
        {
            observer->m_stream_remaining_bandwidth_bps = 0;
            network_observer_set_upload_quota(0);
        }
        else
        {
            observer->m_stream_remaining_bandwidth_bps = remaining_bandwidth_bps;
            network_observer_set_upload_quota(remaining_bandwidth_bps);
        }
    }
    else
    {
        observer->m_stream_remaining_bandwidth_bps = bandwidth_cap;
        network_observer_set_upload_quota(bandwidth_cap);
    }
    return (unsigned long)requiring_count > 0;
}

// 0x3dc780
void c_network_observer::stream_initialize(long observer_index)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    observer->latency_time = -1;
    observer->latency_average = -1;
    observer->latency_drift = 0;
    observer->latency_idle_deviation = g_network_configuration.initial_latency_deviation;
    observer->latency_deviation = g_network_configuration.initial_latency_deviation;
    observer->packet_timeout = m_configuration->stream_initial_packet_timeout;
    observer->channel.set_connection_packet_timeout(observer->packet_timeout);
    memset(&observer->voice_active, 0,
        offsetof(s_channel_observer, quality_statistics_connectivity_reported) - offsetof(s_channel_observer, voice_active));
    observer->voice_active = true;
    observer->stream_packet_rate_limited = false;
    observer->voice_talker = observer->channel.simulation_attached();
    observer->voice_listener = observer->channel.simulation_is_authority();
    observer->stream_packet_rate_halved = observer->channel.simulation_attached() && !observer->channel.simulation_is_authority();
    observer->stream_sent_bps = -1;
    observer->period_latency_minimum = -1;
    observer->period_latency_maximum = -1;
    observer->period_start_latency = -1;
    observer->baseline_latency = -1;
    observer->baseline_loss = -1;
    observer->latency_deviation_maximum = -1;
    observer->latency_average_maximum = -1;
    observer->previous_interval_latency = -1;
    observer->probe_latency_threshold = -1;
    observer->spike_latency_threshold = -1;
    observer->probe_bandwidth_bps = -1;
    observer->failed_probes = 0;
    observer->successful_probes = 0;
    observer->stream_minimum_bandwidth_bps = m_configuration->stream_minimum_bandwidth_bps;
    observer->stream_maximum_bandwidth_bps = m_configuration->stream_maximum_bandwidth_bps + 1024;
    observer->stream_wants_to_grow = false;
    observer->stream_bandwidth_bps = 0;
    if (observer->voice_talker)
        network_observer_bandwidth_monitor_start(observer_index);
    observer->latency_threshold = -1;
    observer->loss_threshold = -1;
    observer->latency_threshold_valid = false;
    stream_set_bandwidth(observer_index, m_configuration->stream_minimum_bandwidth_bps);
    m_stream_bandwidth_changed = true;
}

// 0x3dc918
void c_network_observer::stream_set_bandwidth(long observer_index, long bandwidth_bps)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (observer->stream_bandwidth_bps != bandwidth_bps)
    {
        float packet_rate = packet_rate_get_maximum_given_bandwidth_bps(bandwidth_bps, observer->voice_talker, observer->stream_packet_rate_halved);
        stream_set_parameters(observer_index, bandwidth_bps, packet_rate);
    }
}

// 0x3dc9f8
void c_network_observer::bandwidth_monitor_events(long observer_index, long unused, long bandwidth)
{

  s_channel_observer *observer = &m_channel_observers[observer_index];
  int minimum_bandwidth;
  int maximum_bandwidth;
  int interval_count;
  int max_sample;

  minimum_bandwidth = observer->bandwidth_minimum > bandwidth ? bandwidth : observer->bandwidth_minimum;
  observer->bandwidth_minimum = minimum_bandwidth;

  if (observer->bandwidth_maximum > bandwidth)
    maximum_bandwidth = observer->bandwidth_maximum;
  else
    maximum_bandwidth = bandwidth;
  observer->bandwidth_maximum = maximum_bandwidth;

  interval_count = observer->bandwidth_sample_count + 1;
  observer->bandwidth_sample_count = interval_count;
  if (interval_count < m_configuration->bandwidth_monitor_interval)
    return;

  max_sample = observer->bandwidth_current_maximum;
  if (!(maximum_bandwidth <= max_sample))
    max_sample = maximum_bandwidth;

  if (observer->bandwidth_interval_count != 0) {
    if (minimum_bandwidth == 0) {
      observer->bandwidth_events[2]++;
    } else if (max_sample <= m_configuration->bandwidth_monitor_threshold) {
      observer->bandwidth_events[4]++;
    } else if (minimum_bandwidth < m_configuration->bandwidth_monitor_ratio * (float)max_sample) {
      observer->bandwidth_events[3]++;
    } else {
      observer->bandwidth_events[0]++;
    }
  }

  observer->bandwidth_current_maximum = maximum_bandwidth;
  observer->bandwidth_interval_count++;
  observer->bandwidth_sample_count = 0;
  observer->bandwidth_minimum = 0x100000;
  observer->bandwidth_maximum = 0;
}

// @address 0x3db638 ?get_secure_connection_status@c_network_observer@@ABA?AW4e_transport_secure_connection@@J@Z
e_transport_secure_connection c_network_observer::get_secure_connection_status(long observer_index) const
{
    s_channel_observer *observer = (s_channel_observer *)&m_channel_observers[observer_index];
    e_transport_secure_connection result = _transport_secure_connection_idle;
    if (transport_address_valid(&observer->secure_address))
        result = transport_secure_connection_status(&observer->secure_address);
    return result;
}

// @address 0x3daf28 ?update_observer_connection_initiation@c_network_observer@@AAAXJ@Z
void c_network_observer::update_observer_connection_initiation(long observer_index)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (observer->state == _observer_state_none)
        return;
    bool retry = false;
    if (observer->state == _observer_state_dead &&
        network_observer_time_since(observer->state_change_time) >= m_configuration->dead_observer_retry_delay)
    {
        change_observer_state(observer_index, _observer_state_idle);
        s_channel_observer *reset_observer = &m_channel_observers[observer_index];
        reset_observer->reconnection_attempts = 0;
        observer->secure_connection_failure_flags = 0;
        observer->secure_connection_owner = -1;
        discard_secure_connection(observer_index, false, _network_channel_closure_reason_none);
        update_observer_secure_state(observer_index);
        update_observer_connection_state(observer_index);
        update_observer_connection_initiation(observer_index);
    }
    if (observer->state == _observer_state_waiting && (observer->flags & 1) != 0)
    {
        change_observer_state(observer_index, _observer_state_ready);
        observer->reconnection_attempts = 0;
    }
    if (observer->state != _observer_state_idle && observer->state != _observer_state_ready &&
        observer->state != _observer_state_disconnected)
        return;
    bool proceed = false;
    long retry_count;
    const long *retry_timeouts;
    if (observer->state == _observer_state_idle)
    {
        retry_count = m_configuration->initiation_retry[0].retry_count;
        retry_timeouts = m_configuration->initiation_retry[0].retry_timeouts;
        // match: 0x3daf28: value range assumption guides branch lowering
        __assume(retry_count >= 0);
    }
    else if ((observer->flags & 4) == 0)
    {
        retry_count = m_configuration->initiation_retry[1].retry_count;
        retry_timeouts = m_configuration->initiation_retry[1].retry_timeouts;
        // match: 0x3daf28: value range assumption guides branch lowering
        __assume(retry_count >= 0);
    }
    else if (m_stream_has_host && observer->voice_active && !observer->voice_talker)
    {
        retry_count = m_configuration->initiation_retry[2].retry_count;
        retry_timeouts = m_configuration->initiation_retry[2].retry_timeouts;
        // match: 0x3daf28: value range assumption guides branch lowering
        __assume(retry_count >= 0);
    }
    else
    {
        retry_count = m_configuration->initiation_retry[3].retry_count;
        retry_timeouts = m_configuration->initiation_retry[3].retry_timeouts;
        // match: 0x3daf28: value range assumption guides branch lowering
        __assume(retry_count >= 0);
    }
    if (observer->reconnection_attempts == 0 && (observer->flags & 4) == 0)
    {
        proceed = true;
    }
    else
    {
        for (long owner_index = 0; owner_index < 4; owner_index++)
        {
            if (observer->owner_flags & (1 << owner_index))
            {
                c_network_observer_owner *owner = m_owners[owner_index].owner;
                if (owner->should_initiate_connection(observer_index, (bool)(_rotl(observer->flags, 30) & 1)))
                    proceed = true;
            }
        }
    }
    if (!proceed)
    {
        if (update_undesired_connection_timers(observer_index, _network_channel_closure_reason_none))
        {
            discard_secure_connection(observer_index, false, _network_channel_closure_reason_none);
            change_observer_state(observer_index, _observer_state_dead);
        }
    }
    else
    {
        memset(&observer->undesired_time, 0, sizeof(observer->undesired_time) + sizeof(observer->drained_time));
        short retry_attempts = observer->reconnection_attempts;
        if ((observer->reconnection_attempts) < 0 || retry_attempts >= retry_count)
        {
            discard_secure_connection(observer_index, false, _network_channel_closure_reason_none);
            change_observer_state(observer_index, _observer_state_dead);
        }
        else
        {
            long retry_timeout = retry_timeouts[retry_attempts];
            retry = network_observer_time_since(observer->state_change_time) >= retry_timeout;
        }
    }
    if (!retry)
        return;
    if (observer->state == _observer_state_idle)
    {
        if (acquire_secure_connection(observer_index))
            change_observer_state(observer_index, _observer_state_securing);
        else
            change_observer_state(observer_index, _observer_state_dead);
        return;
    }
    else
    {
        observer->channel.open(&observer->secure_address, true, 0xffffffff);
        if (observer->state == _observer_state_ready)
            change_observer_state(observer_index, _observer_state_connecting);
        else
            change_observer_state(observer_index, _observer_state_reconnecting);
    }
}


// @address 0x1238a8 ?monitor@c_network_observer@@SAXPAV1@@Z
void c_network_observer::monitor(c_network_observer *observer)
{
    observer = g_network_observer;
    for (long observer_index = 0; observer_index < 32; observer_index++)
    {
        s_channel_observer *channel_observer = &observer->m_channel_observers[observer_index];
        if (channel_observer->state != _observer_state_none)
        {
            if (!channel_observer->owner_flags &&
                observer->update_undesired_connection_timers(observer_index, _network_channel_closure_reason_observer_released))
                observer->release_observer(observer_index);
            if (channel_observer->state != _observer_state_none)
            {
                observer->update_observer_secure_state(observer_index);
                observer->update_observer_connection_state(observer_index);
                observer->update_observer_connection_initiation(observer_index);
                observer->update_observer_timeout(observer_index);
                observer->update_observer_qos(observer_index);
            }
        }
    }
    observer->stream_update();
    observer->update_voice_repeater_use();
}

// @address 0x3d9e28 ?observer_channel_initiate_connection@c_network_observer@@QAAXW4e_network_observer_owner@@J@Z
void c_network_observer::observer_channel_initiate_connection(e_network_observer_owner owner, long observer_index)
{
    s_channel_observer *observer = &m_channel_observers[observer_index];
    if (observer->state == _observer_state_dead)
    {
        change_observer_state(observer_index, _observer_state_idle);
        observer->reconnection_attempts = 0;
    }
    observer->flags |= 1;
    update_observer_secure_state(observer_index);
    update_observer_connection_state(observer_index);
    update_observer_connection_initiation(observer_index);
}
