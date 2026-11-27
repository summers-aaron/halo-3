#include "game/players_global_data.h"
#ifndef CAMERA_CINEMATICS_DEPENDENCIES_H
#define CAMERA_CINEMATICS_DEPENDENCIES_H

#include "objects/cinematic_light_globals.h"
#include "cutscene/cinematic_new_globals_types.h"
#include "interface/chud/chud_manager.h"

#include "math/real_math_types.h"
#include "camera/cinematic_lightmap_shadow.h"
struct s_cinematics_wchar_string;
extern "C" bool string_list_get_normal_string_by_language(long tag_index, long string_id, long language, s_cinematics_wchar_string *string);
class c_player_profile_interface;
extern "C" int global_preferences_get_subtitle_setting(c_player_profile_interface *profile);
extern "C" void interface_scale_bounds(rectangle2d *bounds, const real_point2d *scale);

#include "game/game_time_types.h"
struct s_cinematic_new_globals_definition;
#include "objects/object_runtime_globals_types.h"
#include "game/players_globals_types.h"
#include "ai/actor_globals.h"
struct s_object_marker_globals;
#include "main/main_flag_state.h"
extern "C" {
extern s_object_marker_globals g_object_marker_globals[];

unsigned long XTLGetLanguage(void);
#include "cseries/xenon_platform_api.h"
}

#endif // CAMERA_CINEMATICS_DEPENDENCIES_H
