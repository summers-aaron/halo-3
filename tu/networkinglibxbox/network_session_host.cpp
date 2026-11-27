// networkinglibxbox/network_session_host.obj: from the records 0x4260c8 0x426b58 0x426d40
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/player_types.h"
#include "memory/tls_globals.h"
#include "math/random_math.h"
#include "cseries/maximum.h"
#include "math/integer_math.h"
#include "math/random_math_range.h"
#include "game/game_state_accessors.h"
// @address 0x426d40 ?does_peer_have_gamestate@c_network_session@@QAA_NJ@Z
#include <string.h>
#include "cseries/cseries.h"
#include "networking/logic/network_bandwidth.h"
#include "networking/session/network_session_membership.h"
#include "networking/network_configuration_types.h"
#include "networking/session/network_session_parameters.h"
#include "networking/simulation/simulation_accessors.h"
#include "networking/session/network_observer.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/session/network_session_find_host_types.h"
#include "networking/transport/transport_unique_identifier_accessors.h"
#include "networking/bandwidth_configuration_host_preference_types.h"
#include "simulation/simulation_view_establishment_mode_types.h"
#include "simulation/simulation_view.h"
#include "simulation/simulation_world_types.h"
#include "networking/network_configuration.h"
#include "networking/session/network_session.h"

class c_network_session;

struct s_game_state;


bool c_network_session::does_peer_have_gamestate(long peer_index)
{
    bool result = false;
    if (game_in_progress() && !game_is_ui_shell())
    {
        c_simulation_world *world = simulation_get_world();
        c_simulation_view *view = 0;
        if (world->is_distributed())
        {
            s_transport_unique_identifier identifier;
            transport_secure_address_extract_identifier(&m_session_membership.get_peer(peer_index)->secure_address, &identifier);
            view = world->get_client_view_by_machine_identifier((const s_machine_identifier *)&identifier);
            if (view && view->get_view_establishment_mode() == _simulation_view_establishment_mode_joined)
                result = true;
            else if (view && view->get_view_establishment_mode() == _simulation_view_establishment_mode_joining
                && view->client_join_is_finished()
                && world->view_has_acknowledged_active_players(view))
                result = true;
        }
    }
    return result;
}

static __forceinline bool network_session_host_peer_connection_ready(const c_network_session_membership *membership, long peer_index)
{
    return membership->m_current_membership.peers[peer_index].connection_state == 8 || membership->m_current_membership.peers[peer_index].connection_state == 1;
}
static __forceinline long network_bandwidth_get_estimated_desired_machine_capacity_of_bandwidth(long bandwidth)
{
    long level = 16;
    do
    {
        if (bandwidth >= g_network_configuration.player_capacity_bandwidth_thresholds[level])
            break;
    } while (--level > 0);
    level = PIN(level, 1, 16);
    return level;
}

