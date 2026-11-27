// blamlibxbox/game_results.obj: from the records 0xd3f88 0x244308 0x244720 0x244b10 0x244bb8 0x244d00 0x244e28 0x244ea0 0x244f60 0x245008 0x2450e0 0x245168 0x245250 0x245320 0x2453f8 0x2459e8 0x245fb8 0x2467b8 0x246e60 0x246f70 0x247028 0x2470a0 0x247160 0x247260 0x2472f8 0x2473e8 0x247518 0x247650 0x247720 0x247838 0x247d80 0x24a710 0x2a64c8 0x2f45f0
// @flags /TP /O2 /Oc
#include "game/game_results_externs.h"
#include "game/game_results_player_data.h"
#include "game/game_results_replicator.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_infection_variant.h"
#include <time.h>
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "game/game_results_event_carry_types.h"
#include "game/game_achievements_types.h"
#include "game/game_achievements.h"
#include "game/game_results_statistic_globals.h"
#include "game/game_results_medal_info.h"
#include "memory/data_types.h"
#include <stdlib.h>
#include "math/real_math_types.h"
#include "memory/bitstream.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "game/game_options.h"
#include "shell/shell.h"
#include "game/game_results_team_statistics_types.h"
#include "game/game_results_team_data_types.h"
#include <string.h>
#include <stddef.h>
#include "cseries/cseries.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "objects/objects.h"
#include "game/game_engine_scoring.h"
#include "game/game_engine_score_types.h"
#include "game/game_engine_util.h"
#include "game/game_results.h"
#include "networking/session/network_session.h"
#include "game/game_engine_team.h"
#include "networking/session/network_session_membership.h"
#include "simulation/simulation_globals.h"
#include "game/game_results_damage_statistic_types.h"
#include "game/game_results_damage_type_description_types.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/network_globals.h"
#include "game/game_results_damage_statistics_types.h"
#include "game/game_results_player_vs_player_statistic_types.h"
#include "game/game_results_damage_breakdown_types.h"
#include "game/game_results_statistic_shared_types.h"
#include "interface/user_interface_session.h"
#include "game/game_results_statistics_types.h"
#include "xbox/xnet.h"
#include "game/game_results_statistic_types.h"
#include "game/game_results_types.h"
#include "networking/session/network_session_types.h"
#include "simulation/simulation_view.h"
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_watcher_types.h"
#include "game/game_results_storage_types.h"
#include "game/players_globals.h"
#include "memory/data_absolute_index.h"
#include "game/game_results_finalized.h"
#include "memory/gameplay_tls_globals.h"
#include "game/game_results_network_statistics.h"
#include "game/game_engine_infection_globals.h"
#include "game/game_results_incremental_storage.h"
#include "game/game_results_packet.h"
#include "xbox/xam_user.h"
#include "networking/session/network_session_parameter_observer.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_results_incremental_update.h"
#include "memory/datum_index.h"
// @address 0x952d8 ?user_interface_get_interactive_session@@YA_NPAPAVc_network_session@@@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0x2a6760 ?game_conditions_allow_achievement@@YA_NJW4e_achievement@@@Z
// @address 0x208c38 ?game_engine_adjust_player_score@@YA_NJJ@Z
// @address 0x247650 game_results_grant_top_player_achievement
// @address 0xa33f80 ?g_current_game_results@@3Vc_game_results@@A
// @address 0x8c4044 ?g_game_results_globals@@3U0@A
// @address 0x247838 game_results_sort_players_by_statistics
// @address 0x247518 game_results_grant_achievement_to_owner
// @address 0x2a6618 ?game_grant_achievement_to_local_player@@YAXW4e_controller_index@@W4e_achievement@@_N@Z
// @address 0x244bb8 ?game_results_statistic_increment@@YAXJW4e_game_team@@W4e_game_results_statistic@@J@Z
// @address 0x244d00 ?game_results_statistic_set@@YAXJW4e_game_team@@W4e_game_results_statistic@@J@Z
// @address 0x244f60 ?game_results_player_vs_player_statistic_increment@@YAXJJW4e_game_results_player_vs_player_statistic@@J@Z
// @address 0x247028 ?get_machine_index@c_game_results@@QBAJPBUs_machine_identifier@@@Z
// @address 0x2470a0 ?get_host_machine_index@c_game_results@@QBAJXZ
// @address 0x247160 ?fixup_player_machine_references@c_game_results@@QAAXXZ
// @address 0x247260 ?game_results_count_damage_statistic@@YAJJW4e_game_results_damage_statistic@@W4e_game_results_damage_breakdown_type@@@Z
// @address 0x2472f8 ?game_results_get_maximum_rounds_completed@@YAJXZ
// @address 0x2f45f0 game_results_medal_increment
// @address 0x2a64c8 game_results_try_and_grant_achievement

// @address 0x7450e0 ?g_game_results_statistic_definitions@@3PAUs_integer_statistic_definition@@A
s_integer_statistic_definition g_game_results_statistic_definitions[k_game_results_statistic_count] = {
    { "games-played", 0, 0, 1, 1 },
    { "games-completed", 0, 0, 1, 1 },
    { "games-won", 0, 0, 1, 1 },
    { "games-tied", 0, 0, 1, 1 },
    { "rounds-completed", 0, 0, 32, 6 },
    { "rounds-won", 0, 0, 32, 6 },
    { "in-round-score", 0, 0, 1023, 10 },
    { "in-game-total-score", 0, 0, 1023, 10 },
    { "kills", 0, 0, 1023, 10 },
    { "assists", 0, 0, 1023, 10 },
    { "deaths", 0, 0, 1023, 10 },
    { "betrayals", 0, 0, 1023, 10 },
    { "suicides", 0, 0, 1023, 10 },
    { "most-kills-in-a-row", 0, 0, 255, 8 },
    { "seconds-alive", 0, 0, 32767, 15 },
    { "ctf-flag-scores", 0, 0, 1023, 10 },
    { "ctf-flag-grabs", 0, 0, 1023, 10 },
    { "ctf-flag-carrier-kills", 0, 0, 1023, 10 },
    { "ctf-flag-returns", 0, 0, 1023, 10 },
    { "assault-bomb-arms", 0, 0, 1023, 10 },
    { "assault-bomb-grabs", 0, 0, 1023, 10 },
    { "assault-bomb-disarms", 0, 0, 1023, 10 },
    { "assault-bomb-detonations", 0, 0, 1023, 10 },
    { "oddball-time-with-ball", 0, 0, 32767, 15 },
    { "oddball-unused", 0, 0, 1023, 10 },
    { "oddball-kills-as-carrier", 0, 0, 32767, 15 },
    { "oddball-ball-carrier-kills", 0, 0, 1023, 10 },
    { "king-time-on-hill", 0, 0, 32767, 15 },
    { "king-total-control-time", 0, 0, 32767, 15 },
    { "king-unused", 0, 0, 1023, 10 },
    { "king-unused", 0, 0, 32767, 15 },
    { "unused", 0, 0, 1023, 10 },
    { "unused", 1, 0, 32767, 15 },
    { "unused", 1, 0, 32767, 15 },
    { "vip-takedowns", 0, 0, 1023, 10 },
    { "vip-kills-as-vip", 0, 0, 1023, 10 },
    { "vip-guard-time", 0, 0, 32767, 15 },
    { "vip-time-as-vip", 0, 0, 32767, 15 },
    { "vip-lives-as-vip", 0, 0, 1023, 10 },
    { "juggernaut-kills", 0, 0, 1023, 10 },
    { "juggernaut-kills-as-juggernaut", 0, 0, 1023, 10 },
    { "juggernaut-total-control-time", 0, 0, 32767, 15 },
    { "juggernaut-unused", 0, 0, 1023, 10 },
    { "juggernaut-unused", 2, 0, 32767, 15 },
    { "territories-owned", 0, 0, 1023, 10 },
    { "territories-captures", 0, 0, 1023, 10 },
    { "territories-ousts", 0, 0, 1023, 10 },
    { "territories-time-in-territory", 0, 0, 1023, 10 },
    { "infection-zombie-kills", 0, 0, 1023, 10 },
    { "infection-infections", 0, 0, 1023, 10 },
    { "infection-time-as-human", 0, 0, 32767, 15 },
};

// @address 0x745410 ?g_game_results_damage_statistic_definitions@@3PAUs_integer_statistic_definition@@A
s_integer_statistic_definition g_game_results_damage_statistic_definitions[5] = {
    { "damage-kills", 0, 0, 1023, 10 },
    { "damage-deaths", 0, 0, 1023, 10 },
    { "damage-betrayals", 0, 0, 1023, 10 },
    { "damage-suicides", 0, 0, 1023, 10 },
    { "damage-headshots", 0, 0, 32767, 15 },
};

// @address 0x745460 ?g_game_results_player_vs_player_statistic_definitions@@3PAUs_integer_statistic_definition@@A
s_integer_statistic_definition g_game_results_player_vs_player_statistic_definitions[k_game_results_player_vs_player_statistic_count] = {
    { "player-vs-player-kills", 0, 0, 1023, 10 },
    { "player-vs-player-deaths", 0, 0, 1023, 10 },
};

