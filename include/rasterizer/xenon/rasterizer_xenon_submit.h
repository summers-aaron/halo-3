#ifndef RASTERIZER_XENON_RASTERIZER_XENON_SUBMIT_H
#define RASTERIZER_XENON_RASTERIZER_XENON_SUBMIT_H

struct D3DDevice;
struct D3DVertexShader;
struct D3DPixelShader;
struct D3DVertexDeclaration;

extern "C" {
void rasterizer_set_alpha_test_enabled(unsigned char enable, int value);
}

#endif
