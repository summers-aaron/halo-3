#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_OBSERVER_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_OBSERVER_H
#include "networking/delivery/network_channel.h"
#include "networking/transport/transport_security_types.h"
#include "networking/network_observer_owner_types.h"
#include "networking/observer_state_types.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/session/network_observer_configuration.h"
#include "networking/network_statistics.h"
#include "networking/logic/network_bandwidth_globals.h"
#include "networking/transport/transport_qos_types.h"
#include "networking/logic/network_quality_statistics_types.h"
#include "cseries/crt_xdk.h"
#include <string.h>
#include <stddef.h>
#include "networking/transport/transport_unique_identifier_types.h"
class c_network_channel_owner;
class c_network_observer_owner;
class c_network_link;
class c_network_message_gateway;
class c_network_message_handler;
class c_network_message_type_collection;
struct s_network_message_connect_request;


class c_network_observer
{
public:
    c_network_observer();
    struct s_channel_observer;
    __forceinline bool get_simple_status(long observer_index, long *rtt_msec, float *packet_rate, long *bandwidth, long *packet_loss)
    {
        bool result = false;
        if (observer_index >= 0 && observer_index < 32) {
            s_channel_observer *observer = &m_channel_observers[observer_index];
            if (observer->is_established() && observer->is_connected() && observer->get_latency_threshold() != -1) {
                *rtt_msec = observer->get_rtt();
                *packet_rate = observer->get_packet_rate();
                *bandwidth = observer->get_bandwidth();
                *packet_loss = (long)(observer->receive_window.average_values_in_window() * 100.0f);
                result = true;
            }
        }
        return result;
    }

