#ifndef INCLUDE_PHYS_MATH_UTIL_H
#define INCLUDE_PHYS_MATH_UTIL_H
#include "phys/phys_base.h"

 namespace phys_math_util
 {
    
    inline phys_bool PHYS_CALL invert_2x2(const phys_real* m, phys_real* out, phys_real tolerance);

    struct mat_decomposition
    {
        PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_MATH, mat_decomposition );
        phys_vec4 m_translation;	
        phys_quat m_rotation;	
        phys_vec4 m_scale;			

        phys_bool m_has_scale;			
        phys_bool m_has_skew;			
        phys_bool m_has_flip;				

        phys_mat3 m_scale_and_skew;	
        phys_mat3 m_skew_mat;			

        phys_rotation m_basis_rotation;			
                                    
    };

    void PHYS_CALL decompose_matrix (const class phys_mat4& matrix_in, mat_decomposition& decomp_out);

    void PHYS_CALL decompose_matrix (const class phys_mat3& matrix_in, mat_decomposition& decomp_out);

    void PHYS_CALL decompose_4x4_col_xform (const phys_real* matrix_in, mat_decomposition& decomp_out);

 }

 phys_bool PHYS_CALL phys_math_util::invert_2x2(const phys_real* m, phys_real* out, phys_real tolerance)
 {
    const phys_real *coeff_row0 = m;
    const phys_real *coeff_row1 = coeff_row0 + 2;

    phys_real D =  (coeff_row0[0] * coeff_row1[1]) - (coeff_row0[1] * coeff_row1[0]);

    if(D*D < tolerance * tolerance){
        return false;  
    }

    phys_real DI = 1.0f/D;
    phys_real a1 =  coeff_row1[1] * DI;
    phys_real b1 = -coeff_row0[1] * DI;
    phys_real a2 = -coeff_row1[0] * DI;
    phys_real b2 =  coeff_row0[0] * DI;

    phys_real *out_coeff0 = out;
    phys_real *out_coeff1 = out_coeff0 + 2;

    out_coeff0[0] = a1;
    out_coeff1[0] = b1;
    out_coeff0[1] = a2;
    out_coeff1[1] = b2;

    return true;
 }

#endif // INCLUDE_HAVOK_HK_MATH_UTIL_H
