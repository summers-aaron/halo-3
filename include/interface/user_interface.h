#ifndef INTERFACE_USER_INTERFACE_H
#define INTERFACE_USER_INTERFACE_H

// Functions defined in tu/interfacelibxbox/user_interface.cpp.

extern bool g_user_interface_is_alpha;
extern bool g_eula_accepted;

class c_gui_data;
struct s_user_interface_shared_globals;
#include "tag_files/tag_types.h"
#include <stddef.h>

struct s_user_interface_tag_globals
{
    s_tag_reference shared_globals;
    s_tag_reference multiplayer_variant_settings;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    c_typed_tag_block<s_tag_reference> screen_widgets; // March 836761d4/836761ec: count at +0x30, references at +0x34.
};

typedef char user_interface_tag_globals_screens_offset[(offsetof(s_user_interface_tag_globals, screen_widgets) == 0x30) ? 1 : -1];

void user_interface_update(float elapsed_seconds);
const s_user_interface_tag_globals *user_interface_tag_globals_try_and_get(void);
c_gui_data *interface_get_global_datasource(long identifier);
bool get_alpha_is_locked_down(void);
bool get_alpha_is_internal_beta(void);

// Shared by tu/interfacelibxbox/user_interface.cpp and tu/networkinglibxbox/network_session.cpp.
static __forceinline bool get_alpha_is_internal()
{
    bool internal = false;
    if (!g_user_interface_is_alpha || g_eula_accepted)
        internal = true;
    return internal;
}
struct s_user_interface_shared_globals const * user_interface_shared_tag_globals_try_and_get(void);

union rectangle2d;

extern "C" void user_interface_set_player_count_mode(void);
extern "C" void user_interface_get_projection_plane_distances(float *first, float *second, float *third);
extern "C" bool get_alpha_is_external_beta(void);
extern "C" bool get_alpha_custom_games_disabled(void);
extern "C" void user_interface_render_windows(long user_index, long controller_index, long render_flags,
    const rectangle2d *window_bounds, long render_target, bool is_screenshot);
extern "C" bool user_interface_is_idle_timeout_expired(void);
struct s_gui_custom_bitmap_storage_globals;
extern "C" void user_interface_dispose_custom_bitmaps(s_gui_custom_bitmap_storage_globals *storage);
extern "C" void user_interface_get_number_of_render_windows(long user, long *horizontal, long *vertical);

void user_interface_initialize(void);
void user_interface_dispose(void);
void user_interface_initialize_for_new_map(void);
extern "C" void user_interface_dispose_from_old_map(void);

#endif // INTERFACE_USER_INTERFACE_H
