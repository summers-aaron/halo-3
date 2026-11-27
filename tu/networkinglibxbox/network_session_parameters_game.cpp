// networkinglibxbox/network_session_parameters_game.obj: from the records 0x4111a8 0x411360 0x411370 0x4114d0 0x411528 0x411610 0x4116c0 0x4117b8 0x4117d0 0x411938 0x4119a0 0x411ab0 0x411ba0 0x411c98 0x411d68 0x411e68 0x412240 0x4122c0 0x4122d0 0x412370 0x412380 0x412408 0x412418 0x4124c8 0x4124d8 0x412598 0x4125a8 0x4125b8 0x412638 0x412640 0x412698 0x4127a8 0x412858 0x412868 0x412920 0x412958 0x412a08 0x412a50 0x412b10 0x412bf0 0x412c98 0x412dc8 0x413400 0x413550 0x413678 0x413680 0x4135a0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/player_types.h"
#include "game/game_options_encoding.h"
#include "networking/session/network_game_countdown_delayed_reason_types.h"
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x2212e0 ?read_bits_internal@c_bitstream@@AAAXPAEJ@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x4111a8 ?set@c_network_session_parameter_matchmaking_hopper_statistics@@QAA_NPBUs_matchmaking_single_hopper_statistics@@@Z
// @address 0x411938 ?countdown_timer_encode_function@@YAXPAVc_bitstream@@PBXJ@Z
// @address 0x413678 ?game_instance_encode_function@@YAXPAVc_bitstream@@PBXJ@Z
// @address 0x411d68 ?initial_participants_encode_function@@YAXPAVc_bitstream@@PBXJ@Z
// @address 0x3c8638 player_configuration_client_encode_packet
// @address 0x3c8c80 ?player_configuration_host_encode@@YAXPAVc_bitstream@@PBUs_player_configuration_from_host@@W4e_player_configuration_encoding_type@@@Z
// @address 0x413400 network_session_game_options_set
// @address 0x413550 game_options_encode_function
#include "cseries/cseries.h"
#include "game/game.h"
#include "networking/session/network_session_parameters_game.h"
#include "memory/bitstream.h"
#include "networking/player_configuration_encoding_types.h"
#include "networking/network_game_definitions.h"
#include "networking/session/network_session_parameters.h"
#include "game/game_options.h"


struct s_parameter {
    long mode;
    long kind;
    long value;
    bool flag;
};




// @address 0x411360 matchmaking_tip_encode_function
extern "C" void matchmaking_tip_encode_function(c_bitstream *stream, const void *data, long size)
{
    const long *value = (const long *)data;
    stream->write_integer("matchmaking-tip", *value, 6);
}

// @address 0x411370 matchmaking_tip_decode_function
extern "C" bool matchmaking_tip_decode_function(c_bitstream *stream, void *data, long size)
{
    long *value = (long *)data;
    bool success = true;
    *value = stream->read_integer("matchmaking-tip", 6);
    success = success && !stream->error_occurred();
    return success;
}

bool c_network_session_parameter_matchmaking_hopper_statistics::set(const s_matchmaking_single_hopper_statistics *statistics)
{
    bool result = false;
    if (set_allowed()) {
        // match: 0x4111a8: __restrict lets the m_flags load (lwz 0x28) schedule before the 8-byte m_data copy instead of reloading after it
        s_matchmaking_single_hopper_statistics *__restrict current = &m_data;
        if (memcmp(statistics, current, sizeof(m_data)) != 0 || (m_flags & 1) == 0) {
            memcpy(current, statistics, sizeof(m_data));
            set_update_required();
        }
        result = true;
    }
    return result;
}

void game_start_status_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_parameter_game_start_status *status = (const s_network_session_parameter_game_start_status *)data;
    stream->write_integer("game_start_status", status->status, 3);
    stream->write_integer("game_start_error", status->error, 6);
    stream->write_integer("player_error_mask", status->player_error_mask, 16);
    stream->write_integer("map_load_progress", status->map_load_progress, 7);
}

bool game_start_status_decode_function(c_bitstream *stream, void *data, long size)
{
    bool success = true;
    s_network_session_parameter_game_start_status *status = (s_network_session_parameter_game_start_status *)data;
    status->status = stream->read_integer("game_start_status", 3);
    status->error = stream->read_integer("game_start_error", 6);
    status->player_error_mask = (unsigned short)stream->read_integer("player_error_mask", 16);
    status->map_load_progress = (unsigned short)stream->read_integer("map_load_progress", 7);
    success = success && status->map_load_progress >= 0 && status->map_load_progress <= 100;
    success = success && !stream->error_occurred();
    return success;
}


