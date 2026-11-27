// networkinglibxbox/logic_matchmaking_composition.obj: from the records 0x43ce98 0x43ceb8 0x43d108 0x43d2a0 0x43d4e0 0x43e110 0x43e240 0x43e648 0x43ec80 0x57ae80
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x43ce98 ?get_big_squad_size_threshold@c_hopper_configuration@@QBAJXZ
#include <string.h>
#include "game/multiplayer_game_hopper.h"
#include "networking/logic/logic_matchmaking_composition_types.h"
#include "networking/network_team_picker.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/logic_matchmaking_composition.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "game/multiplayer_game_hopper_globals.h"
#include "game/hopper_configuration.h"

#include "networking/logic/logic_matchmaking_composition_data_types.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/network_game_definitions.h"
#include "cseries/maximum.h"
#include "cseries/minimum.h"
struct s_logic_matchmaking_composition_skill_limits { long minimum, maximum; };

#include "networking/network_team_picker_player_types.h"
struct c_logic_matchmaking_composition_team_players {
    s_network_team_picker_player elements[16];
    __forceinline s_network_team_picker_player &operator[](long index) { return elements[index]; }
    __forceinline s_network_team_picker_player *begin() { return elements; }
};

// @address 0x43d108 network_session_matchmaking_composition_build_player_party_indices
extern "C" long network_session_matchmaking_composition_build_player_party_indices(const c_network_session_membership *membership,
    const c_hopper_configuration *hopper, s_network_session_matchmaking_composition *composition)
{
 long result=_network_session_matchmaking_composition_build_result_success;
 unsigned __int64 party_ids[16];
 for (long i=0;i<16;i++) party_ids[i]=0;
 if (membership->session_type()==1) composition->party_count=1;
 for (long player_index=0;player_index<16;player_index++) {
  if (((composition->session_properties.player_valid_flags[player_index >> 5] & (1 << (player_index & 31))) != 0)) {
   s_matchmaking_session_player_properties *properties_player=&composition->session_properties.players[player_index];
   const s_network_session_player *player=membership->get_player(player_index);
   const s_network_session_peer *peer=membership->get_peer(player->peer_index);
   properties_player->party_index=-1;
   properties_player->peer_index=player->peer_index;
   if (membership->session_type()==1) properties_player->party_index=0;
   else {
    for (long party_index=0;party_index<composition->party_count;party_index++) {
     if (peer->party_nonce==party_ids[party_index]) { properties_player->party_index=party_index; break; }
    }
    if (properties_player->party_index==-1) {
     properties_player->party_index=composition->party_count;
     party_ids[composition->party_count]=peer->party_nonce;
     composition->party_count++;
    }
   }
   s_network_session_party_matchmaking_composition *party=&composition->parties[properties_player->party_index];
   party->player_count++;
  }
 }
 for (long party_index=0;party_index<composition->party_count;party_index++) {
  s_network_session_party_matchmaking_composition *party=&composition->parties[party_index];
  if (party->player_count<hopper->get_minimum_party_size() || party->player_count>hopper->get_maximum_party_size()) {
   result=_network_session_matchmaking_composition_build_result_invalid_party_composition;
   break;
  }
 }
 return result;
}
/* Counts the selected units whose state is 9 with either of the 0xc0 flags set, per
   unit and per team; returns 10 when any counted and the option byte is clear, else 18. */


// @address 0x43d2a0 network_session_matchmaking_composition_build_guest_statistics
extern "C" long network_session_matchmaking_composition_build_guest_statistics(
    const c_network_session_membership *membership, const c_hopper_configuration *hopper, s_network_session_matchmaking_composition *composition)
{
    long result = _network_session_matchmaking_composition_build_result_success;
    for (long i = 0; i < 16; i++) {
        s_matchmaking_session_player_properties *player = &composition->session_properties.players[i];
        if ((1 << (i & 31)) & composition->session_properties.player_valid_flags[i >> 5]) {
            unsigned short state = (unsigned short)(membership->get_player(i)->configuration.client.xuid >> 48);
            bool counted = (state & 15) == 9 && (state & 0xc0) != 0;
            if (counted) {
                composition->guest_player_count++;
                composition->parties[player->party_index].guest_player_count++;
            }
        }
    }
    if (composition->guest_player_count > 0 && !hopper->m_configuration.runtime_guests_allowed)
        result = _network_session_matchmaking_composition_build_result_guests_not_allowed;
    return result;
}