// @address 0x745480 ?g_game_results_medal_statistic_definitions@@3PAUs_integer_statistic_definition@@A
s_integer_statistic_definition g_game_results_medal_statistic_definitions[58] = {
    { "medeal-extermination", 0, 0, 1023, 10 },
    { "medeal-perfection", 0, 0, 1023, 10 },
    { "medeal-multiple-kill-2", 0, 0, 1023, 10 },
    { "medeal-multiple-kill-3", 0, 0, 1023, 10 },
    { "medeal-multiple-kill-4", 0, 0, 1023, 10 },
    { "medeal-multiple-kill-5", 0, 0, 1023, 10 },
    { "medeal-multiple-kill-6", 0, 0, 1023, 10 },
    { "medeal-multiple-kill-7", 0, 0, 1023, 10 },
    { "medeal-multiple-kill-8", 0, 0, 1023, 10 },
    { "medeal-multiple-kill-9", 0, 0, 1023, 10 },
    { "medeal-multiple-kill-10", 0, 0, 1023, 10 },
    { "medeal-5-kills-in-a-row", 0, 0, 1023, 10 },
    { "medeal-10-kills-in-a-row", 0, 0, 1023, 10 },
    { "medeal-15-kills-in-a-row", 0, 0, 1023, 10 },
    { "medeal-20-kills-in-a-row", 0, 0, 1023, 10 },
    { "medeal-25-kills-in-a-row", 0, 0, 1023, 10 },
    { "medeal-30-kills-in-a-row", 0, 0, 1023, 10 },
    { "medeal-sniper-kill-5", 0, 0, 1023, 10 },
    { "medeal-sniper-kill-10", 0, 0, 1023, 10 },
    { "medeal-shotgun-kill-5", 0, 0, 1023, 10 },
    { "medeal-shotgun-kill-10", 0, 0, 1023, 10 },
    { "medeal-collision-kill-5", 0, 0, 1023, 10 },
    { "medeal-collision-kill-10", 0, 0, 1023, 10 },
    { "medeal-sword-kill-5", 0, 0, 1023, 10 },
    { "medeal-sword-kill-10", 0, 0, 1023, 10 },
    { "medeal-juggernaut-kill-5", 0, 0, 1023, 10 },
    { "medeal-juggernaut-kill-10", 0, 0, 1023, 10 },
    { "medeal-zombie-kill-5", 0, 0, 1023, 10 },
    { "medeal-zombie-kill-10", 0, 0, 1023, 10 },
    { "medeal-human-kill-5", 0, 0, 1023, 10 },
    { "medeal-human-kill-10", 0, 0, 1023, 10 },
    { "medeal-human-kill-15", 0, 0, 1023, 10 },
    { "medeal-koth-kill-5", 0, 0, 1023, 10 },
    { "medeal-shotgun-kill-sword", 0, 0, 1023, 10 },
    { "medeal-vehicle-impact-kill", 0, 0, 1023, 10 },
    { "medeal-vehicle-hijack", 0, 0, 1023, 10 },
    { "medeal-aircraft-hijack", 0, 0, 1023, 10 },
    { "medeal-deadplayer-kill", 0, 0, 1023, 10 },
    { "medeal-player-kill-spreeplayer", 0, 0, 1023, 10 },
    { "medeal-spartanlaser-kill", 0, 0, 1023, 10 },
    { "medeal-stickygrenade-kill", 0, 0, 1023, 10 },
    { "medeal-sniper-kill", 0, 0, 1023, 10 },
    { "medeal-bashbehind-kill", 0, 0, 1023, 10 },
    { "medeal-bash-kill", 0, 0, 1023, 10 },
    { "medeal-flame-kill", 0, 0, 1023, 10 },
    { "medeal-driver-assist-gunner", 0, 0, 1023, 10 },
    { "medeal-assault-bomb-planted", 0, 0, 1023, 10 },
    { "medeal-assault-player-kill-carrier", 0, 0, 1023, 10 },
    { "medeal-vip-player-kill-vip", 0, 0, 1023, 10 },
    { "medeal-juggernaut-player-kill-juggernaut", 0, 0, 1023, 10 },
    { "medeal-oddball-carrier-kill-player", 0, 0, 1023, 10 },
    { "medeal-ctf-flag-captured", 0, 0, 1023, 10 },
    { "medeal-ctf-flag-player-kill-carrier", 0, 0, 1023, 10 },
    { "medeal-ctf-flag-carrier-kill-player", 0, 0, 1023, 10 },
    { "medeal-infection-survive", 0, 0, 1023, 10 },
    { "unused1", 0, 0, 1023, 10 },
    { "unused2", 0, 0, 1023, 10 },
    { "unused3", 0, 0, 1023, 10 },
};

// @address 0x745820 ?g_game_results_achievement_statistic_definitions@@3PAUs_integer_statistic_definition@@A
s_integer_statistic_definition g_game_results_achievement_statistic_definitions[49] = {
    { "_achievement_complete_mission_jungle", 2, 0, 1, 1 },
    { "_achievement_complete_mission_base", 2, 0, 1, 1 },
    { "_achievement_complete_mission_outskirts", 2, 0, 1, 1 },
    { "_achievement_complete_mission_voi", 2, 0, 1, 1 },
    { "_achievement_complete_mission_floodvoi", 2, 0, 1, 1 },
    { "_achievement_complete_mission_wasteland", 2, 0, 1, 1 },
    { "_achievement_complete_mission_citadel", 2, 0, 1, 1 },
    { "_achievement_complete_mission_highcharity", 2, 0, 1, 1 },
    { "_achievement_complete_mission_halo", 2, 0, 1, 1 },
    { "_achievement_complete_campaign_normal", 2, 0, 1, 1 },
    { "_achievement_complete_campaign_heroic", 2, 0, 1, 1 },
    { "_achievement_complete_campaign_legendary", 2, 0, 1, 1 },
    { "_achievement_complete_access_all_terminals", 2, 0, 1, 1 },
    { "_achievement_campaign_metagame_guerilla", 2, 0, 1, 1 },
    { "_achievement_campaign_metagame_demon", 2, 0, 1, 1 },
    { "_achievement_campaign_metagame_cavalier", 2, 0, 1, 1 },
    { "_achievement_campaign_metagame_askar", 2, 0, 1, 1 },
    { "_achievement_campaign_metagame_exterminator", 2, 0, 1, 1 },
    { "_achievement_campaign_metagame_ranger", 2, 0, 1, 1 },
    { "_achievement_campaign_metagame_vanguard", 2, 0, 1, 1 },
    { "_achievement_campaign_metagame_orpheus", 2, 0, 1, 1 },
    { "_achievement_campaign_metagame_reclaimer", 2, 0, 1, 1 },
    { "_achievement_iron_skull", 2, 0, 1, 1 },
    { "_achievement_black_eye_skull", 2, 0, 1, 1 },
    { "_achievement_tough_luck_skull", 2, 0, 1, 1 },
    { "_achievement_catch_skull", 2, 0, 1, 1 },
    { "_achievement_fog_skull", 2, 0, 1, 1 },
    { "_achievement_famine_skull", 2, 0, 1, 1 },
    { "_achievement_thunderstorm_skull", 2, 0, 1, 1 },
    { "_achievement_tilt_skull", 2, 0, 1, 1 },
    { "_achievement_mythic_skull", 2, 0, 1, 1 },
    { "_achievement_spartan_recruit", 2, 0, 1, 1 },
    { "_achievement_spartan_graduate", 2, 0, 1, 1 },
    { "_achievement_spartan_officer", 2, 0, 1, 1 },
    { "_achievement_extra_crispy", 2, 0, 1, 1 },
    { "_achievement_mvp", 2, 0, 1, 1 },
    { "_achievement_triple_kill", 2, 0, 1, 1 },
    { "_achievement_overkill", 2, 0, 1, 1 },
    { "_achievement_kill_frenzy", 2, 0, 1, 1 },
    { "_achievement_triple_sword_kill", 2, 0, 1, 1 },
    { "_achievement_mongoose_splatter", 2, 0, 1, 1 },
    { "_achievement_melee_kill_x5", 2, 0, 1, 1 },
    { "_achievement_sticky_x5", 2, 0, 1, 1 },
    { "_achievement_reboard_vehicle", 2, 0, 1, 1 },
    { "_achievement_needler_kill_x5", 2, 0, 1, 1 },
    { "_achievement_laser_or_missle_kill_banshee", 2, 0, 1, 1 },
    { "_achievement_headshot_kill_x10", 2, 0, 1, 1 },
    { "_achievement_tripmine_vehicle_kill", 2, 0, 1, 1 },
    { "_achievement_full_vehicle_kill", 2, 0, 1, 1 },
};


struct s_game_results_incremental;
enum e_game_results_event_type
{
    _game_results_event_type_none = 0,
    _game_results_event_type_kill = 1,
    _game_results_event_type_carry = 2
};

namespace {
static __forceinline void players_get_machines(unsigned long *valid_mask, s_machine_identifier *machines)
{
    const s_players_globals *storage = (const s_players_globals *)tls_players_globals();
    *valid_mask = storage->machine_valid_mask;
    memcpy(machines, storage->machines, sizeof(storage->machines));
}
}




// @address 0x8c4044 ?g_game_results_globals@@3U0@A
struct g_game_results_globals g_game_results_globals;
c_game_results g_current_game_results;

// @address 0x33de0 ?g_game_results_medal_info@@3QBUs_medal_info@@B
// @rdata 0x82033de0..0x820340a4
const s_medal_info g_game_results_medal_info[58] = {
    { 0, 2, 0 }, // 0
    { 0, 1, 0 }, // 1
    { 3, 1, 0 }, // 2
    { 3, 2, 0 }, // 3
    { 3, 3, 0 }, // 4
    { 3, 4, 0 }, // 5
    { 3, 5, 0 }, // 6
    { 3, 6, 0 }, // 7
    { 3, 7, 0 }, // 8
    { 3, 8, 0 }, // 9
    { 3, 9, 0 }, // 10
    { 2, 1, 0 }, // 11
    { 2, 2, 0 }, // 12
    { 2, 3, 0 }, // 13
    { 2, 4, 0 }, // 14
    { 2, 5, 0 }, // 15
    { 2, 6, 0 }, // 16
    { 1, 1, 0 }, // 17
    { 1, 2, 0 }, // 18
    { 1, 1, 0 }, // 19
    { 1, 2, 0 }, // 20
    { 1, 1, 0 }, // 21
    { 1, 2, 0 }, // 22
    { 1, 1, 0 }, // 23
    { 1, 2, 0 }, // 24
    { 1, 1, 0 }, // 25
    { 1, 2, 0 }, // 26
    { 1, 1, 0 }, // 27
    { 1, 2, 0 }, // 28
    { 1, 1, 0 }, // 29
    { 1, 2, 0 }, // 30
    { 1, 3, 0 }, // 31
    { 1, 1, 0 }, // 32
    { 5, 0, 0 }, // 33
    { 6, 0, 0 }, // 34
    { 5, 0, 0 }, // 35
    { 5, 0, 0 }, // 36
    { 5, 0, 0 }, // 37
    { 5, 0, 0 }, // 38
    { 6, 0, 0 }, // 39
    { 6, 0, 0 }, // 40
    { 6, 0, 0 }, // 41
    { 6, 0, 0 }, // 42
    { 6, 0, 0 }, // 43
    { 6, 0, 0 }, // 44
    { 5, 0, 0 }, // 45
    { 4, 0, 0 }, // 46
    { 4, 0, 0 }, // 47
    { 4, 0, 0 }, // 48
    { 4, 0, 0 }, // 49
    { 4, 0, 0 }, // 50
    { 4, 0, 0 }, // 51
    { 4, 0, 0 }, // 52
    { 4, 0, 0 }, // 53
    { 4, 0, 0 }, // 54
    { 4, 0, 0 }, // 55
    { 4, 0, 0 }, // 56
    { 4, 0, 0 }, // 57
};

