#ifndef MATH_UNIT_VECTOR_QUANTIZATION_H
#define MATH_UNIT_VECTOR_QUANTIZATION_H

#include "math/real_math_types.h"

class c_dword_unit_vector
{
public:
    void get_real_vector3d(real_vector3d *vector) const;

private:
    unsigned long m_compressed;
};

#endif // MATH_UNIT_VECTOR_QUANTIZATION_H
