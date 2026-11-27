#ifndef AI_OBJECTIVES_RUNTIME_H
#define AI_OBJECTIVES_RUNTIME_H
#include "memory/tls_data_arrays.h"
#include "scenario/scenario.h"
extern "C" {
extern __declspec(thread) long *g_havok_memory_task_data;
// Keep the signed return type used by objectives' accepted leader queries.
long squad_get_leader(short squad_index);
}
#define g_tasks_data_tls_address g_havok_memory_task_data
static __forceinline int tls_current_thread_index() { return g_registered_thread_index; }
#endif
