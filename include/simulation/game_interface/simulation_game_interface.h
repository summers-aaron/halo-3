#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERFACE_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERFACE_H

// Functions defined in tu/networkinglibxbox/simulation_game_interface.cpp.

struct s_aim_assist_targeting_result;
struct s_player_predicted_aim_assist;
struct s_player_prediction;

long simulation_calculate_transmission_period(float relevance, float maximum_relevance, float minimum_relevance, long minimum_period, long maximum_period);
void simulation_game_build_predicted_aim_assist(struct s_aim_assist_targeting_result const * _sym_p0, struct s_player_predicted_aim_assist * _sym_p1);
void simulation_game_reconstruct_predicted_aim_assist(struct s_player_predicted_aim_assist const * _sym_p0, struct s_aim_assist_targeting_result * _sym_p1);
bool simulation_game_build_player_prediction(long player_index, struct s_player_prediction * prediction);
void simulation_game_process_player_prediction(long player_index, const s_player_prediction *prediction);

struct s_player_motion;
struct unit_control_data;

extern "C" void simulation_game_reconstruct_simulated_control(const s_player_motion *motion, unit_control_data *control);

class c_simulation_type_collection;

void simulation_game_build_predicted_weapon(long unit_index, const s_aim_assist_targeting_result *aim_assist_targeting, s_player_motion *player_motion);
extern "C" void get_ultimate_parent_weapon_control_weapon_indicies(unsigned int controlled_unit_index, int *control_weapon_indices);
void simulation_game_register_types(c_simulation_type_collection *simulation_type_collection,long *entity_count,long *event_count);

struct player_action;

extern "C" void simulation_game_reconstruct_simulated_action(const s_player_motion *motion,player_action *action);
extern "C" void simulation_game_build_motion_data_from_action(const player_action *action, long unit_index, s_player_motion *motion);

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERFACE_H
