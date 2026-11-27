// blamlibxbox/hs.obj: from the records 0xab9c0 0x241870 0x241940 0x241950 0x241a18 0x241a90 0x241ad0
// @flags /TP /O2
#include "memory/data_types.h"
#include "memory/runtime_state_allocation.h"
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
// tmp/probe (GAME-CHANGES F6): the retail records, filled from the retail image (tmp/probe/fill/hs_globals.spec).
extern "C" hs_global_external retail_73ff10[];
struct s_ai_debug_throttled_global;
extern s_ai_debug_throttled_global g_ai_debug_throttled_global;
hs_global_external *hs_external_globals[] = {
    &retail_73ff10[(0x73ff10 - 0x73ff10) / 8], &retail_73ff10[(0x73ff18 - 0x73ff10) / 8], &retail_73ff10[(0x73ff20 - 0x73ff10) / 8], &retail_73ff10[(0x73ff28 - 0x73ff10) / 8],
    &retail_73ff10[(0x73ff30 - 0x73ff10) / 8], &retail_73ff10[(0x73ff38 - 0x73ff10) / 8], &retail_73ff10[(0x73ff40 - 0x73ff10) / 8], &retail_73ff10[(0x73ff48 - 0x73ff10) / 8],
    &retail_73ff10[(0x73ff50 - 0x73ff10) / 8], &retail_73ff10[(0x73ff58 - 0x73ff10) / 8], &retail_73ff10[(0x73ff60 - 0x73ff10) / 8], &retail_73ff10[(0x73ff68 - 0x73ff10) / 8],
    &retail_73ff10[(0x73ff70 - 0x73ff10) / 8], &retail_73ff10[(0x73ff78 - 0x73ff10) / 8], &retail_73ff10[(0x73ff80 - 0x73ff10) / 8], &retail_73ff10[(0x73ff88 - 0x73ff10) / 8],
    &retail_73ff10[(0x73ff90 - 0x73ff10) / 8], &retail_73ff10[(0x73ff98 - 0x73ff10) / 8], &retail_73ff10[(0x73ffa0 - 0x73ff10) / 8], &retail_73ff10[(0x73ffa8 - 0x73ff10) / 8],
    &retail_73ff10[(0x73ffb0 - 0x73ff10) / 8], &retail_73ff10[(0x73ffb8 - 0x73ff10) / 8], &retail_73ff10[(0x73ffc0 - 0x73ff10) / 8], &retail_73ff10[(0x73ffc8 - 0x73ff10) / 8],
    &retail_73ff10[(0x73ffd0 - 0x73ff10) / 8], &retail_73ff10[(0x73ffd8 - 0x73ff10) / 8], &retail_73ff10[(0x73ffe0 - 0x73ff10) / 8], &retail_73ff10[(0x73ffe8 - 0x73ff10) / 8],
    &retail_73ff10[(0x73fff0 - 0x73ff10) / 8], &retail_73ff10[(0x73fff8 - 0x73ff10) / 8], &retail_73ff10[(0x740000 - 0x73ff10) / 8], &retail_73ff10[(0x740008 - 0x73ff10) / 8],
    &retail_73ff10[(0x740010 - 0x73ff10) / 8], &retail_73ff10[(0x740018 - 0x73ff10) / 8], &retail_73ff10[(0x740020 - 0x73ff10) / 8], &retail_73ff10[(0x740028 - 0x73ff10) / 8],
    &retail_73ff10[(0x740030 - 0x73ff10) / 8], &retail_73ff10[(0x740038 - 0x73ff10) / 8], &retail_73ff10[(0x740040 - 0x73ff10) / 8], &retail_73ff10[(0x740048 - 0x73ff10) / 8],
    &retail_73ff10[(0x740050 - 0x73ff10) / 8], &retail_73ff10[(0x740058 - 0x73ff10) / 8], &retail_73ff10[(0x740060 - 0x73ff10) / 8], &retail_73ff10[(0x740068 - 0x73ff10) / 8],
    &retail_73ff10[(0x740070 - 0x73ff10) / 8], &retail_73ff10[(0x740078 - 0x73ff10) / 8], &retail_73ff10[(0x740080 - 0x73ff10) / 8], &retail_73ff10[(0x740088 - 0x73ff10) / 8],
    &retail_73ff10[(0x740090 - 0x73ff10) / 8], &retail_73ff10[(0x740098 - 0x73ff10) / 8], &retail_73ff10[(0x7400a0 - 0x73ff10) / 8], &retail_73ff10[(0x7400a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7400b0 - 0x73ff10) / 8], &retail_73ff10[(0x7400b8 - 0x73ff10) / 8], &retail_73ff10[(0x7400c0 - 0x73ff10) / 8], &retail_73ff10[(0x7400c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7400d0 - 0x73ff10) / 8], &retail_73ff10[(0x7400d8 - 0x73ff10) / 8], &retail_73ff10[(0x7400e0 - 0x73ff10) / 8], &retail_73ff10[(0x7400e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7400f0 - 0x73ff10) / 8], &retail_73ff10[(0x7400f8 - 0x73ff10) / 8], &retail_73ff10[(0x740100 - 0x73ff10) / 8], &retail_73ff10[(0x740108 - 0x73ff10) / 8],
    &retail_73ff10[(0x740110 - 0x73ff10) / 8], &retail_73ff10[(0x740118 - 0x73ff10) / 8], &retail_73ff10[(0x740120 - 0x73ff10) / 8], &retail_73ff10[(0x740128 - 0x73ff10) / 8],
    &retail_73ff10[(0x740130 - 0x73ff10) / 8], &retail_73ff10[(0x740138 - 0x73ff10) / 8], &retail_73ff10[(0x740140 - 0x73ff10) / 8], &retail_73ff10[(0x740148 - 0x73ff10) / 8],
    &retail_73ff10[(0x740150 - 0x73ff10) / 8], &retail_73ff10[(0x740158 - 0x73ff10) / 8], &retail_73ff10[(0x740160 - 0x73ff10) / 8], &retail_73ff10[(0x740168 - 0x73ff10) / 8],
    &retail_73ff10[(0x740170 - 0x73ff10) / 8], &retail_73ff10[(0x740178 - 0x73ff10) / 8], &retail_73ff10[(0x740180 - 0x73ff10) / 8], &retail_73ff10[(0x740188 - 0x73ff10) / 8],
    &retail_73ff10[(0x740190 - 0x73ff10) / 8], &retail_73ff10[(0x740198 - 0x73ff10) / 8], &retail_73ff10[(0x7401a0 - 0x73ff10) / 8], &retail_73ff10[(0x7401a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7401b0 - 0x73ff10) / 8], &retail_73ff10[(0x7401b8 - 0x73ff10) / 8], &retail_73ff10[(0x7401c0 - 0x73ff10) / 8], &retail_73ff10[(0x7401c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7401d0 - 0x73ff10) / 8], &retail_73ff10[(0x7401d8 - 0x73ff10) / 8], &retail_73ff10[(0x7401e0 - 0x73ff10) / 8], &retail_73ff10[(0x7401e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7401f0 - 0x73ff10) / 8], &retail_73ff10[(0x7401f8 - 0x73ff10) / 8], &retail_73ff10[(0x740200 - 0x73ff10) / 8], &retail_73ff10[(0x740208 - 0x73ff10) / 8],
    &retail_73ff10[(0x740210 - 0x73ff10) / 8], &retail_73ff10[(0x740218 - 0x73ff10) / 8], &retail_73ff10[(0x740220 - 0x73ff10) / 8], &retail_73ff10[(0x740228 - 0x73ff10) / 8],
    &retail_73ff10[(0x740230 - 0x73ff10) / 8], &retail_73ff10[(0x740238 - 0x73ff10) / 8], &retail_73ff10[(0x740240 - 0x73ff10) / 8], &retail_73ff10[(0x740248 - 0x73ff10) / 8],
    &retail_73ff10[(0x740250 - 0x73ff10) / 8], &retail_73ff10[(0x740258 - 0x73ff10) / 8], &retail_73ff10[(0x740260 - 0x73ff10) / 8], &retail_73ff10[(0x740268 - 0x73ff10) / 8],
    &retail_73ff10[(0x740270 - 0x73ff10) / 8], &retail_73ff10[(0x740278 - 0x73ff10) / 8], &retail_73ff10[(0x740280 - 0x73ff10) / 8], &retail_73ff10[(0x740288 - 0x73ff10) / 8],
    &retail_73ff10[(0x740290 - 0x73ff10) / 8], &retail_73ff10[(0x740298 - 0x73ff10) / 8], &retail_73ff10[(0x7402a0 - 0x73ff10) / 8], &retail_73ff10[(0x7402a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7402b0 - 0x73ff10) / 8], &retail_73ff10[(0x7402b8 - 0x73ff10) / 8], &retail_73ff10[(0x7402c0 - 0x73ff10) / 8], &retail_73ff10[(0x7402c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7402d0 - 0x73ff10) / 8], &retail_73ff10[(0x7402d8 - 0x73ff10) / 8], &retail_73ff10[(0x7402e0 - 0x73ff10) / 8], &retail_73ff10[(0x7402e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7402f0 - 0x73ff10) / 8], &retail_73ff10[(0x7402f8 - 0x73ff10) / 8], &retail_73ff10[(0x740300 - 0x73ff10) / 8], &retail_73ff10[(0x740308 - 0x73ff10) / 8],
    &retail_73ff10[(0x740310 - 0x73ff10) / 8], &retail_73ff10[(0x740318 - 0x73ff10) / 8], &retail_73ff10[(0x740320 - 0x73ff10) / 8], &retail_73ff10[(0x740328 - 0x73ff10) / 8],
    &retail_73ff10[(0x740330 - 0x73ff10) / 8], &retail_73ff10[(0x740338 - 0x73ff10) / 8], &retail_73ff10[(0x740340 - 0x73ff10) / 8], &retail_73ff10[(0x740348 - 0x73ff10) / 8],
    &retail_73ff10[(0x740350 - 0x73ff10) / 8], &retail_73ff10[(0x740358 - 0x73ff10) / 8], &retail_73ff10[(0x740360 - 0x73ff10) / 8], &retail_73ff10[(0x740368 - 0x73ff10) / 8],
    &retail_73ff10[(0x740370 - 0x73ff10) / 8], &retail_73ff10[(0x740378 - 0x73ff10) / 8], &retail_73ff10[(0x740380 - 0x73ff10) / 8], &retail_73ff10[(0x740388 - 0x73ff10) / 8],
    &retail_73ff10[(0x740390 - 0x73ff10) / 8], &retail_73ff10[(0x740398 - 0x73ff10) / 8], &retail_73ff10[(0x7403a0 - 0x73ff10) / 8], &retail_73ff10[(0x7403a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7403b0 - 0x73ff10) / 8], &retail_73ff10[(0x7403b8 - 0x73ff10) / 8], &retail_73ff10[(0x7403c0 - 0x73ff10) / 8], &retail_73ff10[(0x7403c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7403d0 - 0x73ff10) / 8], &retail_73ff10[(0x7403d8 - 0x73ff10) / 8], &retail_73ff10[(0x7403e0 - 0x73ff10) / 8], &retail_73ff10[(0x7403e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7403f0 - 0x73ff10) / 8], &retail_73ff10[(0x7403f8 - 0x73ff10) / 8], &retail_73ff10[(0x740400 - 0x73ff10) / 8], &retail_73ff10[(0x740408 - 0x73ff10) / 8],
    &retail_73ff10[(0x740410 - 0x73ff10) / 8], &retail_73ff10[(0x740418 - 0x73ff10) / 8], &retail_73ff10[(0x740420 - 0x73ff10) / 8], &retail_73ff10[(0x740428 - 0x73ff10) / 8],
    &retail_73ff10[(0x740430 - 0x73ff10) / 8], &retail_73ff10[(0x740438 - 0x73ff10) / 8], &retail_73ff10[(0x740440 - 0x73ff10) / 8], &retail_73ff10[(0x740448 - 0x73ff10) / 8],
    &retail_73ff10[(0x740450 - 0x73ff10) / 8], &retail_73ff10[(0x740458 - 0x73ff10) / 8], &retail_73ff10[(0x740460 - 0x73ff10) / 8], &retail_73ff10[(0x740468 - 0x73ff10) / 8],
    &retail_73ff10[(0x740470 - 0x73ff10) / 8], &retail_73ff10[(0x740478 - 0x73ff10) / 8], &retail_73ff10[(0x740480 - 0x73ff10) / 8], &retail_73ff10[(0x740488 - 0x73ff10) / 8],
    &retail_73ff10[(0x740490 - 0x73ff10) / 8], &retail_73ff10[(0x740498 - 0x73ff10) / 8], &retail_73ff10[(0x7404a0 - 0x73ff10) / 8], &retail_73ff10[(0x7404a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7404b0 - 0x73ff10) / 8], &retail_73ff10[(0x7404b8 - 0x73ff10) / 8], &retail_73ff10[(0x7404c0 - 0x73ff10) / 8], &retail_73ff10[(0x7404c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7404d0 - 0x73ff10) / 8], &retail_73ff10[(0x7404d8 - 0x73ff10) / 8], &retail_73ff10[(0x7404e0 - 0x73ff10) / 8], &retail_73ff10[(0x7404e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7404f0 - 0x73ff10) / 8], &retail_73ff10[(0x7404f8 - 0x73ff10) / 8], &retail_73ff10[(0x740500 - 0x73ff10) / 8], &retail_73ff10[(0x740508 - 0x73ff10) / 8],
    &retail_73ff10[(0x740510 - 0x73ff10) / 8], &retail_73ff10[(0x740518 - 0x73ff10) / 8], &retail_73ff10[(0x740520 - 0x73ff10) / 8], &retail_73ff10[(0x740528 - 0x73ff10) / 8],
    &retail_73ff10[(0x740530 - 0x73ff10) / 8], &retail_73ff10[(0x740538 - 0x73ff10) / 8], &retail_73ff10[(0x740540 - 0x73ff10) / 8], &retail_73ff10[(0x740548 - 0x73ff10) / 8],
    &retail_73ff10[(0x740550 - 0x73ff10) / 8], &retail_73ff10[(0x740558 - 0x73ff10) / 8], &retail_73ff10[(0x740560 - 0x73ff10) / 8], &retail_73ff10[(0x740568 - 0x73ff10) / 8],
    &retail_73ff10[(0x740570 - 0x73ff10) / 8], &retail_73ff10[(0x740578 - 0x73ff10) / 8], &retail_73ff10[(0x740580 - 0x73ff10) / 8], &retail_73ff10[(0x740588 - 0x73ff10) / 8],
    &retail_73ff10[(0x740590 - 0x73ff10) / 8], &retail_73ff10[(0x740598 - 0x73ff10) / 8], &retail_73ff10[(0x7405a0 - 0x73ff10) / 8], &retail_73ff10[(0x7405a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7405b0 - 0x73ff10) / 8], &retail_73ff10[(0x7405b8 - 0x73ff10) / 8], &retail_73ff10[(0x7405c0 - 0x73ff10) / 8], &retail_73ff10[(0x7405c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7405d0 - 0x73ff10) / 8], &retail_73ff10[(0x7405d8 - 0x73ff10) / 8], &retail_73ff10[(0x7405e0 - 0x73ff10) / 8], &retail_73ff10[(0x7405e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7405f0 - 0x73ff10) / 8], &retail_73ff10[(0x7405f8 - 0x73ff10) / 8], &retail_73ff10[(0x740600 - 0x73ff10) / 8], &retail_73ff10[(0x740608 - 0x73ff10) / 8],
    &retail_73ff10[(0x740610 - 0x73ff10) / 8], &retail_73ff10[(0x740618 - 0x73ff10) / 8], &retail_73ff10[(0x740620 - 0x73ff10) / 8], &retail_73ff10[(0x740628 - 0x73ff10) / 8],
    &retail_73ff10[(0x740630 - 0x73ff10) / 8], &retail_73ff10[(0x740638 - 0x73ff10) / 8], &retail_73ff10[(0x740640 - 0x73ff10) / 8], &retail_73ff10[(0x740648 - 0x73ff10) / 8],
    &retail_73ff10[(0x740650 - 0x73ff10) / 8], &retail_73ff10[(0x740658 - 0x73ff10) / 8], &retail_73ff10[(0x740660 - 0x73ff10) / 8], &retail_73ff10[(0x740668 - 0x73ff10) / 8],
    &retail_73ff10[(0x740670 - 0x73ff10) / 8], &retail_73ff10[(0x740678 - 0x73ff10) / 8], &retail_73ff10[(0x740680 - 0x73ff10) / 8], &retail_73ff10[(0x740688 - 0x73ff10) / 8],
    &retail_73ff10[(0x740690 - 0x73ff10) / 8], &retail_73ff10[(0x740698 - 0x73ff10) / 8], &retail_73ff10[(0x7406a0 - 0x73ff10) / 8], &retail_73ff10[(0x7406a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7406b0 - 0x73ff10) / 8], &retail_73ff10[(0x7406b8 - 0x73ff10) / 8], &retail_73ff10[(0x7406c0 - 0x73ff10) / 8], &retail_73ff10[(0x7406c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7406d0 - 0x73ff10) / 8], &retail_73ff10[(0x7406d8 - 0x73ff10) / 8], &retail_73ff10[(0x7406e0 - 0x73ff10) / 8], &retail_73ff10[(0x7406e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7406f0 - 0x73ff10) / 8], &retail_73ff10[(0x7406f8 - 0x73ff10) / 8], &retail_73ff10[(0x740700 - 0x73ff10) / 8], &retail_73ff10[(0x740708 - 0x73ff10) / 8],
    &retail_73ff10[(0x740710 - 0x73ff10) / 8], &retail_73ff10[(0x740718 - 0x73ff10) / 8], &retail_73ff10[(0x740720 - 0x73ff10) / 8], &retail_73ff10[(0x740728 - 0x73ff10) / 8],
    &retail_73ff10[(0x740730 - 0x73ff10) / 8], &retail_73ff10[(0x740738 - 0x73ff10) / 8], &retail_73ff10[(0x740740 - 0x73ff10) / 8], &retail_73ff10[(0x740748 - 0x73ff10) / 8],
    &retail_73ff10[(0x740750 - 0x73ff10) / 8], &retail_73ff10[(0x740758 - 0x73ff10) / 8], &retail_73ff10[(0x740760 - 0x73ff10) / 8], &retail_73ff10[(0x740768 - 0x73ff10) / 8],
    &retail_73ff10[(0x740770 - 0x73ff10) / 8], &retail_73ff10[(0x740778 - 0x73ff10) / 8], &retail_73ff10[(0x740780 - 0x73ff10) / 8], &retail_73ff10[(0x740788 - 0x73ff10) / 8],
    &retail_73ff10[(0x740790 - 0x73ff10) / 8], &retail_73ff10[(0x740798 - 0x73ff10) / 8], &retail_73ff10[(0x7407a0 - 0x73ff10) / 8], &retail_73ff10[(0x7407a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7407b0 - 0x73ff10) / 8], &retail_73ff10[(0x7407b8 - 0x73ff10) / 8], &retail_73ff10[(0x7407c0 - 0x73ff10) / 8], &retail_73ff10[(0x7407c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7407d0 - 0x73ff10) / 8], &retail_73ff10[(0x7407d8 - 0x73ff10) / 8], &retail_73ff10[(0x7407e0 - 0x73ff10) / 8], &retail_73ff10[(0x7407e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7407f0 - 0x73ff10) / 8], &retail_73ff10[(0x7407f8 - 0x73ff10) / 8], &retail_73ff10[(0x740800 - 0x73ff10) / 8], &retail_73ff10[(0x740808 - 0x73ff10) / 8],
    &retail_73ff10[(0x740810 - 0x73ff10) / 8], &retail_73ff10[(0x740818 - 0x73ff10) / 8], &retail_73ff10[(0x740820 - 0x73ff10) / 8], &retail_73ff10[(0x740828 - 0x73ff10) / 8],
    &retail_73ff10[(0x740830 - 0x73ff10) / 8], &retail_73ff10[(0x740838 - 0x73ff10) / 8], &retail_73ff10[(0x740840 - 0x73ff10) / 8], &retail_73ff10[(0x740848 - 0x73ff10) / 8],
    &retail_73ff10[(0x740850 - 0x73ff10) / 8], &retail_73ff10[(0x740858 - 0x73ff10) / 8], &retail_73ff10[(0x740860 - 0x73ff10) / 8], &retail_73ff10[(0x740868 - 0x73ff10) / 8],
    &retail_73ff10[(0x740870 - 0x73ff10) / 8], &retail_73ff10[(0x740878 - 0x73ff10) / 8], &retail_73ff10[(0x740880 - 0x73ff10) / 8], &retail_73ff10[(0x740888 - 0x73ff10) / 8],
    &retail_73ff10[(0x740890 - 0x73ff10) / 8], &retail_73ff10[(0x740898 - 0x73ff10) / 8], &retail_73ff10[(0x7408a0 - 0x73ff10) / 8], &retail_73ff10[(0x7408a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7408b0 - 0x73ff10) / 8], &retail_73ff10[(0x7408b8 - 0x73ff10) / 8], &retail_73ff10[(0x7408c0 - 0x73ff10) / 8], &retail_73ff10[(0x7408c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7408d0 - 0x73ff10) / 8], &retail_73ff10[(0x7408d8 - 0x73ff10) / 8], &retail_73ff10[(0x7408e0 - 0x73ff10) / 8], &retail_73ff10[(0x7408e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7408f0 - 0x73ff10) / 8], &retail_73ff10[(0x7408f8 - 0x73ff10) / 8], &retail_73ff10[(0x740900 - 0x73ff10) / 8], &retail_73ff10[(0x740908 - 0x73ff10) / 8],
    &retail_73ff10[(0x740910 - 0x73ff10) / 8], &retail_73ff10[(0x740918 - 0x73ff10) / 8], &retail_73ff10[(0x740920 - 0x73ff10) / 8], &retail_73ff10[(0x740928 - 0x73ff10) / 8],
    &retail_73ff10[(0x740930 - 0x73ff10) / 8], &retail_73ff10[(0x740938 - 0x73ff10) / 8], &retail_73ff10[(0x740940 - 0x73ff10) / 8], &retail_73ff10[(0x740948 - 0x73ff10) / 8],
    &retail_73ff10[(0x740950 - 0x73ff10) / 8], &retail_73ff10[(0x740958 - 0x73ff10) / 8], &retail_73ff10[(0x740960 - 0x73ff10) / 8], &retail_73ff10[(0x740968 - 0x73ff10) / 8],
    &retail_73ff10[(0x740970 - 0x73ff10) / 8], &retail_73ff10[(0x740978 - 0x73ff10) / 8], &retail_73ff10[(0x740980 - 0x73ff10) / 8], &retail_73ff10[(0x740988 - 0x73ff10) / 8],
    &retail_73ff10[(0x740990 - 0x73ff10) / 8], &retail_73ff10[(0x740998 - 0x73ff10) / 8], &retail_73ff10[(0x7409a0 - 0x73ff10) / 8], &retail_73ff10[(0x7409a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7409b0 - 0x73ff10) / 8], &retail_73ff10[(0x7409b8 - 0x73ff10) / 8], &retail_73ff10[(0x7409c0 - 0x73ff10) / 8], &retail_73ff10[(0x7409c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7409d0 - 0x73ff10) / 8], &retail_73ff10[(0x7409d8 - 0x73ff10) / 8], &retail_73ff10[(0x7409e0 - 0x73ff10) / 8], &retail_73ff10[(0x7409e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7409f0 - 0x73ff10) / 8], &retail_73ff10[(0x7409f8 - 0x73ff10) / 8], &retail_73ff10[(0x740a00 - 0x73ff10) / 8], &retail_73ff10[(0x740a08 - 0x73ff10) / 8],
    &retail_73ff10[(0x740a10 - 0x73ff10) / 8], &retail_73ff10[(0x740a18 - 0x73ff10) / 8], &retail_73ff10[(0x740a20 - 0x73ff10) / 8], &retail_73ff10[(0x740a28 - 0x73ff10) / 8],
    &retail_73ff10[(0x740a30 - 0x73ff10) / 8], &retail_73ff10[(0x740a38 - 0x73ff10) / 8], &retail_73ff10[(0x740a40 - 0x73ff10) / 8], &retail_73ff10[(0x740a48 - 0x73ff10) / 8],
    &retail_73ff10[(0x740a50 - 0x73ff10) / 8], &retail_73ff10[(0x740a58 - 0x73ff10) / 8], &retail_73ff10[(0x740a60 - 0x73ff10) / 8], &retail_73ff10[(0x740a68 - 0x73ff10) / 8],
    &retail_73ff10[(0x740a70 - 0x73ff10) / 8], &retail_73ff10[(0x740a78 - 0x73ff10) / 8], &retail_73ff10[(0x740a80 - 0x73ff10) / 8], &retail_73ff10[(0x740a88 - 0x73ff10) / 8],
    &retail_73ff10[(0x740a90 - 0x73ff10) / 8], &retail_73ff10[(0x740a98 - 0x73ff10) / 8], &retail_73ff10[(0x740aa0 - 0x73ff10) / 8], &retail_73ff10[(0x740aa8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740ab0 - 0x73ff10) / 8], &retail_73ff10[(0x740ab8 - 0x73ff10) / 8], &retail_73ff10[(0x740ac0 - 0x73ff10) / 8], &retail_73ff10[(0x740ac8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740ad0 - 0x73ff10) / 8], &retail_73ff10[(0x740ad8 - 0x73ff10) / 8], &retail_73ff10[(0x740ae0 - 0x73ff10) / 8], &retail_73ff10[(0x740ae8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740af0 - 0x73ff10) / 8], &retail_73ff10[(0x740af8 - 0x73ff10) / 8], &retail_73ff10[(0x740b00 - 0x73ff10) / 8], &retail_73ff10[(0x740b08 - 0x73ff10) / 8],
    &retail_73ff10[(0x740b10 - 0x73ff10) / 8], &retail_73ff10[(0x740b18 - 0x73ff10) / 8], &retail_73ff10[(0x740b20 - 0x73ff10) / 8], &retail_73ff10[(0x740b28 - 0x73ff10) / 8],
    &retail_73ff10[(0x740b30 - 0x73ff10) / 8], &retail_73ff10[(0x740b38 - 0x73ff10) / 8], &retail_73ff10[(0x740b40 - 0x73ff10) / 8], &retail_73ff10[(0x740b48 - 0x73ff10) / 8],
    &retail_73ff10[(0x740b50 - 0x73ff10) / 8], &retail_73ff10[(0x740b58 - 0x73ff10) / 8], &retail_73ff10[(0x740b60 - 0x73ff10) / 8], &retail_73ff10[(0x740b68 - 0x73ff10) / 8],
    &retail_73ff10[(0x740b70 - 0x73ff10) / 8], &retail_73ff10[(0x740b78 - 0x73ff10) / 8], &retail_73ff10[(0x740b80 - 0x73ff10) / 8], &retail_73ff10[(0x740b88 - 0x73ff10) / 8],
    &retail_73ff10[(0x740b90 - 0x73ff10) / 8], &retail_73ff10[(0x740b98 - 0x73ff10) / 8], &retail_73ff10[(0x740ba0 - 0x73ff10) / 8], &retail_73ff10[(0x740ba8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740bb0 - 0x73ff10) / 8], &retail_73ff10[(0x740bb8 - 0x73ff10) / 8], &retail_73ff10[(0x740bc0 - 0x73ff10) / 8], &retail_73ff10[(0x740bc8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740bd0 - 0x73ff10) / 8], &retail_73ff10[(0x740bd8 - 0x73ff10) / 8], &retail_73ff10[(0x740be0 - 0x73ff10) / 8], &retail_73ff10[(0x740be8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740bf0 - 0x73ff10) / 8], &retail_73ff10[(0x740bf8 - 0x73ff10) / 8], &retail_73ff10[(0x740c00 - 0x73ff10) / 8], &retail_73ff10[(0x740c08 - 0x73ff10) / 8],
    &retail_73ff10[(0x740c10 - 0x73ff10) / 8], &retail_73ff10[(0x740c18 - 0x73ff10) / 8], &retail_73ff10[(0x740c20 - 0x73ff10) / 8], &retail_73ff10[(0x740c28 - 0x73ff10) / 8],
    &retail_73ff10[(0x740c30 - 0x73ff10) / 8], &retail_73ff10[(0x740c38 - 0x73ff10) / 8], &retail_73ff10[(0x740c40 - 0x73ff10) / 8], &retail_73ff10[(0x740c48 - 0x73ff10) / 8],
    &retail_73ff10[(0x740c50 - 0x73ff10) / 8], &retail_73ff10[(0x740c58 - 0x73ff10) / 8], &retail_73ff10[(0x740c60 - 0x73ff10) / 8], &retail_73ff10[(0x740c68 - 0x73ff10) / 8],
    &retail_73ff10[(0x740c70 - 0x73ff10) / 8], &retail_73ff10[(0x740c78 - 0x73ff10) / 8], &retail_73ff10[(0x740c80 - 0x73ff10) / 8], &retail_73ff10[(0x740c88 - 0x73ff10) / 8],
    &retail_73ff10[(0x740c90 - 0x73ff10) / 8], &retail_73ff10[(0x740c98 - 0x73ff10) / 8], &retail_73ff10[(0x740ca0 - 0x73ff10) / 8], &retail_73ff10[(0x740ca8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740cb0 - 0x73ff10) / 8], &retail_73ff10[(0x740cb8 - 0x73ff10) / 8], &retail_73ff10[(0x740cc0 - 0x73ff10) / 8], &retail_73ff10[(0x740cc8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740cd0 - 0x73ff10) / 8], &retail_73ff10[(0x740cd8 - 0x73ff10) / 8], &retail_73ff10[(0x740ce0 - 0x73ff10) / 8], &retail_73ff10[(0x740ce8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740cf0 - 0x73ff10) / 8], &retail_73ff10[(0x740cf8 - 0x73ff10) / 8], &retail_73ff10[(0x740d00 - 0x73ff10) / 8], &retail_73ff10[(0x740d08 - 0x73ff10) / 8],
    &retail_73ff10[(0x740d10 - 0x73ff10) / 8], &retail_73ff10[(0x740d18 - 0x73ff10) / 8], &retail_73ff10[(0x740d20 - 0x73ff10) / 8], &retail_73ff10[(0x740d28 - 0x73ff10) / 8],
    &retail_73ff10[(0x740d30 - 0x73ff10) / 8], &retail_73ff10[(0x740d38 - 0x73ff10) / 8], &retail_73ff10[(0x740d40 - 0x73ff10) / 8], &retail_73ff10[(0x740d48 - 0x73ff10) / 8],
    &retail_73ff10[(0x740d50 - 0x73ff10) / 8], &retail_73ff10[(0x740d58 - 0x73ff10) / 8], &retail_73ff10[(0x740d60 - 0x73ff10) / 8], &retail_73ff10[(0x740d68 - 0x73ff10) / 8],
    &retail_73ff10[(0x740d70 - 0x73ff10) / 8], &retail_73ff10[(0x740d78 - 0x73ff10) / 8], &retail_73ff10[(0x740d80 - 0x73ff10) / 8], &retail_73ff10[(0x740d88 - 0x73ff10) / 8],
    &retail_73ff10[(0x740d90 - 0x73ff10) / 8], &retail_73ff10[(0x740d98 - 0x73ff10) / 8], &retail_73ff10[(0x740da0 - 0x73ff10) / 8], &retail_73ff10[(0x740da8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740db0 - 0x73ff10) / 8], &retail_73ff10[(0x740db8 - 0x73ff10) / 8], &retail_73ff10[(0x740dc0 - 0x73ff10) / 8], &retail_73ff10[(0x740dc8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740dd0 - 0x73ff10) / 8], &retail_73ff10[(0x740dd8 - 0x73ff10) / 8], &retail_73ff10[(0x740de0 - 0x73ff10) / 8], &retail_73ff10[(0x740de8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740df0 - 0x73ff10) / 8], &retail_73ff10[(0x740df8 - 0x73ff10) / 8], &retail_73ff10[(0x740e00 - 0x73ff10) / 8], &retail_73ff10[(0x740e08 - 0x73ff10) / 8],
    &retail_73ff10[(0x740e10 - 0x73ff10) / 8], &retail_73ff10[(0x740e18 - 0x73ff10) / 8], &retail_73ff10[(0x740e20 - 0x73ff10) / 8], &retail_73ff10[(0x740e28 - 0x73ff10) / 8],
    &retail_73ff10[(0x740e30 - 0x73ff10) / 8], &retail_73ff10[(0x740e38 - 0x73ff10) / 8], &retail_73ff10[(0x740e40 - 0x73ff10) / 8], &retail_73ff10[(0x740e48 - 0x73ff10) / 8],
    &retail_73ff10[(0x740e50 - 0x73ff10) / 8], &retail_73ff10[(0x740e58 - 0x73ff10) / 8], &retail_73ff10[(0x740e60 - 0x73ff10) / 8], &retail_73ff10[(0x740e68 - 0x73ff10) / 8],
    &retail_73ff10[(0x740e70 - 0x73ff10) / 8], &retail_73ff10[(0x740e78 - 0x73ff10) / 8], &retail_73ff10[(0x740e80 - 0x73ff10) / 8], &retail_73ff10[(0x740e88 - 0x73ff10) / 8],
    &retail_73ff10[(0x740e90 - 0x73ff10) / 8], &retail_73ff10[(0x740e98 - 0x73ff10) / 8], &retail_73ff10[(0x740ea0 - 0x73ff10) / 8], &retail_73ff10[(0x740ea8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740eb0 - 0x73ff10) / 8], &retail_73ff10[(0x740eb8 - 0x73ff10) / 8], &retail_73ff10[(0x740ec0 - 0x73ff10) / 8], &retail_73ff10[(0x740ec8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740ed0 - 0x73ff10) / 8], &retail_73ff10[(0x740ed8 - 0x73ff10) / 8], &retail_73ff10[(0x740ee0 - 0x73ff10) / 8], &retail_73ff10[(0x740ee8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740ef0 - 0x73ff10) / 8], &retail_73ff10[(0x740ef8 - 0x73ff10) / 8], &retail_73ff10[(0x740f00 - 0x73ff10) / 8], &retail_73ff10[(0x740f08 - 0x73ff10) / 8],
    &retail_73ff10[(0x740f10 - 0x73ff10) / 8], &retail_73ff10[(0x740f18 - 0x73ff10) / 8], &retail_73ff10[(0x740f20 - 0x73ff10) / 8], &retail_73ff10[(0x740f28 - 0x73ff10) / 8],
    &retail_73ff10[(0x740f30 - 0x73ff10) / 8], &retail_73ff10[(0x740f38 - 0x73ff10) / 8], &retail_73ff10[(0x740f40 - 0x73ff10) / 8], &retail_73ff10[(0x740f48 - 0x73ff10) / 8],
    &retail_73ff10[(0x740f50 - 0x73ff10) / 8], &retail_73ff10[(0x740f58 - 0x73ff10) / 8], &retail_73ff10[(0x740f60 - 0x73ff10) / 8], &retail_73ff10[(0x740f68 - 0x73ff10) / 8],
    &retail_73ff10[(0x740f70 - 0x73ff10) / 8], &retail_73ff10[(0x740f78 - 0x73ff10) / 8], &retail_73ff10[(0x740f80 - 0x73ff10) / 8], &retail_73ff10[(0x740f88 - 0x73ff10) / 8],
    &retail_73ff10[(0x740f90 - 0x73ff10) / 8], &retail_73ff10[(0x740f98 - 0x73ff10) / 8], &retail_73ff10[(0x740fa0 - 0x73ff10) / 8], &retail_73ff10[(0x740fa8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740fb0 - 0x73ff10) / 8], &retail_73ff10[(0x740fb8 - 0x73ff10) / 8], &retail_73ff10[(0x740fc0 - 0x73ff10) / 8], &retail_73ff10[(0x740fc8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740fd0 - 0x73ff10) / 8], &retail_73ff10[(0x740fd8 - 0x73ff10) / 8], &retail_73ff10[(0x740fe0 - 0x73ff10) / 8], &retail_73ff10[(0x740fe8 - 0x73ff10) / 8],
    &retail_73ff10[(0x740ff0 - 0x73ff10) / 8], &retail_73ff10[(0x740ff8 - 0x73ff10) / 8], &retail_73ff10[(0x741000 - 0x73ff10) / 8], &retail_73ff10[(0x741008 - 0x73ff10) / 8],
    &retail_73ff10[(0x741010 - 0x73ff10) / 8], &retail_73ff10[(0x741018 - 0x73ff10) / 8], &retail_73ff10[(0x741020 - 0x73ff10) / 8], &retail_73ff10[(0x741028 - 0x73ff10) / 8],
    &retail_73ff10[(0x741030 - 0x73ff10) / 8], &retail_73ff10[(0x741038 - 0x73ff10) / 8], &retail_73ff10[(0x741040 - 0x73ff10) / 8], &retail_73ff10[(0x741048 - 0x73ff10) / 8],
    &retail_73ff10[(0x741050 - 0x73ff10) / 8], &retail_73ff10[(0x741058 - 0x73ff10) / 8], &retail_73ff10[(0x741060 - 0x73ff10) / 8], &retail_73ff10[(0x741068 - 0x73ff10) / 8],
    &retail_73ff10[(0x741070 - 0x73ff10) / 8], &retail_73ff10[(0x741078 - 0x73ff10) / 8], &retail_73ff10[(0x741080 - 0x73ff10) / 8], &retail_73ff10[(0x741088 - 0x73ff10) / 8],
    &retail_73ff10[(0x741090 - 0x73ff10) / 8], &retail_73ff10[(0x741098 - 0x73ff10) / 8], &retail_73ff10[(0x7410a0 - 0x73ff10) / 8], &retail_73ff10[(0x7410a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7410b0 - 0x73ff10) / 8], &retail_73ff10[(0x7410b8 - 0x73ff10) / 8], &retail_73ff10[(0x7410c0 - 0x73ff10) / 8], &retail_73ff10[(0x7410c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7410d0 - 0x73ff10) / 8], &retail_73ff10[(0x7410d8 - 0x73ff10) / 8], &retail_73ff10[(0x7410e0 - 0x73ff10) / 8], &retail_73ff10[(0x7410e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7410f0 - 0x73ff10) / 8], &retail_73ff10[(0x7410f8 - 0x73ff10) / 8], &retail_73ff10[(0x741100 - 0x73ff10) / 8], &retail_73ff10[(0x741108 - 0x73ff10) / 8],
    &retail_73ff10[(0x741110 - 0x73ff10) / 8], &retail_73ff10[(0x741118 - 0x73ff10) / 8], &retail_73ff10[(0x741120 - 0x73ff10) / 8], &retail_73ff10[(0x741128 - 0x73ff10) / 8],
    &retail_73ff10[(0x741130 - 0x73ff10) / 8], &retail_73ff10[(0x741138 - 0x73ff10) / 8], &retail_73ff10[(0x741140 - 0x73ff10) / 8], &retail_73ff10[(0x741148 - 0x73ff10) / 8],
    &retail_73ff10[(0x741150 - 0x73ff10) / 8], &retail_73ff10[(0x741158 - 0x73ff10) / 8], &retail_73ff10[(0x741160 - 0x73ff10) / 8], &retail_73ff10[(0x741168 - 0x73ff10) / 8],
    &retail_73ff10[(0x741170 - 0x73ff10) / 8], &retail_73ff10[(0x741178 - 0x73ff10) / 8], &retail_73ff10[(0x741180 - 0x73ff10) / 8], &retail_73ff10[(0x741188 - 0x73ff10) / 8],
    &retail_73ff10[(0x741190 - 0x73ff10) / 8], &retail_73ff10[(0x741198 - 0x73ff10) / 8], &retail_73ff10[(0x7411a0 - 0x73ff10) / 8], &retail_73ff10[(0x7411a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7411b0 - 0x73ff10) / 8], &retail_73ff10[(0x7411b8 - 0x73ff10) / 8], &retail_73ff10[(0x7411c0 - 0x73ff10) / 8], &retail_73ff10[(0x7411c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7411d0 - 0x73ff10) / 8], &retail_73ff10[(0x7411d8 - 0x73ff10) / 8], &retail_73ff10[(0x7411e0 - 0x73ff10) / 8], &retail_73ff10[(0x7411e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7411f0 - 0x73ff10) / 8], &retail_73ff10[(0x7411f8 - 0x73ff10) / 8], &retail_73ff10[(0x741200 - 0x73ff10) / 8], &retail_73ff10[(0x741208 - 0x73ff10) / 8],
    &retail_73ff10[(0x741210 - 0x73ff10) / 8], &retail_73ff10[(0x741218 - 0x73ff10) / 8], &retail_73ff10[(0x741220 - 0x73ff10) / 8], &retail_73ff10[(0x741228 - 0x73ff10) / 8],
    &retail_73ff10[(0x741230 - 0x73ff10) / 8], &retail_73ff10[(0x741238 - 0x73ff10) / 8], &retail_73ff10[(0x741240 - 0x73ff10) / 8], &retail_73ff10[(0x741248 - 0x73ff10) / 8],
    &retail_73ff10[(0x741250 - 0x73ff10) / 8], &retail_73ff10[(0x741258 - 0x73ff10) / 8], &retail_73ff10[(0x741260 - 0x73ff10) / 8], &retail_73ff10[(0x741268 - 0x73ff10) / 8],
    &retail_73ff10[(0x741270 - 0x73ff10) / 8], &retail_73ff10[(0x741278 - 0x73ff10) / 8], &retail_73ff10[(0x741280 - 0x73ff10) / 8], &retail_73ff10[(0x741288 - 0x73ff10) / 8],
    &retail_73ff10[(0x741290 - 0x73ff10) / 8], &retail_73ff10[(0x741298 - 0x73ff10) / 8], &retail_73ff10[(0x7412a0 - 0x73ff10) / 8], &retail_73ff10[(0x7412a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7412b0 - 0x73ff10) / 8], &retail_73ff10[(0x7412b8 - 0x73ff10) / 8], &retail_73ff10[(0x7412c0 - 0x73ff10) / 8], &retail_73ff10[(0x7412c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7412d0 - 0x73ff10) / 8], &retail_73ff10[(0x7412d8 - 0x73ff10) / 8], &retail_73ff10[(0x7412e0 - 0x73ff10) / 8], &retail_73ff10[(0x7412e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7412f0 - 0x73ff10) / 8], &retail_73ff10[(0x7412f8 - 0x73ff10) / 8], &retail_73ff10[(0x741300 - 0x73ff10) / 8], &retail_73ff10[(0x741308 - 0x73ff10) / 8],
    &retail_73ff10[(0x741310 - 0x73ff10) / 8], &retail_73ff10[(0x741318 - 0x73ff10) / 8], &retail_73ff10[(0x741320 - 0x73ff10) / 8], &retail_73ff10[(0x741328 - 0x73ff10) / 8],
    &retail_73ff10[(0x741330 - 0x73ff10) / 8], &retail_73ff10[(0x741338 - 0x73ff10) / 8], &retail_73ff10[(0x741340 - 0x73ff10) / 8], &retail_73ff10[(0x741348 - 0x73ff10) / 8],
    &retail_73ff10[(0x741350 - 0x73ff10) / 8], &retail_73ff10[(0x741358 - 0x73ff10) / 8], &retail_73ff10[(0x741360 - 0x73ff10) / 8], &retail_73ff10[(0x741368 - 0x73ff10) / 8],
    &retail_73ff10[(0x741370 - 0x73ff10) / 8], &retail_73ff10[(0x741378 - 0x73ff10) / 8], &retail_73ff10[(0x741380 - 0x73ff10) / 8], &retail_73ff10[(0x741388 - 0x73ff10) / 8],
    &retail_73ff10[(0x741390 - 0x73ff10) / 8], &retail_73ff10[(0x741398 - 0x73ff10) / 8], &retail_73ff10[(0x7413a0 - 0x73ff10) / 8], &retail_73ff10[(0x7413a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7413b0 - 0x73ff10) / 8], &retail_73ff10[(0x7413b8 - 0x73ff10) / 8], &retail_73ff10[(0x7413c0 - 0x73ff10) / 8], &retail_73ff10[(0x7413c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7413d0 - 0x73ff10) / 8], &retail_73ff10[(0x7413d8 - 0x73ff10) / 8], &retail_73ff10[(0x7413e0 - 0x73ff10) / 8], &retail_73ff10[(0x7413e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7413f0 - 0x73ff10) / 8], &retail_73ff10[(0x7413f8 - 0x73ff10) / 8], &retail_73ff10[(0x741400 - 0x73ff10) / 8], &retail_73ff10[(0x741408 - 0x73ff10) / 8],
    &retail_73ff10[(0x741410 - 0x73ff10) / 8], &retail_73ff10[(0x741418 - 0x73ff10) / 8], &retail_73ff10[(0x741420 - 0x73ff10) / 8], &retail_73ff10[(0x741428 - 0x73ff10) / 8],
    &retail_73ff10[(0x741430 - 0x73ff10) / 8], &retail_73ff10[(0x741438 - 0x73ff10) / 8], &retail_73ff10[(0x741440 - 0x73ff10) / 8], &retail_73ff10[(0x741448 - 0x73ff10) / 8],
    &retail_73ff10[(0x741450 - 0x73ff10) / 8], &retail_73ff10[(0x741458 - 0x73ff10) / 8], &retail_73ff10[(0x741460 - 0x73ff10) / 8], &retail_73ff10[(0x741468 - 0x73ff10) / 8],
    &retail_73ff10[(0x741470 - 0x73ff10) / 8], &retail_73ff10[(0x741478 - 0x73ff10) / 8], &retail_73ff10[(0x741480 - 0x73ff10) / 8], &retail_73ff10[(0x741488 - 0x73ff10) / 8],
    &retail_73ff10[(0x741490 - 0x73ff10) / 8], &retail_73ff10[(0x741498 - 0x73ff10) / 8], &retail_73ff10[(0x7414a0 - 0x73ff10) / 8], &retail_73ff10[(0x7414a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7414b0 - 0x73ff10) / 8], &retail_73ff10[(0x7414b8 - 0x73ff10) / 8], &retail_73ff10[(0x7414c0 - 0x73ff10) / 8], &retail_73ff10[(0x7414c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7414d0 - 0x73ff10) / 8], &retail_73ff10[(0x7414d8 - 0x73ff10) / 8], &retail_73ff10[(0x7414e0 - 0x73ff10) / 8], &retail_73ff10[(0x7414e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7414f0 - 0x73ff10) / 8], &retail_73ff10[(0x7414f8 - 0x73ff10) / 8], &retail_73ff10[(0x741500 - 0x73ff10) / 8], &retail_73ff10[(0x741508 - 0x73ff10) / 8],
    &retail_73ff10[(0x741510 - 0x73ff10) / 8], &retail_73ff10[(0x741518 - 0x73ff10) / 8], &retail_73ff10[(0x741520 - 0x73ff10) / 8], &retail_73ff10[(0x741528 - 0x73ff10) / 8],
    &retail_73ff10[(0x741530 - 0x73ff10) / 8], &retail_73ff10[(0x741538 - 0x73ff10) / 8], &retail_73ff10[(0x741540 - 0x73ff10) / 8], &retail_73ff10[(0x741548 - 0x73ff10) / 8],
    &retail_73ff10[(0x741550 - 0x73ff10) / 8], &retail_73ff10[(0x741558 - 0x73ff10) / 8], &retail_73ff10[(0x741560 - 0x73ff10) / 8], &retail_73ff10[(0x741568 - 0x73ff10) / 8],
    &retail_73ff10[(0x741570 - 0x73ff10) / 8], &retail_73ff10[(0x741578 - 0x73ff10) / 8], &retail_73ff10[(0x741580 - 0x73ff10) / 8], &retail_73ff10[(0x741588 - 0x73ff10) / 8],
    &retail_73ff10[(0x741590 - 0x73ff10) / 8], &retail_73ff10[(0x741598 - 0x73ff10) / 8], &retail_73ff10[(0x7415a0 - 0x73ff10) / 8], &retail_73ff10[(0x7415a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7415b0 - 0x73ff10) / 8], &retail_73ff10[(0x7415b8 - 0x73ff10) / 8], &retail_73ff10[(0x7415c0 - 0x73ff10) / 8], &retail_73ff10[(0x7415c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7415d0 - 0x73ff10) / 8], &retail_73ff10[(0x7415d8 - 0x73ff10) / 8], &retail_73ff10[(0x7415e0 - 0x73ff10) / 8], &retail_73ff10[(0x7415e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7415f0 - 0x73ff10) / 8], &retail_73ff10[(0x7415f8 - 0x73ff10) / 8], &retail_73ff10[(0x741600 - 0x73ff10) / 8], &retail_73ff10[(0x741608 - 0x73ff10) / 8],
    &retail_73ff10[(0x741610 - 0x73ff10) / 8], &retail_73ff10[(0x741618 - 0x73ff10) / 8], &retail_73ff10[(0x741620 - 0x73ff10) / 8], &retail_73ff10[(0x741628 - 0x73ff10) / 8],
    &retail_73ff10[(0x741630 - 0x73ff10) / 8], &retail_73ff10[(0x741638 - 0x73ff10) / 8], &retail_73ff10[(0x741640 - 0x73ff10) / 8], &retail_73ff10[(0x741648 - 0x73ff10) / 8],
    &retail_73ff10[(0x741650 - 0x73ff10) / 8], &retail_73ff10[(0x741658 - 0x73ff10) / 8], &retail_73ff10[(0x741660 - 0x73ff10) / 8], &retail_73ff10[(0x741668 - 0x73ff10) / 8],
    &retail_73ff10[(0x741670 - 0x73ff10) / 8], &retail_73ff10[(0x741678 - 0x73ff10) / 8], &retail_73ff10[(0x741680 - 0x73ff10) / 8], &retail_73ff10[(0x741688 - 0x73ff10) / 8],
    &retail_73ff10[(0x741690 - 0x73ff10) / 8], &retail_73ff10[(0x741698 - 0x73ff10) / 8], &retail_73ff10[(0x7416a0 - 0x73ff10) / 8], &retail_73ff10[(0x7416a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7416b0 - 0x73ff10) / 8], &retail_73ff10[(0x7416b8 - 0x73ff10) / 8], &retail_73ff10[(0x7416c0 - 0x73ff10) / 8], &retail_73ff10[(0x7416c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7416d0 - 0x73ff10) / 8], &retail_73ff10[(0x7416d8 - 0x73ff10) / 8], &retail_73ff10[(0x7416e0 - 0x73ff10) / 8], &retail_73ff10[(0x7416e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7416f0 - 0x73ff10) / 8], &retail_73ff10[(0x7416f8 - 0x73ff10) / 8], &retail_73ff10[(0x741700 - 0x73ff10) / 8], &retail_73ff10[(0x741708 - 0x73ff10) / 8],
    &retail_73ff10[(0x741710 - 0x73ff10) / 8], &retail_73ff10[(0x741718 - 0x73ff10) / 8], &retail_73ff10[(0x741720 - 0x73ff10) / 8], &retail_73ff10[(0x741728 - 0x73ff10) / 8],
    &retail_73ff10[(0x741730 - 0x73ff10) / 8], &retail_73ff10[(0x741738 - 0x73ff10) / 8], &retail_73ff10[(0x741740 - 0x73ff10) / 8], &retail_73ff10[(0x741748 - 0x73ff10) / 8],
    &retail_73ff10[(0x741750 - 0x73ff10) / 8], &retail_73ff10[(0x741758 - 0x73ff10) / 8], &retail_73ff10[(0x741760 - 0x73ff10) / 8], &retail_73ff10[(0x741768 - 0x73ff10) / 8],
    &retail_73ff10[(0x741770 - 0x73ff10) / 8], &retail_73ff10[(0x741778 - 0x73ff10) / 8], &retail_73ff10[(0x741780 - 0x73ff10) / 8], &retail_73ff10[(0x741788 - 0x73ff10) / 8],
    &retail_73ff10[(0x741790 - 0x73ff10) / 8], &retail_73ff10[(0x741798 - 0x73ff10) / 8], &retail_73ff10[(0x7417a0 - 0x73ff10) / 8], &retail_73ff10[(0x7417a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7417b0 - 0x73ff10) / 8], &retail_73ff10[(0x7417b8 - 0x73ff10) / 8], &retail_73ff10[(0x7417c0 - 0x73ff10) / 8], &retail_73ff10[(0x7417c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7417d0 - 0x73ff10) / 8], &retail_73ff10[(0x7417d8 - 0x73ff10) / 8], &retail_73ff10[(0x7417e0 - 0x73ff10) / 8], &retail_73ff10[(0x7417e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7417f0 - 0x73ff10) / 8], &retail_73ff10[(0x7417f8 - 0x73ff10) / 8], &retail_73ff10[(0x741800 - 0x73ff10) / 8], &retail_73ff10[(0x741808 - 0x73ff10) / 8],
    &retail_73ff10[(0x741810 - 0x73ff10) / 8], &retail_73ff10[(0x741818 - 0x73ff10) / 8], &retail_73ff10[(0x741820 - 0x73ff10) / 8], &retail_73ff10[(0x741828 - 0x73ff10) / 8],
    &retail_73ff10[(0x741830 - 0x73ff10) / 8], &retail_73ff10[(0x741838 - 0x73ff10) / 8], &retail_73ff10[(0x741840 - 0x73ff10) / 8], &retail_73ff10[(0x741848 - 0x73ff10) / 8],
    &retail_73ff10[(0x741850 - 0x73ff10) / 8], &retail_73ff10[(0x741858 - 0x73ff10) / 8], &retail_73ff10[(0x741860 - 0x73ff10) / 8], &retail_73ff10[(0x741868 - 0x73ff10) / 8],
    &retail_73ff10[(0x741870 - 0x73ff10) / 8], &retail_73ff10[(0x741878 - 0x73ff10) / 8], &retail_73ff10[(0x741880 - 0x73ff10) / 8], &retail_73ff10[(0x741888 - 0x73ff10) / 8],
    &retail_73ff10[(0x741890 - 0x73ff10) / 8], &retail_73ff10[(0x741898 - 0x73ff10) / 8], &retail_73ff10[(0x7418a0 - 0x73ff10) / 8], &retail_73ff10[(0x7418a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7418b0 - 0x73ff10) / 8], &retail_73ff10[(0x7418b8 - 0x73ff10) / 8], &retail_73ff10[(0x7418c0 - 0x73ff10) / 8], &retail_73ff10[(0x7418c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7418d0 - 0x73ff10) / 8], &retail_73ff10[(0x7418d8 - 0x73ff10) / 8], &retail_73ff10[(0x7418e0 - 0x73ff10) / 8], &retail_73ff10[(0x7418e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7418f0 - 0x73ff10) / 8], &retail_73ff10[(0x7418f8 - 0x73ff10) / 8], &retail_73ff10[(0x741900 - 0x73ff10) / 8], &retail_73ff10[(0x741908 - 0x73ff10) / 8],
    &retail_73ff10[(0x741910 - 0x73ff10) / 8], &retail_73ff10[(0x741918 - 0x73ff10) / 8], &retail_73ff10[(0x741920 - 0x73ff10) / 8], &retail_73ff10[(0x741928 - 0x73ff10) / 8],
    &retail_73ff10[(0x741930 - 0x73ff10) / 8], &retail_73ff10[(0x741938 - 0x73ff10) / 8], &retail_73ff10[(0x741940 - 0x73ff10) / 8], &retail_73ff10[(0x741948 - 0x73ff10) / 8],
    &retail_73ff10[(0x741950 - 0x73ff10) / 8], &retail_73ff10[(0x741958 - 0x73ff10) / 8], &retail_73ff10[(0x741960 - 0x73ff10) / 8], &retail_73ff10[(0x741968 - 0x73ff10) / 8],
    &retail_73ff10[(0x741970 - 0x73ff10) / 8], &retail_73ff10[(0x741978 - 0x73ff10) / 8], &retail_73ff10[(0x741980 - 0x73ff10) / 8], &retail_73ff10[(0x741988 - 0x73ff10) / 8],
    &retail_73ff10[(0x741990 - 0x73ff10) / 8], &retail_73ff10[(0x741998 - 0x73ff10) / 8], &retail_73ff10[(0x7419a0 - 0x73ff10) / 8], &retail_73ff10[(0x7419a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7419b0 - 0x73ff10) / 8], &retail_73ff10[(0x7419b8 - 0x73ff10) / 8], &retail_73ff10[(0x7419c0 - 0x73ff10) / 8], &retail_73ff10[(0x7419c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7419d0 - 0x73ff10) / 8], &retail_73ff10[(0x7419d8 - 0x73ff10) / 8], &retail_73ff10[(0x7419e0 - 0x73ff10) / 8], &retail_73ff10[(0x7419e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7419f0 - 0x73ff10) / 8], &retail_73ff10[(0x7419f8 - 0x73ff10) / 8], &retail_73ff10[(0x741a00 - 0x73ff10) / 8], &retail_73ff10[(0x741a08 - 0x73ff10) / 8],
    &retail_73ff10[(0x741a10 - 0x73ff10) / 8], &retail_73ff10[(0x741a18 - 0x73ff10) / 8], &retail_73ff10[(0x741a20 - 0x73ff10) / 8], &retail_73ff10[(0x741a28 - 0x73ff10) / 8],
    &retail_73ff10[(0x741a30 - 0x73ff10) / 8], &retail_73ff10[(0x741a38 - 0x73ff10) / 8], &retail_73ff10[(0x741a40 - 0x73ff10) / 8], &retail_73ff10[(0x741a48 - 0x73ff10) / 8],
    &retail_73ff10[(0x741a50 - 0x73ff10) / 8], &retail_73ff10[(0x741a58 - 0x73ff10) / 8], &retail_73ff10[(0x741a60 - 0x73ff10) / 8], &retail_73ff10[(0x741a68 - 0x73ff10) / 8],
    &retail_73ff10[(0x741a70 - 0x73ff10) / 8], &retail_73ff10[(0x741a78 - 0x73ff10) / 8], &retail_73ff10[(0x741a80 - 0x73ff10) / 8], &retail_73ff10[(0x741a88 - 0x73ff10) / 8],
    &retail_73ff10[(0x741a90 - 0x73ff10) / 8], &retail_73ff10[(0x741a98 - 0x73ff10) / 8], &retail_73ff10[(0x741aa0 - 0x73ff10) / 8], &retail_73ff10[(0x741aa8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741ab0 - 0x73ff10) / 8], &retail_73ff10[(0x741ab8 - 0x73ff10) / 8], &retail_73ff10[(0x741ac0 - 0x73ff10) / 8], &retail_73ff10[(0x741ac8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741ad0 - 0x73ff10) / 8], &retail_73ff10[(0x741ad8 - 0x73ff10) / 8], &retail_73ff10[(0x741ae0 - 0x73ff10) / 8], &retail_73ff10[(0x741ae8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741af0 - 0x73ff10) / 8], &retail_73ff10[(0x741af8 - 0x73ff10) / 8], &retail_73ff10[(0x741b00 - 0x73ff10) / 8], &retail_73ff10[(0x741b08 - 0x73ff10) / 8],
    &retail_73ff10[(0x741b10 - 0x73ff10) / 8], &retail_73ff10[(0x741b18 - 0x73ff10) / 8], &retail_73ff10[(0x741b20 - 0x73ff10) / 8], &retail_73ff10[(0x741b28 - 0x73ff10) / 8],
    &retail_73ff10[(0x741b30 - 0x73ff10) / 8], &retail_73ff10[(0x741b38 - 0x73ff10) / 8], &retail_73ff10[(0x741b40 - 0x73ff10) / 8], &retail_73ff10[(0x741b48 - 0x73ff10) / 8],
    &retail_73ff10[(0x741b50 - 0x73ff10) / 8], &retail_73ff10[(0x741b58 - 0x73ff10) / 8], &retail_73ff10[(0x741b60 - 0x73ff10) / 8], &retail_73ff10[(0x741b68 - 0x73ff10) / 8],
    &retail_73ff10[(0x741b70 - 0x73ff10) / 8], &retail_73ff10[(0x741b78 - 0x73ff10) / 8], &retail_73ff10[(0x741b80 - 0x73ff10) / 8], &retail_73ff10[(0x741b88 - 0x73ff10) / 8],
    &retail_73ff10[(0x741b90 - 0x73ff10) / 8], &retail_73ff10[(0x741b98 - 0x73ff10) / 8], &retail_73ff10[(0x741ba0 - 0x73ff10) / 8], &retail_73ff10[(0x741ba8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741bb0 - 0x73ff10) / 8], &retail_73ff10[(0x741bb8 - 0x73ff10) / 8], &retail_73ff10[(0x741bc0 - 0x73ff10) / 8], &retail_73ff10[(0x741bc8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741bd0 - 0x73ff10) / 8], &retail_73ff10[(0x741bd8 - 0x73ff10) / 8], &retail_73ff10[(0x741be0 - 0x73ff10) / 8], &retail_73ff10[(0x741be8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741bf0 - 0x73ff10) / 8], &retail_73ff10[(0x741bf8 - 0x73ff10) / 8], &retail_73ff10[(0x741c00 - 0x73ff10) / 8], &retail_73ff10[(0x741c08 - 0x73ff10) / 8],
    &retail_73ff10[(0x741c10 - 0x73ff10) / 8], &retail_73ff10[(0x741c18 - 0x73ff10) / 8], &retail_73ff10[(0x741c20 - 0x73ff10) / 8], &retail_73ff10[(0x741c28 - 0x73ff10) / 8],
    &retail_73ff10[(0x741c30 - 0x73ff10) / 8], &retail_73ff10[(0x741c38 - 0x73ff10) / 8], &retail_73ff10[(0x741c40 - 0x73ff10) / 8], &retail_73ff10[(0x741c48 - 0x73ff10) / 8],
    &retail_73ff10[(0x741c50 - 0x73ff10) / 8], &retail_73ff10[(0x741c58 - 0x73ff10) / 8], &retail_73ff10[(0x741c60 - 0x73ff10) / 8], &retail_73ff10[(0x741c68 - 0x73ff10) / 8],
    &retail_73ff10[(0x741c70 - 0x73ff10) / 8], &retail_73ff10[(0x741c78 - 0x73ff10) / 8], &retail_73ff10[(0x741c80 - 0x73ff10) / 8], &retail_73ff10[(0x741c88 - 0x73ff10) / 8],
    &retail_73ff10[(0x741c90 - 0x73ff10) / 8], &retail_73ff10[(0x741c98 - 0x73ff10) / 8], &retail_73ff10[(0x741ca0 - 0x73ff10) / 8], &retail_73ff10[(0x741ca8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741cb0 - 0x73ff10) / 8], &retail_73ff10[(0x741cb8 - 0x73ff10) / 8], &retail_73ff10[(0x741cc0 - 0x73ff10) / 8], &retail_73ff10[(0x741cc8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741cd0 - 0x73ff10) / 8], &retail_73ff10[(0x741cd8 - 0x73ff10) / 8], &retail_73ff10[(0x741ce0 - 0x73ff10) / 8], &retail_73ff10[(0x741ce8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741cf0 - 0x73ff10) / 8], &retail_73ff10[(0x741cf8 - 0x73ff10) / 8], &retail_73ff10[(0x741d00 - 0x73ff10) / 8], &retail_73ff10[(0x741d08 - 0x73ff10) / 8],
    &retail_73ff10[(0x741d10 - 0x73ff10) / 8], &retail_73ff10[(0x741d18 - 0x73ff10) / 8], &retail_73ff10[(0x741d20 - 0x73ff10) / 8], &retail_73ff10[(0x741d28 - 0x73ff10) / 8],
    &retail_73ff10[(0x741d30 - 0x73ff10) / 8], &retail_73ff10[(0x741d38 - 0x73ff10) / 8], &retail_73ff10[(0x741d40 - 0x73ff10) / 8], &retail_73ff10[(0x741d48 - 0x73ff10) / 8],
    &retail_73ff10[(0x741d50 - 0x73ff10) / 8], &retail_73ff10[(0x741d58 - 0x73ff10) / 8], &retail_73ff10[(0x741d60 - 0x73ff10) / 8], &retail_73ff10[(0x741d68 - 0x73ff10) / 8],
    &retail_73ff10[(0x741d70 - 0x73ff10) / 8], &retail_73ff10[(0x741d78 - 0x73ff10) / 8], &retail_73ff10[(0x741d80 - 0x73ff10) / 8], &retail_73ff10[(0x741d88 - 0x73ff10) / 8],
    &retail_73ff10[(0x741d90 - 0x73ff10) / 8], &retail_73ff10[(0x741d98 - 0x73ff10) / 8], &retail_73ff10[(0x741da0 - 0x73ff10) / 8], &retail_73ff10[(0x741da8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741db0 - 0x73ff10) / 8], &retail_73ff10[(0x741db8 - 0x73ff10) / 8], &retail_73ff10[(0x741dc0 - 0x73ff10) / 8], &retail_73ff10[(0x741dc8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741dd0 - 0x73ff10) / 8], &retail_73ff10[(0x741dd8 - 0x73ff10) / 8], &retail_73ff10[(0x741de0 - 0x73ff10) / 8], &retail_73ff10[(0x741de8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741df0 - 0x73ff10) / 8], &retail_73ff10[(0x741df8 - 0x73ff10) / 8], &retail_73ff10[(0x741e00 - 0x73ff10) / 8], &retail_73ff10[(0x741e08 - 0x73ff10) / 8],
    &retail_73ff10[(0x741e10 - 0x73ff10) / 8], &retail_73ff10[(0x741e18 - 0x73ff10) / 8], &retail_73ff10[(0x741e20 - 0x73ff10) / 8], &retail_73ff10[(0x741e28 - 0x73ff10) / 8],
    &retail_73ff10[(0x741e30 - 0x73ff10) / 8], &retail_73ff10[(0x741e38 - 0x73ff10) / 8], &retail_73ff10[(0x741e40 - 0x73ff10) / 8], &retail_73ff10[(0x741e48 - 0x73ff10) / 8],
    &retail_73ff10[(0x741e50 - 0x73ff10) / 8], &retail_73ff10[(0x741e58 - 0x73ff10) / 8], &retail_73ff10[(0x741e60 - 0x73ff10) / 8], &retail_73ff10[(0x741e68 - 0x73ff10) / 8],
    &retail_73ff10[(0x741e70 - 0x73ff10) / 8], &retail_73ff10[(0x741e78 - 0x73ff10) / 8], &retail_73ff10[(0x741e80 - 0x73ff10) / 8], &retail_73ff10[(0x741e88 - 0x73ff10) / 8],
    &retail_73ff10[(0x741e90 - 0x73ff10) / 8], &retail_73ff10[(0x741e98 - 0x73ff10) / 8], &retail_73ff10[(0x741ea0 - 0x73ff10) / 8], &retail_73ff10[(0x741ea8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741eb0 - 0x73ff10) / 8], &retail_73ff10[(0x741eb8 - 0x73ff10) / 8], &retail_73ff10[(0x741ec0 - 0x73ff10) / 8], &retail_73ff10[(0x741ec8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741ed0 - 0x73ff10) / 8], &retail_73ff10[(0x741ed8 - 0x73ff10) / 8], &retail_73ff10[(0x741ee0 - 0x73ff10) / 8], &retail_73ff10[(0x741ee8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741ef0 - 0x73ff10) / 8], &retail_73ff10[(0x741ef8 - 0x73ff10) / 8], &retail_73ff10[(0x741f00 - 0x73ff10) / 8], &retail_73ff10[(0x741f08 - 0x73ff10) / 8],
    &retail_73ff10[(0x741f10 - 0x73ff10) / 8], &retail_73ff10[(0x741f18 - 0x73ff10) / 8], &retail_73ff10[(0x741f20 - 0x73ff10) / 8], &retail_73ff10[(0x741f28 - 0x73ff10) / 8],
    &retail_73ff10[(0x741f30 - 0x73ff10) / 8], &retail_73ff10[(0x741f38 - 0x73ff10) / 8], &retail_73ff10[(0x741f40 - 0x73ff10) / 8], &retail_73ff10[(0x741f48 - 0x73ff10) / 8],
    &retail_73ff10[(0x741f50 - 0x73ff10) / 8], &retail_73ff10[(0x741f58 - 0x73ff10) / 8], &retail_73ff10[(0x741f60 - 0x73ff10) / 8], &retail_73ff10[(0x741f68 - 0x73ff10) / 8],
    &retail_73ff10[(0x741f70 - 0x73ff10) / 8], &retail_73ff10[(0x741f78 - 0x73ff10) / 8], &retail_73ff10[(0x741f80 - 0x73ff10) / 8], &retail_73ff10[(0x741f88 - 0x73ff10) / 8],
    &retail_73ff10[(0x741f90 - 0x73ff10) / 8], &retail_73ff10[(0x741f98 - 0x73ff10) / 8], &retail_73ff10[(0x741fa0 - 0x73ff10) / 8], &retail_73ff10[(0x741fa8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741fb0 - 0x73ff10) / 8], &retail_73ff10[(0x741fb8 - 0x73ff10) / 8], &retail_73ff10[(0x741fc0 - 0x73ff10) / 8], &retail_73ff10[(0x741fc8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741fd0 - 0x73ff10) / 8], &retail_73ff10[(0x741fd8 - 0x73ff10) / 8], &retail_73ff10[(0x741fe0 - 0x73ff10) / 8], &retail_73ff10[(0x741fe8 - 0x73ff10) / 8],
    &retail_73ff10[(0x741ff0 - 0x73ff10) / 8], &retail_73ff10[(0x741ff8 - 0x73ff10) / 8], &retail_73ff10[(0x742000 - 0x73ff10) / 8], &retail_73ff10[(0x742008 - 0x73ff10) / 8],
    &retail_73ff10[(0x742010 - 0x73ff10) / 8], &retail_73ff10[(0x742018 - 0x73ff10) / 8], &retail_73ff10[(0x742020 - 0x73ff10) / 8], &retail_73ff10[(0x742028 - 0x73ff10) / 8],
    &retail_73ff10[(0x742030 - 0x73ff10) / 8], &retail_73ff10[(0x742038 - 0x73ff10) / 8], &retail_73ff10[(0x742040 - 0x73ff10) / 8], &retail_73ff10[(0x742048 - 0x73ff10) / 8],
    &retail_73ff10[(0x742050 - 0x73ff10) / 8], &retail_73ff10[(0x742058 - 0x73ff10) / 8], &retail_73ff10[(0x742060 - 0x73ff10) / 8], &retail_73ff10[(0x742068 - 0x73ff10) / 8],
    &retail_73ff10[(0x742070 - 0x73ff10) / 8], &retail_73ff10[(0x742078 - 0x73ff10) / 8], &retail_73ff10[(0x742080 - 0x73ff10) / 8], &retail_73ff10[(0x742088 - 0x73ff10) / 8],
    &retail_73ff10[(0x742090 - 0x73ff10) / 8], &retail_73ff10[(0x742098 - 0x73ff10) / 8], &retail_73ff10[(0x7420a0 - 0x73ff10) / 8], &retail_73ff10[(0x7420a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7420b0 - 0x73ff10) / 8], &retail_73ff10[(0x7420b8 - 0x73ff10) / 8], &retail_73ff10[(0x7420c0 - 0x73ff10) / 8], &retail_73ff10[(0x7420c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7420d0 - 0x73ff10) / 8], &retail_73ff10[(0x7420d8 - 0x73ff10) / 8], &retail_73ff10[(0x7420e0 - 0x73ff10) / 8], &retail_73ff10[(0x7420e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7420f0 - 0x73ff10) / 8], &retail_73ff10[(0x7420f8 - 0x73ff10) / 8], &retail_73ff10[(0x742100 - 0x73ff10) / 8], &retail_73ff10[(0x742108 - 0x73ff10) / 8],
    &retail_73ff10[(0x742110 - 0x73ff10) / 8], &retail_73ff10[(0x742118 - 0x73ff10) / 8], &retail_73ff10[(0x742120 - 0x73ff10) / 8], &retail_73ff10[(0x742128 - 0x73ff10) / 8],
    &retail_73ff10[(0x742130 - 0x73ff10) / 8], &retail_73ff10[(0x742138 - 0x73ff10) / 8], &retail_73ff10[(0x742140 - 0x73ff10) / 8], &retail_73ff10[(0x742148 - 0x73ff10) / 8],
    &retail_73ff10[(0x742150 - 0x73ff10) / 8], &retail_73ff10[(0x742158 - 0x73ff10) / 8],
    // tmp/probe: the 164 retail entries after the 1098 the source has (GAME-CHANGES F22)
    &retail_73ff10[(0x742160 - 0x73ff10) / 8],
    &retail_73ff10[(0x742168 - 0x73ff10) / 8],
    &retail_73ff10[(0x742170 - 0x73ff10) / 8],
    &retail_73ff10[(0x742178 - 0x73ff10) / 8],
    &retail_73ff10[(0x742180 - 0x73ff10) / 8],
    &retail_73ff10[(0x742188 - 0x73ff10) / 8],
    &retail_73ff10[(0x742190 - 0x73ff10) / 8],
    &retail_73ff10[(0x742198 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421a0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421b0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421b8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421c0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421d0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421d8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421e0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421f0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7421f8 - 0x73ff10) / 8],
    &retail_73ff10[(0x742200 - 0x73ff10) / 8],
    &retail_73ff10[(0x742208 - 0x73ff10) / 8],
    &retail_73ff10[(0x742210 - 0x73ff10) / 8],
    &retail_73ff10[(0x742218 - 0x73ff10) / 8],
    &retail_73ff10[(0x742220 - 0x73ff10) / 8],
    &retail_73ff10[(0x742228 - 0x73ff10) / 8],
    &retail_73ff10[(0x742230 - 0x73ff10) / 8],
    &retail_73ff10[(0x742238 - 0x73ff10) / 8],
    &retail_73ff10[(0x742240 - 0x73ff10) / 8],
    &retail_73ff10[(0x742248 - 0x73ff10) / 8],
    &retail_73ff10[(0x742250 - 0x73ff10) / 8],
    &retail_73ff10[(0x742258 - 0x73ff10) / 8],
    &retail_73ff10[(0x742260 - 0x73ff10) / 8],
    &retail_73ff10[(0x742268 - 0x73ff10) / 8],
    &retail_73ff10[(0x742270 - 0x73ff10) / 8],
    &retail_73ff10[(0x742278 - 0x73ff10) / 8],
    &retail_73ff10[(0x742280 - 0x73ff10) / 8],
    &retail_73ff10[(0x742288 - 0x73ff10) / 8],
    &retail_73ff10[(0x742290 - 0x73ff10) / 8],
    &retail_73ff10[(0x742298 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422a0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422b0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422b8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422c0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422d0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422d8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422e0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422f0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7422f8 - 0x73ff10) / 8],
    &retail_73ff10[(0x742300 - 0x73ff10) / 8],
    &retail_73ff10[(0x742308 - 0x73ff10) / 8],
    &retail_73ff10[(0x742310 - 0x73ff10) / 8],
    &retail_73ff10[(0x742318 - 0x73ff10) / 8],
    &retail_73ff10[(0x742320 - 0x73ff10) / 8],
    &retail_73ff10[(0x742328 - 0x73ff10) / 8],
    &retail_73ff10[(0x742330 - 0x73ff10) / 8],
    &retail_73ff10[(0x742338 - 0x73ff10) / 8],
    &retail_73ff10[(0x742340 - 0x73ff10) / 8],
    &retail_73ff10[(0x742348 - 0x73ff10) / 8],
    &retail_73ff10[(0x742350 - 0x73ff10) / 8],
    &retail_73ff10[(0x742358 - 0x73ff10) / 8],
    &retail_73ff10[(0x742360 - 0x73ff10) / 8],
    &retail_73ff10[(0x742368 - 0x73ff10) / 8],
    &retail_73ff10[(0x742370 - 0x73ff10) / 8],
    &retail_73ff10[(0x742378 - 0x73ff10) / 8],
    &retail_73ff10[(0x742380 - 0x73ff10) / 8],
    &retail_73ff10[(0x742388 - 0x73ff10) / 8],
    &retail_73ff10[(0x742390 - 0x73ff10) / 8],
    &retail_73ff10[(0x742398 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423a0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423b0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423b8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423c0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423d0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423d8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423e0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423f0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7423f8 - 0x73ff10) / 8],
    &retail_73ff10[(0x742400 - 0x73ff10) / 8],
    &retail_73ff10[(0x742408 - 0x73ff10) / 8],
    &retail_73ff10[(0x742410 - 0x73ff10) / 8],
    &retail_73ff10[(0x742418 - 0x73ff10) / 8],
    &retail_73ff10[(0x742420 - 0x73ff10) / 8],
    &retail_73ff10[(0x742428 - 0x73ff10) / 8],
    &retail_73ff10[(0x742430 - 0x73ff10) / 8],
    &retail_73ff10[(0x742438 - 0x73ff10) / 8],
    &retail_73ff10[(0x742440 - 0x73ff10) / 8],
    &retail_73ff10[(0x742448 - 0x73ff10) / 8],
    &retail_73ff10[(0x742450 - 0x73ff10) / 8],
    &retail_73ff10[(0x742458 - 0x73ff10) / 8],
    &retail_73ff10[(0x742460 - 0x73ff10) / 8],
    &retail_73ff10[(0x742468 - 0x73ff10) / 8],
    &retail_73ff10[(0x742470 - 0x73ff10) / 8],
    &retail_73ff10[(0x742478 - 0x73ff10) / 8],
    &retail_73ff10[(0x742480 - 0x73ff10) / 8],
    &retail_73ff10[(0x742488 - 0x73ff10) / 8],
    &retail_73ff10[(0x742490 - 0x73ff10) / 8],
    &retail_73ff10[(0x742498 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424a0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424b0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424b8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424c0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424d0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424d8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424e0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424f0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7424f8 - 0x73ff10) / 8],
    &retail_73ff10[(0x742500 - 0x73ff10) / 8],
    &retail_73ff10[(0x742508 - 0x73ff10) / 8],
    &retail_73ff10[(0x742510 - 0x73ff10) / 8],
    &retail_73ff10[(0x742518 - 0x73ff10) / 8],
    &retail_73ff10[(0x742520 - 0x73ff10) / 8],
    &retail_73ff10[(0x742528 - 0x73ff10) / 8],
    &retail_73ff10[(0x742530 - 0x73ff10) / 8],
    &retail_73ff10[(0x742538 - 0x73ff10) / 8],
    &retail_73ff10[(0x742540 - 0x73ff10) / 8],
    &retail_73ff10[(0x742548 - 0x73ff10) / 8],
    &retail_73ff10[(0x742550 - 0x73ff10) / 8],
    &retail_73ff10[(0x742558 - 0x73ff10) / 8],
    &retail_73ff10[(0x742560 - 0x73ff10) / 8],
    &retail_73ff10[(0x742568 - 0x73ff10) / 8],
    &retail_73ff10[(0x742570 - 0x73ff10) / 8],
    &retail_73ff10[(0x742578 - 0x73ff10) / 8],
    &retail_73ff10[(0x742580 - 0x73ff10) / 8],
    &retail_73ff10[(0x742588 - 0x73ff10) / 8],
    &retail_73ff10[(0x742590 - 0x73ff10) / 8],
    &retail_73ff10[(0x742598 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425a0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425a8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425b0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425b8 - 0x73ff10) / 8],
    // PC port: retail 0x827c5f2c is g_ai_debug_throttled_global (ai.cpp), 548 KB past retail_73ff10's 10,088 B slab.
    (hs_global_external *)&g_ai_debug_throttled_global,
    &retail_73ff10[(0x7425c0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425c8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425d0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425d8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425e0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425e8 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425f0 - 0x73ff10) / 8],
    &retail_73ff10[(0x7425f8 - 0x73ff10) / 8],
    &retail_73ff10[(0x742600 - 0x73ff10) / 8],
    &retail_73ff10[(0x742608 - 0x73ff10) / 8],
    &retail_73ff10[(0x742610 - 0x73ff10) / 8],
    &retail_73ff10[(0x742618 - 0x73ff10) / 8],
    &retail_73ff10[(0x742620 - 0x73ff10) / 8],
    &retail_73ff10[(0x742628 - 0x73ff10) / 8],
    &retail_73ff10[(0x742630 - 0x73ff10) / 8],
    &retail_73ff10[(0x742638 - 0x73ff10) / 8],
    &retail_73ff10[(0x742640 - 0x73ff10) / 8],
    &retail_73ff10[(0x742648 - 0x73ff10) / 8],
    &retail_73ff10[(0x742650 - 0x73ff10) / 8],
    &retail_73ff10[(0x742658 - 0x73ff10) / 8],
    &retail_73ff10[(0x742660 - 0x73ff10) / 8],
    &retail_73ff10[(0x742668 - 0x73ff10) / 8],
    &retail_73ff10[(0x742670 - 0x73ff10) / 8],
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
