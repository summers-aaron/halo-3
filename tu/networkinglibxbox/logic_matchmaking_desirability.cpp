// networkinglibxbox/logic_matchmaking_desirability.obj: from the records 0x422d88 0x422e10 0x422ea0 0x422f08 0x422ff8 0x4230e8 0x4231f8 0x423a78 0x423b00 0x423ba0 0x423c70 0x423db8
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
// @address 0x422ea0 network_matchmaking_gather_party_properties_valid
// @address 0x423a78 ?network_matchmaking_session_properties_valid@@YA_NPBUs_matchmaking_session_properties@@@Z
// @address 0x4230e8 ?network_matchmaking_build_allowed_skill_range@@YAXPBUs_matchmaking_session_properties@@JPAJ1@Z
// @address 0x423c70 ?network_matchmaking_session_properties_encode@@YAXPAVc_bitstream@@PBUs_matchmaking_session_properties@@@Z
// @address 0x4231f8 ?network_matchmaking_compute_session_desirability@@YA?AW4e_matchmaking_desirability_result@@PBUs_matchmaking_gather_party_properties@@PBUs_matchmaking_search_party_properties@@PAJ@Z
// @address 0x441c00 ?network_team_picker_pick_teams@@YA_NJJJJ_N000JPBUs_network_team_picker_player@@PAJ2@Z
// @address 0x57b878 ?multiplayer_game_hopper_compute_skill_match_delta@@YAJJ@Z
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
// @address 0x422f08 ?network_matchmaking_gather_party_properties_decode@@YA_NPAVc_bitstream@@PAUs_matchmaking_gather_party_properties@@@Z
// @address 0x422ff8 ?network_matchmaking_search_party_properties_decode@@YA_NPAVc_bitstream@@PAUs_matchmaking_search_party_properties@@@Z
#include "networking/network_configuration.h"
#include <math.h>
#include "networking/network_configuration_types.h"
#include <string.h>
#include "game/multiplayer_game_hopper.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/network_team_picker.h"
#include "networking/logic/matchmaking_desirability_result_types.h"







#include "networking/network_team_picker_player_types.h"

#include "game/hopper_configuration.h"


template<typename t_type> inline const t_type &PIN(const t_type &value, const t_type &lower, const t_type &upper)
{
    const t_type &pinned = value > lower ? value : lower;
    return pinned < upper ? pinned : upper;
}

const long logic_matchmaking_desirability_skill_level_bounds[2] = { 1, 50 };


// 0x4230e8
void network_matchmaking_build_allowed_skill_range(const s_matchmaking_session_properties *session_properties, long skill_range,
    long *minimum_skill_level_out, long *maximum_skill_level_out)
{
    long minimum_allowed_skill_level = PIN(
        session_properties->minimum_skill_level - multiplayer_game_hopper_compute_skill_match_delta(session_properties->minimum_skill_level),
        logic_matchmaking_desirability_skill_level_bounds[0], logic_matchmaking_desirability_skill_level_bounds[1]);
    long maximum_allowed_skill_level = PIN(
        session_properties->maximum_skill_level + multiplayer_game_hopper_compute_skill_match_delta(session_properties->maximum_skill_level),
        logic_matchmaking_desirability_skill_level_bounds[0], logic_matchmaking_desirability_skill_level_bounds[1]);
    *maximum_skill_level_out = PIN(session_properties->minimum_skill_level + skill_range, minimum_allowed_skill_level, maximum_allowed_skill_level);
    *minimum_skill_level_out = PIN(session_properties->maximum_skill_level - skill_range, minimum_allowed_skill_level, maximum_allowed_skill_level);
}


// 0x422d88
// @address 0x422d88 network_matchmaking_build_gather_party_properties
extern "C" void network_matchmaking_build_gather_party_properties(
    const s_matchmaking_session_properties *composition, long seconds, long preference, s_matchmaking_gather_party_properties *properties)
{
    properties->gather_seconds = 0;
    memcpy(&properties->session_properties, composition, sizeof(properties->session_properties));
    properties->gather_seconds = (seconds > 0 ? seconds : 0) < 0x1fff ? (seconds > 0 ? seconds : 0) : 0x1fff;
    properties->preferences.type = preference;
    if (preference == 1)
    {
        network_matchmaking_build_allowed_skill_range(composition, g_network_configuration.strict_skill_range_end,
            (long *)&properties->preferences.minimum, (long *)&properties->preferences.maximum);
    }
}

