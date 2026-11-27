#ifndef PHYSICS_HAVOK_MEMORY_ALLOCATOR_LOCK_H
#define PHYSICS_HAVOK_MEMORY_ALLOCATOR_LOCK_H

extern "C" { extern __declspec(thread) bool g_havok_memory_allocator_locked; }

#endif // PHYSICS_HAVOK_MEMORY_ALLOCATOR_LOCK_H
