#include "networking/logic/network_search_types.h"
// networkinglibxbox/network_game_definitions.obj: from the records 0x221648 0x247448 0x3c71b0 0x3c7550 0x3c7db0 0x3c82d0 0x3c83c0 0x3c8438 0x3c84d8 0x3c8568 0x3c8638 0x3c8850 0x3c8c80 0x3c8da8 0x3c9048 0x3c9170 0x3c9288 0x3c93f8 0x3c94d8
// @flags /TP /O2 /Oc
#include "cseries/cseries_unicode.h"
#include "text/unicode.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/network_game_definitions.h"
#include "simulation/simulation.h"
#include "game/players.h"
#include "networking/player_configuration_encoding_types.h"
#include "game/player_identifier_types.h"
#include "game/player_configuration.h"
#include "networking/session/network_session_membership.h"
#include "networking/network_configuration.h"
#include "networking/transport/transport_security.h"
#include <string.h>

struct s_network_session_qos_status_header
{
    unsigned short type;
    unsigned short version;
    long status;
    long index0;
    long index1;
};

struct s_network_squad_status_player_configuration
{
    s_player_configuration_from_client client_configuration;
    s_player_configuration_from_host host_configuration;
};

struct s_network_squad_status_data;
struct utf8;
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x3c93f8 network_session_qos_status_header_verify
// @address 0x3c9170 player_appearance_encode
// units/bitstream/unit.h: c_bitstream as the release build declares it (bitstream.obj). Member names and access from
// the March 2007 dev map; the public read/write calls take a name that release does not use and forward to the
// internals, which stay out of line.
// Layout from the records: the position (stream and memory bit positions, the 64-bit window, the bits it holds, the
// next byte) at 0x18, saved by push_position into a four-deep stack at 0x38.

#include "memory/bitstream_wide_strings.h"




#include "networking/network_configuration_types.h"
#include "text/utf8_conversion.h"
#include "game/game_results_player_data.h"
// @address 0x45708 k_fake_host_name_digits
// @rdata 0x82045708..0x8204574c
extern "C" {
const wchar_t k_fake_host_name_digits[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', 0, 0
};
}
// @rdata-end



// the 64-bit window internals (no name in the March 2007 build)

struct s_network_squad_status_data;



bool c_bitstream::read_bit_internal()
{
    bool result;
    if (m_current.window_bits_used < sizeof(m_current.window) * 8) {
        result = (m_current.window & 0x8000000000000000ui64) != 0;
        m_current.window <<= 1;
        m_current.window_bits_used++;
        m_current.current_memory_bit_position++;
    } else {
        unsigned __int64 value = 0;
        long size;
        unsigned __int64 old_window = m_current.window;
        if (m_current.next_data + 8 <= m_data_max) {
            value = bitstream_load_word(m_current.next_data);
            size = 64;
            m_current.next_data += 8;
        } else if (m_current.next_data < m_data_max) {
            size = 0;
            while (m_current.next_data < m_data_max) {
                value = (value << 8) | *m_current.next_data++;
                size += 8;
            }
            value <<= (unsigned long)(64 - size);
        }
        long remaining = m_current.window_bits_used - 63;
        m_current.window = value << remaining;
        m_current.window_bits_used = remaining;
        m_current.current_stream_bit_position += size;
        m_current.current_memory_bit_position++;
        result = ((value >> (unsigned long)(64 - remaining)) | (old_window >> 63)) != 0;
    }
    return result;
}


static __forceinline long network_game_definitions_read_signed_integer(c_bitstream *packet, const char *name, long size_in_bits)
{
    long value = packet->read_integer(name, size_in_bits);
    if (size_in_bits < 32 && (unsigned long)(value & (1 << (size_in_bits - 1))) != 0)
    {
        long sign_extension = (long)~((1i64 << size_in_bits) - 1);
        value |= sign_extension;
    }
    return value;
}

// @address 0x3c82d0 ?network_session_qos_status_decode_from_block@@YA_NJPBXPAUs_network_squad_status_data@@@Z
bool network_session_qos_status_decode_from_block(long block_size, void const * block, struct s_network_squad_status_data * status_data_out)
{
    bool success = false;
    if (block_size > 0 && block != 0)
    {
        c_bitstream packet;
        bitstream_initialize(&packet, (const unsigned char *)block, block_size);
        packet.reset(3);
        success = squad_status_decode(&packet, status_data_out);
        success = success && !packet.overflowed();
        success = success && !packet.error_occurred();
    }
    return success;
}

// @address 0x3c83c0 ?player_configuration_client_global_stats_encode@@YAXPAVc_bitstream@@PBUs_queried_player_global_statistics@@W4e_player_configuration_encoding_type@@@Z
void player_configuration_client_global_stats_encode(
    c_bitstream *packet, const s_queried_player_global_statistics *statistics, e_player_configuration_encoding_type encoding_type)
{
    packet->write_bool("global-stats-valid", statistics->valid);
    if (statistics->valid)
    {
        if (encoding_type == _player_configuration_encoding_full)
        {
            packet->write_integer("global-stats-experience-base", statistics->experience_base, 17);
            packet->write_signed_integer("global-stats-experience-penalty", statistics->experience_penalty, 18);
        }
        packet->write_integer("global-stats-highest-skill", statistics->highest_skill, 6);
    }
}

// @address 0x3c8438 ?player_configuration_client_global_stats_decode@@YA_NPAVc_bitstream@@PAUs_queried_player_global_statistics@@W4e_player_configuration_encoding_type@@@Z
bool player_configuration_client_global_stats_decode(
    c_bitstream *packet, s_queried_player_global_statistics *statistics, e_player_configuration_encoding_type encoding_type)
{
    statistics->valid = packet->read_bool("global-stats-valid");
    if (statistics->valid)
    {
        if (encoding_type == _player_configuration_encoding_full)
        {
            statistics->experience_base = packet->read_integer("global-stats-experience-base", 17);
            statistics->experience_penalty = network_game_definitions_read_signed_integer(packet, "global-stats-experience-penalty", 18);
        }
        statistics->highest_skill = packet->read_integer("global-stats-highest-skill", 6);
    }
    return true;
}

