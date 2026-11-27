// OBJ: from the records 0x677418
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x677418 ?verify_schema_info_arrays@phys_schema@@SAXXZ
#include "phys/phys_schema.h"
void phys_schema::verify_schema_info_arrays()
{
    // All schema size and temporary-count checks are debug assertions.
}

// Port link (P5): tables owned by hkconstraintsolver:hkJacobianSchema.obj
// (data-owners.tsv). Entries are indexed by SchemaType; contents match the
// reference image exactly (default.pe, file offset = rva).
// @address 0x726404 ?s_schemaSize@phys_schema@@2PAEA
unsigned char phys_schema::s_schemaSize[] = {
    0x10, // END
    0x10, // HEADER
    0x10, // GOTO
    0x10, // SHIFT_SOLVER_RESULTS_POINTER
    0xff, // SIMPLE_BEGIN
    0x30, // 1D_BILATERAL
    0x40, // 1D_BILATERAL_WITH_IMPULSE_LIMIT
    0x40, // 1D_LINEAR_LIMITS
    0x40, // 1D_FRICTION
    0x50, // 1D_LINEAR_MOTOR
    0x40, // 1D_PULLEY
    0x20, // 1D_ANGULAR
    0x30, // 1D_ANGULAR_LIMITS
    0x30, // 1D_ANGULAR_FRICTION
    0x40, // 1D_ANGULAR_MOTOR
    0x30, // SINGLE_CONTACT
    0x30, // SINGLE_CONTACT_WITH_ACCESSOR_CHECK
    0x70, // PAIR_CONTACT
    0x70, // PAIR_CONTACT_WITH_ACCESSOR_CHECK
    0x70, // 2D_FRICTION
    0x90, // 3D_FRICTION
    0x20, // SET_MASS
    0x20, // ADD_VELOCITY
    0xff, // SIMPLE_END
    0xff, // STIFF_SPRING_CHAIN
    0xff, // BALL_SOCKET_CHAIN
    0xff, // POWERED_CHAIN
    0x00, // MAX
};
// @address 0x726420 ?s_schemaNumSolverElemTemps@phys_schema@@2PAEA
unsigned char phys_schema::s_schemaNumSolverElemTemps[] = {
    0xff, // END
    0xff, // HEADER
    0xff, // GOTO
    0xff, // SHIFT_SOLVER_RESULTS_POINTER
    0xff, // SIMPLE_BEGIN
    0x01, // 1D_BILATERAL
    0x01, // 1D_BILATERAL_WITH_IMPULSE_LIMIT
    0x01, // 1D_LINEAR_LIMITS
    0x02, // 1D_FRICTION
    0x03, // 1D_LINEAR_MOTOR
    0x01, // 1D_PULLEY
    0x01, // 1D_ANGULAR
    0x01, // 1D_ANGULAR_LIMITS
    0x02, // 1D_ANGULAR_FRICTION
    0x03, // 1D_ANGULAR_MOTOR
    0x01, // SINGLE_CONTACT
    0x01, // SINGLE_CONTACT_WITH_ACCESSOR_CHECK
    0x02, // PAIR_CONTACT
    0x02, // PAIR_CONTACT_WITH_ACCESSOR_CHECK
    0x03, // 2D_FRICTION
    0x04, // 3D_FRICTION
    0x00, // SET_MASS
    0x00, // ADD_VELOCITY
    0xff, // SIMPLE_END
    0xff, // STIFF_SPRING_CHAIN
    0xff, // BALL_SOCKET_CHAIN
    0xff, // POWERED_CHAIN
    0x00, // MAX
};

