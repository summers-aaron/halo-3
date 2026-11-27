#ifndef GAME_GAME_ENGINE_AREA_INITIALIZATION_H
#define GAME_GAME_ENGINE_AREA_INITIALIZATION_H

class c_area;
template<class t_area, long k_area_count> class c_area_set;

// Constructs the two runs of nine team area sets that start at area_sets.
extern "C" c_area_set<c_area, 3> *game_engine_initialize_team_area_sets(c_area_set<c_area, 3> *area_sets);

#endif
