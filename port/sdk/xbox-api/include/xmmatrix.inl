// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xmmatrix.inl: xboxmath 4x4 matrix routines (product, transpose, inverse, identity) and XMMATRIX members, VMX32
// evaluation. The vector operations are calls into the port's VMX intrinsics, so their order is part of the result.
#pragma once
#ifndef __XMMATRIX_INL__
#define __XMMATRIX_INL__

// M1 * M2: each result row is the M2 rows weighted by that M1 row's elements, accumulated with multiply-adds
// starting from -0 (so a zero product keeps its sign), element 0 of every row first, then 1, 2, 3.
XMFINLINE XMMATRIX XMMatrixMultiply(XMMATRIX M1, XMMATRIX M2)
{
    XMVECTOR s0, s1, s2, s3;
    XMVECTOR acc0, acc1, acc2, acc3;
    XMMATRIX product;

    s0 = __vspltw(M1.r[0], 0);
    s1 = __vspltw(M1.r[1], 0);
    s2 = __vspltw(M1.r[2], 0);
    s3 = __vspltw(M1.r[3], 0);
    acc0 = __vmaddfp(s0, M2.r[0], *(XMVECTOR *)g_XMNegativeZero);
    acc1 = __vmaddfp(s1, M2.r[0], *(XMVECTOR *)g_XMNegativeZero);
    acc2 = __vmaddfp(s2, M2.r[0], *(XMVECTOR *)g_XMNegativeZero);
    acc3 = __vmaddfp(s3, M2.r[0], *(XMVECTOR *)g_XMNegativeZero);

    s0 = __vspltw(M1.r[0], 1);
    s1 = __vspltw(M1.r[1], 1);
    s2 = __vspltw(M1.r[2], 1);
    s3 = __vspltw(M1.r[3], 1);
    acc0 = __vmaddfp(s0, M2.r[1], acc0);
    acc1 = __vmaddfp(s1, M2.r[1], acc1);
    acc2 = __vmaddfp(s2, M2.r[1], acc2);
    acc3 = __vmaddfp(s3, M2.r[1], acc3);

    s0 = __vspltw(M1.r[0], 2);
    s1 = __vspltw(M1.r[1], 2);
    s2 = __vspltw(M1.r[2], 2);
    s3 = __vspltw(M1.r[3], 2);
    acc0 = __vmaddfp(s0, M2.r[2], acc0);
    acc1 = __vmaddfp(s1, M2.r[2], acc1);
    acc2 = __vmaddfp(s2, M2.r[2], acc2);
    acc3 = __vmaddfp(s3, M2.r[2], acc3);

    s0 = __vspltw(M1.r[0], 3);
    s1 = __vspltw(M1.r[1], 3);
    s2 = __vspltw(M1.r[2], 3);
    s3 = __vspltw(M1.r[3], 3);
    product.r[0] = __vmaddfp(s0, M2.r[3], acc0);
    product.r[1] = __vmaddfp(s1, M2.r[3], acc1);
    product.r[2] = __vmaddfp(s2, M2.r[3], acc2);
    product.r[3] = __vmaddfp(s3, M2.r[3], acc3);

    return product;
}

// Transpose by two rounds of word merges: rows 0/2 and 1/3 interleaved, then the halves of those interleaved.
XMFINLINE XMMATRIX XMMatrixTranspose(XMMATRIX M)
{
    XMMATRIX merged;
    XMMATRIX transposed;

    merged.r[0] = __vmrghw(M.r[0], M.r[2]);
    merged.r[1] = __vmrghw(M.r[1], M.r[3]);
    merged.r[2] = __vmrglw(M.r[0], M.r[2]);
    merged.r[3] = __vmrglw(M.r[1], M.r[3]);

    transposed.r[0] = __vmrghw(merged.r[0], merged.r[1]);
    transposed.r[1] = __vmrglw(merged.r[0], merged.r[1]);
    transposed.r[2] = __vmrghw(merged.r[2], merged.r[3]);
    transposed.r[3] = __vmrglw(merged.r[2], merged.r[3]);

    return transposed;
}