// @address 0x43e110 ?network_session_matchmaking_composition_build_flags@@YA?AW4e_network_session_matchmaking_composition_build_result@@PBVc_network_session_membership@@PBVc_hopper_configuration@@PAUs_network_session_matchmaking_composition@@@Z
e_network_session_matchmaking_composition_build_result network_session_matchmaking_composition_build_flags(
    const c_network_session_membership *membership, const c_hopper_configuration *hopper,
    s_network_session_matchmaking_composition *composition)
{
    long player_index = 0;
    do {
        if (((composition->session_properties.player_valid_flags[player_index >> 5] & (1 << (player_index & 31))) != 0)) {
            const s_matchmaking_session_player_properties *properties_player = &composition->session_properties.players[player_index];
            const s_network_session_player_matchmaking_composition *player_composition = &composition->player_compositions[player_index];
            s_network_session_party_matchmaking_composition *party = &composition->parties[properties_player->party_index];
            if (player_composition->adjusted_skill_level != player_composition->hopper_skill) {
                composition->session_properties.composition_flags |= 2;
                party->flags |= 2;
            }
            if (party->player_count <= hopper->m_configuration.quitter_filter_maximum_party_size) {
                const s_network_session_player *player = membership->get_player(player_index);
                long percentage = 0;
                if (player->configuration.client.hopper_statistics.games_played > 0)
                    percentage = (long)(100.0f -
                        (float)player->configuration.client.hopper_statistics.games_completed /
                            (float)player->configuration.client.hopper_statistics.games_played * 100.0f);
                if (percentage > hopper->m_configuration.quitter_filter_percentage) {
                    composition->session_properties.composition_flags |= 1;
                    party->flags |= 1;
                }
            }
        }
        player_index++;
    } while (player_index < 16);
    return _network_session_matchmaking_composition_build_result_success;
}

// @address 0x43e240 ?network_session_matchmaking_composition_session_state_ready@@YA_NPBVc_network_session_membership@@PBVc_hopper_configuration@@PAW4e_network_session_matchmaking_composition_build_result@@@Z
bool network_session_matchmaking_composition_session_state_ready(const c_network_session_membership *membership,
    const c_hopper_configuration *hopper, e_network_session_matchmaking_composition_build_result *build_result)
{
    bool ready = true;
    bool local_player_unready = false;
    e_network_session_matchmaking_composition_build_result result = _network_session_matchmaking_composition_build_result_none;
    long local_peer_index = membership->local_peer_index();
    for (long player_index = 0; player_index < 16; player_index++) {
        if (membership->is_player_valid(player_index)) {
            const s_network_session_player *player = membership->get_player(player_index);
            bool player_ready = true;
            if (!player->configuration.client.global_statistics.valid || !player->configuration.client.hopper_statistics.valid ||
                !player->configuration.host.global_stats_valid || !player->configuration.host.hopper_stats_valid)
                player_ready = false;
            if (!player_ready) {
                result = _network_session_matchmaking_composition_build_result_player_not_ready;
                ready = false;
            }
            if (player->configuration.client.player_flags & 2) {
                result = _network_session_matchmaking_composition_build_result_player_busy;
                ready = false;
            }
            if (player->peer_index == local_peer_index && player->controller_index != -1) {
                bool in_state = ((unsigned short)(player->configuration.client.xuid >> 48) & 15) == 9 &&
                    ((unsigned short)(player->configuration.client.xuid >> 48) & 0xc0) != 0;
                if (!in_state)
                    local_player_unready = true;
            }
        }
        if (!ready)
            break;
    }
    if (ready && !local_player_unready) {
        result = _network_session_matchmaking_composition_build_result_no_local_player_in_state;
        ready = false;
    }
    if (!ready)
        *build_result = result;
    return ready;
}

extern "C" {
}
namespace {
static __forceinline bool player_in_matchmaking_state(const s_network_session_player *player)
{
    bool in_state = ((unsigned short)(player->configuration.client.xuid >> 48) & 15) == 9 &&
        ((unsigned short)(player->configuration.client.xuid >> 48) & 0xc0) != 0;
    return in_state;
}
}

