#ifndef MAIN_MAIN_FLAG_STATE_H
#define MAIN_MAIN_FLAG_STATE_H

#include <stddef.h>
#include <string.h>

// Shared runtime request state, preserved from main.obj.
struct s_main_flag_state
{
    s_main_flag_state()
    {
        startup_masking_pending = 0;
        memset(&startup_masking_done, 0,
            offsetof(s_main_flag_state, core_name) - offsetof(s_main_flag_state, startup_masking_done));
        request7 = 0;
        memset(&save_requested, 0, sizeof(*this) - offsetof(s_main_flag_state, save_requested));
    }
    unsigned char startup_masking_pending;
    unsigned char startup_masking_done;
    signed char main_menu_disabled;
    unsigned char request3;
    unsigned char loop_exit_requested;
    union { unsigned char request5; bool save_core_requested; };
    union { unsigned char request6; bool load_core_requested; };
    union {
        unsigned char request7;
        char core_name[64];
    };
    union { unsigned char save_requested; bool reset_map; };
    bool reset_map_parameter;
    union { unsigned char revert_requested; bool revert_map; };
    unsigned char reset_requested;
    unsigned char shutdown_requested;
    unsigned char map_change_requested;
    unsigned char active_flag;
    unsigned char secondary_flag;
    unsigned char game_change_requested;
    unsigned char zone_change_requested;
    bool campaign_progress_saved;
    union { unsigned char dirty; bool zone_activation_pending; };
    unsigned char pending_action;
    union { unsigned char play_requested; bool gamestate_ready; };
    unsigned char stop_requested;
    unsigned char sync_requested;
    unsigned char flush_requested;
    unsigned char request58;
    int active_index;
    int secondary_index;
    union { unsigned int deactivate_bits; unsigned long designer_zones_to_deactivate; };
    union { unsigned int activate_bits; unsigned long designer_zones_to_activate; };
    union { unsigned int enable_bits; unsigned long cinematic_zones_to_deactivate; };
    union { unsigned int disable_bits; unsigned long cinematic_zones_to_activate; };
    unsigned int reserved_74;
    unsigned int recursion_count;
    unsigned int last_idle_tick;
};

typedef char main_flag_menu_offset[(offsetof(s_main_flag_state, main_menu_disabled) == 2) ? 1 : -1];
typedef char main_flag_campaign_progress_offset[(offsetof(s_main_flag_state, campaign_progress_saved) == 0x51) ? 1 : -1];
typedef char main_flag_size[(sizeof(s_main_flag_state) == 0x80) ? 1 : -1];

typedef char main_flag_reset_offset[(offsetof(s_main_flag_state, reset_requested) == 0x4a) ? 1 : -1];

typedef char main_flag_core_name_offset[(offsetof(s_main_flag_state, core_name) == 7) ? 1 : -1];
typedef char main_flag_core_request_offset[(offsetof(s_main_flag_state, request7) == 7) ? 1 : -1];
typedef char main_flag_reset_parameter_offset[(offsetof(s_main_flag_state, reset_map_parameter) == 0x48) ? 1 : -1];
typedef char main_flag_active_index_offset[(offsetof(s_main_flag_state, active_index) == 0x5c) ? 1 : -1];
typedef char main_flag_designer_zones_offset[(offsetof(s_main_flag_state, designer_zones_to_deactivate) == 0x64) ? 1 : -1];

extern s_main_flag_state main_globals;

#endif