    friend class c_network_session;
    friend class c_network_session_membership;
    friend class c_simulation_view;
    friend void network_memory_recreate_channels();
    __forceinline void destroy_channels();
    __forceinline c_network_channel *channel_iterate(const c_network_channel *channel);
    __forceinline s_network_quality_statistics *quality_statistics_get()
    {
        return &m_network_quality_statistics;
    }
    __forceinline void notify_channel_packet_received(c_network_channel *channel, long size_on_wire);
    __forceinline void notify_channel_packet_sent(c_network_channel *channel, long size_on_wire, long game_bits, long filler_bits, long voice_bytes);
    __forceinline void observer_channel_release(e_network_observer_owner owner, long index)
    {
        s_channel_observer *observer = &m_channel_observers[index];
        observer->owner_flags &= ~(1 << owner);
    }
    __forceinline void observer_clear_security_flags()
    {
        for (long i = 0; i < 32; ++i)
            m_channel_observers[i].flags &= ~32;
    }
    __forceinline c_network_channel *observer_channel_get_network_channel(e_network_observer_owner owner, long observer_index)
    {
        return &m_channel_observers[observer_index].channel;
    }
    __forceinline void destroy_observer()
    {
        for (long observer_index = 0; observer_index < 32; ++observer_index)
            if (m_channel_observers[observer_index].state)
                release_observer(observer_index);
        for (long owner_index = 0; owner_index < 4; ++owner_index)
        {
            m_owners[owner_index].owner = 0;
            m_owners[owner_index].session_index = -1;
        }
        m_configuration = 0;
        m_message_types = 0;
        m_link = 0;
    }
    __forceinline void register_owner(e_network_observer_owner owner, c_network_channel_owner *channel_owner)
    {
        m_owners[owner].owner = (c_network_observer_owner *)channel_owner;
        m_owners[owner].session_index = -1;
    }
    __forceinline void update_owner(e_network_observer_owner owner, long managed_session_index)
    {
        m_owners[owner].session_index = managed_session_index;
    }
    s_channel_observer *get_observer(e_network_observer_owner owner, long observer_index)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        return observer;
    }
    bool observer_channel_connected(e_network_observer_owner owner, long observer_index) const
    {
        return get_observer(owner, observer_index)->state == _observer_state_connected;
    }
    __forceinline bool observer_channel_dead(e_network_observer_owner owner, long observer_index) const
    {
        return m_channel_observers[observer_index].state == _observer_state_dead;
    }
    __forceinline void observer_channel_set_quality_statistics_ignored(long observer_channel_index, bool ignored)
    {
        m_channel_observers[observer_channel_index].quality_statistics_ignored = ignored;
    }
    __forceinline void observer_channel_set_peer_properties(long observer_channel_index, long upstream_bandwidth, long downstream_bandwidth)
    {
        s_channel_observer *channel_observer = &m_channel_observers[observer_channel_index];
        channel_observer->downstream_bandwidth_bps = downstream_bandwidth;
        channel_observer->estimated_bandwidth_bps = upstream_bandwidth;
    }
    __forceinline void network_session_mark_message_waiting(long index, long type)
    {
        s_channel_observer *observer = &m_channel_observers[index];
        if (observer->channel.get_state() == _network_channel_state_connected)
        {
            unsigned __int64 waiting = observer->message_type_flags;
            if ((waiting & (1ui64 << type)) == 0)
                observer->message_type_flags = waiting | (1ui64 << type);
        }
    }
    void observer_channel_set_waiting_on_backlog(e_network_observer_owner owner, long observer_index, e_network_message_type message_type)
    {
        s_channel_observer *observer = get_observer(owner, observer_index);
        c_network_channel *channel = &observer->channel;
        if (channel->connected())
        {
            if (!(observer->message_type_flags & (1i64 << message_type)))
                observer->message_type_flags |= (1i64 << message_type);
        }
    }
    struct s_channel_observer
    {
        __forceinline bool is_established() const { return state != 0; }
        __forceinline bool is_connected() const { return voice_active; }
        __forceinline long get_latency_threshold() const { return latency_threshold; }
        __forceinline long get_rtt() const { return latency_average; }
        __forceinline float get_packet_rate() const { return stream_packet_rate; }
        __forceinline long get_bandwidth() const { return stream_sent_bps; }
        c_network_channel channel;
        e_observer_state state;
        unsigned long state_change_time;
        unsigned char flags;
        unsigned char owner_flags;
        short reconnection_attempts;
        unsigned long owner_connection_identifier;
        s_transport_secure_address secure_identifier;
        unsigned long secure_connection_failure_flags;
        long secure_connection_owner;
        transport_address secure_address;
        unsigned long latency_time;
        long latency_average;
        long latency_deviation;
        long latency_idle_deviation;
        long latency_drift;
        long packet_timeout;
        bool quality_statistics_ignored;
        long downstream_bandwidth_bps;
        long estimated_bandwidth_bps;
        long remote_machine_index;
        s_transport_qos_result qos_result;
        unsigned long last_inactive_time;
        unsigned long undesired_time;
        unsigned long drained_time;
        c_network_time_statistics packet_statistics;
        c_network_time_statistics throughput_statistics;
        c_network_time_statistics acknowledged_statistics;
        c_network_window_statistics receive_window;
        c_network_window_statistics send_window;
        long bandwidth_interval_count;
        long bandwidth_sample_count;
        long bandwidth_minimum;
        long bandwidth_maximum;
        long bandwidth_current_maximum;
        long bandwidth_events[5];
        unsigned long stream_last_packet_time;
        bool voice_active;
        bool stream_packet_rate_limited;
        bool voice_talker;
        bool voice_listener;
        bool stream_packet_rate_halved;
        bool stream_timeout_disabled;
        bool stream_voice_active;
        bool stream_requires_more_bandwidth;
        long stream_bandwidth_bps;
        float stream_packet_rate;
        bool stream_packet_rate_limiting;
        bool stream_saturated;
        long period_bytes_received;
        long period_bytes_sent;
        long period_latency_minimum;
        long period_latency_maximum;
        long period_loss_minimum;
        long period_loss_maximum;
        long period_start_latency;
        long baseline_latency;
        long baseline_loss;
        long packets_received;
        long packets_lost;
        long latency_average_maximum;
        long latency_deviation_maximum;
        long previous_interval_latency;
        long interval_counts[3];
        long interval_percentages[3];
        long loss_accumulator;
        long loss_average;
        long loss_deviation;
        bool voice_repeater;
        long stream_received_bps;
        long stream_sent_bps;
        long probe_latency_threshold;
        long spike_latency_threshold;
        long latency_threshold;
        long loss_threshold;
        bool latency_threshold_valid;
        long best_loss_threshold;
        long best_loss_deviation;
        long best_loss_bandwidth_bps;
        unsigned char stream_probed;
        bool stream_initialized;
        long overprobe_bps;
        long stream_minimum_bandwidth_bps;
        long stream_maximum_bandwidth_bps;
        long backoff_level;
        unsigned long backoff_time;
        long backoff_duration;
        long packet_loss_events;
        long bad_bandwidth_intervals;
        long bad_throughput_intervals;
        long bad_latency_intervals;
        long bad_loss_intervals;
        long bad_packet_loss_intervals;
        long failed_probes;
        long successful_probes;
        long throttle_count;
        long probe_bandwidth_bps;
        bool stream_wants_to_grow;
        char spare_after_stream_wants_to_grow[3];
        bool quality_statistics_connectivity_reported;
        bool quality_statistics_reported[4];
        bool quality_statistics_active[4];
        unsigned __int64 message_type_flags;
    };
    const s_channel_observer *get_observer(e_network_observer_owner owner, long observer_index) const
    {
        const s_channel_observer *observer = &m_channel_observers[observer_index];
        return observer;
    }
    __forceinline long observer_channel_find_by_machine_identifier(e_network_observer_owner owner, const s_transport_unique_identifier *identifier) const;
    __forceinline const c_network_channel *observer_channel_get_network_channel(e_network_observer_owner owner, long observer_index) const
    {
        return &m_channel_observers[observer_index].channel;
    }
    friend long network_observer_initialize_stream_bandwidth(c_network_observer *, long, long, long, long, long, const char *);
    friend long network_observer_update_stream_bandwidth(c_network_observer *observer, long total_bandwidth_bps, long stable_bandwidth_bps, bool stable,
        bool voice, long stream_count, long host_count, long client_count, unsigned long growing_mask, long requiring_count);
    static bool initialize_observer(c_network_observer *observer, c_network_link *link, c_network_message_type_collection *message_types,
        c_network_message_gateway *message_gateway, c_network_message_handler *message_handler,
        const s_observer_configuration *configuration);
    void set_online_network_environment(bool online_network_environment) { m_stream_control = online_network_environment; }
    void reset_bandwidth();
    static void monitor(c_network_observer *observer);
    void observer_channel_initiate_connection(e_network_observer_owner owner, long observer_index);
    void observer_channel_send_message(e_network_observer_owner owner, long observer_index, bool direct,
        e_network_message_type message_type, long message_size, const void *message);
    long observer_channel_find_by_network_channel(e_network_observer_owner owner_type, c_network_channel *channel) const;
    static bool voice_data_available(long observer_index, bool potentially_available, long *voice_bytes_available);
    void quality_statistics_get_ratings(long *connectivity, long *host_badness, long *client_badness);
    bool observer_channel_backlogged(e_network_observer_owner owner, long observer_index, e_network_message_type message_type) const;
    bool observer_channel_currently_alive(e_network_observer_owner owner, long observer_index) const;
    void handle_packet_event(c_network_channel *channel, e_network_packet_event packet_event, long sequence_number, long packet_size_bytes,
        long round_trip_msec);
    void handle_connect_request(const transport_address *address, const s_network_message_connect_request *request);
    bool channel_should_send_packet(c_network_channel *channel, bool game_data_pending, bool force_heartbeat, bool send_sequenced,
        bool *out_send_sequenced_packet, bool *out_force_fill_packet, long *out_game_data_size_bytes, long *out_voice_data_size_bytes,
        long voice_packet_bytes, void *voice_data_buffer);

