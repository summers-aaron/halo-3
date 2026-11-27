#ifndef RASTERIZER_RASTERIZER_SHADER_PARAMETER_CALLS_H
#define RASTERIZER_RASTERIZER_SHADER_PARAMETER_CALLS_H

union real_vector4d;

#if !defined(_XBOX) || defined(__clang__)
extern "C" void render_method_submit_get_extern_constant(long index, real_vector4d *value);
#else
extern "C" long render_method_submit_get_extern_constant(long index, real_vector4d *value);
#endif

#endif // RASTERIZER_RASTERIZER_SHADER_PARAMETER_CALLS_H
