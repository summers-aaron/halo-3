#ifndef SIMULATION_SIMULATION_SERVICES_H
#define SIMULATION_SIMULATION_SERVICES_H

extern "C++" {
struct s_saved_film_update;
struct s_saved_film_snippet_marker;
struct s_world_update_queue;
struct s_saved_film_snippet_camera;

extern "C" void game_results_grant_top_player_achievement(char *game_results);
extern "C" void simulation_add_view_to_world(int p0, const unsigned char *p1, long p2, const char *name);
extern "C" bool saved_film_try_and_get_update_header(long *update);
extern "C" int saved_film_get_marker_count(const s_saved_film_snippet_marker *entries);
extern "C" s_saved_film_snippet_marker *saved_film_new_work_entry(s_saved_film_snippet_marker *entries, bool flag);
extern "C" bool saved_film_is_update_pending(void);
extern "C" void update_queue_delete_ready_head(s_world_update_queue *world);
extern "C" void saved_film_snippet_set_cameras_from_states(s_saved_film_snippet_camera *cameras);
}

#endif // SIMULATION_SIMULATION_SERVICES_H
