#ifndef BINK_FFT_H
#define BINK_FFT_H

// Bink's real FFT (binkxenon fft.obj): Takuya Ooura's split-radix FFT in single precision. The tree defines
// rdft, rftfsub, rftbsub, cftrec1 and cftrec2; the other declarations below are its complex-transform helpers.

#ifdef __cplusplus
extern "C" {
#endif

void rdft(int n, int isgn, float *a, int *ip, float *w);
void makewt(int nw, int *ip, float *w);
void makect(int nc, int *ip, float *c);
void cftfsub(int n, float *a, int *ip, int nw, float *w);
void cftbsub(int n, float *a, int *ip, int nw, float *w);
void rftfsub(int n, float *a, int nc, float *c);
void rftbsub(int n, float *a, int nc, float *c);
void cftrec1(int n, float *a, int nw, float *w);
void cftrec2(int n, float *a, int nw, float *w);
void cftexp1(int n, float *a, int nw, float *w);
void cftexp2(int n, float *a, int nw, float *w);
void cftmdl1(int n, float *a, float *w);
void cftmdl2(int n, float *a, float *w);

#ifdef __cplusplus
}
#endif

#endif // BINK_FFT_H
