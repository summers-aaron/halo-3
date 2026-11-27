// networkinglibxbox/network_session_parameters_session.obj: from the records 0x3ed080 0x3ed118 0x3ed1e8 0x3ed2a0 0x3ed500 0x3ed5a0 0x3ed5e0 0x3ed6c8 0x3ed798 0x3ed808 0x3ed810 0x3ed860 0x3ed950 0x4151f0
// @flags /TP /O2
// @address 0x3ed080 ?request_change@c_network_session_parameter_requested_remote_join_data@@QAA_NPBUs_network_session_remote_session_join_data@@@Z
// 0x3ed118: set of requested_remote_join_data and remote_join_data is one body, folded by the linker (ICF)
// @address 0x3ed118 ?set@c_network_session_parameter_requested_remote_join_data@@QAA_NPBUs_network_session_remote_session_join_data@@@Z
// @address 0x3ed500 ?set_max_player_count@c_network_session_parameter_session_size@@QAA_NJ@Z
// @address 0x3ed5a0 ?session_size_encode_function@@YAXPAVc_bitstream@@PBXJ@Z
// @address 0x3ed5e0 ?session_size_decode_function@@YA_NPAVc_bitstream@@PAXJ@Z
// @address 0x3ed6c8 ?set@c_network_session_parameter_session_mode@@QAA_NW4e_network_session_mode@@@Z
// @address 0x3ed798 ?handle_update_payload@c_network_session_parameter_session_mode@@EAA_NPBX@Z
// @address 0x3ed808 ?handle_change_request_payload@c_network_session_parameter_session_mode@@EAA_NPBX@Z
// @address 0x3ed810 ?session_mode_encode_function@@YAXPAVc_bitstream@@PBXJ@Z
// @address 0x3ed860 ?session_mode_decode_function@@YA_NPAVc_bitstream@@PAXJ@Z
// @address 0x3ed950 ?host_frame_quality_decode@@YA_NPAVc_bitstream@@PAK@Z
// @address 0x4151f0 network_session_request_parameter_set_value
// @address 0x3ed1e8 ?remote_join_data_encode_function@@YAXPAVc_bitstream@@PBXJ@Z
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
#include <string.h>
#include "cseries/cseries_types.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "networking/session/network_session_parameters_session.h"

#include "networking/session/network_session_parameter_access.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session.h"
#include "memory/bitstream.h"
#include "networking/session/network_session_parameter_session_types.h"
#include "networking/network_time_types.h"
// Takes new settings (104 bytes) while the game is active: when they differ or
// none were set yet, keeps them, clears the pending bytes and notifies the
// block. Returns whether the game was active.

#include "networking/network_time_access.h"
// @address 0x8c447c g_network_time_globals

struct s_network_session_remote_join_decode
{
    long state;
    long from;
    long to;
    long target;
    unsigned char nonce[8];
    long platform;
    unsigned char id[8];
    unsigned char key[16];
    unsigned char address[36];
    long session_class;
    long result;
    bool public_slots;
};

// 0x4151f0


bool c_network_session_parameter_requested_remote_join_data::request_change(const s_network_session_remote_session_join_data *new_settings)
{
    bool result = false;
    if (set_allowed())
        result = set(new_settings);
    else if (change_request_allowed()) {
        memcpy(&m_requested_data, new_settings, sizeof(m_requested_data));
        set_desires_change_request();
        result = true;
    }
    return result;
}

bool c_network_session_parameter_requested_remote_join_data::set(const s_network_session_remote_session_join_data *new_settings)
{
    bool success = false;
    if (set_allowed()) {
        if (memcmp(new_settings, &m_data, sizeof(m_data)) != 0 || !get_allowed()) {
            memcpy(&m_data, new_settings, sizeof(m_data));
            set_update_required();
        }
        success = true;
    }
    return success;
}

