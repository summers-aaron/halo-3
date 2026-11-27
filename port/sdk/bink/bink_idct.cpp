// port/sdk/bink (both hosts): Bink's 8x8 inverse DCT (binkxenon dct.obj, include/bink/dct.h), called by the game's
// FastIDCT8x8 / FastIDCT8x8d (tu/binkxenon/dct.cpp) with a quantization table of k_bink_quantization_tables.
//
// The retail functions (0x71d0f0 fastidct8x8, 0x71d418 fastidct8x8d) are integer code; this is the same arithmetic:
//   - coefficients and quantizers are in natural order (row * 8 + column); a coefficient is dequantized as
//     (coefficient * quantizer) >> 11, column by column;
//   - columns first, a column whose rows 1..7 are all zero (the raw coefficients) is its dequantized DC in every row;
//   - the 1-D transform (constants in 1/2048 units: 2896 = sqrt(1/2), 3784, -5352, 2217), then the rows with the
//     same transform and the output (x + 127) >> 8 stored as a byte (truncated, not clamped, as the retail stb);
//   - fastidct8x8 writes 8x8 pixels at destination with `pitch` bytes per row; fastidct8x8d writes the block twice
//     the size, 16x16, every pixel doubled horizontally and every row doubled vertically.
// Both return a value nobody reads (the retail functions leave a scratch register there); 0 here.
#include "bink/dct.h"

namespace {

// The 1-D transform of 8 values in[0 * stride], .., in[7 * stride] into out[0], .., out[7].
inline void idct_1d(const int *in, int stride, int *out)
{
    const int s0 = in[0], s1 = in[stride], s2 = in[2 * stride], s3 = in[3 * stride];
    const int s4 = in[4 * stride], s5 = in[5 * stride], s6 = in[6 * stride], s7 = in[7 * stride];
    const int a0 = s0 + s4;
    const int a1 = s0 - s4;
    const int a2 = s2 + s6;
    const int a3 = (2896 * (s2 - s6)) >> 11;
    const int a4 = s5 + s3;
    const int a5 = s5 - s3;
    const int a6 = s1 + s7;
    const int a7 = s1 - s7;
    const int b0 = a4 + a6;
    const int b1 = (3784 * (a5 + a7)) >> 11;
    const int b2 = ((-5352 * a5) >> 11) - b0 + b1;
    const int b3 = ((2896 * (a6 - a4)) >> 11) - b2;
    const int b4 = ((2217 * a7) >> 11) + b3 - b1;
    out[0] = a0 + a2 + b0;
    out[1] = a1 + a3 - a2 + b2;
    out[2] = a1 - a3 + a2 + b3;
    out[3] = a0 - a2 - b4;
    out[4] = a0 - a2 + b4;
    out[5] = a1 - a3 + a2 - b3;
    out[6] = a1 + a3 - a2 - b2;
    out[7] = a0 + a2 - b0;
}

// Dequantized columns, then rows: pixels[row * 8 + column], already rounded and truncated to bytes.
void idct_8x8(short const *coefficients, long const *quantization, unsigned char pixels[64])
{
    int columns[64]; // columns[row * 8 + column]
    for (int c = 0; c < 8; c++) {
        int dequantized[8];
        int any = 0;
        for (int r = 1; r < 8; r++)
            any |= coefficients[r * 8 + c];
        const int dc = (int)(coefficients[c] * quantization[c]) >> 11;
        if (any == 0) {
            for (int r = 0; r < 8; r++)
                columns[r * 8 + c] = dc;
            continue;
        }
        for (int r = 0; r < 8; r++)
            dequantized[r] = (int)(coefficients[r * 8 + c] * quantization[r * 8 + c]) >> 11;
        int out[8];
        idct_1d(dequantized, 1, out);
        for (int r = 0; r < 8; r++)
            columns[r * 8 + c] = out[r];
    }
    for (int r = 0; r < 8; r++) {
        int out[8];
        idct_1d(&columns[r * 8], 1, out);
        for (int c = 0; c < 8; c++)
            pixels[r * 8 + c] = (unsigned char)((out[c] + 127) >> 8);
    }
}

} // namespace

extern "C" int fastidct8x8(unsigned char *destination, long pitch, short const *coefficients, long const *quantization)
{
    unsigned char pixels[64];
    idct_8x8(coefficients, quantization, pixels);
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            destination[r * pitch + c] = pixels[r * 8 + c];
    return 0;
}

extern "C" int fastidct8x8d(unsigned char *destination, long pitch, short const *coefficients, long const *quantization)
{
    unsigned char pixels[64];
    idct_8x8(coefficients, quantization, pixels);
    for (int r = 0; r < 8; r++) {
        unsigned char *row0 = destination + 2 * r * pitch, *row1 = row0 + pitch;
        for (int c = 0; c < 8; c++)
            row0[2 * c] = row0[2 * c + 1] = row1[2 * c] = row1[2 * c + 1] = pixels[r * 8 + c];
    }
    return 0;
}