// @address 0x43ec80 network_matchmaking_composition_build_local_properties
extern "C" long network_matchmaking_composition_build_local_properties(
    const c_network_session_membership *membership, s_network_session_matchmaking_composition *composition)
{
    long result = _network_session_matchmaking_composition_build_result_success;
    bool found_local_player = false;
    long local_peer_index = membership->local_peer_index();
    const s_network_session_peer *local_peer = membership->get_peer(local_peer_index);
    composition->session_properties.region = local_peer->properties.language;
    composition->session_properties.worst_nat_type = local_peer->properties.nat_type;
    for (long player_index = (membership)->get_first_player(); player_index != -1;
        player_index = (membership)->get_next_player(player_index))
    {
        const s_network_session_player *player = membership->get_player(player_index);
        if (player->peer_index == local_peer_index && player->controller_index != -1)
        {
            bool in_state = player_in_matchmaking_state(player);
            if (!in_state)
            {
                found_local_player = true;
                composition->session_properties.country = player->configuration.client.gamer_region;
                composition->session_properties.language = player->configuration.client.gamer_zone;
                break;
            }
        }
    }
    if (!found_local_player)
        result = _network_session_matchmaking_composition_build_result_no_signed_in_local_players;
    return result;
}


// @address 0x57ae80 network_session_matchmaking_composition_session_state_valid
extern "C" int network_session_matchmaking_composition_session_state_valid(void)
{
    int result;
    result = 0;
    if (((s_hopper_update_globals *)&g_multiplayer_game_hopper_globals)->game_set_failed != 0) {
        result = 3;
    } else if (((s_hopper_update_globals *)&g_multiplayer_game_hopper_globals)->game_set_valid != 0) {
        result = 2;
    } else {
        int call_0;
        call_0 = network_http_buffer_downloader_get_public_status(
            ((s_hopper_update_globals *)&g_multiplayer_game_hopper_globals)->game_set_downloader.m_internal_status);
        if (call_0 == 1) {
            result = 1;
        }
    }
    return result;
}

