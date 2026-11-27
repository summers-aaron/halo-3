#ifndef PHYSICS_PROPERTY_SET_TYPES_H
#define PHYSICS_PROPERTY_SET_TYPES_H

#include "phys/phys_scalar_types.h"

struct phys_property_value {
    unsigned __int64 m_data;
    __forceinline phys_property_value() {}
    __forceinline phys_property_value(int i) { m_data = static_cast<unsigned __int64>(i); }
    __forceinline phys_property_value(void *p) { m_data = reinterpret_cast<phys_ulong>(p); }
    __forceinline phys_property_value(unsigned __int64 d) : m_data(d) {}
    __forceinline int getInt() const { return static_cast<int>(m_data); }
    __forceinline void *getPtr() const { return reinterpret_cast<void *>(static_cast<phys_ulong>(m_data)); }
};

class phys_property {
public:
    unsigned int m_key;
    unsigned int m_alignmentPadding;
    phys_property_value m_value;
};

#endif // PHYSICS_PROPERTY_SET_TYPES_H