// @address 0x3c84d8 ?player_configuration_client_hopper_stats_encode@@YAXPAVc_bitstream@@PBUs_queried_player_hopper_statistics@@@Z
void player_configuration_client_hopper_stats_encode(c_bitstream *packet, const s_queried_player_hopper_statistics *statistics)
{
    packet->write_bool("hopper-stats-valid", statistics->valid);
    if (statistics->valid)
    {
        packet->write_integer("hopper-identifier", statistics->hopper_identifier, 16);
        unsigned long mu_bits;
        memcpy(&mu_bits, &statistics->mu, sizeof(mu_bits));
        packet->write_integer("hopper-mu", mu_bits, 32);
        unsigned long sigma_bits;
        memcpy(&sigma_bits, &statistics->sigma, sizeof(sigma_bits));
        packet->write_integer("hopper-sigma", sigma_bits, 32);
        packet->write_integer("hopper-old-skill", statistics->old_skill, 6);
        packet->write_integer("hopper-games-played", statistics->games_played, 17);
        packet->write_integer("hopper-games-completed", statistics->games_completed, 17);
        packet->write_integer("hopper-games-won", statistics->games_won, 17);
    }
}

// @address 0x3c8568 ?player_configuration_client_hopper_stats_decode@@YA_NPAVc_bitstream@@PAUs_queried_player_hopper_statistics@@@Z
bool player_configuration_client_hopper_stats_decode(c_bitstream *packet, s_queried_player_hopper_statistics *statistics)
{
    statistics->valid = packet->read_bool("hopper-stats-valid");
    if (statistics->valid)
    {
        statistics->hopper_identifier = (unsigned short)packet->read_integer("hopper-identifier", 16);
        unsigned long mu_bits = packet->read_integer("hopper-mu", 32);
        memcpy(&statistics->mu, &mu_bits, sizeof(statistics->mu));
        unsigned long sigma_bits = packet->read_integer("hopper-sigma", 32);
        memcpy(&statistics->sigma, &sigma_bits, sizeof(statistics->sigma));
        statistics->old_skill = packet->read_integer("hopper-old-skill", 6);
        statistics->games_played = packet->read_integer("hopper-games-played", 17);
        statistics->games_completed = packet->read_integer("hopper-games-completed", 17);
        statistics->games_won = packet->read_integer("hopper-games-won", 17);
    }
    return true;
}

// @address 0x3c8c80 ?player_configuration_host_encode@@YAXPAVc_bitstream@@PBUs_player_configuration_from_host@@W4e_player_configuration_encoding_type@@@Z
void player_configuration_host_encode(c_bitstream *packet, const s_player_configuration_from_host *player_configuration_from_host,
    e_player_configuration_encoding_type encoding_type)
{
    packet->write_string_wchar_remapped("player-name", (const unsigned short *)player_configuration_from_host->name, 16);
    packet->write_bool("player-team-valid", player_configuration_from_host->team_index + 1 != 0);
    if (player_configuration_from_host->team_index != -1)
    {
        packet->write_integer("player-team", player_configuration_from_host->team_index, 5);
    }
    if (encoding_type == _player_configuration_encoding_full)
    {
        packet->write_bool("player-user-team-valid", player_configuration_from_host->user_team_index + 1 != 0);
        if (player_configuration_from_host->user_team_index != -1)
        {
            packet->write_integer("player-user-team", player_configuration_from_host->user_team_index, 5);
        }
    }
    packet->write_bool("stats-global-valid", player_configuration_from_host->global_stats_valid);
    if (player_configuration_from_host->global_stats_valid)
    {
        packet->write_signed_integer("stats-global-experience", player_configuration_from_host->global_experience, 18);
        packet->write_integer("stats-global-rank", player_configuration_from_host->global_rank, 4);
        packet->write_integer("stats-global-grade", player_configuration_from_host->global_grade, 2);
    }
    packet->write_bool("stats-hopper-valid", player_configuration_from_host->hopper_stats_valid);
    if (player_configuration_from_host->hopper_stats_valid)
    {
        packet->write_integer("stats-hopper-skill-display", player_configuration_from_host->hopper_skill_display, 6);
        if (encoding_type == _player_configuration_encoding_full)
        {
            packet->write_integer("stats-hopper-skill", player_configuration_from_host->hopper_skill, 6);
            packet->write_integer("stats-hopper-weight", player_configuration_from_host->hopper_weight, 7);
        }
    }
}

// @address 0x3c8da8 ?player_configuration_host_decode@@YA_NPAVc_bitstream@@PAUs_player_configuration_from_host@@W4e_player_configuration_encoding_type@@@Z
bool player_configuration_host_decode(c_bitstream *packet, s_player_configuration_from_host *from_host, e_player_configuration_encoding_type encoding_type)
{
    bool success = true;
    packet->read_string_wchar_remapped("player-name", (unsigned short *)from_host->name, 16);
    if (packet->read_bool("player-team-valid"))
    {
        from_host->team_index = packet->read_integer("player-team", 5);
    }
    else
    {
        from_host->team_index = -1;
    }
    if (encoding_type == _player_configuration_encoding_full && packet->read_bool("player-user-team-valid"))
    {
        from_host->user_team_index = packet->read_integer("player-user-team", 5);
    }
    else
    {
        from_host->user_team_index = -1;
    }
    from_host->global_stats_valid = packet->read_bool("stats-global-valid");
    if (from_host->global_stats_valid)
    {
        from_host->global_experience = network_game_definitions_read_signed_integer(packet, "stats-global-experience", 18);
        from_host->global_rank = packet->read_integer("stats-global-rank", 4);
        from_host->global_grade = packet->read_integer("stats-global-grade", 2);
    }
    from_host->hopper_stats_valid = packet->read_bool("stats-hopper-valid");
    if (from_host->hopper_stats_valid)
    {
        from_host->hopper_skill_display = packet->read_integer("stats-hopper-skill-display", 6);
        if (encoding_type == _player_configuration_encoding_full)
        {
            from_host->hopper_skill = packet->read_integer("stats-hopper-skill", 6);
            from_host->hopper_weight = packet->read_integer("stats-hopper-weight", 7);
        }
    }

    success = success && (from_host->team_index == -1 || (from_host->team_index >= 0 && from_host->team_index < 16));
    success = success && (from_host->user_team_index == -1 || (from_host->user_team_index >= 0 && from_host->user_team_index < 16));
    if (from_host->global_stats_valid)
    {
        success = success && from_host->global_rank >= 0 && from_host->global_rank < 14;
        success = success && from_host->global_grade >= 0 && from_host->global_grade < 4;
    }
    if (from_host->hopper_stats_valid)
    {
        success = success && from_host->hopper_skill_display >= 1 && from_host->hopper_skill_display <= 50;
        if (encoding_type == _player_configuration_encoding_full)
        {
            success = success && from_host->hopper_skill >= 1 && from_host->hopper_skill <= 50;
            success = success && from_host->hopper_weight >= 0 && from_host->hopper_weight < 101;
        }
    }
    return success;
}