// 0x422e10
// @address 0x422e10 network_matchmaking_build_search_party_properties
extern "C" void network_matchmaking_build_search_party_properties(
    const s_matchmaking_session_properties *composition, long seconds, long preference, s_matchmaking_search_party_properties *properties)
{
    memset(&properties->seconds, 0, sizeof(properties->seconds));
    properties->latency = 1000;
    memcpy(&properties->session_properties, composition, sizeof(properties->session_properties));
    properties->seconds = (seconds > 0 ? seconds : 0) < 0x3ff ? (seconds > 0 ? seconds : 0) : 0x3ff;
    properties->preferences.type = preference;
    if (preference == 1)
    {
        network_matchmaking_build_allowed_skill_range(composition, g_network_configuration.strict_skill_range_end,
            (long *)&properties->preferences.minimum, (long *)&properties->preferences.maximum);
    }
}

// @address 0x422ea0 network_matchmaking_gather_party_properties_valid
extern "C" bool network_matchmaking_gather_party_properties_valid(const s_matchmaking_gather_party_properties *properties)
{
    bool valid = true;
    valid = valid && network_matchmaking_session_properties_valid(&properties->session_properties);
    valid = valid && network_matchmaking_session_preferences_valid(&properties->preferences);
    return valid;
}

// 0x423a78
bool network_matchmaking_session_properties_valid(const s_matchmaking_session_properties *session_properties)
{
    bool valid = true;
    if (session_properties->player_count < 1 || session_properties->player_count > 16)
        valid = false;
    if (session_properties->average_skill_level < 1 || session_properties->average_skill_level > 50)
        valid = false;
    if (session_properties->minimum_skill_level < 1 || session_properties->minimum_skill_level > 50)
        valid = false;
    if (session_properties->maximum_skill_level < 1 || session_properties->maximum_skill_level > 50)
        valid = false;
    if (session_properties->average_experience < 0 || session_properties->average_experience >= 14)
        valid = false;
    return valid;
}

bool network_matchmaking_session_preferences_valid(const s_matchmaking_search_party_preferences *preferences)
{
    bool valid = true;
    valid = valid && preferences->type >= 0 && preferences->type < 4;
    if (valid && preferences->type == 1) {
        valid = valid && preferences->minimum >= 1 && preferences->minimum <= 50;
        valid = valid && preferences->maximum >= 1 && preferences->maximum <= 50;
        valid = valid && preferences->minimum <= preferences->maximum;
    }
    return valid;
}

// @address 0x423ba0 network_matchmaking_search_party_preferences_decode
bool network_matchmaking_search_party_preferences_decode(c_bitstream *packet, s_matchmaking_search_party_preferences *preferences)
{
    preferences->type = packet->read_integer("skill-preference", 2);
    if (preferences->type == 1)
    {
        preferences->minimum = packet->read_integer("skill-minimum", 6);
        preferences->maximum = packet->read_integer("skill-maximum", 6);
    }
    bool success = true;
    success = success && !packet->error_occurred();
    success = success && network_matchmaking_session_preferences_valid(preferences);
    return success;
}

void network_matchmaking_session_properties_encode(c_bitstream *packet, const s_matchmaking_session_properties *properties)
{
    packet->write_integer("hopper-identifier", properties->hopper_identifier, 16);
    packet->write_bool("hopper-ranked", properties->ranked);
    packet->write_integer("player-count", properties->player_count - 1, 4);
    packet->write_integer("player-valid-flags", properties->player_valid_flags[0], 16);
    long player_index = 0;
    const s_matchmaking_session_player_properties *player = properties->players;
    do
    {
        if (properties->player_valid_flags[player_index >> 5] & (1 << (player_index & 31)))
        {
            packet->write_integer("player-peer-index", player->peer_index, 5);
            packet->write_integer("player-party-index", player->party_index, 4);
        }
        player_index++;
        player++;
    } while (player_index < 16);
    packet->write_integer("avg-skill", properties->average_skill_level, 6);
    packet->write_integer("min-skill", properties->minimum_skill_level, 6);
    packet->write_integer("max-skill", properties->maximum_skill_level, 6);
    packet->write_raw_data("avg-mu", &properties->average_mu, 32);
    packet->write_integer("avg-rank", properties->average_experience, 4);
    packet->write_integer("party-needed", properties->preferred_party_size_mask, 16);
    packet->write_integer("party-join", properties->acceptable_party_size_mask, 16);
    packet->write_integer("good-host-count", properties->guest_count, 5);
    packet->write_integer("party-nat-type", properties->nat_type, 2);
    packet->write_integer("gamer-zone", properties->language, 3);
    packet->write_integer("gamer-region", properties->country, 7);
    packet->write_integer("language", properties->region, 4);
    packet->write_integer("nat-type", properties->worst_nat_type, 2);
    packet->write_integer("flags", properties->composition_flags, 4);
}

