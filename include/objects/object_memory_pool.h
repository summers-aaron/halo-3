#ifndef OBJECTS_OBJECT_MEMORY_POOL_H
#define OBJECTS_OBJECT_MEMORY_POOL_H

struct s_memory_pool;

// Object datum storage; object handles are byte offsets into this pool.
extern "C" {
extern __declspec(thread) s_memory_pool *g_object_memory_pool;
}

#endif // OBJECTS_OBJECT_MEMORY_POOL_H