// @address 0x3ed118 ?set@c_network_session_parameter_remote_join_data@@QAA_NPBUs_network_session_remote_session_join_data@@@Z
// PC build only: on Xbox the image holds one body for this name, the requested_remote_join_data twin above.
#if !defined(_XBOX) || defined(__clang__)
bool c_network_session_parameter_remote_join_data::set(const s_network_session_remote_session_join_data *new_settings)
{
    bool success = false;
    if (set_allowed()) {
        if (memcmp(new_settings, &m_data, sizeof(m_data)) != 0 || !get_allowed()) {
            memcpy(&m_data, new_settings, sizeof(m_data));
            set_update_required();
        }
        success = true;
    }
    return success;
}
#endif

bool remote_join_data_decode_function(c_bitstream *stream, void *update_message, long size)
{
    s_network_session_remote_join_decode *join = (s_network_session_remote_join_decode *)update_message;
    bool success = true;

    join->state = stream->read_integer("join-state", 3);
    join->from = stream->read_integer("join-from", 1);
    join->to = stream->read_integer("join-to", 1);
    join->target = stream->read_integer("join-target", 1);
    join->platform = stream->read_integer("platform", 1);
    stream->read_raw_data("session-id", join->id, 64);
    stream->read_raw_data("session-key", join->key, 128);
    stream->read_raw_data("host-secure-address", join->address, 288);
    join->session_class = stream->read_integer("session-class", 2);
    stream->read_raw_data("join-nonce", join->nonce, 64);
    join->result = stream->read_integer("join-result", 5);
    join->public_slots = stream->read_bool("join-to-public-slots");

    success = success && join->state >= 0 && join->state < 7;
    success = success && join->from >= 0 && join->from < 2;
    success = success && join->to >= 0 && join->to < 2;
    success = success && join->result >= 0 && join->result < 20;
    success = success && join->result >= 0 && join->result < 20;
    success = success && join->platform >= 0 && join->platform < 2;
    success = success && join->session_class >= 0 && join->session_class < 3;
    success = success && !stream->error_occurred();
    return success;
}

void remote_join_data_encode_function(c_bitstream *stream, const void *update_message, long size)
{
    const s_network_session_remote_join_decode *join = (const s_network_session_remote_join_decode *)update_message;

    stream->write_integer("join-state", join->state, 3);
    stream->write_integer("join-from", join->from, 1);
    stream->write_integer("join-to", join->to, 1);
    stream->write_integer("join-target", join->target, 1);
    stream->write_integer("platform", join->platform, 1);
    stream->write_raw_data("session-id", join->id, 64);
    stream->write_raw_data("session-key", join->key, 128);
    stream->write_raw_data("host-secure-address", join->address, 288);
    stream->write_integer("session-class", join->session_class, 2);
    stream->write_raw_data("join-nonce", join->nonce, 64);
    stream->write_integer("join-result", join->result, 5);
    stream->write_bool("join-to-public-slots", join->public_slots);
}

bool c_network_session_parameter_session_size::set_max_player_count(long maximum_player_count)
{
    bool result = false;
    if (set_allowed())
    {
        if (maximum_player_count != m_data.maximum_player_count || !get_allowed())
        {
            m_data.maximum_player_count = maximum_player_count;
            set_update_required();
        }
        result = true;
    }
    return result;
}

void session_size_encode_function(c_bitstream *packet, const void *data, long size)
{
    const s_network_session_parameter_session_size *session_size = (const s_network_session_parameter_session_size *)data;
    packet->write_integer("maximum-peer-count", session_size->maximum_peer_count, 5);
    packet->write_integer("maximum-player-count", session_size->maximum_player_count, 5);
}

bool session_size_decode_function(c_bitstream *stream,void *data,long size) {
 bool success=true;
 s_network_session_parameter_session_size *session_size=(s_network_session_parameter_session_size *)data;
 session_size->maximum_peer_count=stream->read_integer("count",5);
 session_size->maximum_player_count=stream->read_integer("count",5);
 success=success && session_size->maximum_peer_count>=0 && session_size->maximum_peer_count<=16;
 success=success && session_size->maximum_player_count>=0 && session_size->maximum_player_count<=16;
 success=success && !stream->error_occurred();
 return success;
}