private:
    void clear_observer(long observer_index)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        memset(observer, 0, sizeof(*observer));
        new (&observer->channel) c_network_channel;
        observer->remote_machine_index = -1;
    }
    long find_observer_index_by_observer(const s_channel_observer *observer) const { return observer - m_channel_observers; }
    bool observer_desires_heartbeat(long observer_index) const;
    void stream_packet_bandwidth_restricted(long observer_index) { m_channel_observers[observer_index].stream_saturated = true; }
    long voice_data_build(long observer_index, long voice_data_bytes, void *voice_data_buffer) const;
    void stream_update();
    void stream_probe_overhead(long observer_index, long *overhead) const;
    void stream_backoff_update(long observer_index, long backoff_tolerance_bps, long maximum_backoff_level, long backoff_time);
    long stream_get_current_latency_threshold(long observer_index, bool probing) const;
    void update_observer_secure_state(long observer_index);
    void update_observer_connection_state(long observer_index);
    void update_observer_connection_initiation(long observer_index);
    bool acquire_secure_connection(long observer_index);
    bool build_secure_connection(e_network_observer_owner owner, bool include_private, const s_transport_secure_address *secure_address,
        transport_address *address) const;
    e_transport_secure_connection get_secure_connection_status(long observer_index) const;
    float packet_rate_maximum() const { return 30.0f; }
    float packet_rate_get_maximum_given_bandwidth_bps(long bandwidth_bps, bool high_overhead, bool limit) const;
    void quality_statistics_notify_established_connectivity(long observer_index, bool good);
    s_channel_observer *find_observer_by_channel(const c_network_channel *channel)
    {
        s_channel_observer *observer = (s_channel_observer *)channel;
        return observer;
    }
    const s_channel_observer *find_observer_by_channel(const c_network_channel *channel) const
    {
        const s_channel_observer *observer = (const s_channel_observer *)channel;
        return observer;
    }
    bool packet_rate_is_limiting(float packet_rate, bool limited, bool high_overhead, bool halved) const
    {
        float maximum_packet_rate = packet_rate_maximum();
        float limiting_packet_rate = maximum_packet_rate * m_configuration->stream_packet_rate_limiting_fraction;
        bool limiting = false;
        if (limited && packet_rate + 0.0001f < limiting_packet_rate)
            limiting = true;
        if (high_overhead)
        {
            if (halved)
                maximum_packet_rate = maximum_packet_rate / 2.0f;
            if (packet_rate + 0.0001f < maximum_packet_rate)
                limiting = true;
        }
        return limiting;
    }
    __forceinline void stream_set_parameters(long observer_index, long bandwidth_bps, float packet_rate)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        long minimum_bandwidth_bps = m_configuration->stream_minimum_bandwidth_bps;
        long stream_bandwidth_bps = bandwidth_bps;
        if (bandwidth_bps <= minimum_bandwidth_bps)
            stream_bandwidth_bps = minimum_bandwidth_bps;
        bool limiting = packet_rate_is_limiting(packet_rate, observer->stream_packet_rate_limited, observer->voice_talker, observer->stream_packet_rate_halved);
        observer->stream_bandwidth_bps = stream_bandwidth_bps;
        observer->stream_packet_rate = packet_rate;
        observer->stream_packet_rate_limiting = limiting;
    }
    bool stream_check_stream_channel_connections(bool *all_streams_changed);
    __forceinline void stream_dispose(long observer_index, bool disconnected, const char *reason);
    __forceinline void stream_report_bad(long observer_index, const char *reason)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        if (observer->voice_active && observer->voice_talker)
            quality_statistics_report_badness(observer_index, true, reason);
    }
    void release_observer(long observer_index);
    long allocate_observer(e_network_observer_owner owner, const s_transport_secure_address *address);
    void update_observer_qos(long observer_index);
    void stream_initialize(long observer_index);
    void stream_set_bandwidth(long observer_index, long bandwidth_bps);
    void update_voice_repeater_use();
    void quality_statistics_report_badness(long observer_index, bool bad, const char *reason);
    void change_observer_state(long observer_index, e_observer_state state);
    void update_observer_timeout(long observer_index);
    bool update_undesired_connection_timers(long observer_index, e_network_channel_closure_reason closure_reason);
    void discard_secure_connection(long observer_index, bool failed, e_network_channel_closure_reason closure_reason);
    void record_throughput_bandwidth(long sent_bps, long stream_count, long saturated_count, long elapsed_msec);
    void bandwidth_monitor_events(long observer_index, long unused, long bandwidth);
    __forceinline void network_observer_bandwidth_monitor_start(long observer_index)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        observer->bandwidth_interval_count = 0;
        observer->bandwidth_sample_count = 0;
        observer->bandwidth_minimum = 0x100000;
        observer->bandwidth_maximum = 0;
        observer->bandwidth_current_maximum = 0;
    }
    __forceinline void network_observer_bandwidth_monitor_stop(long observer_index)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        observer->bandwidth_events[1]++;
        observer->bandwidth_interval_count = 0;
        observer->bandwidth_sample_count = 0;
        observer->bandwidth_minimum = 0x100000;
        observer->bandwidth_maximum = 0;
        observer->bandwidth_current_maximum = 0;
    }
    __forceinline void network_observer_stream_dispose(long observer_index)
    {
        s_channel_observer *observer = &m_channel_observers[observer_index];
        if (observer->voice_talker)
            network_observer_bandwidth_monitor_stop(observer_index);
        observer->voice_active = false;
    }

    struct s_owner_slot
    {
        c_network_observer_owner *owner;
        long session_index;
    };