// @address 0x33fb0 ?g_game_results_damage_type_descriptions@@3QBUe_game_results_damage_type_description@@B
const e_game_results_damage_type_description g_game_results_damage_type_descriptions[61] = {
    { _game_results_damage_breakdown_type_none }, // 0
    { _game_results_damage_breakdown_type_none }, // 1
    { _game_results_damage_breakdown_type_none }, // 2
    { _game_results_damage_breakdown_type_melee }, // 3
    { _game_results_damage_breakdown_type_none }, // 4
    { _game_results_damage_breakdown_type_weapon }, // 5
    { _game_results_damage_breakdown_type_weapon }, // 6
    { _game_results_damage_breakdown_type_weapon }, // 7
    { _game_results_damage_breakdown_type_weapon }, // 8
    { _game_results_damage_breakdown_type_weapon }, // 9
    { _game_results_damage_breakdown_type_weapon }, // 10
    { _game_results_damage_breakdown_type_weapon }, // 11
    { _game_results_damage_breakdown_type_weapon }, // 12
    { _game_results_damage_breakdown_type_weapon }, // 13
    { _game_results_damage_breakdown_type_weapon }, // 14
    { _game_results_damage_breakdown_type_weapon }, // 15
    { _game_results_damage_breakdown_type_weapon }, // 16
    { _game_results_damage_breakdown_type_weapon }, // 17
    { _game_results_damage_breakdown_type_weapon }, // 18
    { _game_results_damage_breakdown_type_weapon }, // 19
    { _game_results_damage_breakdown_type_weapon }, // 20
    { _game_results_damage_breakdown_type_weapon }, // 21
    { _game_results_damage_breakdown_type_weapon }, // 22
    { _game_results_damage_breakdown_type_weapon }, // 23
    { _game_results_damage_breakdown_type_weapon }, // 24
    { _game_results_damage_breakdown_type_melee }, // 25
    { _game_results_damage_breakdown_type_melee }, // 26
    { _game_results_damage_breakdown_type_grenade }, // 27
    { _game_results_damage_breakdown_type_grenade }, // 28
    { _game_results_damage_breakdown_type_grenade }, // 29
    { _game_results_damage_breakdown_type_grenade }, // 30
    { _game_results_damage_breakdown_type_melee }, // 31
    { _game_results_damage_breakdown_type_melee }, // 32
    { _game_results_damage_breakdown_type_none }, // 33
    { _game_results_damage_breakdown_type_melee }, // 34
    { _game_results_damage_breakdown_type_weapon }, // 35
    { _game_results_damage_breakdown_type_weapon }, // 36
    { _game_results_damage_breakdown_type_weapon }, // 37
    { _game_results_damage_breakdown_type_weapon }, // 38
    { _game_results_damage_breakdown_type_vehicle }, // 39
    { _game_results_damage_breakdown_type_vehicle }, // 40
    { _game_results_damage_breakdown_type_vehicle }, // 41
    { _game_results_damage_breakdown_type_vehicle }, // 42
    { _game_results_damage_breakdown_type_vehicle }, // 43
    { _game_results_damage_breakdown_type_vehicle }, // 44
    { _game_results_damage_breakdown_type_vehicle }, // 45
    { _game_results_damage_breakdown_type_vehicle }, // 46
    { _game_results_damage_breakdown_type_vehicle }, // 47
    { _game_results_damage_breakdown_type_vehicle }, // 48
    { _game_results_damage_breakdown_type_vehicle }, // 49
    { _game_results_damage_breakdown_type_vehicle }, // 50
    { _game_results_damage_breakdown_type_vehicle }, // 51
    { _game_results_damage_breakdown_type_vehicle }, // 52
    { _game_results_damage_breakdown_type_vehicle }, // 53
    { _game_results_damage_breakdown_type_vehicle }, // 54
    { _game_results_damage_breakdown_type_vehicle }, // 55
    { _game_results_damage_breakdown_type_weapon }, // 56
    { _game_results_damage_breakdown_type_weapon }, // 57
    { _game_results_damage_breakdown_type_none }, // 58
    { _game_results_damage_breakdown_type_none }, // 59
    { _game_results_damage_breakdown_type_weapon }, // 60
};
// @rdata-end

// @address 0x7ea8e4 g_game_results_sort_results
extern "C" {
const c_game_results *g_game_results_sort_results = 0;
}

// @address 0x119a42f g_game_results_player_left
extern "C" {
bool g_game_results_player_left = false;
}


static __forceinline bool game_results_get_game_recording() { return g_game_results_globals.active; }


static __forceinline bool stats_available(void)
{
    return g_current_game_results.initialized && g_current_game_results.finalized;
}
static __forceinline bool game_results_player_exists(long player_absolute_index)
{
    return g_current_game_results.players[player_absolute_index].player_exists;
}
__forceinline long game_results_statistic_get_direct(long player_absolute_index, e_game_team team_index, e_game_results_statistic statistic)
{
    c_game_results *game_results = &g_current_game_results;
    long value = NONE;

    if (player_absolute_index != NONE)
    {
        value = game_results->statistics.player[player_absolute_index].statistics[statistic].value;
    }
    if (team_index >= 0 && team_index < 16)
    {
        value = game_results->statistics.team[team_index].statistics[statistic].value;
    }
    return value;
}

// @address 0x244720 ?game_results_notify_player_indices_changed@@YAXXZ
void game_results_notify_player_indices_changed()
{
    if (game_results_get_game_recording() && !game_results_get_game_finalized()) {
        c_game_results *game_results = &g_current_game_results;
        long old_player_indices[16];
        long new_player_indices[16];
        for (long player_index = 0; player_index < 16; ++player_index) {
            old_player_indices[player_index] = NONE;
            new_player_indices[player_index] = NONE;
        }
        for (long player_index = 0; player_index < 16; ++player_index) {
            player_datum *player = (player_datum *)datum_try_and_get_absolute(player_data, player_index);
            if (player) {
                for (long old_player_index = 0; old_player_index < 16; ++old_player_index) {
                    if (game_results->players[old_player_index].player_exists &&
                        memcmp(player->player_identifier.data,
                game_results->players[old_player_index].identifier, sizeof(player->player_identifier.data)) == 0)
                    {
                        if (new_player_indices[old_player_index] == NONE) {
                            new_player_indices[old_player_index] = player_index;
                            old_player_indices[player_index] = old_player_index;
                        }
                        break;
                    }
                }
            }
        }
        s_game_results_player_data old_player_data[16];
        memcpy(old_player_data, game_results->players, sizeof(old_player_data));
        for (long player_index = 0; player_index < 16; ++player_index) {
            player_datum *player = (player_datum *)datum_try_and_get_absolute(player_data, player_index);
            long old_player_index = old_player_indices[player_index];
            if (player) {
                if (old_player_index == NONE) {
                    game_results->players[player_index].player_exists = true;
                    memcpy(game_results->players[player_index].identifier, player->player_identifier.data, sizeof(player->player_identifier.data));
                    game_results->players[player_index].configuration = player->configuration;
                    game_results->players[player_index].standing = NONE;
                    game_results->players[player_index].machine_index = NONE;
                } else {
                    game_results->players[player_index] = old_player_data[old_player_index];
                }
            }
        }
        s_game_results_player_statistics old_player_statistics[16];
        memcpy(old_player_statistics, game_results->statistics.player, sizeof(old_player_statistics));
        for (long player_index = 0; player_index < 16; ++player_index) {
            long old_player_index = old_player_indices[player_index];
            if (old_player_index == NONE)
                memset(&game_results->statistics.player[player_index], 0, sizeof(s_game_results_player_statistics));
            else
                game_results->statistics.player[player_index] = old_player_statistics[old_player_index];
        }
        unsigned short old_player_vs_player_statistics[16][16][2];
        memcpy(old_player_vs_player_statistics, game_results->statistics.pvp_stats, sizeof(old_player_vs_player_statistics));
        for (long player_index = 0; player_index < 16; ++player_index) {
            for (long opponent_index = 0; opponent_index < 16; ++opponent_index) {
                long old_opponent_index = old_player_indices[opponent_index];
                long old_player_index = old_player_indices[player_index];
                if (old_opponent_index != NONE && old_player_index != NONE)
                    memcpy(game_results->statistics.pvp_stats[player_index][opponent_index],
                        old_player_vs_player_statistics[old_player_index][old_opponent_index], sizeof(unsigned short) * 2);
                else
                    memset(game_results->statistics.pvp_stats[player_index][opponent_index], 0, sizeof(unsigned short) * 2);
            }
        }
        for (unsigned long event_index = 0; event_index < 1000; ++event_index) {
            s_game_results_event *event = &game_results->events[event_index];
            for (long index = 0; index < 2; ++index) {
                long old_player_index = event->player_indices[index];
                if (old_player_index != NONE && old_player_index >= 0 && old_player_index < 16)
                    event->player_indices[index] = (signed char)new_player_indices[old_player_index];
                else
                    event->player_indices[index] = NONE;
            }
        }
        game_results->fixup_player_machine_references();
    }
}

// @address 0x244b10 ?game_results_notify_active_teams_changed@@YAXXZ
void game_results_notify_active_teams_changed()
{
    if (game_results_get_game_recording() && !game_results_get_game_finalized()) {
        c_game_results *game_results = &g_current_game_results;
        for (e_game_team team = _game_team_default; team < 16; team = (e_game_team)(team + 1)) {
            s_game_results_team_data *team_data = &game_results->teams.team[team];
            bool team_active = game_engine_is_team_ever_active(team);
            if (team_active && !team_data->team_exists)
                team_data->team_exists = true;
        }
    }
}

void game_results_statistic_increment(long player_absolute_index, e_game_team team_index, e_game_results_statistic statistic, long amount)
{
    if (g_game_results_globals.active && !g_game_results_globals.finished) {
        int minimum = g_game_results_statistic_definitions[statistic].minimum;
        int maximum = g_game_results_statistic_definitions[statistic].maximum;
        if (player_absolute_index != NONE && g_current_game_results.players[player_absolute_index].player_exists) {
            s_game_results_statistic *stat = &g_current_game_results.statistics.player[player_absolute_index].statistics[statistic];
            int value = stat->value + amount;
            stat->value = MIN(MAX(value, minimum), maximum);
        }
        if (team_index >= 0 && team_index < 16U && g_current_game_results.teams.team[team_index].team_exists) {
            s_game_results_statistic *stat = &g_current_game_results.statistics.team[team_index].statistics[statistic];
            int value = stat->value + amount;
            stat->value = MIN(MAX(value, minimum), maximum);
        }
    }
}

