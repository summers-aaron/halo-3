#ifndef INTERFACE_USER_INTERFACE_EXTERNS_H
#define INTERFACE_USER_INTERFACE_EXTERNS_H

// External dependencies used by user_interface.
#include "interface/gui_pregame_setup_manager_types.h"
class c_gui_error_manager;

extern "C" unsigned char game_is_scoreboard_blocked(int *);
extern "C" void gui_error_manager_update(c_gui_error_manager *, unsigned long);

#include "interface/user_interface_data.h"
#include "interface/user_interface_text_parser.h"
#include "interface/gui_custom_bitmap_storage.h"
#include "interface/user_interface_dialog_globals.h"
#include "interface/view_config_types.h"
#include "sound/game_sound_types.h"
#include "networking/online/online_globals_types.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"

class c_network_session;
struct s_font_cache_globals;
struct s_memory_pool;
struct s_rasterizer_render_globals;
struct s_node;



extern "C" {

#include "memory/shared_resource_allocator.h"
extern void *XNotifyCreateListener(unsigned __int64 areas);
extern void XCustomRegisterDynamicActions(void);
extern s_memory_pool *g_user_interface_memory_pool;
void XNotifyPositionUI(long value);
unsigned long XFlushUtilityDrive(void);
void XLaunchNewImage(const char *, const void *);

void XCustomUnregisterDynamicActions(void);
}
#include "text/font_cache_globals_types.h"

#endif // INTERFACE_USER_INTERFACE_EXTERNS_H
