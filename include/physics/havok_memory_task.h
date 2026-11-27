#ifndef PHYSICS_HAVOK_MEMORY_TASK_H
#define PHYSICS_HAVOK_MEMORY_TASK_H
extern "C" { extern __declspec(thread) long *g_havok_memory_task_data; }
// Two task timestamps: structure BSP initialization and garbage collection.
static __forceinline long *havok_memory_task_times() { return g_havok_memory_task_data; }
extern "C" unsigned char havok_cleanup_large_simulation_islands(int, unsigned char, int, int);
#endif
