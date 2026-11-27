// graphicslibxbox/render_water.obj: from the records 0x165ca8 0x3306d0 0x3306e0 0x330728 0x330740 0x330758 0x330868 0x330908 0x4491c8 0x44a9e8 0x44ac48 0x44ace8 0x44ad40 0x44ad78 0x44ae08 0x44b5c0 0x44b7c0 0x44ba60 0x44bd40 0x44bf08 0x44c170 0x44c3a8 0x44c8f0 0x44c990 0x44c9a8 0x44c9c8
// @address 0x81dab0 g_rasterizer_current_z_buffer_mode
// @address 0x8c4694 g_rasterizer_device
// @address 0x81dab8 g_rasterizer_alpha_blend_mode
// @address 0x7e7cb0 g_water_composite_shading_constants
// @address 0x11df7e0 g_water_interaction_events
// @address 0x11df7b0 g_water_interaction_data_address
// @address 0x32938 k_real_constants_minus_one
// @compiler xdk6021
// @address 0x5a42f8 D3DArrayTexture_GetArraySize
// @flags /TP /O2 /Oc

union real_vector4d;
struct D3DDevice;
struct D3DVertexBuffer;


long g_water_interaction_event_count = 0;

#include "geometry/geometry_definitions.h"
#include "cseries/pointer_utilities.h"
#include "render/water_interaction_tls.h"
#include "math/real_math_constant_pool.h"
#include "rasterizer/rasterizer_depth_bias_globals.h"
#include "render/render_camera_types.h"
#include "render/render_water_xenon.h"
#include "render/render_geometry.h"
#include "render/render.h"
#include "game/materials.h"
#include "memory/tls_globals.h"
#include "memory/datum_index.h"
#include "rasterizer/xenon/rasterizer_xenon_d3d.h"
#include "rasterizer/rasterizer_globals.h"
#include "render/render_water_globals.h"
#include "render/render_water_runtime_globals.h"
#include "render_methods/render_method_submit_globals.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "math/real_math_types.h"
#include "rasterizer/xenon/rasterizer_buffer_resource_types.h"
#include <string.h>
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include <stddef.h>
#include "tag_files/tag_groups.h"
#include "objects/object_reference_types.h"
#include "cseries/cseries.h"
#include "game/game_time_types.h"
#include "game/game_globals.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "cache/restricted_memory_regions.h"
#include "main/main.h"
#include "render/render_water.h"
#include "render_methods/render_method_submit.h"
#include "physics/water_physics.h"
#include "render/render_cameras.h"
#include "rasterizer/rasterizer_states.h"
#include "render/render_structure.h"
#include "rasterizer/rasterizer_index_buffer.h"
#include "scenario/scenario_tag.h"
#include "render/dynamic_vertex_buffer_types.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "cache/restricted_memory.h"
#include "render_methods/render_method.h"
#include "game/game_time_inline.h"
#include "render/render_water_allocators.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "render/water_renderer.h"
#include "rasterizer/xenon/rasterizer_xenon_resource.h"
#include "main/game_tick_time_samples.h"
#include "rasterizer/rasterizer_sampler_texture_calls.h"
#include "render_methods/render_method_data.h"
#include "cache/restricted_memory_layout.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_registration.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include "math/xenon_matrix_types.h"
#include "gpu_particle/common_gpu.h"
#include "geometry/geometry_material_types.h"
#include "geometry/render_geometry_part_types.h"
#include "geometry/render_geometry_mesh_types.h"
#include "geometry/render_geometry_types.h"
#include "cubemaps/cubemap_sample.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "render/render_structure_globals_types.h"
#include "structures/structure_bsp_tag.h"
#include "render/views/player_view_types.h"
#include "rasterizer/xenon/rasterizer_device_types.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "render/structure_renderer.h"
#include "rasterizer/xenon/xenon_matrix.h"
#include "sky_atm/sky_atm_parameters_types.h"

real_vector4d g_water_composite_shading_constants;

__declspec(thread) s_water_interaction_data *g_water_interaction_data_address;
__declspec(thread) s_water_interaction_event *g_water_interaction_events;

#pragma bitfield_order(push)
#pragma bitfield_order(lsb_to_msb)
union water_renderer_memexport_stream_constant
{
    struct {
        unsigned long BaseAddress : 30;
        unsigned long _01 : 2;
        unsigned long _4B000000 : 32;
        unsigned long EndianSwap : 3;
        unsigned long : 5;
        unsigned long Format : 6;
        unsigned long : 2;
        unsigned long NumericType : 3;
        unsigned long ComponentSwap : 1;
        unsigned long _4B0 : 12;
        unsigned long StreamMaxIndex : 23;
        unsigned long _96 : 9;
    };
    unsigned long dword[4];
    float c[4];
    real_vector4d vector;
};
#pragma bitfield_order(pop)

struct s_water_ripple_globals
{
    c_rasterizer_vertex_buffer vertex_buffer;
};

struct s_water_ripple_apply_constants
{
    union {
        real_vector4d shading_constants;
        struct {
            real_vector3d shading_direction;
            float ripple_displacement_i;
        };
    };
};

enum e_cull_mode {
    _cull_mode_none = 0,
    _cull_mode_back = 2,
};

enum e_z_buffer_mode {
    _z_buffer_mode_normal = 0,
    _z_buffer_mode_read_only,
    _z_buffer_mode_off,
};

struct s_water_float2
{
    float x, y;
    __forceinline s_water_float2() {}
    s_water_float2(float x_value, float y_value);
    s_water_float2 &operator=(const s_water_float2 &other);
};

struct s_water_ripple_vertex
{
    s_water_float2 position;              // pos_flow
    s_water_float2 flow;
    float life;                         // life_height
    float duration;
    float rise_period;
    float height;
    s_water_float2 shock;                // shock_spread
    float size;
    float spread;
    float pendulum_phase;               // pendulum
    float pendulum_revolution;
    float pendulum_repeat;
    float pendulum_unused;
    float pattern_start_index;          // pattern
    float pattern_end_index;
    float pattern_unused[2];
    float foam_out_radius;              // foam
    float foam_fade_distance;
    float foam_life;
    float foam_duration;
    float flag_drift;                   // flags
    float flag_pendulum;
    float flag_foam;
    float flag_foam_game_unit;
    float func_rise;                    // funcs
    float func_descend;
    float func_pattern;
    float func_foam;
};

struct s_water_extern_parameter {
    long type;
    long register_index;
    long register_count;
    unsigned long flags;
    const char *name;
};

struct s_water_extern_block
{
    s_water_extern_parameter parameters[49];
    float degrees_to_radians_tail;
};

struct s_water_pool_prefix
{
    unsigned long values[7];
};

union s_water_pool_suffix
{
    unsigned int words[4];
    real_vector4d shader_constant;
};

#if defined(_XBOX) && !defined(__clang__)
union s_water_ripple_register
{
    struct
    {
        float radius;
        unsigned char adjacent_literals[3 * sizeof(float)];
    } pool;
    real_vector4d shader_constant;
};
typedef char water_ripple_register_size[(sizeof(s_water_ripple_register) == sizeof(real_vector4d)) ? 1 : -1];
typedef char water_ripple_literals_offset[(offsetof(s_water_ripple_register, pool.adjacent_literals) == sizeof(float)) ? 1 : -1];
#endif

union water_texture_size_constant
{
    unsigned __int64 integer;
    double integer_bits;
    real_vector4d value;
};

struct s_water_ripple_definition
{
    unsigned long flags;                // drift, pendulum, foam, foam game unit
    float size;
    float height;
    float spread;
    float velocity_bias;
    float position_random_range;
    float duration_maximum;
    float duration_minimum;
    float rise_period;
    short rise_function;
    short descend_function;
    float pendulum_revolution;
    float pendulum_repeat;
    float pattern_start_index;
    float pattern_end_index;
    short pattern_function;
    float foam_out_radius;
    float foam_fade_distance;
    float foam_duration;
    short foam_function;
};

enum
{
    _water_ripple_drift_bit = 0,
    _water_ripple_pendulum_bit,
    _water_ripple_foam_bit,
    _water_ripple_foam_game_unit_bit,
};


typedef unsigned int uint;
typedef char water_ripple_definition_size[(sizeof(s_water_ripple_definition) == 0x4c) ? 1 : -1];
typedef char water_ripple_foam_radius_offset[(offsetof(s_water_ripple_definition, foam_out_radius) == 0x3c) ? 1 : -1];

void __tls_set_g_water_interaction_event_allocator(void *address)
{
    g_water_interaction_data_address = (s_water_interaction_data *)address;
}

// @address 0x3306e0 ?water_interaction_ripples_initialize@@YAXXZ
void water_interaction_ripples_initialize(void)
{
    g_water_interaction_data_address = (s_water_interaction_data *)g_water_interaction_event_allocator.reserve_memory(
        "object schedule globals", 0, sizeof(s_water_interaction_data), 0);
}

extern "C" {
// @address 0x58e5c0 memset
}

// @address 0x330728 ?water_interaction_ripples_clear@@YAXXZ
void water_interaction_ripples_clear(void)
{
    memset(g_water_interaction_data_address, 0, sizeof(*g_water_interaction_data_address));
}

// @address 0x330740 ?water_interaction_ripples_dispose@@YAXXZ
void water_interaction_ripples_dispose(void)
{
    g_water_interaction_data_address = 0;
}

// @address 0x330758 water_interaction_ripples_find_best_offset
extern "C" int water_interaction_ripples_find_best_offset(int current_offset, int offset_count, float reuse_bias)
{
    int candidate_offset;
    int stride;
    float sample_value;
    float average_score;
    int sample_count;
    int best_offset;
    float best_score;
    int start_offset;
    int trial_index;
    float total_score;
    int sample_index;
    const float *sample;
    float bias;
    start_offset = 0;
    sample = 0;
    best_offset = -1;
    best_score = 3.4028234663852886e+38f;
    if (current_offset != -1)
        start_offset = current_offset;
    else
        start_offset = offset_count - (tls_game_time_globals()->time % offset_count);
    trial_index = 0;
    if (offset_count > 0)
    {
        do
        {
            total_score = 0.0f;
            sample_count = 0;
            candidate_offset = (trial_index + start_offset) % offset_count;
            sample_index = candidate_offset;
            if (candidate_offset < 30)
            {
                stride = offset_count;
                sample = &g_water_interaction_data_address->ripple_amplitudes[candidate_offset];
                do
                {
                    if (candidate_offset == current_offset)
                        bias = reuse_bias;
                    else
                        bias = 0.0f;
                    sample_value = *sample;
                    sample_index += offset_count;
                    ++sample_count;
                    total_score = (sample_value - bias) + total_score;
                    sample += stride;
                } while (sample_index < 30);
                if (sample_count != 0)
                {
                    average_score = total_score / (float)((double)sample_count);
                    if (!(average_score >= best_score))
                    {
                        best_offset = candidate_offset;
                        best_score = average_score;
                    }
                }
            }
            ++trial_index;
        } while (trial_index < offset_count);
    }
    return best_offset;
}

