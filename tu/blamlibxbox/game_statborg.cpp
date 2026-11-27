// blamlibxbox/game_statborg.obj: from the records 0x29ad18 0x29b340 0x29ae90 0x29b0e0 0x29b5e8 0x29b748 0x29b7e8 0x29bfd8 0x29c318
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x29b340 ?apply_statborg_update@c_game_statborg@@QAA_NKJPBX@Z
// @address 0x29b5e8 ?adjust_player_stat@c_game_statborg@@QAAXJW4e_statborg_entry@@FJ_N@Z
// @address 0x29b7e8 ?record_kill@c_game_statborg@@QAAXJJW4e_game_team@@EM_N@Z
// @address 0x29bfd8 ?record_player_kill@c_game_statborg@@AAAXJJJE@Z
#include "game/game_time_inline.h"
#include "game/game_statborg_globals.h"
#include "memory/tls_globals.h"
#include "game/player_mapping_types.h"
#include "game/game_grief.h"
#include "units/unit_seat.h"
#include "memory/data_types.h"
#include "game/game_time_types.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_score_types.h"
#include "game/game_results_team_statistics_types.h"
#include "game/game_results_team_data_types.h"
#include "cseries/cseries.h"
#include "game/players.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "game/game_results.h"
#include "game/game_engine_notifications.h"
#include "memory/data.h"
#include "memory/datum_storage_access.h"
#include "memory/data_absolute_index.h"
#include "memory/datum_index.h"
#include "memory/data_handle_access.h"
#include "game/game_allegiance.h"
#include "units/unit_types.h"
#include "game/game_results_damage_statistics_types.h"
#include "game/game_results_player_vs_player_statistic_types.h"
#include "game/game_results_statistic_shared_types.h"
#include "game/statborg_entry_types.h"
#include "game/game_results_types.h"
#include "game/game_engine_default.h"
#include "game/game_results_statistics_types.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_list.h"
#include "game/game_results_player_data.h"
#include "game/game_results_storage_types.h"
#include <string.h>
#include "game/game_simulation_inline.h"
#include "game/game_engine_current_entry.h"
#include "objects/damage_reporting_types.h"

class c_allocation_base;

