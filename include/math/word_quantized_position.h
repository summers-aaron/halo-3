#ifndef MATH_WORD_QUANTIZED_POSITION_H
#define MATH_WORD_QUANTIZED_POSITION_H

#include "math/real_math_types.h"

class c_word_quantized_position
{
public:
    void get_real_point3d(const real_rectangle3d *bounds, real_point3d *point) const;
    void set_real_point3d(const real_rectangle3d *bounds, const real_point3d *point);

private:
    unsigned short m_position[3];
};

typedef char word_quantized_position_size[(sizeof(c_word_quantized_position) == 6) ? 1 : -1];

#endif // MATH_WORD_QUANTIZED_POSITION_H