static __forceinline void water_interaction_events_dispose(void)
{
    if (g_water_interaction_events) {
        for (unsigned int event_index = 0; event_index < 128; ++event_index) {
            s_water_interaction_event *event = &g_water_interaction_events[event_index];
            event->type = -1;
        }
    }
#if !defined(_XBOX) || defined(__clang__)
    g_water_interaction_event_count = 0;
#endif
}

struct s_render_geometry;
struct geometry_material;

void c_water_renderer::dispose_from_old_map()
{
    water_interaction_events_dispose();
}

// @address 0x82553c g_water_camera_position
static real_point3d g_water_camera_position;
// @address 0x825548 g_water_cluster_part_indices
#if defined(_XBOX) && !defined(__clang__)
static long g_water_cluster_part_indices[512];
#else
long g_water_render_structure_part_indices[512];
#endif
// @address 0x825d48 g_water_ripple_event_counts
static long g_water_ripple_event_counts[2];
#if defined(_XBOX) && !defined(__clang__)
#define WATER_RIPPLE_DEPTH_BIAS g_water_ripple_render_state.depth_bias
#define WATER_RIPPLE_WRITE_INDEX g_water_ripple_render_state.ripple_index
#define WATER_RIPPLE_BUFFER_INDEX g_water_ripple_render_state.ripple_buffer_index
#else
static long g_water_ripple_buffer_index;
#define WATER_RIPPLE_DEPTH_BIAS g_rasterizer_depth_bias
#define WATER_RIPPLE_WRITE_INDEX g_water_ripple_write_index
#define WATER_RIPPLE_BUFFER_INDEX g_water_ripple_buffer_index
#endif
// @address 0x825d50 g_water_ripple_event_vertex_buffers
static c_rasterizer_vertex_buffer g_water_ripple_event_vertex_buffers[2];
// @address 0x825d60 g_water_ripple_globals
static s_water_ripple_globals g_water_ripple_globals;
// @address 0x825d68 g_water_ripple_apply_constants
static s_water_ripple_apply_constants g_water_ripple_apply_constants;
// @address 0x825d78 g_water_ripple_view_forward_dot
static float g_water_ripple_view_forward_dot;
// @address 0x825d7c g_water_underwater_color
static real_rgb_color g_water_underwater_color;
// @address 0x825d88 g_water_underwater_murkiness
static real_vector4d g_water_underwater_murkiness;

// @address 0x44ac48 ?dispose@c_water_renderer@@SAXXZ
void c_water_renderer::dispose()
{
    water_interaction_events_dispose();

    if (g_water_ripple_globals.vertex_buffer.m_d3d_buffer)
    {
        D3DResource_Release(g_water_ripple_globals.vertex_buffer.m_d3d_buffer);
    }

    unsigned long count = 2;
    c_rasterizer_vertex_buffer *buffer = &g_water_ripple_event_vertex_buffers[0];
    do
    {
        if (buffer->m_d3d_buffer)
        {
            D3DResource_Release(buffer->m_d3d_buffer);
        }
        buffer++;
    } while (--count != 0);
}

extern "C" {
// @address 0x73a184 g_water_interaction_last_time
long g_water_interaction_last_time = -1;

// @address 0x81dc64 g_water_part_count
// @address 0x8c46f0 g_water_ripple_write_index
int g_water_ripple_write_index;
}

// @address 0x44ace8 ?initialize_for_new_map@c_water_renderer@@SAXXZ
void c_water_renderer::initialize_for_new_map()
{
    water_interaction_events_dispose();
    g_water_part_count = 0;
    g_water_ripple_write_index = 0;
#if !defined(_XBOX) || defined(__clang__)
    g_water_ripple_buffer_index = 0;
#endif
    g_water_interaction_last_time = -1;
}

void c_water_renderer::game_interation_event_add(long type, const real_point3d *position,
    const real_vector3d *direction, const real_vector3d *velocity)
{
    if ((unsigned long)g_water_interaction_event_count < 128) {
        s_water_interaction_event *event = &g_water_interaction_events[g_water_interaction_event_count];
        ++g_water_interaction_event_count;
        event->type = type;
        event->position = *position;
        event->direction = *direction;
        event->velocity = *velocity;
    }
}


extern "C" {
// @address 0x119a49c g_water_ripple_update_buffer_address
void *g_water_ripple_update_buffer_address;
}


static __forceinline unsigned long water_renderer_cpu_to_gpu_address(uintptr_t address)
{
    return (unsigned long)((address & 0x1fffffff) +
        (((address >> 20) + 0x200) & 0x1000));
}

static __forceinline void water_renderer_set_memexport_stream_constant(
    water_renderer_memexport_stream_constant *constant, uintptr_t base_address,
    unsigned long stream_max_index, unsigned long component_swap, unsigned long numeric_type,
    unsigned long format, unsigned long endian_swap)
{
    constant->_01 = 0x01;
    constant->BaseAddress = water_renderer_cpu_to_gpu_address(base_address) >> 2;
    constant->_4B000000 = 0x4b000000;
    constant->_4B0 = 0x4b0;
    constant->ComponentSwap = component_swap;
    constant->NumericType = numeric_type;
    constant->Format = format;
    constant->EndianSwap = endian_swap;
    constant->_96 = 0x96;
    constant->StreamMaxIndex = stream_max_index;
}

static __forceinline void water_renderer_set_z_buffer_mode(e_z_buffer_mode z_buffer_mode)
{
    float scale = 1.0f;
    union { float as_float; unsigned long as_dword; } slope_scale_bias = { WATER_RIPPLE_DEPTH_BIAS };
    union { float as_float; unsigned long as_dword; } depth_bias = { WATER_RIPPLE_DEPTH_BIAS };
    unsigned long z_function;

    if (g_rasterizer_use_floating_point_z_buffer) {
        z_function = 7;
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x44b5c0: shared constant-pool base register; the -1.0 lfs reuses the ripple threshold base
        scale = k_real_constants_minus_one[0];
#else
        scale = -1.0f;
#endif
    } else {
        z_function = 4;
    }

    if (z_buffer_mode != g_rasterizer_current_z_buffer_mode || g_rasterizer_use_floating_point_z_buffer != g_rasterizer_current_z_buffer_floating_point) {
        g_rasterizer_current_z_buffer_mode = z_buffer_mode;
        g_rasterizer_current_z_buffer_floating_point = g_rasterizer_use_floating_point_z_buffer;

        switch (z_buffer_mode) {
        case _z_buffer_mode_normal:
            D3DDevice_SetRenderState_ZEnable(g_rasterizer_device, 1);
            D3DDevice_SetRenderState_ZWriteEnable(g_rasterizer_device, 1);
            D3DDevice_SetRenderState_ZFunc(g_rasterizer_device, z_function);
            break;
        case _z_buffer_mode_read_only:
            D3DDevice_SetRenderState_ZEnable(g_rasterizer_device, 1);
            D3DDevice_SetRenderState_ZWriteEnable(g_rasterizer_device, 0);
            D3DDevice_SetRenderState_ZFunc(g_rasterizer_device, z_function);
            break;
        case _z_buffer_mode_off:
            D3DDevice_SetRenderState_ZEnable(g_rasterizer_device, 0);
            break;
        }

        slope_scale_bias.as_float *= scale;
        depth_bias.as_float *= scale;
        D3DDevice_SetRenderState_SlopeScaleDepthBias(g_rasterizer_device, slope_scale_bias.as_dword);
        D3DDevice_SetRenderState_DepthBias(g_rasterizer_device, depth_bias.as_dword);
    }
}

static __forceinline void water_renderer_set_cull_mode(e_cull_mode cull_mode)
{
    D3DDevice_SetRenderState_CullMode(g_rasterizer_device, cull_mode);
}

void c_water_renderer::ripple_update()
{
    float ripple_rate = g_main_render_timing_data->total;
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x44b5c0: shared constant-pool base register; the threshold base is reused by the inlined z-buffer -1.0 lfs
    float minimum_ripple_rate = k_real_constants_minus_one[1175];
#else
    float minimum_ripple_rate = 0.00001f;
#endif
    if (ripple_rate < minimum_ripple_rate) {
        return;
    }

    ripple_rate *= 30.3030303f;
    water_renderer_set_z_buffer_mode(_z_buffer_mode_off);
    water_renderer_set_cull_mode(_cull_mode_none);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);

    if (!g_water_ripple_update_buffer_address) {
        g_water_ripple_update_buffer_address = c_rasterizer_vertex_buffer_factory_interface::write_lock(&g_water_ripple_globals.vertex_buffer);
        c_rasterizer_vertex_buffer_factory_interface::unlock(&g_water_ripple_globals.vertex_buffer);
    }
    c_rasterizer::set_vertex_stream(&g_water_ripple_globals.vertex_buffer);
    c_rasterizer::set_explicit_shaders(58, _vertex_type_ripple, _transfer_vector_vertex_none, _entry_point_default);
    D3DDevice *device = g_rasterizer_device;
    D3DDevice_BeginExport(device, 0, g_water_ripple_globals.vertex_buffer.get_d3d_buffer(), 0);

    water_renderer_memexport_stream_constant stream_constant;
    water_renderer_set_memexport_stream_constant(&stream_constant, (uintptr_t)g_water_ripple_update_buffer_address, 16384, 0, 7, 38, 2);
    c_rasterizer::set_vertex_shader_constant(130, 1, &stream_constant.vector);
    c_rasterizer::set_vertex_shader_constant(135, 1, g_water_memexport_stream_constants);
#if defined(_XBOX) && !defined(__clang__)
    c_rasterizer::set_vertex_shader_constant(133, 1, (const real_vector4d *)&ripple_rate);
#else
    const real_vector4d ripple_rate_constant = { ripple_rate, 0.0f, 0.0f, 0.0f };
    c_rasterizer::set_vertex_shader_constant(133, 1, &ripple_rate_constant);
#endif

    c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_point_list, 0, 0x800);
    D3DDevice_EndExport(device, 0, g_water_ripple_globals.vertex_buffer.get_d3d_buffer(), 0);
}


static __forceinline void water_renderer_set_alpha_blend_mode(long alpha_blend_mode)
{
    if (g_rasterizer_alpha_blend_mode != alpha_blend_mode) {
        g_rasterizer_alpha_blend_mode = alpha_blend_mode;
        rasterizer_set_device_alpha_blend_mode(g_rasterizer_device, alpha_blend_mode);
    }
}