struct s_apply_stat_values {short values[11];};
struct s_apply_stat_update {s_apply_stat_values stats; bool flag;};
struct s_apply_statborg_update {s_apply_stat_update players[16],teams[8]; bool flag;};
struct s_statborg_mapping { e_statborg_entry statborg_value; e_game_results_statistic game_results_value; long engine; };
// @rdata 0x82032cb0..0x82032d58
extern "C" {
// @address 0x32cb0 k_statborg_stat_mappings
const s_statborg_mapping k_statborg_stat_mappings[14] = {
    { _statborg_entry_score, _game_results_statistic_in_round_score, 0 },
    { _statborg_entry_total_score, _game_results_statistic_in_game_total_score, 0 },
    { _statborg_entry_kills, _game_results_statistic_kills, 0 },
    { _statborg_entry_deaths, _game_results_statistic_deaths, 0 },
    { _statborg_entry_suicides, _game_results_statistic_suicides, 0 },
    { _statborg_entry_betrayals, _game_results_statistic_betrayals, 0 },
    { _statborg_entry_assists, _game_results_statistic_assists, 0 },
    { _statborg_entry_rounds_won, _game_results_statistic_rounds_won, 0 },
    { _statborg_entry_infection_time_as_human, _game_results_statistic_infection_time_as_human, 10 },
    { _statborg_entry_infection_infections, _game_results_statistic_infection_infections, 10 },
    { _statborg_entry_infection_zombie_kills, _game_results_statistic_infection_zombie_kills, 10 },
    { _statborg_entry_territories_owned, _game_results_statistic_territories_owned, 8 },
    { _statborg_entry_territories_captures, _game_results_statistic_territories_captures, 8 },
    { _statborg_entry_territories_ousts, _game_results_statistic_territories_ousts, 8 },
};
}
// @rdata-end
void c_game_statborg::stats_reset_for_round_switch() {
 for(unsigned long player_index=0;player_index<16;player_index++) {
  players[player_index].round_kills=0;
  players[player_index].round_time=0;
  players[player_index].round_deaths=0;
  players[player_index].round_score=0;
  players[player_index].round_assists=0;
  players[player_index].round_betrayals=0;
  players[player_index].round_suicides=0;
  players[player_index].round_sprees=0;
  players[player_index].round_damage=0;
  players[player_index].last_kill=-1;
  if(players[player_index].pending != 0 && !players[player_index].round_locked) {
   players[player_index].total += players[player_index].pending;
   game_results_statistic_set(player_index,_game_team_none,_game_results_statistic_in_game_total_score,players[player_index].total);
   if((tls_game_globals()->game_finished != 0)) players[player_index].round_locked=1;
   else players[player_index].pending=0;
   simulation_action_game_statborg_update(1 << player_index);
  }
 }
 for(unsigned long team_index=0;team_index<8;team_index++) {
  if(teams[team_index].pending != 0 && !teams[team_index].round_locked) {
   teams[team_index].total += teams[team_index].pending;
   game_results_statistic_set(-1,(e_game_team)team_index,_game_results_statistic_in_game_total_score,teams[team_index].total);
   if((tls_game_globals()->game_finished != 0)) teams[team_index].round_locked=1;
   else teams[team_index].pending=0;
   simulation_action_game_statborg_update(1 << (team_index+16));
  }
 }
 for(long index=0;index<2;index++) g_game_engine_score_cache[index].time=-1;
}
void c_game_statborg::stats_promote_to_authority() {
 const c_game_results *results=&g_current_game_results;
 if(results->summary_valid) {
  long engine_type=0;
  if(current_game_engine_entry()) engine_type=current_game_engine()->get_type();
  for(long player_index=0;player_index<16;player_index++) {
   const s_game_results_player_data *player=&results->players[player_index];
   if(player->player_exists && !(update_mask & (1 << player_index))) {
    const s_game_results_player_statistics *statistics=&results->statistics.player[player_index];
    unsigned long mapping_index=0;
    for(const s_statborg_mapping *mapping=k_statborg_stat_mappings;mapping_index<14;mapping_index++,mapping++) {
     if(mapping->engine==0 || mapping->engine==engine_type) {
      short value=statistics->statistics[mapping->game_results_value].value;
      set_player_stat(player_index,mapping->statborg_value,value);
     }
    }
   }
  }
  for(unsigned long team_index=0;team_index<8;team_index++) {
   const s_game_results_team_data *team=&results->teams.team[team_index];
   if(team->team_exists && !(update_mask & (1 << (team_index+16)))) {
    const s_game_results_team_statistics *statistics=&results->statistics.team[team_index];
    unsigned long mapping_index=0;
    for(const s_statborg_mapping *mapping=k_statborg_stat_mappings;mapping_index<14;mapping_index++,mapping++) {
     if(mapping->engine==0 || mapping->engine==engine_type) {
      short value=statistics->statistics[mapping->game_results_value].value;
      set_team_stat(team_index,mapping->statborg_value,value);
     }
    }
   }
  }
 }
 simulation_action_game_statborg_update(0x1ffffff);
}
static __forceinline bool game_statborg_game_has_teams()
{
 return current_game_engine_entry() != 0 && (tls_game_engine_globals()->flags & 8) != 0;
}
void c_game_statborg::build_statborg_update(unsigned long *mask, long size, void *out)
{
 unsigned long changed = 0;
 for (unsigned long player = 0; player < 16; player++) {
  unsigned long bit = 1 << player;
  if (*mask & bit) {
   bool different = memcmp(m_player_stats[player], ((s_statborg_update *)out)->players[player].values,
                        sizeof(((s_statborg_update *)out)->players[player].values)) != 0
    || (unsigned char)players[player].round_locked != ((s_statborg_update *)out)->players[player].flag;
   if (different) {
    memcpy(((s_statborg_update *)out)->players[player].values, m_player_stats[player], sizeof(((s_statborg_update *)out)->players[player].values));
    ((s_statborg_update *)out)->players[player].flag = (unsigned char)players[player].round_locked;
   }
   if (different) changed |= bit; else changed &= ~bit;
  }
 }
 if (game_statborg_game_has_teams()) {
  for (long team = 0; team < 8; team++) {
   if (*mask & (1 << (team + 16))) {
    bool different = memcmp(m_team_stats[team], ((s_statborg_update *)out)->teams[team].values, sizeof(((s_statborg_update *)out)->teams[team].values)) != 0
     || (unsigned char)teams[team].round_locked != ((s_statborg_update *)out)->teams[team].flag;
    if (different) {
     memcpy(((s_statborg_update *)out)->teams[team].values, m_team_stats[team], sizeof(((s_statborg_update *)out)->teams[team].values));
     ((s_statborg_update *)out)->teams[team].flag = (unsigned char)teams[team].round_locked;
    }
    if (different) changed |= 1 << (team + 16); else changed &= ~(1 << (team + 16));
   }
  }
 }
 if (*mask & (1 << 24)) {
  bool different = ((s_statborg_update *)out)->flag != flag;
  if (different) ((s_statborg_update *)out)->flag = flag;
  if (different) changed |= 1 << 24; else changed &= ~(1 << 24);
 }
 *mask = changed;
}
void c_game_statborg::adjust_player_stat(long player_index, e_statborg_entry statistic,
    short adjustment, long game_results_statistic, bool adjust_team)
{
    long absolute_player_index = (unsigned short)player_index;
    short &player_stat = m_player_stats[absolute_player_index][statistic];
    player_stat = (short)MIN(MAX(player_stat + adjustment, -30000), 30000);
    simulation_action_game_statborg_update(1 << absolute_player_index);
    if (game_results_statistic != -1)
        game_results_statistic_set(absolute_player_index, _game_team_none, (e_game_results_statistic)game_results_statistic, player_stat);
    if (adjust_team && current_game_engine_entry() && game_engine_has_teams()) {
        player_datum *player = (player_datum *)datum_try_and_get_absolute(tls_player_data(), absolute_player_index);
        if (player && player->configuration.host.team_index >= 0 && player->configuration.host.team_index < 8)
            adjust_team_stat((e_game_team)player->configuration.host.team_index, statistic, adjustment, game_results_statistic);
    }
}