static __forceinline void logic_matchmaking_desirability_read_raw_data(c_bitstream *packet, const char *name, void *buffer, long size_in_bits)
{
    unsigned char *data = (unsigned char *)buffer;
    if ((unsigned long)size_in_bits >= 64)
    {
        unsigned long word_count = (unsigned long)size_in_bits / 64;
        size_in_bits -= word_count * 64;
        do
        {
            *(unsigned __int64 *)data = bitstream_read_window_bits(packet, 64);
            data += sizeof(unsigned __int64);
        } while (--word_count != 0);
    }
    if (size_in_bits != 0)
    {
        unsigned __int64 value = bitstream_read_window_bits(packet, size_in_bits) << (unsigned long)(64 - size_in_bits);
        if (size_in_bits >= 8)
        {
            unsigned long byte_count = (unsigned long)size_in_bits / 8;
            size_in_bits -= byte_count * 8;
            do
            {
                *data++ = (unsigned char)(value >> 56);
                value <<= 8;
            } while (--byte_count != 0);
        }
        if (size_in_bits != 0)
            *data = (unsigned char)(value >> 56);
    }
}

// 0x423db8
// @address 0x423db8 ?network_matchmaking_session_properties_decode@@YA_NPAVc_bitstream@@PAUs_matchmaking_session_properties@@@Z
bool network_matchmaking_session_properties_decode(c_bitstream *packet, s_matchmaking_session_properties *properties)
{
    bool valid = true;
    properties->hopper_identifier = (unsigned short)packet->read_integer("hopper-identifier", 16);
    properties->ranked = packet->read_bool("hopper-ranked");
    properties->player_count = packet->read_integer("player-count", 4) + 1;
    properties->player_valid_flags[0] = packet->read_integer("player-valid-flags", 16);
    for (long player_index = 0; player_index < 16; player_index++)
    {
        if (properties->player_valid_flags[player_index >> 5] & (1 << (player_index & 31)))
        {
            s_matchmaking_session_player_properties *player = &properties->players[player_index];
            player->peer_index = packet->read_integer("player-peer-index", 5);
            player->party_index = packet->read_integer("player-party-index", 4);
        }
    }
    properties->average_skill_level = packet->read_integer("avg-skill", 6);
    properties->minimum_skill_level = packet->read_integer("min-skill", 6);
    properties->maximum_skill_level = packet->read_integer("max-skill", 6);
    logic_matchmaking_desirability_read_raw_data(packet, "avg-mu", &properties->average_mu, 32);
    properties->average_experience = packet->read_integer("avg-rank", 4);
    properties->preferred_party_size_mask = packet->read_integer("party-needed", 16);
    properties->acceptable_party_size_mask = packet->read_integer("party-join", 16);
    properties->guest_count = packet->read_integer("good-host-count", 5);
    properties->nat_type = packet->read_integer("party-nat-type", 2);
    properties->language = packet->read_integer("gamer-zone", 3);
    properties->country = packet->read_integer("gamer-region", 7);
    properties->region = packet->read_integer("language", 4);
    properties->worst_nat_type = packet->read_integer("nat-type", 2);
    properties->composition_flags = (unsigned char)packet->read_integer("flags", 4);
    valid = valid && !packet->error_occurred();
    valid = valid && properties->player_count >= 1 && properties->player_count <= 16;
    valid = valid && properties->average_skill_level >= 1 && properties->average_skill_level <= 50;
    valid = valid && properties->minimum_skill_level >= 1 && properties->minimum_skill_level <= 50;
    valid = valid && properties->maximum_skill_level >= 1 && properties->maximum_skill_level <= 50;
    valid = valid && properties->average_experience >= 0 && properties->average_experience < 14;
    valid = valid && properties->guest_count >= 0 && properties->guest_count < 16;
    valid = valid && properties->nat_type >= 0 && properties->nat_type < 4;
    valid = valid && properties->worst_nat_type >= 0 && properties->worst_nat_type < 4;
    valid = valid && properties->language >= 0 && properties->language < 5;
    valid = valid && properties->country >= 0 && properties->country < 108;
    valid = valid && properties->region >= 0 && properties->region < 12;
    return valid;
}

