#ifndef INCLUDE_PHYS_REL_PTR32_H
#define INCLUDE_PHYS_REL_PTR32_H
#include "phys/phys_base.h"
template <typename T>
class phys_rel_ptr32 {
public:
    // match: 0x662950: the shipped image keeps this destructor implicit; March's empty user destructor emits an extra deleting-destructor call.
    __forceinline phys_rel_ptr32() {}
    __forceinline void set(T *pointer) { m_rel_offset = phys_get_byte_offset_int(this, pointer); }
    __forceinline T *val() const { return (T *)phys_add_byte_offset_const(this, m_rel_offset); }
    __forceinline operator T *() const { return val(); }
    int m_rel_offset;
};
#endif
