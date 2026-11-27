#ifndef MAIN_MAIN_GLOBAL_DECLARATIONS_H
#define MAIN_MAIN_GLOBAL_DECLARATIONS_H

extern "C" unsigned char g_main_halt_requested;
extern "C" bool g_main_crash_recursion_lock;
extern "C" bool g_shell_application_paused;

extern "C" bool g_main_halt_recursion_lock;
extern "C" bool g_webstats_enabled;

// Crash-report status text and main-loop switches. Address tags live in main.cpp.
extern "C" bool g_main_resource_demand_update_enabled;
extern "C" bool g_catch_exceptions;

// Main-loop and crash-report switches; storage addresses are tagged in main.cpp.
extern "C" char g_main_save_map_enabled;
extern "C" char g_main_startup_movie_path[8];
extern "C" bool disable_main_loop_throttle;
extern "C" bool g_force_upload_even_if_untracked;
extern "C" bool g_fake_minidump_creation;
extern "C" bool g_suppress_keyboard_for_minidump;
extern "C" bool g_set_never_a_debugger_present;
extern "C" unsigned char g_main_halt_on_state_change;
extern "C" bool g_main_crash_reporting_enabled;
extern "C" bool g_main_halt_test_options;
extern "C" bool g_main_halt_exit_spin_disabled;
extern "C" bool g_main_resource_demands_invalidated;
extern "C" unsigned long g_main_last_ethernet_link_status;
extern "C" char g_main_minidump_copy_buffer[1024];

#include "interface/gui_screens/campaign/campaign_select_difficulty_message.h"
#endif // MAIN_MAIN_GLOBAL_DECLARATIONS_H
