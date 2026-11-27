// networkinglibxbox/network_team_picker.obj: from the records 0x440990 0x4409a0 0x440a48 0x440af0 0x440b68 0x440cb0 0x440d50 0x440dd8 0x440e60 0x4416a0 0x4417a8 0x441930 0x441a88 0x441c00
// @flags /TP /O2 /Oc
// @address 0x440990 ??0c_network_team_picker_player@@QAA@XZ
// @address 0x4409a0 ?add_player@c_network_team_picker_squad@@QAAXABVc_network_team_picker_player@@@Z
// @address 0x440af0 ?add_squad@c_network_team_picker_group@@QAAXABVc_network_team_picker_squad@@@Z
// @address 0x440b68 ?remove_squad@c_network_team_picker_group@@QAAXABVc_network_team_picker_squad@@@Z
// @address 0x440cb0 ?remove_biggest_and_most_skillfull@c_network_team_picker_group@@QAAXAAVc_network_team_picker_squad@@@Z
// @address 0x440d50 ?get_max_total_skill_level_imbalance@c_game_teams@@QBAJXZ
// @address 0x440dd8 ?get_maximum_team_imbalance@c_game_teams@@QBAJXZ
// @address 0x440e60 ?pick_teams@c_round_team_picker@@QAA_NABVc_network_team_picker_group@@JJJJ_N111@Z
// @address 0x4416a0 ?teams_valid@c_round_team_picker@@CA_NAAVc_game_teams@@JJJ_N@Z
// @address 0x4417a8 ?distribute_group_to_teams@c_round_team_picker@@AAAXJAAVc_network_team_picker_group@@@Z
// @address 0x441930 ?get_squad_to_break_up@c_round_team_picker@@AAA_NW4e_squad_break_up_type@1@PAJ1@Z
// @address 0x441a88 ?network_team_picker_can_add_whole_squad@@YA_NJJJJJPBUs_network_team_picker_player@@@Z
// @address 0x440a48 ??0c_network_team_picker_group@@QAA@XZ
// @address 0x441c00 ?network_team_picker_pick_teams@@YA_NJJJJ_N000JPBUs_network_team_picker_player@@PAJ2@Z
#include "cseries/cseries_types.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include <string.h>
#include "networking/network_team_picker.h"
#include "networking/network_team_picker_player_types.h"

class c_network_team_picker_player
{
public:
    c_network_team_picker_player();
    __forceinline long get_peer_index() const { return value; }

    int value;
    int skill;
    int extra;
};

class c_network_team_picker_squad
{
public:
    __forceinline c_network_team_picker_squad() { clear(); }
    inline void clear() { count = 0; total_skill = 0; flags = -1; }
    void add_player(const c_network_team_picker_player &player);
    __forceinline void remove_player(const c_network_team_picker_player &player);
    __forceinline void remove_players(const c_network_team_picker_squad &squads);
    __forceinline long get_player_count() const { return count; }
    __forceinline void set_index(long index) { flags = index; }
    __forceinline const c_network_team_picker_player &get_player(long index) const { return players[index]; }

    int flags;
    int count;
    int total_skill;
    c_network_team_picker_player players[16];
};

class c_network_team_picker_group
{
public:
    c_network_team_picker_group();
    inline void clear() { squad_count = 0; merged.clear(); merged.flags = 0; }
    __forceinline long get_squad_count() const { return squad_count; }
    __forceinline const c_network_team_picker_squad &get_squad(long index) const { return squads[index]; }
    __forceinline long get_player_count() const { return merged.get_player_count(); }
    void remove_squad(const c_network_team_picker_squad &squad);
    void add_squad(const c_network_team_picker_squad &squad);
    void remove_biggest_and_most_skillfull(c_network_team_picker_squad &squad);
    long squad_count;
    c_network_team_picker_squad squads[16];
    c_network_team_picker_squad merged;
};
class c_network_team_picker_team : public c_network_team_picker_group {};