// @address 0x44ba60 ?ripple_slope@c_water_renderer@@SAXXZ
void c_water_renderer::ripple_slope()
{
    water_renderer_set_z_buffer_mode(_z_buffer_mode_off);
    water_renderer_set_cull_mode(_cull_mode_none);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
    water_renderer_set_alpha_blend_mode(0);

    c_rasterizer::set_vertex_stream(&g_water_ripple_globals.vertex_buffer);
    c_rasterizer::set_explicit_shaders(58, _vertex_type_ripple, _transfer_vector_vertex_none, _entry_point_dynamic_light);
    c_rasterizer::set_aliased_surface_as_texture(1, (c_rasterizer::e_surface)56);
    c_rasterizer::set_sampler_address_mode(1, (c_rasterizer::e_sampler_address_mode)(c_rasterizer::_sampler_address_clamp),
        (c_rasterizer::e_sampler_address_mode)(c_rasterizer::_sampler_address_clamp),
        (c_rasterizer::e_sampler_address_mode)(c_rasterizer::_sampler_address_clamp));
    c_rasterizer::set_sampler_filter_mode(1, c_rasterizer::_sampler_filter_mode_bilinear);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)57);
    c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_quad_list, 0, 1);
    water_renderer_set_alpha_blend_mode(0);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)57, 0, 0, 0, 0);
    c_rasterizer::g_HDR_alpha_blend = false;
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)13);
}


// @address 0x330868 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_water_interaction_event_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<
    1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_water_interaction_event_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x330868: add_member gets alignment_bits re-materialized as li r7,0 instead of the incoming r7
    __assume(alignment_bits == 0);
    long index = g_restricted_regions[1].add_member(name, type, size, alignment_bits, __tls_set_g_water_interaction_event_allocator, 0, 0);
    void *address = restricted_region_get_member_address(1, index);
    __tls_set_g_water_interaction_event_allocator(address);
    m_member_index = index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

// @address 0x330908 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_water_interaction_event_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<
    1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_water_interaction_event_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

#if !defined(_XBOX) || defined(__clang__)
// Keep events a skipped render frame left unread when the next publish overwrites its mirror.
static void water_interaction_events_pre_overwrite(void *mirror_allocation)
{
    s_water_interaction_event *primary = g_water_interaction_events;
    const s_water_interaction_event *mirror = static_cast<const s_water_interaction_event *>(mirror_allocation);
    if (!primary || !mirror || primary == mirror)
        return;

    long count = 0;
    while (count < 128 && primary[count].type != -1)
        ++count;
    for (long index = 0; count < 128 && index < 128 && mirror[index].type != -1; ++index)
    {
        primary[count] = mirror[index];
        ++count;
    }
    g_water_interaction_event_count = count;
}

// Drain primary after copying so interpolated publishes cannot replay the same events.
static void water_interaction_events_post_copy(void *mirror_allocation)
{
    s_water_interaction_event *primary = g_water_interaction_events;
    if (!primary || primary == mirror_allocation)
        return;

    for (long index = 0; index < 128 && primary[index].type != -1; ++index)
        primary[index].type = -1;
    g_water_interaction_event_count = 0;
}
#endif

template<>
void *t_restricted_allocation_manager<
    3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_water_interaction_event_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x44c8f0: add_member gets alignment_bits re-materialized as li r7,0 instead of the incoming r7
    __assume(alignment_bits == 0);
#if !defined(_XBOX) || defined(__clang__)
    long index = g_restricted_regions[3].add_member(name, type, size, alignment_bits,
        __tls_set_g_main_render_timing_data_allocator,
        &water_interaction_events_pre_overwrite, &water_interaction_events_post_copy);
#else
    long index = g_restricted_regions[3].add_member(name, type, size, alignment_bits, __tls_set_g_main_render_timing_data_allocator, 0, 0);
#endif
    void *address = restricted_region_get_member_address(3, index);
    __tls_set_g_main_render_timing_data_allocator(address);
    m_member_index = index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// The SDK's out-of-line matrix inverse uses the VMX128 implementation.
inline XMMATRIX XMMatrixInverse(XMVECTOR *pDeterminant, XMMATRIX M)
{
    XMMATRIX R;
    XMMATRIX P, MT;
    XMVECTOR PR1, PR2;
    XMVECTOR D0, D1, D2;
    XMVECTOR S0, S1, Refine;
    XMVECTOR V0[4], V1[4];
    XMVECTOR Determinant;
    XMVECTOR Reciprocal;
    XMVECTOR Zero, SignMask, Sign0, Sign1;
    XMMATRIX Result;


    S1 = __vspltisw(0);

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);

    PR2 = P.r[2];
    PR1 = P.r[1];

    V0[0] = __vpermwi(MT.r[2], 0x5);
    V1[0] = __vpermwi(MT.r[3], 0xBB);
    V0[1] = __vpermwi(MT.r[0], 0x5);
    V1[1] = __vpermwi(MT.r[1], 0xBB);
    PR2 = __vrlimi(PR2, P.r[0], 0x3, 2);
    PR1 = __vrlimi(PR1, P.r[3], 0xC, 2);

    D0 = __vmulfp(V0[0], V1[0]);
    D1 = __vmulfp(V0[1], V1[1]);
    D2 = __vmulfp(PR2, PR1);

    V0[0] = __vpermwi(MT.r[2], 0xBB);
    V1[0] = __vpermwi(MT.r[3], 0x5);
    V0[1] = __vpermwi(MT.r[0], 0xBB);
    V1[1] = __vpermwi(MT.r[1], 0x5);
    P.r[3] = __vrlimi(P.r[3], P.r[1], 0x3, 2);
    P.r[0] = __vrlimi(P.r[0], P.r[2], 0xC, 2);

    D0 = __vnmsubfp(V0[0], V1[0], D0);
    D1 = __vnmsubfp(V0[1], V1[1], D1);
    D2 = __vnmsubfp(P.r[3], P.r[0], D2);

    SignMask = __vspltisw(-1);
    Zero = __vspltisw(0);
    SignMask = __vslw(SignMask, SignMask);
    Sign0 = __vmrghw(Zero, SignMask);
    Sign1 = __vmrghw(SignMask, Zero);

    V1[0] = __vpermwi(D0, 0x9C);
    V1[1] = __vpermwi(D0, 0xC6);
    V1[2] = __vpermwi(D1, 0x9C);
    V1[3] = __vpermwi(D1, 0xC6);

    V0[0] = __vpermwi(MT.r[1], 0x61);
    V1[0] = __vrlimi(V1[0], D2, 0x8, 1);
    V0[1] = __vpermwi(MT.r[0], 0x84);
    V1[1] = __vrlimi(V1[1], D2, 0x4, 0);
    V0[2] = __vpermwi(MT.r[3], 0x61);
    V1[2] = __vrlimi(V1[2], D2, 0x8, 3);
    V0[3] = __vpermwi(MT.r[2], 0x84);
    V1[3] = __vrlimi(V1[3], D2, 0x4, 2);

    R.r[0] = __vmulfp(V0[0], V1[0]);
    R.r[1] = __vmulfp(V0[1], V1[1]);
    R.r[2] = __vmulfp(V0[2], V1[2]);
    R.r[3] = __vmulfp(V0[3], V1[3]);

    V1[0] = __vpermwi(D0, 0xC6);
    V1[1] = __vpermwi(D0, 0x9C);
    V1[2] = __vpermwi(D1, 0xC6);
    V1[3] = __vpermwi(D1, 0x9C);

    V0[0] = __vpermwi(MT.r[1], 0xB6);
    V1[0] = __vrlimi(V1[0], D2, 0x1, 1);
    V0[1] = __vpermwi(MT.r[0], 0xED);
    V1[1] = __vrlimi(V1[1], D2, 0x2, 2);
    V0[2] = __vpermwi(MT.r[3], 0xB6);
    V1[2] = __vrlimi(V1[2], D2, 0x1, 3);
    V0[3] = __vpermwi(MT.r[2], 0xED);
    V1[3] = __vrlimi(V1[3], D2, 0x2, 0);

    R.r[0] = __vnmsubfp(V0[0], V1[0], R.r[0]);
    R.r[1] = __vnmsubfp(V0[1], V1[1], R.r[1]);
    R.r[2] = __vnmsubfp(V0[2], V1[2], R.r[2]);
    R.r[3] = __vnmsubfp(V0[3], V1[3], R.r[3]);

    V1[0] = __vsldoi(D0, D2, 2 << 2);
    V1[1] = __vpermwi(D0, 0x4E);
    V1[2] = __vmrglw(D1, D2);
    V1[3] = __vpermwi(D1, 0x4E);

    V0[0] = __vpermwi(MT.r[1], 0xCC);
    V1[0] = __vpermwi(V1[0], 0x38);
    V0[1] = __vpermwi(MT.r[0], 0x72);
    V1[1] = __vrlimi(V1[1], D2, 0x9, 1);
    V0[2] = __vpermwi(MT.r[3], 0xCC);
    V1[2] = __vpermwi(V1[2], 0x34);
    V0[3] = __vpermwi(MT.r[2], 0x72);
    V1[3] = __vrlimi(V1[3], D2, 0x9, 3);

    V0[0] = __vxor(V0[0], Sign0);
    V0[1] = __vxor(V0[1], Sign1);
    V0[2] = __vxor(V0[2], Sign0);
    V0[3] = __vxor(V0[3], Sign1);

    R.r[0] = __vmaddfp(V0[0], V1[0], R.r[0]);
    R.r[1] = __vmaddfp(V0[1], V1[1], R.r[1]);
    R.r[2] = __vmaddfp(V0[2], V1[2], R.r[2]);
    R.r[3] = __vmaddfp(V0[3], V1[3], R.r[3]);

    Determinant = __vmsum4fp(R.r[0], MT.r[0]);

    *pDeterminant = Determinant;

    Reciprocal = __vrefp(Determinant);
    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(Determinant, Reciprocal, S0);
    S0 = __vmaddfp(Reciprocal, S0, Reciprocal);
    S1 = __vspltw(S1, 3);
    S1 = __vnmsubfp(Determinant, S0, S1);
    Refine = __vcmpeqfp(S0, S0);
    S0 = __vmaddfp(S0, S1, S0);
    Refine = __vsel(Reciprocal, S0, Refine);

    Result.r[0] = __vmulfp(R.r[0], Refine);
    Result.r[1] = __vmulfp(R.r[1], Refine);
    Result.r[2] = __vmulfp(R.r[2], Refine);
    Result.r[3] = __vmulfp(R.r[3], Refine);

    return Result;

}


// @address 0x44c990 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_water_interaction_event_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<
    3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_water_interaction_event_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x44c9a8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_water_interaction_event_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<
    3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_water_interaction_event_allocator>::deallocate(void *memory)
{
    long value = m_member_index;
    g_water_interaction_events = 0;
    restricted_region_free_member(3, value);
}


__forceinline s_water_float2::s_water_float2(float x_value, float y_value)
{
    x = x_value;
    y = y_value;
}

__forceinline s_water_float2 &s_water_float2::operator=(const s_water_float2 &other)
{
    float *y_address = &y;
    x = other.x;
    *y_address = other.y;
    return *this;
}

// one memexported ripple particle (hlsl `s_ripple`, ripple.fx): eight float4 vertex elements

