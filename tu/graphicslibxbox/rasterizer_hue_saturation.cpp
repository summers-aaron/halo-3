// graphicslibxbox/rasterizer_hue_saturation.obj: from the records 0x1849e8 0x184e60
// @flags /TP /O2 /Oc
#include <xtl.h>
#include "cseries/crt_xdk.h"
#include "math/real_math_types.h"
#include <xboxmath.h>
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_hue_saturation.h"

void xrotatemat(XMMATRIX *m, float sine, float cosine)
{
    m->_11=1.0f; m->_12=0.0f; m->_13=0.0f; m->_14=0.0f;
    m->_21=0.0f; m->_22=cosine; m->_23=sine; m->_24=0.0f;
    m->_31=0.0f; m->_32=-sine; m->_33=cosine; m->_34=0.0f;
    m->_41=0.0f; m->_42=0.0f; m->_43=0.0f; m->_44=1.0f;
}
void yrotatemat(XMMATRIX *m, float sine, float cosine)
{
    m->_11=cosine; m->_12=0.0f; m->_13=-sine; m->_14=0.0f;
    m->_21=0.0f; m->_22=1.0f; m->_23=0.0f; m->_24=0.0f;
    m->_31=sine; m->_32=0.0f; m->_33=cosine; m->_34=0.0f;
    m->_41=0.0f; m->_42=0.0f; m->_43=0.0f; m->_44=1.0f;
}
void zrotatemat(XMMATRIX *m, float sine, float cosine)
{
    m->_11=cosine; m->_12=sine; m->_13=0.0f; m->_14=0.0f;
    m->_21=-sine; m->_22=cosine; m->_23=0.0f; m->_24=0.0f;
    m->_31=0.0f; m->_32=0.0f; m->_33=1.0f; m->_34=0.0f;
    m->_41=0.0f; m->_42=0.0f; m->_43=0.0f; m->_44=1.0f;
}
void zshearmat(XMMATRIX *m, float x, float y)
{
    m->_11=1.0f; m->_12=0.0f; m->_13=x; m->_14=0.0f;
    m->_21=0.0f; m->_22=1.0f; m->_23=y; m->_24=0.0f;
    m->_31=0.0f; m->_32=0.0f; m->_33=1.0f; m->_34=0.0f;
    m->_41=0.0f; m->_42=0.0f; m->_43=0.0f; m->_44=1.0f;
}
void xformpnt(const XMMATRIX *m, float x, float y, float z, float *out_x, float *out_y, float *out_z)
{
    *out_x = m->_11*x + m->_21*y + m->_31*z + m->_41;
    *out_y = m->_12*x + m->_22*y + m->_32*z + m->_42;
    *out_z = m->_13*x + m->_23*y + m->_33*z + m->_43;
}

static __forceinline XMMATRIX rasterizer_hue_multiply(XMMATRIX a, XMMATRIX b)
{
    XMMATRIX p, t, result;
    p.r[0]=__vmrghw(b.r[0], b.r[2]); p.r[1]=__vmrghw(b.r[1], b.r[3]);
    p.r[2]=__vmrglw(b.r[0], b.r[2]); p.r[3]=__vmrglw(b.r[1], b.r[3]);
    t.r[0]=__vmrghw(p.r[0], p.r[1]); t.r[2]=__vmrghw(p.r[2], p.r[3]);
    t.r[1]=__vmrglw(p.r[0], p.r[1]); t.r[3]=__vmrglw(p.r[2], p.r[3]);
    XMVECTOR d00=__vmsum4fp(a.r[0],t.r[0]);
    XMVECTOR d02=__vmsum4fp(a.r[0],t.r[2]);
    XMVECTOR d01=__vmsum4fp(a.r[0],t.r[1]);
    XMVECTOR d03=__vmsum4fp(a.r[0],t.r[3]);
    XMVECTOR d10=__vmsum4fp(a.r[1],t.r[0]);
    XMVECTOR d12=__vmsum4fp(a.r[1],t.r[2]);
    XMVECTOR d11=__vmsum4fp(a.r[1],t.r[1]);
    XMVECTOR d13=__vmsum4fp(a.r[1],t.r[3]);
    XMVECTOR d20=__vmsum4fp(a.r[2],t.r[0]);
    XMVECTOR d22=__vmsum4fp(a.r[2],t.r[2]);
    XMVECTOR d21=__vmsum4fp(a.r[2],t.r[1]);
    XMVECTOR d23=__vmsum4fp(a.r[2],t.r[3]);
    XMVECTOR d30=__vmsum4fp(a.r[3],t.r[0]);
    XMVECTOR d32=__vmsum4fp(a.r[3],t.r[2]);
    XMVECTOR d31=__vmsum4fp(a.r[3],t.r[1]);
    XMVECTOR d33=__vmsum4fp(a.r[3],t.r[3]);
    result.r[0]=__vmrghw(__vmrghw(d00,d02),__vmrghw(d01,d03));
    result.r[1]=__vmrghw(__vmrghw(d10,d12),__vmrghw(d11,d13));
    result.r[2]=__vmrghw(__vmrghw(d20,d22),__vmrghw(d21,d23));
    result.r[3]=__vmrghw(__vmrghw(d30,d32),__vmrghw(d31,d33));
    return result;
}

