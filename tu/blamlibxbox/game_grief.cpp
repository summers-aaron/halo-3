// OBJ: from the records 0x222f78 0x222fa0 0x222fc8 0x223098
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "game/game_globals_storage.h"
#include <stdlib.h>
#include <string.h>
#include "game/game.h"
#include "main/game_preferences.h"
#include "networking/network_configuration.h"
#include "cseries/crt_xdk.h"
#include "memory/tls_globals.h"
#include "game/player_mapping_types.h"
#include "interface/user_interface_controller.h"
#include "networking/network_configuration_types.h"

#include "game/game_grief.h"
#include "game/game_grief_types.h"
#include "interface/user_interface_controller_globals.h"
#include "game/game_variant_inline.h"
#include "memory/datum_index.h"
// @address 0x7e86cc g_game_grief_globals
game_grief_timers g_game_grief_globals;

// @address 0x80608 GetTickCount
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0x119d60 global_preferences_get_ejection_count
// @address 0x119cd8 ?global_preferences_get_betrayal_count@@YAJXZ
// @address 0x19f6e8 global_preferences_set_betrayal_count
// @address 0x46fc30 ?user_interface_controller_update_network_properties@@YAXW4e_controller_index@@@Z

static inline game_options *game_grief_options_get()
{
    return &tls_game_globals()->options;
}

static inline bool game_grief_betrayal_booting_enabled(const c_game_variant *variant)
{
    return _rotl(variant->social_options_flags, 31) & 1;
}

// @address 0x222f78 ?game_grief_initialize@@YAXXZ
void game_grief_initialize(void)
{
    memset(&g_game_grief_globals, 0, offsetof(game_grief_timers, reprieve_time));
    g_game_grief_globals.reprieve_time = 0;
    memset(g_game_grief_globals.player_betrayed, 0, sizeof(g_game_grief_globals.player_betrayed));
}

// @address 0x5bec98 ?game_grief_dispose@@YAXXZ
void game_grief_dispose(void)
{
}

// @address 0x222fa0 ?game_grief_reset_betrayals@@YAXXZ
void game_grief_reset_betrayals(void)
{
    g_game_grief_globals.reprieve_time = 0;
    for (long player_index = 0; player_index < 4; player_index++) {
        g_game_grief_globals.player_betrayed[player_index] = false;
    }
}

// @address 0x222fc8 ?game_grief_record_betrayal@@YAXJ@Z
void game_grief_record_betrayal(long player_index)
{
    if (!game_is_cooperative()) {
        long index = tls_player_mapping_globals()->player_input_controller_mapping[datum_index_to_absolute_index(player_index)];
        long count = global_preferences_get_betrayal_count();
        g_game_grief_globals.betrayal_decay_time = GetTickCount();
        count++;
        if ((float)count >= g_network_configuration.betrayal_threshold && index != -1) {
            g_user_interface_controller_globals.controllers[index].griefer = true;
            user_interface_controller_update_network_properties((e_controller_index)index);
        }
        global_preferences_set_betrayal_count(count);
    }
}

// @address 0x223098 game_grief_can_boot_betrayer
extern "C" bool game_grief_can_boot_betrayer(long)
{
    bool result = false;
    if (!game_is_cooperative()) {
        bool betrayal_booting_enabled = true;
        if (current_game_variant()) {
            game_options *current_options = game_grief_options_get();
            long mode = current_options->game_mode;
            c_game_variant *variant = &current_options->game_variant;
            if (mode != 2) variant = 0;
            betrayal_booting_enabled = game_grief_betrayal_booting_enabled(variant);
        }
        if (betrayal_booting_enabled &&
            global_preferences_get_ejection_count() <= g_network_configuration.ejection_cutoff &&
            g_game_grief_globals.reprieve_time <= 0)
        {
            result = true;
        }
    }
    return result;
}
