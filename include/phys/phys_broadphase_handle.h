#ifndef INCLUDE_PHYS_BROADPHASE_HANDLE_H
#define INCLUDE_PHYS_BROADPHASE_HANDLE_H

#include "phys/phys_base.h"

class phys_broadphase_handle {
public:
    // March 0x82ccc7b0 initializes the id; the finish constructor preserves loaded storage.
    __forceinline phys_broadphase_handle() : m_handle_id(0) {}
    __forceinline phys_broadphase_handle(phys_fixup_tag) {}
    __forceinline void set_handle_id(long id) { m_handle_id = id; }
    unsigned int m_handle_id;
};

#endif
