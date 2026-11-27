#ifndef PHYSICS_HAVOK_MEMORY_H
#define PHYSICS_HAVOK_MEMORY_H

// Functions defined in tu/blamlibxbox/havok_memory.cpp.

struct s_data_array;
struct s_havok_memory_usage;

bool havok_override_thread_memory_allocations(void);
int havok_get_available_memory(void);
void havok_memory_initialize(void);
void havok_memory_update_initialize(void);
void havok_memory_garbage_collect(void);
void havok_memory_garbage_collect_reset_allocated_state(void);
void havok_compute_memory_in_use(s_havok_memory_usage *usage);
class c_entity_listener;
class c_world_entity_listener;
void havok_components_initialize_for_new_map_ltcg_inline(s_data_array *components, c_entity_listener *entity_listener,
    c_world_entity_listener *world_entity_listener);

class c_runtime_pool;

extern "C" void havok_memory_dispose_overflow_pool(void);
extern "C" bool havok_memory_is_runtime_pool_full(const c_runtime_pool *runtime_pool);

class phys_memory;

extern "C" int havok_get_memory_condition_type(int);
float havok_memory_runtime_pool_memory_fraction(phys_memory *memory);

// Data defined in tu/blamlibxbox/havok_memory.cpp that other units use.

class c_havok_memory;

extern c_havok_memory g_havok_memory;

struct s_havok_memory_globals;

extern "C" s_havok_memory_globals g_havok_memory_globals;

extern bool g_havok_memory_always_system;
#endif // PHYSICS_HAVOK_MEMORY_H