// 0x422f08
bool network_matchmaking_gather_party_properties_decode(c_bitstream *packet, s_matchmaking_gather_party_properties *properties)
{
    bool success = true;
    success = success && network_matchmaking_search_party_preferences_decode(packet, &properties->preferences);
    success = success && network_matchmaking_session_properties_decode(packet, &properties->session_properties);
    properties->gather_seconds = packet->read_integer("gather-seconds", 13);
    success = success && !packet->error_occurred();
    success = success && network_matchmaking_gather_party_properties_valid(properties);
    return success;
}

static __forceinline bool logic_matchmaking_desirability_search_party_properties_valid(const s_matchmaking_search_party_properties *properties)
{
    bool valid = true;
    valid = valid && network_matchmaking_session_properties_valid(&properties->session_properties);
    valid = valid && network_matchmaking_session_preferences_valid(&properties->preferences);
    return valid;
}

// 0x422ff8
bool network_matchmaking_search_party_properties_decode(c_bitstream *packet, s_matchmaking_search_party_properties *properties)
{
    bool success = true;
    properties->seconds = packet->read_integer("search-seconds", 10);
    properties->latency = packet->read_integer("search-latency", 13);
    success = success && network_matchmaking_search_party_preferences_decode(packet, &properties->preferences);
    success = success && network_matchmaking_session_properties_decode(packet, &properties->session_properties);
    success = success && logic_matchmaking_desirability_search_party_properties_valid(properties);
    return success;
}


#define ABS(x) ((x) >= 0 ? (x) : -(x))



extern "C" {
}

static __forceinline long logic_matchmaking_desirability_hopper_type(const c_hopper_configuration *hopper) { return hopper->m_configuration.hopper_type; }

static __forceinline bool logic_matchmaking_desirability_hopper_is_team_based(const c_hopper_configuration *hopper)
{
    return hopper->m_configuration.hopper_type == 2 || hopper->m_configuration.hopper_type == 3;
}

static __forceinline bool logic_matchmaking_desirability_hopper_is_ranked(const c_hopper_configuration *hopper)
{
    return hopper->m_configuration.hopper_type == 1 || hopper->m_configuration.hopper_type == 3;
}

static __forceinline bool logic_matchmaking_desirability_hopper_is_unranked(const c_hopper_configuration *hopper)
{
    bool result = false;
    switch (logic_matchmaking_desirability_hopper_type(hopper)) {
    case 1:
    case 3:
        result = false;
        break;
    case 0:
    case 2:
        result = true;
        break;
    }
    return result;
}

static __forceinline bool logic_matchmaking_desirability_hopper_big_squad_restrictions(const c_hopper_configuration *hopper)
{
    bool result = false;
    if (logic_matchmaking_desirability_hopper_type(hopper) == 3)
        result = hopper->m_configuration.team_configuration.configuration.ranked.enable_big_squad_mixed_skill_restrictions;
    return result;
}

static __forceinline long logic_matchmaking_desirability_hopper_maximum_team_imbalance(const c_hopper_configuration *hopper)
{
    long result = 0;
    switch (logic_matchmaking_desirability_hopper_type(hopper)) {
    case 2:
        if (hopper->m_configuration.team_configuration.configuration.unranked.allow_uneven_teams)
            result = 15;
        break;
    case 3:
        result = hopper->m_configuration.team_configuration.configuration.ranked.maximum_team_imbalance;
        break;
    }
    return result;
}

static __forceinline long logic_matchmaking_desirability_hopper_team_count(const c_hopper_configuration *hopper)
{
    long result = 0;
    switch (logic_matchmaking_desirability_hopper_type(hopper)) {
    case 0:
    case 1:
        result = hopper->m_configuration.team_configuration.minimum_team_size;
        break;
    case 2:
    case 3:
        result = hopper->m_configuration.team_configuration.team_count;
        break;
    }
    return result;
}

static __forceinline long logic_matchmaking_desirability_hopper_maximum_team_size(const c_hopper_configuration *hopper)
{
    long result = 1;
    long hopper_type = logic_matchmaking_desirability_hopper_type(hopper);
    if (hopper_type == 2 || hopper_type == 3)
        result = hopper->m_configuration.team_configuration.maximum_team_size;
    return result;
}