extern "C" {

}
// @address 0x3c9048 matchmaking_calculate_best_possible_host
void matchmaking_calculate_best_possible_host(
    const c_network_session_membership *membership_pointer, long *host, long *bandwidth, long *capacity, long *maximum, long *good)
{
 const c_network_session_membership *m = membership_pointer;
 long best_host=m->host_peer_index();
     long best_bandwidth=m->get_host_peer()->properties.upstream_bandwidth;
     long best_capacity=m->get_host_peer()->properties.downstream_bandwidth;
     long max=0;
     long good_count=0;
 for (long i = (membership_pointer)->get_first_peer(); i != -1;
     i = (membership_pointer)->get_next_peer(i))
 {
     const s_network_session_peer *p = m->get_peer(i);
     bool skip = g_network_configuration.matchmaking_disallow_restricted_nat_hosts;
     if (skip && p->properties.nat_type == 3)
         continue;
 if (p->properties.upstream_bandwidth >= g_network_configuration.matchmaking_host_minimum_upstream_bandwidth &&
     p->properties.downstream_bandwidth >= g_network_configuration.matchmaking_host_minimum_downstream_bandwidth
     && p->properties.connectivity.peer_connectivity_mask == m->get_peer_valid_mask())
 {
     if (p->properties.upstream_bandwidth >= g_network_configuration.matchmaking_good_host_upstream_bandwidth &&
         p->properties.downstream_bandwidth >= g_network_configuration.matchmaking_good_host_downstream_bandwidth)
         good_count++;
     if (p->properties.downstream_bandwidth >= best_capacity + g_network_configuration.matchmaking_host_downstream_bandwidth_improvement)
     {
         best_host = i;
         best_bandwidth = p->properties.upstream_bandwidth;
         best_capacity = p->properties.downstream_bandwidth;
     }
 }
 }
 for(long i=16;i>0;i--) {if(best_capacity>=g_network_configuration.machine_capacity_bandwidth_thresholds[i]) {max=i;break;}}
 max=max>g_network_configuration.matchmaking_host_minimum_machine_capacity?max:g_network_configuration.matchmaking_host_minimum_machine_capacity;
 if(bandwidth) *bandwidth=best_bandwidth;if(capacity) *capacity=best_capacity;if(good) *good=good_count;
}



void player_appearance_encode(c_bitstream *packet, const s_player_appearance *appearance, e_player_configuration_encoding_type encoding_type)
{
    packet->write_bool("appearance-flags", (appearance->flags & 1) != 0);
    packet->write_integer("primary-color", appearance->change_colors[0] + 1, 6);
    packet->write_integer("secondary-color", appearance->change_colors[1] + 1, 6);
    packet->write_integer("tertiary-color", appearance->change_colors[2] + 1, 6);
    packet->write_integer("player-model-choice", appearance->player_model_choice, 1);
    if (encoding_type == _player_configuration_encoding_full)
    {
        const char *model_area_names[2][4] =
        {
            {"spartan-model-area0", "spartan-model-area1", "spartan-model-area2", "spartan-model-area3"},
            {"elite-model-area0", "elite-model-area1", "elite-model-area2", "elite-model-area3"}
        };
        for (long model_index = 0; model_index < 2; model_index++)
        {
            for (long area_index = 0; area_index < 4; area_index++)
            {
                packet->write_integer(model_area_names[model_index][area_index], appearance->model_permutations[model_index][area_index], 5);
            }
        }
    }
    packet->write_string_wchar_remapped("service-tag", (const unsigned short *)appearance->service_tag, 4);
    packet->write_integer("foreground-emblem", appearance->foreground_emblem, 7);
    packet->write_integer("background-emblem", appearance->background_emblem, 7);
    packet->write_integer("emblem-flags", appearance->emblem_flags, 3);
    packet->write_integer("emblem-primary-color", appearance->emblem_primary_color, 6);
    packet->write_integer("emblem-secondary-color", appearance->emblem_secondary_color, 6);
    packet->write_integer("emblem-background-color", appearance->emblem_background_color, 6);
}

// @address 0x3c9288 ?player_appearance_decode@@YA_NPAVc_bitstream@@PAUs_player_appearance@@W4e_player_configuration_encoding_type@@@Z
bool player_appearance_decode(c_bitstream *packet, s_player_appearance *appearance, e_player_configuration_encoding_type encoding_type)
{
    memset(appearance, 0, sizeof(*appearance));
    if (packet->read_bool("appearance-flags"))
        appearance->flags |= 1;
    else
        appearance->flags &= ~1;
    appearance->change_colors[0] = (signed char)(packet->read_integer("primary-color", 6) - 1);
    appearance->change_colors[1] = (signed char)(packet->read_integer("secondary-color", 6) - 1);
    appearance->change_colors[2] = (signed char)(packet->read_integer("tertiary-color", 6) - 1);
    appearance->player_model_choice = (signed char)packet->read_integer("player-model-choice", 1);
    if (encoding_type == _player_configuration_encoding_full)
    {
        const char *model_area_names[2][4] =
        {
            {"spartan-model-area0", "spartan-model-area1", "spartan-model-area2", "spartan-model-area3"},
            {"elite-model-area0", "elite-model-area1", "elite-model-area2", "elite-model-area3"}
        };
        for (long model_index = 0; model_index < 2; model_index++)
        {
            for (long area_index = 0; area_index < 4; area_index++)
            {
                long permutation = packet->read_integer(model_area_names[model_index][area_index], 5);
                appearance->model_permutations[model_index][area_index] = (unsigned char)permutation;
            }
        }
    }
    packet->read_string_wchar_remapped("service-tag", (unsigned short *)appearance->service_tag, 4);
    appearance->foreground_emblem = (unsigned char)packet->read_integer("foreground-emblem", 7);
    appearance->background_emblem = (unsigned char)packet->read_integer("background-emblem", 7);
    appearance->emblem_flags = (unsigned char)packet->read_integer("emblem-flags", 3);
    appearance->emblem_primary_color = (signed char)packet->read_integer("emblem-primary-color", 6);
    appearance->emblem_secondary_color = (signed char)packet->read_integer("emblem-secondary-color", 6);
    appearance->emblem_background_color = (signed char)packet->read_integer("emblem-background-color", 6);
    bool valid = player_appearance_valid(appearance);
    return valid;
}