void game_results_statistic_set(long player_absolute_index, e_game_team team_index, e_game_results_statistic statistic, long value)
{
  if (g_game_results_globals.active && !g_game_results_globals.finished) {
    long minimum = g_game_results_statistic_definitions[statistic].minimum;
    long maximum = g_game_results_statistic_definitions[statistic].maximum;
    if (player_absolute_index != NONE) {
      unsigned short *entry = &g_current_game_results.statistics.player[player_absolute_index].values[statistic];
      *entry = (*entry & 1) | (MIN(MAX(value, minimum), maximum) << 1);
    }
    if (team_index >= 0 && team_index < 16U && g_current_game_results.teams.team[team_index].team_exists) {
      unsigned short *entry = &g_current_game_results.statistics.team[team_index].values[statistic];
      *entry = (*entry & 1) | (MIN(MAX(value, minimum), maximum) << 1);
    }
  }
}

// @address 0x244e28 ?game_results_set_final_player_score_and_standing@@YAXJJJ@Z
void game_results_set_final_player_score_and_standing(long player_absolute_index, long score, long standing)
{
    if (game_results_get_game_recording() && !game_results_get_game_finalized() && player_absolute_index != NONE &&
        g_current_game_results.players[player_absolute_index].player_exists)
    {
        g_current_game_results.players[player_absolute_index].final_score = (short)score;
        g_current_game_results.players[player_absolute_index].standing = (signed char)standing;
    }
}

// @address 0x244ea0 ?game_results_set_final_team_score_and_standing@@YAXW4e_game_team@@JJ@Z
void game_results_set_final_team_score_and_standing(e_game_team team_index, long score, long standing)
{
    if (game_results_get_game_recording() && !game_results_get_game_finalized() && team_index >= 0 && team_index < 16U &&
        g_current_game_results.teams.team[team_index].team_exists)
    {
        long value;
        if (score > -32768 && score < 32767)
            value = score;
        else if (score <= -32768)
            value = -32768;
        else
            value = 32767;
        g_current_game_results.teams.team[team_index].final_score = (short)value;
        g_current_game_results.teams.team[team_index].standing = (signed char)standing;
    }
}

void game_results_player_vs_player_statistic_increment(
    long player_absolute_index, long opponent_absolute_index, e_game_results_player_vs_player_statistic statistic, long amount)
{
  if (g_game_results_globals.active && !g_game_results_globals.finished) {
    long minimum = g_game_results_player_vs_player_statistic_definitions[statistic].minimum;
    long maximum = g_game_results_player_vs_player_statistic_definitions[statistic].maximum;
    unsigned short *entry = &g_current_game_results.statistics.pvp_stats[player_absolute_index][opponent_absolute_index][statistic];
    long value = (*entry >> 1) + 1;
    *entry = (*entry & 1) | (MIN(MAX(value, minimum), maximum) << 1);
  }
}

// @address 0x245008 game_results_damage_statistic_increment
extern "C" void game_results_damage_statistic_increment(long player, long kind, long damage, long amount)
{
    if (g_game_results_globals.active && !g_game_results_globals.finished) {
        s_game_results_damage_statistics *stats = &g_current_game_results.statistics.player[player].damage_statistics[damage];
        stats->active = true;
        s_game_results_statistic *value = &g_current_game_results.statistics.player[player].damage_statistics[damage].values[kind];
        long minimum = g_game_results_damage_statistic_definitions[kind].minimum;
        long maximum = g_game_results_damage_statistic_definitions[kind].maximum;
        value->value = MIN(MAX(value->value + 1, minimum), maximum);
    }
}

// @address 0x2450e0 ?game_results_insert_event@@YAXPBUs_game_results_event@@@Z
void game_results_insert_event(const s_game_results_event *event)
{
    if (game_results_get_game_recording() && !g_game_results_globals.finished) {
        long event_index = g_game_results_globals.event_index;
        g_current_game_results.events[event_index] = *event;
        g_game_results_globals.event_index = (unsigned long)(event_index + 1) % 1000;
    }
}


static __forceinline unsigned long game_results_get_system_seconds()
{
    return (unsigned long)_time64(0);
}

static __forceinline long game_results_tracked_damage_type(unsigned char damage_reporting_info)
{
    return damage_reporting_info >> 2;
}

// @address 0x245168 ?game_results_insert_kill_event@@YAXJJE@Z
void game_results_insert_kill_event(long killing_player_index, long dead_player_index, unsigned char damage_reporting_info)
{
    if (game_results_get_game_recording() && !g_game_results_globals.finished) {
        bool positions_valid = true;
        s_game_results_event event;
        memset(&event.player_indices[0], 0, sizeof(event) - offsetof(s_game_results_event, player_indices));
        event.type = _game_results_event_type_kill;
        event.player_indices[0] = (signed char)killing_player_index;
        event.player_indices[1] = (signed char)dead_player_index;
        unsigned long now = game_results_get_system_seconds();
        event.kill.damage_type = game_results_tracked_damage_type(damage_reporting_info);
        event.kill.time = now - g_current_game_results.end_reason;
        positions_valid = positions_valid && game_results_get_player_position(killing_player_index, &event.kill.positions[0]);
        positions_valid = positions_valid && game_results_get_player_position(dead_player_index, &event.kill.positions[1]);
        if (positions_valid)
            game_results_insert_event(&event);
    }
}

// @address 0x245250 ?game_results_insert_carry_event@@YAXJW4e_game_results_event_carry_type@@J@Z
void game_results_insert_carry_event(long player_index, e_game_results_event_carry_type carry_type, long carry_value)
{
    if (game_results_get_game_recording() && !g_game_results_globals.finished) {
        unsigned long now = game_results_get_system_seconds();
        if (now - g_game_results_globals.last_carry_event_time > 1) {
            s_game_results_event event;
            memset(&event.player_indices[0], 0, sizeof(event) - offsetof(s_game_results_event, player_indices));
            event.player_indices[0] = (signed char)player_index;
            event.type = _game_results_event_type_carry;
            event.player_indices[1] = NONE;
            unsigned long seconds = game_results_get_system_seconds();
            event.carry.carry_type = carry_type;
            event.carry.carry_value = carry_value;
            event.carry.time = seconds - g_current_game_results.end_reason;
            if (game_results_get_player_position(player_index, &event.carry.position))
                game_results_insert_event(&event);
            g_game_results_globals.last_carry_event_time = now;
        }
    }
}

// @address 0x246e60 ?get_first_unreferenced_machine_index@c_game_results@@QAAJXZ
long c_game_results::get_first_unreferenced_machine_index()
{
    long result = NONE;
    unsigned long referenced_machines = 0;
    unsigned long player_machine_valid_mask;
    s_machine_identifier player_machines[16];
    players_get_machines(&player_machine_valid_mask, player_machines);
    for (long player_index = 0; player_index < 16; ++player_index) {
        s_game_results_player_data *result_player_data = &players[player_index];
        player_datum *player = (player_datum *)datum_try_and_get_absolute(player_data, player_index);
        if (player) {
            if (player->machine_index != NONE)
                result_player_data->machine_index = (signed char)get_machine_index(&player_machines[player->machine_index]);
        }
        if (result_player_data->player_exists && result_player_data->machine_index != NONE)
            referenced_machines |= 1 << result_player_data->machine_index;
    }
    for (unsigned long machine_index = 0; machine_index < 16; ++machine_index) {
        if ((referenced_machines & (1 << machine_index)) == 0) {
            result = machine_index;
            break;
        }
    }
    return result;
}

// @address 0x246f70 ?add_machine@c_game_results@@QAAJPBUs_machine_identifier@@@Z
long c_game_results::add_machine(const s_machine_identifier *identifier)
{
    long machine_index = NONE;
    for (unsigned long index = 0; index < 16; ++index)
    {
        if (!entries[index].valid)
        {
            machine_index = index;
            break;
        }
    }
    if (machine_index == NONE)
        machine_index = get_first_unreferenced_machine_index();
    if (machine_index != NONE)
    {
        memset(&entries[machine_index], 0, sizeof(entries[machine_index]));
        entries[machine_index].valid = true;
        entries[machine_index].identifier = *identifier;
    }
    return machine_index;
}

long c_game_results::get_machine_index(const s_machine_identifier *identifier) const
{
    long result = -1;
    for (unsigned long i = 0; i < 16 && result == -1; i++) {
        const s_entry *entry = &this->entries[i];
        if (entry->valid != 0 && memcmp(entry->key, identifier->data, sizeof(identifier->data)) == 0)
            result = i;
    }
    return result;
}

long c_game_results::get_host_machine_index() const
{
    long result = -1;
    for (unsigned int i = 0; i < 16 && result == -1; i++) {
        const unsigned char *entry = (const unsigned char *)&g_current_game_results + 0xeff0 + i * 22;
        if (entry[6] != 0 && entry[8] != 0)
            result = i;
    }
    return result;
}

void c_game_results::fixup_player_machine_references(void)
{
    s_machine_identifier machines[16];
    const s_players_globals *storage = (const s_players_globals *)tls_players_globals();
    memcpy(machines, storage->machines, sizeof(machines));
    unsigned long player_index = 0;
    signed char *machine_index = &this->players[0].machine_index;
    do {
        s_data_array *array = player_data;
        if (player_index != 0xffffffff && 0 <= (int)player_index && (int)player_index < array->first_unallocated) {
            player_datum *reference = (player_datum *)(array->data + array->size * player_index);
            if (reference->identifier != 0 && reference->machine_index != NONE && *machine_index == NONE) {
                signed char result = (signed char)this->get_machine_index(&machines[reference->machine_index]);
                *machine_index = result;
                if (result == NONE) {
                    result = (signed char)this->add_machine(&machines[reference->machine_index]);
                    *machine_index = result;
                }
            }
        }
        player_index = player_index + 1;
        machine_index += sizeof(s_game_results_player_data);
    } while (player_index < 0x10);
}

long game_results_count_damage_statistic(long player_absolute_index, e_game_results_damage_statistic statistic, e_game_results_damage_breakdown_type category)
{
  long result = 0;
  if (stats_available()) {
    const unsigned short *values = g_current_game_results.statistics.player[player_absolute_index].values;
    for (long i = 0; i < 61; i++) {
      if (category == _game_results_damage_breakdown_type_all || g_game_results_damage_type_descriptions[i].breakdown_type == category)
        result += values[159 + statistic + i * 6] >> 1;
    }
  }
  return result;
}

long game_results_get_maximum_rounds_completed(void)
{
    long maximum_rounds_completed = 0;
    for (long player_absolute_index = 0; player_absolute_index < 16; player_absolute_index++)
    {
        if (game_results_player_exists(player_absolute_index))
        {
            long rounds_completed = game_results_statistic_get_direct(player_absolute_index, _game_team_none, _game_results_statistic_rounds_completed);
            maximum_rounds_completed = MAX(maximum_rounds_completed, rounds_completed);
        }
    }
    return maximum_rounds_completed;
}



