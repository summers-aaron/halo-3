#ifndef SCENARIO_SCENARIO_SKY_H
#define SCENARIO_SCENARIO_SKY_H

// Functions defined in tu/blamlibxbox/scenario_sky.cpp that other units call.

struct s_game_cluster_bit_vectors;
struct scenario;

// March 82be6d68..82be7028: static methods, scenario\scenario_sky.cpp (the class name is Bungie's spelling).
class c_scenerio_sky_objects_helper
{
public:
    static long create_sky_object(long sky_index);
    static const s_game_cluster_bit_vectors *calculate_sky_object_cluster_visibility(long object_index,
        s_game_cluster_bit_vectors *cluster_visibility);
};

void scenario_sky_objects_initialize(void);
void scenario_sky_objects_dispose(void);
void scenario_sky_objects_initialize_for_new_map(void);
void scenario_sky_objects_dispose_from_old_map(void);
#endif // SCENARIO_SCENARIO_SKY_H