static __forceinline void *water_vertex_buffer_write_lock(c_rasterizer_vertex_buffer *buffer)
{
    void *result = 0;
    if (g_rasterizer_device && buffer->m_vertex_count)
    {
        if (g_restricted_address[2])
            result = D3DVertexBuffer_Lock(buffer->m_d3d_buffer, 0, buffer->m_vertex_count * buffer->m_vertex_stride, 0);
        else
            result = pointer_from_address(buffer->m_d3d_buffer->Format[0] & ~3u);
    }
    return result;
}
static __forceinline void *water_ripple_buffer_write_lock(c_rasterizer_vertex_buffer *buffer)
{
    void *result = 0;
    if (g_rasterizer_device && buffer->get_vertex_count())
    {
        D3DVertexBuffer *vb = buffer->get_d3d_buffer();
        if (g_restricted_address[2])
            result = D3DVertexBuffer_Lock(vb, 0, buffer->get_vertex_count() * buffer->get_vertex_stride(), 0);
        else
            result = pointer_from_address(vb->Format[0] & ~3u);
    }
    return result;
}
static __forceinline void water_vertex_buffer_unlock(c_rasterizer_vertex_buffer *buffer)
{
    if (g_rasterizer_device && buffer->m_vertex_count && g_restricted_address[2])
        D3DVertexBuffer_Unlock(buffer->m_d3d_buffer);
}


// @address 0x4491c8 ?water_renderer_set_fetch_constant@@YAXPAHPAXIIIII@Z
void water_renderer_set_fetch_constant(int *fetch_constant_words, void *fetch_vertex_buffer, unsigned int fetch_max_index,
    unsigned int fetch_component_swap, unsigned int fetch_numeric_type, unsigned int fetch_format, unsigned int fetch_endian_swap)
{
    uintptr_t fetch_base_address = (uintptr_t)c_rasterizer_vertex_buffer_factory_interface::write_lock((c_rasterizer_vertex_buffer *)fetch_vertex_buffer);
    fetch_constant_words[0] = ((((fetch_base_address >> 20) + 0x200) & 0x1000) + (fetch_base_address & 0x1ffffffc) >> 2) |
                 0x40000000;
    fetch_constant_words[1] = 0x4b000000;
    fetch_constant_words[2] = (((((fetch_component_swap & 1) << 3) | 0x4b00 | (fetch_numeric_type & 7)) << 8 |
                     (fetch_format & 0x3f)) << 8) | (fetch_constant_words[2] & 0xc0f8) | (fetch_endian_swap & 7);
    fetch_constant_words[3] = (fetch_max_index & 0x7fffff) | 0x4b000000;
    c_rasterizer_vertex_buffer_factory_interface::unlock((c_rasterizer_vertex_buffer *)fetch_vertex_buffer);
}

// @address 0x73a15c g_render_method_submit_cached_definition_index

// @rdata 0x82025088..0x820254ac
static const s_water_extern_block render_water_shader_parameters = {
    {
        {0, -1, 6, 0, "_render_method_extern_none"},
        {3, 10, 0, 0, "_render_method_extern_texture_global_target_texaccum"},
        {3, 11, 0, 0, "_render_method_extern_texture_global_target_normal"},
        {3, 11, 0, 0, "_render_method_extern_texture_global_target_z"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_shadow_buffer1"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_shadow_buffer2"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_shadow_buffer3"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_shadow_buffer4"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_texture_camera"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_reflection"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_refraction"},
        {2, 12, 0, 0, "_render_method_extern_texture_lightprobe_texture"},
        {2, 13, 0, 0, "_render_method_extern_texture_dominant_light_intensity_map"},
        {0, -1, 6, 0, "_render_method_extern_texture_unused1"},
        {0, -1, 6, 0, "_render_method_extern_texture_unused2"},
        {0, -1, 6, 0, "_render_method_extern_object_change_color_primary"},
        {0, -1, 6, 0, "_render_method_extern_object_change_color_secondary"},
        {0, -1, 6, 0, "_render_method_extern_object_change_color_tertiary"},
        {0, -1, 6, 0, "_render_method_extern_object_change_color_quaternary"},
        {0, -1, 6, 0, "_render_method_extern_object_emblem_color_background"},
        {0, -1, 6, 0, "_render_method_extern_object_emblem_color_primary"},
        {0, -1, 6, 0, "_render_method_extern_object_emblem_color_secondary"},
        {0, -1, 6, 0, "_render_method_extern_texture_dynamic_environment_map_0"},
        {0, -1, 6, 0, "_render_method_extern_texture_dynamic_environment_map_1"},
        {1, -1, 6, 65536, "_render_method_extern_texture_cook_torrance_cc0236"},
        {1, -1, 6, 65536, "_render_method_extern_texture_cook_torrance_dd0236"},
        {1, -1, 6, 65536, "_render_method_extern_texture_cook_torrance_c78d78"},
        {0, -1, 6, 0, "_render_method_extern_light_dir_0"},
        {0, -1, 6, 0, "_render_method_extern_light_color_0"},
        {0, -1, 6, 0, "_render_method_extern_light_dir_1"},
        {0, -1, 6, 0, "_render_method_extern_light_color_1"},
        {0, -1, 6, 0, "_render_method_extern_light_dir_2"},
        {0, -1, 6, 0, "_render_method_extern_light_color_2"},
        {0, -1, 6, 0, "_render_method_extern_light_dir_3"},
        {0, -1, 6, 0, "_render_method_extern_light_color_3"},
        {0, -1, 6, 0, "_render_method_extern_texture_unused_3"},
        {0, -1, 6, 0, "_render_method_extern_texture_unused_4"},
        {0, -1, 6, 0, "_render_method_extern_texture_unused_5"},
        {0, -1, 6, 0, "_render_method_extern_texture_dynamic_light_gel_0"},
        {0, -1, 6, 0, "_render_method_extern_flat_envmap_matrix_x"},
        {0, -1, 6, 0, "_render_method_extern_flat_envmap_matrix_y"},
        {0, -1, 6, 0, "_render_method_extern_flat_envmap_matrix_z"},
        {0, -1, 6, 0, "_render_method_extern_debug_tint"},
        {1, -1, 6, 0, "_render_method_extern_screen_constants"},
        {0, -1, 6, 0, "_render_method_extern_active_camo_distortion_texture"},
        {3, 10, 0, 0, "_render_method_extern_scene_ldr_texture"},
        {3, 11, 0, 0, "_render_method_extern_scene_hdr_texture"},
        {1, -1, 6, 0, "_render_method_extern_water_memory_export_address"},
        {1, -1, 6, 0, "_render_method_extern_tree_animation_timer"}
    },
    0.01745329238474369049f
};
static const s_water_pool_prefix render_water_pooled_prefix = {{
    0x7f7fffff, 0xff7fffff, 0x3d088889, 0x3c8efa35,
    0x38d1b717, 0x3a83126f, 0x3f4ccccd
}};
static const real_rgb_color render_water_default_material_color = { 0.1f, 0.2f, 0.12f };
static const s_water_pool_suffix render_water_pooled_suffix = {{
    0x7fc00000, 0x00000000, 0x00000000, 0x7fc00000
}};
static const float render_water_ripple_spread_scale = 0.033f;
#if defined(_XBOX) && !defined(__clang__)
// @address 0x2549c k_ripple_buffer_radius
// match: 0x44bf08: float initializers hoist the three trailing loads out of the register 133 upload.
static const s_water_ripple_register k_ripple_buffer_radius = {{
    10.0f,
    {
        0x40, 0xc9, 0x0f, 0xdb,
        0x3c, 0x8e, 0xfa, 0x35,
        0x38, 0xd1, 0xb7, 0x17
    }
}};
#endif
// @rdata-end

extern "C" {
// @address 0x7bba54 g_water_memexport_stream_constants
const real_vector4d *g_water_memexport_stream_constants = &render_water_pooled_suffix.shader_constant;
}

#if defined(_XBOX) && !defined(__clang__)
static __forceinline const real_vector4d &render_water_ripple_register()
{
    return k_ripple_buffer_radius.shader_constant;
}
#else
static const real_vector4d k_ripple_buffer_radius = { 10.0f, 0.0f, 0.0f, 0.0f };

static __forceinline const real_vector4d &render_water_ripple_register()
{
    return k_ripple_buffer_radius;
}
#endif

static __forceinline void render_water_submit_data(bool valid, long definition_index,
    s_cluster_reference option, long object_index, s_cluster_reference cubemap_option,
    long cubemap_index, s_cluster_reference lightmap_option, long lightmap_index,
    float time)
{
    c_render_method_data::m_valid = valid;
    BLAM_ASSUME(option.bsp_index == -1 || option.cluster_index == 255 || definition_index == option.bsp_index);
    c_render_method_data::m_lightmap_bsp_index = definition_index;
    c_render_method_data::m_lightmap_cluster_reference = option;
    c_render_method_data::m_lightmap_instance_index = object_index;
    c_render_method_data::m_cubemap_dynamic_0_cluster_reference = cubemap_option;
    c_render_method_data::m_cubemap_dynamic_0_index = cubemap_index;
    c_render_method_data::m_cubemap_dynamic_1_cluster_reference = lightmap_option;
    c_render_method_data::m_cubemap_dynamic_1_index = lightmap_index;
    c_render_method_data::m_cubemap_blend_factor = time;
    c_render_method_data::m_context_interface = 0;
    c_render_method_data::m_context = -1;
    if (definition_index != -1 && ((unsigned char)option.bsp_index != 255 || object_index != -1)
        && definition_index != g_render_method_submit_cached_definition_index)
    {
        for (long i = 0; i < 49; i++)
        {
            if (render_water_shader_parameters.parameters[i].type == 2)
                rasterizer_set_global_shader_parameter(i);
        }
        g_render_method_submit_cached_definition_index = definition_index;
    }
}

// @address 0x44c170 ?render_cluster_parts@c_water_renderer@@CAXW4e_entry_point@@J@Z
void c_water_renderer::render_cluster_parts(e_entry_point entry_point, long)
{
    for (long part_list_index = 0; part_list_index < g_water_part_count; part_list_index++)
    {
        float time = 1.0f;
#if defined(_XBOX) && !defined(__clang__)
        unsigned short sorted_index = (unsigned short)g_water_cluster_part_indices[part_list_index];
#else
        unsigned short sorted_index = (unsigned short)g_water_render_structure_part_indices[part_list_index];
#endif
        s_render_cluster_part *part = &g_render_structure_globals.cluster_parts[sorted_index];
        if ((part->pass & 3) != 0)
        {
            long bsp_index = part->cluster_reference.bsp_index;
            const scenario_structure_bsp_reference *scenario_bsp = (const scenario_structure_bsp_reference *)tag_block_get_element_with_size(
                &global_scenario->structure_bsps, bsp_index, sizeof(scenario_structure_bsp_reference));
            // match: 0x44c170: tag_get changes constant-base register allocation.
            const structure_bsp *definition = (const structure_bsp *)
                global_tag_instances[datum_index_to_absolute_index(scenario_bsp->structure_bsp.index)].definition;
            int lightmap_available = (g_render_structure_globals.lightmap_bsp_data[bsp_index] != 0);
            D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, 100, &lightmap_available, 1);
            const geometry_material *materials = (const geometry_material *)tag_block_get_element_with_size(
                &definition->materials, 0, sizeof(geometry_material));
            long cubemap_index = c_dynamic_cubemap_sample::get_cluster_cubemap_index(part->cluster_reference);
            const s_render_geometry *render_geometry = g_render_structure_globals.render_geometry[bsp_index];
            render_water_submit_data(true, bsp_index, part->cluster_reference, -1,
                part->cluster_reference, cubemap_index, part->cluster_reference, cubemap_index, time);
            render_water_part(render_geometry, part->mesh_index, part->part_index, entry_point, materials);
            c_render_method_data::m_valid = false;
            render_method_submit_valid();
        }
    }
}

