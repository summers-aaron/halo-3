#ifndef NETWORKING_REPLICATION_REPLICATION_CONTROL_VIEW_H
#define NETWORKING_REPLICATION_REPLICATION_CONTROL_VIEW_H

#include "networking/replication/replication_motion.h"
#include "networking/replication/replication_types.h"
#include "networking/replication/replication_scheduler_client.h"

class c_bitstream;
class c_simulation_view_telemetry_provider;

class c_replication_control_view : public c_replication_scheduler_client
{
public:
    c_replication_control_view() { m_initialized = false; }
    ~c_replication_control_view() {}

    virtual bool has_data_to_transmit();
    virtual long build_outgoing_requests(const void *data, long maximum_requests, s_replication_outgoing_request *requests);
    virtual long terminator_required_bits() const { return 1; }
    virtual void write_to_packet(LONG_PTR packet_index, long request_index, const void *request, long request_size, c_bitstream *packet, long space_bits);
    virtual void write_terminator_to_packet(long packet_index, c_bitstream *packet);
    virtual e_network_read_result read_from_packet(long packet_index, c_bitstream *packet, long maximum_requests,
        s_replication_incoming_request *requests, long *request_count);
    virtual void process_incoming_request(s_replication_incoming_request *request);
    virtual void notify_packet_acknowledged(long packet_index) {}
    virtual void notify_packet_retired(long packet_index, bool delivered) {}

    // March 82661d50: motion at +0x18, outgoing mask at +0x10.
    __forceinline void send_motion(long index, const s_player_motion *motion)
    {
        m_motions[index] = *motion;
        m_motion_available_send |= 1 << index;
    }

    __forceinline bool retrieve_motion(long index, s_player_motion *motion)
    {
        bool result = false;
        unsigned long motion_bit = 1 << index;
        unsigned long motion_mask = m_pending_motion_mask;
        if ((motion_mask & motion_bit) != 0)
        {
            *motion = m_motions[index];
            m_pending_motion_mask = motion_mask & ~motion_bit;
            result = true;
        }
        return result;
    }
    // March 82662478.
    __forceinline bool retrieve_prediction(long index, s_player_prediction *prediction)
    {
        bool result = false;
        if ((m_pending_prediction_mask & (1 << index)) != 0)
        {
            *prediction = m_predictions[index];
            m_pending_prediction_mask &= ~(1 << index);
            result = true;
        }
        return result;
    }

public:
    __forceinline void initialize(c_simulation_view_telemetry_provider *manager)
    {
        m_initialized = true;
        m_motion_available_send = 0;
        m_pending_motion_mask = 0;
        m_prediction_available_send = 0;
        m_pending_prediction_mask = 0;
        m_telemetry_provider = manager;
        for (long index = 0; index < 32; ++index)
            m_control_times[index] = 0;
    }
    __forceinline void destroy()
    {
        m_initialized = false;
        m_telemetry_provider = 0;
    }
private:
    friend class c_simulation_world;
    friend class c_simulation_player;
    bool m_initialized;
    c_simulation_view_telemetry_provider *m_telemetry_provider;
    unsigned long m_motion_available_send;
    unsigned long m_pending_motion_mask;
    s_player_motion m_motions[32];
    unsigned long m_control_times[32];
    unsigned long m_prediction_available_send;
    unsigned long m_pending_prediction_mask;
    s_player_prediction m_predictions[32];
};

#endif // NETWORKING_REPLICATION_REPLICATION_CONTROL_VIEW_H
