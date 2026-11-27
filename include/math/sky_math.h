#ifndef MATH_SKY_MATH_H
#define MATH_SKY_MATH_H

#include "math/real_math_types.h"

class c_spectral_curve
{
public:
	c_spectral_curve();
	c_spectral_curve(const float *x_values, const float *y_values, long count);
	c_spectral_curve(float, float, const float *, long);
	c_spectral_curve(float, float, float);
	float get_value(float) const;
	void convert_to_RGB(float *, float *, float *);
	void convert_to_XYZ(float *, float *, float *);
	void set(float, float, const float *, long);

private:
	real_vector4d m_samples[512];
	long m_count;
};
typedef char spectral_curve_size[(sizeof(c_spectral_curve) == 0x2004) ? 1 : -1];

extern c_spectral_curve cie_cmf_curve_X;
extern c_spectral_curve cie_cmf_curve_Y;
extern c_spectral_curve cie_cmf_curve_Z;

#endif // MATH_SKY_MATH_H
