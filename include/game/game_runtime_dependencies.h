#ifndef GAME_GAME_RUNTIME_DEPENDENCIES_H
#define GAME_GAME_RUNTIME_DEPENDENCIES_H

struct s_cluster_reference;
struct game_options;
struct s_campaing_metagame_globals;
struct s_campaign_metagame_player_globals;
struct s_render_postprocess_color_tweaking;

extern "C" bool network_life_cycle_in_session(void **session);
extern "C" const char *k_game_simulation_names[];      /* k_game_simulation_names */
extern "C" const char *k_game_playback_names[];      /* k_game_playback_names */
extern "C" void hs_wake(long thread_index, long waking_thread_index);
extern "C++" s_cluster_reference scripted_camera_get_deterministic_cluster_reference(void);
extern "C" bool game_lsp_submit_campaign_result(game_options *options, s_campaing_metagame_globals *metagame, s_campaign_metagame_player_globals *slots);
extern "C" s_render_postprocess_color_tweaking g_render_postprocess_color_tweaking;

#endif // GAME_GAME_RUNTIME_DEPENDENCIES_H