// @address 0x2473e8 game_results_get_player_position
unsigned char game_results_get_player_position(long player_index, real_point3d *position)
{
    bool result = false;
    player_datum *player = &((player_datum *)player_data->data)[datum_index_to_absolute_index(player_index)];
    long unit_index = -1;
    if (player->unit_index != -1)
        unit_index = player->unit_index;
    else if (player->dead_unit_index != -1)
        unit_index = player->dead_unit_index;
    if (unit_index != -1) {
        object_get_origin(unit_index, position);
        result = true;
    }
    return result;
}


typedef char game_results_player_team_offset[(offsetof(s_game_results_player_data, configuration.host.team_index) == 248) ? 1 : -1];
typedef char game_results_player_score_offset[(offsetof(s_game_results_player_data, final_score) == 300) ? 1 : -1];


static __forceinline long game_results_sort_statistic(
    const s_game_results_player_statistics *first,
    const s_game_results_player_statistics *second,
    e_game_results_statistic statistic)
{
    unsigned long second_value = second->statistics[statistic].value;
    unsigned long first_value = first->statistics[statistic].value;
    long result = 0;
    if (first_value > second_value)
        result = -1;
    else if (first_value < second_value)
        result = 1;
    return result;
}

static __forceinline long game_results_sort_statistic_alt(
    const s_game_results_player_statistics *first,
    const s_game_results_player_statistics *second,
    e_game_results_statistic statistic)
{
    unsigned long first_value;
    unsigned long second_value;
    second_value = second->statistics[statistic].value;
    first_value = first->statistics[statistic].value;
    long result = 0;
    if (second_value > first_value)
        result = -1;
    else if (second_value < first_value)
        result = 1;
    return result;
}

extern "C" {
}

static __forceinline const unsigned char *game_results_player_machine_key(const s_game_results_player_data *player)
{
    signed char machine_index = player->machine_index;
    const unsigned char *results = (const unsigned char *)&g_current_game_results;
    if (results[0xeff6 + machine_index * 22])
        return results + 0xeff0 + machine_index * 22;
    return 0;
}

// 0x247518
extern "C" void game_results_grant_achievement_to_owner(long player_index, e_achievement achievement)
{
    s_game_results_player_data *player = &g_current_game_results.players[player_index];
    if (player->player_exists) {
        if ((signed char)player->machine_index >= 0 && (unsigned char)player->machine_index < 16) {
            long index = game_options_find_player(player->identifier, game_results_player_machine_key(player));
            if (index >= 0 && (unsigned long)index < 16) {
                game_player_options *options_player = &tls_game_globals()->options.players[index];
                if (options_player->valid && options_player->controller_index >= 0 && options_player->controller_index < 4) {
                    if (memcmp(options_player->machine_identifier.data,
                        &g_transport_security_globals.local_machine_identifier, sizeof(options_player->machine_identifier.data)) == 0)
                        game_grant_achievement_to_local_player((e_controller_index)options_player->controller_index, achievement, false);
                }
            }
        }
    }
}

// @address 0x247720 game_results_sort_players_by_standing
extern "C" int game_results_sort_players_by_standing(const void *a, const void *b)
{
    int result = 0;
    long first_index = *(const long *)a;
    long second_index = *(const long *)b;
    const s_game_results_player_data *first_player = &g_game_results_sort_results->players[first_index];
    const s_game_results_player_data *second_player = &g_game_results_sort_results->players[second_index];
    if (g_game_results_sort_results->teams_enabled)
    {
        long second_team = ((const s_game_results_player_data *)second_player)->configuration.host.team_index;
        long first_team = ((const s_game_results_player_data *)first_player)->configuration.host.team_index;
        long first_standing = g_game_results_sort_results->teams.team[first_team].standing;
        long second_standing = g_game_results_sort_results->teams.team[second_team].standing;
        if (first_standing < second_standing)
            result = -1;
        else if (first_standing > second_standing)
            result = 1;
        else if (first_team < second_team)
            result = -1;
        else if (first_team > second_team)
            result = 1;
    }
    if (result == 0)
    {
        if (first_player->standing < second_player->standing)
            result = -1;
        else if (first_player->standing > second_player->standing)
            result = 1;
        else if ((intptr_t)a < (intptr_t)b)
            result = -1;
        else if ((intptr_t)a > (intptr_t)b)
            result = 1;
    }
    return result;
}

extern "C" int game_results_sort_players_by_statistics(const void *a, const void *b)
{
    int result = 0;
    long first_index = *(const long *)a;
    long second_index = *(const long *)b;
    const s_game_results_player_data *first_player = &g_game_results_sort_results->players[first_index];
    const s_game_results_player_statistics *first_stats = &g_game_results_sort_results->statistics.player[first_index];
    const s_game_results_player_data *second_player = &g_game_results_sort_results->players[second_index];
    const s_game_results_player_statistics *second_stats = &g_game_results_sort_results->statistics.player[second_index];
    if ((bool)g_game_results_sort_results->teams_enabled)
    {
        switch (g_game_results_sort_results->game_variant.get_game_engine_index())
        {
        case _game_engine_ctf:
            result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_ctf_flag_scores);
            if (result == 0)
            {
                result = game_results_compare_team_score_share(
                    (const s_game_results_player_data *)first_player, (const s_game_results_player_data *)second_player);
                if (result == 0)
                {
                    result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_ctf_flag_grabs);
                    if (result == 0)
                    {
                        result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_ctf_flag_returns);
                        if (result == 0)
                            result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_kills);
                    }
                }
            }
            break;

        case _game_engine_slayer:
            result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_kills);
            if (result == 0)
            {
                result = game_results_compare_team_score_share(
                    (const s_game_results_player_data *)first_player, (const s_game_results_player_data *)second_player);
                if (result == 0)
                {
                    result = -game_results_sort_statistic_alt(first_stats, second_stats, _game_results_statistic_deaths);
                    if (result == 0)
                        result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_assists);
                }
            }
            break;

        case _game_engine_oddball:
            result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_oddball_time_with_ball);
            if (result == 0)
            {
                result = game_results_compare_team_score_share(
                    (const s_game_results_player_data *)first_player, (const s_game_results_player_data *)second_player);
                if (result == 0)
                    result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_kills);
            }
            break;

        case _game_engine_king:
            result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_king_unused_0);
            if (result == 0)
            {
                result = game_results_compare_team_score_share(
                    (const s_game_results_player_data *)first_player, (const s_game_results_player_data *)second_player);
                if (result == 0)
                    result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_kills);
            }
            break;

        case _game_engine_vip:
            result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_vip_takedowns);
            if (result == 0)
            {
                result = game_results_compare_team_score_share(
                    (const s_game_results_player_data *)first_player, (const s_game_results_player_data *)second_player);
                if (result == 0)
                {
                    result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_vip_time_as_vip);
                    if (result == 0)
                        result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_kills);
                }
            }
            break;

        case _game_engine_juggernaut:
            result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_juggernaut_kills);
            break;

        case _game_engine_territories:
            result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_territories_captures);
            if (result == 0)
            {
                result = game_results_compare_team_score_share(
                    (const s_game_results_player_data *)first_player, (const s_game_results_player_data *)second_player);
                if (result == 0)
                    result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_kills);
            }
            break;

        case _game_engine_assault:
            result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_assault_bomb_detonations);
            if (result == 0)
            {
                result = game_results_compare_team_score_share(
                    (const s_game_results_player_data *)first_player, (const s_game_results_player_data *)second_player);
                if (result == 0)
                {
                    result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_assault_bomb_disarms);
                    if (result == 0)
                        result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_kills);
                }
            }
            break;

        case _game_engine_infection:
            result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_infection_zombie_kills);
            if (result == 0)
                result = game_results_sort_statistic(first_stats, second_stats, _game_results_statistic_kills);
            break;
        }
    }
    return result;
}

// @address 0x247d80 game_results_compare_team_score_share
int game_results_compare_team_score_share(const s_game_results_player_data *a, const s_game_results_player_data *b)
{
    int result = 0;
    float a_denominator = (float)g_game_results_sort_results->teams.team[a->configuration.host.team_index].final_score;
    float b_denominator = (float)g_game_results_sort_results->teams.team[b->configuration.host.team_index].final_score;
    float a_ratio = a_denominator > 0.0f ? (float)a->final_score / a_denominator : 0.0f;
    float b_ratio = 0.0f;
    if (b_denominator > 0.0f)
        b_ratio = (float)b->final_score / b_denominator;
    if (a_ratio > b_ratio)
        result = -1;
    else if (a_ratio < b_ratio)
        result = 1;
    return result;
}




// @address 0x7ea8e8 g_game_results_network_statistics


