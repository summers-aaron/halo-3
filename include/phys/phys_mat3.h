#ifndef INCLUDE_PHYS_MAT3_H
#define INCLUDE_PHYS_MAT3_H

// Functions defined in tu/hkmath/hkMatrix3.cpp that other units call.

class phys_mat3;

extern "C" void phys_mat3_invert_symmetric( phys_mat3& this_matrix );

#endif // HAVOK_HK_MATRIX3_H