bool c_network_session_parameter_session_mode::set(e_network_session_mode mode)
{
    int value = (int)mode;
    bool result = false;
    if (set_allowed()) {
        if (m_data.mode != value || (m_flags & 1) == 0) {
            m_data.mode = (e_network_session_mode)value;
            m_data.sequence++;
            m_data.time = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
            set_update_required();
        }
        result = true;
    }
    return result;
}

bool c_network_session_parameter_session_mode::handle_update_payload(const void *payload)
{
    bool success = true;
    const s_network_session_parameter_session_mode *mode = (const s_network_session_parameter_session_mode *)payload;
    m_data = *mode;
    m_data.time = network_time_get();
    return success;
}

bool c_network_session_parameter_session_mode::handle_change_request_payload(const void *payload)
{
    const s_network_session_parameter_session_mode *requested = (const s_network_session_parameter_session_mode *)payload;
    return set((e_network_session_mode)requested->mode);
}

// 0x3ed810
void session_mode_encode_function(c_bitstream *packet, const void *data, long size)
{
    const s_network_session_parameter_session_mode *session_mode = (const s_network_session_parameter_session_mode *)data;
    packet->write_integer("session-mode-sequence", session_mode->sequence, 32);
    packet->write_integer("session-mode", session_mode->mode, 5);
    packet->write_integer("session-mode-timestamp", session_mode->time, 32);
}

// 0x3ed860
bool session_mode_decode_function(c_bitstream *stream, void *data, long size)
{
    bool success = true;
    s_network_session_parameter_session_mode *session_mode = (s_network_session_parameter_session_mode *)data;
    session_mode->sequence = stream->read_integer("session-mode-sequence", 32);
    session_mode->mode = (e_network_session_mode)stream->read_integer("session-mode", 5);
    session_mode->time = stream->read_integer("session-mode-timestamp", 32);
    success = success && !stream->error_occurred();
    success = success && session_mode->mode > 0 && session_mode->mode < 13;
    success = success && session_mode->sequence >= 0;
    return success;
}

bool host_frame_quality_decode(c_bitstream *stream, unsigned long *out)
{
    bool success = true;
    *out = stream->read_integer("host-frame-quality", 2);
    success = success && !stream->error_occurred();
    return success;
}




bool network_session_request_parameter_set_value(
    c_generic_network_session_parameter<s_network_session_parameter_matchmaking_messaging> *parameter, long message_type)
{
    bool result = false;
    if (parameter->set_allowed()) {
        const c_network_session *session = parameter->m_session;
        long display_cookie = parameter->m_data.display_cookie + 1;
        unsigned long flags = parameter->m_flags;
        parameter->m_data.message_type = message_type;
        parameter->m_data.display_cookie = display_cookie % 16;
        long player_sequence_number = session->get_session_membership()->get_current_membership()->player_sequence_number;
        parameter->m_flags = flags | 1;
        parameter->m_data.message_data = player_sequence_number;
        parameter->m_peers_updated.set_all(false);
        parameter->notify_set_update_required();
        result = true;
    }
    return result;
}

// @address 0x3b5bf0 ?set@?$c_generic_network_session_parameter@J@@QAA_NPBJ@Z
// PC build only: on Xbox the image holds one body for this name, the <e_campaign_difficulty_level> twin
// in network_session_interface.cpp.
#if !defined(_XBOX) || defined(__clang__)
template<>
bool c_generic_network_session_parameter<long>::set(const long *parameter)
{
    bool result = false;
    if (this->set_allowed()) {
        if (memcmp(&m_data, parameter, sizeof(m_data)) != 0 || !get_allowed()) {
            m_data = *parameter;
            set_update_required();
        }
        result = true;
    }
    return result;
}
#endif