extern "C" {
// @address 0x72cdca g_water_refraction_enabled
#if !defined(_XBOX) || defined(__clang__)
byte g_water_refraction_enabled = 1;
#endif
// @address 0x72cdcb g_water_interaction_enabled
bool g_water_interaction_enabled = true;
// @address 0x7372f8 g_water_tessellation_primitive_type
long g_water_tessellation_primitive_type = 17;
// @address 0x21388 k_water_tessellation_primitive_counts
#if !defined(_XBOX) || defined(__clang__)
const unsigned int k_water_tessellation_primitive_counts[][2] =
{
    {0, 0},
    {0, 0},
    {2, 0},
    {1, 1},
    {3, 0},
    {1, 2},
    {1, 2},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {4, 0},
    {0, 0},
    {0, 0},
    {1, 0},
    {1, 0},
    {1, 0},
};
#endif
}


static __forceinline unsigned long render_water_tag_index(unsigned long tag)
{
    return tag >> 16;
}

static __forceinline unsigned __int64 render_water_tag_subset_mask(unsigned long tag, unsigned long start_index, unsigned long end_index)
{
    return (unsigned __int64)((__int64)0x8000000000000000i64 >> (((tag & 0xff) + end_index) - ((tag & 0xff) + start_index))) >>
        ((tag & 0xff) + start_index);
}

static __forceinline unsigned __int64 render_water_shader_constant_mask(unsigned long start_constant, unsigned long constant_count)
{
    return render_water_tag_subset_mask(0x4000, start_constant / 4, (start_constant + constant_count - 1) / 4);
}

static __forceinline void render_water_tag_collection_set(D3DTAGCOLLECTION *collection, unsigned long index, unsigned __int64 mask)
{
    collection->m_Mask[index] |= mask;
}

static __forceinline void render_water_set_vertex_shader_constant_f1(D3DDevice *device, unsigned int start_register,
    const float *constant_data)
{
    device->m_Constants.VertexShaderF[start_register].i = constant_data[0];
    device->m_Constants.VertexShaderF[start_register].j = constant_data[1];
    device->m_Constants.VertexShaderF[start_register].k = constant_data[2];
    device->m_Constants.VertexShaderF[start_register].w = constant_data[3];
    render_water_tag_collection_set(&device->m_Pending, render_water_tag_index(0x4000), render_water_shader_constant_mask(start_register, 1));
}

static __forceinline void render_water_set_vertex_shader_constant_f(D3DDevice *device, unsigned int start_register,
    const float *constant_data, unsigned long vector4f_count)
{
    if (__IsIntConst((int)vector4f_count)) {
        if (vector4f_count == 1) {
            render_water_set_vertex_shader_constant_f1(device, start_register, constant_data);
        } else {
            D3DDevice_SetVertexShaderConstantFN(device, start_register, constant_data, vector4f_count,
                render_water_shader_constant_mask(start_register, vector4f_count));
        }
    } else {
        D3DDevice_SetVertexShaderConstantFN(device, start_register, constant_data, vector4f_count,
            render_water_shader_constant_mask(start_register, vector4f_count));
    }
}


static __forceinline void render_water_draw_indexed_tessellated_primitive(const c_rasterizer_vertex_buffer *index_buffer,
    long start_index, long primitive_count)
{
    const D3DIndexBuffer *d3d_index_buffer =
        (const D3DIndexBuffer *)index_buffer->get_d3d_buffer();
    D3DIndexBuffer header;
    XGSetIndexBufferHeader(primitive_count * 3 * sizeof(unsigned long), 0, 6, 0, 0, &header);
    XGOffsetResourceAddress(&header, (void *)(d3d_index_buffer->Address + start_index * sizeof(unsigned long)));
    D3DDevice_SetIndices(g_rasterizer_device, &header);
    D3DDevice_DrawIndexedTessellatedVertices(g_rasterizer_device, g_water_tessellation_primitive_type, 0, 0,
        primitive_count * k_water_tessellation_primitive_counts[g_water_tessellation_primitive_type][0] +
        k_water_tessellation_primitive_counts[g_water_tessellation_primitive_type][1]);
    D3DDevice_SetIndices(g_rasterizer_device, 0);
}

static __forceinline void render_water_set_tessellation_level(float maximum_level, float minimum_level)
{
    union { float as_float; unsigned long as_dword; } maximum = { maximum_level };
    unsigned long minimum_bits;
    memcpy(&minimum_bits, &minimum_level, sizeof(minimum_bits));
    D3DDevice_SetRenderState_MaxTessellationLevel(g_rasterizer_device, maximum.as_dword);
    D3DDevice_SetRenderState_MinTessellationLevel(g_rasterizer_device, minimum_bits);
}


static __forceinline bool render_water_interaction_recent()
{
    return g_water_interaction_last_time != -1 && game_time_get() - g_water_interaction_last_time < 300;
}

extern "C" {
// @address 0x10f5ce0 g_water_inverse_view_projection_matrix
real_vector4d g_water_inverse_view_projection_matrix[4];
// @address 0x73a244 g_render_far_plane
// @address 0x825d8c g_water_camera_in_water
#if !defined(_XBOX) || defined(__clang__)
int g_water_camera_in_water = 0;
#endif
// @address 0x8c46e9 g_water_wireframe_enabled
unsigned char g_water_wireframe_enabled;

}

static __forceinline void render_water_set_pixel_shader_constant_f1(D3DDevice *device, unsigned int start_register,
    const float *constant_data)
{
    device->m_Constants.PixelShaderF[start_register].i = constant_data[0];
    device->m_Constants.PixelShaderF[start_register].j = constant_data[1];
    device->m_Constants.PixelShaderF[start_register].k = constant_data[2];
    device->m_Constants.PixelShaderF[start_register].w = constant_data[3];
    render_water_tag_collection_set(&device->m_Pending, render_water_tag_index(0x14000), render_water_shader_constant_mask(start_register, 1));
}

static __forceinline void render_water_get_z_planes(float *near_plane, float *far_plane)
{
    *near_plane = global_z_near;
    *far_plane = g_render_far_plane;
}

static __forceinline void render_water_set_pixel_shader_constant_f(D3DDevice *device, unsigned int start_register,
    const float *constant_data, unsigned long vector4f_count)
{
    if (__IsIntConst((int)vector4f_count)) {
        if (vector4f_count == 1)
            render_water_set_pixel_shader_constant_f1(device, start_register, constant_data);
        else
            D3DDevice_SetPixelShaderConstantFN(device, start_register, constant_data, vector4f_count,
                render_water_shader_constant_mask(start_register, vector4f_count));
    } else {
        D3DDevice_SetPixelShaderConstantFN(device, start_register, constant_data, vector4f_count,
            render_water_shader_constant_mask(start_register, vector4f_count));
    }
}

// @address 0x44bd40 water_renderer_draw_composite
extern "C" void water_renderer_draw_composite()
{
    if (g_water_camera_in_water)
    {
        water_renderer_set_z_buffer_mode(_z_buffer_mode_off);
        water_renderer_set_cull_mode(_cull_mode_none);
        D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
        c_rasterizer::set_pixel_shader_constant(213, 4, g_water_inverse_view_projection_matrix);
        c_rasterizer::set_pixel_shader_constant(218, 1, &g_water_composite_shading_constants);
#if defined(_XBOX) && !defined(__clang__)
        c_rasterizer::set_pixel_shader_constant(219, 1, (const real_vector4d *)&g_water_camera_position);
        c_rasterizer::set_pixel_shader_constant(220, 1, &g_water_underwater_murkiness);
        c_rasterizer::set_pixel_shader_constant(221, 1, (const real_vector4d *)&g_water_underwater_color);
#else
        const real_vector4d camera_position = { g_water_camera_position.x, g_water_camera_position.y, g_water_camera_position.z, 0.0f };
        const real_vector4d underwater_color = { g_water_underwater_color.red, g_water_underwater_color.green, g_water_underwater_color.blue, 0.0f };
        c_rasterizer::set_pixel_shader_constant(219, 1, &camera_position);
        c_rasterizer::set_pixel_shader_constant(220, 1, &g_water_underwater_murkiness);
        c_rasterizer::set_pixel_shader_constant(221, 1, &underwater_color);
#endif
        c_rasterizer::set_explicit_shaders(58, _vertex_type_ripple, _transfer_vector_vertex_none, _entry_point_shadow_apply);
        c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)13);
        c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp,
            c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_bilinear);
        c_rasterizer::set_aliased_surface_as_texture(1, (c_rasterizer::e_surface)3);
        c_rasterizer::set_sampler_address_mode(1, c_rasterizer::_sampler_address_clamp,
            c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        c_rasterizer::set_sampler_filter_mode(1, c_rasterizer::_sampler_filter_mode_bilinear);
        c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_quad_list, 0, 1);
    }
}

// @address 0x44bf08 ?render_shading@c_water_renderer@@SAXXZ
void c_water_renderer::render_shading()
{
    render_water_set_pixel_shader_constant_f(g_rasterizer_device, 213, g_water_inverse_view_projection_matrix[0].n, 4);

    float near_plane, far_plane;
    render_water_get_z_planes(&near_plane, &far_plane);
    real_vector4d depth_constants;
    depth_constants.i = 1.0f / near_plane;
    depth_constants.j = -(far_plane - near_plane) / (near_plane * far_plane);
    depth_constants.k = 0.0f;
    depth_constants.w = 0.0f;
    render_water_set_pixel_shader_constant_f(g_rasterizer_device, 217, depth_constants.n, 1);
    render_water_set_vertex_shader_constant_f(g_rasterizer_device, 134, g_water_ripple_apply_constants.shading_constants.n, 1);
    render_water_set_vertex_shader_constant_f(g_rasterizer_device, 133, render_water_ripple_register().n, 1);
    render_water_set_pixel_shader_constant_f(g_rasterizer_device, 218, g_water_composite_shading_constants.n, 1);
#if defined(_XBOX) && !defined(__clang__)
    render_water_set_pixel_shader_constant_f(g_rasterizer_device, 219, g_water_camera_position.n, 1);
    D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, 103, &g_water_camera_in_water, 1);
    render_water_set_pixel_shader_constant_f(g_rasterizer_device, 220, g_water_underwater_murkiness.n, 1);
    render_water_set_pixel_shader_constant_f(g_rasterizer_device, 221, g_water_underwater_color.n, 1);
