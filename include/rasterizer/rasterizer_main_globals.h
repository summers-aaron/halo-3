#ifndef RASTERIZER_RASTERIZER_MAIN_GLOBALS_H
#define RASTERIZER_RASTERIZER_MAIN_GLOBALS_H

#include "math/real_math_constants.h"

// Globals owned by rasterizer_main.cpp, shared with device-state users.
struct D3DDevice;
struct D3DPixelShader;
struct D3DVertexShader;
class c_interlocked_long;

extern c_interlocked_long g_thread_owning_device;

struct rasterizer_main_shader_cache
{
    D3DPixelShader *pixel_shaders[8];
    D3DVertexShader *vertex_shaders[8];
};

typedef char rasterizer_main_shader_cache_size[(sizeof(rasterizer_main_shader_cache) == 16 * sizeof(void *)) ? 1 : -1];

extern "C" rasterizer_main_shader_cache g_rasterizer_shared_shader_cache;

struct D3DTexture;
extern "C" void rasterizer_main_create_atlas_texture(char *base_address, long index, D3DTexture **out_texture);
struct s_render_postprocess_color_tweaking;
extern "C" s_render_postprocess_color_tweaking g_render_postprocess_color_tweaking;

struct s_rasterizer_main_declaration;
struct s_rasterizer_main_declaration_element;
struct rasterizer_main_memory_globals;
struct rasterizer_main_pair { long first, second; };

struct s_light_volume_gpu_target_table;

extern "C" {
extern rasterizer_main_pair g_rasterizer_main_atmosphere_state;
extern unsigned long k_rasterizer_vertex_shader_1[], k_rasterizer_vertex_shader_2[], k_rasterizer_vertex_shader_3[], k_rasterizer_vertex_shader_4[];
extern unsigned long k_rasterizer_vertex_shader_5[], k_rasterizer_vertex_shader_6[], k_rasterizer_vertex_shader_7[], k_rasterizer_pixel_shader_0[];
extern unsigned long k_rasterizer_pixel_shader_1[], k_rasterizer_pixel_shader_2[], k_rasterizer_pixel_shader_3[], k_rasterizer_pixel_shader_4[];
extern unsigned long k_rasterizer_pixel_shader_5[], k_rasterizer_pixel_shader_6[], k_rasterizer_pixel_shader_7[];
extern s_light_volume_gpu_target_table g_light_volume_gpu_target_table[];
#include "memory/shared_resource_allocator.h"
}

extern "C" D3DDevice *g_rasterizer_device;

#endif