class c_game_teams
{
public:
    __forceinline c_game_teams() { team_count = 0; }
    long get_max_total_skill_level_imbalance() const;
    long get_maximum_team_imbalance() const;
    void add_squad(long team_index, c_network_team_picker_squad &squad);

    __forceinline long get_team_count() const { return team_count; }
    __forceinline const c_network_team_picker_team &get_team(long index) const { return m_teams[index]; }
    __forceinline void set_empty_teams(long count)
    {
        team_count = count;
        for (long team_index = 0; team_index < count; team_index++)
            m_teams[team_index].clear();
    }
    int team_count;
    c_network_team_picker_team m_teams[16];
};


// get_maximum_team_imbalance reads each team's merged player count.

class c_round_team_picker
{
public:
    __forceinline c_round_team_picker() { m_success = false; }
    __forceinline const c_game_teams &get_teams() const { return m_game_teams; }
    bool pick_teams(const c_network_team_picker_group &group, long team_count, long minimum_player_count, long maximum_player_count,
        long maximum_imbalance, bool keep_squads_together, bool random_polish, bool check_imbalance, bool skill_polish);
private:
    enum e_squad_break_up_type { _break_up_multiple_peers, _break_up_any_squad, k_squad_break_up_type_count };
    bool get_squad_to_break_up(e_squad_break_up_type type, long *team_index_out, long *squad_index_out);
    void distribute_group_to_teams(long round, c_network_team_picker_group &group);
    bool m_success;
    c_game_teams m_game_teams;
    static bool teams_valid(c_game_teams &teams, long min_player_count, long max_player_count, long max_imbalance, bool check_imbalance);
};

static __forceinline long network_team_picker_minimum_team_value(const c_game_teams *game_teams)
{
    long minimum = game_teams->m_teams[0].merged.count;
    for (long team_index = 1; team_index < game_teams->team_count; team_index++) {
        if (minimum > game_teams->m_teams[team_index].merged.count)
            minimum = game_teams->m_teams[team_index].merged.count;
    }
    return minimum;
}

static __forceinline long network_team_picker_maximum_team_value(const c_game_teams *game_teams)
{
    long maximum = game_teams->m_teams[0].merged.count;
    for (long team_index = 1; team_index < game_teams->team_count; team_index++) {
        if (maximum <= game_teams->m_teams[team_index].merged.count)
            maximum = game_teams->m_teams[team_index].merged.count;
    }
    return maximum;
}

// 0x440990
c_network_team_picker_player::c_network_team_picker_player()
{
    skill = -1;
    extra = -1;
}

// 0x4409a0
inline void c_network_team_picker_squad::add_player(const c_network_team_picker_player &player)
{
    int index = count;
    while (index > 0 && players[index - 1].skill < player.skill) {
        players[index] = players[index - 1];
        index--;
    }
    players[index] = player;
    count++;
    total_skill += player.skill;
}

// 0x440a48
c_network_team_picker_group::c_network_team_picker_group()
{
    clear();
}


void c_network_team_picker_group::add_squad(const c_network_team_picker_squad &squad)
{
    memcpy(&squads[squad_count], &squad, sizeof(c_network_team_picker_squad));
    squad_count++;
    for (int i = 0; i < squad.count; i++)
        merged.add_player(squad.players[i]);
}

__forceinline void c_game_teams::add_squad(long team_index, c_network_team_picker_squad &squad)
{
    m_teams[team_index].add_squad(squad);
}

// 0x440b68
__forceinline void c_network_team_picker_squad::remove_player(const c_network_team_picker_player &player)
{
    long index = 0;
    const c_network_team_picker_player *candidate;
    do {
        candidate = &players[index];
        if (candidate->extra == player.extra)
            break;
        index++;
    } while (candidate != 0);
    count--;
    while (index < count) {
        players[index] = players[index + 1];
        index++;
    }
    total_skill -= player.skill;
}

__forceinline void c_network_team_picker_squad::remove_players(const c_network_team_picker_squad &squads)
{
    for (long index = 0; index < squads.count; index++)
        remove_player(squads.players[index]);
}

