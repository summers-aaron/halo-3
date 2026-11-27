#ifndef INCLUDE_PHYS_MATH_ALLOCATOR_H
#define INCLUDE_PHYS_MATH_ALLOCATOR_H

#include "phys/phys_math.h"
#include "phys/phys_thread_memory.h"

// hkVector4 array allocator using the math memory class.
// match: 0x689130 (getMeanAndCovariance): the allocator reaches the thread memory through Havok's own getInstance
// body, which names the pointer before dereferencing it; hkThreadMemory::getInstance() here reads the slot directly
// (kept for hkSimulation 0x6520d8), and that shape swaps two stack slots in the caller.
__forceinline void *phys_vec4::operator new[](size_t nbytes) {
    phys_thread_memory *instance = phys_thread_memory::get_instance_ptr();
    return instance->allocate(static_cast<int>(nbytes), PHYS_MEM_MATH);
}
__forceinline void phys_vec4::operator delete[](void *p) {
    phys_thread_memory *instance = phys_thread_memory::get_instance_ptr();
    instance->deallocate(p);
}

#endif