#else
    const real_vector4d camera_position = { g_water_camera_position.x, g_water_camera_position.y, g_water_camera_position.z, 0.0f };
    const real_vector4d underwater_color = { g_water_underwater_color.red, g_water_underwater_color.green, g_water_underwater_color.blue, 0.0f };
    render_water_set_pixel_shader_constant_f(g_rasterizer_device, 219, camera_position.n, 1);
    D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, 103, &g_water_camera_in_water, 1);
    render_water_set_pixel_shader_constant_f(g_rasterizer_device, 220, g_water_underwater_murkiness.n, 1);
    render_water_set_pixel_shader_constant_f(g_rasterizer_device, 221, underwater_color.n, 1);
#endif

    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_normal);
    if (g_water_camera_in_water)
        D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 6);
    else
        D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 2);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
    if (g_water_wireframe_enabled)
        D3DDevice_SetRenderState_FillMode(g_rasterizer_device, 37);
    if (g_water_refraction_enabled)
        render_cluster_parts(_entry_point_static_per_pixel, 3);
    else
        render_cluster_parts(_entry_point_static_per_vertex, 3);
    D3DDevice_SetRenderState_FillMode(g_rasterizer_device, 0);
}


// @address 0x44c3a8 ?render_water_part@c_water_renderer@@CAXPBUs_render_geometry@@JJW4e_entry_point@@PBUgeometry_material@@@Z
void c_water_renderer::render_water_part(const s_render_geometry *render_geometry, long mesh_index, long part_index,
    e_entry_point entry_point, const geometry_material *materials)
{
    const s_mesh *mesh = &((const s_mesh *)render_geometry->meshes.address)[mesh_index];
    const s_render_part *part = &((const s_render_part *)mesh->parts.address)[part_index];
    const unsigned short *first_indices = (const unsigned short *)mesh->water_indices.address;

    if (!(part->flags[0] & 8) || materials[part->render_method_index].render_method.index == -1) {
        return;
    }

    if (entry_point != 6) {
        c_rasterizer::set_vertex_stream((const c_rasterizer_vertex_buffer *)
            render_geometry_get_vertex_buffer(render_geometry, (unsigned short)mesh->vertex_buffer_indices[0]));
        c_rasterizer::set_vertex_stream((const c_rasterizer_vertex_buffer *)
            render_geometry_get_vertex_buffer(render_geometry, (unsigned short)mesh->vertex_buffer_indices[6]));
        c_rasterizer::set_vertex_stream((const c_rasterizer_vertex_buffer *)
            render_geometry_get_vertex_buffer(render_geometry, (unsigned short)mesh->vertex_buffer_indices[7]));
        c_rasterizer::set_vertex_stream((const c_rasterizer_vertex_buffer *)
            render_geometry_get_vertex_buffer(render_geometry, (unsigned short)mesh->vertex_buffer_indices[1]));
    }

    long render_method_index = materials[part->render_method_index].render_method.index;
    if (!render_method_submit(render_method_index,
            (const c_render_method *)tag_get('rm  ', render_method_index),
            _vertex_type_water, (e_transfer_vector_vertex_types)mesh->prt_type, entry_point, false)) {
        return;
    }

    real_vector4d triangle_count;
    triangle_count.i = (float)(first_indices[part_index] / 3);
#if !defined(_XBOX) || defined(__clang__)
    triangle_count.j = triangle_count.k = triangle_count.w = 0.0f;
#endif
    render_water_set_vertex_shader_constant_f(g_rasterizer_device, 131, triangle_count.n, 1);

    if (entry_point == 3 || entry_point == 4) {
        render_water_set_vertex_shader_constant_f(g_rasterizer_device, 133, render_water_ripple_register().n, 1);

        int interaction_enabled = g_water_interaction_enabled && render_water_interaction_recent();
        D3DDevice_SetVertexShaderConstantB(g_rasterizer_device, 101, &interaction_enabled, 1);
        D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, 101, &interaction_enabled, 1);
        if (interaction_enabled) {
            c_rasterizer::set_aliased_surface_as_texture(1, (c_rasterizer::e_surface)57);
            c_rasterizer::set_aliased_surface_as_texture(21, (c_rasterizer::e_surface)57);
        }
        int refraction_enabled = g_water_refraction_enabled;
        D3DDevice_SetVertexShaderConstantB(g_rasterizer_device, 102, &refraction_enabled, 1);

        if (entry_point == 3) {
            const c_rasterizer_vertex_buffer *index_buffer =
                (const c_rasterizer_vertex_buffer *)render_geometry_get_index_buffer(render_geometry, (unsigned short)mesh->index_buffer_indices[1]);
            D3DDevice_SetRenderState_TessellationMode(g_rasterizer_device, 2);
            render_water_set_tessellation_level(15.0f, 1.0f);

            render_water_draw_indexed_tessellated_primitive(index_buffer, first_indices[part_index], part->index_count / 3);
        } else if (entry_point == 4) {
            c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_triangle_list,
                first_indices[part_index], part->index_count / 3);
        }
    } else if (entry_point == 15) {
        const c_rasterizer_vertex_buffer *export_buffer =
            (const c_rasterizer_vertex_buffer *)render_geometry_get_index_buffer(render_geometry, (unsigned short)mesh->index_buffer_indices[1]);
        D3DDevice *device = g_rasterizer_device;
        D3DVertexBuffer *d3d_buffer = export_buffer->get_d3d_buffer();
        D3DDevice_BeginExport(device, 0, d3d_buffer, 0);

        const D3DIndexBuffer *export_d3d_buffer = (const D3DIndexBuffer *)d3d_buffer;
        water_renderer_memexport_stream_constant stream_constant;
        water_renderer_set_memexport_stream_constant(&stream_constant,
            export_d3d_buffer->Address, export_d3d_buffer->Size / 4, 0, 7, 36, 2);
        render_water_set_vertex_shader_constant_f(g_rasterizer_device, 130, stream_constant.c, 1);
        render_water_set_vertex_shader_constant_f(g_rasterizer_device, 135, g_water_memexport_stream_constants->n, 1);
        const render_camera *camera = &c_player_view::get_current()->render_camera_state;
#if defined(_XBOX) && !defined(__clang__)
        render_water_set_vertex_shader_constant_f(g_rasterizer_device, 132, camera->position.n, 1);
        c_rasterizer::set_vertex_shader_constant(133, 1, (const real_vector4d *)camera->forward.n);
#else
        const real_vector4d camera_position = { camera->position.x, camera->position.y, camera->position.z, 0.0f };
        const real_vector4d camera_forward = { camera->forward.i, camera->forward.j, camera->forward.k, 0.0f };
        render_water_set_vertex_shader_constant_f(g_rasterizer_device, 132, camera_position.n, 1);
        c_rasterizer::set_vertex_shader_constant(133, 1, &camera_forward);
#endif

        real_vector4d ripple_constants;
        ripple_constants.i = g_water_ripple_apply_constants.ripple_displacement_i;
        ripple_constants.j = g_water_ripple_view_forward_dot;
        ripple_constants.k = ripple_constants.w = 0.0f;
        c_rasterizer::set_vertex_shader_constant(134, 1, &ripple_constants);

        c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_point_list, 0, part->index_count / 3);
        D3DDevice_EndExport(device, 0, d3d_buffer, 0);
    }
}

// @address 0x44a9e8 ?initialize@c_water_renderer@@SAXXZ
void c_water_renderer::initialize()
{
    g_water_interaction_events = (s_water_interaction_event *)
        g_water_interaction_events_allocator.reserve_memory("interaction ripples",
            0, sizeof(s_water_interaction_event) * 128, 0);
    water_interaction_events_dispose();
    bool success = g_water_ripple_globals.vertex_buffer.create((c_rasterizer_vertex_buffer::e_vertex_buffer_declaration)28, 2048);
    for (unsigned long buffer_index = 0; buffer_index < 2; ++buffer_index)
        success &= g_water_ripple_event_vertex_buffers[buffer_index].create((c_rasterizer_vertex_buffer::e_vertex_buffer_declaration)28, 128);
    if (success)
    {
        s_water_ripple_vertex *vertices = (s_water_ripple_vertex *)water_vertex_buffer_write_lock(&g_water_ripple_globals.vertex_buffer);
        for (unsigned long vertex_index = 0; vertex_index < 2048; ++vertex_index)
            vertices[vertex_index].life = -1.0f;
        water_vertex_buffer_unlock(&g_water_ripple_globals.vertex_buffer);
        g_water_interaction_event_count = 0;
        for (unsigned long buffer_index = 0; buffer_index < 2; ++buffer_index)
        {
            s_water_ripple_vertex *buffer_vertices = (s_water_ripple_vertex *)
                water_ripple_buffer_write_lock(&g_water_ripple_event_vertex_buffers[buffer_index]);
            for (unsigned long vertex_index = 0; vertex_index < 128; ++vertex_index)
                buffer_vertices[vertex_index].life = -1.0f;
            water_vertex_buffer_unlock(&g_water_ripple_event_vertex_buffers[buffer_index]);
            g_water_ripple_event_counts[buffer_index] = 0;
        }
    }
}

static __forceinline float water_renderer_ripple_apply_set_z_buffer_mode()
{
    float scale = 1.0f;
    union { float as_float; unsigned long as_dword; } slope_scale_bias = { WATER_RIPPLE_DEPTH_BIAS };
    union { float as_float; unsigned long as_dword; } depth_bias = { WATER_RIPPLE_DEPTH_BIAS };
    if (g_rasterizer_use_floating_point_z_buffer)
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x44b7c0: shared constant-pool base register; one base feeds the -1.0 scale and the 1.0 lfs
        scale = k_real_constants_minus_one[0];
#else
        scale = -1.0f;
#endif
    float one = 1.0f;
    if (g_rasterizer_current_z_buffer_mode != _z_buffer_mode_off || g_rasterizer_use_floating_point_z_buffer != g_rasterizer_current_z_buffer_floating_point)
    {
        g_rasterizer_current_z_buffer_mode = _z_buffer_mode_off;
        g_rasterizer_current_z_buffer_floating_point = g_rasterizer_use_floating_point_z_buffer;
        D3DDevice_SetRenderState_ZEnable(g_rasterizer_device, 0);
        slope_scale_bias.as_float *= scale;
        depth_bias.as_float *= scale;
        D3DDevice_SetRenderState_SlopeScaleDepthBias(g_rasterizer_device, slope_scale_bias.as_dword);
        D3DDevice_SetRenderState_DepthBias(g_rasterizer_device, depth_bias.as_dword);
    }
    return one;
}


// @address 0x44b7c0
void c_water_renderer::ripple_apply()
{
    float one = water_renderer_ripple_apply_set_z_buffer_mode();
    water_renderer_set_cull_mode(_cull_mode_none);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
    water_renderer_set_alpha_blend_mode(1);
    c_rasterizer::set_depth_stencil_surface(c_rasterizer::_surface_none);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)56);
    c_rasterizer::set_render_target(1, c_rasterizer::_surface_none);
    c_rasterizer::clear(15, 0, one, 0);
    c_rasterizer::set_vertex_stream(&g_water_ripple_globals.vertex_buffer);
    c_rasterizer::set_explicit_shaders(58, _vertex_type_ripple, _transfer_vector_vertex_none, _entry_point_albedo);
    const render_camera *camera = &c_player_view::get_current()->rasterizer_camera;
