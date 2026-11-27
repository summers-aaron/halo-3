#ifndef INCLUDE_PHYS_CONTACT_SCHEMA_TYPES_H
#define INCLUDE_PHYS_CONTACT_SCHEMA_TYPES_H

#include "phys/phys_schema.h"
#include "phys/phys_jacobian_element.h"
#include "tag_files/tag_types.h"

class __declspec(align(16)) phys_double_bilateral_jacobian {
public:
    phys_1lin2ang_jacobian m_jac_0;
    phys_1lin2ang_jacobian m_jac_1;
};

class __declspec(align(16)) phys_triple_bilateral_ang_jacobian {
public:
    phys_1lin2ang_jacobian m_jac_0;
    phys_1lin2ang_jacobian m_jac_1;
    phys_2ang_jacobian m_jac_2;
};

class __declspec(align(16)) phys_single_contact_schema : public phys_schema {
public:
    void init_single_contact(const phys_1lin2ang_jacobian &lin_ang_jac, int request_accessor_check);
    phys_1lin2ang_jacobian m_contact_jac;
};

class __declspec(align(16)) phys_pair_contact_schema : public phys_schema {
public:
    void init_pair_contact(const phys_double_bilateral_jacobian &lin_ang_jac, phys_real inv_jac01, int accessor_check);
    phys_double_bilateral_jacobian m_contact_jac;
    phys_real m_inv_jac01;
};

class __declspec(align(16)) phys_2d_friction_schema : public phys_schema {
public:
    void init_2d_friction(const phys_double_bilateral_jacobian &lin_ang_jac, phys_real contact_max_friction, phys_real inv_jac01, phys_solver_results *sr);
    __forceinline void unpack_solver_results(phys_solver_results *&sr) const { sr = m_solver_results; }
    phys_double_bilateral_jacobian m_contact_jac;
    phys_real m_inv_jac01;
    phys_real m_max_friction_force;
    TAG_POINTER(phys_solver_results) m_solver_results;
};

class __declspec(align(16)) phys_3d_friction_schema : public phys_schema {
public:
    void init_3d_friction(const phys_triple_bilateral_ang_jacobian &lin_ang_jac, phys_real contact_max_friction, phys_real inv_jac01,
        phys_real radius, phys_solver_results *sr);
    __forceinline void unpack_solver_results(phys_solver_results *&sr) const { sr = m_solver_results; }
    phys_triple_bilateral_ang_jacobian m_contact_jac;
    phys_real m_inv_jac01;
    phys_real m_max_friction_force;
    phys_real m_friction_radius;
    TAG_POINTER(phys_solver_results) m_solver_results;
};

// March 2007: init2dFriction 0x8352d4a8 and init3dFriction 0x8352d520.
typedef char phys_double_bilateral_jacobian_size[(sizeof(phys_double_bilateral_jacobian) == 96) ? 1 : -1];
typedef char phys_triple_bilateral_ang_jacobian_size[(sizeof(phys_triple_bilateral_ang_jacobian) == 128) ? 1 : -1];
typedef char phys_pair_contact_schema_size[(sizeof(phys_pair_contact_schema) == 112) ? 1 : -1];
typedef char phys_2d_friction_schema_size[(sizeof(phys_2d_friction_schema) == 112) ? 1 : -1];
typedef char phys_3d_friction_schema_size[(sizeof(void *) != 4 || sizeof(phys_3d_friction_schema) == 144) ? 1 : -1];
typedef char phys_2d_friction_inv_offset[(offsetof(phys_2d_friction_schema, m_inv_jac01) == 96) ? 1 : -1];
typedef char phys_2d_friction_results_offset[(offsetof(phys_2d_friction_schema, m_solver_results) == 104) ? 1 : -1];
typedef char phys_3d_friction_inv_offset[(offsetof(phys_3d_friction_schema, m_inv_jac01) == 128) ? 1 : -1];
typedef char phys_3d_friction_results_offset[(sizeof(void *) != 4 || offsetof(phys_3d_friction_schema, m_solver_results) == 140) ? 1 : -1];

typedef char phys_single_contact_schema_size[(sizeof(phys_single_contact_schema) == 48) ? 1 : -1];
typedef char phys_single_contact_schema_jacobian_offset[(offsetof(phys_single_contact_schema, m_contact_jac) == 0) ? 1 : -1];

#endif
