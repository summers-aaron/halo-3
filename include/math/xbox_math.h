#ifndef MATH_XBOX_MATH_H
#define MATH_XBOX_MATH_H

#include <xtl.h>
#include <xboxmath.h>

extern "C" FLOAT *xm_convert_half_to_float_stream(FLOAT *output, UINT output_stride, const HALF *input, UINT input_stride, UINT count);

#endif // MATH_XBOX_MATH_H