// @address 0x184e60 ?build_hue_matrix@@YAXMPAU_XMMATRIX@@@Z
void build_hue_matrix(float angle, _XMMATRIX *result)
{
    XMMATRIX temporary, matrix;
    float root = sqrtf(2.0f);
    float x_sine = (float)(1.0 / root);
    float x_cosine = (float)(1.0 / root);
    xrotatemat(&matrix, x_sine, x_cosine);
    root = (float)sqrt(3.0);
    float y_sine = (float)(-1.0 / root);
    float y_cosine = (float)(sqrt(2.0) / root);
    yrotatemat(&temporary, y_sine, y_cosine);
    matrix = rasterizer_hue_multiply(matrix, temporary);
    float x, y, z;
    xformpnt(&matrix, 0.212656f, 0.715158f, 0.0721856f, &x, &y, &z);
    float shear_x = x/z;
    float shear_y = y/z;
    zshearmat(&temporary, shear_x, shear_y);
    matrix = rasterizer_hue_multiply(matrix, temporary);
    float sine = sin(angle * 0.017453292f);
    float cosine = cos(angle * 0.017453292f);
    zrotatemat(&temporary, sine, cosine);
    matrix = rasterizer_hue_multiply(matrix, temporary);
    zshearmat(&temporary, -shear_x, -shear_y);
    matrix = rasterizer_hue_multiply(matrix, temporary);
    yrotatemat(&temporary, -y_sine, y_cosine);
    matrix = rasterizer_hue_multiply(matrix, temporary);
    xrotatemat(&temporary, -x_sine, x_cosine);
    matrix = rasterizer_hue_multiply(matrix, temporary);
    *result = matrix;
}

#define RASTERIZER_HUE_SATURATION_PIN(v, lo, hi) ((((v) > (lo)) ? (v) : (lo)) < (hi) ? (((v) > (lo)) ? (v) : (lo)) : (hi))


static __forceinline void build_saturation_matrix(float saturation, XMMATRIX *matrix)
{
    float red_red = (float)((1.0 - saturation) * 0.212656f + saturation);
    float red_green = (float)((1.0 - saturation) * 0.212656f);
    float red_blue = (float)((1.0 - saturation) * 0.212656f);
    float green_red = (float)((1.0 - saturation) * 0.715158f);
    float green_green = (float)((1.0 - saturation) * 0.715158f + saturation);
    float green_blue = (float)((1.0 - saturation) * 0.715158f);
    float blue_red = (float)((1.0 - saturation) * 0.0721856f);
    float blue_green = (float)((1.0 - saturation) * 0.0721856f);
    float blue_blue = (float)((1.0 - saturation) * 0.0721856f + saturation);
    matrix->_11 = red_red; matrix->_12 = red_green; matrix->_13 = red_blue; matrix->_14 = 0.0f;
    matrix->_21 = green_red; matrix->_22 = green_green; matrix->_23 = green_blue; matrix->_24 = 0.0f;
    matrix->_31 = blue_red; matrix->_32 = blue_green; matrix->_33 = blue_blue; matrix->_34 = 0.0f;
    matrix->_41 = 0.0f; matrix->_42 = 0.0f; matrix->_43 = 0.0f; matrix->_44 = 1.0f;
}

// @address 0x1849e8 rasterizer_set_hue_saturation_constants
extern "C" void rasterizer_set_hue_saturation_constants(
    const s_render_postprocess_color_tweaking *parameters, float hue_delta, float saturation_delta,
    float brightness, const real_rgb_color *filter, const real_rgb_color *offset)
{
    float hue = (float)fmod((double)(parameters->hue_shift + hue_delta), 360.0);
    if (hue < 0.0f)
        hue += 360.0f;
    float saturation = RASTERIZER_HUE_SATURATION_PIN(parameters->saturation + saturation_delta, -10.0f, 10.0f);
    float red = filter->red * RASTERIZER_HUE_SATURATION_PIN(parameters->color_filter.red, 0.0f, 1.0f);
    float green = filter->green * RASTERIZER_HUE_SATURATION_PIN(parameters->color_filter.green, 0.0f, 1.0f);
    float blue = filter->blue * RASTERIZER_HUE_SATURATION_PIN(parameters->color_filter.blue, 0.0f, 1.0f);
    float brightness_squared = brightness * brightness;
    float scale = brightness_squared * 10.0f + 1.0f;
    XMMATRIX filter_matrix;
    filter_matrix = XMMatrixIdentity();
    filter_matrix._11 = scale * red;
    filter_matrix._22 = scale * green;
    filter_matrix._33 = blue * scale;
    filter_matrix._41 = -offset->red - brightness_squared;
    filter_matrix._42 = -offset->green - brightness_squared;
    filter_matrix._43 = -offset->blue - brightness_squared;
    XMMATRIX hue_matrix;
    build_hue_matrix(hue, &hue_matrix);
    XMMATRIX saturation_matrix;
    build_saturation_matrix(saturation, &saturation_matrix);
    XMMATRIX matrix = XMMatrixTranspose(rasterizer_hue_multiply(rasterizer_hue_multiply(saturation_matrix, filter_matrix), hue_matrix));
    c_rasterizer::get_device()->SetPixelShaderConstantF(229, &matrix._11, 3);
}

// @address 0x73a194 g_default_hue_saturation_parameters
s_render_postprocess_color_tweaking g_default_hue_saturation_parameters = { 0.0f, 1.0f, { 1.0f, 1.0f, 1.0f } };
// @address 0x10f5c80 g_render_postprocess_color_tweaking
s_render_postprocess_color_tweaking g_render_postprocess_color_tweaking = g_default_hue_saturation_parameters;

