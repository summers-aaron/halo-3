#ifndef MATH_VECTOR_MATH_H
#define MATH_VECTOR_MATH_H

#include <vectorintrinsics.h>

// Computes the sine and cosine of each input angle.
// match: 0x11a5f8: preserve the accepted SDK call's output-only memory summary.
__declspec(noalias) void XMVectorSinCos(__vector4 *sines, __vector4 *cosines, __vector4 angles);

#endif // MATH_VECTOR_MATH_H
