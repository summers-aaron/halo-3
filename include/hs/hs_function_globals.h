#include "hs/hs_runtime_data.h"
#include "render/depth_of_field_globals.h"
#include "game/players_global_data.h"
#include "scenario/scenario_kill_trigger_volumes_state_types.h"
#include "sound/sound_class_tls.h"
#include "objects/light_globals.h"
#include "game/player_control_deterministic_globals.h"
#include "scenario/scenario_interpolator_globals.h"
#include "game/campaign_metagame_globals.h"
#ifndef HS_HS_FUNCTION_GLOBALS_H
#define HS_HS_FUNCTION_GLOBALS_H

#include "objects/cinematic_light_globals.h"
#include "render/camera_fx_globals_types.h"
#include "cutscene/cinematic_new_globals_types.h"
#include "game/player_control_globals.h"
#include "game/player_training_tls.h"
#include "objects/object_runtime_globals_types.h"
#include "objects/object_scripting_tls.h"

#include "objects/object_memory_pool.h"

#include "objects/object_name_list.h"

// Engine TLS services used by the scripting evaluators.
#include "physics/breakable_surface_globals.h"
#include "game/game_save_globals.h"
#include "sound/game_sound_globals.h"
#include "interface/chud/chud_dispose_state_types.h"
#include "camera/camera_script_globals_types.h"
struct s_memory_pool;

struct s_data_array;
struct s_camera_script_globals;
struct s_cinematic_new_globals_definition;
struct s_player_training_entry_data;
struct s_depth_of_field;
struct s_object_globals;
struct s_camera_fx_globals;
struct s_campaing_metagame_globals;
struct chud_dispose_state;
struct s_cinematic_light_globals;

extern "C" {
#include "ai/command_script_tls.h"

}

#endif // HS_HS_FUNCTION_GLOBALS_H