#define LOGIC_MATCHMAKING_COMPOSITION_MAXIMUM(a, b) ((a) > (b) ? (a) : (b))
#define LOGIC_MATCHMAKING_COMPOSITION_MINIMUM(a, b) ((a) < (b) ? (a) : (b))
#define LOGIC_MATCHMAKING_COMPOSITION_LOWER(a, b) ((a) > (b) ? (b) : (a))
static __forceinline long logic_matchmaking_composition_compute_skill_match_delta(long index)
{
    long skill_match_delta = 0;
    if (multiplayer_game_hoppers_get_current_hopper_configuration()) {
        const c_hopper_configuration *configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
        if (configuration->is_ranked())
            skill_match_delta = configuration->get_maximum_skill_level_match_delta(index);
        else
            skill_match_delta = 50;
    }
    return skill_match_delta;
}
// @address 0x258ac k_matchmaking_skill_limits
// @rdata 0x820258ac..0x820258b4
extern "C" {
const s_logic_matchmaking_composition_skill_limits k_matchmaking_skill_limits = { 1, 50 };
}
// @rdata-end
// match: 0x43d4e0: without it 8 fadds emit fr0,fr0,fr13 instead of fr0,fr13,fr0
#pragma float_control(precise,on)
// @address 0x43d4e0 ?network_session_matchmaking_composition_build_player_statistics@@YA?AW4e_network_session_matchmaking_composition_build_result@@PBVc_network_session_membership@@PBVc_hopper_configuration@@PAUs_network_session_matchmaking_composition@@@Z
e_network_session_matchmaking_composition_build_result network_session_matchmaking_composition_build_player_statistics(
    const c_network_session_membership *membership, const c_hopper_configuration *hopper,
    s_network_session_matchmaking_composition *composition)
{
    e_network_session_matchmaking_composition_build_result result = _network_session_matchmaking_composition_build_result_success;
    for (long player_index = 0; player_index < 16; player_index++) {
        if (((composition->session_properties.player_valid_flags[player_index >> 5] & (1 << (player_index & 31))) != 0)) {
            s_network_session_player_matchmaking_composition *player_composition = &composition->player_compositions[player_index];
            s_network_session_party_matchmaking_composition *party = &composition->parties[composition->session_properties.players[player_index].party_index];
            const s_network_session_player *player = membership->get_player(player_index);
            const s_player_configuration_from_host *skill_profile = &player->configuration.host;
            player_composition->hopper_skill = skill_profile->hopper_skill;
            player_composition->experience_rank = skill_profile->global_rank;
            player_composition->mu = player->configuration.client.hopper_statistics.mu;
            composition->session_properties.minimum_skill_level = LOGIC_MATCHMAKING_COMPOSITION_MAXIMUM(
                1, LOGIC_MATCHMAKING_COMPOSITION_MAXIMUM(composition->session_properties.minimum_skill_level, player_composition->hopper_skill));
            party->maximum_skill_level = LOGIC_MATCHMAKING_COMPOSITION_MAXIMUM(1, 
                LOGIC_MATCHMAKING_COMPOSITION_MAXIMUM(party->maximum_skill_level, player_composition->hopper_skill));
        }
    }
    composition->session_properties.maximum_skill_level = 50;
    for (long party_index = 0; party_index < composition->party_count; party_index++) {
        s_network_session_party_matchmaking_composition *party = &composition->parties[party_index];
        long skill = party->maximum_skill_level;
        long skill_delta = logic_matchmaking_composition_compute_skill_match_delta(skill);
        // match: 0x43d4e0: value range assumption guides branch lowering
        __assume(k_matchmaking_skill_limits.minimum == 1);
    // match: 0x43d4e0: value range assumption guides branch lowering
    __assume(k_matchmaking_skill_limits.maximum == 50);
        long lower_skill = minimum(
            maximum(skill - skill_delta, k_matchmaking_skill_limits.minimum),
            k_matchmaking_skill_limits.maximum);
        party->minimum_adjusted_skill_level = 50;
        for (long player_index = 0; player_index < 16; player_index++) {
            if (((composition->session_properties.player_valid_flags[player_index >> 5] & (1 << (player_index & 31))) != 0)
                && composition->session_properties.players[player_index].party_index == party_index) {
                s_network_session_player_matchmaking_composition *player_composition = &composition->player_compositions[player_index];
                player_composition->adjusted_skill_level = LOGIC_MATCHMAKING_COMPOSITION_MAXIMUM(player_composition->hopper_skill, lower_skill);
                composition->session_properties.maximum_skill_level = LOGIC_MATCHMAKING_COMPOSITION_LOWER(
                    composition->session_properties.maximum_skill_level, player_composition->adjusted_skill_level);
                party->minimum_adjusted_skill_level = LOGIC_MATCHMAKING_COMPOSITION_LOWER(
                    party->minimum_adjusted_skill_level, player_composition->adjusted_skill_level);
            }
        }
    }
    for (long player_index = 0; player_index < 16; player_index++) {
        if (((composition->session_properties.player_valid_flags[player_index >> 5] & (1 << (player_index & 31))) != 0)) {
            const s_network_session_player_matchmaking_composition *player_composition = &composition->player_compositions[player_index];
            s_network_session_party_matchmaking_composition *party = &composition->parties[composition->session_properties.players[player_index].party_index];
            composition->session_properties.average_experience += player_composition->experience_rank;
            float total_weight = composition->session_properties.average_mu;
            total_weight += player_composition->mu;
            composition->session_properties.average_mu = total_weight;
            composition->session_properties.average_skill_level += player_composition->adjusted_skill_level;
            party->average_experience_rank += player_composition->experience_rank;
            party->average_mu += player_composition->mu;
            party->average_adjusted_skill_level += player_composition->adjusted_skill_level;
        }
    }
    composition->session_properties.average_experience /= composition->session_properties.player_count;
    composition->session_properties.average_skill_level /= composition->session_properties.player_count;
    composition->session_properties.average_mu /= (float)composition->session_properties.player_count;
    for (long party_index = 0; party_index < composition->party_count; party_index++) {
        s_network_session_party_matchmaking_composition *party = &composition->parties[party_index];
        party->average_adjusted_skill_level /= party->player_count;
        party->average_experience_rank /= party->player_count;
        party->average_mu /= (float)party->player_count;
    }
    long minimum_limit = LOGIC_MATCHMAKING_COMPOSITION_MINIMUM(
        LOGIC_MATCHMAKING_COMPOSITION_MAXIMUM(
            composition->session_properties.minimum_skill_level
                - logic_matchmaking_composition_compute_skill_match_delta(composition->session_properties.minimum_skill_level), 1),
        50);
    long maximum_limit = LOGIC_MATCHMAKING_COMPOSITION_MINIMUM(
        LOGIC_MATCHMAKING_COMPOSITION_MAXIMUM(
            composition->session_properties.maximum_skill_level
                + logic_matchmaking_composition_compute_skill_match_delta(composition->session_properties.maximum_skill_level), 1),
        50);
    if (composition->session_properties.maximum_skill_level < minimum_limit || composition->session_properties.minimum_skill_level > maximum_limit)
        result = _network_session_matchmaking_composition_build_result_skill_spread_too_large;
    return result;
}

