#ifndef INCLUDE_PHYS_STATISTICS_COLLECTOR_H
#define INCLUDE_PHYS_STATISTICS_COLLECTOR_H

// The memory statistics interface that calcStatistics implementations report to. Virtual order and the
// destructor body are part of the interface contract.

#include "phys/phys_base.h"

class phys_statistics_collector {
public:
    enum stat_category {
        k_stat_class_all = 0xf,
        stat_category_shared = 1,
        stat_category_instance = 2,
        stat_category_engine = 4,
        stat_category_runtime = 8,
        stat_category_application = 16,
        k_stat_class_last = 32
    };

    virtual ~phys_statistics_collector() { m_stats_client_data = 0; }
    virtual void begin_stats_object(const char *name, stat_category stat_class, const phys_ref_object *object) = 0;
    virtual void add_chunk_stats(const char *name, stat_category stat_class, const void *chunk_addr, int used_size, int allocated_size) = 0;
    virtual void add_child_object(const char *name, stat_category stat_class, const phys_ref_object *object) = 0;
    virtual void push_stats_dir(const char *directory_name) = 0;
    virtual void pop_dir() = 0;

    void add_allocated(const char *name, stat_category stat_class, const void *chunk_addr);

    template <typename T>
    __forceinline void add_array_stats(const char *name, stat_category stat_class, const phys_array<T> &array) {
        if (!(array.m_capacity_and_flags & PHYS_ARRAY_DONT_DEALLOCATE_FLAG)) {
            add_chunk_stats(name, stat_class, array.begin(), array.getSize() * int(sizeof(T)), array.get_sector_capacity() * int(sizeof(T)));
        }
    }

    template <typename T>
    __forceinline void add_small_array_stats(const char *name, stat_category stat_class, const phys_small_array<T> &array) {
        if (!(array.m_capacity_and_flags & phys_small_array<T>::ARRAY_NO_FREE_FLAG)) {
            add_chunk_stats(name, stat_class, array.begin(), array.getSize() * int(sizeof(T)), array.get_sector_capacity() * int(sizeof(T)));
        }
    }

    template <typename T>
    __forceinline void add_object_array_stats(const char *name, stat_category stat_class, const phys_object_array<T> &array) {
        if (!(array.m_inner_array.m_capacity_and_flags & PHYS_ARRAY_DONT_DEALLOCATE_FLAG)) {
            add_chunk_stats(name, stat_class, array.begin(), array.getSize() * int(sizeof(T)), array.get_sector_capacity() * int(sizeof(T)));
        }
    }

    virtual void end_object() = 0;

    const void *m_stats_client_data;
};

#endif // HAVOK_HK_STATISTICS_COLLECTOR_H
