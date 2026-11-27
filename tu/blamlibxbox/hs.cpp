// blamlibxbox/hs.obj: from the records 0xab9c0 0x241870 0x241940 0x241950 0x241a18 0x241a90 0x241ad0
// @flags /TP /O2
#include "memory/data_types.h"
#include "memory/runtime_state_allocation.h"
#if !defined(_XBOX) || defined(__clang__)
#include "ai/ai_debug_throttled_global.h"
#endif
// @address 0xab9c0 ?hs_global_get_type@@YAFF@Z
#include "ai/ai_script.h"
#include "hs/hs.h"
#include "hs/hs_compile.h"
#include "hs/hs_runtime.h"
#include "hs/hs_runtime_data.h"
#include "memory/data.h"
#include "memory/datum_index_inlines.h"
#include "memory/normal_allocation.h"
#include "cseries/cseries.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "hs/hs.h"
#include "hs/hs_thread_types.h"
#include "hs/hs_thread_iterator_types.h"
#include "hs/object_lists.h"

// Index with the top bit (0x8000) set names a static entry in
// hs_external_globals; otherwise it is an entry of global_scenario's table.
short hs_global_get_type(short global_index)
{
  if ((global_index & 0x8000) != 0) {
    return (short)hs_external_globals[global_index & 0x7fff]->type;
  }
  return ((s_hs_global_definition *)global_scenario->globals.address)[global_index & 0x7fff].type;
}

namespace {
static __forceinline void hs_runtime_dispose()
{
    tls_hs_global_data()->valid = false;
}
static __forceinline void object_lists_dispose()
{
}
static __forceinline void object_lists_dispose_from_old_map()
{
    tls_object_list_header_data()->valid = false;
    tls_list_object_reference_data()->valid = false;
}
}
// @address 0x74b80c g_runtime_state_allocation

#if !defined(_XBOX) || defined(__clang__)
// The identifier arrays are the shipped .data string tables (GAME-CHANGES F126), named as in the March build, where
// each belongs to the object noted; the Xbox table holds their addresses. Counts and order are the image's.
static const char *global_campaign_difficulty_level_names[4] = { // 0x8274b568, game.obj
    "easy", "normal", "heroic", "legendary",
};
static const char *global_campaign_team_names[16] = { // 0x82745cf0, game_allegiance.obj
    "default", "player", "human", "covenant", "flood", "sentinel", "heretic", "prophet", "guilty", "unused9",
    "unused10", "unused11", "unused12", "unused13", "unused14", "unused15",
};
static const char *global_multiplayer_team_names[8] = { // 0x82744c94, game_engine_team.obj
    "mp_team_red", "mp_team_blue", "mp_team_green", "mp_team_yellow", "mp_team_purple", "mp_team_orange",
    "mp_team_brown", "mp_team_grey",
};
static const char *global_controller_index_names[4] = { // 0x82730464, c_controller.obj
    "controller1", "controller2", "controller3", "controller4",
};
static const char *global_button_preset_names[6] = { // 0x82730474, c_controller.obj
    "standard", "south_paw", "boxer", "green_thumb", "professional", "walkie-talkie",
};
static const char *global_joystick_preset_names[4] = { // 0x8273048c, c_controller.obj
    "standard", "south_paw", "legacy", "legacy_south_paw",
};
static const char *global_player_color_index_names[32] = { // 0x827304a0, c_controller.obj
    "player_color_00", "player_color_01", "player_color_02", "player_color_03", "player_color_04",
    "player_color_05", "player_color_06", "player_color_07", "player_color_08", "player_color_09",
    "player_color_10", "player_color_11", "player_color_12", "player_color_13", "player_color_14",
    "player_color_15", "player_color_16", "player_color_17", "player_color_18", "player_color_19",
    "player_color_20", "player_color_21", "player_color_22", "player_color_23", "player_color_24",
    "player_color_25", "player_color_26", "player_color_27", "player_color_28", "player_color_29",
    "player_color_30", "player_color_31",
};
static const char *global_player_character_type_names[6] = { // 0x82730520, c_controller.obj
    "sp_masterchief", "sp_dervish", "mp_masterchief", "mp_elite", "co-op elite", "monitor",
};
static const char *global_voice_output_setting_names[2] = { // 0x82730538, c_controller.obj
    "default", "headset",
};
static const char *global_voice_mask_names[2] = { // 0x82730540, c_controller.obj
    "none", "anonymous",
};
static const char *global_subtitle_setting_names[3] = { // 0x8272fe2c, c_controller.obj
    "automatic", "enabled", "disabled",
};
static const char *global_actor_type_names[25] = { // 0x8272fb00, actor_types.obj
    "elite", "jackal", "grunt", "hunter", "engineer", "assassin", "player", "marine", "crew", "combat_form",
    "infection_form", "carrier_form", "monitor", "sentinel", "none", "mounted_weapon", "brute", "prophet", "bugger",
    "juggernaut", "pure_form_stealth", "pure_form_tank", "pure_form_ranged", "scarab", "guardian",
};
static const char *global_model_state_names[5] = { // 0x8273fe3c, model_definitions.obj
    "standard", "minor damage", "medium damage", "major damage", "destroyed",
};
static const char *k_event_level_names[7] = { // 0x8273e73c, cseries_events.obj
    "verbose", "status", "message", "warning", "error", "critical", "none",
};
static const char *global_character_physics_override_names[3] = { // 0x82744494, character_physics.obj
    "NONE", "player", "biped",
};
#endif

// The script-enum table: one entry per hs type from _hs_type_game_difficulty (50) through
// _hs_type_character_physics (64). Counts and identifier-array addresses are the shipped bytes.
// @rdata 0x82034380..0x820343f8
// @address 0x34380 ?hs_enum_table@@3QBUhs_enum_definition@@B
const hs_enum_definition hs_enum_table[] = {
#if defined(_XBOX) && !defined(__clang__)
    { 4, (const char **)0x8274b568 },
    { 16, (const char **)0x82745cf0 },
    { 8, (const char **)0x82744c94 },
    { 4, (const char **)0x82730464 },
    { 6, (const char **)0x82730474 },
    { 4, (const char **)0x8273048c },
    { 32, (const char **)0x827304a0 },
    { 6, (const char **)0x82730520 },
    { 2, (const char **)0x82730538 },
    { 2, (const char **)0x82730540 },
    { 3, (const char **)0x8272fe2c },
    { 25, (const char **)0x8272fb00 },
    { 5, (const char **)0x8273fe3c },
    { 7, (const char **)0x8273e73c },
    { 3, (const char **)0x82744494 },
#else
    { 4, global_campaign_difficulty_level_names },
    { 16, global_campaign_team_names },
    { 8, global_multiplayer_team_names },
    { 4, global_controller_index_names },
    { 6, global_button_preset_names },
    { 4, global_joystick_preset_names },
    { 32, global_player_color_index_names },
    { 6, global_player_character_type_names },
    { 2, global_voice_output_setting_names },
    { 2, global_voice_mask_names },
    { 3, global_subtitle_setting_names },
    { 25, global_actor_type_names },
    { 5, global_model_state_names },
    { 7, k_event_level_names },
    { 3, global_character_physics_override_names },
#endif
};
// @rdata-end