static __forceinline unsigned long network_session_host_compute_peer_score(c_network_session *session,
    const s_bandwidth_host_preference_weights *weights, long peer_index, long valid_peer_mask, long maximum_peer_count,
    unsigned long ignored_connectivity_mask, unsigned long latency_tier, unsigned long quality_tier, unsigned long *connected_peer_mask,
    bool *all_peers_connected, unsigned char *peer_connection_count)
{
    const c_network_session_membership *membership = session->get_session_membership();
    const s_bandwidth_configuration *configuration = &g_network_configuration.bandwidth;
    const s_network_session_peer *peer = membership->get_peer(peer_index);
    unsigned long score = 0;
    long peer_flag = FLAG(peer_index);
    long connectivity_mask = peer->properties.connectivity.peer_connectivity_mask & valid_peer_mask;
    long probe_mask = peer->properties.connectivity.peer_probe_mask & valid_peer_mask;
    long bandwidth = peer->properties.downstream_bandwidth;
    if (membership->host_peer_index() == peer_index)
        connectivity_mask &= ~ignored_connectivity_mask;
    if (session->m_session_class != 2)
        bandwidth = 0xa00000;
    unsigned long latency_bucket = (peer->properties.connectivity.latency_average_msec + 49) / 50;
    long desired_capacity = maximum(1L, network_bandwidth_get_estimated_desired_machine_capacity_of_bandwidth(bandwidth));
    long maximum_capacity = maximum(1L, network_bandwidth_get_estimated_maximum_machine_capacity_of_bandwidth(bandwidth));
    if (peer_index == membership->host_peer_index())
        score += weights->local_host;
    *peer_connection_count = (unsigned char)count_bits<long>(connectivity_mask);
    score += MAX(0, *peer_connection_count - 1) * weights->connectivity;
    if (connectivity_mask == valid_peer_mask)
        *connected_peer_mask |= peer_flag;
    long bounded_desired_hostable_peers = MAX(MIN(desired_capacity, maximum_peer_count), 1);
    score += (bounded_desired_hostable_peers - 1) * weights->peer_capacity;
    score += (maximum_capacity - 1) * weights->machine_capacity;
    score += (MAX(desired_capacity, 1) - 1) * weights->player_capacity;
    long bandwidth_capacity = bandwidth / g_network_configuration.observer.bandwidth_per_player;
    long bounded_bandwidth_hostable_peers = MAX(MIN(bandwidth_capacity, maximum_peer_count), 1);
    score += (MAX(bounded_bandwidth_hostable_peers, 1) - 1) * weights->bandwidth_capacity;
    unsigned long latency_score = configuration->latency_scores[MIN(latency_bucket, 19)];
    score += latency_score * weights->latency;
    if (session->does_peer_have_gamestate(peer_index))
        score += weights->gamestate;
    if (peer->properties.nat_type == 1)
        score += weights->open_nat;
    if (probe_mask != valid_peer_mask)
        *all_peers_connected = false;
    if (bandwidth == 0)
        *all_peers_connected = false;
    if (peer->properties.has_gamestate_bonus)
        score += weights->flagged;
    long player_count_score = MAX(0, configuration->player_count_scores[peer->player_count] - 1);
    score += player_count_score * weights->player_count;
    score += latency_tier * weights->quality_tier;
    score += quality_tier * weights->latency_tier;
    return score;
}

// Fold the score key and every peer score into a single total for logging.
static __forceinline void network_session_host_log_score_key(const s_bandwidth_host_preference_weights *weights, const unsigned long *peer_scores)
{
    unsigned long score_key[12];
    score_key[0] = weights->connectivity * 15;
    score_key[1] = weights->local_host;
    score_key[2] = weights->peer_capacity * 15;
    score_key[3] = weights->machine_capacity * 15;
    score_key[4] = weights->player_capacity * 15;
    score_key[5] = weights->latency * 15;
    score_key[6] = weights->gamestate;
    score_key[7] = weights->open_nat;
    score_key[8] = weights->flagged;
    score_key[9] = weights->player_count * 3;
    score_key[10] = weights->quality_tier * 7;
    score_key[11] = weights->latency_tier * 7;
    long total = 0;
    total += peer_scores[0] + peer_scores[1] + peer_scores[2] + peer_scores[3];
    total += peer_scores[4] + peer_scores[5] + peer_scores[6] + peer_scores[7];
    for (long index = 0; index < 12; index++)
        total += score_key[index];
}

