#ifndef AI_AI_TLS_DATA_H
#define AI_AI_TLS_DATA_H
#include "ai/squad_data.h"

struct s_data_array;
struct s_swarm_spawner;

extern "C" {
extern __declspec(thread) s_data_array *g_flocks_data;
extern __declspec(thread) s_data_array *g_swarm_data;
extern __declspec(thread) s_data_array *g_ai_squad_group_data;
extern __declspec(thread) s_swarm_spawner *g_swarm_spawners;
}

#endif
