// blamlibxbox/game_engine_scripting.obj: from the records 0x29ab28 0x29ac18
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/game_globals_storage.h"
#include "game/game_simulation_predicates.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_scripting.h"
#include "memory/tls_globals.h"
#include "game/player_types.h"
#include "game/player_in_game_iterator.h"
#include "game/game_engine_scoring_globals.h"
#include "memory/data_types.h"
#include "objects/object_datum.h"
// @address 0x29ab28 game_engine_respawn_override_timers
extern "C" void game_engine_respawn_override_timers(short team) {
 if(game_simulation::game_is_authoritative() && team>=0 && team<8) {
  c_player_in_game_iterator iterator;
  iterator.m_iterator.m_iterator.data=tls_player_data();
  iterator.m_iterator.m_iterator.absolute_index=-1;
  iterator.m_iterator.m_iterator.index=-1;
  while(iterator.next()) {
   player_datum *player=iterator.m_iterator.m_datum;
   if(player->configuration.host.team_index==team && player->respawn_timer>0) {
    long absolute_index=iterator.m_iterator.m_iterator.index & 65535;
    s_game_engine_player_waypoint *waypoint=&tls_game_engine_globals()->player_waypoints[absolute_index];
    player->respawn_timer=0;
    waypoint->active=false;
    ::simulation_action_game_engine_player_update((short)absolute_index, 1);
   }
  }
 }
}
void game_engine_give_object_ownership_to_team(long object_index,short team_index) {
 if(game_simulation::game_is_authoritative() && object_index != -1 && (team_index == -1 || (team_index >= 0 && team_index < 8))) {
  object_datum *object=(object_datum *)object_try_and_get_and_verify_type(object_index,(unsigned long)-1);
  if(object) {
   object->owner_team_index=team_index;
   ::simulation_action_object_update(object_index, 1024);
  }
 }
}