// The engine-globals table (March hs_globals_external.obj keeps its size in
// k_hs_external_global_count = 1098 there; this build has 1262). Entries are verbatim image bytes; the structs
// form one array at 0x8273ff10, so each entry is 8 past the last, except entry 1238, whose
// record is at 0x827c5f2c outside the array.
// @address 0x742678 ?hs_external_globals@@3PAPAUhs_global_external@@A
hs_global_external *hs_external_globals[] = {
    (hs_global_external *)0x8273ff10, (hs_global_external *)0x8273ff18, (hs_global_external *)0x8273ff20, (hs_global_external *)0x8273ff28,
    (hs_global_external *)0x8273ff30, (hs_global_external *)0x8273ff38, (hs_global_external *)0x8273ff40, (hs_global_external *)0x8273ff48,
    (hs_global_external *)0x8273ff50, (hs_global_external *)0x8273ff58, (hs_global_external *)0x8273ff60, (hs_global_external *)0x8273ff68,
    (hs_global_external *)0x8273ff70, (hs_global_external *)0x8273ff78, (hs_global_external *)0x8273ff80, (hs_global_external *)0x8273ff88,
    (hs_global_external *)0x8273ff90, (hs_global_external *)0x8273ff98, (hs_global_external *)0x8273ffa0, (hs_global_external *)0x8273ffa8,
    (hs_global_external *)0x8273ffb0, (hs_global_external *)0x8273ffb8, (hs_global_external *)0x8273ffc0, (hs_global_external *)0x8273ffc8,
    (hs_global_external *)0x8273ffd0, (hs_global_external *)0x8273ffd8, (hs_global_external *)0x8273ffe0, (hs_global_external *)0x8273ffe8,
    (hs_global_external *)0x8273fff0, (hs_global_external *)0x8273fff8, (hs_global_external *)0x82740000, (hs_global_external *)0x82740008,
    (hs_global_external *)0x82740010, (hs_global_external *)0x82740018, (hs_global_external *)0x82740020, (hs_global_external *)0x82740028,
    (hs_global_external *)0x82740030, (hs_global_external *)0x82740038, (hs_global_external *)0x82740040, (hs_global_external *)0x82740048,
    (hs_global_external *)0x82740050, (hs_global_external *)0x82740058, (hs_global_external *)0x82740060, (hs_global_external *)0x82740068,
    (hs_global_external *)0x82740070, (hs_global_external *)0x82740078, (hs_global_external *)0x82740080, (hs_global_external *)0x82740088,
    (hs_global_external *)0x82740090, (hs_global_external *)0x82740098, (hs_global_external *)0x827400a0, (hs_global_external *)0x827400a8,
    (hs_global_external *)0x827400b0, (hs_global_external *)0x827400b8, (hs_global_external *)0x827400c0, (hs_global_external *)0x827400c8,
    (hs_global_external *)0x827400d0, (hs_global_external *)0x827400d8, (hs_global_external *)0x827400e0, (hs_global_external *)0x827400e8,
    (hs_global_external *)0x827400f0, (hs_global_external *)0x827400f8, (hs_global_external *)0x82740100, (hs_global_external *)0x82740108,
    (hs_global_external *)0x82740110, (hs_global_external *)0x82740118, (hs_global_external *)0x82740120, (hs_global_external *)0x82740128,
    (hs_global_external *)0x82740130, (hs_global_external *)0x82740138, (hs_global_external *)0x82740140, (hs_global_external *)0x82740148,
    (hs_global_external *)0x82740150, (hs_global_external *)0x82740158, (hs_global_external *)0x82740160, (hs_global_external *)0x82740168,
    (hs_global_external *)0x82740170, (hs_global_external *)0x82740178, (hs_global_external *)0x82740180, (hs_global_external *)0x82740188,
    (hs_global_external *)0x82740190, (hs_global_external *)0x82740198, (hs_global_external *)0x827401a0, (hs_global_external *)0x827401a8,
    (hs_global_external *)0x827401b0, (hs_global_external *)0x827401b8, (hs_global_external *)0x827401c0, (hs_global_external *)0x827401c8,
    (hs_global_external *)0x827401d0, (hs_global_external *)0x827401d8, (hs_global_external *)0x827401e0, (hs_global_external *)0x827401e8,
    (hs_global_external *)0x827401f0, (hs_global_external *)0x827401f8, (hs_global_external *)0x82740200, (hs_global_external *)0x82740208,
    (hs_global_external *)0x82740210, (hs_global_external *)0x82740218, (hs_global_external *)0x82740220, (hs_global_external *)0x82740228,
    (hs_global_external *)0x82740230, (hs_global_external *)0x82740238, (hs_global_external *)0x82740240, (hs_global_external *)0x82740248,
    (hs_global_external *)0x82740250, (hs_global_external *)0x82740258, (hs_global_external *)0x82740260, (hs_global_external *)0x82740268,
    (hs_global_external *)0x82740270, (hs_global_external *)0x82740278, (hs_global_external *)0x82740280, (hs_global_external *)0x82740288,
    (hs_global_external *)0x82740290, (hs_global_external *)0x82740298, (hs_global_external *)0x827402a0, (hs_global_external *)0x827402a8,
    (hs_global_external *)0x827402b0, (hs_global_external *)0x827402b8, (hs_global_external *)0x827402c0, (hs_global_external *)0x827402c8,
    (hs_global_external *)0x827402d0, (hs_global_external *)0x827402d8, (hs_global_external *)0x827402e0, (hs_global_external *)0x827402e8,
    (hs_global_external *)0x827402f0, (hs_global_external *)0x827402f8, (hs_global_external *)0x82740300, (hs_global_external *)0x82740308,
    (hs_global_external *)0x82740310, (hs_global_external *)0x82740318, (hs_global_external *)0x82740320, (hs_global_external *)0x82740328,
    (hs_global_external *)0x82740330, (hs_global_external *)0x82740338, (hs_global_external *)0x82740340, (hs_global_external *)0x82740348,
    (hs_global_external *)0x82740350, (hs_global_external *)0x82740358, (hs_global_external *)0x82740360, (hs_global_external *)0x82740368,
    (hs_global_external *)0x82740370, (hs_global_external *)0x82740378, (hs_global_external *)0x82740380, (hs_global_external *)0x82740388,
    (hs_global_external *)0x82740390, (hs_global_external *)0x82740398, (hs_global_external *)0x827403a0, (hs_global_external *)0x827403a8,
    (hs_global_external *)0x827403b0, (hs_global_external *)0x827403b8, (hs_global_external *)0x827403c0, (hs_global_external *)0x827403c8,
    (hs_global_external *)0x827403d0, (hs_global_external *)0x827403d8, (hs_global_external *)0x827403e0, (hs_global_external *)0x827403e8,
    (hs_global_external *)0x827403f0, (hs_global_external *)0x827403f8, (hs_global_external *)0x82740400, (hs_global_external *)0x82740408,
    (hs_global_external *)0x82740410, (hs_global_external *)0x82740418, (hs_global_external *)0x82740420, (hs_global_external *)0x82740428,
    (hs_global_external *)0x82740430, (hs_global_external *)0x82740438, (hs_global_external *)0x82740440, (hs_global_external *)0x82740448,
    (hs_global_external *)0x82740450, (hs_global_external *)0x82740458, (hs_global_external *)0x82740460, (hs_global_external *)0x82740468,
    (hs_global_external *)0x82740470, (hs_global_external *)0x82740478, (hs_global_external *)0x82740480, (hs_global_external *)0x82740488,
    (hs_global_external *)0x82740490, (hs_global_external *)0x82740498, (hs_global_external *)0x827404a0, (hs_global_external *)0x827404a8,
    (hs_global_external *)0x827404b0, (hs_global_external *)0x827404b8, (hs_global_external *)0x827404c0, (hs_global_external *)0x827404c8,
    (hs_global_external *)0x827404d0, (hs_global_external *)0x827404d8, (hs_global_external *)0x827404e0, (hs_global_external *)0x827404e8,
    (hs_global_external *)0x827404f0, (hs_global_external *)0x827404f8, (hs_global_external *)0x82740500, (hs_global_external *)0x82740508,
    (hs_global_external *)0x82740510, (hs_global_external *)0x82740518, (hs_global_external *)0x82740520, (hs_global_external *)0x82740528,
    (hs_global_external *)0x82740530, (hs_global_external *)0x82740538, (hs_global_external *)0x82740540, (hs_global_external *)0x82740548,
    (hs_global_external *)0x82740550, (hs_global_external *)0x82740558, (hs_global_external *)0x82740560, (hs_global_external *)0x82740568,
    (hs_global_external *)0x82740570, (hs_global_external *)0x82740578, (hs_global_external *)0x82740580, (hs_global_external *)0x82740588,
    (hs_global_external *)0x82740590, (hs_global_external *)0x82740598, (hs_global_external *)0x827405a0, (hs_global_external *)0x827405a8,
    (hs_global_external *)0x827405b0, (hs_global_external *)0x827405b8, (hs_global_external *)0x827405c0, (hs_global_external *)0x827405c8,
    (hs_global_external *)0x827405d0, (hs_global_external *)0x827405d8, (hs_global_external *)0x827405e0, (hs_global_external *)0x827405e8,
    (hs_global_external *)0x827405f0, (hs_global_external *)0x827405f8, (hs_global_external *)0x82740600, (hs_global_external *)0x82740608,
    (hs_global_external *)0x82740610, (hs_global_external *)0x82740618, (hs_global_external *)0x82740620, (hs_global_external *)0x82740628,
    (hs_global_external *)0x82740630, (hs_global_external *)0x82740638, (hs_global_external *)0x82740640, (hs_global_external *)0x82740648,
    (hs_global_external *)0x82740650, (hs_global_external *)0x82740658, (hs_global_external *)0x82740660, (hs_global_external *)0x82740668,
    (hs_global_external *)0x82740670, (hs_global_external *)0x82740678, (hs_global_external *)0x82740680, (hs_global_external *)0x82740688,
    (hs_global_external *)0x82740690, (hs_global_external *)0x82740698, (hs_global_external *)0x827406a0, (hs_global_external *)0x827406a8,
    (hs_global_external *)0x827406b0, (hs_global_external *)0x827406b8, (hs_global_external *)0x827406c0, (hs_global_external *)0x827406c8,
    (hs_global_external *)0x827406d0, (hs_global_external *)0x827406d8, (hs_global_external *)0x827406e0, (hs_global_external *)0x827406e8,
    (hs_global_external *)0x827406f0, (hs_global_external *)0x827406f8, (hs_global_external *)0x82740700, (hs_global_external *)0x82740708,
    (hs_global_external *)0x82740710, (hs_global_external *)0x82740718, (hs_global_external *)0x82740720, (hs_global_external *)0x82740728,
    (hs_global_external *)0x82740730, (hs_global_external *)0x82740738, (hs_global_external *)0x82740740, (hs_global_external *)0x82740748,
    (hs_global_external *)0x82740750, (hs_global_external *)0x82740758, (hs_global_external *)0x82740760, (hs_global_external *)0x82740768,
    (hs_global_external *)0x82740770, (hs_global_external *)0x82740778, (hs_global_external *)0x82740780, (hs_global_external *)0x82740788,
    (hs_global_external *)0x82740790, (hs_global_external *)0x82740798, (hs_global_external *)0x827407a0, (hs_global_external *)0x827407a8,
    (hs_global_external *)0x827407b0, (hs_global_external *)0x827407b8, (hs_global_external *)0x827407c0, (hs_global_external *)0x827407c8,
    (hs_global_external *)0x827407d0, (hs_global_external *)0x827407d8, (hs_global_external *)0x827407e0, (hs_global_external *)0x827407e8,
    (hs_global_external *)0x827407f0, (hs_global_external *)0x827407f8, (hs_global_external *)0x82740800, (hs_global_external *)0x82740808,
    (hs_global_external *)0x82740810, (hs_global_external *)0x82740818, (hs_global_external *)0x82740820, (hs_global_external *)0x82740828,
    (hs_global_external *)0x82740830, (hs_global_external *)0x82740838, (hs_global_external *)0x82740840, (hs_global_external *)0x82740848,
    (hs_global_external *)0x82740850, (hs_global_external *)0x82740858, (hs_global_external *)0x82740860, (hs_global_external *)0x82740868,
    (hs_global_external *)0x82740870, (hs_global_external *)0x82740878, (hs_global_external *)0x82740880, (hs_global_external *)0x82740888,
    (hs_global_external *)0x82740890, (hs_global_external *)0x82740898, (hs_global_external *)0x827408a0, (hs_global_external *)0x827408a8,
    (hs_global_external *)0x827408b0, (hs_global_external *)0x827408b8, (hs_global_external *)0x827408c0, (hs_global_external *)0x827408c8,
    (hs_global_external *)0x827408d0, (hs_global_external *)0x827408d8, (hs_global_external *)0x827408e0, (hs_global_external *)0x827408e8,
    (hs_global_external *)0x827408f0, (hs_global_external *)0x827408f8, (hs_global_external *)0x82740900, (hs_global_external *)0x82740908,
    (hs_global_external *)0x82740910, (hs_global_external *)0x82740918, (hs_global_external *)0x82740920, (hs_global_external *)0x82740928,
    (hs_global_external *)0x82740930, (hs_global_external *)0x82740938, (hs_global_external *)0x82740940, (hs_global_external *)0x82740948,
    (hs_global_external *)0x82740950, (hs_global_external *)0x82740958, (hs_global_external *)0x82740960, (hs_global_external *)0x82740968,
    (hs_global_external *)0x82740970, (hs_global_external *)0x82740978, (hs_global_external *)0x82740980, (hs_global_external *)0x82740988,
    (hs_global_external *)0x82740990, (hs_global_external *)0x82740998, (hs_global_external *)0x827409a0, (hs_global_external *)0x827409a8,
    (hs_global_external *)0x827409b0, (hs_global_external *)0x827409b8, (hs_global_external *)0x827409c0, (hs_global_external *)0x827409c8,
    (hs_global_external *)0x827409d0, (hs_global_external *)0x827409d8, (hs_global_external *)0x827409e0, (hs_global_external *)0x827409e8,
    (hs_global_external *)0x827409f0, (hs_global_external *)0x827409f8, (hs_global_external *)0x82740a00, (hs_global_external *)0x82740a08,
    (hs_global_external *)0x82740a10, (hs_global_external *)0x82740a18, (hs_global_external *)0x82740a20, (hs_global_external *)0x82740a28,
    (hs_global_external *)0x82740a30, (hs_global_external *)0x82740a38, (hs_global_external *)0x82740a40, (hs_global_external *)0x82740a48,
    (hs_global_external *)0x82740a50, (hs_global_external *)0x82740a58, (hs_global_external *)0x82740a60, (hs_global_external *)0x82740a68,
    (hs_global_external *)0x82740a70, (hs_global_external *)0x82740a78, (hs_global_external *)0x82740a80, (hs_global_external *)0x82740a88,
    (hs_global_external *)0x82740a90, (hs_global_external *)0x82740a98, (hs_global_external *)0x82740aa0, (hs_global_external *)0x82740aa8,
    (hs_global_external *)0x82740ab0, (hs_global_external *)0x82740ab8, (hs_global_external *)0x82740ac0, (hs_global_external *)0x82740ac8,
    (hs_global_external *)0x82740ad0, (hs_global_external *)0x82740ad8, (hs_global_external *)0x82740ae0, (hs_global_external *)0x82740ae8,
    (hs_global_external *)0x82740af0, (hs_global_external *)0x82740af8, (hs_global_external *)0x82740b00, (hs_global_external *)0x82740b08,
    (hs_global_external *)0x82740b10, (hs_global_external *)0x82740b18, (hs_global_external *)0x82740b20, (hs_global_external *)0x82740b28,
    (hs_global_external *)0x82740b30, (hs_global_external *)0x82740b38, (hs_global_external *)0x82740b40, (hs_global_external *)0x82740b48,
    (hs_global_external *)0x82740b50, (hs_global_external *)0x82740b58, (hs_global_external *)0x82740b60, (hs_global_external *)0x82740b68,
    (hs_global_external *)0x82740b70, (hs_global_external *)0x82740b78, (hs_global_external *)0x82740b80, (hs_global_external *)0x82740b88,
    (hs_global_external *)0x82740b90, (hs_global_external *)0x82740b98, (hs_global_external *)0x82740ba0, (hs_global_external *)0x82740ba8,
    (hs_global_external *)0x82740bb0, (hs_global_external *)0x82740bb8, (hs_global_external *)0x82740bc0, (hs_global_external *)0x82740bc8,
    (hs_global_external *)0x82740bd0, (hs_global_external *)0x82740bd8, (hs_global_external *)0x82740be0, (hs_global_external *)0x82740be8,
    (hs_global_external *)0x82740bf0, (hs_global_external *)0x82740bf8, (hs_global_external *)0x82740c00, (hs_global_external *)0x82740c08,
    (hs_global_external *)0x82740c10, (hs_global_external *)0x82740c18, (hs_global_external *)0x82740c20, (hs_global_external *)0x82740c28,
    (hs_global_external *)0x82740c30, (hs_global_external *)0x82740c38, (hs_global_external *)0x82740c40, (hs_global_external *)0x82740c48,
    (hs_global_external *)0x82740c50, (hs_global_external *)0x82740c58, (hs_global_external *)0x82740c60, (hs_global_external *)0x82740c68,
    (hs_global_external *)0x82740c70, (hs_global_external *)0x82740c78, (hs_global_external *)0x82740c80, (hs_global_external *)0x82740c88,
    (hs_global_external *)0x82740c90, (hs_global_external *)0x82740c98, (hs_global_external *)0x82740ca0, (hs_global_external *)0x82740ca8,
    (hs_global_external *)0x82740cb0, (hs_global_external *)0x82740cb8, (hs_global_external *)0x82740cc0, (hs_global_external *)0x82740cc8,
    (hs_global_external *)0x82740cd0, (hs_global_external *)0x82740cd8, (hs_global_external *)0x82740ce0, (hs_global_external *)0x82740ce8,
    (hs_global_external *)0x82740cf0, (hs_global_external *)0x82740cf8, (hs_global_external *)0x82740d00, (hs_global_external *)0x82740d08,
    (hs_global_external *)0x82740d10, (hs_global_external *)0x82740d18, (hs_global_external *)0x82740d20, (hs_global_external *)0x82740d28,
    (hs_global_external *)0x82740d30, (hs_global_external *)0x82740d38, (hs_global_external *)0x82740d40, (hs_global_external *)0x82740d48,
    (hs_global_external *)0x82740d50, (hs_global_external *)0x82740d58, (hs_global_external *)0x82740d60, (hs_global_external *)0x82740d68,
    (hs_global_external *)0x82740d70, (hs_global_external *)0x82740d78, (hs_global_external *)0x82740d80, (hs_global_external *)0x82740d88,
    (hs_global_external *)0x82740d90, (hs_global_external *)0x82740d98, (hs_global_external *)0x82740da0, (hs_global_external *)0x82740da8,
    (hs_global_external *)0x82740db0, (hs_global_external *)0x82740db8, (hs_global_external *)0x82740dc0, (hs_global_external *)0x82740dc8,
    (hs_global_external *)0x82740dd0, (hs_global_external *)0x82740dd8, (hs_global_external *)0x82740de0, (hs_global_external *)0x82740de8,
    (hs_global_external *)0x82740df0, (hs_global_external *)0x82740df8, (hs_global_external *)0x82740e00, (hs_global_external *)0x82740e08,
    (hs_global_external *)0x82740e10, (hs_global_external *)0x82740e18, (hs_global_external *)0x82740e20, (hs_global_external *)0x82740e28,
    (hs_global_external *)0x82740e30, (hs_global_external *)0x82740e38, (hs_global_external *)0x82740e40, (hs_global_external *)0x82740e48,
    (hs_global_external *)0x82740e50, (hs_global_external *)0x82740e58, (hs_global_external *)0x82740e60, (hs_global_external *)0x82740e68,
    (hs_global_external *)0x82740e70, (hs_global_external *)0x82740e78, (hs_global_external *)0x82740e80, (hs_global_external *)0x82740e88,
    (hs_global_external *)0x82740e90, (hs_global_external *)0x82740e98, (hs_global_external *)0x82740ea0, (hs_global_external *)0x82740ea8,
    (hs_global_external *)0x82740eb0, (hs_global_external *)0x82740eb8, (hs_global_external *)0x82740ec0, (hs_global_external *)0x82740ec8,
    (hs_global_external *)0x82740ed0, (hs_global_external *)0x82740ed8, (hs_global_external *)0x82740ee0, (hs_global_external *)0x82740ee8,
    (hs_global_external *)0x82740ef0, (hs_global_external *)0x82740ef8, (hs_global_external *)0x82740f00, (hs_global_external *)0x82740f08,
    (hs_global_external *)0x82740f10, (hs_global_external *)0x82740f18, (hs_global_external *)0x82740f20, (hs_global_external *)0x82740f28,
    (hs_global_external *)0x82740f30, (hs_global_external *)0x82740f38, (hs_global_external *)0x82740f40, (hs_global_external *)0x82740f48,
    (hs_global_external *)0x82740f50, (hs_global_external *)0x82740f58, (hs_global_external *)0x82740f60, (hs_global_external *)0x82740f68,
    (hs_global_external *)0x82740f70, (hs_global_external *)0x82740f78, (hs_global_external *)0x82740f80, (hs_global_external *)0x82740f88,
    (hs_global_external *)0x82740f90, (hs_global_external *)0x82740f98, (hs_global_external *)0x82740fa0, (hs_global_external *)0x82740fa8,
    (hs_global_external *)0x82740fb0, (hs_global_external *)0x82740fb8, (hs_global_external *)0x82740fc0, (hs_global_external *)0x82740fc8,
    (hs_global_external *)0x82740fd0, (hs_global_external *)0x82740fd8, (hs_global_external *)0x82740fe0, (hs_global_external *)0x82740fe8,
    (hs_global_external *)0x82740ff0, (hs_global_external *)0x82740ff8, (hs_global_external *)0x82741000, (hs_global_external *)0x82741008,
    (hs_global_external *)0x82741010, (hs_global_external *)0x82741018, (hs_global_external *)0x82741020, (hs_global_external *)0x82741028,
    (hs_global_external *)0x82741030, (hs_global_external *)0x82741038, (hs_global_external *)0x82741040, (hs_global_external *)0x82741048,
    (hs_global_external *)0x82741050, (hs_global_external *)0x82741058, (hs_global_external *)0x82741060, (hs_global_external *)0x82741068,
    (hs_global_external *)0x82741070, (hs_global_external *)0x82741078, (hs_global_external *)0x82741080, (hs_global_external *)0x82741088,
    (hs_global_external *)0x82741090, (hs_global_external *)0x82741098, (hs_global_external *)0x827410a0, (hs_global_external *)0x827410a8,
    (hs_global_external *)0x827410b0, (hs_global_external *)0x827410b8, (hs_global_external *)0x827410c0, (hs_global_external *)0x827410c8,
    (hs_global_external *)0x827410d0, (hs_global_external *)0x827410d8, (hs_global_external *)0x827410e0, (hs_global_external *)0x827410e8,
    (hs_global_external *)0x827410f0, (hs_global_external *)0x827410f8, (hs_global_external *)0x82741100, (hs_global_external *)0x82741108,
    (hs_global_external *)0x82741110, (hs_global_external *)0x82741118, (hs_global_external *)0x82741120, (hs_global_external *)0x82741128,
    (hs_global_external *)0x82741130, (hs_global_external *)0x82741138, (hs_global_external *)0x82741140, (hs_global_external *)0x82741148,
    (hs_global_external *)0x82741150, (hs_global_external *)0x82741158, (hs_global_external *)0x82741160, (hs_global_external *)0x82741168,
    (hs_global_external *)0x82741170, (hs_global_external *)0x82741178, (hs_global_external *)0x82741180, (hs_global_external *)0x82741188,
    (hs_global_external *)0x82741190, (hs_global_external *)0x82741198, (hs_global_external *)0x827411a0, (hs_global_external *)0x827411a8,
    (hs_global_external *)0x827411b0, (hs_global_external *)0x827411b8, (hs_global_external *)0x827411c0, (hs_global_external *)0x827411c8,
    (hs_global_external *)0x827411d0, (hs_global_external *)0x827411d8, (hs_global_external *)0x827411e0, (hs_global_external *)0x827411e8,
    (hs_global_external *)0x827411f0, (hs_global_external *)0x827411f8, (hs_global_external *)0x82741200, (hs_global_external *)0x82741208,
    (hs_global_external *)0x82741210, (hs_global_external *)0x82741218, (hs_global_external *)0x82741220, (hs_global_external *)0x82741228,
    (hs_global_external *)0x82741230, (hs_global_external *)0x82741238, (hs_global_external *)0x82741240, (hs_global_external *)0x82741248,
    (hs_global_external *)0x82741250, (hs_global_external *)0x82741258, (hs_global_external *)0x82741260, (hs_global_external *)0x82741268,
    (hs_global_external *)0x82741270, (hs_global_external *)0x82741278, (hs_global_external *)0x82741280, (hs_global_external *)0x82741288,
    (hs_global_external *)0x82741290, (hs_global_external *)0x82741298, (hs_global_external *)0x827412a0, (hs_global_external *)0x827412a8,
    (hs_global_external *)0x827412b0, (hs_global_external *)0x827412b8, (hs_global_external *)0x827412c0, (hs_global_external *)0x827412c8,
    (hs_global_external *)0x827412d0, (hs_global_external *)0x827412d8, (hs_global_external *)0x827412e0, (hs_global_external *)0x827412e8,
    (hs_global_external *)0x827412f0, (hs_global_external *)0x827412f8, (hs_global_external *)0x82741300, (hs_global_external *)0x82741308,
    (hs_global_external *)0x82741310, (hs_global_external *)0x82741318, (hs_global_external *)0x82741320, (hs_global_external *)0x82741328,
    (hs_global_external *)0x82741330, (hs_global_external *)0x82741338, (hs_global_external *)0x82741340, (hs_global_external *)0x82741348,
    (hs_global_external *)0x82741350, (hs_global_external *)0x82741358, (hs_global_external *)0x82741360, (hs_global_external *)0x82741368,
    (hs_global_external *)0x82741370, (hs_global_external *)0x82741378, (hs_global_external *)0x82741380, (hs_global_external *)0x82741388,
    (hs_global_external *)0x82741390, (hs_global_external *)0x82741398, (hs_global_external *)0x827413a0, (hs_global_external *)0x827413a8,
    (hs_global_external *)0x827413b0, (hs_global_external *)0x827413b8, (hs_global_external *)0x827413c0, (hs_global_external *)0x827413c8,
    (hs_global_external *)0x827413d0, (hs_global_external *)0x827413d8, (hs_global_external *)0x827413e0, (hs_global_external *)0x827413e8,
    (hs_global_external *)0x827413f0, (hs_global_external *)0x827413f8, (hs_global_external *)0x82741400, (hs_global_external *)0x82741408,
    (hs_global_external *)0x82741410, (hs_global_external *)0x82741418, (hs_global_external *)0x82741420, (hs_global_external *)0x82741428,
    (hs_global_external *)0x82741430, (hs_global_external *)0x82741438, (hs_global_external *)0x82741440, (hs_global_external *)0x82741448,
    (hs_global_external *)0x82741450, (hs_global_external *)0x82741458, (hs_global_external *)0x82741460, (hs_global_external *)0x82741468,
    (hs_global_external *)0x82741470, (hs_global_external *)0x82741478, (hs_global_external *)0x82741480, (hs_global_external *)0x82741488,
    (hs_global_external *)0x82741490, (hs_global_external *)0x82741498, (hs_global_external *)0x827414a0, (hs_global_external *)0x827414a8,
    (hs_global_external *)0x827414b0, (hs_global_external *)0x827414b8, (hs_global_external *)0x827414c0, (hs_global_external *)0x827414c8,
    (hs_global_external *)0x827414d0, (hs_global_external *)0x827414d8, (hs_global_external *)0x827414e0, (hs_global_external *)0x827414e8,
    (hs_global_external *)0x827414f0, (hs_global_external *)0x827414f8, (hs_global_external *)0x82741500, (hs_global_external *)0x82741508,
    (hs_global_external *)0x82741510, (hs_global_external *)0x82741518, (hs_global_external *)0x82741520, (hs_global_external *)0x82741528,
    (hs_global_external *)0x82741530, (hs_global_external *)0x82741538, (hs_global_external *)0x82741540, (hs_global_external *)0x82741548,
    (hs_global_external *)0x82741550, (hs_global_external *)0x82741558, (hs_global_external *)0x82741560, (hs_global_external *)0x82741568,
    (hs_global_external *)0x82741570, (hs_global_external *)0x82741578, (hs_global_external *)0x82741580, (hs_global_external *)0x82741588,
    (hs_global_external *)0x82741590, (hs_global_external *)0x82741598, (hs_global_external *)0x827415a0, (hs_global_external *)0x827415a8,
    (hs_global_external *)0x827415b0, (hs_global_external *)0x827415b8, (hs_global_external *)0x827415c0, (hs_global_external *)0x827415c8,
    (hs_global_external *)0x827415d0, (hs_global_external *)0x827415d8, (hs_global_external *)0x827415e0, (hs_global_external *)0x827415e8,
    (hs_global_external *)0x827415f0, (hs_global_external *)0x827415f8, (hs_global_external *)0x82741600, (hs_global_external *)0x82741608,
    (hs_global_external *)0x82741610, (hs_global_external *)0x82741618, (hs_global_external *)0x82741620, (hs_global_external *)0x82741628,
    (hs_global_external *)0x82741630, (hs_global_external *)0x82741638, (hs_global_external *)0x82741640, (hs_global_external *)0x82741648,
    (hs_global_external *)0x82741650, (hs_global_external *)0x82741658, (hs_global_external *)0x82741660, (hs_global_external *)0x82741668,
    (hs_global_external *)0x82741670, (hs_global_external *)0x82741678, (hs_global_external *)0x82741680, (hs_global_external *)0x82741688,
    (hs_global_external *)0x82741690, (hs_global_external *)0x82741698, (hs_global_external *)0x827416a0, (hs_global_external *)0x827416a8,
    (hs_global_external *)0x827416b0, (hs_global_external *)0x827416b8, (hs_global_external *)0x827416c0, (hs_global_external *)0x827416c8,
    (hs_global_external *)0x827416d0, (hs_global_external *)0x827416d8, (hs_global_external *)0x827416e0, (hs_global_external *)0x827416e8,
    (hs_global_external *)0x827416f0, (hs_global_external *)0x827416f8, (hs_global_external *)0x82741700, (hs_global_external *)0x82741708,
    (hs_global_external *)0x82741710, (hs_global_external *)0x82741718, (hs_global_external *)0x82741720, (hs_global_external *)0x82741728,
    (hs_global_external *)0x82741730, (hs_global_external *)0x82741738, (hs_global_external *)0x82741740, (hs_global_external *)0x82741748,
    (hs_global_external *)0x82741750, (hs_global_external *)0x82741758, (hs_global_external *)0x82741760, (hs_global_external *)0x82741768,
    (hs_global_external *)0x82741770, (hs_global_external *)0x82741778, (hs_global_external *)0x82741780, (hs_global_external *)0x82741788,
    (hs_global_external *)0x82741790, (hs_global_external *)0x82741798, (hs_global_external *)0x827417a0, (hs_global_external *)0x827417a8,
    (hs_global_external *)0x827417b0, (hs_global_external *)0x827417b8, (hs_global_external *)0x827417c0, (hs_global_external *)0x827417c8,
    (hs_global_external *)0x827417d0, (hs_global_external *)0x827417d8, (hs_global_external *)0x827417e0, (hs_global_external *)0x827417e8,
    (hs_global_external *)0x827417f0, (hs_global_external *)0x827417f8, (hs_global_external *)0x82741800, (hs_global_external *)0x82741808,
    (hs_global_external *)0x82741810, (hs_global_external *)0x82741818, (hs_global_external *)0x82741820, (hs_global_external *)0x82741828,
    (hs_global_external *)0x82741830, (hs_global_external *)0x82741838, (hs_global_external *)0x82741840, (hs_global_external *)0x82741848,
    (hs_global_external *)0x82741850, (hs_global_external *)0x82741858, (hs_global_external *)0x82741860, (hs_global_external *)0x82741868,
    (hs_global_external *)0x82741870, (hs_global_external *)0x82741878, (hs_global_external *)0x82741880, (hs_global_external *)0x82741888,
    (hs_global_external *)0x82741890, (hs_global_external *)0x82741898, (hs_global_external *)0x827418a0, (hs_global_external *)0x827418a8,
    (hs_global_external *)0x827418b0, (hs_global_external *)0x827418b8, (hs_global_external *)0x827418c0, (hs_global_external *)0x827418c8,
    (hs_global_external *)0x827418d0, (hs_global_external *)0x827418d8, (hs_global_external *)0x827418e0, (hs_global_external *)0x827418e8,
    (hs_global_external *)0x827418f0, (hs_global_external *)0x827418f8, (hs_global_external *)0x82741900, (hs_global_external *)0x82741908,
    (hs_global_external *)0x82741910, (hs_global_external *)0x82741918, (hs_global_external *)0x82741920, (hs_global_external *)0x82741928,
    (hs_global_external *)0x82741930, (hs_global_external *)0x82741938, (hs_global_external *)0x82741940, (hs_global_external *)0x82741948,
    (hs_global_external *)0x82741950, (hs_global_external *)0x82741958, (hs_global_external *)0x82741960, (hs_global_external *)0x82741968,
    (hs_global_external *)0x82741970, (hs_global_external *)0x82741978, (hs_global_external *)0x82741980, (hs_global_external *)0x82741988,
    (hs_global_external *)0x82741990, (hs_global_external *)0x82741998, (hs_global_external *)0x827419a0, (hs_global_external *)0x827419a8,
    (hs_global_external *)0x827419b0, (hs_global_external *)0x827419b8, (hs_global_external *)0x827419c0, (hs_global_external *)0x827419c8,
    (hs_global_external *)0x827419d0, (hs_global_external *)0x827419d8, (hs_global_external *)0x827419e0, (hs_global_external *)0x827419e8,
    (hs_global_external *)0x827419f0, (hs_global_external *)0x827419f8, (hs_global_external *)0x82741a00, (hs_global_external *)0x82741a08,
    (hs_global_external *)0x82741a10, (hs_global_external *)0x82741a18, (hs_global_external *)0x82741a20, (hs_global_external *)0x82741a28,
    (hs_global_external *)0x82741a30, (hs_global_external *)0x82741a38, (hs_global_external *)0x82741a40, (hs_global_external *)0x82741a48,
    (hs_global_external *)0x82741a50, (hs_global_external *)0x82741a58, (hs_global_external *)0x82741a60, (hs_global_external *)0x82741a68,
    (hs_global_external *)0x82741a70, (hs_global_external *)0x82741a78, (hs_global_external *)0x82741a80, (hs_global_external *)0x82741a88,
    (hs_global_external *)0x82741a90, (hs_global_external *)0x82741a98, (hs_global_external *)0x82741aa0, (hs_global_external *)0x82741aa8,
    (hs_global_external *)0x82741ab0, (hs_global_external *)0x82741ab8, (hs_global_external *)0x82741ac0, (hs_global_external *)0x82741ac8,
    (hs_global_external *)0x82741ad0, (hs_global_external *)0x82741ad8, (hs_global_external *)0x82741ae0, (hs_global_external *)0x82741ae8,
    (hs_global_external *)0x82741af0, (hs_global_external *)0x82741af8, (hs_global_external *)0x82741b00, (hs_global_external *)0x82741b08,
    (hs_global_external *)0x82741b10, (hs_global_external *)0x82741b18, (hs_global_external *)0x82741b20, (hs_global_external *)0x82741b28,
    (hs_global_external *)0x82741b30, (hs_global_external *)0x82741b38, (hs_global_external *)0x82741b40, (hs_global_external *)0x82741b48,
    (hs_global_external *)0x82741b50, (hs_global_external *)0x82741b58, (hs_global_external *)0x82741b60, (hs_global_external *)0x82741b68,
    (hs_global_external *)0x82741b70, (hs_global_external *)0x82741b78, (hs_global_external *)0x82741b80, (hs_global_external *)0x82741b88,
    (hs_global_external *)0x82741b90, (hs_global_external *)0x82741b98, (hs_global_external *)0x82741ba0, (hs_global_external *)0x82741ba8,
    (hs_global_external *)0x82741bb0, (hs_global_external *)0x82741bb8, (hs_global_external *)0x82741bc0, (hs_global_external *)0x82741bc8,
    (hs_global_external *)0x82741bd0, (hs_global_external *)0x82741bd8, (hs_global_external *)0x82741be0, (hs_global_external *)0x82741be8,
    (hs_global_external *)0x82741bf0, (hs_global_external *)0x82741bf8, (hs_global_external *)0x82741c00, (hs_global_external *)0x82741c08,
    (hs_global_external *)0x82741c10, (hs_global_external *)0x82741c18, (hs_global_external *)0x82741c20, (hs_global_external *)0x82741c28,
    (hs_global_external *)0x82741c30, (hs_global_external *)0x82741c38, (hs_global_external *)0x82741c40, (hs_global_external *)0x82741c48,
    (hs_global_external *)0x82741c50, (hs_global_external *)0x82741c58, (hs_global_external *)0x82741c60, (hs_global_external *)0x82741c68,
    (hs_global_external *)0x82741c70, (hs_global_external *)0x82741c78, (hs_global_external *)0x82741c80, (hs_global_external *)0x82741c88,
    (hs_global_external *)0x82741c90, (hs_global_external *)0x82741c98, (hs_global_external *)0x82741ca0, (hs_global_external *)0x82741ca8,
    (hs_global_external *)0x82741cb0, (hs_global_external *)0x82741cb8, (hs_global_external *)0x82741cc0, (hs_global_external *)0x82741cc8,
    (hs_global_external *)0x82741cd0, (hs_global_external *)0x82741cd8, (hs_global_external *)0x82741ce0, (hs_global_external *)0x82741ce8,
    (hs_global_external *)0x82741cf0, (hs_global_external *)0x82741cf8, (hs_global_external *)0x82741d00, (hs_global_external *)0x82741d08,
    (hs_global_external *)0x82741d10, (hs_global_external *)0x82741d18, (hs_global_external *)0x82741d20, (hs_global_external *)0x82741d28,
    (hs_global_external *)0x82741d30, (hs_global_external *)0x82741d38, (hs_global_external *)0x82741d40, (hs_global_external *)0x82741d48,
    (hs_global_external *)0x82741d50, (hs_global_external *)0x82741d58, (hs_global_external *)0x82741d60, (hs_global_external *)0x82741d68,
    (hs_global_external *)0x82741d70, (hs_global_external *)0x82741d78, (hs_global_external *)0x82741d80, (hs_global_external *)0x82741d88,
    (hs_global_external *)0x82741d90, (hs_global_external *)0x82741d98, (hs_global_external *)0x82741da0, (hs_global_external *)0x82741da8,
    (hs_global_external *)0x82741db0, (hs_global_external *)0x82741db8, (hs_global_external *)0x82741dc0, (hs_global_external *)0x82741dc8,
    (hs_global_external *)0x82741dd0, (hs_global_external *)0x82741dd8, (hs_global_external *)0x82741de0, (hs_global_external *)0x82741de8,
    (hs_global_external *)0x82741df0, (hs_global_external *)0x82741df8, (hs_global_external *)0x82741e00, (hs_global_external *)0x82741e08,
    (hs_global_external *)0x82741e10, (hs_global_external *)0x82741e18, (hs_global_external *)0x82741e20, (hs_global_external *)0x82741e28,
    (hs_global_external *)0x82741e30, (hs_global_external *)0x82741e38, (hs_global_external *)0x82741e40, (hs_global_external *)0x82741e48,
    (hs_global_external *)0x82741e50, (hs_global_external *)0x82741e58, (hs_global_external *)0x82741e60, (hs_global_external *)0x82741e68,
    (hs_global_external *)0x82741e70, (hs_global_external *)0x82741e78, (hs_global_external *)0x82741e80, (hs_global_external *)0x82741e88,
    (hs_global_external *)0x82741e90, (hs_global_external *)0x82741e98, (hs_global_external *)0x82741ea0, (hs_global_external *)0x82741ea8,
    (hs_global_external *)0x82741eb0, (hs_global_external *)0x82741eb8, (hs_global_external *)0x82741ec0, (hs_global_external *)0x82741ec8,
    (hs_global_external *)0x82741ed0, (hs_global_external *)0x82741ed8, (hs_global_external *)0x82741ee0, (hs_global_external *)0x82741ee8,
    (hs_global_external *)0x82741ef0, (hs_global_external *)0x82741ef8, (hs_global_external *)0x82741f00, (hs_global_external *)0x82741f08,
    (hs_global_external *)0x82741f10, (hs_global_external *)0x82741f18, (hs_global_external *)0x82741f20, (hs_global_external *)0x82741f28,
    (hs_global_external *)0x82741f30, (hs_global_external *)0x82741f38, (hs_global_external *)0x82741f40, (hs_global_external *)0x82741f48,
    (hs_global_external *)0x82741f50, (hs_global_external *)0x82741f58, (hs_global_external *)0x82741f60, (hs_global_external *)0x82741f68,
    (hs_global_external *)0x82741f70, (hs_global_external *)0x82741f78, (hs_global_external *)0x82741f80, (hs_global_external *)0x82741f88,
    (hs_global_external *)0x82741f90, (hs_global_external *)0x82741f98, (hs_global_external *)0x82741fa0, (hs_global_external *)0x82741fa8,
    (hs_global_external *)0x82741fb0, (hs_global_external *)0x82741fb8, (hs_global_external *)0x82741fc0, (hs_global_external *)0x82741fc8,
    (hs_global_external *)0x82741fd0, (hs_global_external *)0x82741fd8, (hs_global_external *)0x82741fe0, (hs_global_external *)0x82741fe8,
    (hs_global_external *)0x82741ff0, (hs_global_external *)0x82741ff8, (hs_global_external *)0x82742000, (hs_global_external *)0x82742008,
    (hs_global_external *)0x82742010, (hs_global_external *)0x82742018, (hs_global_external *)0x82742020, (hs_global_external *)0x82742028,
    (hs_global_external *)0x82742030, (hs_global_external *)0x82742038, (hs_global_external *)0x82742040, (hs_global_external *)0x82742048,
    (hs_global_external *)0x82742050, (hs_global_external *)0x82742058, (hs_global_external *)0x82742060, (hs_global_external *)0x82742068,
    (hs_global_external *)0x82742070, (hs_global_external *)0x82742078, (hs_global_external *)0x82742080, (hs_global_external *)0x82742088,
    (hs_global_external *)0x82742090, (hs_global_external *)0x82742098, (hs_global_external *)0x827420a0, (hs_global_external *)0x827420a8,
    (hs_global_external *)0x827420b0, (hs_global_external *)0x827420b8, (hs_global_external *)0x827420c0, (hs_global_external *)0x827420c8,
    (hs_global_external *)0x827420d0, (hs_global_external *)0x827420d8, (hs_global_external *)0x827420e0, (hs_global_external *)0x827420e8,
    (hs_global_external *)0x827420f0, (hs_global_external *)0x827420f8, (hs_global_external *)0x82742100, (hs_global_external *)0x82742108,
    (hs_global_external *)0x82742110, (hs_global_external *)0x82742118, (hs_global_external *)0x82742120, (hs_global_external *)0x82742128,
    (hs_global_external *)0x82742130, (hs_global_external *)0x82742138, (hs_global_external *)0x82742140, (hs_global_external *)0x82742148,
    (hs_global_external *)0x82742150, (hs_global_external *)0x82742158,
    (hs_global_external *)0x82742160, (hs_global_external *)0x82742168, (hs_global_external *)0x82742170, (hs_global_external *)0x82742178,
    (hs_global_external *)0x82742180, (hs_global_external *)0x82742188, (hs_global_external *)0x82742190, (hs_global_external *)0x82742198,
    (hs_global_external *)0x827421a0, (hs_global_external *)0x827421a8, (hs_global_external *)0x827421b0, (hs_global_external *)0x827421b8,
    (hs_global_external *)0x827421c0, (hs_global_external *)0x827421c8, (hs_global_external *)0x827421d0, (hs_global_external *)0x827421d8,
    (hs_global_external *)0x827421e0, (hs_global_external *)0x827421e8, (hs_global_external *)0x827421f0, (hs_global_external *)0x827421f8,
    (hs_global_external *)0x82742200, (hs_global_external *)0x82742208, (hs_global_external *)0x82742210, (hs_global_external *)0x82742218,
    (hs_global_external *)0x82742220, (hs_global_external *)0x82742228, (hs_global_external *)0x82742230, (hs_global_external *)0x82742238,
    (hs_global_external *)0x82742240, (hs_global_external *)0x82742248, (hs_global_external *)0x82742250, (hs_global_external *)0x82742258,
    (hs_global_external *)0x82742260, (hs_global_external *)0x82742268, (hs_global_external *)0x82742270, (hs_global_external *)0x82742278,
    (hs_global_external *)0x82742280, (hs_global_external *)0x82742288, (hs_global_external *)0x82742290, (hs_global_external *)0x82742298,
    (hs_global_external *)0x827422a0, (hs_global_external *)0x827422a8, (hs_global_external *)0x827422b0, (hs_global_external *)0x827422b8,
    (hs_global_external *)0x827422c0, (hs_global_external *)0x827422c8, (hs_global_external *)0x827422d0, (hs_global_external *)0x827422d8,
    (hs_global_external *)0x827422e0, (hs_global_external *)0x827422e8, (hs_global_external *)0x827422f0, (hs_global_external *)0x827422f8,
    (hs_global_external *)0x82742300, (hs_global_external *)0x82742308, (hs_global_external *)0x82742310, (hs_global_external *)0x82742318,
    (hs_global_external *)0x82742320, (hs_global_external *)0x82742328, (hs_global_external *)0x82742330, (hs_global_external *)0x82742338,
    (hs_global_external *)0x82742340, (hs_global_external *)0x82742348, (hs_global_external *)0x82742350, (hs_global_external *)0x82742358,
    (hs_global_external *)0x82742360, (hs_global_external *)0x82742368, (hs_global_external *)0x82742370, (hs_global_external *)0x82742378,
    (hs_global_external *)0x82742380, (hs_global_external *)0x82742388, (hs_global_external *)0x82742390, (hs_global_external *)0x82742398,
    (hs_global_external *)0x827423a0, (hs_global_external *)0x827423a8, (hs_global_external *)0x827423b0, (hs_global_external *)0x827423b8,
    (hs_global_external *)0x827423c0, (hs_global_external *)0x827423c8, (hs_global_external *)0x827423d0, (hs_global_external *)0x827423d8,
    (hs_global_external *)0x827423e0, (hs_global_external *)0x827423e8, (hs_global_external *)0x827423f0, (hs_global_external *)0x827423f8,
    (hs_global_external *)0x82742400, (hs_global_external *)0x82742408, (hs_global_external *)0x82742410, (hs_global_external *)0x82742418,
    (hs_global_external *)0x82742420, (hs_global_external *)0x82742428, (hs_global_external *)0x82742430, (hs_global_external *)0x82742438,
    (hs_global_external *)0x82742440, (hs_global_external *)0x82742448, (hs_global_external *)0x82742450, (hs_global_external *)0x82742458,
    (hs_global_external *)0x82742460, (hs_global_external *)0x82742468, (hs_global_external *)0x82742470, (hs_global_external *)0x82742478,
    (hs_global_external *)0x82742480, (hs_global_external *)0x82742488, (hs_global_external *)0x82742490, (hs_global_external *)0x82742498,
    (hs_global_external *)0x827424a0, (hs_global_external *)0x827424a8, (hs_global_external *)0x827424b0, (hs_global_external *)0x827424b8,
    (hs_global_external *)0x827424c0, (hs_global_external *)0x827424c8, (hs_global_external *)0x827424d0, (hs_global_external *)0x827424d8,
    (hs_global_external *)0x827424e0, (hs_global_external *)0x827424e8, (hs_global_external *)0x827424f0, (hs_global_external *)0x827424f8,
    (hs_global_external *)0x82742500, (hs_global_external *)0x82742508, (hs_global_external *)0x82742510, (hs_global_external *)0x82742518,
    (hs_global_external *)0x82742520, (hs_global_external *)0x82742528, (hs_global_external *)0x82742530, (hs_global_external *)0x82742538,
    (hs_global_external *)0x82742540, (hs_global_external *)0x82742548, (hs_global_external *)0x82742550, (hs_global_external *)0x82742558,
    (hs_global_external *)0x82742560, (hs_global_external *)0x82742568, (hs_global_external *)0x82742570, (hs_global_external *)0x82742578,
    (hs_global_external *)0x82742580, (hs_global_external *)0x82742588, (hs_global_external *)0x82742590, (hs_global_external *)0x82742598,
    (hs_global_external *)0x827425a0, (hs_global_external *)0x827425a8, (hs_global_external *)0x827425b0, (hs_global_external *)0x827425b8,
#if defined(_XBOX) && !defined(__clang__)
    (hs_global_external *)0x827c5f2c,
#else
    &g_ai_debug_throttled_global,
#endif
    (hs_global_external *)0x827425c0, (hs_global_external *)0x827425c8, (hs_global_external *)0x827425d0,
    (hs_global_external *)0x827425d8, (hs_global_external *)0x827425e0, (hs_global_external *)0x827425e8, (hs_global_external *)0x827425f0,
    (hs_global_external *)0x827425f8, (hs_global_external *)0x82742600, (hs_global_external *)0x82742608, (hs_global_external *)0x82742610,
    (hs_global_external *)0x82742618, (hs_global_external *)0x82742620, (hs_global_external *)0x82742628, (hs_global_external *)0x82742630,
    (hs_global_external *)0x82742638, (hs_global_external *)0x82742640, (hs_global_external *)0x82742648, (hs_global_external *)0x82742650,
    (hs_global_external *)0x82742658, (hs_global_external *)0x82742660, (hs_global_external *)0x82742668, (hs_global_external *)0x82742670,
};