static __forceinline long logic_matchmaking_desirability_maximum_compatible_nat_type(long nat_type)
{
    switch (nat_type) {
    case 1:
        return 3;
    case 2:
        return 2;
    case 0:
    case 3:
        return 1;
    default:
        return 1;
    }
}


// @address 0x4231f8 ?network_matchmaking_compute_session_desirability@@YA?AW4e_matchmaking_desirability_result@@PBUs_matchmaking_gather_party_properties@@PBUs_matchmaking_search_party_properties@@PAJ@Z
e_matchmaking_desirability_result network_matchmaking_compute_session_desirability(
    const s_matchmaking_gather_party_properties *gather,
    const s_matchmaking_search_party_properties *search, long *desirability_out)
{
    e_matchmaking_desirability_result result = _matchmaking_desirability_desirable;
    long score = 0;
    long search_count = search->session_properties.player_count;
    const c_hopper_configuration *hopper;
    long total_count;
    if (!(gather->session_properties.acceptable_party_size_mask & (1 << (search_count - 1))))
        result = _matchmaking_desirability_unavailable;

    if (result == _matchmaking_desirability_desirable) {
        if (gather->preferences.type == 1 &&
            (search->session_properties.maximum_skill_level < gather->preferences.minimum ||
             search->session_properties.minimum_skill_level > gather->preferences.maximum))
            result = _matchmaking_desirability_incompatible;
        if (search->preferences.type == 1 &&
            (gather->session_properties.maximum_skill_level < search->preferences.minimum ||
             gather->session_properties.minimum_skill_level > search->preferences.maximum))
            result = _matchmaking_desirability_incompatible;
        if ((gather->preferences.type == 2 || search->preferences.type == 2) &&
            search->latency > g_network_configuration.maximum_latency)
            result = _matchmaking_desirability_incompatible;
        if ((gather->preferences.type == 3 || search->preferences.type == 3) &&
            gather->session_properties.region != search->session_properties.region)
            result = _matchmaking_desirability_incompatible;
    }
    if (result == _matchmaking_desirability_desirable) {
        hopper = (*(c_hopper_configuration * *)&g_multiplayer_game_hopper_globals);
        long minimum_count = hopper->get_minimum_player_count();
        long gather_count = gather->session_properties.player_count;
        total_count = gather_count + search_count;
        long maximum_count = hopper->get_maximum_player_count();
        if (total_count > maximum_count)
            result = _matchmaking_desirability_unavailable;
        else {
            score = g_network_configuration.matchmaking_desirability.base_desirability +
                g_network_configuration.matchmaking_desirability.desirability_per_player * total_count;
            if (gather_count < minimum_count && total_count >= minimum_count)
                score += g_network_configuration.matchmaking_desirability.minimum_player_count_reached_desirability;
        }
    }
    if (result == _matchmaking_desirability_desirable) {
        long high = (gather->session_properties.maximum_skill_level > search->session_properties.maximum_skill_level ?
            search->session_properties.maximum_skill_level : gather->session_properties.maximum_skill_level);
        long low = (gather->session_properties.minimum_skill_level > search->session_properties.minimum_skill_level ?
            gather->session_properties.minimum_skill_level : search->session_properties.minimum_skill_level);
        long allowed_low = PIN(low - multiplayer_game_hopper_compute_skill_match_delta(low),
            logic_matchmaking_desirability_skill_level_bounds[0], logic_matchmaking_desirability_skill_level_bounds[1]);
        long allowed_high = PIN(high + multiplayer_game_hopper_compute_skill_match_delta(high),
            logic_matchmaking_desirability_skill_level_bounds[0], logic_matchmaking_desirability_skill_level_bounds[1]);
        if (high < allowed_low || low > allowed_high)
            result = _matchmaking_desirability_incompatible;
    }
    if (result == _matchmaking_desirability_desirable && logic_matchmaking_desirability_hopper_big_squad_restrictions(hopper) &&
        ((gather->session_properties.composition_flags & (1 << 2)) != 0) != ((search->session_properties.composition_flags & (1 << 2)) != 0))
        result = _matchmaking_desirability_unavailable;
    if (result == _matchmaking_desirability_desirable && gather->session_properties.worst_nat_type >
        logic_matchmaking_desirability_maximum_compatible_nat_type(search->session_properties.nat_type))
        result = _matchmaking_desirability_incompatible;

    if (result == _matchmaking_desirability_desirable && logic_matchmaking_desirability_hopper_is_team_based(hopper)) {
        s_network_team_picker_player players[16];
        long count = 0;
        long highest_party = 0;
        memset(players, 0, sizeof(players));
        for (long i = 0; i < 16; ++i) {
            if (((gather->session_properties.player_valid_flags[i >> 5] & (1 << (i & 31))) != 0)) {
                const s_matchmaking_session_player_properties *player = &gather->session_properties.players[i];
                players[count].skill_level = 1;
                players[count].squad_index = player->party_index;
                players[count].peer_index = player->peer_index;
                highest_party = (highest_party > player->party_index ? highest_party : player->party_index);
                ++count;
            }
        }
        long party_base = highest_party + 1;
        for (long i = 0; i < 16; ++i) {
            if (((search->session_properties.player_valid_flags[i >> 5] & (1 << (i & 31))) != 0)) {
                const s_matchmaking_session_player_properties *player = &search->session_properties.players[i];
                players[count].skill_level = 1;
                players[count].squad_index = party_base + player->party_index;
                players[count].peer_index = player->peer_index;
                ++count;
            }
        }
        if (!network_team_picker_pick_teams(logic_matchmaking_desirability_hopper_team_count(hopper), 0,
            logic_matchmaking_desirability_hopper_maximum_team_size(hopper), logic_matchmaking_desirability_hopper_maximum_team_imbalance(hopper),
            !logic_matchmaking_desirability_hopper_is_unranked(hopper), false, false, false, total_count, players, NULL, NULL))
            result = _matchmaking_desirability_unavailable;
    }
    if (result == _matchmaking_desirability_desirable) {
        if (gather->session_properties.preferred_party_size_mask & (1 << (search->session_properties.player_count - 1)))
            score += g_network_configuration.matchmaking_desirability.preferred_party_size_desirability;
        if (gather->gather_seconds >= g_network_configuration.matchmaking_desirability.gather_seconds_threshold)
            score += g_network_configuration.matchmaking_desirability.gather_seconds_desirability;
        if (search->seconds >= g_network_configuration.matchmaking_desirability.search_seconds_threshold)
            score += g_network_configuration.matchmaking_desirability.search_seconds_desirability;
        long skill_difference = ABS(gather->session_properties.average_skill_level - search->session_properties.average_skill_level);
        if (skill_difference <= g_network_configuration.matchmaking_desirability.skill_difference_maximum)
            score += (g_network_configuration.matchmaking_desirability.skill_difference_maximum - skill_difference) *
                g_network_configuration.matchmaking_desirability.skill_difference_desirability;
        float experience_difference = fabsf(gather->session_properties.average_mu - search->session_properties.average_mu);
        if (!(experience_difference > g_network_configuration.matchmaking_desirability.experience_difference_maximum))
            score += (g_network_configuration.matchmaking_desirability.experience_difference_maximum - experience_difference) *
                g_network_configuration.matchmaking_desirability.experience_difference_desirability;
        long rank_difference = ABS(gather->session_properties.average_experience - search->session_properties.average_experience);
        if (rank_difference <= g_network_configuration.matchmaking_desirability.rank_difference_maximum)
            score += (g_network_configuration.matchmaking_desirability.rank_difference_maximum - rank_difference) *
                g_network_configuration.matchmaking_desirability.rank_difference_desirability;
        if (gather->session_properties.guest_count + search->session_properties.guest_count == 1)
            score += g_network_configuration.matchmaking_desirability.single_guest_desirability;
        if (gather->session_properties.region == search->session_properties.region)
            score += g_network_configuration.matchmaking_desirability.region_desirability;
        score += g_network_configuration.matchmaking_desirability.country_desirability;
        if (!logic_matchmaking_desirability_hopper_is_ranked(hopper) && gather->session_properties.language == search->session_properties.language)
            score += g_network_configuration.matchmaking_desirability.language_desirability;
        if (!logic_matchmaking_desirability_hopper_is_ranked(hopper) &&
            ((gather->session_properties.composition_flags & (1 << 0)) != 0) ==
                ((search->session_properties.composition_flags & (1 << 0)) != 0))
            score += g_network_configuration.matchmaking_desirability.composition_flag_desirability;
        if (search->latency <= g_network_configuration.matchmaking_desirability.latency_maximum)
            score += g_network_configuration.matchmaking_desirability.latency_maximum -
                search->latency * g_network_configuration.matchmaking_desirability.latency_scale /
                    (float)g_network_configuration.matchmaking_desirability.latency_divisor;
        *desirability_out = score;
    }
    return result;
}
