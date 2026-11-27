#ifndef INCLUDE_PHYS_SCHEMA_H
#define INCLUDE_PHYS_SCHEMA_H

#include "phys/phys_base_types.h"

// The schema tag is the low byte of a schema's first 32-bit word, written over the least significant byte of the
// first jacobian float. On the big-endian Xbox that is byte 3; on a little-endian PC build it is byte 0, and the
// schemas whose first word holds byte fields order those fields to match.
#if defined(_XBOX) && !defined(__clang__)
#define PHYS_SCHEMA_TAG_OFFSET 3
#else
#define PHYS_SCHEMA_TAG_OFFSET 0
#endif
class phys_schema {
public:
    enum schema_kind {
        SCHEMA_KIND_END = 0,
        SCHEMA_KIND_HEADER,
        SCHEMA_KIND_GOTO,
        SCHEMA_KIND_SHIFT_RESULTS_PTR,
        SCHEMA_KIND_SIMPLE_BEGIN,
        SCHEMA_KIND_1D_BILATERAL,
        SCHEMA_KIND_1D_BILATERAL_IMPULSE_CAP,
        SCHEMA_KIND_1D_LIN_LIMITS,
        SCHEMA_KIND_1D_FRICTION,
        SCHEMA_KIND_1D_LIN_MOTOR,
        SCHEMA_KIND_1D_PULLEY,
        SCHEMA_KIND_1D_ANG,
        SCHEMA_KIND_1D_ANG_LIMITS,
        SCHEMA_KIND_1D_ANG_FRICTION,
        SCHEMA_KIND_1D_ANG_MOTOR,
        SCHEMA_KIND_SINGLE_CONTACT,
        SCHEMA_KIND_SINGLE_CONTACT_CHECKED,
        SCHEMA_KIND_PAIR_CONTACT,
        SCHEMA_KIND_PAIR_CONTACT_CHECKED,
        SCHEMA_KIND_2D_FRICTION,
        SCHEMA_KIND_3D_FRICTION,
        SCHEMA_KIND_SET_MASS,
        SCHEMA_KIND_ADD_VEL,
        SCHEMA_KIND_SIMPLE_END,
        SCHEMA_KIND_STIFF_SPRING_CHAIN,
        SCHEMA_KIND_BALL_SOCKET_CHAIN,
        SCHEMA_KIND_POWERED_CHAIN,
        SCHEMA_KIND_MAX,
        // The solver and atom builder name schema 6 USER_TAU; the export unit names it WITH_IMPULSE_LIMIT.
        SCHEMA_KIND_1D_BILATERAL_USER_TAU = SCHEMA_KIND_1D_BILATERAL_IMPULSE_CAP
    };

    typedef phys_enum<schema_kind, signed char> joint_schema_kind;

    // March 0x83528450: no named local, returning the tag wrapper by value.
    static __forceinline joint_schema_kind get_type_id(const phys_schema *jacobian_schema) {
        return *reinterpret_cast<const joint_schema_kind *>(reinterpret_cast<const char *>(jacobian_schema) + PHYS_SCHEMA_TAG_OFFSET);
    }

    // March 0x8350c760: one address local before storing the tag wrapper.
    static __forceinline void set_schema_type(phys_schema *jacobian_schema, joint_schema_kind type) {
        joint_schema_kind *address = reinterpret_cast<joint_schema_kind *>(reinterpret_cast<char *>(jacobian_schema) + PHYS_SCHEMA_TAG_OFFSET);
        *address = type;
    }

    // March 0x8352bee8 and 0x83528490: each names the table result before its debug assertions.
    static __forceinline int getSize(const phys_schema *jacobian_schema) {
        int size = s_schemaSize[get_type_id(jacobian_schema)];
        return size;
    }
    static __forceinline int get_solver_temp_count(const phys_schema *jacobian_schema) {
        // match: 0x66e730, 0x670858, 0x672fd0: keep the tag temporary before indexing the element-count table.
        const joint_schema_kind type = get_type_id(jacobian_schema);
        int solver_elem_temp_count = s_schemaNumSolverElemTemps[type];
        return solver_elem_temp_count;
    }

    static void verify_schema_info_arrays();
    static unsigned char s_schemaSize[];
    static unsigned char s_schemaNumSolverElemTemps[];
};

#endif // HAVOK_HK_JACOBIAN_SCHEMA_H