// @address 0x7ea8e0 ?g_hs_syntax_data@@3PAUs_data_array@@A
s_data_array *g_hs_syntax_data;

// @address 0x241870 ?hs_initialize@@YAXXZ
void hs_initialize(void)
{
    char name[256];

    g_hs_syntax_data = (s_data_array *)data_initialize_disconnected("script node", 0xf000, 0x18, 0, &g_runtime_state_allocation);
    tls_object_list_header_data() = data_new("object list header", 0x30, 0xc, 0, (c_allocation_base *)&g_hs_object_list_header_allocator);
    csnzprintf(name, sizeof(name), "%s reference", "list object");
    tls_list_object_reference_data() = data_new(name, 0x80, 0xc, 0, (c_allocation_base *)&g_hs_object_list_reference_allocator);
    hs_runtime_initialize();
    hs_initialize_for_new_map(false, true);
}


static __forceinline void hs_object_lists_initialize_for_new_map()
{
    s_data_array *headers = tls_object_list_header_data();
    headers->valid = true;
    data_delete_all(headers);
    s_data_array *objects = tls_list_object_reference_data();
    objects->valid = true;
    data_delete_all(objects);
}

static __forceinline bool hs_scenario_has_only_source_files(const scenario *definition)
{
    return definition->scripts.count == 0 && definition->source_files.count > 0;
}

