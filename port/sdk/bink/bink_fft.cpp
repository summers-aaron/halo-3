// port/sdk/bink (both hosts): the complex-FFT helpers of Bink's real FFT (binkxenon fft.obj: Takuya Ooura's split
// radix FFT in single precision, include/bink/fft.h). The game's own units define rdft, rftfsub, rftbsub, cftrec1 and
// cftrec2 (tu/binkxenon/fft.cpp); these are the rest of the package: makewt, makect, cftfsub, cftbsub, cftexp1,
// cftexp2, cftmdl1, cftmdl2. Built like the game (the declarations of include/bink/fft.h).
//
// What the game's code relies on (rdft, cftrec1, cftrec2 call these with Ooura's arguments):
//   - rdft(n, isgn, a, ip, w): w[0 .. nw) holds makewt's tables (nw = ip[0]), w[nw .. nw + nc) makect's cosine
//     table (nc = ip[1]); ip[2 ..] is the bit reversal's work area;
//   - cftfsub(n, a, ...) transforms n / 2 complex values in place, X[k] = sum_j x[j] exp(+2 pi i j k / (n / 2)), in
//     natural order; cftbsub the same with exp(-...); a[2j] is the real and a[2j + 1] the imaginary part;
//   - the split-radix stages: a "type 1" block of N complex values is a plain DFT_N; a "type 2" block is the DFT_N of
//     x[p] exp(+2 pi i p / 2N) (the odd outputs of a DFT_2N). cftmdl1(n, a, &w[nw - n / 2]) is the first radix-4
//     stage of a type 1 block of N = n / 2, leaving its quarters as blocks of types 1, 2, 1, 1;
//     cftmdl2(n, a, &w[nw - n]) the first stage of a type 2 block, leaving types 1, 2, 1, 2 (cftrec1 / cftrec2
//     recurse by exactly these types and offsets). cftexp1 / cftexp2 finish a block of type 1 / 2 whose first
//     stage is done (cftrec1 / cftrec2 call them at n <= 512). Every block leaves its outputs in bit-reversed order
//     (quarters hold the residues 0, 2, 1, 3 mod 4), which cftfsub's final bit reversal undoes.
//   - the twiddle tables: L(M), M = nw, nw / 2, ... 4, at w[nw - M], is exp(2 pi i q / M) for q < M / 4 as (cos,
//     sin) pairs (M / 2 floats; the full circle follows by quarter turns). cftmdl1 of N reads L(N) (at
//     nw - n / 2), cftmdl2 of N reads L(2N) (at nw - n), the first stage of cftfsub reads L(N / 2) (at
//     nw - n / 4: the top level has no L(N)) and makes the odd twiddles with one complex multiply.
#include <math.h>
#include "bink/fft.h"

