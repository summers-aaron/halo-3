#ifndef BINK_DCT_H
#define BINK_DCT_H

extern "C" {
extern const long k_bink_quantization_tables[16][64];
int fastidct8x8(unsigned char *destination, long pitch, short const *coefficients, long const *quantization);
int fastidct8x8d(unsigned char *destination, long pitch, short const *coefficients, long const *quantization);
}

#endif // BINK_DCT_H