void c_game_statborg::adjust_team_stat(e_game_team team_index,e_statborg_entry statistic,short adjustment,long game_results_statistic) {
 m_team_stats[team_index][statistic]=(short)MIN(MAX(m_team_stats[team_index][statistic]+adjustment,-30000),30000);
 simulation_action_game_statborg_update(1 << (team_index+16));
 if (game_results_statistic != -1)
     game_results_statistic_set(-1, team_index, (e_game_results_statistic)game_results_statistic, m_team_stats[team_index][statistic]);
}

static __forceinline long game_statborg_results_statistic_get_current(long player,long team,long statistic)
{
 long result=-1;
 if(g_game_results_globals.active) {
  if(player!=-1) result=g_current_game_results.statistics.player[player].statistics[statistic].value;
  else if((unsigned long)team<16) result=g_current_game_results.statistics.team[team].statistics[statistic].value;
 }
 return result;
}
void c_game_statborg::record_player_kill(long killer_index, long victim_index, long multikill_time, unsigned char damage_info)
{
 long damage_type=damage_info>>2;
 long damage_modifier=damage_info&3;
 player_datum *victim=datum_get((player_datum *)tls_player_data()->data, victim_index);
 player_datum *killer=datum_get((player_datum *)tls_player_data()->data, killer_index);
 long absolute=(unsigned short)killer_index;
 adjust_player_stat(killer_index,_statborg_entry_kills,1,8,true);
 game_results_damage_statistic_increment(absolute,0,damage_type,1);
 if((damage_info&3)==1) game_results_damage_statistic_increment(absolute,4,damage_type,1);
 short current_spree=(short)game_statborg_results_statistic_get_current(absolute,-1,13);
 stats[absolute].killing_spree++;
 if(current_spree<stats[absolute].killing_spree)
  game_results_statistic_set(absolute,_game_team_none,_game_results_statistic_most_kills_in_a_row,stats[absolute].killing_spree);
 if(game_engine_has_teams() && killer->configuration.host.team_index>=0 && killer->configuration.host.team_index<8) {
  short current_team_spree=(short)game_statborg_results_statistic_get_current(-1,killer->configuration.host.team_index,13);
  if(current_team_spree<stats[absolute].killing_spree)
   game_results_statistic_set(-1,(e_game_team)killer->configuration.host.team_index,
      _game_results_statistic_most_kills_in_a_row,stats[absolute].killing_spree);
 }
 if(stats[absolute].last_kill_time>=multikill_time) {
  stats[absolute].multikill_count++;
  if(game_engine_has_teams() && victim->configuration.host.team_index>=0 && victim->configuration.host.team_index<8) {
   if(victim->configuration.host.team_index==stats[absolute].last_team)
    stats[absolute].team_kills++;
   else {
    stats[absolute].team_kills=1;
    stats[absolute].last_team=(short)victim->configuration.host.team_index;
   }
  } else {
   stats[absolute].team_kills=0;
   stats[absolute].last_team=-1;
  }
 } else {
  stats[absolute].multikill_count=1;
  if(game_engine_has_teams() && victim->configuration.host.team_index>=0 && victim->configuration.host.team_index<8) {
   stats[absolute].team_kills=1;
   stats[absolute].last_team=(short)victim->configuration.host.team_index;
  } else {
   stats[absolute].team_kills=0;
   stats[absolute].last_team=-1;
  }
 }
 if(damage_type!=stats[absolute].last_damage) {
  stats[absolute].last_damage=damage_type;
  stats[absolute].weapon_streak[0]=0;
  stats[absolute].weapon_streak[1]=0;
  stats[absolute].weapon_streak[2]=0;
  stats[absolute].weapon_streak[3]=0;
 }
 switch(damage_type) {
 case _damage_reporting_type_sniper_rifle:
 case _damage_reporting_type_beam_rifle:
  if(damage_modifier==0 || damage_modifier==1) stats[absolute].weapon_streak[0]++;
  break;
 case _damage_reporting_type_13: stats[absolute].weapon_streak[1]++; break;
 case _damage_reporting_type_25: stats[absolute].weapon_streak[2]++; break;
 default:
  if(damage_type==2 || damage_modifier==3) stats[absolute].weapon_streak[3]++;
  break;
 }
 stats[absolute].last_kill_time=game_time_get();
 for(long i=0;i<2;i++) g_game_engine_score_cache[i].time=-1;
 tls_game_engine_globals()->flags|=1;
}
static __forceinline void game_statborg_scoring_leaders_make_dirty()
{
 for(long i=0;i<2;i++) g_game_engine_score_cache[i].time=-1;
 tls_game_engine_globals()->flags|=1;
}
// @address 0x29c318 ?record_player_death@c_game_statborg@@AAAXJJ_N0E@Z
void c_game_statborg::record_player_death(long player_index, long killer_player_index,
    bool suicide, bool betrayal, unsigned char damage_reporting_info)
{
 int absolute_killer_index;
 absolute_killer_index = killer_player_index & 65535;
 int absolute_player_index;
 absolute_player_index = player_index & 65535;
 unsigned int damage_type;
 damage_type = (unsigned int)damage_reporting_info >> 2 & 63;
 adjust_player_stat(player_index, _statborg_entry_deaths, 1, 10, true);
 game_results_damage_statistic_increment(absolute_player_index, 1, damage_type, 1);
 s_statborg_player *player_stats;
 if (suicide != 0) {
  adjust_player_stat(player_index, _statborg_entry_suicides, 1, 12, true);
  game_results_damage_statistic_increment(absolute_player_index, 3, damage_type, 1);
 } else if (betrayal != 0 && killer_player_index != -1) {
  if (tls_player_mapping_globals()->player_input_user_mapping[absolute_killer_index] != -1) {
   game_grief_record_betrayal(killer_player_index);
  }
  adjust_player_stat(killer_player_index, _statborg_entry_betrayals, 1, 11, true);
  game_results_damage_statistic_increment(absolute_killer_index, 2, damage_type, 1);
  player_datum *player = &((player_datum *)tls_player_data()->data)[absolute_player_index];
  if (player->respawn_player_index == -1) {
   player->respawn_player_index = killer_player_index;
   ::simulation_action_game_engine_player_update((short)player_index, 1024);
  }
 }
 player_stats = &players[absolute_player_index];
 player_stats->round_kills = 0;
 player_stats->round_deaths = 0;
 player_stats->round_score = 0;
 player_stats->round_time = -1;
 player_stats->round_assists = 0;
 player_stats->round_betrayals = 0;
 player_stats->round_suicides = 0;
 player_stats->round_sprees = 0;
 player_stats->round_damage = 0;
 player_stats->last_kill = (short)-1;
 game_statborg_scoring_leaders_make_dirty();
 return;
}
static __forceinline void *apply_game_statborg_current_game_engine()
{
    long index = tls_game_engine_globals()->game_engine_index;
    if (index < 1)
        return 0;
    if (index > 10)
        return 0;
    return (void *)game_engines[index];
}
static __forceinline bool apply_game_has_teams() {return apply_game_statborg_current_game_engine()!=0 && (tls_game_engine_globals()->flags&8)!=0;}
bool c_game_statborg::apply_statborg_update(unsigned long mask,long size,const void *data) {
 const s_apply_statborg_update *update=(const s_apply_statborg_update *)data;
 for(long i=0;i<16;i++) {
  if(mask&(1<<i)) {
   long betrayals=update->players[i].stats.values[5]-m_player_stats[i][5];
   if(betrayals>0) {
    long player=player_try_and_get_player_index_from_absolute_player_index(i);
    if(player!=-1 && tls_player_mapping_globals()->player_input_user_mapping[datum_index_to_absolute_index(player)]!=-1) {
     while(betrayals--) game_grief_record_betrayal(player);
    }
   }
   memcpy(m_player_stats[i],&update->players[i].stats,sizeof(s_apply_stat_values));
   players[i].round_locked=update->players[i].flag;
  }
 }
 if(apply_game_has_teams()) {
  for(long team=0;team<8;team++) if(mask&(1<<(team+16))) {
   memcpy(m_team_stats[team],&update->teams[team].stats,sizeof(s_apply_stat_values));
   teams[team].round_locked=update->teams[team].flag;
  }
 }
 if(mask&(1<<24)) flag=update->flag;
 if(!game_is_authoritative()) update_mask |= mask;
 return true;
}
// Local checked-datum lookup: calling the shared helper directly perturbs
// link-time register allocation in record_kill, so this unit keeps its own.
static __forceinline void *statborg_datum_try_and_get(const s_data_array *data, long datum_index)
{
 void *result=0;
 if(datum_index!=-1) {
  int absolute_index=datum_index&0xffff;
  if(absolute_index<data->first_unallocated) {
   short *datum=(short *)(data->data+data->size*absolute_index);
   int identifier=*datum;
   if(identifier!=0 && identifier==(datum_index>>16))
    result=datum;
  }
 }
 return result;
}
static __forceinline long game_statborg_player_index_from_unit_index(long unit_index)
{
 unit_datum *unit=(unit_datum *)object_try_and_get_and_verify_type(unit_index,0x1003);
 long player_index=-1;
 if(unit)
  player_index=unit->unit.player_index;
 return player_index;
}
static __forceinline long statborg_object_get_ultimate_parent(long object_index)
{
 long parent_index;
 do {
  parent_index=object_index;
  object_index=((unit_datum *)datum_get<s_object_header>(tls_object_header_data(), object_index)->datum)->object.parent_object_index;
 } while(object_index!=-1);
 return parent_index;
}
void c_game_statborg::record_kill(long unit_index,long killer_player_index,e_game_team killer_team,
    unsigned char damage_reporting_info,float damage,bool flag)
{
 long dead_player_index=game_statborg_player_index_from_unit_index(unit_index);
 e_game_team dead_team=_game_team_none;
 e_game_team responsible_team=_game_team_none;
 long responsible_player_index=-1;
 long responsible_unit_index=-1;
 unsigned char final_damage_reporting_info=damage_reporting_info;
 bool betrayal_known=false;
 bool betrayal_cached=false;
 if(dead_player_index!=-1)
  dead_team=(e_game_team)datum_get<player_datum>(tls_player_data(), dead_player_index)->configuration.host.team_index;
 long kill_window=game_time_get()-game_seconds_to_ticks_round(4.0f);
 long assist_window=game_time_get()-game_seconds_to_ticks_round(6.0f);
 unit_datum *unit=(unit_datum *)datum_get<s_object_header>(tls_object_header_data(), unit_index)->datum;
 short killer_recent_index=-1;
 bool consider_enemies=true;
 if(killer_player_index!=-1 && game_team_is_enemy(dead_team,killer_team))
  consider_enemies=false;
 short best_recent_index=-1;
 float best_value=-3.402823466e+38f;
 short recent_index;
 s_unit_damage_record *recent;
 for(recent_index=0,recent=unit->unit.damage_records;recent_index<4;recent_index++,recent++) {
  if(recent->player_index!=-1) {
   bool valid=false;
   if(recent->player_index==killer_player_index) {
    killer_recent_index=recent_index;
    valid=true;
   } else if(consider_enemies) {
    player_datum *player=(player_datum *)datum_try_and_get(tls_player_data(),recent->player_index);
    valid=player && game_team_is_enemy(dead_team,(e_game_team)player->configuration.host.team_index);
   }
   if(valid && recent->time>(unsigned long)assist_window && recent->damage>best_value) {
    best_recent_index=recent_index;
    best_value=recent->damage;
   }
  }
 }
 if(best_recent_index==-1)
  best_recent_index=killer_recent_index;
 bool betrayal=false;
 float score=0.0f;
 long assist_player_count=0;
 long assist_player_indices[5];
 if(best_recent_index!=-1) {
  s_unit_damage_record *best=&unit->unit.damage_records[best_recent_index];
  final_damage_reporting_info=unit->unit.damage_record_flags[best_recent_index];
  if(statborg_datum_try_and_get(tls_player_data(),best->player_index)) {
   responsible_player_index=best->player_index;
   responsible_unit_index=best->object_index;
   score=0.4f*best->damage;
  }
 }
 if(responsible_player_index!=-1 && responsible_player_index==killer_player_index)
  final_damage_reporting_info=damage_reporting_info;
 if(responsible_player_index!=-1 && responsible_player_index!=killer_player_index)
  final_damage_reporting_info=0;
 if(responsible_player_index==-1 && statborg_datum_try_and_get(tls_player_data(),killer_player_index)) {
  responsible_player_index=killer_player_index;
  final_damage_reporting_info=damage_reporting_info;
  score=0.4f*(damage<1.0f?1.0f:damage);
 }
 if(responsible_player_index!=-1) {
  player_datum *responsible=datum_get<player_datum>(tls_player_data(), responsible_player_index);
  responsible_team=(e_game_team)responsible->configuration.host.team_index;
  if(game_team_is_enemy(dead_team,(e_game_team)responsible->configuration.host.team_index) && dead_player_index!=-1) {
   game_results_insert_kill_event(responsible_player_index,dead_player_index,final_damage_reporting_info);
   record_player_kill(responsible_player_index,dead_player_index,kill_window,final_damage_reporting_info);
  } else {
   betrayal=true;
  }
 }
 if(score>0.0f && dead_player_index!=-1) {
  short assist_index;
  s_unit_damage_record *assist;
  for(assist_index=0,assist=unit->unit.damage_records;assist_index<4;assist_index++,assist++) {
   if(assist->player_index!=-1) {
    player_datum *player=(player_datum *)statborg_datum_try_and_get(tls_player_data(),assist->player_index);
    if (player && assist->player_index != responsible_player_index &&
        (assist->player_index == killer_player_index || assist->damage > score) &&
        game_team_is_enemy(dead_team, (e_game_team)player->configuration.host.team_index))
     assist_player_indices[assist_player_count++]=assist->player_index;
   }
  }
 }
 if(responsible_player_index!=-1 && dead_player_index!=-1) {
  player_datum *responsible=datum_get<player_datum>(tls_player_data(), responsible_player_index);
  if(game_team_is_enemy(dead_team,(e_game_team)responsible->configuration.host.team_index) && responsible->unit_index!=-1) {
   long ultimate_parent=statborg_object_get_ultimate_parent(responsible->unit_index);
   if(((1<<datum_get<s_object_header>(tls_object_header_data(), ultimate_parent)->object_type)&0x1003)!=0) {
    unit_datum *parent_unit=(unit_datum *)datum_get<s_object_header>(tls_object_header_data(), ultimate_parent)->datum;
    if(parent_unit->unit.driver_object_index!=-1 && parent_unit->unit.driver_object_index!=responsible->unit_index) {
     unit_datum *aiming_unit=(unit_datum *)datum_get<s_object_header>(tls_object_header_data(), parent_unit->unit.driver_object_index)->datum;
     if(aiming_unit->unit.player_index!=-1) {
      player_datum *aiming_player=datum_get<player_datum>(tls_player_data(), aiming_unit->unit.player_index);
      if(game_team_is_enemy(dead_team,(e_game_team)aiming_player->configuration.host.team_index) && (unsigned long)assist_player_count<5)
       assist_player_indices[assist_player_count++]=aiming_unit->unit.player_index;
     }
    }
   }
  }
 }
 if(dead_player_index!=-1) {
  if(tls_game_globals()->options.game_mode==2) {
   const c_game_engine *engine=current_game_engine_entry();
   if(engine && engine->get_type()==7) {
    betrayal_known=true;
    betrayal_cached=!game_team_is_enemy(responsible_team,dead_team);
   }
  }
  game_engine_notify_player_killed(responsible_player_index, responsible_unit_index, dead_player_index, betrayal,
      final_damage_reporting_info, responsible_player_index == killer_player_index);
 }
 if(dead_player_index!=-1 && responsible_player_index!=-1) {
  long responsible_absolute_index=(unsigned short)responsible_player_index;
  long dead_absolute_index=(unsigned short)dead_player_index;
  game_results_player_vs_player_statistic_increment(responsible_absolute_index,dead_absolute_index,
      _game_results_player_vs_player_statistic_kills,1);
  game_results_player_vs_player_statistic_increment(dead_absolute_index,responsible_absolute_index,
      _game_results_player_vs_player_statistic_deaths,1);
 }
 for(long i=0;i<assist_player_count;i++) {
  bool already_credited=false;
  for(long j=0;j<i;j++) {
   if(assist_player_indices[i]==assist_player_indices[j]) {
    already_credited=true;
    break;
   }
  }
  if(!already_credited) {
   long assist_player_index=assist_player_indices[i];
   adjust_player_stat(assist_player_index,_statborg_entry_assists,1,9,true);
   game_statborg_scoring_leaders_make_dirty();
   const c_game_engine *engine=current_game_engine_entry();
   if(engine)
    engine->player_assisted_with_kill(assist_player_index);
  }
 }
 if(dead_player_index!=-1) {
  bool suicide=responsible_player_index==dead_player_index;
  bool is_betrayal=false;
  if(!flag) {
   if(betrayal_known)
    is_betrayal=betrayal_cached;
   else
    is_betrayal=!game_team_is_enemy(responsible_team,dead_team);
  }
  record_player_death(dead_player_index,responsible_player_index,suicide,is_betrayal,final_damage_reporting_info);
 }
}