public:
    c_network_link *m_link;
    c_network_message_gateway *m_message_gateway;
    c_network_message_handler *m_message_handler;
    c_network_message_type_collection *m_message_types;
    const s_observer_configuration *m_configuration;
    s_owner_slot m_owners[4];
    s_channel_observer m_channel_observers[32];
    bool m_quality_statistics_valid;
    s_network_quality_statistics m_network_quality_statistics;
    long m_stream_expansion_state;
    long m_stream_state;
    bool m_stream_disable_expansion;
    bool m_stream_control;
    bool m_stream_low_bandwidth;
    unsigned char m_bandwidth_locked;
    long m_bandwidth_bps;
    long m_stream_peak_bandwidth_bps;
    long m_stream_total_bandwidth_average;
    unsigned long m_time_of_bandwidth_probe;
    bool m_bandwidth_probing;
    long m_bandwidth_probe_counts[2];
    char spare_after_bandwidth_probe_counts[0x21e2c - 0x21e28];
    bool m_stream_has_host;
    unsigned long m_stream_probed_mask;
    unsigned long m_time_of_expansion;
    unsigned long m_time_of_probe;
    unsigned long m_stream_unstable_duration;
    unsigned long m_time_of_throttle;
    long m_throttle_duration;
    long m_bandwidth_minimum_bps;
    long m_bandwidth_growth_bps;
    long m_bandwidth_safe_bps;
    c_network_time_statistics m_bandwidth_statistics;
    long m_bandwidth_successful_bps;
    long m_bandwidth_unsafe_bps;
    float m_bandwidth_satiation_fraction;
    float m_bandwidth_satiation_average;
    long m_stream_bandwidth_cap_bps;
    long m_stream_remaining_bandwidth_bps;
    unsigned long m_time_of_bandwidth_measurement;
    unsigned long m_time_of_period;
    bool m_stream_rebalance;
    bool m_stream_settle;
    bool m_stream_bandwidth_changed;
    bool m_stream_expansion_pending;
    bool m_stream_expansion_blocked;
    char spare_after_stream_expansion_blocked[0x21f60 - 0x21f55];
    unsigned long m_time_of_bandwidth_reset;
    bool m_stream_stable;
    unsigned long m_stream_unstable_mask;
};
typedef char observer_channel_size[(sizeof(void *) != 4 || ((sizeof(c_network_observer::s_channel_observer) == 0x10e8))) ? 1 : -1];
typedef char observer_channels_offset[(sizeof(void *) != 4 || ((offsetof(c_network_observer, m_channel_observers) == 0x38))) ? 1 : -1];
typedef char observer_quality_valid_offset[(sizeof(void *) != 4 || ((offsetof(c_network_observer, m_quality_statistics_valid) == 0x21d38))) ? 1 : -1];
typedef char observer_quality_statistics_offset[(sizeof(void *) != 4 || ((offsetof(c_network_observer, m_network_quality_statistics) == 0x21d40))) ? 1 : -1];
typedef char observer_downstream_offset[
    (sizeof(void *) != 4 || ((offsetof(c_network_observer::s_channel_observer, downstream_bandwidth_bps) == 0xadc))) ? 1 : -1];