bool network_session_qos_status_header_verify(const s_network_session_qos_status_header *header)
{
    bool valid = true;
    valid = valid && header != 0;
    valid = valid && (short)header->version >= 0 && (short)header->version < 2;
    valid = valid && header->status >= 0 && header->status < 6;
    valid = valid && (header->index0 == -1 || (header->index0 > 0 && header->index0 <= 65534));
    valid = valid && (header->index1 == -1 || (header->index1 > 0 && header->index0 <= 65534));
    return valid;
}


// @address 0x3c94d8 network_session_qos_status_header_decode
bool network_session_qos_status_header_decode(c_bitstream *packet, s_network_session_qos_status_header *header)
{
    header->type = (unsigned short)packet->read_integer("type", 8);
    header->version = (unsigned short)packet->read_integer("version", 2);
    header->status = packet->read_integer("status", 3);
    header->index0 = packet->read_integer("index0", 16) - 1;
    header->index1 = packet->read_integer("index1", 16) - 1;
    return !packet->error_occurred() && network_session_qos_status_header_verify(header);
}

struct s_matchmaking_gather_party_properties;






// @address 0x3c71b0 squad_status_verify
extern "C" bool squad_status_verify(const s_network_squad_status_data *squad_status_data)

{
  int executable_version_value;
  unsigned int is_valid;
  
  is_valid = true;
  is_valid = (unsigned char)is_valid && squad_status_data != 0;
  is_valid = (unsigned char)is_valid && squad_status_data->flags >= 0 && squad_status_data->flags < 2;
  is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->type >= 0 && squad_status_data->type < 6;
  if (((unsigned char)is_valid) &&
      ((executable_version_value = squad_status_data->version, executable_version_value == -1
      || ((executable_version_value >= 0 && (executable_version_value < 0xffff))))))
     {
    is_valid = true;
  }
  else {
    is_valid = false;
  }
  if (((unsigned char)is_valid) &&
     ((squad_status_data->build == -1 ||
      ((squad_status_data->version >= 0 && (squad_status_data->version < 0xffff)))))) {
    is_valid = true;
  }
  else {
    is_valid = false;
  }
  is_valid = (unsigned char)is_valid;
  if (squad_status_data->state == 1) {
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.session_class >= 0 && squad_status_data->details.session_class < 3;
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.session_type >= 0 && squad_status_data->details.session_type < 3;
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.join_mode >= 0 && squad_status_data->details.join_mode < 4;
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.sort_priority >= 0 && squad_status_data->details.sort_priority < 18;
    is_valid = (unsigned char)(unsigned char)is_valid && ((short)squad_status_data->details.slot_count_a) >= 0
        && ((short)squad_status_data->details.slot_count_a) < 17;
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.slot_count_b >= 0 && squad_status_data->details.slot_count_b < 17;
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.slot_count_c >= 0 && squad_status_data->details.slot_count_c < 17;
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.slot_count_d >= 0 && squad_status_data->details.slot_count_d < 17;
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.start_mode >= 0 && squad_status_data->details.start_mode < 4;
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.timer_type >= 0 && squad_status_data->details.timer_type < 3;
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.timer_seconds >= 0;
    is_valid = (unsigned char)(unsigned char)is_valid && squad_status_data->details.player_count >= 0 && squad_status_data->details.player_count < 17;
    is_valid = (unsigned char)(unsigned char)is_valid && (squad_status_data->details.occupied_team_mask & 0xffffff00) == 0;
    return (unsigned char)is_valid &&
        (squad_status_data->details.campaign_difficulty == -1
            || (squad_status_data->details.campaign_difficulty >= 0 && squad_status_data->details.campaign_difficulty < 4));
  }
  else if (squad_status_data->state == 2) {
    return (unsigned char)is_valid &&
        (network_matchmaking_gather_party_properties_valid(&squad_status_data->details.gather_party_properties) !=
            0);
  }
  return false;
}