bool c_network_session_parameter_countdown_timer::set(
    bool running, long countdown, e_network_game_countdown_delayed_reason reason, const s_player_identifier *culprit)
{
    bool result = false;
    if (set_allowed()) {
        set_internal(running, countdown, m_data.delayed_countdown, reason, culprit);
        if (!(m_flags & 1))
            set_update_required();
        result = true;
    }
    return result;
}

bool c_network_session_parameter_countdown_timer::request_change(
    bool running, long countdown, e_network_game_countdown_delayed_reason reason, const s_player_identifier *culprit)
{
    bool result = false;
    if (set_allowed())
        result = set(running, countdown, reason, culprit);
    else if (change_request_allowed()) {
        s_network_session_parameter_countdown_timer change;
        memset(&change, 0, sizeof(change));
        change.running = running;
        change.countdown = countdown;
        change.reason = reason;
        change.delayed_countdown = -1;
        if (culprit)
            change.culprit = *culprit;
        m_requested_data = change;
        set_desires_change_request();
        result = true;
    }
    return result;
}

bool c_network_session_parameter_countdown_timer::handle_change_request_payload(const void *payload)
{
    const s_network_session_parameter_countdown_timer *change = (const s_network_session_parameter_countdown_timer *)payload;
    return set_internal(change->running, change->countdown, change->delayed_countdown, change->reason, &change->culprit);
}

bool c_network_session_parameter_countdown_timer::set_internal(bool running, long countdown, long delayed_countdown,
    e_network_game_countdown_delayed_reason reason, const s_player_identifier *culprit)
{
    bool delay = false;
    bool update = false;
    if (m_data.reason != 0 && m_data.delayed_countdown > countdown) {
        m_data.reason = _network_game_countdown_delayed_reason_none;
        m_data.delayed_countdown = -1;
        memset(&m_data.culprit, 0, sizeof(m_data.culprit));
        update = true;
    }
    if (reason != 0 && running && m_data.running)
        delay = true;
    if (m_data.running != running || m_data.countdown != countdown || m_data.reason != reason) {
        m_data.running = running;
        m_data.countdown = countdown;
        if (delay) {
            m_data.reason = reason;
            m_data.delayed_countdown = countdown;
            if (reason == 1)
                m_data.culprit = *culprit;
            else
                memset(&m_data.culprit, 0, sizeof(m_data.culprit));
        } else {
            m_data.reason = _network_game_countdown_delayed_reason_none;
            m_data.delayed_countdown = -1;
            memset(&m_data.culprit, 0, sizeof(m_data.culprit));
        }
        update = true;
    }
    if (update)
        set_update_required();
    return true;
}

void countdown_timer_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_parameter_countdown_timer *countdown = (const s_network_session_parameter_countdown_timer *)data;
    stream->write_bool("countdown-running", countdown->running);
    stream->write_integer("countdown-timer", countdown->countdown, 6);
    stream->write_integer("delayed-reason", countdown->reason, 2);
    stream->write_integer("countdown-delayed-timer", countdown->delayed_countdown + 1, 6);
    stream->write_raw_data("responsible-identifier", countdown->culprit.data, 64);
}

bool countdown_timer_decode_function(c_bitstream *stream, void *data, long size)
{
    s_network_session_parameter_countdown_timer *countdown = (s_network_session_parameter_countdown_timer *)data;
    bool success = true;
    countdown->running = stream->read_bool("countdown-running");
    countdown->countdown = stream->read_integer("countdown-timer", 6);
    countdown->reason = (e_network_game_countdown_delayed_reason)stream->read_integer("delayed-reason", 2);
    countdown->delayed_countdown = stream->read_integer("countdown-delayed-timer", 6) - 1;
    stream->read_raw_data("responsible-identifier", countdown->culprit.data, 64);
    success = success && !stream->error_occurred();
    success = success && countdown->countdown >= 0;
    success = success && countdown->reason >= 0 && countdown->reason < 3;
    return success;
}



void map_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_parameter_map *map = (const s_network_session_parameter_map *)data;
    stream->write_integer("campaign-id", map->campaign_id, 32);
    stream->write_integer("map-id", map->map_id, 32);
    stream->write_string("scenario-path", map->scenario_path, 128);
    game_options_encode_campaign_armaments(stream, map->campaign_players);
}