typedef char observer_voice_active_offset[(sizeof(void *) != 4 || ((offsetof(c_network_observer::s_channel_observer, voice_active) == 0xfec))) ? 1 : -1];
typedef char observer_voice_talker_offset[(sizeof(void *) != 4 || ((offsetof(c_network_observer::s_channel_observer, voice_talker) == 0xfee))) ? 1 : -1];
typedef char observer_upstream_offset[
    (sizeof(void *) != 4 || ((offsetof(c_network_observer::s_channel_observer, estimated_bandwidth_bps) == 0xae0))) ? 1 : -1];
typedef char observer_status_latency_offset[(sizeof(void *) != 4 || ((offsetof(c_network_observer::s_channel_observer, latency_average) == 0xac4))) ? 1 : -1];
typedef char observer_status_window_offset[(sizeof(void *) != 4 || ((offsetof(c_network_observer::s_channel_observer, receive_window) == 0xda0))) ? 1 : -1];
typedef char observer_status_bandwidth_offset[
    (sizeof(void *) != 4 || ((offsetof(c_network_observer::s_channel_observer, stream_sent_bps) == 0x1064))) ? 1 : -1];
typedef char observer_status_threshold_offset[
    (sizeof(void *) != 4 || ((offsetof(c_network_observer::s_channel_observer, latency_threshold) == 0x1070))) ? 1 : -1];
