#ifndef INTERFACE_CHUD_CHUD_NAVPOINTS_RUNTIME_H
#define INTERFACE_CHUD_CHUD_NAVPOINTS_RUNTIME_H
#include "interface/chud/chud_scripting_globals_types.h"
#include "game/game_engine_area_types.h"
#include "items/equipment_globals.h"
// The waypoint view overlaps the scripting view's final consumables byte.
struct chud_navpoints_runtime
{
    s_equipment_slot equipment_slots[8];
    long user_index;
    chud_script_flags scripting;
    bool show_compass;
    bool show_stamina;
    bool show_energy_meters;
    union
    {
        s_game_engine_player_waypoint waypoints[16];
        s_game_engine_fade_entry player_waypoints[16];
    };
};
typedef char chud_waypoints_offset[(offsetof(chud_navpoints_runtime, waypoints) == 0x150) ? 1 : -1];
// Same TLS slot as g_chud_scripting_globals (0x11df75c), viewed as navpoint waypoints.
static __forceinline chud_navpoints_runtime *chud_navpoint_globals(void) { return (chud_navpoints_runtime *)g_chud_scripting_globals; }
#endif
