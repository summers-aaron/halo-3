#ifndef INCLUDE_PHYS_HEADER_SCHEMA_H
#define INCLUDE_PHYS_HEADER_SCHEMA_H

#include "phys/phys_schema.h"
#include "phys/phys_vel_accum.h"
#include "tag_files/tag_types.h"

class phys_solver_results;
class phys_constraint;

class __declspec(align(16)) phys_header_schema : public phys_schema {
public:
    __forceinline void init_schema_header(int schema_body_a_index, int schema_body_b_index, phys_solver_results *results, int solver_result_stride) {
        set_schema_type(this, SCHEMA_KIND_HEADER);
        m_body_a_index = static_cast<unsigned short>(schema_body_a_index);
        m_body_b_index = static_cast<unsigned short>(schema_body_b_index);
        m_solver_results_main = results;
        m_solver_result_stride = static_cast<unsigned char>(solver_result_stride);
    }
    __forceinline phys_vel_accum *get_body_a(phys_vel_accum *accum_array) const {
        return accum_array + m_body_a_index;
    }
    __forceinline phys_vel_accum *get_body_b(phys_vel_accum *accum_array) const {
        return accum_array + m_body_b_index;
    }
    __forceinline const phys_vel_accum *get_body_a(const phys_vel_accum *accum_array) const {
        return accum_array + m_body_a_index;
    }
    __forceinline const phys_vel_accum *get_body_b(const phys_vel_accum *accum_array) const {
        return accum_array + m_body_b_index;
    }

private:
    unsigned char m_header_pad0;
public:
    unsigned char m_solver_result_stride;
private:
    unsigned short m_header_pad1;
public:
    unsigned short m_body_a_index;
    unsigned short m_body_b_index;
    TAG_POINTER(phys_solver_results) m_solver_results_main;
    TAG_POINTER(phys_constraint) m_query_constraint;
};

class __declspec(align(16)) phys_goto_schema : public phys_schema {
public:
    __forceinline void init_goto(phys_schema *destination) {
        m_offset = phys_get_byte_offset(this, destination);
        set_schema_type(this, SCHEMA_KIND_GOTO);
    }
    __forceinline void init_goto_offset(int offset) {
        m_offset = offset;
        set_schema_type(this, SCHEMA_KIND_GOTO);
    }
private:
    // First word is filler carrying the schema tag at byte 3; the SDK hkJacobianGotoSchema spells it scalar m_padding0.
    unsigned int m_header_pad0;
public:
    int m_offset;
};

typedef char phys_header_schema_size[(sizeof(void *) != 4 || ((sizeof(phys_header_schema) == 16) )) ? 1 : -1];
typedef char phys_header_schema_body_a_offset[(offsetof(phys_header_schema, m_body_a_index) == 4) ? 1 : -1];
typedef char phys_header_schema_body_b_offset[(offsetof(phys_header_schema, m_body_b_index) == 6) ? 1 : -1];
typedef char phys_header_schema_results_offset[(offsetof(phys_header_schema, m_solver_results_main) == 8) ? 1 : -1];
typedef char phys_jacobian_goto_size[(sizeof(phys_goto_schema) == 16) ? 1 : -1];
typedef char phys_jacobian_goto_offset_offset[(offsetof(phys_goto_schema, m_offset) == 4) ? 1 : -1];

#endif
