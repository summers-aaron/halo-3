#ifndef MATH_SHORT_FIXED_STORAGE_H
#define MATH_SHORT_FIXED_STORAGE_H

// Shared lazy conversion scale for t_short_fixed<8>::import_real.
// The flag and scale are initialized together.
extern "C" {
extern unsigned long g_short_fixed_8_scale_initialized;
extern float g_short_fixed_8_scale;
}

#endif
