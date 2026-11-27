#ifndef INCLUDE_PHYS_OBJECT_H
#define INCLUDE_PHYS_OBJECT_H

#include "phys/phys_base_types.h"
#include "phys/phys_fixup_tag.h"
#include "phys/phys_thread_memory.h"

class phys_statistics_collector;
class phys_ref_object;
bool halo_managed_memory_address(const phys_ref_object *object);

class phys_object {
public:
    virtual __forceinline ~phys_object() {}
};

class phys_ref_object : public phys_object {
public:
    __forceinline phys_ref_object() : m_object_ref_count(1) {}
    // March 2007 (0x834e2f98): a copy starts with a reference count of one; the size field is not copied.
    __forceinline phys_ref_object(const phys_ref_object &) : phys_object(), m_object_ref_count(1) {}
    __forceinline phys_ref_object(phys_fixup_tag) {}
    virtual __forceinline ~phys_ref_object() {}
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;

    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_BASE_CLASS);

    __forceinline void add_object_reference() const {
        if (m_mem_size_and_flags != 0) {
            ++m_object_ref_count;
        }
    }

    __forceinline void object_remove_ref() const {
        if (m_mem_size_and_flags != 0) {
            if (--m_object_ref_count == 0) {
                if (halo_managed_memory_address(this)) {
                    m_object_ref_count = 1;
                } else {
                    delete this;
                }
            }
        }
    }

    __forceinline int get_object_ref_count() const { return m_object_ref_count; }
    __forceinline int get_sector_allocated_size() const { return m_mem_size_and_flags & 0x7fff; }

    unsigned short m_mem_size_and_flags;
    mutable short m_object_ref_count;
};

#endif // INCLUDE_HAVOK_HK_BASE_OBJECT_H