namespace {

struct s_complex
{
    float re, im;
};

inline s_complex multiply(s_complex a, s_complex b)
{
    s_complex r = { a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re };
    return r;
}

// exp(2 pi i p / m) from L(m) (any p: reduced to a quarter turn and a table entry).
s_complex twiddle(const float *table, int m, int p)
{
    s_complex r = { 1.0f, 0.0f };
    if (m <= 1)
        return r;
    p &= m - 1;
    const int quarter = m >> 2;
    int turns, q;
    if (quarter == 0) { // m = 2: +1 or -1
        turns = p * 2;
        q = 0;
    } else {
        turns = p / quarter;
        q = p % quarter;
    }
    s_complex base = { 1.0f, 0.0f };
    if (q != 0) {
        base.re = table[2 * q];
        base.im = table[2 * q + 1];
    }
    switch (turns & 3) {
    case 0: r = base; break;
    case 1: r.re = -base.im; r.im = base.re; break;
    case 2: r.re = -base.re; r.im = -base.im; break;
    default: r.re = base.im; r.im = -base.re; break;
    }
    return r;
}

inline s_complex load(const float *a, int j)
{
    s_complex c = { a[2 * j], a[2 * j + 1] };
    return c;
}

inline void store(float *a, int j, s_complex c)
{
    a[2 * j] = c.re;
    a[2 * j + 1] = c.im;
}

// The radix-4 stage of a type 1 block of n floats with twiddles w1(p) = exp(2 pi i p / N), w3(p) = w1(3p).
template <typename t_twiddles> void radix4_type1(int n, float *a, const t_twiddles &twiddles)
{
    const int quarter = n >> 3; // complex values per quarter
    for (int p = 0; p < quarter; p++) {
        const s_complex a0 = load(a, p), a1 = load(a, p + quarter), a2 = load(a, p + 2 * quarter),
                        a3 = load(a, p + 3 * quarter);
        const s_complex x0 = { a0.re + a2.re, a0.im + a2.im }, x1 = { a0.re - a2.re, a0.im - a2.im };
        const s_complex x2 = { a1.re + a3.re, a1.im + a3.im }, x3 = { a1.re - a3.re, a1.im - a3.im };
        const s_complex q0 = { x0.re + x2.re, x0.im + x2.im }, q1 = { x0.re - x2.re, x0.im - x2.im };
        const s_complex y = { x1.re - x3.im, x1.im + x3.re }; // x1 + i x3: residue 1
        const s_complex z = { x1.re + x3.im, x1.im - x3.re }; // x1 - i x3: residue 3
        store(a, p, q0);
        store(a, p + quarter, q1);
        store(a, p + 2 * quarter, multiply(y, twiddles(p)));
        store(a, p + 3 * quarter, multiply(z, twiddles(3 * p)));
    }
}

struct s_table_twiddles
{
    const float *table;
    int m;
    s_complex operator()(int p) const { return twiddle(table, m, p); }
};

// exp(2 pi i p / N) from L(N / 2): even p from the table, odd p one step of exp(2 pi i / N) further.
struct s_fine_twiddles
{
    const float *table;
    int half;                   // N / 2
    s_complex step;             // exp(2 pi i / N)
    s_fine_twiddles(const float *table_, int n_complex) : table(table_), half(n_complex >> 1)
    {
        const s_complex w2 = twiddle(table, half, 1); // exp(4 pi i / N): its half angle
        const double c = sqrt(0.5 * (1.0 + (double)w2.re));
        step.re = (float)c;
        step.im = (float)((double)w2.im / (2.0 * c));
    }
    s_complex operator()(int p) const
    {
        p &= 2 * half - 1;
        if ((p & 1) == 0)
            return twiddle(table, half, p >> 1);
        return multiply(twiddle(table, half, p >> 1), step);
    }
};

// The first stage of the whole transform (a type 1 block of N = n / 2), its twiddles from L(N / 2).
void cftf1st(int n, float *a, const float *w)
{
    radix4_type1(n, a, s_fine_twiddles(w, n >> 1));
}

void cft_quarters(int type, int n, float *a, int nw, float *w);

// A whole block of `type` and n floats: its first stage, then its quarters.
void cft_block(int type, int n, float *a, int nw, float *w)
{
    const int count = n >> 1;
    if (count <= 1)
        return;
    if (count == 2) {
        const s_complex a0 = load(a, 0), a1 = load(a, 1);
        // type 2: the second value turns by exp(2 pi i / 4) = i first
        const s_complex b = type == 1 ? a1 : s_complex{ -a1.im, a1.re };
        store(a, 0, s_complex{ a0.re + b.re, a0.im + b.im });
        store(a, 1, s_complex{ a0.re - b.re, a0.im - b.im });
        return;
    }
    // L(N) / L(2N); a transform smaller than the tables' first level (n = 8 with nw = 2) reads no entry of them
    const int offset = type == 1 ? n >> 1 : n;
    float *table = nw >= offset ? &w[nw - offset] : w;
    if (type == 1)
        cftmdl1(n, a, table);
    else
        cftmdl2(n, a, table);
    cft_quarters(type, n, a, nw, w);
}

// The quarters of a block of `type` whose first stage is done: types 1, 2, 1 and the block's own.
void cft_quarters(int type, int n, float *a, int nw, float *w)
{
    const int m = n >> 2;
    if (m < 2)
        return;
    cft_block(1, m, a, nw, w);
    cft_block(2, m, a + m, nw, w);
    cft_block(1, m, a + 2 * m, nw, w);
    cft_block(type, m, a + 3 * m, nw, w);
}

// Bit reversal of the n / 2 complex values (natural order from the stages' bit-reversed one). Ooura's versions use
// ip[] as a work area; this one does not need it.
void bitrv2(int n, float *a)
{
    const int count = n >> 1;
    for (int i = 0, j = 0; i < count; i++) {
        if (i < j) {
            const s_complex t = load(a, i);
            store(a, i, load(a, j));
            store(a, j, t);
        }
        int bit = count >> 1;
        while (bit != 0 && (j & bit) != 0) {
            j ^= bit;
            bit >>= 1;
        }
        j |= bit;
    }
}

void conjugate(int n, float *a)
{
    for (int j = 1; j < n; j += 2)
        a[j] = -a[j];
}

} // namespace