// @address 0x3c8850 ?player_configuration_client_decode@@YA_NPAVc_bitstream@@PAUs_player_configuration_from_client@@W4e_player_configuration_encoding_type@@@Z
bool player_configuration_client_decode(
    c_bitstream *packet, s_player_configuration_from_client *from_client, e_player_configuration_encoding_type encoding_type)
{
    bool success = true;
    if (encoding_type == _player_configuration_encoding_full)
        packet->read_string_wchar_remapped("player-name", (unsigned short *)from_client->name, 16);
    bool appearance_valid = player_appearance_decode(packet, &from_client->appearance, encoding_type);
    packet->read_raw_data("player-xuid", &from_client->xuid, 64);
    from_client->online = packet->read_bool("is-silver-or-gold-live");
    from_client->silver = packet->read_bool("is-online-enabled");
    from_client->is_content_creator = packet->read_bool("is-content-creator");
    if (packet->read_bool("player-last-team-valid"))
        from_client->desired_team_index = (signed char)packet->read_integer("player-last-team", 5);
    else
        from_client->desired_team_index = -1;
    if (encoding_type == _player_configuration_encoding_full)
    {
        from_client->ready = packet->read_bool("desires-veto");
        from_client->desires_rematch = packet->read_bool("desires-rematch");
        from_client->hopper_access_flags = (unsigned char)packet->read_integer("hopper-access-flags", 8);
        from_client->is_free_live_gold_account = packet->read_bool("is-free-live-gold-account");
        from_client->guest = packet->read_bool("has-beta-permissions");
        from_client->bot = packet->read_bool("has-external-storage");
        from_client->is_griefer = packet->read_bool("player-is-griefer") != 0;
        from_client->bungienet_user = packet->read_integer("bungienet-user", 32);
        from_client->cheat_flags = packet->read_integer("cheat-flags", 1);
        from_client->player_flags = packet->read_integer("ban-flags", 10);
        from_client->gamer_region = packet->read_integer("gamer-region", 7);
        from_client->gamer_zone = packet->read_integer("gamer-zone", 3);
        from_client->repeated_play_coefficient = packet->read_integer("repeated-play-coefficient", 7) - 1;
        from_client->experience_growth_banned = packet->read_bool("experience-growth-banned");
    }
    from_client->bungienet_user_role = (signed char)packet->read_integer("bungienet-user-role", 4);
    success = success && player_configuration_client_global_stats_decode(packet, &from_client->global_statistics, encoding_type);
    if (encoding_type == _player_configuration_encoding_full)
    {
        from_client->displayed_statistics.valid = packet->read_bool("displayed-stats-valid");
        if (from_client->displayed_statistics.valid)
        {
            from_client->displayed_statistics.ranked_completed = packet->read_integer("displayed-stats-ranked-completed", 17);
            from_client->displayed_statistics.unranked_completed = packet->read_integer("displayed-stats-unranked-completed", 17);
            from_client->displayed_statistics.custom_completed = packet->read_integer("displayed-stats-custom-completed", 17);
            from_client->displayed_statistics.ranked_won = packet->read_integer("displayed-stats-ranked-won", 17);
            from_client->displayed_statistics.unranked_won = packet->read_integer("displayed-stats-unranked-won", 17);
            from_client->displayed_statistics.custom_won = packet->read_integer("displayed-stats-custom-won", 17);
            from_client->displayed_statistics.ranked_played = packet->read_integer("displayed-stats-ranked-played", 17);
            from_client->displayed_statistics.unranked_played = packet->read_integer("displayed-stats-unranked-played", 17);
            from_client->displayed_statistics.first_played = packet->read_integer("displayed-stats-first-played", 32);
            from_client->displayed_statistics.last_played = packet->read_integer("displayed-stats-last-played", 32);
        }
    }
    success = success &&
        (encoding_type == _player_configuration_encoding_full
                ? player_configuration_client_hopper_stats_decode(packet, &from_client->hopper_statistics)
                : true);
    success = success && wcslen(from_client->name) < 16;
    success = success && appearance_valid;
    success = success && (from_client->desired_team_index == -1 || (from_client->desired_team_index >= 0 && from_client->desired_team_index < 16));
    success = success && from_client->bungienet_user_role >= 0 && from_client->bungienet_user_role < 9;
    if (encoding_type == _player_configuration_encoding_full)
        success = success &&
            (from_client->repeated_play_coefficient == -1 ||
                (from_client->repeated_play_coefficient >= 0 && from_client->repeated_play_coefficient <= 100));
    if (from_client->global_statistics.valid)
        success = success && from_client->global_statistics.highest_skill >= 1 && from_client->global_statistics.highest_skill <= 50;
    return success;
}

// @address 0x247448 network_game_definition_player_decode

void network_game_definition_player_decode(c_bitstream *packet, s_game_results_player_data *player)
{
    player->player_exists = packet->read_bool("player-exists");
    player->machine_index = (signed char)(packet->read_integer("machine-index", 5) - 1);
    packet->read_raw_data("data", player->identifier, 64);
    player_configuration_client_decode(packet, &player->configuration.client, _player_configuration_encoding_full);
    player_configuration_host_decode(packet, &player->configuration.host, _player_configuration_encoding_full);
    player->standing = (signed char)(packet->read_integer("standing", 5) - 1);
    unsigned char *buffer = (unsigned char *)&player->final_score;
    unsigned __int64 value = bitstream_read_window_bits(packet, 16) << 48;
    unsigned long bytes = 2;
    do {
        *buffer++ = (unsigned char)(value >> 56);
        value <<= 8;
        --bytes;
    } while (bytes != 0);
}

// the March 2007 debug name is player_configuration_client_encode


// @address 0x3c8638 player_configuration_client_encode_packet
void player_configuration_client_encode_packet(c_bitstream *packet,
    const s_network_game_definitions_client_configuration *client_pointer,
    e_player_configuration_encoding_type encoding_type)
{
    const s_player_configuration_from_client *player_configuration_from_client =
        (const s_player_configuration_from_client *)client_pointer;
    if (encoding_type == _player_configuration_encoding_full)
    {
        packet->write_string_wchar_remapped("player-name", (const unsigned short *)player_configuration_from_client->name, 16);
    }
    player_appearance_encode(packet, &player_configuration_from_client->appearance, encoding_type);
    packet->write_raw_data("player-xuid", &player_configuration_from_client->xuid, 64);
    packet->write_bool("is-silver-or-gold-live", player_configuration_from_client->online);
    packet->write_bool("is-online-enabled", player_configuration_from_client->silver);
    packet->write_bool("unknown-4a", player_configuration_from_client->is_content_creator);
    packet->write_bool("player-last-team-valid", player_configuration_from_client->desired_team_index + 1 != 0);
    if (player_configuration_from_client->desired_team_index != -1)
    {
        packet->write_integer("player-last-team", player_configuration_from_client->desired_team_index, 5);
    }
    if (encoding_type == _player_configuration_encoding_full)
    {
        packet->write_bool("desires-veto", player_configuration_from_client->ready);
        packet->write_bool("desires-rematch", player_configuration_from_client->desires_rematch);
        packet->write_integer("hopper-access-flags", player_configuration_from_client->hopper_access_flags, 8);
        packet->write_bool("is-free-live-gold-account", player_configuration_from_client->is_free_live_gold_account);
        packet->write_bool("has-beta-permissions", player_configuration_from_client->guest);
        packet->write_bool("unknown-51", player_configuration_from_client->bot);
        packet->write_bool("player-is-griefer", (signed char)player_configuration_from_client->is_griefer > 0);
        packet->write_integer("unknown-54", player_configuration_from_client->bungienet_user, 32);
        packet->write_integer("cheat-flags", player_configuration_from_client->cheat_flags, 1);
        packet->write_integer("ban-flags", player_configuration_from_client->player_flags, 10);
        packet->write_integer("gamer-region", player_configuration_from_client->gamer_region, 7);
        packet->write_integer("gamer-zone", player_configuration_from_client->gamer_zone, 3);
        packet->write_integer("repeated_play_coefficient", player_configuration_from_client->repeated_play_coefficient + 1, 7);
        packet->write_bool("experience_growth_banned", player_configuration_from_client->experience_growth_banned);
    }
    packet->write_integer("bungienet-user-role", player_configuration_from_client->bungienet_user_role, 4);
    player_configuration_client_global_stats_encode(packet, &player_configuration_from_client->global_statistics, encoding_type);
    if (encoding_type == _player_configuration_encoding_full)
    {
        packet->write_bool("displayed-stats-valid", player_configuration_from_client->displayed_statistics.valid);
        if (player_configuration_from_client->displayed_statistics.valid)
        {
            packet->write_integer("displayed-stats-ranked-completed", player_configuration_from_client->displayed_statistics.ranked_completed, 17);
            packet->write_integer("displayed-stats-unranked-completed", player_configuration_from_client->displayed_statistics.unranked_completed, 17);
            packet->write_integer("displayed-stats-custom-completed", player_configuration_from_client->displayed_statistics.custom_completed, 17);
            packet->write_integer("displayed-stats-ranked-won", player_configuration_from_client->displayed_statistics.ranked_won, 17);
            packet->write_integer("displayed-stats-unranked-won", player_configuration_from_client->displayed_statistics.unranked_won, 17);
            packet->write_integer("displayed-stats-custom-won", player_configuration_from_client->displayed_statistics.custom_won, 17);
            packet->write_integer("displayed-stats-ranked-played", player_configuration_from_client->displayed_statistics.ranked_played, 17);
            packet->write_integer("displayed-stats-unranked-played", player_configuration_from_client->displayed_statistics.unranked_played, 17);
            packet->write_integer("displayed-stats-first-played", player_configuration_from_client->displayed_statistics.first_played, 32);
            packet->write_integer("displayed-stats-last-played", player_configuration_from_client->displayed_statistics.last_played, 32);
        }
        player_configuration_client_hopper_stats_encode(packet, &player_configuration_from_client->hopper_statistics);
    }
}








