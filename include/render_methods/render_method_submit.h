#ifndef RENDER_METHODS_RENDER_METHOD_SUBMIT_H
#define RENDER_METHODS_RENDER_METHOD_SUBMIT_H

#include "rasterizer/rasterizer_vertex_types.h"
#include "render_methods/render_method_definitions.h"

// Functions defined in tu/graphicslibxbox/render_method_submit.cpp.

struct s_render_method_animated_parameter;

bool render_method_submit_set_texture(long sampler, long bitmap_group_index, long bitmap_index);
void render_method_submit_valid();
float evaluate_function(long p0, const s_render_method_animated_parameter *ap);
extern "C" void render_method_submit_reset_cached_state();

class c_render_method;

extern "C" void render_method_submit_update_externs(const c_render_method *render_method, long entry_point, long type, unsigned char force);

class c_rasterizer_xenon_memory;
union real_vector4d;
struct geometry_material;
struct s_part;
struct s_render_geometry;
struct s_rasterizer_entry;
struct s_mesh;

extern "C" s_rasterizer_entry **rasterizer_xenon_memory_find_entry(c_rasterizer_xenon_memory *memory, unsigned __int64 key, unsigned int type);
extern "C" bool render_method_submit_test_geometry_keys(const unsigned __int64 *keys, int count);
extern "C" bool render_method_submit_try_submit_cached(c_rasterizer_xenon_memory *memory, unsigned __int64 p0, const s_render_geometry *geometry,
    const s_mesh *mesh, const s_part *part, int mode, const geometry_material *materials);
extern "C" void render_method_submit_set_cached_state(c_rasterizer_xenon_memory *memory, s_rasterizer_entry *cached, long context,
    const c_render_method *render_method, const s_mesh *mesh, long entry_point);
extern "C" bool render_method_submit_update_animated_state(long context, const c_render_method *render_method, long entry_point,
    bool force, bool set_sampler_state);
extern "C" void render_method_submit_get_extern_constant(long index, real_vector4d *value);
extern "C" unsigned char render_method_submit_build_cached_entry(c_rasterizer_xenon_memory *memory, s_rasterizer_entry *cached,
    const c_render_method *render_method, long tag_index, const s_render_geometry *geometry, const s_mesh *part, const s_part *range, long mode);
bool render_method_submit(long index, class c_render_method const *render_method, enum e_vertex_type vertex_type,
    enum e_transfer_vector_vertex_types transfer_type, enum e_entry_point entry_point, bool force);
bool set_pixel_and_vertex_shaders(const c_render_method *render_method, e_vertex_type vertex_type, e_transfer_vector_vertex_types transfer_type,
    e_entry_point entry_point);

extern "C" void render_method_submit_set_extern_info(long structure_bsp_index, void *shader_extern_info);

struct D3DDevice;
struct s_rasterizer_xenon_memory_entry;
extern "C" bool render_method_submit_set_dynamic_texture(D3DDevice *device, long sampler, long texture_type);
extern "C" void render_method_submit_set_extern_sampler(D3DDevice *device, s_rasterizer_xenon_memory_entry *memory_entry, long index, long sampler);

extern "C" bool render_method_submit_set_shader_state(const c_render_method *render_method, int vertex_type, int transfer_type, int entry_point, bool force);

#endif // RENDER_METHODS_RENDER_METHOD_SUBMIT_H