static __forceinline void logic_matchmaking_composition_initialize_team_player(
    s_network_team_picker_player *team_player, const s_matchmaking_session_player_properties *player)
{
    long squad_index = player->party_index;
    long peer_index = player->peer_index;
    team_player->skill_level = 1;
    team_player->peer_index = peer_index;
    team_player->squad_index = squad_index;
}
// match: 0x43e648: these are c_hopper_configuration::get_maximum_team_imbalance, get_team_count and get_maximum_team_size
// (life_cycle_manager.cpp). Called by name, they share their setup and the argument
// moves before network_team_picker_can_add_whole_squad (image check, 2026-11-14).
static __forceinline long logic_matchmaking_composition_team_count(const c_hopper_configuration *hopper)
{
    long result = 0;
    switch (hopper->get_hopper_type()) {
    case 0: case 1: result = hopper->m_configuration.team_configuration.minimum_team_size; break;
    case 2: case 3: result = hopper->m_configuration.team_configuration.team_count; break;
    }
    return result;
}
static __forceinline long logic_matchmaking_composition_team_size(const c_hopper_configuration *hopper)
{
    long result = 1;
    switch (hopper->get_hopper_type()) {
    case 2: result = hopper->m_configuration.team_configuration.maximum_team_size; break;
    case 3: result = hopper->m_configuration.team_configuration.maximum_team_size; break;
    }
    return result;
}
static __forceinline long logic_matchmaking_composition_team_imbalance(const c_hopper_configuration *hopper)
{
    long result = 0;
    switch (hopper->get_hopper_type()) {
    case 2: if (hopper->m_configuration.team_configuration.configuration.unranked.allow_uneven_teams) result = 15; break;
    case 3: result = hopper->m_configuration.team_configuration.configuration.ranked.maximum_team_imbalance; break;
    }
    return result;
}
static __forceinline long logic_matchmaking_composition_big_squad_imbalance(const c_hopper_configuration *hopper)
{
    long result = 0;
    if (hopper->get_hopper_type() == 3) result = hopper->m_configuration.team_configuration.configuration.ranked.maximum_big_squad_imbalance;
    return result;
}
static __forceinline bool logic_matchmaking_composition_big_squad_mixed_skill(const c_hopper_configuration *hopper)
{
    bool result = false;
    if (hopper->get_hopper_type() == 3) result = hopper->m_configuration.team_configuration.configuration.ranked.enable_big_squad_mixed_skill_restrictions;
    return result;
}
static __forceinline bool logic_matchmaking_composition_has_teams(const c_hopper_configuration *hopper)
{
    return hopper->get_hopper_type() == 2 || hopper->get_hopper_type() == 3;
}
static __forceinline bool logic_matchmaking_composition_ranked(const c_hopper_configuration *hopper)
{
    return hopper->get_hopper_type() == 1 || hopper->get_hopper_type() == 3;
}
static __forceinline bool logic_matchmaking_composition_teams_equal(const c_hopper_configuration *hopper)
{
    bool result = false;
    switch (hopper->get_hopper_type()) {
    case 1: case 3: result = false; break;
    case 0: case 2: result = true; break;
    }
    return result;
}
static __forceinline long logic_matchmaking_composition_multiply(long a, long b)
{
    return a * b;
}
static __forceinline long logic_matchmaking_composition_volatile_players(const c_hopper_configuration *hopper)
{
    return *(const long *)&hopper->m_configuration.team_configuration.team_count;
}
static __forceinline const c_hopper_configuration *logic_matchmaking_composition_hopper_alias(const c_hopper_configuration *hopper)
{
    return hopper;
}
static __forceinline long logic_matchmaking_composition_maximum_players(const c_hopper_configuration *hopper)
{
    long result = 0;
    switch (hopper->get_hopper_type()) {
    case 0: case 1: result = hopper->m_configuration.team_configuration.minimum_team_size; if (result >= 16) result = 16; break;
    case 2:
    result = hopper->m_configuration.team_configuration.team_count * hopper->m_configuration.team_configuration.maximum_team_size;
    if (result >= 16)
        result = 16;
    break;
    case 3:
    result = hopper->m_configuration.team_configuration.team_count * hopper->m_configuration.team_configuration.maximum_team_size;
    if (result >= 16)
        result = 16;
    break;
    }
    return result;
}
static __forceinline long logic_matchmaking_composition_big_squad_threshold(const c_hopper_configuration *hopper)
{
    long result = 16;
    if (hopper->get_hopper_type() == 3) result = hopper->m_configuration.team_configuration.configuration.ranked.big_squad_size_threshold;
    return result;
}
static __forceinline void logic_matchmaking_composition_add_party_size(c_flags_no_init<long, unsigned long, 32> *mask, long party_size)
{
    mask->set((party_size ? party_size : 0) - 1, true);
}
// @address 0x43e648 ?network_session_matchmaking_composition_build_party_masks@@YA?AW4e_network_session_matchmaking_composition_build_result@@PBVc_network_session_membership@@PBVc_hopper_configuration@@PAUs_network_session_matchmaking_composition@@@Z
e_network_session_matchmaking_composition_build_result network_session_matchmaking_composition_build_party_masks(
    const c_network_session_membership *membership, const c_hopper_configuration *hopper,
    s_network_session_matchmaking_composition *composition)
{
    e_network_session_matchmaking_composition_build_result result = _network_session_matchmaking_composition_build_result_success;
    bool big_squads_only = false;
    if (hopper->get_hopper_type() == 3 && hopper->m_configuration.team_configuration.configuration.ranked.big_squad_size_threshold != 16) {
        long maximum_big_squad_size = 0;
        long minimum_big_squad_size = 16;
        for (long party_index = 0; party_index < composition->party_count; party_index++) {
            const s_network_session_party_matchmaking_composition *party = &composition->parties[party_index];
            if (party->player_count >= hopper->get_big_squad_size_threshold()) {
                composition->big_squad_count++;
                maximum_big_squad_size = maximum_big_squad_size > party->player_count ? maximum_big_squad_size : party->player_count;
                minimum_big_squad_size = minimum_big_squad_size <= party->player_count ? minimum_big_squad_size : party->player_count;
            }
        }
        composition->big_squad_size_imbalance = maximum_big_squad_size - minimum_big_squad_size;
        if (composition->big_squad_count > 0 && composition->big_squad_count < logic_matchmaking_composition_team_count(hopper)) {
            long maximum_imbalance = logic_matchmaking_composition_big_squad_imbalance(hopper);
            if (logic_matchmaking_composition_big_squad_mixed_skill(hopper) && composition->session_properties.flags.test(1))
                composition->session_properties.flags.set(2, true);
            for (long party_size = logic_matchmaking_composition_big_squad_threshold(hopper); party_size <= hopper->get_maximum_party_size(); party_size++) {
                if (maximum_big_squad_size - party_size <= maximum_imbalance && party_size - minimum_big_squad_size <= maximum_imbalance)
                    composition->session_properties.preferred_party_sizes.m_storage |= 1 << (party_size - 1);
            }
            big_squads_only = true;
            composition->session_properties.acceptable_party_sizes = composition->session_properties.preferred_party_sizes;
        }
        if (big_squads_only && composition->party_count != composition->big_squad_count)
            result = _network_session_matchmaking_composition_build_result_big_party_no_rabble_yet;
        else if (composition->big_squad_count > logic_matchmaking_composition_team_count(hopper))
            result = _network_session_matchmaking_composition_build_result_big_party_count_exceeds_teams;
        else if (composition->big_squad_size_imbalance > logic_matchmaking_composition_big_squad_imbalance(hopper))
            result = _network_session_matchmaking_composition_build_result_big_party_teams_exceed_imbalance;
    }
    c_logic_matchmaking_composition_team_players players;
    memset(players.begin(), 0, sizeof(players));
    long team_player_count = 0;
    for (long player_index = 0; player_index < 16; player_index++) {
        if (((composition->session_properties.player_valid_flags[player_index >> 5] & (1 << (player_index & 31))) != 0)) {
            const s_matchmaking_session_player_properties *player = &composition->session_properties.players[player_index];
            long squad_index = player->party_index;
            long peer_index = player->peer_index;
            players[team_player_count].skill_level = 1;
            players[team_player_count].squad_index = squad_index;
            players[team_player_count].peer_index = peer_index;
            team_player_count++;
        }
    }
    if (logic_matchmaking_composition_has_teams(hopper)) {
        if (!network_team_picker_pick_teams(logic_matchmaking_composition_team_count(hopper), 0,
            logic_matchmaking_composition_team_size(hopper), logic_matchmaking_composition_team_imbalance(hopper),
            !logic_matchmaking_composition_teams_equal(hopper), false, false, false,
            composition->session_properties.player_count, players.begin(), &composition->team_imbalance, 0))
        {
            result = _network_session_matchmaking_composition_build_result_cant_pick_teams;
        }
    } else composition->team_imbalance = 0;
    if (result == _network_session_matchmaking_composition_build_result_success && !big_squads_only) {
        long players_needed = composition->team_imbalance <= logic_matchmaking_composition_team_imbalance(hopper)
            ? 0
            : composition->team_imbalance - logic_matchmaking_composition_team_imbalance(hopper);
        long player_space_available = logic_matchmaking_composition_maximum_players(hopper) - composition->session_properties.player_count;
        if (players_needed > player_space_available) {
            result = _network_session_matchmaking_composition_build_result_cant_even_out_teams;
        }
        else if (logic_matchmaking_composition_ranked(hopper)) {
            if (logic_matchmaking_composition_has_teams(hopper)) {
                long party_size = hopper->get_minimum_party_size();
                long maximum_party_size = hopper->get_maximum_party_size();
                if (hopper->get_hopper_type() == 3
                    && hopper->m_configuration.team_configuration.configuration.ranked.big_squad_size_threshold != 16)
                    maximum_party_size = hopper->m_configuration.team_configuration.configuration.ranked.big_squad_size_threshold - 1;
                for (; party_size <= maximum_party_size; party_size++) {
                    if (party_size <= player_space_available) {
                        bool can_add = network_team_picker_can_add_whole_squad(party_size, hopper->get_team_count(), hopper->get_maximum_team_size(),
                            hopper->get_maximum_team_imbalance(), composition->session_properties.player_count, players.begin());
                        if (can_add) logic_matchmaking_composition_add_party_size(&composition->session_properties.acceptable_party_sizes, party_size);
                    }
                }
            } else if (player_space_available > 0) composition->session_properties.acceptable_party_sizes.set_unsafe(1);
        } else composition->session_properties.acceptable_party_sizes.set_unsafe((1 << player_space_available) - 1);
    }
    return result;
}
inline long c_hopper_configuration::get_big_squad_size_threshold() const
{
    long threshold = 16;
    if (get_hopper_type() == 3)
        threshold = m_configuration.team_configuration.configuration.ranked.big_squad_size_threshold;
    return threshold;
}




