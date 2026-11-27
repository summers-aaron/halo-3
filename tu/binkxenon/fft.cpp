// binkxenon/fft.obj: from the records 0x71b9a0 0x71ba70 0x71c5c8 0x71c6a8 0x71cd48
// @flags /TP /O2 /Oc
// @compiler xdk3424
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "bink/fft.h"


extern "C" void rftfsub(int n, float *a, int nc, float *c)
{
    int j, k, kk, ks, m;
    float wkr, wki, xr, xi, yr, yi;

    m = n >> 1;
    ks = 2 * nc / m;
    kk = 0;
    for (j = 2; j < m; j += 2) {
        k = n - j;
        kk += ks;
        wkr = 0.5f - c[nc - kk];
        wki = c[kk];
        xr = a[j] - a[k];
        xi = a[j + 1] + a[k + 1];
        yr = wkr * xr - wki * xi;
        yi = wkr * xi + wki * xr;
        a[j] -= yr;
        a[j + 1] -= yi;
        a[k] += yr;
        a[k + 1] -= yi;
    }
}

// Inverse real-transform postprocessing for paired frequency bins.
// @address 0x71ba70 rftbsub
extern "C" void rftbsub(int n, float *a, int nc, float *c)
{
    int j, k, kk, ks, m;
    float wkr, wki, xr, xi, yr, yi;

    m = n >> 1;
    ks = 2 * nc / m;
    kk = 0;
    for (j = 2; j < m; j += 2) {
        k = n - j;
        kk += ks;
        wkr = 0.5f - c[nc - kk];
        wki = c[kk];
        xr = a[j] - a[k];
        xi = a[j + 1] + a[k + 1];
        yr = wkr * xr + wki * xi;
        yi = wkr * xi - wki * xr;
        a[j] -= yr;
        a[j + 1] -= yi;
        a[k] += yr;
        a[k + 1] -= yi;
    }
}

// cftrec1: recursive stage of the complex FFT (Ooura layout: n, a, nw, w)
extern "C" void cftrec1(int n, float *a, int nw, float *w)
{
    int m;
    int k;
    float *j;
    m = n;
    k = m >> 2;
    j = a;
    cftmdl1(n, a, (nw - (k << 1)) + w);
    if (m > 0x200)
    {
        do
        {
            cftrec1(k, j, nw, w);
            cftrec2(k, k + j, nw, w);
            cftrec1(k, (k << 1) + j, nw, w);
            m = k;
            j = (k + (k << 1)) + j;
            k = (k >> 2);
            cftmdl1(m, j, (nw - (k << 1)) + w);
        } while (m > 0x200);
    }
    cftexp1(m, j, nw, w);
}

// @address 0x71c6a8 cftrec2
extern "C" void cftrec2(int n, float *a, int nw, float *w)
{
    int k;
    float *j;
    j = a;
    k = n >> 2;
    cftmdl2(n, a, (nw - n) + w);
    if (n > 0x200)
    {
        do
        {
            cftrec1(k, j, nw, w);
            cftrec2(k, k + j, nw, w);
            cftrec1(k, (k << 1) + j, nw, w);
            n = k;
            j = (k + (k << 1)) + j;
            k >>= 2;
            cftmdl2(n, j, (nw - n) + w);
        } while (n > 0x200);
    }
    cftexp2(n, j, nw, w);
}

// rdft: real FFT driver (Ooura layout: n, isgn, a, ip, w)
extern "C" void rdft(int n, int isgn, float *a, int *ip, float *w)
{
    int nw;
    int nc;
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = (n >> 2);
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > (nc << 2)) {
        nc = (n >> 2);
        makect(nc, ip, nw + w);
    }
    float re;
    float im;
    if (isgn >= 0) {
        if (n > 4) {
            cftfsub(n, a, ip + 2, nw, w);
            rftfsub(n, a, nc, nw + w);
        } else if (n == 4) {
            cftfsub(4, a, ip + 2, nw, w);
        }
        re = a[0];
        im = a[1];
        a[1] = re - im;
        a[0] = im + re;
        return;
    } else {
        re = a[0];
        im = (re - a[1]) * 0.5f;
        a[1] = im;
        a[0] = re - im;
        int tr3;
        if (n > 4) {
            rftbsub(n, a, nc, nw + w);
            tr3 = n;
            cftbsub(tr3, a, ip + 2, nw, w);
        } else if (n == 4) {
            tr3 = 4;
            cftbsub(tr3, a, ip + 2, nw, w);
            return;
        }
        return;
    }
}