// @address 0x24a710 game_results_update_network_statistics
void game_results_update_network_statistics(void)
{
    c_network_session *session = NULL;
    g_game_results_network_statistics.squad = false;
    g_game_results_network_statistics.group = false;
    memset(g_game_results_network_statistics.peers, 0, sizeof(g_game_results_network_statistics.peers));

    if (user_interface_get_interactive_session(&session))
    {
        bool in_game = (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
        if (in_game)
        {
            g_game_results_network_statistics.squad = session->m_session_type == _network_session_type_squad;
            g_game_results_network_statistics.group = session->m_session_type == _network_session_type_group;

            const c_network_session_membership *membership = session->get_session_membership();
            for (long peer_index = membership->get_first_peer();
                 peer_index != -1;
                 peer_index = membership->get_next_peer(peer_index))
            {
                s_game_results_network_peer_statistics *peer_stats = &g_game_results_network_statistics.peers[peer_index];
                const s_network_session_peer *peer = membership->get_peer(peer_index);

                peer_stats->valid = 1;
                peer_stats->has_gamestate_bonus = peer->properties.has_gamestate_bonus;
                peer_stats->party_nonce = peer->party_nonce;
                peer_stats->secure_address = peer->secure_address;
                peer_stats->peer_version = peer->peer_version;
                peer_stats->upstream_bandwidth = peer->properties.upstream_bandwidth;
                peer_stats->downstream_bandwidth = peer->properties.downstream_bandwidth;
                peer_stats->nat_type = peer->properties.nat_type;
                peer_stats->peer_connectivity_mask = peer->properties.connectivity.peer_connectivity_mask;
                peer_stats->peer_probe_mask = peer->properties.connectivity.peer_probe_mask;
            }
        }
    }
}

// @address 0x8c43d8 g_infection_variant

extern "C" void game_results_medal_increment(long player_index)
{
    int medal_data[4];
    game_engine_adjust_player_score(player_index, g_infection_variant->m_time_signed_bytes[1]);
    if (player_index != -1) {
        medal_data[0] = 0;
        ::game_engine_handle_time_messages(0x36, (unsigned short)player_index, (int)player_index, -1, (int *)(medal_data));
    }
}


extern "C" void game_results_try_and_grant_achievement(short player_index, e_achievement achievement)
{
    if (game_is_cooperative())
        return;
    bool skip = false;
    if (tls_game_globals()->options.game_simulation == 4)
        skip = true;
    if (skip)
        return;
    unsigned short player = (unsigned short)player_index;
    player_datum *players = &((player_datum *)player_data->data)[player];
    long index = game_options_find_player(players->player_identifier.data, players->machine_identifier.data);
    if ((unsigned long)index > 15)
        return;
    if (!game_conditions_allow_achievement(index, achievement))
        return;
    if (player_is_on_local_machine(player))
        game_grant_achievement_to_local_player((e_controller_index)players->controller_index, achievement, true);
    if (tls_game_globals()->options.game_mode != 2)
        return;
    bool skip_simulation = false;
    if (tls_game_globals()->options.game_simulation == 4)
        skip_simulation = true;
    if (skip_simulation)
        return;
    if (!g_game_results_globals.active || g_game_results_globals.finished)
        return;
    if (!g_current_game_results.players[player].player_exists)
        return;
    unsigned short *values = &g_current_game_results.statistics.player[0].values[109];
    long value_index = player * 524 + achievement;
    values[value_index] = (values[value_index] & 1) | 2;
    game_results_grant_achievement_to_owner(player, achievement);
}


// match: 0x245320: retain the accepted external results view; direct owner access changes the team copy to memcpy.
// @address 0xa33f80 g_game_results_incremental_storage
#if !defined(_XBOX) || defined(__clang__)
// PC link only: on Xbox this name is the absolute 0xa33f80 alias of g_current_game_results.
s_game_results_incremental_storage g_game_results_incremental_storage;
#endif

// match: simulation.cpp still declares this C bridge with an erased argument.

// @address 0x245320 ?game_results_set_incremental@@YAXPBUs_game_results_incremental@@@Z
void game_results_set_incremental(struct s_game_results_incremental const * incremental)
{
    if (!g_game_results_incremental_storage.finalized) {
        g_game_results_incremental_storage.summary_valid = incremental->summary_valid;
        g_game_results_incremental_storage.end_reason = incremental->summary_value;
        g_game_results_incremental_storage.finish_valid = incremental->finish_valid;
        g_game_results_incremental_storage.finish_value = incremental->finish_value;
        memcpy(g_game_results_incremental_storage.players, &incremental->players, sizeof(g_game_results_incremental_storage.players));
        memcpy(&g_game_results_incremental_storage.statistics, &incremental->statistics, sizeof(g_game_results_incremental_storage.statistics));
        g_game_results_incremental_storage.teams = incremental->teams;
        memcpy(g_game_results_incremental_storage.entries, &incremental->machines, sizeof(g_game_results_incremental_storage.entries));
        if (incremental->finalized) {
            g_game_results_incremental_storage.finalized = true;
            game_results_grant_top_player_achievement((c_game_results *)&g_game_results_incremental_storage);
        }
    }
}

extern "C" void game_results_grant_top_player_achievement(c_game_results *game_results)
{
    long sorted_indices[16];
    long count = 0;
    memset(sorted_indices, -1, sizeof(sorted_indices));
    long *output = sorted_indices;
    long player_index = 0;
    const s_game_results_player_data *player = g_current_game_results.players;
    unsigned long remaining = 16;
    do
    {
        if (player->player_exists)
        {
            *output = player_index;
            ++count;
            ++output;
        }
        ++player_index;
        ++player;
    } while (--remaining);
    if (g_current_game_results.teams_enabled && count > 0)
    {
        g_game_results_sort_results = &g_current_game_results;
        qsort(sorted_indices, count, sizeof(long), game_results_sort_players_by_statistics);
        g_game_results_sort_results = 0;
        game_results_grant_achievement_to_owner(sorted_indices[0], _achievement_mvp);
    }
}


// @address 0x2453f8 game_results_decode_statistics
extern "C" bool game_results_decode_statistics(c_bitstream *packet, s_game_results_statistic *statistics, long count,
    const s_integer_statistic_definition *definitions, long definition_count)
{
    bool success = true;
    for (long index = 0; index < count; ++index)
    {
        statistics[index].flag = packet->read_bool("flag");
        if (statistics[index].flag)
        {
            long value = definitions[index].minimum + packet->read_integer("statistic-value", definitions[index].bit_count);
            success = success && value <= definitions[index].maximum;
            statistics[index].value = value;
        }
    }
    return success;
}

static __forceinline void game_results_read_bytes(c_bitstream *packet, unsigned char *buffer, long size_in_bits)
{
    unsigned __int64 value = bitstream_read_window_bits(packet, size_in_bits) << (64 - size_in_bits);
    unsigned long bytes = size_in_bits / 8;
    do {
        *buffer++ = (unsigned char)(value >> 56);
        value <<= 8;
        --bytes;
    } while (bytes != 0);
}

static __forceinline long game_results_read_signed_integer(c_bitstream *packet, const char *name, long size_in_bits)
{
    unsigned long value = packet->read_integer(name, size_in_bits);
    if (size_in_bits < 32 && (value & (1UL << (size_in_bits - 1))) != 0)
        value |= ~((1UL << size_in_bits) - 1);
    return (long)value;
}

static __forceinline bool &game_results_packet_damage_active(s_game_results_statistic *values)
{
    return *(bool *)((char *)values - offsetof(s_game_results_damage_statistics, values));
}

// @address 0x2459e8 game_results_decode_packet
extern "C" bool game_results_decode_packet(c_bitstream *packet, s_game_results_incremental_update *results)
{
    bool success = true;
    memset(results, 0, sizeof(*results));
    results->summary_valid = packet->read_bool("started");
    if (results->summary_valid)
        game_results_read_bytes(packet, (unsigned char *)&results->summary_value, 32);
    results->finish_valid = packet->read_bool("finished");
    if (results->finish_valid)
        game_results_read_bytes(packet, (unsigned char *)&results->finish_value, 32);
    results->finalized = packet->read_bool("finalized");
    for (long player_index = 0; player_index < 16; ++player_index)
    {
        s_game_results_packet_player *player = &results->players[player_index];
        player->exists = packet->read_bool("exists");
        if (player->exists)
        {
            player->has_definition = packet->read_bool("has-definition");
            if (player->has_definition)
            {
                network_game_definition_player_decode(packet, &player->definition);
            }
            else
            {
                player->definition.player_exists = packet->read_bool("valid");
                player->definition.standing = (signed char)(packet->read_integer("standing", 5) - 1);
            }
        }
    }
    for (long team_index = 0; team_index < 16; ++team_index)
    {
        s_game_results_packet_team *team = &results->teams[team_index];
        team->exists = packet->read_bool("exists");
        if (team->exists)
        {
            team->has_identifier = packet->read_bool("has-identifier");
            if (team->has_identifier)
            {
                game_results_read_bytes(packet, team->identifier, 32);
            }
            else
            {
                team->flag = packet->read_bool("flag");
                team->standing = (signed char)(packet->read_integer("standing", 5) - 1);
                team->final_score = (short)game_results_read_signed_integer(packet, "score", 16);
            }
        }
    }
    for (long player_index = 0; player_index < 16; ++player_index)
    {
        s_game_results_packet_player_statistics *player = &results->player_statistics[player_index];
        player->exists = packet->read_bool("exists");
        if (player->exists)
        {
            player->statistics_valid = packet->read_bool("statistics-valid");
            if (player->statistics_valid)
                success = success && game_results_decode_statistics(packet, player->statistics, 51, g_game_results_statistic_definitions, 51);
            player->statistics_58_valid = packet->read_bool("statistics-58-valid");
            if (player->statistics_58_valid)
                success = success && game_results_decode_statistics(packet, player->statistics_58, 58, g_game_results_medal_statistic_definitions, 58);
            player->statistics_49_valid = packet->read_bool("statistics-49-valid");
            if (player->statistics_49_valid)
                success = success && game_results_decode_statistics(packet, player->statistics_49, 49, g_game_results_achievement_statistic_definitions, 49);
            for (long damage_index = 0; damage_index < 61; ++damage_index)
            {
                s_game_results_damage_statistics *damage = &player->damage[damage_index];
                s_game_results_statistic *values = damage->values;
                bool &active = game_results_packet_damage_active(values);
                active = packet->read_bool("active");
                if (active)
                    success = success && game_results_decode_statistics(packet, values, 5, g_game_results_damage_statistic_definitions, 5);
            }
        }
    }
    for (long player_index = 0; player_index < 16; ++player_index)
    {
        for (long opponent_index = 0; opponent_index < 16; ++opponent_index)
        {
            s_game_results_packet_player_vs_player_statistics *statistics = &results->player_vs_player_statistics[player_index][opponent_index];
            statistics->valid = packet->read_bool("valid");
            if (statistics->valid)
                success = success &&
                    game_results_decode_statistics(
                        packet, statistics->statistics, 2, g_game_results_player_vs_player_statistic_definitions, 2);
        }
    }
    for (long team_index = 0; team_index < 16; ++team_index)
    {
        s_game_results_packet_team_statistics *statistics = &results->team_statistics[team_index];
        statistics->valid = packet->read_bool("valid");
        if (statistics->valid)
            success = success && game_results_decode_statistics(packet, statistics->statistics, 51, g_game_results_statistic_definitions, 51);
    }
    for (long machine_index = 0; machine_index < 16; ++machine_index)
    {
        s_game_results_packet_machine *machine = &results->machines[machine_index];
        machine->valid = packet->read_bool("valid");
        if (machine->valid)
        {
            game_results_read_bytes(packet, machine->identifier, 48);
            machine->exists = packet->read_bool("exists");
            machine->connected = packet->read_bool("connected");
            machine->initial_host = packet->read_bool("initial-host");
            machine->host = packet->read_bool("host");
            machine->voluntary_quit = packet->read_bool("voluntary-quit");
            packet->read_raw_data("bandwidth-estimate", machine->bandwidth_estimate, 80);
        }
    }
    return success && !packet->error_occurred();
}

// match: 0x5a0328: SDK header adoption is blocked by this unit's current types/API arguments: 'XNADDR' : redefinition; different basic types.
// match: 0x609a70: SDK header adoption is blocked by this unit's current types/API arguments: 'XNADDR' : redefinition; different basic types.

static __forceinline unsigned short *game_results_ustrncpy(unsigned short *dest, const wchar_t *source, long count)
{
    for (long index = 0; index <= count; index++)
    {
        if (!(dest[index] = *source))
            break;
        ++source;
    }
    return dest;
}


static __forceinline bool game_results_options_simulation_is_client()
{
    bool result = false;
    if (tls_game_globals()->options.game_simulation == 4)
        result = true;
    return result;
}

static __forceinline bool game_results_options_recording_blocked()
{
    bool valid = tls_game_globals() > static_cast<game_globals_storage *>(0)
        && (tls_game_globals()->initializing || tls_game_globals()->map_active);
    return valid && tls_game_globals()->options.game_playback != 0;
}

// @address 0x244308 ?game_results_initialize_for_new_map@@YAXXZ
void game_results_initialize_for_new_map()
{
    if (game_engine_running())
    {
        const game_options *game_options = &tls_game_globals()->options;
        c_game_results *game_results = &g_current_game_results;
        memset(game_results, 0, sizeof(*game_results));
        g_game_results_globals.event_index = 0;
        g_game_results_globals.last_carry_event_time = (long)_time64(0);
        game_results->hopper_identifier = (unsigned short)NONE;
        game_results->unique_id = game_options->game_instance;
        if (game_options->matchmade_game)
        {
            game_results->hopper_identifier = game_options->matchmaking_options.hopper_identifier;
            game_results_ustrncpy(game_results->hopper_name, game_options->matchmaking_options.hopper_name, 32);
            game_results->hopper_ranked = game_options->matchmaking_options.is_ranked;
            game_results->hopper_team_based = game_options->matchmaking_options.is_team_game;
            game_results->hopper_unknown48 = game_options->matchmaking_options.xlast_index;
            game_results->hopper_unknown4c = game_options->matchmaking_options.trueskill_draw_probability;
            game_results->hopper_unknown50 = game_options->matchmaking_options.trueskill_beta_performance_variation;
            game_results->hopper_unknown54 = game_options->matchmaking_options.trueskill_tau_dynamics_factor;
            game_results->hopper_unknown58 = game_options->matchmaking_options.team_count;
            game_results->hopper_unknown5c = game_options->matchmaking_options.player_count;
        }
        game_results->unique_id = game_options->game_instance;
        memcpy(&game_results->game_variant, &game_options->game_variant, sizeof(game_options->game_variant));
        game_results_ustrncpy(game_results->map_name, game_options->map_variant.m_metadata.name, 32);
        game_results->map_id = game_options->map_id;
        memcpy(game_results->scenario_path, game_options->scenario_path, sizeof(game_options->scenario_path));
        for (unsigned long player_index = 0; player_index < 16; ++player_index)
        {
            const game_player_options *options_player = &game_options->players[player_index];
            s_game_results_player_data *player = &game_results->players[player_index];
            bool exists = options_player->valid;
            player->player_exists = exists;
            if (exists)
            {
                // match: 0x244308: aligned identifier copy requires four halfword loads.
                memcpy(player->identifier, (const unsigned short *)&options_player->player_identifier, sizeof(player->identifier));
                memcpy(&player->configuration, &options_player->configuration, sizeof(player->configuration));
                player->machine_index = (signed char)game_results->get_machine_index((const s_machine_identifier *)options_player->machine_identifier.data);
                if (player->machine_index == NONE)
                    player->machine_index = (signed char)game_results->add_machine((const s_machine_identifier *)options_player->machine_identifier.data);
            }
        }
        game_results->teams_enabled = game_results->game_variant.has_teams();
        if (game_results->teams_enabled)
        {
            for (unsigned long player_index = 0; player_index < 16; ++player_index)
            {
                const s_game_results_player_data *player = &game_results->players[player_index];
                if (player->player_exists)
                {
                    long team_index = ((const s_game_results_player_data *)player)->configuration.host.team_index;
                    if (team_index >= 0 && (unsigned long)team_index < 16 && !game_results->teams.team[team_index].team_exists)
                    {
                        game_results->teams.team[team_index].team_exists = true;
                        game_results->teams.team[team_index].standing = '\xff';
                        game_results->teams.team[team_index].final_score = 0;
                    }
                }
            }
        }
        for (unsigned long entry_index = 0; entry_index < 16; ++entry_index)
        {
            s_entry *entry = &game_results->entries[entry_index];
            if (entry->valid)
            {
                entry->host = 0;
                entry->initial_host = 0;
            }
        }
        game_results->initialized = true;
        if (!game_results_options_simulation_is_client())
        {
            if (!game_results_options_recording_blocked())
            {
                g_game_results_globals.active = true;
                g_game_results_globals.finished = false;
                game_results_notify_player_indices_changed();
                game_results_notify_active_teams_changed();
            }
        }
        unsigned __int64 xuid;
        unsigned __int64 machine_id = 0;
        memset(&xuid, 0, sizeof(xuid));
        for (long user_index = 0; user_index < 4; ++user_index)
        {
            if (XUserGetXUID(user_index, &xuid) == 0)
                break;
        }
        XNADDR xnaddr;
        unsigned long flags = XNetGetTitleXnAddr(&xnaddr);
        if ((flags & 0x80) != 0)
            XNetXnAddrToMachineId(&xnaddr, &machine_id);
        game_results_update_network_statistics();
    }
}


class c_simulation_watcher;

static __forceinline bool game_results_get_view_statistics(c_simulation_view *view, long *bandwidth_event_counters)
{
    const c_simulation_view *view_data = view;
    bool result = false;
    long channel_index = view_data->m_observer_channel_index;
    if (channel_index != NONE)
    {
        const c_network_observer::s_channel_observer *channel = &view_data->m_observer->m_channel_observers[channel_index];
        memcpy(bandwidth_event_counters, channel->bandwidth_events, sizeof(channel->bandwidth_events));
        result = true;
    }
    return result;
}

static __forceinline const long &game_results_clamp_high(const long &value) { return value < 32767 ? value : k_game_results_maximum_statistic; }
template<typename t_type> static __forceinline const t_type &game_results_max(const t_type &a, const t_type &b) { return a > b ? a : b; }




static __forceinline void game_results_apply_statistic_update(
    const s_game_results_statistic *update, long count, s_game_results_statistic *statistics, long statistic_count)
{
    for (long index = 0; index < count; ++index)
    {
        if (update[index].flag)
            statistics[index].value = update[index].value;
    }
}

static __forceinline e_achievement operator++(e_achievement &achievement, int)
{
    e_achievement result = achievement;
    achievement = (e_achievement)(achievement + 1);
    return result;
}

static __forceinline void game_results_apply_achievement_update(long player_index, const s_game_results_statistic *update)
{
    for (e_achievement achievement = _achievement_landfall; achievement < k_number_of_achievements; achievement++)
    {
        if (update[achievement].flag && update[achievement].value)
            game_results_grant_achievement_to_owner(player_index, achievement);
    }
}

// @address 0x2467b8 ?game_results_apply_incremental_update@@YAXPAUs_game_results_incremental@@PBUs_game_results_incremental_update@@@Z
void game_results_apply_incremental_update(s_game_results_incremental *results, const s_game_results_incremental_update *update)
{
    results->summary_valid = update->summary_valid;
    if (results->summary_valid)
        results->summary_value = update->summary_value;
    results->finish_valid = update->finish_valid;
    if (results->finish_valid)
        results->finish_value = update->finish_value;
    results->finalized = update->finalized;
    for (long index = 0; index < 16; ++index)
    {
        const s_game_results_packet_player *player_update = &update->players[index];
        s_game_results_player_data *player = &results->players.player[index];
        if (player_update->exists)
        {
            player->player_exists = player_update->definition.player_exists;
            player->standing = player_update->definition.standing;
            if (player_update->has_definition)
                *player = player_update->definition;
        }
    }
    for (long index = 0; index < 16; ++index)
    {
        const s_game_results_packet_team *team_update = &update->teams[index];
        s_game_results_team_data *team = &results->teams.team[index];
        if (team_update->exists)
        {
            team->team_exists = team_update->flag;
            team->standing = team_update->standing;
            team->final_score = team_update->final_score;
            if (team_update->has_identifier)
                *team = *(const s_game_results_team_data *)team_update->identifier;
        }
    }
    for (long index = 0; index < 16; ++index)
    {
        const s_game_results_packet_player_statistics *player_update = &update->player_statistics[index];
        if (player_update->exists)
        {
            if (player_update->statistics_valid)
                game_results_apply_statistic_update(player_update->statistics, 51, results->statistics.values.player[index].statistics, 51);
            if (player_update->statistics_58_valid)
                game_results_apply_statistic_update(player_update->statistics_58, 58, results->statistics.values.player[index].medals, 58);
            if (player_update->statistics_49_valid)
            {
                game_results_apply_statistic_update(player_update->statistics_49, 49, results->statistics.values.player[index].achievements, 49);
                game_results_apply_achievement_update(index, player_update->statistics_49);
            }
            for (long damage_index = 0; damage_index < 61; ++damage_index)
            {
                if (player_update->damage[damage_index].active)
                    game_results_apply_statistic_update(player_update->damage[damage_index].values, 5,
                        results->statistics.values.player[index].damage_statistics[damage_index].values, 5);
            }
        }
    }
    long player_vs_player_index = 0;
    for (long index = 0; index < 16; ++index)
    {
        for (long opponent = 0; opponent < 16; ++opponent)
        {
            const s_game_results_packet_player_vs_player_statistics *player_update = &update->player_vs_player_statistics[index][opponent];
            if (player_update->valid)
                for (long statistic_index = 0; statistic_index < 2; ++statistic_index)
                    game_results_apply_statistic_update(player_update->statistics, 2, results->statistics.values.pvp_statistics[index][opponent], 2);
            player_vs_player_index++;
        }
    }
    for (long index = 0; index < 16; ++index)
    {
        const s_game_results_packet_team_statistics *team_update = &update->team_statistics[index];
        if (team_update->valid)
            game_results_apply_statistic_update(team_update->statistics, 51, results->statistics.values.team[index].statistics, 51);
    }
    for (long index = 0; index < 16; ++index)
    {
        const s_game_results_packet_machine *machine_update = &update->machines[index];
        s_entry *machine = &results->machines.machine[index];
        if (machine_update->valid)
            *machine = *(const s_entry *)machine_update->identifier;
    }
}


static __forceinline void game_results_calculate_statistics_update(const s_game_results_statistic *statistics,
    const s_game_results_statistic *previous_statistics, long statistic_count, bool *update_valid,
    s_game_results_statistic *update_statistics, long update_statistic_count)
{
    for (long statistic_index = 0; statistic_index < statistic_count; ++statistic_index)
    {
        if (statistics[statistic_index].value != previous_statistics[statistic_index].value)
        {
            update_statistics[statistic_index].flag = true;
            update_statistics[statistic_index].value = statistics[statistic_index].value;
            *update_valid = true;
        }
    }
}

// @address 0x245fb8 ?game_results_calculate_incremental_update@@YA_NPBUs_game_results_incremental@@PAUs_game_results_incremental_update@@@Z
bool game_results_calculate_incremental_update(const s_game_results_incremental *previous_state, s_game_results_incremental_update *incremental_update)
{
    s_game_results_player_data test_player;
    const c_game_results *game_results = &g_current_game_results;
    bool update_needed = false;

    incremental_update->summary_valid = game_results->summary_valid;
    if (incremental_update->summary_valid)
        incremental_update->summary_value = game_results->end_reason;
    incremental_update->finish_valid = game_results->finish_valid;
    if (incremental_update->finish_valid)
        incremental_update->finish_value = game_results->finish_value;
    incremental_update->finalized = game_results->finalized;
    if (incremental_update->summary_valid != previous_state->summary_valid ||
        (unsigned long)incremental_update->summary_value != (unsigned long)previous_state->summary_value ||
        incremental_update->finish_valid != previous_state->finish_valid ||
        (unsigned long)incremental_update->finish_value != (unsigned long)previous_state->finish_value ||
        incremental_update->finalized != previous_state->finalized)
    {
        update_needed = true;
    }

    for (long player_index = 0; player_index < 16; ++player_index)
    {
        s_game_results_packet_player *player_update = &incremental_update->players[player_index];
        const s_game_results_player_data *player = &game_results->players[player_index];
        const s_game_results_player_data *previous_player = &previous_state->players.player[player_index];
        if (memcmp(player, previous_player, sizeof(s_game_results_player_data)) != 0)
        {
            player_update->exists = true;
            player_update->definition.player_exists = player->player_exists;
            player_update->definition.standing = player->standing;
            test_player = *previous_player;
            test_player.player_exists = player->player_exists;
            test_player.standing = player->standing;
            if (memcmp(&test_player, player, sizeof(s_game_results_player_data)) != 0)
            {
                player_update->definition = *player;
                player_update->has_definition = true;
            }
            update_needed = true;
        }
    }

    for (long team_index = 0; team_index < 16; ++team_index)
    {
        s_game_results_packet_team *team_update = &incremental_update->teams[team_index];
        const s_game_results_team_data *team = &game_results->teams.team[team_index];
        const s_game_results_team_data *previous_team = &previous_state->teams.team[team_index];
        if (memcmp(team, previous_team, sizeof(s_game_results_team_data)) != 0)
        {
            team_update->exists = true;
            team_update->flag = team->team_exists;
            team_update->standing = team->standing;
            team_update->final_score = team->final_score;
            s_game_results_team_data test_team = *previous_team;
            test_team.team_exists = team->team_exists;
            test_team.standing = team->standing;
            test_team.final_score = team->final_score;
            if (memcmp(&test_team, team, sizeof(s_game_results_team_data)) != 0)
            {
                *(s_game_results_team_data *)team_update->identifier = *team;
                team_update->has_identifier = true;
            }
            update_needed = true;
        }
    }

    for (long absolute_player_index = 0; absolute_player_index < 16; ++absolute_player_index)
    {
        bool player_update_needed = false;
        game_results_calculate_statistics_update(
            game_results->statistics.player[absolute_player_index].game_statistics,
            previous_state->statistics.values.player[absolute_player_index].game_statistics,
            51,
            &incremental_update->player_statistics[absolute_player_index].statistics_valid,
            incremental_update->player_statistics[absolute_player_index].statistics,
            51);
        player_update_needed |= incremental_update->player_statistics[absolute_player_index].statistics_valid;
        game_results_calculate_statistics_update(
            game_results->statistics.player[absolute_player_index].medals,
            previous_state->statistics.values.player[absolute_player_index].medals,
            58,
            &incremental_update->player_statistics[absolute_player_index].statistics_58_valid,
            incremental_update->player_statistics[absolute_player_index].statistics_58,
            58);
        player_update_needed |= incremental_update->player_statistics[absolute_player_index].statistics_58_valid;
        game_results_calculate_statistics_update(
            game_results->statistics.player[absolute_player_index].achievements,
            previous_state->statistics.values.player[absolute_player_index].achievements,
            49,
            &incremental_update->player_statistics[absolute_player_index].statistics_49_valid,
            incremental_update->player_statistics[absolute_player_index].statistics_49,
            49);
        player_update_needed |= incremental_update->player_statistics[absolute_player_index].statistics_49_valid;
        for (long damage_index = 0; damage_index < 61; ++damage_index)
        {
            game_results_calculate_statistics_update(
                game_results->statistics.player[absolute_player_index].damage_statistics[damage_index].values,
                previous_state->statistics.values.player[absolute_player_index].damage_statistics[damage_index].values,
                5,
                &incremental_update->player_statistics[absolute_player_index].damage[damage_index].active,
                incremental_update->player_statistics[absolute_player_index].damage[damage_index].values,
                5);
            player_update_needed |= incremental_update->player_statistics[absolute_player_index].damage[damage_index].active;
        }
        incremental_update->player_statistics[absolute_player_index].exists = player_update_needed;
        update_needed |= player_update_needed;
    }

    for (long player_index = 0; player_index < 16; ++player_index)
    {
        for (long opponent_index = 0; opponent_index < 16; ++opponent_index)
        {
            game_results_calculate_statistics_update(
                game_results->statistics.pvp_statistics[player_index][opponent_index],
                previous_state->statistics.values.pvp_statistics[player_index][opponent_index],
                2,
                &incremental_update->player_vs_player_statistics[player_index][opponent_index].valid,
                incremental_update->player_vs_player_statistics[player_index][opponent_index].statistics,
                2);
            update_needed |= incremental_update->player_vs_player_statistics[player_index][opponent_index].valid;
        }
    }

    for (long team_index = 0; team_index < 16; ++team_index)
    {
        game_results_calculate_statistics_update(
            game_results->statistics.team[team_index].statistics,
            previous_state->statistics.values.team[team_index].statistics,
            51,
            &incremental_update->team_statistics[team_index].valid,
            incremental_update->team_statistics[team_index].statistics,
            51);
        update_needed |= incremental_update->team_statistics[team_index].valid;
    }

    for (long machine_index = 0; machine_index < 16; ++machine_index)
    {
        s_game_results_packet_machine *machine_update = &incremental_update->machines[machine_index];
        const s_entry *machine = &game_results->entries[machine_index];
        const s_entry *previous_machine = &previous_state->machines.machine[machine_index];
        if (memcmp(machine, previous_machine, sizeof(s_entry)) != 0)
        {
            *(s_entry *)machine_update->identifier = *machine;
            machine_update->valid = true;
            update_needed = true;
        }
    }

    return update_needed;
}
// @address 0xd3f88 ?game_results_update@@YAXXZ
void game_results_update()
{
    if (game_results_get_game_recording() && !g_current_game_results.finalized)
    {
        c_game_results *results = &g_current_game_results;
        for (long player_index = 0; player_index < 16; ++player_index)
        {
            s_game_results_player_data *result_player_data = &results->players[player_index];
            player_datum *player = (player_datum *)datum_try_and_get_absolute(player_data, player_index);
            if (player)
            {
                if (results->hopper_identifier == 0xffff)
                    memcpy(&result_player_data->configuration, &player->configuration, sizeof(result_player_data->configuration));
                result_player_data->machine_index = NONE;
                results->statistics.player[player_index].values[0] = (results->statistics.player[player_index].values[0] & 1) | 2;
            }
            else if (result_player_data->player_exists && !g_game_results_player_left)
                g_game_results_player_left = true;
        }
        if (game_engine_globals->flags & 1UL)
        {
            s_game_engine_score_list scores;
            game_engine_scoring_build_score_list(&scores, 1, false);
            long team_count = scores.team_count;
            for (long team_index = 0; team_index < team_count; ++team_index)
            {
                e_game_team team = (e_game_team)scores.team_indices[team_index];
                s_game_results_team_data *result_team_data = &results->teams.team[team];
                long standing = scores.team_ranks[team_index] / 2;
                result_team_data->standing = (signed char)standing;
                long rounds = game_engine_get_team_rounds_won(team);
                result_team_data->final_score = (short)game_results_clamp_high(game_results_max(rounds, 0L));
            }
            long player_count = scores.player_count;
            for (long player_index = 0; player_index < player_count; ++player_index)
            {
                s_game_results_player_data *result_player_data = &results->players[datum_index_to_absolute_index(scores.player_indices[player_index])];
                long standing = scores.player_ranks[player_index] / 2;
                result_player_data->standing = (signed char)standing;
            }
        }
        results->fixup_player_machine_references();
        s_simulation_globals *simulation_globals = &g_simulation_globals;
        long view_statistics[5];
        const s_machine_identifier *local_machine_identifier = &g_transport_security_globals.local_machine_identifier;
        for (long machine_index = 0; machine_index < 16; ++machine_index)
        {
            s_entry *entry = &results->entries[machine_index];
            if (entry->valid)
            {
                if (memcmp(local_machine_identifier->data, entry->identifier.data, sizeof(entry->identifier.data)) == 0)
                {
                    entry->host = true;
                    entry->joined = true;
                }
                else
                {
                    entry->host = false;
                    entry->joined = simulation_globals->watcher->get_machine_connectivity(&entry->identifier);
                    bool have_statistics = false;
                    const unsigned long &initialized = simulation_globals->initialized;
                    if (initialized)
                    {
                        c_simulation_world *world = simulation_globals->world;
                        const int &state = world->m_world_type;
                        if (state)
                        {
                            c_simulation_view *view = world->get_client_view_by_machine_identifier(&entry->identifier);
                            if (view && game_results_get_view_statistics(view, view_statistics))
                                have_statistics = true;
                        }
                    }
                    if (have_statistics)
                    {
                        entry->statistics[0] = (short)PIN(view_statistics[0], 0L, 32767L);
                        entry->statistics[1] = (short)PIN(view_statistics[1], 0L, 32767L);
                        entry->statistics[2] = (short)PIN(view_statistics[2], 0L, 32767L);
                        entry->statistics[3] = (short)PIN(view_statistics[3], 0L, 32767L);
                        entry->statistics[4] = (short)PIN(view_statistics[4], 0L, 32767L);
                    }
                }
            }
        }
        if (!results->summary_valid)
        {
            long host_machine_index = results->get_host_machine_index();
            results->summary_valid = true;
            results->end_reason = (long)_time64(0);
            if (host_machine_index != NONE)
                results->entries[host_machine_index].initial_host = true;
            game_results_update_network_statistics();
        }
    }
}
