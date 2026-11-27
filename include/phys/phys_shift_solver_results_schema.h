#ifndef INCLUDE_PHYS_SHIFT_SOLVER_RESULTS_SCHEMA_H
#define INCLUDE_PHYS_SHIFT_SOLVER_RESULTS_SCHEMA_H

#include <stddef.h>
#include "phys/phys_schema.h"

class phys_shift_solver_results_schema : public phys_schema {
public:
#if defined(_XBOX) && !defined(__clang__)
    unsigned char m_header[3];
    joint_schema_kind m_type;
#else
    joint_schema_kind m_type;
    unsigned char m_header[3];
#endif
    unsigned char m_solver_result_count;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;

    // March 0x83524178 / 0x8352c0c0: the count follows the schema tag.
    __forceinline void init_shift_results_ptr(unsigned char count) {
        phys_schema::set_schema_type(this, SCHEMA_KIND_SHIFT_RESULTS_PTR);
        m_solver_result_count = count;
    }
    __forceinline int get_solver_result_count() const { return m_solver_result_count; }
};

typedef char phys_shift_solver_results_schema_size[(sizeof(phys_shift_solver_results_schema) == 16) ? 1 : -1];
typedef char phys_shift_solver_results_schema_type_offset[(offsetof(phys_shift_solver_results_schema, m_type)
    == PHYS_SCHEMA_TAG_OFFSET) ? 1 : -1];
typedef char phys_shift_solver_results_schema_count_offset[(offsetof(phys_shift_solver_results_schema, m_solver_result_count) == 4) ? 1 : -1];

#endif
