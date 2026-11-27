#ifndef INTERFACE_USER_INTERFACE_CLOCK_TYPES_H
#define INTERFACE_USER_INTERFACE_CLOCK_TYPES_H

#include <stddef.h>

// The shared UI state exposes its current millisecond clock at byte +8.
// c_gui_widget and start_menu_game_editor read this field directly.
struct s_ui_clock
{
    long flags;
    // Controller index used for coop game settings.
    long coop_game_settings_controller;
    unsigned long milliseconds;
    long game_shell_time;
    long reserved_10;
    long scoreboard_blocked_until_tick; // Tick until which the scoreboard stays blocked; INT_MAX when inactive
    long campaign_id;
    long map_id;
};

typedef char ui_clock_milliseconds_offset[(offsetof(s_ui_clock, milliseconds) == 8) ? 1 : -1];

typedef char ui_clock_coop_controller_offset[(offsetof(s_ui_clock, coop_game_settings_controller) == 4) ? 1 : -1];

typedef char ui_clock_campaign_offset[(offsetof(s_ui_clock, campaign_id) == 0x18) ? 1 : -1];
typedef char ui_clock_map_offset[(offsetof(s_ui_clock, map_id) == 0x1c) ? 1 : -1];

typedef char ui_clock_scoreboard_tick_offset[(offsetof(s_ui_clock, scoreboard_blocked_until_tick) == 0x14) ? 1 : -1];

extern "C" s_ui_clock g_user_interface_clock;

#endif // INTERFACE_USER_INTERFACE_CLOCK_TYPES_H
