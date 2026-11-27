#ifndef GAME_GAME_RESULTS_FINALIZED_H
#define GAME_GAME_RESULTS_FINALIZED_H
#include "game/game_results_storage_types.h"
#include "game/game_results.h"
__forceinline bool game_results_get_game_finalized() {
    return g_current_game_results.initialized && g_current_game_results.finalized;
}
__forceinline const c_game_results *game_results_get_final_results() {
    const c_game_results *results = 0;
    if (game_results_get_game_finalized()) results = &g_current_game_results;
    return results;
}
#endif