static __forceinline bool logic_matchmaking_composition_target_session_valid(
    const c_network_session *session, e_network_session_matchmaking_composition_build_result *result_out)
{
    bool valid = true;
    const c_hopper_configuration *hopper_configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
    e_network_session_matchmaking_composition_build_result result = _network_session_matchmaking_composition_build_result_invalid_session;
    if (valid)
    {
        valid = hopper_configuration != 0;
        if (!valid) result = _network_session_matchmaking_composition_build_result_no_hopper;
    }
    if (valid)
    {
        const s_network_session_matchmaking_hopper *hopper = session->get_session_parameters()->m_parameters.matchmaking_hopper.get();
        bool hopper_matches;
        if (hopper && hopper->hopper_identifier == hopper_configuration->get_hopper_identifier()) hopper_matches = true;
        else hopper_matches = false;
        valid = hopper_matches;
        if (!valid) result = _network_session_matchmaking_composition_build_result_hopper_mismatch;
    }
    if (valid)
    {
        long load_status = network_session_matchmaking_composition_session_state_valid();
        valid = load_status == 2;
        if (!valid) result = _network_session_matchmaking_composition_build_result_hoppers_not_loaded;
    }
    if (!valid) *result_out = result;
    return valid;
}

static __forceinline e_network_session_matchmaking_composition_build_result logic_matchmaking_composition_target_build_players(
    const c_network_session_membership *membership, const c_hopper_configuration *hopper_configuration,
    s_network_session_matchmaking_composition *composition)
{
    e_network_session_matchmaking_composition_build_result result = _network_session_matchmaking_composition_build_result_success;
    composition->session_properties.player_count = membership->get_player_count();
    composition->session_properties.player_valid_flags[0] = membership->get_player_valid_mask();
    if (composition->session_properties.player_count > hopper_configuration->get_maximum_player_count())
        result = _network_session_matchmaking_composition_build_result_too_many_players;
    if (result == _network_session_matchmaking_composition_build_result_success)
        result = (e_network_session_matchmaking_composition_build_result)network_session_matchmaking_composition_build_player_party_indices(
            membership, hopper_configuration, composition);
    if (result == _network_session_matchmaking_composition_build_result_success)
        result = (e_network_session_matchmaking_composition_build_result)network_session_matchmaking_composition_build_guest_statistics(
            membership, hopper_configuration, composition);
    if (result == _network_session_matchmaking_composition_build_result_success)
        result = network_session_matchmaking_composition_build_player_statistics(membership, hopper_configuration, composition);
    if (result == _network_session_matchmaking_composition_build_result_success)
        result = network_session_matchmaking_composition_build_flags(membership, hopper_configuration, composition);
    return result;
}

