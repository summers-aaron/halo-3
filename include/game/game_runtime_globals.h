#ifndef GAME_GAME_RUNTIME_GLOBALS_H
#define GAME_GAME_RUNTIME_GLOBALS_H
#include "cutscene/cinematic_new_globals_types.h"
#include "animations/animation_manager_storage.h"
#include "math/real_math_constants.h"

extern "C" bool g_shell_floating_point_exceptions_enabled;
extern "C" const unsigned char k_zero_identifier[];

#include "memory/tls_globals.h"
#include "main/main_flag_state.h"

struct s_cinematic_new_globals_definition;

static __forceinline s_main_flag_state &game_main_requests() { return main_globals; }
static __forceinline s_cinematic_new_globals_definition *game_cinematic_objects() { return cinematic_new_globals; }
static __forceinline const wchar_t *game_empty_player_name() { return L""; }
#define TLS_OBSERVER_GLOBALS g_observer_globals

static __forceinline const float *game_scoreboard_real_constants() { return k_real_half_pool; }
static __forceinline const float *game_holiday_real_constants() { return k_real_constant_pool; }

#endif // GAME_GAME_RUNTIME_GLOBALS_H