static __forceinline void network_game_definitions_player_configuration_client_encode(c_bitstream *packet,
    const s_player_configuration_from_client *player_configuration_from_client,
    e_player_configuration_encoding_type encoding_type)
{
    const unsigned __int64 *const &xuid_ptr = &player_configuration_from_client->xuid;
    if (encoding_type == _player_configuration_encoding_full)
    {
        packet->write_string_wchar_remapped("player-name", (const unsigned short *)player_configuration_from_client->name, 16);
    }
    player_appearance_encode(packet, &player_configuration_from_client->appearance, encoding_type);
    packet->write_raw_data("player-xuid", xuid_ptr, 64);
    packet->write_bool("is-silver-or-gold-live", player_configuration_from_client->online);
    packet->write_bool("is-online-enabled", player_configuration_from_client->silver);
    packet->write_bool("unknown-4a", player_configuration_from_client->is_content_creator);
    packet->write_bool("player-last-team-valid", player_configuration_from_client->desired_team_index + 1 != 0);
    if (player_configuration_from_client->desired_team_index != -1)
    {
        packet->write_integer("player-last-team", player_configuration_from_client->desired_team_index, 5);
    }
    if (encoding_type == _player_configuration_encoding_full)
    {
        packet->write_bool("desires-veto", player_configuration_from_client->ready);
        packet->write_bool("desires-rematch", player_configuration_from_client->desires_rematch);
        packet->write_integer("hopper-access-flags", player_configuration_from_client->hopper_access_flags, 8);
        packet->write_bool("is-free-live-gold-account", player_configuration_from_client->is_free_live_gold_account);
        packet->write_bool("has-beta-permissions", player_configuration_from_client->guest);
        packet->write_bool("unknown-51", player_configuration_from_client->bot);
        packet->write_bool("player-is-griefer", (signed char)player_configuration_from_client->is_griefer > 0);
        packet->write_integer("unknown-54", player_configuration_from_client->bungienet_user, 32);
        packet->write_integer("cheat-flags", player_configuration_from_client->cheat_flags, 1);
        packet->write_integer("ban-flags", player_configuration_from_client->player_flags, 10);
        packet->write_integer("gamer-region", player_configuration_from_client->gamer_region, 7);
        packet->write_integer("gamer-zone", player_configuration_from_client->gamer_zone, 3);
        packet->write_integer("repeated_play_coefficient", player_configuration_from_client->repeated_play_coefficient + 1, 7);
        packet->write_bool("experience_growth_banned", player_configuration_from_client->experience_growth_banned);
    }
    packet->write_integer("bungienet-user-role", player_configuration_from_client->bungienet_user_role, 4);
    packet->write_bool("global-stats-valid", player_configuration_from_client->global_statistics.valid);
    if (player_configuration_from_client->global_statistics.valid)
    {
        if (encoding_type == _player_configuration_encoding_full)
        {
            packet->write_integer("global-stats-experience-base", player_configuration_from_client->global_statistics.experience_base, 17);
            packet->write_signed_integer("global-stats-experience-penalty", player_configuration_from_client->global_statistics.experience_penalty, 18);
        }
        packet->write_integer("global-stats-highest-skill", player_configuration_from_client->global_statistics.highest_skill, 6);
    }
    if (encoding_type == _player_configuration_encoding_full)
    {
        packet->write_bool("displayed-stats-valid", player_configuration_from_client->displayed_statistics.valid);
        if (player_configuration_from_client->displayed_statistics.valid)
        {
            packet->write_integer("displayed-stats-ranked-completed", player_configuration_from_client->displayed_statistics.ranked_completed, 17);
            packet->write_integer("displayed-stats-unranked-completed", player_configuration_from_client->displayed_statistics.unranked_completed, 17);
            packet->write_integer("displayed-stats-custom-completed", player_configuration_from_client->displayed_statistics.custom_completed, 17);
            packet->write_integer("displayed-stats-ranked-won", player_configuration_from_client->displayed_statistics.ranked_won, 17);
            packet->write_integer("displayed-stats-unranked-won", player_configuration_from_client->displayed_statistics.unranked_won, 17);
            packet->write_integer("displayed-stats-custom-won", player_configuration_from_client->displayed_statistics.custom_won, 17);
            packet->write_integer("displayed-stats-ranked-played", player_configuration_from_client->displayed_statistics.ranked_played, 17);
            packet->write_integer("displayed-stats-unranked-played", player_configuration_from_client->displayed_statistics.unranked_played, 17);
            packet->write_integer("displayed-stats-first-played", player_configuration_from_client->displayed_statistics.first_played, 32);
            packet->write_integer("displayed-stats-last-played", player_configuration_from_client->displayed_statistics.last_played, 32);
        }
        player_configuration_client_hopper_stats_encode(packet, &player_configuration_from_client->hopper_statistics);
    }
}