// @address 0x4260c8 ?find_good_peer_to_be_host@c_network_session@@QAA?AUs_network_session_find_host@@W4e_bandwidth_configuration_host_preference@@_N11JJJ1@Z
s_network_session_find_host c_network_session::find_good_peer_to_be_host(e_bandwidth_configuration_host_preference host_preference,
    bool prefer_current_host, bool exclude_current_host, bool unused_flag, long maximum_peer_count, long excluded_peer_mask,
    long ignored_connectivity_mask, bool unused_flag2)
{
    const c_network_session_membership *membership = get_session_membership();
    const s_bandwidth_configuration *configuration = &g_network_configuration.bandwidth;
    s_network_session_find_host result = { 0 };
    long host_peer_index = NONE;
    long second_host_peer_index = NONE;
    bool host_fully_connected = false;
    long valid_peer_mask = membership->get_peer_valid_mask();
    if (maximum_peer_count == NONE)
        maximum_peer_count = membership->get_peer_count();
    if (exclude_current_host && TEST_BIT(valid_peer_mask, membership->host_peer_index()))
    {
        valid_peer_mask &= ~FLAG(membership->host_peer_index());
        maximum_peer_count--;
    }

    bool all_peers_connected = true;
    unsigned long connected_peer_mask = 0;
    unsigned long best_score = 0;
    unsigned long second_best_score = 0;
    long best_score_count = 0;
    long best_peer_index = NONE;
    long second_best_peer_index = NONE;
    bool peer_valid[16];
    unsigned char peer_latency_tier[16];
    unsigned char peer_quality_tier[16];
    unsigned char peer_connection_count[16];
    unsigned long peer_scores[16];
    unsigned long peer_quality[16];
    unsigned long peer_latency[16];
    memset(peer_valid, 0, sizeof(peer_valid));
    memset(peer_scores, 0, sizeof(peer_scores));
    memset(peer_quality, 0, sizeof(peer_quality));
    memset(peer_quality_tier, 0, sizeof(peer_quality_tier));
    memset(peer_latency, 0, sizeof(peer_latency));
    memset(peer_latency_tier, 0, sizeof(peer_latency_tier));
    memset(peer_connection_count, 0, sizeof(peer_connection_count));
    long minimum_quality = 100;
    long minimum_latency = 100;

    for (long peer_index = membership->get_first_peer(); peer_index != NONE; peer_index = membership->get_next_peer(peer_index))
    {
        const s_network_session_peer_properties *peer_properties = &membership->get_peer(peer_index)->properties;
        unsigned long player_flags = 0;
        if (TEST_BIT(excluded_peer_mask, peer_index))
            continue;
        long observer_channel_index = membership->m_peer_channels[peer_index].observer_channel_index;
        if (observer_channel_index != NONE && m_observer->m_channel_observers[observer_channel_index].state != 7)
            continue;
        if (!network_session_host_peer_connection_ready(membership, peer_index))
            continue;
        if (prefer_current_host && !peer_properties->is_peer_flagged && peer_index != membership->host_peer_index())
            continue;
        long player_index = 0;
        do
        {
            const s_network_session_player *player = membership->get_player_from_peer(peer_index, player_index);
            if (player)
                player_flags |= player->configuration.client.player_flags;
        } while (++player_index < 4);
        if (player_flags & 8)
            continue;
        if (host_preference == _bandwidth_configuration_host_preference_latency && ((player_flags & 4) || (player_flags & 0x20) || (player_flags & 0x40)))
            continue;
        peer_quality[peer_index] = peer_properties->connection_quality;
        peer_latency[peer_index] = peer_properties->connectivity_badness;
        if (network_owner_entry_is_pending(const_cast<c_network_session_membership *>(membership), peer_index) || (player_flags & 0x40))
        {
            peer_quality[peer_index] = 100;
            peer_latency[peer_index] = 100;
        }
        else if (player_flags & 0x10)
        {
            peer_latency[peer_index] = 100 - (100 - peer_latency[peer_index]) * g_network_configuration.qos_quality_scale / 100;
            if (peer_latency[peer_index] > 100)
                peer_latency[peer_index] = 100;
        }
        if (peer_quality[peer_index] < minimum_quality)
            minimum_quality = peer_quality[peer_index];
        if (peer_latency[peer_index] < minimum_latency)
            minimum_latency = peer_latency[peer_index];
        peer_valid[peer_index] = true;
    }

    for (long tier = 7; tier > 0; tier--)
    {
        long next_quality = 100;
        long next_latency = 100;
        long quality_threshold = MIN(minimum_quality + configuration->latency_tier_delta, 99);
        long latency_threshold = MIN(minimum_latency + configuration->quality_tier_delta, 99);
        long peer_index = 0;
        do
        {
            if (!peer_valid[peer_index])
                continue;
            if (minimum_quality != 100 && peer_quality[peer_index] >= (unsigned long)minimum_quality)
            {
                if (peer_quality[peer_index] < (unsigned long)quality_threshold)
                    peer_quality_tier[peer_index] = (unsigned char)tier;
                else if (peer_quality[peer_index] < (unsigned long)next_quality)
                    next_quality = peer_quality[peer_index];
            }
            if (minimum_latency != 100 && peer_latency[peer_index] >= (unsigned long)minimum_latency)
            {
                if (peer_latency[peer_index] < (unsigned long)latency_threshold)
                    peer_latency_tier[peer_index] = (unsigned char)tier;
                else if (peer_latency[peer_index] < (unsigned long)next_latency)
                    next_latency = peer_latency[peer_index];
            }
        } while (++peer_index < 16);
        if (next_quality == 100 && next_latency == 100)
            break;
        minimum_quality = next_quality;
        minimum_latency = next_latency;
    }

    for (long peer_index = 0; peer_index < 16; peer_index++)
    {
        if (!peer_valid[peer_index])
            continue;
        unsigned long score = network_session_host_compute_peer_score(this, &configuration->host_preference_weights[host_preference],
            peer_index, valid_peer_mask, maximum_peer_count, ignored_connectivity_mask, peer_latency_tier[peer_index],
            peer_quality_tier[peer_index], &connected_peer_mask, &all_peers_connected, &peer_connection_count[peer_index]);
        peer_scores[peer_index] = score;
        if (score > best_score)
        {
            best_score = score;
            best_score_count = 1;
            best_peer_index = peer_index;
        }
        else if (score == best_score)
        {
            best_score_count++;
            second_best_score = best_score;
            second_best_peer_index = peer_index;
        }
        else if (score > second_best_score)
        {
            second_best_score = best_score;
            second_best_peer_index = peer_index;
        }
    }

    if (best_score)
    {
        long skip_count = host_preference == _bandwidth_configuration_host_preference_latency
            ? random_range(get_local_random_seed_address(), 0, (short)best_score_count)
            : 0;
        host_peer_index = best_peer_index;
        while (skip_count > 0)
        {
            do
            {
                host_peer_index++;
            } while (peer_scores[host_peer_index] != best_score);
            skip_count--;
        }
        second_host_peer_index = second_best_peer_index;
        if (host_peer_index == second_host_peer_index)
            second_host_peer_index = best_peer_index;
    }

    if (prefer_current_host && host_peer_index != membership->host_peer_index() && host_peer_index != NONE)
    {
        long current_host_peer_index = membership->host_peer_index();
        if (host_preference == _bandwidth_configuration_host_preference_connectivity)
        {
            unsigned long weight = configuration->host_preference_weights[0].latency_tier;
            unsigned long current_score = peer_scores[current_host_peer_index] & (weight * 7);
            unsigned long host_score = peer_scores[host_peer_index] & (weight * 7);
            if (host_score <= current_score)
                host_peer_index = current_host_peer_index;
        }
        else
        {
            unsigned long capacity_weight = configuration->host_preference_weights[host_preference].peer_capacity;
            unsigned long tier_weight = configuration->host_preference_weights[host_preference].quality_tier;
            long capacity_threshold;
            long tier_threshold;
            if (host_preference == _bandwidth_configuration_host_preference_latency)
            {
                capacity_threshold = configuration->capacity_threshold;
                tier_threshold = configuration->latency_tier_threshold;
            }
            else
            {
                capacity_threshold = configuration->other_capacity_threshold;
                tier_threshold = configuration->other_latency_tier_threshold;
            }
            long capacity_difference = (long)((peer_scores[host_peer_index] & (capacity_weight * 15)) / capacity_weight) -
                (long)((peer_scores[membership->host_peer_index()] & (capacity_weight * 15)) / capacity_weight);
            long tier_difference = (long)((peer_scores[host_peer_index] & (tier_weight * 7)) / tier_weight) -
                (long)((peer_scores[membership->host_peer_index()] & (tier_weight * 7)) / tier_weight);
            bool significantly_better = false;
            if ((capacity_difference != 0 || tier_difference != 0) && capacity_difference >= capacity_threshold && tier_difference >= tier_threshold)
                significantly_better = true;
            if (!significantly_better)
                host_peer_index = current_host_peer_index;
        }
    }
    else if (ignored_connectivity_mask)
    {
        long current_host_peer_index = membership->host_peer_index();
        if (host_peer_index != current_host_peer_index && host_peer_index != NONE)
        {
            unsigned long current_score = peer_scores[current_host_peer_index] & (configuration->host_preference_weights[0].connectivity * 15);
            unsigned long host_score = peer_scores[host_peer_index] & (configuration->host_preference_weights[0].connectivity * 15);
            if (host_score <= current_score)
                host_peer_index = current_host_peer_index;
        }
    }
    if (host_peer_index != NONE)
        host_fully_connected = peer_connection_count[host_peer_index] == maximum_peer_count;

    if (unused_flag2)
        network_session_host_log_score_key(&configuration->host_preference_weights[host_preference], peer_scores);

    result.host_peer_index = host_peer_index;
    result.second_host_peer_index = second_host_peer_index;
    result.all_peers_connected = all_peers_connected;
    result.host_fully_connected = host_fully_connected;
    result.connected_peer_mask = connected_peer_mask;
    return result;
}