static __forceinline e_network_session_matchmaking_composition_build_result logic_matchmaking_composition_target_build_peers(
    const c_network_session_membership *membership, s_network_session_matchmaking_composition *composition)
{
    long maximum_nat_type = 0;
    for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
        peer_index = (membership)->get_next_peer(peer_index))
    {
        const s_network_session_peer *peer = membership->get_peer(peer_index);
        maximum_nat_type = maximum_nat_type > peer->properties.nat_type ? maximum_nat_type : peer->properties.nat_type;
    }
    composition->session_properties.maximum_peer_nat_type = maximum_nat_type;
    matchmaking_calculate_best_possible_host(membership, 0, &composition->host_bandwidth, &composition->host_capacity, 0,
        &composition->session_properties.good_host_count);
    return _network_session_matchmaking_composition_build_result_success;
}

// @address 0x43ceb8 ?network_session_build_matchmaking_composition@@YA?AW4e_network_session_matchmaking_composition_build_result@@PBVc_network_session@@PAUs_network_session_matchmaking_composition@@@Z
e_network_session_matchmaking_composition_build_result network_session_build_matchmaking_composition(
    const c_network_session *session, s_network_session_matchmaking_composition *composition)
{
    e_network_session_matchmaking_composition_build_result result = _network_session_matchmaking_composition_build_result_none;
    const c_hopper_configuration *hopper_configuration = 0;
    memset(composition, 0, sizeof(s_network_session_matchmaking_composition));
    const c_network_session_membership *membership = session->get_session_membership();
    if (logic_matchmaking_composition_target_session_valid(session, &result))
    {
        hopper_configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
        if (network_session_matchmaking_composition_session_state_ready(membership, multiplayer_game_hoppers_get_current_hopper_configuration(), &result))
            result = _network_session_matchmaking_composition_build_result_success;
    }
    if (result == _network_session_matchmaking_composition_build_result_success)
    {
        composition->session_properties.hopper_identifier = hopper_configuration->get_hopper_identifier();
        composition->session_properties.ranked = hopper_configuration->is_ranked();
        result = logic_matchmaking_composition_target_build_players(membership, hopper_configuration, composition);
        if (result == _network_session_matchmaking_composition_build_result_success)
        {
            result = network_session_matchmaking_composition_build_party_masks(membership, hopper_configuration, composition);
            if (result == _network_session_matchmaking_composition_build_result_success)
            {
                result = logic_matchmaking_composition_target_build_peers(membership, composition);
                if (result == _network_session_matchmaking_composition_build_result_success)
                {
                    result = (e_network_session_matchmaking_composition_build_result)network_matchmaking_composition_build_local_properties(
                        membership, composition);
                    if (result == _network_session_matchmaking_composition_build_result_success)
                        composition->session_properties.flags.set(3, true);
                }
            }
        }
    }
    return result;
}