void c_network_team_picker_group::remove_squad(const c_network_team_picker_squad &squad)
{
    long squad_index = 0;
    while (squads[squad_index].flags != squad.flags)
        squad_index++;
    squad_count--;
    while (squad_index < squad_count) {
        squads[squad_index] = squads[squad_index + 1];
        squad_index++;
    }
    merged.remove_players(squad);
}

// 0x440cb0
void c_network_team_picker_group::remove_biggest_and_most_skillfull(c_network_team_picker_squad &squad)
{
    c_network_team_picker_squad *biggest = &squads[0];
    for (long squad_index = 1; squad_index < squad_count; squad_index++) {
        c_network_team_picker_squad *candidate = &squads[squad_index];
        if (candidate->count <= biggest->count) {
            if (candidate->count == biggest->count && candidate->total_skill > biggest->total_skill)
                biggest = candidate;
        } else {
            biggest = candidate;
        }
    }
    memcpy(&squad, biggest, sizeof(c_network_team_picker_squad));
    remove_squad(squad);
}

// 0x440d50
long c_game_teams::get_max_total_skill_level_imbalance() const
{
    long maximum = 0;
    for (long i = 0; i < team_count; i++) {
        for (long j = 0; j < team_count; j++) {
            if (i != j) {
                long b = m_teams[j].merged.total_skill;
                long a = m_teams[i].merged.total_skill;
                long difference = (a - b >= 0) ? a - b : b - a;
                if (maximum <= difference)
                    maximum = difference;
            }
        }
    }
    return maximum;
}

// 0x440dd8
long c_game_teams::get_maximum_team_imbalance() const
{
    const c_game_teams *game_teams = this;
    long maximum = 0;
    for (long i = 0; i < game_teams->team_count; i++) {
        for (long j = 0; j < game_teams->team_count; j++) {
            if (i != j) {
                long b = game_teams->m_teams[j].merged.count;
                long a = game_teams->m_teams[i].merged.count;
                long difference = (a - b >= 0) ? a - b : b - a;
                if (maximum <= difference)
                    maximum = difference;
            }
        }
    }
    return maximum;
}