bool map_decode_function(c_bitstream *stream, void *data, long size)
{
    s_network_session_parameter_map *map = (s_network_session_parameter_map *)data;
    map->campaign_id = (e_campaign_id)stream->read_integer("campaign-id", 32);
    map->map_id = (e_map_id)stream->read_integer("map-id", 32);
    char *buffer = map->scenario_path;
    stream->read_string("scenario-path", buffer, 128);
    bool success = true;
    success = success && game_options_decode_campaign_armaments(stream, map->campaign_players);
    success = success && !stream->error_occurred();
    return success;
}


bool c_network_session_parameter_initial_participants::set(const game_machine_options *machine_options, const game_player_options *player_options) {
    bool result = false;
    if (set_allowed()) {
        if (machine_options) {
            m_data.valid = true;
            memcpy(&m_data.machine_options, machine_options, sizeof(m_data.machine_options));
            memcpy(m_data.player_options, player_options, sizeof(m_data.player_options));
        } else {
            m_data.valid = false;
            memset(&m_data.machine_options, 0, sizeof(m_data.machine_options));
            memset(m_data.player_options, 0, sizeof(m_data.player_options));
        }
        set_update_required();
        result = true;
    }
    return result;
}

struct s_network_game_definitions_client_configuration;

void initial_participants_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_parameter_initial_participants *message = (const s_network_session_parameter_initial_participants *)data;
    stream->write_bool("initial-participants-exist", message->valid);
    if (message->valid) {
        stream->write_integer("machine-valid-mask", message->machine_options.valid_machine_mask, 16);
        for (long machine_index = 0; machine_index < 16; ++machine_index) {
            if (message->machine_options.valid_machine_mask & (1 << machine_index))
                stream->write_raw_data("machine-identifier", message->machine_options.machines[machine_index].data, 48);
        }
        for (long player_index = 0; player_index < 16; ++player_index) {
            const game_player_options *player = &message->player_options[player_index];
            stream->write_bool("player-valid", player->valid);
            if (player->valid) {
                stream->write_bool("player-left-game", player->left_game);
                if (!player->left_game) {
                    stream->write_raw_data("player-machine-identifier", player->machine_identifier.data, 48);
                    stream->write_integer("player-user-index", player->user_index, 2);
                    stream->write_integer("player-controller-index", player->controller_index, 2);
                }
                stream->write_raw_data("player-identifier", player->player_identifier.data, 64);
                player_configuration_client_encode_packet(stream,
                    (const s_network_game_definitions_client_configuration *)&player->configuration.client,
                    _player_configuration_encoding_full);
                player_configuration_host_encode(
                    stream, (const s_player_configuration_from_host *)&player->configuration.host, _player_configuration_encoding_full);
            }
        }
    }
}

