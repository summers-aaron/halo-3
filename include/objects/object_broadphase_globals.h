#include "physics/havok_memory_allocator_lock.h"
#include "physics/havok_monitor_storage.h"
#ifndef OBJECTS_OBJECT_BROADPHASE_GLOBALS_H
#define OBJECTS_OBJECT_BROADPHASE_GLOBALS_H

struct s_object_broadphase;
extern "C" { extern __declspec(thread) s_object_broadphase *g_object_broadphase; }
void __tls_set_g_object_broadphase(s_object_broadphase *broadphase);

#endif // OBJECTS_OBJECT_BROADPHASE_GLOBALS_H
