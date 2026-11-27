#ifndef INCLUDE_INTERFACE_USER_INTERFACE_MEMORY_GLOBALS_H
#define INCLUDE_INTERFACE_USER_INTERFACE_MEMORY_GLOBALS_H

struct s_memory_pool;
class c_physical_memory_allocation;

extern c_physical_memory_allocation g_user_interface_memory_allocator;

extern "C" {
extern s_memory_pool *g_user_interface_memory_pool;
extern bool g_user_interface_memory_allocation_failed;
extern const char k_user_interface_memory_pool_name[];
extern unsigned int g_user_interface_memory_pool_size;
s_memory_pool *memory_pool_new(const char *name, int size, unsigned int callbacks, unsigned int *allocator);
}

#endif // INCLUDE_INTERFACE_USER_INTERFACE_MEMORY_GLOBALS_H
