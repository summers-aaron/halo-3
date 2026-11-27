#ifndef GAME_GAME_LSP_STATISTICS_EXTERNS_H
#define GAME_GAME_LSP_STATISTICS_EXTERNS_H

#include "cseries/async_xoverlapped.h"
#include "memory/tls_globals.h"

struct display_statistics;
struct game_options;
struct s_campaing_metagame_globals;
struct s_campaign_metagame_player_globals;
struct s_blffile_webstats_multiplayer_game_result;
class c_game_results;
class c_game_variant;

extern "C" void game_lsp_statistics_get_display_info(display_statistics *out);
extern "C" unsigned __int64 game_variant_compute_unique_id(const c_game_variant *variant);
#include "game/map_variant_files.h"
extern "C" void game_lsp_statistics_copy_game_info(s_blffile_webstats_multiplayer_game_result *webstats,
    const c_game_results *game_results);

extern "C" {

bool game_lsp_submit_campaign_result(game_options *options, s_campaing_metagame_globals *metagame_globals,
    s_campaign_metagame_player_globals *player_globals);
}

#endif // GAME_GAME_LSP_STATISTICS_EXTERNS_H
