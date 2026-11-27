#ifndef INCLUDE_PHYS_CONSTRAINT_INFO_H
#define INCLUDE_PHYS_CONSTRAINT_INFO_H

// hkConstraintInfo on its own (hk_constraint_data.h includes it), for units that keep their own copies of
// the classes hk_base.h defines.

#include "phys/phys_thread_memory.h"
#include "phys/phys_math.h"

#define PHYS_SIZE_OF_END_SCHEMA 16
#define PHYS_SIZE_OF_HEADER_SCHEMA 16

struct phys_constraint_info {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_DYNAMICS, phys_constraint_info);

    int m_max_schema_size;
    int m_schemas_size;
    int m_solver_result_count;
    int m_solver_temp_count;

    inline void clear() { m_max_schema_size = 0; m_schemas_size = 0; m_solver_result_count = 0; m_solver_temp_count = 0; }
    inline void add_header_size() { m_schemas_size += PHYS_SIZE_OF_HEADER_SCHEMA; }
    inline void add(int schema_size, int solver_result_count, int solver_temp_elem_count) {
        m_schemas_size += schema_size;
        m_solver_result_count += solver_result_count;
        m_solver_temp_count += solver_temp_elem_count;
    }

    inline void add(const phys_constraint_info &other);
    inline void sub(const phys_constraint_info &other);

    inline void merge(const phys_constraint_info &other);
};

inline void phys_constraint_info::merge(const phys_constraint_info &delta)
{
    m_max_schema_size = phys_math::max_of_two(m_max_schema_size, delta.m_max_schema_size);
    m_schemas_size += delta.m_schemas_size;
    m_solver_result_count += delta.m_solver_result_count;
    m_solver_temp_count += delta.m_solver_temp_count;
}

inline void phys_constraint_info::add(const phys_constraint_info &delta)
{
    m_max_schema_size = phys_math::max_of_two(m_max_schema_size, int(delta.m_max_schema_size));
    m_max_schema_size = phys_math::max_of_two(m_max_schema_size, int(delta.m_schemas_size));
    m_schemas_size += delta.m_schemas_size;
    m_solver_result_count += delta.m_solver_result_count;
    m_solver_temp_count += delta.m_solver_temp_count;
}

inline void phys_constraint_info::sub(const phys_constraint_info &delta)
{
    m_schemas_size -= delta.m_schemas_size;
    m_solver_result_count -= delta.m_solver_result_count;
    m_solver_temp_count -= delta.m_solver_temp_count;
}

typedef char phys_constraint_info_size[(sizeof(phys_constraint_info) == 16) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_CONSTRAINT_INFO_H