// 0x440e60
bool c_round_team_picker::pick_teams(const c_network_team_picker_group &input_group, long team_count, long minimum_player_count,
    long maximum_player_count, long maximum_imbalance, bool keep_squads_together, bool random_polish, bool check_imbalance,
    bool skill_polish)
{
    c_network_team_picker_group group = input_group;
    c_game_teams *game_teams = &m_game_teams;
    m_success = false;
    game_teams->set_empty_teams(team_count);

    long team_index = random_range(get_local_random_seed_address(), 0, (short)team_count);
    long assigned_count = 0;
    while (group.get_squad_count() > 0) {
        if (assigned_count >= team_count)
            break;
        c_network_team_picker_squad squad;
        group.remove_biggest_and_most_skillfull(squad);
        game_teams->add_squad(team_index, squad);
        assigned_count++;
        team_index = (team_index + 1) % team_count;
    }

    distribute_group_to_teams(2, group);

    if (!teams_valid(m_game_teams, minimum_player_count, maximum_player_count, maximum_imbalance, check_imbalance) && !keep_squads_together) {
        e_squad_break_up_type break_up_type = _break_up_multiple_peers;
        while (!teams_valid(m_game_teams, minimum_player_count, maximum_player_count, maximum_imbalance, check_imbalance)) {
            long break_up_team_index = -1;
            long break_up_squad_index = -1;
            while (!get_squad_to_break_up(break_up_type, &break_up_team_index, &break_up_squad_index)) {
                break_up_type = (e_squad_break_up_type)(break_up_type + 1);
                if (break_up_type == k_squad_break_up_type_count)
                    break;
            }
            if (break_up_type == k_squad_break_up_type_count)
                break;
            c_network_team_picker_squad squad = m_game_teams.m_teams[break_up_team_index].squads[break_up_squad_index];
            m_game_teams.m_teams[break_up_team_index].remove_squad(squad);
            if ((unsigned long)break_up_type >= 1) {
                for (long player_index = 0; player_index < squad.get_player_count(); player_index++) {
                    c_network_team_picker_squad new_squad;
                    new_squad.set_index(group.get_squad_count());
                    new_squad.add_player(squad.get_player(player_index));
                    group.add_squad(new_squad);
                }
            } else {
                for (long peer_index = 0; peer_index < 16; peer_index++) {
                    c_network_team_picker_squad new_squad;
                    new_squad.set_index(group.get_squad_count());
                    for (long player_index = 0; player_index < squad.get_player_count(); player_index++) {
                        if (squad.get_player(player_index).get_peer_index() == peer_index)
                            new_squad.add_player(squad.get_player(player_index));
                    }
                    if (new_squad.get_player_count() > 0)
                        group.add_squad(new_squad);
                }
            }
            distribute_group_to_teams(3, group);
        }
    }

    if (skill_polish && teams_valid(m_game_teams, minimum_player_count, maximum_player_count, maximum_imbalance, check_imbalance)) {
        for (long first_team_index = 0; first_team_index < team_count; first_team_index++) {
            for (long second_team_index = 0; second_team_index < team_count; second_team_index++) {
                if (first_team_index != second_team_index) {
                    long first_squad_count = m_game_teams.get_team(first_team_index).get_squad_count();
                    long second_squad_count = m_game_teams.get_team(second_team_index).get_squad_count();
                    for (long first_squad_index = 0; first_squad_index < first_squad_count; first_squad_index++) {
                        for (long second_squad_index = 0; second_squad_index < second_squad_count; second_squad_index++) {
                            c_game_teams new_teams = m_game_teams;
                            long old_imbalance = m_game_teams.get_max_total_skill_level_imbalance();
                            c_network_team_picker_squad first_squad = m_game_teams.m_teams[first_team_index].get_squad(first_squad_index);
                            c_network_team_picker_squad second_squad = m_game_teams.m_teams[second_team_index].get_squad(second_squad_index);
                            new_teams.m_teams[first_team_index].remove_squad(first_squad);
                            new_teams.m_teams[second_team_index].remove_squad(second_squad);
                            new_teams.m_teams[second_team_index].add_squad(first_squad);
                            new_teams.m_teams[first_team_index].add_squad(second_squad);
                            if (new_teams.get_max_total_skill_level_imbalance() < old_imbalance &&
                                teams_valid(new_teams, minimum_player_count, maximum_player_count, maximum_imbalance, check_imbalance))
                                m_game_teams = new_teams;
                        }
                    }
                }
            }
        }
    }

    if (random_polish && teams_valid(m_game_teams, minimum_player_count, maximum_player_count, maximum_imbalance, check_imbalance)) {
        for (long first_team_index = 0; first_team_index < team_count; first_team_index++) {
            for (long second_team_index = 0; second_team_index < team_count; second_team_index++) {
                if (first_team_index != second_team_index) {
                    long first_squad_count = m_game_teams.get_team(first_team_index).get_squad_count();
                    long second_squad_count = m_game_teams.get_team(second_team_index).get_squad_count();
                    for (long first_squad_index = 0; first_squad_index < first_squad_count; first_squad_index++) {
                        if (random_seed_next(get_local_random_seed_address()) & 0x80000000) {
                            for (long second_squad_index = 0; second_squad_index < second_squad_count; second_squad_index++) {
                                if (random_seed_next(get_local_random_seed_address()) & 0x80000000) {
                                    c_game_teams new_teams = m_game_teams;
                                    c_network_team_picker_squad first_squad = m_game_teams.m_teams[first_team_index].get_squad(first_squad_index);
                                    c_network_team_picker_squad second_squad = m_game_teams.m_teams[second_team_index].get_squad(second_squad_index);
                                    new_teams.m_teams[first_team_index].remove_squad(first_squad);
                                    new_teams.m_teams[second_team_index].remove_squad(second_squad);
                                    new_teams.m_teams[second_team_index].add_squad(first_squad);
                                    new_teams.m_teams[first_team_index].add_squad(second_squad);
                                    if (teams_valid(new_teams, minimum_player_count, maximum_player_count, maximum_imbalance, check_imbalance))
                                        m_game_teams = new_teams;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    bool valid = teams_valid(m_game_teams, minimum_player_count, maximum_player_count, maximum_imbalance, check_imbalance);
    m_success = valid;
    return valid;
}

// 0x4416a0
bool c_round_team_picker::teams_valid(c_game_teams &teams, long min_player_count, long max_player_count, long max_imbalance, bool check_imbalance)
{
    const c_game_teams *game_teams = &teams;
    bool valid = true;
    valid = valid && network_team_picker_minimum_team_value(game_teams) >= min_player_count;
    valid = valid && network_team_picker_maximum_team_value(game_teams) <= max_player_count;
    if (check_imbalance)
        valid = valid && teams.get_maximum_team_imbalance() <= max_imbalance;
    return valid;
}

static __forceinline int network_team_picker_smallest_and_less_skillfull(c_game_teams *teams)
{
    int team = 0;
    for (int i = 1; i < teams->team_count; ++i) {
        if (teams->m_teams[i].merged.count < teams->m_teams[team].merged.count ||
            (teams->m_teams[i].merged.count == teams->m_teams[team].merged.count &&
             teams->m_teams[i].merged.total_skill < teams->m_teams[team].merged.total_skill))
            team = i;
    }
    return team;
}

// 0x4417a8
void c_round_team_picker::distribute_group_to_teams(long round, c_network_team_picker_group &input_group)
{
    c_network_team_picker_group *group = &input_group;
    while (group->squad_count > 0) {
        c_network_team_picker_squad squad;
        int team = -1;
        int old_index = -1;
        group->remove_biggest_and_most_skillfull(squad);
        team = network_team_picker_smallest_and_less_skillfull(&m_game_teams);
        old_index = squad.flags;
        {
            c_game_teams *teams = &m_game_teams;
            int next = -1;
            unsigned int used = 0;
            for (int i = 0; i < teams->team_count; ++i) {
                c_network_team_picker_group team_group = teams->m_teams[i];
                for (int j = 0; j < team_group.squad_count; ++j)
                    used |= 1 << team_group.squads[j].flags;
            }
            for (int i = 0; i < 16; ++i) {
                if ((used & (1 << i)) == 0) {
                    next = i;
                    break;
                }
            }
            squad.flags = next;
        }
        m_game_teams.m_teams[team].add_squad(squad);
    }
}

// 0x441930
// match: 0x441930: __restrict on team_index_out keeps the result=true immediate ahead of both out-pointer stores
bool c_round_team_picker::get_squad_to_break_up(e_squad_break_up_type type, long *__restrict team_index_out, long *squad_index_out)
{
    bool result = false;
    long selected_team_index = -1;
    long selected_squad_index = -1;
    long best_team_player_count = 0;
    long best_squad_player_count = 0;
    long team_count = m_game_teams.get_team_count();
    for (long team_index = 0; team_index < team_count; ++team_index) {
        c_network_team_picker_team team = m_game_teams.get_team(team_index);
        long squad_count = team.get_squad_count();
        for (long squad_index = 0; squad_index < squad_count; ++squad_index) {
            bool break_up = false;
            c_network_team_picker_squad squad = team.get_squad(squad_index);
            long player_count = squad.get_player_count();
            if (player_count > 1) {
                switch (type) {
                case _break_up_multiple_peers: {
                    long peer_index = -1;
                    for (long player_index = 0; player_index < player_count; ++player_index) {
                        if (peer_index == -1) {
                            peer_index = squad.get_player(player_index).get_peer_index();
                        } else if (peer_index != squad.get_player(player_index).get_peer_index()) {
                            break_up = true;
                            break;
                        }
                    }
                    break;
                }
                case _break_up_any_squad:
                    break_up = true;
                    break;
                default:
                    // match: 0x441930: __assume keeps the type-dispatch branch polarity and the peer-scan loop laid out in line
                    __assume(0);
                }
            }
            if (break_up && (team.get_player_count() > best_team_player_count ||
                (team.get_player_count() == best_team_player_count && squad.get_player_count() > best_squad_player_count))) {
                selected_team_index = team_index;
                selected_squad_index = squad_index;
                best_team_player_count = team.get_player_count();
                best_squad_player_count = squad.get_player_count();
            }
        }
    }
    if (selected_team_index != -1 && selected_squad_index != -1) {
        *team_index_out = selected_team_index;
        *squad_index_out = selected_squad_index;
        result = true;
    }
    return result;
}



bool network_team_picker_can_add_whole_squad(long player_count, long team_count, long maximum_team_size,
                                             long maximum_imbalance, long new_player_count,
                                             const s_network_team_picker_player *new_players)
{
    long maximum_player_count = team_count * maximum_team_size;
    long total_player_count = new_player_count + player_count;
    long player_space_available = maximum_player_count - total_player_count;
    long squad_count = 0;
    long new_squad_count = 0;
    long imbalance = -1;
    bool picked = false;
    bool result = false;
    s_network_team_picker_player players[16];
    memset(players, 0, sizeof(players));
    for (long i = 0; i < new_player_count; i++) {
        long squad_index = new_players[i].squad_index;
        squad_count = (squad_count > squad_index + 1) ? squad_count : squad_index + 1;
        players[i].squad_index = squad_index;
    }
    new_squad_count = squad_count + 1;
    for (long i = new_player_count; i < total_player_count; i++)
        players[i].squad_index = new_squad_count - 1;
    for (long i = 0; i < total_player_count; i++) {
        players[i].peer_index = 0;
        players[i].skill_level = 1;
    }
    picked = network_team_picker_pick_teams(team_count, 0, maximum_team_size, maximum_imbalance, true, false, false, false,
                          total_player_count, players, &imbalance, 0);
    if (picked) {
        imbalance = (imbalance - player_space_available > 0) ? imbalance - player_space_available : 0;
        result = !(imbalance > maximum_imbalance);
    }
    return result;
}

// 0x441c00
bool network_team_picker_pick_teams(long a, long b, long c, long d, bool e, bool f, bool g, bool h,
                    // match: 0x441c00: __restrict on assignments avoids a team-count reload in the assignment loop and keeps loop register colors
                    long player_count, const s_network_team_picker_player *players, long *imbalance, long *__restrict assignments)
{
    c_network_team_picker_squad squads[16];
    c_network_team_picker_group group;
    c_round_team_picker picker;
    bool result = false;
    for (long i = 0; i < player_count; ++i) {
        c_network_team_picker_player player;
        player.extra = i;
        player.skill = players[i].skill_level;
        player.value = players[i].peer_index;
        squads[players[i].squad_index].add_player(player);
    }
    for (long i = 0; i < 16; ++i) {
        if (squads[i].count > 0) {
            c_network_team_picker_squad &squad = squads[i];
            squad.flags = i;
            group.add_squad(squad);
        }
    }
    if (picker.pick_teams(group, a, b, c, d, e, f, g, h)) {
        const c_game_teams &teams = picker.get_teams();
        if (assignments) {
            for (long team_index = 0; team_index < teams.team_count; ++team_index) {
                const c_network_team_picker_group &team = teams.m_teams[team_index];
                for (long player_index = 0; player_index < team.merged.count; ++player_index) {
                    const c_network_team_picker_player &player = team.merged.players[player_index];
                    assignments[player.extra] = team_index;
                }
            }
        }
        if (imbalance)
            *imbalance = teams.get_maximum_team_imbalance();
        result = true;
    }
    return result;
}