typedef char observer_bandwidth_history_offset[
    (sizeof(void *) != 4 || ((offsetof(c_network_observer, m_network_quality_statistics.bandwidth_history) == 0x21d90))) ? 1 : -1];

typedef char observer_packet_statistics_layout[
    (sizeof(void *) != 4 || offsetof(c_network_observer::s_channel_observer, packet_statistics) == 0xb18) ? 1 : -1];
typedef char observer_period_layout[
    (sizeof(void *) != 4 || offsetof(c_network_observer::s_channel_observer, period_bytes_received) == 0x1000) ? 1 : -1];
typedef char observer_stream_received_layout[
    (sizeof(void *) != 4 || offsetof(c_network_observer::s_channel_observer, stream_received_bps) == 0x1060) ? 1 : -1];
typedef char observer_best_loss_layout[
    (sizeof(void *) != 4 || offsetof(c_network_observer::s_channel_observer, best_loss_threshold) == 0x107c) ? 1 : -1];
typedef char observer_overprobe_layout[
    (sizeof(void *) != 4 || offsetof(c_network_observer::s_channel_observer, overprobe_bps) == 0x108c) ? 1 : -1];
typedef char observer_message_flags_layout[
    (sizeof(void *) != 4 || offsetof(c_network_observer::s_channel_observer, message_type_flags) == 0x10e0) ? 1 : -1];
typedef char observer_probe_counts_layout[
    (sizeof(void *) != 4 || offsetof(c_network_observer, m_bandwidth_probe_counts) == 0x21e20) ? 1 : -1];
typedef char observer_stream_mask_layout[
    (sizeof(void *) != 4 || offsetof(c_network_observer, m_stream_probed_mask) == 0x21e30) ? 1 : -1];
typedef char observer_bandwidth_statistics_layout[
    (sizeof(void *) != 4 || offsetof(c_network_observer, m_bandwidth_statistics) == 0x21e58) ? 1 : -1];
typedef char observer_unstable_mask_layout[
    (sizeof(void *) != 4 || offsetof(c_network_observer, m_stream_unstable_mask) == 0x21f68) ? 1 : -1];

#endif