// @address 0x3c7550 ?squad_status_encode@@YAXPAVc_bitstream@@PBUs_network_squad_status_data@@@Z
void squad_status_encode(c_bitstream *packet, const s_network_squad_status_data *status_data)
{
// match: 0x3c7550: value range assumption guides branch lowering
__assume(packet != 0);
    packet->write_integer("protocol", status_data->protocol, 8);
    packet->write_integer("platform", status_data->flags, 2);
    packet->write_integer("executable-type", status_data->type, 3);
    packet->write_integer("executable-version", status_data->version + 1, 16);
    packet->write_integer("compatible-version", status_data->build + 1, 16);
    packet->write_integer("session-mode", status_data->state, 2);
    if (status_data->state == 1)
    {
        packet->write_integer("session-class", status_data->details.session_class, 2);
        packet->write_integer("session-type", status_data->details.session_type, 2);
        packet->write_integer("privacy-mode", status_data->details.join_mode, 2);
        char session_name[32];
        wchar_string_to_utf8_string(status_data->details.session_name, session_name, 32, 0);
        packet->write_string("session-name", session_name, 32);
        packet->write_integer("life-cycle-state", status_data->details.sort_priority, 5);
        packet->write_raw_data("session-id", &status_data->details.session_id, 64);
        packet->write_raw_data("session-key", &status_data->details.session_key, 128);
        packet->write_raw_data("host-address", &status_data->details.secure_address, 288);
        packet->write_integer("public-slots", ((short)status_data->details.slot_count_a), 5);
        packet->write_integer("private-slots", status_data->details.slot_count_b, 5);
        packet->write_integer("public-players", status_data->details.slot_count_c, 5);
        packet->write_integer("private-players", status_data->details.slot_count_d, 5);
        packet->write_string("playlist-name", status_data->details.hopper_name, 16);
        packet->write_integer("game-mode", status_data->details.kind + 1, 3);
        packet->write_integer("game-state", ((short)status_data->details.variant), 3);
        packet->write_integer("network-game-type", status_data->details.start_mode, 2);
        packet->write_integer("connection-quality", status_data->details.average_rating, 3);
        packet->write_integer("game-engine-type", status_data->details.game_engine_type, 4);
        packet->write_string_wchar("game-engine-variant-name", ((unsigned short *)status_data->details.game_variant_name), 16);
        packet->write_integer("campaign-id", status_data->details.campaign_id, 32);
        packet->write_integer("campaign-difficulty-level", status_data->details.campaign_difficulty + 1, 3);
        packet->write_integer("map-id", status_data->details.map_id, 32);
        packet->write_string_wchar("map-variant-name", ((unsigned short *)status_data->details.map_variant_name), 16);
        packet->write_string_wchar("saved-film-name", ((unsigned short *)status_data->details.film_name), 16);
        packet->write_bool("game-has-teams", status_data->details.teams_enabled);
        packet->write_integer("game-team-count", status_data->details.maximum_team_count, 5);
        packet->write_integer("target-score", ((unsigned short)status_data->details.score_to_win_round), 16);
        packet->write_integer("current-score", ((unsigned short)status_data->details.best_player_score), 16);
        packet->write_integer("player-count", status_data->details.player_count, 5);
        long player_encoded_count = status_data->details.player_count;
        packet->write_integer("player-encoded-count", player_encoded_count, 5);
        for (long player_index = 0; player_index < player_encoded_count; player_index++)
        {
            if (player_index < status_data->details.player_count)
            {
                packet->write_raw_data("player-identifier", &status_data->details.players[player_index].identifier, 64);
                network_game_definitions_player_configuration_client_encode(packet,
                    &((s_network_squad_status_player_configuration &)status_data->details.players[player_index].configuration).client_configuration,
                    _player_configuration_encoding_squad_status);
                player_configuration_host_encode(
                    packet,
                    &((s_network_squad_status_player_configuration &)status_data->details.players[player_index].configuration).host_configuration,
                    _player_configuration_encoding_squad_status);
            }
            else
            {
                s_network_squad_status_player *player = (s_network_squad_status_player *)&status_data->details.players[player_index];
                if (player_index > 0)
                {
                    player->identifier = status_data->details.players[0].identifier;
                    ((s_network_squad_status_player_configuration &)player->configuration)
                        = ((s_network_squad_status_player_configuration &)status_data->details.players[0].configuration);
                }
                s_player_configuration_from_host *fake_host_configuration
                    = &((s_network_squad_status_player_configuration &)player->configuration).host_configuration;
                usnzprintf((wchar_t *)fake_host_configuration->name, 16, k_fake_host_name_digits);
                packet->write_raw_data("player-identifier", &player->identifier, 64);
                network_game_definitions_player_configuration_client_encode(
                    packet,
                    &((s_network_squad_status_player_configuration &)player->configuration).client_configuration,
                    _player_configuration_encoding_squad_status);
                player_configuration_host_encode(packet, fake_host_configuration, _player_configuration_encoding_squad_status);
            }
        }
        packet->write_integer("game-timer-type", status_data->details.timer_type, 2);
        packet->write_integer("game-timer-seconds", status_data->details.timer_seconds, 32);
        packet->write_integer("team-mask", status_data->details.occupied_team_mask, 8);
        unsigned long team_encoded_mask = status_data->details.occupied_team_mask;
        packet->write_integer("team-encoded-mask", team_encoded_mask, 8);
        for (long team_index = 0; team_index < 8; team_index++)
        {
            long team_bit = 1 << team_index;
            long score = 0;
            if ((status_data->details.occupied_team_mask & team_bit) != 0)
            {
                score = status_data->details.team_scores[team_index];
            }
            if ((team_encoded_mask & team_bit) != 0)
            {
                packet->write_integer("score", score, 32);
            }
        }
    }
    else if (status_data->state == 2)
    {
        const s_matchmaking_gather_party_properties *gather_party = &status_data->details.gather_party_properties;
        packet->write_integer("skill-preference", gather_party->preferences.type, 2);
        if (gather_party->preferences.type == 1)
        {
            packet->write_integer("skill-minimum", gather_party->preferences.minimum, 6);
            packet->write_integer("skill-maximum", gather_party->preferences.maximum, 6);
        }
        network_matchmaking_session_properties_encode(packet, &gather_party->session_properties);
        packet->write_integer("gather-seconds", gather_party->gather_seconds, 13);
    }
}