// Inverse by cofactors. The transpose's rows (t0..t3) are swizzled into pairs whose products give the 2x2
// sub-determinants (d0..d2), those are combined with further swizzles of t into the cofactor rows (alternating signs
// picked with a select), the determinant is the dot product of the first cofactor row with t0 (returned through
// pDeterminant), and its reciprocal is the vrefp estimate refined by two Newton-Raphson steps (kept only where the
// refinement is not NaN). Every product accumulates from -0.
XMINLINE XMMATRIX XMMatrixInverse(XMVECTOR *pDeterminant, XMMATRIX M)
{
    XMVECTOR row0, row1, row2, row3;
    XMVECTOR t0, t1, t2, t3;
    XMVECTOR lo01, lo23, hi01, hi23;
    XMVECTOR d0, d1, d2;
    XMVECTOR c0, c1, c2, c3, c4, c5, c6, c7;
    XMVECTOR a[4], b[4];
    XMVECTOR det;
    XMVECTOR recip, refined, error, valid;
    XMMATRIX inverse;
    static const XMVECTORI SwizzleXXYY = {XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0Y};
    static const XMVECTORI SwizzleZWZW = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0W};
    static const XMVECTORI SwizzleYZXY = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y};
    static const XMVECTORI SwizzleZWYZ = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static const XMVECTORI SwizzleWXWX = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static const XMVECTORI SwizzleZXYX = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0X};
    static const XMVECTORI SwizzleYWXZ = {XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Z};
    static const XMVECTORI SwizzleWZWY = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y};
    static const XMVECTORI Permute0X0Z1X1Z = {XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Z};
    static const XMVECTORI Permute0Y0W1Y1W = {XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_1W};
    static const XMVECTORI Permute1Y0Y0W0X = {XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static const XMVECTORI Permute0W0X0Y1X = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X};
    static const XMVECTORI Permute0Z1Y1X0Z = {XM_PERMUTE_0Z, XM_PERMUTE_1Y, XM_PERMUTE_1X, XM_PERMUTE_0Z};
    static const XMVECTORI Permute0W1Y0Y0Z = {XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static const XMVECTORI Permute0Z0Y1X0X = {XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0X};
    static const XMVECTORI Permute1Y0X0W1X = {XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1X};
    static const XMVECTORI Permute1W0Y0W0X = {XM_PERMUTE_1W, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static const XMVECTORI Permute0W0X0Y1Z = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1Z};
    static const XMVECTORI Permute0Z1W1Z0Z = {XM_PERMUTE_0Z, XM_PERMUTE_1W, XM_PERMUTE_1Z, XM_PERMUTE_0Z};
    static const XMVECTORI Permute0W1W0Y0Z = {XM_PERMUTE_0W, XM_PERMUTE_1W, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static const XMVECTORI Permute0Z0Y1Z0X = {XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1Z, XM_PERMUTE_0X};
    static const XMVECTORI Permute1W0X0W1Z = {XM_PERMUTE_1W, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1Z};

    XMASSERT(pDeterminant);

    // Transpose: t0..t3 are the columns of M.
    hi01 = __vmrghw(M.r[0], M.r[1]);
    hi23 = __vmrghw(M.r[2], M.r[3]);
    lo01 = __vmrglw(M.r[0], M.r[1]);
    lo23 = __vmrglw(M.r[2], M.r[3]);
    t0 = __vperm(hi01, hi23, *(XMVECTOR *)g_XMPermute0X0Y1X1Y);
    t1 = __vperm(hi01, hi23, *(XMVECTOR *)g_XMPermute0Z0W1Z1W);
    t2 = __vperm(lo01, lo23, *(XMVECTOR *)g_XMPermute0X0Y1X1Y);
    t3 = __vperm(lo01, lo23, *(XMVECTOR *)g_XMPermute0Z0W1Z1W);

    // 2x2 sub-determinants.
    a[0] = __vperm(t2, t2, *(XMVECTOR *)SwizzleXXYY);
    b[0] = __vperm(t3, t3, *(XMVECTOR *)SwizzleZWZW);
    a[1] = __vperm(t0, t0, *(XMVECTOR *)SwizzleXXYY);
    b[1] = __vperm(t1, t1, *(XMVECTOR *)SwizzleZWZW);
    a[2] = __vperm(t2, t0, *(XMVECTOR *)Permute0X0Z1X1Z);
    b[2] = __vperm(t3, t1, *(XMVECTOR *)Permute0Y0W1Y1W);
    d0 = __vmaddfp(a[0], b[0], *(XMVECTOR *)g_XMNegativeZero);
    d1 = __vmaddfp(a[1], b[1], *(XMVECTOR *)g_XMNegativeZero);
    d2 = __vmaddfp(a[2], b[2], *(XMVECTOR *)g_XMNegativeZero);

    a[0] = __vperm(t2, t2, *(XMVECTOR *)SwizzleZWZW);
    b[0] = __vperm(t3, t3, *(XMVECTOR *)SwizzleXXYY);
    a[1] = __vperm(t0, t0, *(XMVECTOR *)SwizzleZWZW);
    b[1] = __vperm(t1, t1, *(XMVECTOR *)SwizzleXXYY);
    a[2] = __vperm(t2, t0, *(XMVECTOR *)Permute0Y0W1Y1W);
    b[2] = __vperm(t3, t1, *(XMVECTOR *)Permute0X0Z1X1Z);
    d0 = __vnmsubfp(a[0], b[0], d0);
    d1 = __vnmsubfp(a[1], b[1], d1);
    d2 = __vnmsubfp(a[2], b[2], d2);

    // Cofactors, first term.
    a[0] = __vperm(t1, t1, *(XMVECTOR *)SwizzleYZXY);
    b[0] = __vperm(d0, d2, *(XMVECTOR *)Permute1Y0Y0W0X);
    a[1] = __vperm(t0, t0, *(XMVECTOR *)SwizzleZXYX);
    b[1] = __vperm(d0, d2, *(XMVECTOR *)Permute0W1Y0Y0Z);
    a[2] = __vperm(t3, t3, *(XMVECTOR *)SwizzleYZXY);
    b[2] = __vperm(d1, d2, *(XMVECTOR *)Permute1W0Y0W0X);
    a[3] = __vperm(t2, t2, *(XMVECTOR *)SwizzleZXYX);
    b[3] = __vperm(d1, d2, *(XMVECTOR *)Permute0W1W0Y0Z);
    c0 = __vmaddfp(a[0], b[0], *(XMVECTOR *)g_XMNegativeZero);
    c2 = __vmaddfp(a[1], b[1], *(XMVECTOR *)g_XMNegativeZero);
    c4 = __vmaddfp(a[2], b[2], *(XMVECTOR *)g_XMNegativeZero);
    c6 = __vmaddfp(a[3], b[3], *(XMVECTOR *)g_XMNegativeZero);

    // Second term (subtracted).
    a[0] = __vperm(t1, t1, *(XMVECTOR *)SwizzleZWYZ);
    b[0] = __vperm(d0, d2, *(XMVECTOR *)Permute0W0X0Y1X);
    a[1] = __vperm(t0, t0, *(XMVECTOR *)SwizzleWZWY);
    b[1] = __vperm(d0, d2, *(XMVECTOR *)Permute0Z0Y1X0X);
    a[2] = __vperm(t3, t3, *(XMVECTOR *)SwizzleZWYZ);
    b[2] = __vperm(d1, d2, *(XMVECTOR *)Permute0W0X0Y1Z);
    a[3] = __vperm(t2, t2, *(XMVECTOR *)SwizzleWZWY);
    b[3] = __vperm(d1, d2, *(XMVECTOR *)Permute0Z0Y1Z0X);
    c0 = __vnmsubfp(a[0], b[0], c0);
    c2 = __vnmsubfp(a[1], b[1], c2);
    c4 = __vnmsubfp(a[2], b[2], c4);
    c6 = __vnmsubfp(a[3], b[3], c6);

    // Third term, both signs; the odd elements take the other sign.
    a[0] = __vperm(t1, t1, *(XMVECTOR *)SwizzleWXWX);
    b[0] = __vperm(d0, d2, *(XMVECTOR *)Permute0Z1Y1X0Z);
    a[1] = __vperm(t0, t0, *(XMVECTOR *)SwizzleYWXZ);
    b[1] = __vperm(d0, d2, *(XMVECTOR *)Permute1Y0X0W1X);
    a[2] = __vperm(t3, t3, *(XMVECTOR *)SwizzleWXWX);
    b[2] = __vperm(d1, d2, *(XMVECTOR *)Permute0Z1W1Z0Z);
    a[3] = __vperm(t2, t2, *(XMVECTOR *)SwizzleYWXZ);
    b[3] = __vperm(d1, d2, *(XMVECTOR *)Permute1W0X0W1Z);
    c1 = __vnmsubfp(a[0], b[0], c0);
    c0 = __vmaddfp(a[0], b[0], c0);
    c3 = __vmaddfp(a[1], b[1], c2);
    c2 = __vnmsubfp(a[1], b[1], c2);
    c5 = __vnmsubfp(a[2], b[2], c4);
    c4 = __vmaddfp(a[2], b[2], c4);
    c7 = __vmaddfp(a[3], b[3], c6);
    c6 = __vnmsubfp(a[3], b[3], c6);

    row0 = __vsel(c0, c1, *(XMVECTOR *)g_XMSelect0101);
    row1 = __vsel(c2, c3, *(XMVECTOR *)g_XMSelect0101);
    row2 = __vsel(c4, c5, *(XMVECTOR *)g_XMSelect0101);
    row3 = __vsel(c6, c7, *(XMVECTOR *)g_XMSelect0101);

    // Determinant, summed across the lanes by rotating 8 then 4 bytes.
    d0 = __vmaddfp(row0, t0, *(XMVECTOR *)g_XMNegativeZero);
    d1 = __vsldoi(d0, d0, 8);
    d0 = __vaddfp(d0, d1);
    d1 = __vsldoi(d0, d0, 4);
    det = __vaddfp(d0, d1);

    recip = __vrefp(det);

    *pDeterminant = det;

    error = __vnmsubfp(det, recip, g_XMOne);
    refined = __vmaddfp(recip, error, recip);
    error = __vnmsubfp(det, refined, g_XMOne);
    valid = __vcmpeqfp(refined, refined);
    refined = __vmaddfp(refined, error, refined);
    recip = __vsel(recip, refined, valid);

    inverse.r[0] = __vmaddfp(row0, recip, *(XMVECTOR *)g_XMNegativeZero);
    inverse.r[1] = __vmaddfp(row1, recip, *(XMVECTOR *)g_XMNegativeZero);
    inverse.r[2] = __vmaddfp(row2, recip, *(XMVECTOR *)g_XMNegativeZero);
    inverse.r[3] = __vmaddfp(row3, recip, *(XMVECTOR *)g_XMNegativeZero);

    return inverse;
}

XMFINLINE XMMATRIX XMMatrixIdentity()
{
    XMMATRIX I;
    I.r[0] = g_XMIdentityR0;
    I.r[1] = g_XMIdentityR1;
    I.r[2] = g_XMIdentityR2;
    I.r[3] = g_XMIdentityR3;
    return I;
}

#ifdef __cplusplus

XMFINLINE _XMMATRIX &_XMMATRIX::operator=(const _XMMATRIX &M)
{
    r[0] = M.r[0];
    r[1] = M.r[1];
    r[2] = M.r[2];
    r[3] = M.r[3];
    return *this;
}

#endif

#endif
