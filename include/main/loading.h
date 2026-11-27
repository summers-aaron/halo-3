#ifndef MAIN_LOADING_H
#define MAIN_LOADING_H

// Functions defined in tu/blamlibxbox/loading.cpp that other units call.

extern "C" void loading_get_campaign_map(int *campaign_id, int *map_id);
extern "C" long loading_load_map_with_progress(short progress,const char *path,long type);
extern "C" long loading_get_map_load_state(short map_index, const char *path);
extern "C" char *get_next_scenario_path(char *path,long path_size);
extern "C" void loading_update_progress(unsigned int units);

struct s_next_scenario_path
{
    char path[256];
    long type;
    bool has_path;
    bool matchmaking;
};

extern "C" bool loading_get_next_scenario_path(s_next_scenario_path *result);

struct loading_map_path_cache
{
    long campaign;
    long map;
    char path[260];
};


extern "C" loading_map_path_cache g_loading_map_path_cache;

#endif // MAIN_LOADING_H
