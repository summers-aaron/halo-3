#ifndef SIMULATION_SIMULATION_WORLD_SERVICES_H
#define SIMULATION_SIMULATION_WORLD_SERVICES_H

extern "C++" {
enum e_director_perspective;
struct unit_control_data;

enum e_director_perspective director_get_perspective(long, long);
extern "C" bool player_is_on_local_machine(long player_index);
extern "C" void simulation_game_build_motion_data_from_control(const unit_control_data *control, long unit_index, void *motion);
extern "C" void simulation_queue_game_simulation_insert(int);
extern "C" long map_variant_initialize_for_new_map(void *);
}

#endif // SIMULATION_SIMULATION_WORLD_SERVICES_H
