#ifndef ITEMS_EQUIPMENT_GLOBALS_H
#define ITEMS_EQUIPMENT_GLOBALS_H
#include "render/camera_fx_globals_types.h"
#include "game/player_control_globals.h"

#include "game/players_global_data.h"

struct s_equipment_slot;
struct chud_scripting_globals_tls;
struct s_players_globals;
struct s_player_control_globals;
struct s_camera_fx_globals;

// Shared globals read by equipment and terminal camera logic.
extern "C" {
extern __declspec(thread) chud_scripting_globals_tls *g_chud_scripting_globals;
}

#endif // ITEMS_EQUIPMENT_GLOBALS_H
