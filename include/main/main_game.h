#ifndef MAIN_MAIN_GAME_H
#define MAIN_MAIN_GAME_H

#include <stddef.h>
#include "game/game_options.h"

// Functions defined in tu/blamlibxbox/main_game.cpp.

bool main_game_goto_next_level(void);
void main_menu_launch(void);
void main_game_unload_and_prepare_for_next_game(const game_options *options);
void main_game_load_panic();
void main_game_reset_map(bool reset_random_seed);
bool main_game_change_immediate(const game_options *options);
extern "C" void main_menu_update_launch_options(void);
extern "C" void main_game_dispose_from_old_structure_bsp(int flags);
extern "C" void main_game_set_pending_options(const game_options *options);
void main_menu_build_game_options(game_options *options);
void data_mine_insert_single_player_game_options(char const * value);
extern "C" void main_game_initialize_loading_progress(const char *scenario_path);
extern "C" unsigned char main_game_load_scenario(game_options *options);

// The main game globals (main_game.obj's main_game_globals, dynamic initializer 0x707ac8).
struct s_main_game_globals
{
    ~s_main_game_globals() {}
    long state;
    long status;
    char scenario_path[260];
    bool reset_in_progress;                       // March main_game_reset_in_progress() reads +268
    bool game_won;
    bool game_change_in_progress;
    bool game_unload_pending;
    unsigned long game_change_time;
    long game_change_timer;                       // zeroed with the change flags when a change completes (March main_game_change_update)
    game_options options;
};

// @address 0x95e608 main_game_globals
extern "C" s_main_game_globals main_game_globals;

__forceinline bool main_game_reset_in_progress() { return main_game_globals.reset_in_progress; }

typedef char main_game_globals_offset_game_won[(offsetof(s_main_game_globals, game_won) == 0x10d) ? 1 : -1];
typedef char main_game_globals_offset_reset_in_progress[(offsetof(s_main_game_globals, reset_in_progress) == 0x10c) ? 1 : -1];
typedef char main_game_globals_offset_options[(offsetof(s_main_game_globals, options) == 0x118) ? 1 : -1];
typedef char main_game_globals_offset_game_unload_pending[(offsetof(s_main_game_globals, game_unload_pending) == 0x10f) ? 1 : -1];
typedef char main_game_globals_size_check[(sizeof(s_main_game_globals) == 0xf928) ? 1 : -1];


// Shared declarations used by main_game.cpp.
enum e_map_memory_configuration;

struct main_game_callback { void (*dispose)(void); void (*initialize)(e_map_memory_configuration configuration); };

extern "C" const main_game_callback k_main_game_map_memory_callbacks[8];
extern "C" char g_main_game_load_panic_in_progress;
extern "C" long g_main_game_previous_game_mode;

#endif // MAIN_MAIN_GAME_H