#if defined(_XBOX) && !defined(__clang__)
    c_rasterizer::set_vertex_shader_constant(131, 1, (const real_vector4d *)&camera->position);
#else
    const real_vector4d camera_position = { camera->position.x, camera->position.y, camera->position.z, 0.0f };
    c_rasterizer::set_vertex_shader_constant(131, 1, &camera_position);
#endif
    c_rasterizer::set_vertex_shader_constant(133, 1, &render_water_ripple_register());
    c_rasterizer::set_vertex_shader_constant(134, 1, &g_water_ripple_apply_constants.shading_constants);
    long globals_index = global_game_globals->rasterizer_globals_ref.index;
    const c_rasterizer_globals *globals = globals_index != NONE ?
        (const c_rasterizer_globals *)tag_get('rasg', globals_index) : 0;
    c_rasterizer_texture_ref texture = globals->get_default_texture_rasterizer_texture_ref(11);
    rasterizer_set_sampler_texture_reference(0, c_rasterizer_texture_ref(texture));
    c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp,
        c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
    c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_bilinear);
#if defined(_XBOX) && !defined(__clang__)
    water_texture_size_constant texture_constant;
    texture_constant.integer = D3DArrayTexture_GetArraySize((D3DArrayTexture *)texture.get_d3d_texture());
    texture_constant.value.i = (float)__fcfid(texture_constant.integer_bits);
    c_rasterizer::set_vertex_shader_constant(132, 1, &texture_constant.value);
#else
    const real_vector4d pattern_count = {
        (float)D3DArrayTexture_GetArraySize((D3DArrayTexture *)texture.get_d3d_texture()), 0.0f, 0.0f, 0.0f
    };
    c_rasterizer::set_vertex_shader_constant(132, 1, &pattern_count);
#endif
    c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_quad_list, 0, 8192);
    water_renderer_set_alpha_blend_mode(0);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)56, 0, 0, 0, 0);
}

// 'rwrd' water ripple definition


extern "C" {
// @address 0x119a498 g_water_ripple_append_buffer_address
void *g_water_ripple_append_buffer_address;
}


// @address 0xb34 k_random_word_normalization


// @rdata 0x820249c8
static const float render_water_ripple_two_pi = 6.2831855f;

// @rdata 0x82042914
static const float render_water_ripple_motion_scale = 0.03f;

static __forceinline void water_renderer_set_z_buffer_off()
{
    float scale = 1.0f;
    union { float as_float; unsigned long as_dword; } slope_scale_bias = { WATER_RIPPLE_DEPTH_BIAS };
    union { float as_float; unsigned long as_dword; } depth_bias = { WATER_RIPPLE_DEPTH_BIAS };
    if (g_rasterizer_use_floating_point_z_buffer)
        scale = -1.0f;
    if (g_rasterizer_current_z_buffer_mode != _z_buffer_mode_off || g_rasterizer_use_floating_point_z_buffer != g_rasterizer_current_z_buffer_floating_point) {
        g_rasterizer_current_z_buffer_mode = _z_buffer_mode_off;
        g_rasterizer_current_z_buffer_floating_point = g_rasterizer_use_floating_point_z_buffer;
        D3DDevice_SetRenderState_ZEnable(g_rasterizer_device, 0);
        slope_scale_bias.as_float *= scale;
        depth_bias.as_float *= scale;
        D3DDevice_SetRenderState_SlopeScaleDepthBias(g_rasterizer_device, slope_scale_bias.as_dword);
        D3DDevice_SetRenderState_DepthBias(g_rasterizer_device, depth_bias.as_dword);
    }
}

// @address 0x44ae08 water_renderer_handle_interaction_events
extern "C" void water_renderer_handle_interaction_events(unsigned long event_count)
{
    if (event_count == 0)
        return;

#if defined(_XBOX) && !defined(__clang__)
    // match: 0x44ae08: the append upload uses 80(r1) in block scope and 84(r1) in function scope.
    float copy_particle_index;
    float append_particle_index;
#endif
    unsigned long ripple_count = 0;
    bool next_buffer_index = !WATER_RIPPLE_BUFFER_INDEX;
    g_water_ripple_event_counts[next_buffer_index] = event_count;
    s_water_ripple_vertex *ripples = (s_water_ripple_vertex *)
        c_rasterizer_vertex_buffer_factory_interface::write_lock(&g_water_ripple_event_vertex_buffers[next_buffer_index]);
    for (unsigned long event_index = 0; event_index < event_count; ++event_index)
    {
        s_water_ripple_vertex *ripple = &ripples[event_index];
        s_water_interaction_event *event = &g_water_interaction_events[event_index];
        // match: 0x44ae08: tag_get changes constant-base register allocation.
        const s_water_ripple_definition *definition =
            (const s_water_ripple_definition *)global_tag_instances[datum_index_to_absolute_index(event->type)].definition;
        if (definition)
        {
            real_vector3d velocity;
            real_vector3d direction;
            real_vector3d shock;
            scale_vector3d(&event->velocity, render_water_ripple_motion_scale, &velocity);
            scale_vector3d(&event->direction, render_water_ripple_motion_scale, &direction);
            subtract_vectors3d(&direction, &velocity, &shock);

            float position_range = definition->position_random_range;
            float speed = (magnitude3d(&shock) + sqrtf(direction.j * direction.j +
                direction.i * direction.i + direction.k * direction.k)) * 25.0f;
            float scale = sqrtf(1.0f + (speed - 1.0f) * definition->velocity_bias);
            float height = definition->height * 10.0f;
            float spread = definition->spread * render_water_ripple_spread_scale;
            ripple->position = s_water_float2(
                event->position.x + real_random_range_from_seed(get_local_random_seed_address(), -position_range, position_range),
                event->position.y + real_random_range_from_seed(get_local_random_seed_address(), -position_range, position_range));
            ripple->flow = s_water_float2(velocity.i, velocity.j);

            float duration_fraction = real_random_range_from_seed(get_local_random_seed_address(), 0.0f, 1.0f);
            float duration = definition->duration_minimum +
                (definition->duration_maximum - definition->duration_minimum) * (duration_fraction * duration_fraction);
            ripple->life = duration;
            ripple->duration = duration;
            ripple->rise_period = definition->rise_period;
            ripple->height = MIN(height * scale, 2.0f);

            normalize3d(&shock);
            ripple->shock = s_water_float2(shock.i, shock.j);
            ripple->size = definition->size;
            ripple->spread = PIN(spread * scale, 0.0f, render_water_ripple_motion_scale);

            ripple->pendulum_phase = real_random_range_from_seed(get_local_random_seed_address(), 0.0f, render_water_ripple_two_pi);
            ripple->pendulum_revolution = definition->pendulum_revolution * 0.20734511f;
            ripple->pendulum_repeat = definition->pendulum_repeat * render_water_ripple_two_pi;
            ripple->pattern_start_index = definition->pattern_start_index;
            ripple->pattern_end_index = definition->pattern_end_index;
            ripple->foam_out_radius = definition->foam_out_radius;
            ripple->foam_fade_distance = definition->foam_fade_distance;
            ripple->foam_life = definition->foam_duration;
            ripple->foam_duration = definition->foam_duration;
            ripple->flag_drift = (float)TEST_BIT(definition->flags, _water_ripple_drift_bit);
            ripple->flag_pendulum = (float)TEST_BIT(definition->flags, _water_ripple_pendulum_bit);
            ripple->flag_foam = (float)TEST_BIT(definition->flags, _water_ripple_foam_bit);
            ripple->flag_foam_game_unit = (float)TEST_BIT(definition->flags, _water_ripple_foam_game_unit_bit);
            ripple->func_rise = (float)definition->rise_function;
            ripple->func_descend = (float)definition->descend_function;
            ripple->func_pattern = (float)definition->pattern_function;
            ripple->func_foam = (float)definition->foam_function;
        }
        else
        {
            ripple->life = -1.0f;
        }
        event->type = NONE;
    }
    c_rasterizer_vertex_buffer_factory_interface::unlock(&g_water_ripple_event_vertex_buffers[next_buffer_index]);

    water_renderer_set_z_buffer_off();
    water_renderer_set_cull_mode(_cull_mode_none);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);

    long buffer_index = WATER_RIPPLE_BUFFER_INDEX;
    bool source_buffer_index = !buffer_index;
    ripple_count = g_water_ripple_event_counts[source_buffer_index];
    if (ripple_count)
    {
        g_water_ripple_event_counts[buffer_index] = ripple_count;
        c_rasterizer_vertex_buffer *destination_buffer = &g_water_ripple_event_vertex_buffers[buffer_index];
        void *address = c_rasterizer_vertex_buffer_factory_interface::write_lock(destination_buffer);
        c_rasterizer_vertex_buffer_factory_interface::unlock(destination_buffer);
        water_renderer_memexport_stream_constant stream_constant;
        water_renderer_set_memexport_stream_constant(&stream_constant, (uintptr_t)address, 16384, 0, 7, 38, 2);

        c_rasterizer::set_vertex_stream(&g_water_ripple_event_vertex_buffers[source_buffer_index]);
        c_rasterizer::set_explicit_shaders(58, _vertex_type_ripple, _transfer_vector_vertex_none, _entry_point_active_camo);
        D3DDevice *device = g_rasterizer_device;
        D3DDevice_BeginExport(device, 0, destination_buffer->get_d3d_buffer(), 0);
        c_rasterizer::set_vertex_shader_constant(130, 1, &stream_constant.vector);
        c_rasterizer::set_vertex_shader_constant(135, 1, g_water_memexport_stream_constants);
#if defined(_XBOX) && !defined(__clang__)
        copy_particle_index = 0.0f;
        c_rasterizer::set_vertex_shader_constant(138, 1, (const real_vector4d *)&copy_particle_index);
        copy_particle_index = 2048.0f;
        c_rasterizer::set_vertex_shader_constant(139, 1, (const real_vector4d *)&copy_particle_index);
#else
        const real_vector4d particle_index_start = { 0.0f, 0.0f, 0.0f, 0.0f };
        const real_vector4d particle_index_limit = { 2048.0f, 0.0f, 0.0f, 0.0f };
        c_rasterizer::set_vertex_shader_constant(138, 1, &particle_index_start);
        c_rasterizer::set_vertex_shader_constant(139, 1, &particle_index_limit);
#endif
        c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_point_list, 0, ripple_count);
        D3DDevice_EndExport(device, 0, destination_buffer->get_d3d_buffer(), 0);
        WATER_RIPPLE_WRITE_INDEX = (WATER_RIPPLE_WRITE_INDEX + ripple_count) % 2048;
    }

    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_off);
    water_renderer_set_cull_mode(_cull_mode_none);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);

    if (!g_water_ripple_append_buffer_address)
    {
        g_water_ripple_append_buffer_address = c_rasterizer_vertex_buffer_factory_interface::write_lock(&g_water_ripple_globals.vertex_buffer);
        c_rasterizer_vertex_buffer_factory_interface::unlock(&g_water_ripple_globals.vertex_buffer);
    }
    water_renderer_memexport_stream_constant ripple_stream_constant;
    water_renderer_set_memexport_stream_constant(&ripple_stream_constant, (uintptr_t)g_water_ripple_append_buffer_address, 16384, 0, 7, 38, 2);
    unsigned long new_ripple_count = g_water_ripple_event_counts[WATER_RIPPLE_BUFFER_INDEX];
    if (new_ripple_count)
    {
        c_rasterizer::set_vertex_stream(&g_water_ripple_event_vertex_buffers[WATER_RIPPLE_BUFFER_INDEX]);
        c_rasterizer::set_explicit_shaders(58, _vertex_type_ripple, _transfer_vector_vertex_none, _entry_point_active_camo);
        D3DDevice *device = g_rasterizer_device;
        D3DDevice_BeginExport(device, 0, g_water_ripple_globals.vertex_buffer.get_d3d_buffer(), 0);
        c_rasterizer::set_vertex_shader_constant(130, 1, &ripple_stream_constant.vector);
        c_rasterizer::set_vertex_shader_constant(135, 1, g_water_memexport_stream_constants);
#if defined(_XBOX) && !defined(__clang__)
        append_particle_index = (float)WATER_RIPPLE_WRITE_INDEX;
        c_rasterizer::set_vertex_shader_constant(138, 1, (const real_vector4d *)&append_particle_index);
        append_particle_index = 2048.0f;
        c_rasterizer::set_vertex_shader_constant(139, 1, (const real_vector4d *)&append_particle_index);
#else
        const real_vector4d particle_index_start = { (float)WATER_RIPPLE_WRITE_INDEX, 0.0f, 0.0f, 0.0f };
        const real_vector4d particle_index_limit = { 2048.0f, 0.0f, 0.0f, 0.0f };
        c_rasterizer::set_vertex_shader_constant(138, 1, &particle_index_start);
        c_rasterizer::set_vertex_shader_constant(139, 1, &particle_index_limit);
#endif
        c_rasterizer::draw_primitive(c_rasterizer_index_buffer::_primitive_type_point_list, 0, new_ripple_count);
        D3DDevice_EndExport(device, 0, g_water_ripple_globals.vertex_buffer.get_d3d_buffer(), 0);
        WATER_RIPPLE_WRITE_INDEX = (new_ripple_count + WATER_RIPPLE_WRITE_INDEX) % 2048;
    }
    WATER_RIPPLE_BUFFER_INDEX = !WATER_RIPPLE_BUFFER_INDEX;
}