extern "C" {

// Twiddle tables for transforms of up to 4 * nw floats (see the top of the file); ip[0] = nw, ip[1] = 1 (makect's
// table, after w[nw], is made again).
void makewt(int nw, int *ip, float *w)
{
    ip[0] = nw;
    ip[1] = 1;
    if (nw < 4)
        return;
    const double turn = 8.0 * atan(1.0); // 2 pi
    // L(nw) at w[0]: exp(2 pi i q / nw), q < nw / 4
    for (int q = 0; q < nw / 4; q++) {
        w[2 * q] = (float)cos(turn * q / nw);
        w[2 * q + 1] = (float)sin(turn * q / nw);
    }
    // L(M) for the smaller M: every (nw / M)th entry of L(nw)
    for (int m = nw >> 1; m >= 4; m >>= 1) {
        float *table = &w[nw - m];
        const int stride = nw / m;
        for (int q = 0; q < m / 4; q++) {
            table[2 * q] = w[2 * q * stride];
            table[2 * q + 1] = w[2 * q * stride + 1];
        }
    }
}

// Ooura's cosine table for rftfsub / rftbsub: c[j] = cos(j pi / 4nc) / 2, c[nc - j] = sin(j pi / 4nc) / 2, c[0] =
// cos(pi / 4).
void makect(int nc, int *ip, float *c)
{
    ip[1] = nc;
    if (nc > 1) {
        const int nch = nc >> 1;
        const double delta = atan(1.0) / nch;
        c[0] = (float)cos(delta * nch);
        c[nch] = 0.5f * c[0];
        for (int j = 1; j < nch; j++) {
            c[j] = (float)(0.5 * cos(delta * j));
            c[nc - j] = (float)(0.5 * sin(delta * j));
        }
    }
}

// The first radix-4 stage of a type 1 block of n floats (w: L(n / 2)).
void cftmdl1(int n, float *a, float *w)
{
    s_table_twiddles twiddles = { w, n >> 1 };
    radix4_type1(n, a, twiddles);
}

// The first radix-4 stage of a type 2 block of n floats (w: L(n)): the block's turn exp(2 pi i p / n) folded in.
void cftmdl2(int n, float *a, float *w)
{
    const int quarter = n >> 3, m = n; // complex values per quarter; L(2N) = L(n)
    for (int p = 0; p < quarter; p++) {
        const s_complex a0 = load(a, p), a1 = load(a, p + quarter), a2 = load(a, p + 2 * quarter),
                        a3 = load(a, p + 3 * quarter);
        const s_complex x0 = { a0.re - a2.im, a0.im + a2.re }; // a0 + i a2
        const s_complex x1 = { a0.re + a2.im, a0.im - a2.re }; // a0 - i a2
        const s_complex x2 = { a1.re - a3.im, a1.im + a3.re }; // a1 + i a3
        const s_complex x3 = { a1.re + a3.im, a1.im - a3.re }; // a1 - i a3
        const s_complex y0 = multiply(x0, twiddle(w, m, p));
        const s_complex y2 = multiply(x2, twiddle(w, m, p + quarter));
        const s_complex y1 = multiply(x1, twiddle(w, m, 3 * p));
        const s_complex y3 = multiply(x3, twiddle(w, m, 3 * (p + quarter)));
        store(a, p, s_complex{ y0.re + y2.re, y0.im + y2.im });
        store(a, p + quarter, s_complex{ y0.re - y2.re, y0.im - y2.im });
        store(a, p + 2 * quarter, s_complex{ y1.re + y3.re, y1.im + y3.im });
        store(a, p + 3 * quarter, s_complex{ y1.re - y3.re, y1.im - y3.im });
    }
}

void cftexp1(int n, float *a, int nw, float *w)
{
    cft_quarters(1, n, a, nw, w);
}

void cftexp2(int n, float *a, int nw, float *w)
{
    cft_quarters(2, n, a, nw, w);
}

void cftfsub(int n, float *a, int *ip, int nw, float *w)
{
    (void)ip;
    if (n > 8) {
        cftf1st(n, a, &w[nw - (n >> 2)]);
        if (n > 512) {
            const int m = n >> 2; // the game's recursion for the large blocks
            cftrec1(m, a, nw, w);
            cftrec2(m, a + m, nw, w);
            cftrec1(m, a + 2 * m, nw, w);
            cftrec1(m, a + 3 * m, nw, w);
        } else {
            cftexp1(n, a, nw, w);
        }
    } else {
        cft_block(1, n, a, nw, w);
    }
    bitrv2(n, a);
}

// The inverse direction: the forward transform of the conjugate, conjugated.
void cftbsub(int n, float *a, int *ip, int nw, float *w)
{
    conjugate(n, a);
    cftfsub(n, a, ip, nw, w);
    conjugate(n, a);
}

} // extern "C"
