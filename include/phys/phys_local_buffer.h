#ifndef INCLUDE_PHYS_LOCAL_BUFFER_H
#define INCLUDE_PHYS_LOCAL_BUFFER_H

// hkLocalBuffer: a fixed array over a block of the thread's stack allocator, released when it goes out of scope.

#include "phys/phys_base.h"

// March 2007 (0x83477870, 0x834778c0): the block comes from hkAllocateStack and goes back through
// hkDeallocateStack; the label is for debug builds only.
template <typename T>
class phys_local_buffer : public phys_fixed_array<T> {
public:
    __forceinline phys_local_buffer(int capacity, const char *label = 0) { this->m_data = phys_alloc_stack<T>(capacity); }
    __forceinline ~phys_local_buffer() { phys_free_stack(this->m_data); }
};

#endif // INCLUDE_HAVOK_HK_LOCAL_BUFFER_H