bool random_seed_decode_function(c_bitstream *stream, void *data, long size)
{
    bool success = true;
    unsigned long *seed = (unsigned long *)data;
    *seed = stream->read_integer("random-seed", 32);
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x4122c0 language_encode_function
extern "C" void language_encode_function(c_bitstream *stream, const void *data, long size)
{
    const long *value = (const long *)data;
    stream->write_integer("language", *value, 4);
}

// @address 0x4122d0 language_decode_function
extern "C" bool language_decode_function(c_bitstream *stream, void *data, long size)
{
    long *index = (long *)data;
    bool success = true;
    *index = stream->read_integer("language", 4);
    success = success && !stream->error_occurred();
    success = success && *index >= 0 && *index < 12;
    return success;
}

// @address 0x412370 determinism_version_encode_function
extern "C" void determinism_version_encode_function(c_bitstream *stream, const void *data, long size)
{
    const long *index = (const long *)data;
    stream->write_integer("determinism-version", *index + 1, 9);
}

// @address 0x412380 determinism_version_decode_function
extern "C" bool determinism_version_decode_function(c_bitstream *stream, void *data, long size)
{
    long *value = (long *)data;
    bool success = true;
    *value = stream->read_integer("determinism-version", 9) - 1;
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x412408 simulation_protocol_encode_function
extern "C" void simulation_protocol_encode_function(c_bitstream *stream, const void *data, long size)
{
    const long *value = (const long *)data;
    stream->write_integer("simulation-protocol", *value, 2);
}

bool simulation_protocol_decode_function(c_bitstream *stream, void *data, long size)
{
    bool success = true;
    long *protocol = (long *)data;
    *protocol = stream->read_integer("simulation-protocol", 2);
    success = success && *protocol >= 0 && *protocol < 3;
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x4124c8 campaign_difficulty_encode_function
extern "C" void campaign_difficulty_encode_function(c_bitstream *stream, const void *data, long size)
{
    const long *value = (const long *)data;
    stream->write_integer("campaign-difficulty", *value + 1, 3);
}

bool campaign_difficulty_decode_function(c_bitstream *stream, void *data, long size)
{
    bool success = true;
    long *difficulty = (long *)data;
    *difficulty = stream->read_integer("campaign-difficulty", 3) - 1;
    success = success && (*difficulty == -1 || (*difficulty >= 0 && *difficulty < 4));
    success = success && !stream->error_occurred();
    return success;
}

void random_seed_encode_function(c_bitstream *stream, const void *data, long size)
{
    const unsigned long *seed = (const unsigned long *)data;
    stream->write_integer("random-seed", *seed, 32);
}

// @address 0x4125a8 campaign_metagame_scoring_encode_function
extern "C" void campaign_metagame_scoring_encode_function(c_bitstream *stream, const void *data, long size)
{
    const short *value = (const short *)data;
    stream->write_integer("campaign-metagame-scoring", *value, 2);
}

// @address 0x4125b8 campaign_metagame_scoring_decode_function
extern "C" bool campaign_metagame_scoring_decode_function(c_bitstream *stream, void *data, long size)
{
    unsigned short *value = (unsigned short *)data;
    bool success = true;
    *value = stream->read_integer("campaign-metagame-scoring", 2);
    success = success && !stream->error_occurred();
    return success;
}

void end_game_encode_function(c_bitstream *stream, const void *data, long size)
{
    const bool *end_game = (const bool *)data;
    stream->write_bool("end-game", *end_game);
}

// @address 0x412640 privacy_mode_encode_function
extern "C" void privacy_mode_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_parameter *parameter = (const s_parameter *)data;
    stream->write_integer("mode", parameter->mode, 2);
    stream->write_integer("kind", parameter->kind, 2);
    stream->write_integer("privacy-value", parameter->value, 4);
    stream->write_bool("flag", parameter->flag);
}

// @address 0x412698 privacy_mode_extended_decode_function
extern "C" bool privacy_mode_extended_decode_function(c_bitstream *stream, void *data, long size)
{
    s_parameter *parameter = (s_parameter *)data;
    parameter->mode = stream->read_integer("mode", 2);
    parameter->kind = stream->read_integer("kind", 2);
    parameter->value = stream->read_integer("privacy-value", 4);
    parameter->flag = stream->read_bool("flag");
    bool success = parameter->mode >= 0 && parameter->mode < 3;
    success = success && parameter->kind >= 0 && parameter->kind < 4;
    success = success && !stream->error_occurred();
    return success;
}

bool privacy_mode_decode_function(c_bitstream *stream, void *data, long size)
{
    long *privacy_mode = (long *)data;
    bool success = true;
    *privacy_mode = stream->read_integer("privacy-mode", 2);
    success = success && *privacy_mode >= 0 && *privacy_mode < 4;
    success = success && !stream->error_occurred();
    return success;
}

void voice_repeater_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_parameter_voice_repeater *parameter = (const s_network_session_parameter_voice_repeater *)data;
    stream->write_integer("voice-repeater-peer-index", parameter->peer_index + 1, 5);
}

bool voice_repeater_decode_function(c_bitstream *stream, void *data, long size)
{
    s_network_session_parameter_voice_repeater *parameter = (s_network_session_parameter_voice_repeater *)data;
    bool success = true;
    parameter->peer_index = stream->read_integer("voice-repeater-peer-index", 5) - 1;
    success = success && ((parameter->peer_index >= 0 && parameter->peer_index < 16) || parameter->peer_index == -1);
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x412920 synchronous_out_of_sync_encode_function
extern "C" void synchronous_out_of_sync_encode_function(c_bitstream *stream, const void *data, long size)
{
    const bool *flags = (const bool *)data;
    stream->write_bool("flag", flags[0]);
    stream->write_bool("flag", flags[1]);
}

// @address 0x412958 synchronous_out_of_sync_decode_function
extern "C" int synchronous_out_of_sync_decode_function(c_bitstream *stream, bool *flags)
{
    flags[0] = stream->read_bool("flag");
    flags[1] = stream->read_bool("flag");
    bool success = true;
    success = success && !stream->error_occurred();
    success = success && (flags[0] || !flags[1]);
    return success;
}

// @address 0x412a08 request_campaign_quit_encode_function
extern "C" void request_campaign_quit_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_parameter_request_campaign_quit *parameter = (const s_network_session_parameter_request_campaign_quit *)data;
    stream->write_integer("quit-request-type", parameter->quit_request_type, 2);
    stream->write_integer("quit-request-peer-index", parameter->quit_request_peer_index + 1, 5);
}

// @address 0x412a50 request_campaign_quit_decode_function
extern "C" bool request_campaign_quit_decode_function(c_bitstream *stream, void *data, long size)
{
    s_network_session_parameter_request_campaign_quit *parameter = (s_network_session_parameter_request_campaign_quit *)data;
    bool success = true;
    parameter->quit_request_type = stream->read_integer("quit-request-type", 2);
    parameter->quit_request_peer_index = stream->read_integer("quit-request-peer-index", 5) - 1;
    success = success && !stream->error_occurred();
    success = success && parameter->quit_request_type >= 0 && parameter->quit_request_type < 4;
    return success;
}

// @address 0x412b10 leader_properties_encode_function
extern "C" void leader_properties_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_parameter_leader_properties *parameter = (const s_network_session_parameter_leader_properties *)data;
    stream->write_string("name", parameter->name, sizeof(parameter->name));
}

