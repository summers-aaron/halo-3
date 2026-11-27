#ifndef MEMORY_TLS_DATA_ARRAYS_H
#define MEMORY_TLS_DATA_ARRAYS_H
#include "ai/joint_data.h"
#include "ai/squad_data.h"

#include "ai/ai_tls_data.h"
#include "objects/object_list_data.h"
#include "objects/object_render_state_tls.h"

struct s_data_array;

#include "cache/restricted_address.h"
#include "ai/actor_data.h"
#include "game/player_data.h"
extern "C" {
#include "ai/prop_ref_data.h"
}
#include "multithreading/registered_thread_index.h"
extern "C" {
#include "ai/prop_data.h"
#include "ai/clump_data.h"
}
#include "objects/object_header_data.h"
extern __declspec(thread) s_data_array *tracking_data;
extern "C" {
extern __declspec(thread) s_data_array *g_impact_data;
extern __declspec(thread) s_data_array *g_simulation_gamestate_entity_data;           // simulation gamestate entity data
extern __declspec(thread) s_data_array *g_impact_array_data;
}

// Joint behavior datum storage; preserve the canonical TLS linker name.
static __forceinline s_data_array *&joint_state_data() { return g_joint_data; }

#endif