extern "C" {
// @address 0x72cd4a g_water_interaction_requested
static bool g_water_interaction_requested = true;
// @address 0x72cd4b g_water_refraction_requested
static bool g_water_refraction_requested = true;
// @address 0x10f5ca0 g_water_view_projection_matrix
XMMATRIX g_water_view_projection_matrix;


}

static __forceinline XMMATRIX water_matrix_transpose(XMMATRIX M)
{
    XMMATRIX P, MT;
    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);
    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);
    return MT;
}

static __forceinline XMVECTOR water_vector4_transform(XMVECTOR V, XMMATRIX M)
{
    XMMATRIX MT = water_matrix_transpose(M);
    XMVECTOR X = __vmsum4fp(V, MT.r[0]);
    XMVECTOR Y = __vmsum4fp(V, MT.r[1]);
    XMVECTOR Z = __vmsum4fp(V, MT.r[2]);
    XMVECTOR W = __vmsum4fp(V, MT.r[3]);
    XMVECTOR XZ = __vmrghw(X, Z);
    XMVECTOR YW = __vmrghw(Y, W);
    return __vmrghw(XZ, YW);
}

static __forceinline XMVECTOR water_vector_replicate(float value)
{
    XMVECTOR result = __lvlx(&value, 0);
    return __vspltw(result, 0);
}

static __forceinline XMVECTOR water_vector3_normalize(XMVECTOR V)
{
    XMVECTOR D;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR H;
    XMVECTOR N;
    H = __vspltisw(1);
    D = __vmsum3fp(V, V);
    H = __vcfsx(H, 1);
    Zero = __vspltisw(0);
    const XMVECTOR &RcpLength = __vrsqrtefp(D);
    OneHalfLengthSq = __vmulfp(D, H);
    Result = __vcmpeqfp(D, Zero);
    Scale = __vmulfp(RcpLength, RcpLength);
    H = __vnmsubfp(OneHalfLengthSq, Scale, H);
    H = __vmaddfp(RcpLength, H, RcpLength);
    N = __vmulfp(V, H);
    Result = __vsel(N, D, Result);
    return Result;
}


static __forceinline XMVECTOR render_water_vector3_dot(XMVECTOR first, XMVECTOR second)
{
    XMVECTOR result = __vmsum3fp(first, second);
    return result;
}

// @rdata 0x8202215c distinct
static const float render_water_pool_anchor = 0.95f;

#if defined(_XBOX) && !defined(__clang__)
// match: 0x165ca8: 1.0/0.0/-0.5/0.8 load at fixed offsets from the 0x8202215c anchor base register; plain literals
// load each from its own __real@ address and 7 words of update_water_part_list change
#define RENDER_WATER_POOLED_REAL(value, address) ((&render_water_pool_anchor)[((address) - 0x8202215c) / sizeof(float)])
#else
#define RENDER_WATER_POOLED_REAL(value, address) (value)
#endif

// @address 0x165ca8 ?update_water_part_list@c_water_renderer@@SA_NXZ
bool c_water_renderer::update_water_part_list()
{
    bool interaction_enabled = g_water_interaction_requested != 0;
    unsigned long refraction_enabled = g_water_refraction_requested != 0;
    BLAM_ASSUME(interaction_enabled <= 1 && refraction_enabled <= 1);
    g_water_interaction_enabled = interaction_enabled;
    g_water_refraction_enabled = (unsigned char)refraction_enabled;
    c_structure_renderer::update_water_structure_part_list(g_water_render_structure_part_indices, &g_water_part_count);
    bool result = true;
    if (g_water_part_count <= 0)
        result = false;
    const render_camera *camera = &c_player_view::get_current()->rasterizer_camera;
    g_water_camera_position = camera->position;
    if (result)
    {
        real_rectangle2d frustum_bounds;
        render_projection projection;
        render_camera_build_viewport_frustum_bounds(camera, &frustum_bounds);
        float zero = RENDER_WATER_POOLED_REAL(0.0f, 0x8202251c);
        render_camera_build_projection(camera, &frustum_bounds, &projection, zero);
        combine_projection_and_view_matrix(&projection.world_to_view, projection.projection_matrix, g_water_view_projection_matrix.m[0]);
        XMVECTOR determinant = xmmatrix_determinant(g_water_view_projection_matrix);
        *(XMMATRIX *)g_water_inverse_view_projection_matrix = XMMatrixInverse(&determinant, g_water_view_projection_matrix);
        XMVECTOR point;
        point.x = RENDER_WATER_POOLED_REAL(1.0f, 0x820221fc);
        point.y = RENDER_WATER_POOLED_REAL(1.0f, 0x820221fc);
        point.z = RENDER_WATER_POOLED_REAL(-0.5f, 0x820254e4);
        point.w = RENDER_WATER_POOLED_REAL(1.0f, 0x820221fc);
        point = water_vector4_transform(point, water_matrix_transpose(*(const XMMATRIX *)g_water_inverse_view_projection_matrix));
        point = __vmulfp(point, water_vector_replicate(
            RENDER_WATER_POOLED_REAL(1.0f, 0x820221fc) / point.w));
        point.x -= camera->position.x;
        point.y -= camera->position.y;
        point.z -= camera->position.z;
        float forward_i = camera->forward.i;
        point = water_vector3_normalize(point);
        float forward_j = camera->forward.j;
        float forward_k = camera->forward.k;
        XMVECTOR forward;
        forward.x = forward_i;
        forward.y = forward_j;
        forward.z = forward_k;
        forward.w = zero;
        XMVECTOR dot = render_water_vector3_dot(point, forward);
        g_water_ripple_view_forward_dot = dot.x;
        g_water_ripple_apply_constants.ripple_displacement_i = sqrtf(
            RENDER_WATER_POOLED_REAL(1.0f, 0x820221fc) - g_water_ripple_view_forward_dot * g_water_ripple_view_forward_dot);
        g_water_ripple_apply_constants.shading_direction.i = zero;
        g_water_ripple_apply_constants.shading_direction.j = zero;
        g_water_ripple_apply_constants.shading_direction.k = zero;
        set_real_vector3d(&g_water_ripple_apply_constants.shading_direction, -forward_i, -forward_j, zero);
        scale_vector3d(&g_water_ripple_apply_constants.shading_direction,
            RENDER_WATER_POOLED_REAL(1.0f, 0x820221fc) / g_water_ripple_apply_constants.ripple_displacement_i,
            &g_water_ripple_apply_constants.shading_direction);
        float length = normalize3d(&g_water_ripple_apply_constants.shading_direction);
        float scale = length * 0.95f < 0.95f ? length * 0.95f : 0.95f;
        scale_vector3d(&g_water_ripple_apply_constants.shading_direction, scale, &g_water_ripple_apply_constants.shading_direction);
    }
    long material_name;
    g_water_camera_in_water = water_physics_try_and_get_material(&camera->position, 0, &material_name);
    if (g_water_camera_in_water)
    {
        const real_rgb_color *const &default_color = &render_water_default_material_color;
        const void *const &scenario_address = global_scenario;
        g_water_underwater_murkiness.i = RENDER_WATER_POOLED_REAL(0.8f, 0x82022510);
        g_water_underwater_color = *default_color;
        long water_index = static_cast<const scenario *>(scenario_address)->sky_parameters.index;
        if (water_index != -1)
        {
            const s_sky_atm_parameters_definition *definition =
                (const s_sky_atm_parameters_definition *)tag_get('skya', water_index);
            if (definition)
            {
                for (long i = 0; i < definition->materials.count(); i++)
                {
                    const s_water_material_parameters *material = &definition->materials[i];
                    if (material && material_name == material->name)
                    {
                        g_water_underwater_murkiness.i = material->murkiness;
                        g_water_underwater_color = material->color;
                        break;
                    }
                }
            }
        }
    }
    return result;
}
#undef RENDER_WATER_POOLED_REAL

void render_water_set_enabled_flags(bool interaction, bool refraction)
{
    g_water_interaction_requested = interaction;
    g_water_refraction_requested = refraction;
}

water_interaction_allocation_manager g_water_interaction_event_allocator;
water_allocation_manager g_water_interaction_events_allocator;

// match: 0x44ae08: ripple counters share one base at +0x20/+0x24.
// The extern overlay emits no storage; rasterizer_main owns the scalar at +0.
// @address 0x8c46d0 g_water_ripple_render_state

// @address 0x8c46bc g_rasterizer_use_floating_point_z_buffer