// @address 0x426b58 ?has_multiple_potential_hosts@c_network_session@@QAA_NXZ
bool c_network_session::has_multiple_potential_hosts()
{
    bool result;
    const c_network_session_parameter_matchmaking_hopper *hopper_parameter;
    const c_network_session_membership *membership;
    s_network_session_find_host first_host_result;
    int first_host_index;
    const s_network_session_player *first_player;
    s_network_session_find_host next_host_result;
    int next_host_index;
    const s_network_session_player *next_player;
    bool multiple_hosts;
    const s_network_session_matchmaking_hopper *hopper;
    int excluded_peer_mask;
    int first_machine;
    int first_player_slot;
    int next_player_slot;
    bool zero = false;
    bool one = true;
    excluded_peer_mask = 0;
    multiple_hosts = false;
    hopper_parameter = &get_session_parameters()->m_parameters.matchmaking_hopper;
    membership = &m_session_membership;
    hopper = 0;
    if (hopper_parameter->get_allowed())
        hopper = &hopper_parameter->m_data;
    excluded_peer_mask = 0;
    find_good_peer_to_be_host(_bandwidth_configuration_host_preference_latency, false, false, true, NONE, 0, 0, true);
    do
    {
        first_host_result = find_good_peer_to_be_host(_bandwidth_configuration_host_preference_latency, false, false, true, NONE, excluded_peer_mask, 0, false);
        first_host_index = first_host_result.host_peer_index;
        if (first_host_index == NONE)
            break;
        excluded_peer_mask |= 1 << first_host_index;
    }
    while (!first_host_result.host_fully_connected);
    if (first_host_index == NONE)
        result = zero;
    else if ((unsigned int)hopper->require_hosts_on_multiple_teams != 0)
    {
        first_machine = NONE;
        first_player_slot = 0;
        do
        {
            first_player = membership->get_player_from_peer(first_host_index, first_player_slot);
            if (first_player != 0)
            {
                first_machine = first_player->configuration.host.team_index;
                break;
            }
            first_player_slot++;
        }
        while (first_player_slot < 4);
        do
        {
            next_host_result = find_good_peer_to_be_host(
                _bandwidth_configuration_host_preference_latency, false, false, true, NONE, excluded_peer_mask, 0, false);
            next_host_index = next_host_result.host_peer_index;
            if (next_host_index == NONE)
                break;
            excluded_peer_mask |= 1 << next_host_index;
            if (next_host_result.host_fully_connected)
            {
                next_player_slot = 0;
                do
                {
                    next_player = membership->get_player_from_peer(next_host_index, next_player_slot);
                    if (next_player != 0 && first_machine != next_player->configuration.host.team_index)
                    {
                        multiple_hosts = true;
                        break;
                    }
                    next_player_slot++;
                }
                while (next_player_slot < 4);
            }
        }
        while (!multiple_hosts);
        return multiple_hosts;
    }
    else
    {
        result = one;
    }
    return result;
}