// @address 0x241940 ?hs_initialize_for_new_map@@YAXXZ
void hs_initialize_for_new_map()
{
    hs_initialize_for_new_map(false, true);
}

// @address 0x241950 ?hs_initialize_for_new_map@@YAX_N0@Z
void hs_initialize_for_new_map(bool, bool)
{
    scenario *definition = global_scenario;
    if (definition != 0) {
        if ((g_hs_syntax_data->flags & 2) != 0)
            ::data_connect(g_hs_syntax_data, definition->syntax_nodes.count, (void *)(definition->syntax_nodes.address));
        if (!hs_scenario_has_only_source_files(definition)) {
            hs_compile_postprocess_fast();
            ai_handle_script_verification(true);
        }
    }
    hs_object_lists_initialize_for_new_map();
    hs_runtime_initialize_for_new_map();
}

// @address 0x241a18 ?hs_dispose_from_old_map@@YAXXZ
void hs_dispose_from_old_map()
{
    hs_node_gc();
    if ((g_hs_syntax_data->flags & 2) == 0)
        data_disconnect(g_hs_syntax_data);
    hs_runtime_dispose_from_old_map();
    object_lists_dispose_from_old_map();
}

// @address 0x241a90 ?hs_dispose@@YAXXZ
void hs_dispose()
{
    data_dispose(g_hs_syntax_data);
    hs_runtime_dispose();
    object_lists_dispose();
}

// @address 0x241ad0 ?hs_node_gc@@YAXXZ
void hs_node_gc(void)
{
    if (hs_runtime_initialized()) {
        s_hs_thread_iterator iterator;
        ((void (*)(s_hs_thread_iterator *, bool, bool))hs_thread_iterator_new)(&iterator, true, true);
        long thread_index;
        while ((thread_index = hs_thread_iterator_next(&iterator)) != -1) {
            hs_thread *thread = datum_get<hs_thread>(tls_hs_thread_deterministic_data(), thread_index);
            if (thread->type == _hs_thread_type_console)
                hs_thread_delete(thread_index, true);
        }
    }

    s_data_array *array = g_hs_syntax_data;
    if ((array->flags & 2) == 0) {
        for (long index = ::data_next_index(array, -1); index != -1; index = inl_data_next_index(array, index)) {
            hs_syntax_node *datum = (hs_syntax_node *)(array->data + (index & 0xffff) * sizeof(hs_syntax_node));
            if ((datum->flags & 8) == 0)
                ::datum_delete(array, index);
        }
    }
}