static __forceinline unsigned long network_game_definitions_read_score(c_bitstream *packet, const char *name)
{
    unsigned long value = packet->read_integer(name, 16);
    if (value & 0x8000)
        value |= 0xffff0000;
    return value;
}

// @address 0x3c7db0 ?squad_status_decode@@YA_NPAVc_bitstream@@PAUs_network_squad_status_data@@@Z
bool squad_status_decode(c_bitstream *packet, s_network_squad_status_data *status_data)
// match: 0x3c7db0: value range assumption guides branch lowering
{ __assume(packet != 0);
    bool success = true;
    network_session_qos_status_header_decode(packet, (s_network_session_qos_status_header *)status_data);
    status_data->state = packet->read_integer("session-mode", 2);
    if (status_data->state == 1)
    {
        char session_name[32] = {0};
        status_data->details.session_class = packet->read_integer("session-class", 2);
        status_data->details.session_type = packet->read_integer("session-type", 2);
        status_data->details.join_mode = packet->read_integer("privacy-mode", 2);
        packet->read_string("session-name", session_name, 32);
        ::utf8_string_to_wchar_string((utf8 const *)(const unsigned char *)session_name,
            (wchar_t *)(((unsigned short *)status_data->details.session_name)), 32, (long *)(0));
        status_data->details.sort_priority = (short)packet->read_integer("life-cycle-state", 5);
        packet->read_raw_data("session-id", &status_data->details.session_id, 64);
        packet->read_raw_data("session-key", &status_data->details.session_key, 128);
        packet->read_raw_data("host-address", &status_data->details.secure_address, 288);
        status_data->details.slot_count_a = (short)packet->read_integer("public-slots", 5);
        status_data->details.slot_count_b = (short)packet->read_integer("private-slots", 5);
        status_data->details.slot_count_c = (short)packet->read_integer("public-players", 5);
        status_data->details.slot_count_d = (short)packet->read_integer("private-players", 5);
        packet->read_string("playlist-name", status_data->details.hopper_name, 16);
        status_data->details.kind = (short)(packet->read_integer("game-mode", 3) - 1);
        status_data->details.variant = (short)packet->read_integer("game-state", 3);
        status_data->details.start_mode = (short)packet->read_integer("network-game-type", 2);
        status_data->details.average_rating = (short)packet->read_integer("connection-quality", 3);
        status_data->details.game_engine_type = packet->read_integer("game-engine-type", 4);
        packet->read_string_wchar("game-engine-variant-name", ((unsigned short *)status_data->details.game_variant_name), 16);
        status_data->details.campaign_id = packet->read_integer("campaign-id", 32);
        status_data->details.campaign_difficulty = packet->read_integer("campaign-difficulty-level", 3) - 1;
        status_data->details.map_id = packet->read_integer("map-id", 32);
        packet->read_string_wchar("map-variant-name", ((unsigned short *)status_data->details.map_variant_name), 16);
        packet->read_string_wchar("saved-film-name", ((unsigned short *)status_data->details.film_name), 16);
        status_data->details.teams_enabled = packet->read_bool("game-has-teams");
        status_data->details.maximum_team_count = (short)packet->read_integer("game-team-count", 5);
        status_data->details.score_to_win_round = (unsigned short)network_game_definitions_read_score(packet, "target-score");
        status_data->details.best_player_score = (unsigned short)network_game_definitions_read_score(packet, "current-score");
        status_data->details.player_count = (short)packet->read_integer("player-count", 5);
        success = success && status_data->details.player_count >= 0 && status_data->details.player_count <= 16;
        long player_encoded_count = packet->read_integer("player-encoded-count", 5);
        success = success && player_encoded_count >= status_data->details.player_count && player_encoded_count <= 16;
        player_encoded_count = (player_encoded_count > status_data->details.player_count ? player_encoded_count : status_data->details.player_count) < 16
            ? (player_encoded_count > status_data->details.player_count ? player_encoded_count : status_data->details.player_count) : 16;
        {
            for (long player_index = 0; success && player_index < player_encoded_count; player_index++)
            {
                packet->read_raw_data("player-identifier", &status_data->details.players[player_index].identifier, 64);
                player_configuration_client_decode(packet,
                    (s_player_configuration_from_client *)&((s_network_squad_status_player_configuration &)
                        status_data->details.players[player_index].configuration)
                    .client_configuration,
                    _player_configuration_encoding_squad_status);
                player_configuration_host_decode(
                    packet,
                    &((s_network_squad_status_player_configuration &)status_data->details.players[player_index].configuration).host_configuration,
                    _player_configuration_encoding_squad_status);
            }
        }
        status_data->details.timer_type = packet->read_integer("game-timer-type", 2);
        status_data->details.timer_seconds = packet->read_integer("game-timer-seconds", 32);
        status_data->details.occupied_team_mask = packet->read_integer("team-mask", 8);
        unsigned long team_encoded_mask = packet->read_integer("team-encoded-mask", 8);
        for (long team_index = 0; team_index < 8; team_index++)
        {
            long team_bit = 1 << team_index;
            if (team_encoded_mask & team_bit)
            {
                long score = packet->read_integer("score", 32);
                if (status_data->details.occupied_team_mask & team_bit)
                    status_data->details.team_scores[team_index] = score;
            }
        }
    }
    else if (status_data->state == 2)
    {
        network_matchmaking_gather_party_properties_decode(packet, &status_data->details.gather_party_properties);
    }
    success = success && !packet->error_occurred();
    success = success && squad_status_verify(status_data);
    return success;
}