// @address 0x412bf0 leader_properties_decode_function
extern "C" bool leader_properties_decode_function(c_bitstream *stream, void *data, long size)
{
    s_network_session_parameter_leader_properties *parameter = (s_network_session_parameter_leader_properties *)data;
    bool success = true;
    stream->read_string("name", parameter->name, sizeof(parameter->name));
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x412c98 end_game_decode_function
extern "C" bool end_game_decode_function(c_bitstream *stream, void *data, long size)
{
    bool *value = (bool *)data;
    bool success = true;
    *value = stream->read_bool("end-game");
    success = success && !stream->error_occurred();
    return success;
}

// @address 0x412dc8 ui_game_mode_param_decode_function
extern "C" bool ui_game_mode_param_decode_function(c_bitstream *stream, void *data, long size)
{
    long *value = (long *)data;
    bool success = true;
    *value = stream->read_integer("ui-game-mode", 3) - 1;
    success = success && !stream->error_occurred();
    return success;
}

void game_instance_encode_function(c_bitstream *stream, const void *data, long size)
{
    stream->write_raw_data("game-instance", data, 64);
}

bool game_instance_decode_function(c_bitstream *stream, void *data, long size)
{
    unsigned __int64 *instance = (unsigned __int64 *)data;
    bool success = true;
    stream->read_raw_data("game-instance", instance, 64);
    success = success && !stream->error_occurred();
    return success;
}

extern "C" bool network_session_game_options_set(c_network_session_parameter_saved_film_game_options *view,
    long length_in_ticks, long start_ticks, const void *data)
{
    const game_options *options = (const game_options *)data;
    c_generic_network_session_parameter_data<s_network_session_parameter_saved_film_game_options> *parameter =
        (c_generic_network_session_parameter_data<s_network_session_parameter_saved_film_game_options> *)view;
    bool success = false;
    if (parameter->set_allowed()) {
        if (!game_options_verify(options, 0, 0)) {
            for (long index = 0; index < 16; index++)
                parameter->m_peers_updated[index] = 0;
            parameter->m_flags = 0;
            static_cast<c_network_session_parameter_base *>(parameter)->notify_clear();
        } else {
            if (!(parameter->m_flags & 1) || parameter->m_data.length_in_ticks != length_in_ticks || parameter->m_data.start_ticks != start_ticks
                || memcmp(&parameter->m_data.options, options, sizeof(parameter->m_data.options)) != 0) {
                parameter->m_data.length_in_ticks = length_in_ticks;
                parameter->m_data.start_ticks = start_ticks;
                memcpy(&parameter->m_data.options, options, sizeof(parameter->m_data.options));
                parameter->m_flags |= 1;
                for (long index = 0; index < 16; index++)
                    parameter->m_peers_updated[index] = 0;
                parameter->notify_set_update_required();
            }
            success = true;
        }
    }
    return success;
}


extern "C" void game_options_encode_function(void *unused, c_bitstream *stream, const game_options *options)
{
    const s_network_session_parameter_saved_film_game_options *values = (const s_network_session_parameter_saved_film_game_options *)options;
    stream->write_integer("length-in-ticks", values->length_in_ticks, 32);
    stream->write_integer("start-ticks", values->start_ticks, 32);
    game_options_encode(stream, &values->options);
}

static __forceinline void read_machine_identifier(c_bitstream *stream, s_machine_identifier *identifier)
{
    unsigned char *destination = identifier->data;
    unsigned __int64 value = bitstream_read_window_bits(stream, 48) << 16;
    unsigned long remaining = 6;
    do {
        *destination++ = (unsigned char)(value >> 56);
        value <<= 8;
        --remaining;
    } while (remaining != 0);
}

// @address 0x411e68 ?initial_participants_decode_function@@YA_NPAVc_bitstream@@PAXJ@Z
bool initial_participants_decode_function(c_bitstream *stream, void *data, long size)
{
    s_network_session_parameter_initial_participants *message = (s_network_session_parameter_initial_participants *)data;
    bool success = true;
    message->valid = stream->read_bool("initial-participants-exist");
    if (message->valid) {
        message->machine_options.valid_machine_mask = stream->read_integer("machine-valid-mask", 16);
        for (long machine_index = 0; machine_index < 16; ++machine_index) {
            if (message->machine_options.valid_machine_mask & (1 << machine_index)) {
                read_machine_identifier(stream, &message->machine_options.machines[machine_index]);
                for (long prior = 0; prior < machine_index; ++prior) {
                    if ((message->machine_options.valid_machine_mask & (1 << prior)) &&
                        !memcmp(&message->machine_options.machines[machine_index], &message->machine_options.machines[prior], sizeof(s_machine_identifier)))
                        success = false;
                }
            }
        }
        for (long player_index = 0; player_index < 16; ++player_index) {
            message->player_options[player_index].valid = stream->read_bool("player-valid");
            if (message->player_options[player_index].valid) {
                message->player_options[player_index].left_game = stream->read_bool("player-left-game");
                if (message->player_options[player_index].left_game) {
                    message->player_options[player_index].user_index = -1;
                    message->player_options[player_index].controller_index = -1;
                } else {
                    read_machine_identifier(stream, &message->player_options[player_index].machine_identifier);
                    message->player_options[player_index].user_index = (short)stream->read_integer("player-user-index", 2);
                    message->player_options[player_index].controller_index = stream->read_integer("player-controller-index", 2);
                }
                stream->read_raw_data("player-identifier", message->player_options[player_index].player_identifier.data, 64);
                bool client_decoded = player_configuration_client_decode(stream,
                    (s_player_configuration_from_client *)&message->player_options[player_index].configuration.client,
                    _player_configuration_encoding_full);
                success = success && client_decoded;
                bool host_decoded = player_configuration_host_decode(stream,
                    (s_player_configuration_from_host *)&message->player_options[player_index].configuration.host,
                    _player_configuration_encoding_full);
                success = success && host_decoded;
                if (!success)
                    continue;
                if (!message->player_options[player_index].left_game) {
                    long machine_index = -1;
                    for (long index = 0; index < 16; ++index)
                        if ((message->machine_options.valid_machine_mask & (1 << index)) &&
                            !memcmp(&message->machine_options.machines[index], &message->player_options[player_index].machine_identifier,
                                sizeof(s_machine_identifier)))
                            machine_index = index;
                    success = success && machine_index != -1;
                }
                for (long prior = 0; prior < player_index; ++prior) {
                    if (message->player_options[prior].valid) {
                        success = success &&
                            memcmp(&message->player_options[player_index].player_identifier, &message->player_options[prior].player_identifier,
                                sizeof(s_player_identifier)) != 0;
                        if (!message->player_options[player_index].left_game &&
                            !memcmp(&message->player_options[player_index].machine_identifier, &message->player_options[prior].machine_identifier,
                                sizeof(s_machine_identifier)))
                        {
                            success = success && message->player_options[player_index].user_index != message->player_options[prior].user_index;
                            success = success && message->player_options[player_index].controller_index != message->player_options[prior].controller_index;
                        }
                    }
                }
            }
        }
    }
    return success;
}


// @address 0x4135a0 game_options_decode_function
extern "C" bool game_options_decode_function(long unused, c_bitstream *stream, void *data)
{
    s_network_session_parameter_saved_film_game_options *values = (s_network_session_parameter_saved_film_game_options *)data;
    values->length_in_ticks = stream->read_integer("length-in-ticks", 32);
    values->start_ticks = stream->read_integer("start-ticks", 32);
    bool result = true;
    unsigned char decoded = game_options_decode(stream, &values->options);
    result = result && decoded != 0;
    result = result && !stream->error_occurred();
    return result;
}
