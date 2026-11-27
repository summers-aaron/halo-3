// blamlibxbox/geometry_sampling.obj: from the records 0x15d3a0 0x15dd78 0x15df78 0x15e930 0x15eb80 0x15ebe8 0x15ed20 0x15eda8 0x15efc8 0x15f128 0x15f6e8 0x15f930 0x161f78 0x161ff0 0x1761e8 0x2b7b28 0x2b7ba8 0x2b7c90 0x2b7ec0 0x2b8310 0x2b8ba8
#include "geometry/geometry_definitions.h"
#include "objects/object_render_state_tls.h"
#include "math/matrix_math_types.h"
#include "game/game_globals.h"
#include "geometry/compression_info_types.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "objects/object_datum.h"
#include "game/map_variant.h"
#include "cseries/cseries.h"
#include <xtl.h>
#include <xboxmath.h>
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "math/real_math.h"
#include "math/projection_math.h"
#include "cache/restricted_memory_regions_inline.h"
#include "geometry/geometry_sampling.h"
#include "geometry/geometry_sampling_types.h"
#include "structures/structure_bsp_tag.h"
#include "structures/structure_instanced_geometry.h"
#include "geometry/render_geometry_mesh_types.h"
#include "game/materials_definitions.h"
#include "physics/havok_runtime_pool_types.h"
#include "scenario/scenario_lightmap_types.h"
#include "bitmaps/bitmap_data.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "game/game_engine_scoring_globals.h"
#include "math/spherical_harmonics.h"
#include "render/render_objects_static_lighting.h"
#include "scenario/scenario_geometry.h"
#include "math/matrix_math.h"
#include "render/render_structure.h"
#include "scenario/scenario.h"
#include "game/materials.h"
#include "physics/collision_types.h"
#include "rasterizer/rasterizer_index_buffer.h"
#include "cache/tag_resource_cache_thread_lock.h"
#include "geometry/geometry_block_types.h"
#include "cache/cache_files.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "rasterizer/rasterizer_geometry.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "render_methods/render_method.h"
#include "memory/tls_globals.h"
#include "physics/collision_test_vector.h"
#include "math/matrix_math_constants.h"
#include "camera/camera_fx_settings.h"
#include "bitmaps/bitmap_group.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "memory/tls_data_arrays.h"
#include "multithreading/thread_tls.h"
#include "render/render_structure_globals_types.h"
#include "scenario/scenario_tag.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer_geometry_types.h"
#include "geometry/render_geometry_parts.h"
#include "scenario/scenario_lightmap_lighting_types.h"
#include "structures/structure_instanced_geometry_definition.h"
#include "geometry/geometry_material_types.h"
#include "render/render_visibility_cached_state_types.h"
#include "rasterizer/rasterizer_gpu_address.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "models/render_model_definitions.h"
#include "effects/decal_types.h"
#include "math/real_constant_pool.h"
#include <math.h>
#include "geometry/geometry_sampling_dependencies.h"
#include "objects/object_header_access.h"
#include "memory/data.h"

struct rasterizer_vertex_rigid {
    unsigned short position[4];
    unsigned short texcoord[2];
    unsigned long normal;
    unsigned long tangent;
    unsigned long binormal;
};
struct s_uncompressed_triangle {
    real_point3d positions[3];
    real_point2d texcoords[3];
    real_vector3d normals[3];
    long indices[3];
};
struct s_geometry_sampling_probe {
    real_point3d position;
    float coefficients[3][36];
    real_vector3d dominant_direction;
    real_vector3d direction;
    real_vector3d key_color;
    real_vector3d fill_color;
    float radius;
    bool valid;
};
struct s_geometry_sampling_lightmap_texcoord { unsigned short u, v; };
struct s_geometry_sampling_byte_color { unsigned char alpha, red, green, blue; };
struct s_geometry_sampling_model_probe { real_point3d position; float coefficients[81]; };
struct s_geometry_sampling_model_result { real_point3d position; float coefficients[81]; };

// @flags /TP /O2
// @address 0x15eb80 ?render_geometry_get_vertex_buffer@@YAPAVc_rasterizer_vertex_buffer@@PBUs_render_geometry@@J@Z


struct s_sampling_surface { unsigned short first, count; };
struct s_sampling_surface_reference { unsigned short triangle, cluster; };
enum e_sampling_instance_lightmap_policy {
    _sampling_instance_lightmap_policy_per_pixel_separate = 0,
    _sampling_instance_lightmap_policy_per_vertex,
    _sampling_instance_lightmap_policy_single_probe,
    _sampling_instance_lightmap_policy_per_pixel_shared,
};


struct render_structure_globals;
struct s_collision_bsp_cache;
namespace {
inline structure_bsp *geometry_sampling_bsp_get(long index) {
    long tag = ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[index].structure_bsp.index;
    return (structure_bsp *)tag_get('sbsp', tag);
}
inline const scenario_structure_bsp_reference *geometry_sampling_bsp_reference_get(long index) {
    return &((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[index];
}
inline structure_bsp *geometry_sampling_bsp_reference_get_bsp(const scenario_structure_bsp_reference *reference) {
    return (structure_bsp *)tag_get('sbsp', reference->structure_bsp.index);
}
inline const s_render_geometry *geometry_sampling_geometry_get(long index) {
    return g_render_structure_globals.render_geometry[index];
}
inline const s_scenario_lightmap_bsp_data *geometry_sampling_lightmap_bsp_get(long index) {
    return g_render_structure_globals.lightmap_bsp_data[index];
}
inline structure_instanced_geometry_definition *geometry_sampling_definitions_get(long index) {
    return (structure_instanced_geometry_definition *)g_collision_bsp_cache.users[get_current_thread_index()].entries[index].data;
}
}

// @address 0x15e930 ?find_render_method_index_from_collision@c_geometry_sampler@@CAJJJJ@Z
long c_geometry_sampler::find_render_method_index_from_collision(long bsp_index, long instance_index, long surface_index)
{
    long result = -1;
    if (instance_index != -1)
    {
        structure_bsp *bsp = geometry_sampling_bsp_get(bsp_index);
        const s_render_geometry *geometry = geometry_sampling_geometry_get(bsp_index);
        structure_instanced_geometry_instance *instance = &((structure_instanced_geometry_instance *)bsp->instanced_geometry_instances.address)[instance_index];
        structure_instanced_geometry_definition *definition = &geometry_sampling_definitions_get(bsp_index)[instance->definition_index];
        if (definition->surface_planes.count > 0)
        {
            s_sampling_surface *surface = TAG_BLOCK_GET_ELEMENT(&definition->surface_planes, surface_index, s_sampling_surface);
            for (long i = 0; result == -1 && i < surface->count; ++i)
            {
                s_sampling_surface_reference *reference = TAG_BLOCK_GET_ELEMENT(&definition->surface_to_triangle_mapping,
                    surface->first + i, s_sampling_surface_reference);
                const s_mesh *mesh = &((s_mesh *)geometry->meshes.address)[(unsigned short)definition->mesh_index];
                for (long j = 0; result == -1 && j < mesh->parts.count; ++j)
                {
                    const s_part *part = &((const s_part *)mesh->parts.address)[j];
                    if (reference->triangle >= part->index_start && reference->triangle < part->index_start + part->index_count)
                        result = part->material_index;
                }
            }
        }
    }
    else
    {
        structure_bsp *bsp = geometry_sampling_bsp_get(bsp_index);
        if (bsp->structure_surfaces.count > 0)
        {
            s_sampling_surface *surface = TAG_BLOCK_GET_ELEMENT(&bsp->structure_surfaces, surface_index, s_sampling_surface);
            for (long i = 0; result == -1 && i < surface->count; ++i)
            {
                s_sampling_surface_reference *reference = TAG_BLOCK_GET_ELEMENT(&bsp->structure_surface_to_triangle_mapping,
                    surface->first + i, s_sampling_surface_reference);
                structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[reference->cluster];
                const s_render_geometry *geometry = geometry_sampling_geometry_get(bsp_index);
                const s_mesh *mesh = &((s_mesh *)geometry->meshes.address)[cluster->mesh_index];
                for (long j = 0; result == -1 && j < mesh->parts.count; ++j)
                {
                    const s_part *part = &((const s_part *)mesh->parts.address)[j];
                    if (reference->triangle >= part->index_start && reference->triangle < part->index_start + part->index_count)
                        result = part->material_index;
                }
            }
        }
    }
    return result;
}

c_rasterizer_vertex_buffer *render_geometry_get_vertex_buffer(const s_render_geometry *geometry, long vertex_buffer_index)
{
    long resource_index = geometry->api_resource_handle;
    s_geometry_storage_member *member = &g_geometry_globals.manager->m_resource_cache.m_geometry_storage;
    s_geometry_second_base *base = member;
    const c_tag_resource_cache_file_access_cache *cache = base->m_per_thread_access_cache[get_current_thread_index()];
    const s_geometry_block *block = static_cast<const s_geometry_block *>(cache->m_resource_values[datum_index_to_absolute_index(resource_index)]);
    return block->vertex_entries[vertex_buffer_index].vertex_buffer;
}



// @address 0x15ebe8 ?read_lock@c_rasterizer_vertex_buffer_factory_interface@@SAPBXPAVc_rasterizer_vertex_buffer@@_N@Z
const void *c_rasterizer_vertex_buffer_factory_interface::read_lock(c_rasterizer_vertex_buffer *buffer, bool read_only)
{
    const void *result = 0;
    if (g_rasterizer_device && buffer->get_vertex_count() > 0) {
        D3DVertexBuffer *d3d_buffer = buffer->get_d3d_buffer();
        if (restricted_region_locked_for_current_thread(2))
            result = D3DVertexBuffer_Lock(d3d_buffer, 0, buffer->get_vertex_count() * buffer->get_vertex_stride(), 17);
        else
            result = rasterizer_gpu_cpu_to_cached_readonly(
                d3d_buffer->Format.dword[0] & ~3UL);
    }
    return result;
}


extern "C" {
// XDK `XMLoadDHenN3` (`_VMX128_INTRINSICS_`) statics, spelled with the header's own integer initializers (`xmconvert.inl`).
// @address 0xf50 k_dhenn3_unpack_scale
const unsigned int k_dhenn3_unpack_scale[4] = { 0x3B004020, 0x3A802008, 0x3A802008, 0 };
// @address 0xf60 k_dhenn3_unpack_shift
const unsigned int k_dhenn3_unpack_shift[4] =
    { ((32 - 10) << 16) | (32 - 10), ((32 - 11) << 16) | (32 - 21), ((32 - 11) << 16) | (32 - 32), 0 };
}
namespace {
static __forceinline __vector4 geometry_sampling_load_DHenN3(const XMDHENN3 *source)
{
    __vector4 unpacked;
    __vector4 shift_r;
    unpacked = __lvlx(source, 0);
    shift_r = __vspltisw(-16);
    unpacked = __vspltw(unpacked, 0);
    shift_r = __vsrw(__lvx(&k_dhenn3_unpack_shift, 0), shift_r);
    unpacked = __vslw(unpacked, __lvx(&k_dhenn3_unpack_shift, 0));
    unpacked = __vsraw(unpacked, shift_r);
    unpacked = __vcfsx(unpacked, 0);
    unpacked = __vmulfp(unpacked, __lvx(&k_dhenn3_unpack_scale, 0));
    return unpacked;
}
static __forceinline void geometry_sampling_uncompress_DHenN3_to_real_vector3d_normalized(unsigned long compressed, real_vector3d *normal)
{
    XMFLOAT4A unpacked;
    XMDHENN3 packed;
    packed.v = compressed;
    XMStoreFloat4A(&unpacked, geometry_sampling_load_DHenN3(&packed));
    normal->i = unpacked.x;
    normal->j = unpacked.y;
    normal->k = unpacked.z;
}
static __forceinline float geometry_sampling_uncompress_word_to_real_normalized(unsigned short value)
{
    return (float)value / 65535.0f;
}
static __forceinline void geometry_sampling_uncompress_rigid_point3d(const s_compression_info *compression, const real_point3d *point, real_point3d *out)
{
    real_point3d delta;
    delta.x = compression->x.upper - compression->x.lower;
    delta.y = compression->y.upper - compression->y.lower;
    delta.z = compression->z.upper - compression->z.lower;
    out->x = compression->x.lower + delta.x * point->x;
    out->y = compression->y.lower + delta.y * point->y;
    out->z = compression->z.lower + delta.z * point->z;
}
static __forceinline void geometry_sampling_uncompress_rigid_texcoord2d(const s_compression_info *compression, const real_point2d *point, real_point2d *out)
{
    real_point2d delta;
    delta.x = compression->u.upper - compression->u.lower;
    delta.y = compression->v.upper - compression->v.lower;
    out->x = compression->u.lower + delta.x * point->x;
    out->y = compression->v.lower + delta.y * point->y;
}
}
// @address 0x15eda8 ?extract_rigid_vertex_data@@YAXPBUrasterizer_vertex_rigid@@PBUs_compression_info@@JJPAUs_uncompressed_triangle@@@Z
inline void extract_rigid_vertex_data(const rasterizer_vertex_rigid *vertices, const s_compression_info *compression, long vertex_index,
    long triangle_vertex_index, s_uncompressed_triangle *triangle)
{
    const rasterizer_vertex_rigid *vertex = &vertices[vertex_index];
    real_point3d position;
    real_point2d texcoord;
    position.x = geometry_sampling_uncompress_word_to_real_normalized(vertex->position[0]);
    position.y = geometry_sampling_uncompress_word_to_real_normalized(vertex->position[1]);
    position.z = geometry_sampling_uncompress_word_to_real_normalized(vertex->position[2]);
    texcoord.x = geometry_sampling_uncompress_word_to_real_normalized(vertex->texcoord[0]);
    texcoord.y = geometry_sampling_uncompress_word_to_real_normalized(vertex->texcoord[1]);
    if (compression)
    {
        geometry_sampling_uncompress_rigid_point3d(compression, &position, &triangle->positions[triangle_vertex_index]);
        geometry_sampling_uncompress_rigid_texcoord2d(compression, &texcoord, &triangle->texcoords[triangle_vertex_index]);
    }
    else
    {
        triangle->positions[triangle_vertex_index] = position;
        triangle->texcoords[triangle_vertex_index] = texcoord;
    }
    triangle->indices[triangle_vertex_index] = vertex_index;
    geometry_sampling_uncompress_DHenN3_to_real_vector3d_normalized(vertex->normal, &triangle->normals[triangle_vertex_index]);
    __lwsync();
    __dcbf(0, vertex);
    __dcbf(sizeof(rasterizer_vertex_rigid), vertex);
}

// @address 0x15ed20 ?get_instance_triangle@@YAXPBXPBUs_compression_info@@JPBGJPAUs_uncompressed_triangle@@@Z
void get_instance_triangle(const void *vertices, const s_compression_info *compression, long index, const unsigned short *indices, long,
    s_uncompressed_triangle *triangle)
{
    unsigned short vertex0 = indices[index - 2];
    unsigned short vertex1 = indices[index - 1];
    unsigned short vertex2 = indices[index];
    extract_rigid_vertex_data((const rasterizer_vertex_rigid *)vertices, compression, vertex0, 0, triangle);
    extract_rigid_vertex_data((const rasterizer_vertex_rigid *)vertices, compression, vertex1, 1, triangle);
    extract_rigid_vertex_data((const rasterizer_vertex_rigid *)vertices, compression, vertex2, 2, triangle);
    __lwsync();
    __dcbf(0, &indices[index - 2]);
    __dcbf(0, &indices[index]);
}

// @address 0x161f78 ?find_dominant_light_direction@@YAXPBM00PATreal_vector3d@@@Z
void find_dominant_light_direction(const float *key_light, const float *fill_light, const float *ambient_light, real_vector3d *direction)
{
    direction->i = -(0.212656f * key_light[3] + 0.715158f * fill_light[3] + 0.0721856f * ambient_light[3]);
    direction->j = -(0.212656f * key_light[1] + 0.715158f * fill_light[1] + 0.0721856f * ambient_light[1]);
    direction->k = 0.212656f * key_light[2] + 0.715158f * fill_light[2] + 0.0721856f * ambient_light[2];
    normalize3d(direction);
}


// @address 0x161ff0
void reconstruct_quadratic_lightprobe_from_linear_and_intensity(
    float *key_light, float *fill_light, float *ambient_light, const real_rgb_color *color, real_vector3d *direction, float *intensity)
{
    float coefficients[9];
    float scaled[9];
    find_dominant_light_direction(key_light, fill_light, ambient_light, direction);
    sh_eval_direction(coefficients, 3, direction);
    sh_scale(scaled, 3, coefficients, color->red);
    key_light[4] = scaled[4];
    key_light[5] = scaled[5];
    key_light[6] = scaled[6];
    key_light[7] = scaled[7];
    key_light[8] = scaled[8];
    sh_scale(scaled, 3, coefficients, color->green);
    fill_light[4] = scaled[4];
    fill_light[5] = scaled[5];
    fill_light[6] = scaled[6];
    fill_light[7] = scaled[7];
    fill_light[8] = scaled[8];
    sh_scale(scaled, 3, coefficients, color->blue);
    ambient_light[4] = scaled[4];
    ambient_light[5] = scaled[5];
    ambient_light[6] = scaled[6];
    ambient_light[7] = scaled[7];
    ambient_light[8] = scaled[8];
    float residual = 0.0f;
    float denominator = 0.0f;
    for (long i = 0; i < 9; ++i)
    {
        float difference = color->green * coefficients[i] - fill_light[i];
        residual += difference * difference;
        denominator += fill_light[i] * fill_light[i] * 2.0f;
    }
    if (denominator > 0.0001f)
    {
        float t = 1.0f - residual / denominator;
        *intensity = t * t * t * t * 10.0f;
    }
    else
        *intensity = 10.0f;
}

// @address 0x1761e8 ?calculate_dominant_light_from_lightprobe@@YAXPBM00PATreal_vector3d@@PATreal_rgb_color@@PAM@Z
void calculate_dominant_light_from_lightprobe(const float *key_light, const float *fill_light, const float *ambient_light,
    real_vector3d *direction, real_rgb_color *color, float *intensity)
{
    float coefficients[9];
    find_dominant_light_direction(key_light, fill_light, ambient_light, direction);
    sh_eval_direction(coefficients, 3, direction);
    float key = *key_light * coefficients[0];
    float fill = *fill_light * coefficients[0];
    float ambient = *ambient_light * coefficients[0];
    float total = coefficients[0] * coefficients[0];
    for (long i = 1; i < 9; ++i)
    {
        key += coefficients[i] * key_light[i];
        fill += coefficients[i] * fill_light[i];
        ambient += coefficients[i] * ambient_light[i];
        total += coefficients[i] * coefficients[i];
    }
    if (key <= 0.0f)
        key = 0.0f;
    if (fill <= 0.0f)
        fill = 0.0f;
    if (ambient <= 0.0f)
        ambient = 0.0f;
    if (total <= 0.0001f)
        total = 0.0001f;
    total = 1.0f / total;
    color->red = total * key;
    color->green = total * fill;
    color->blue = total * ambient;
    float residual = 0.0f;
    float denominator = 0.0f;
    for (long i = 0; i < 9; ++i)
    {
        float difference = color->green * coefficients[i] - fill_light[i];
        residual += difference * difference;
        denominator += 2.0f * (fill_light[i] * fill_light[i]);
    }
    if (denominator > 0.0001f)
    {
        float t = 1.0f - residual / denominator;
        *intensity = t * t * t * t * 10.0f;
    }
    else
        *intensity = 10.0f;
}


typedef char geometry_sampling_volume_samples_offset[(offsetof(render_model_definition, volume_samples) == 0x1b4) ? 1 : -1];

// @address 0x2b7b28 ?object_has_lighting_samples@@YA_NJ@Z
inline bool object_has_lighting_samples(long object_index)
{
    bool result = false;
    object_datum *object = (object_datum *)object_get(object_index);
    s_object_definition *object_definition = (s_object_definition *)tag_get('obje', object->definition_index);
    if (object_definition->model.index != -1)
    {
        s_model_definition *model_definition = (s_model_definition *)tag_get('hlmt', object_definition->model.index);
        if (model_definition->render_model_index != -1)
        {
            render_model_definition *render_model =
                (render_model_definition *)tag_get('mode', model_definition->render_model_index);
            if (render_model->volume_samples.count > 0)
                result = true;
        }
    }
    return result;
}

// @address 0x2b7ba8 ?extract_world_vertex_data@@YAXPBUrasterizer_vertex_world@@JJPAUs_uncompressed_triangle@@@Z
inline void extract_world_vertex_data(const rasterizer_vertex_world *vertices, long vertex_index, long triangle_vertex_index, s_uncompressed_triangle *triangle)
{
    const rasterizer_vertex_world *vertex = &vertices[vertex_index];
    triangle->indices[triangle_vertex_index] = vertex_index;
    triangle->positions[triangle_vertex_index] = vertex->position;
    geometry_sampling_uncompress_DHenN3_to_real_vector3d_normalized(vertex->normal, &triangle->normals[triangle_vertex_index]);
    triangle->texcoords[triangle_vertex_index] = vertex->texcoord;
    __lwsync();
    __dcbf(0, vertex);
    __dcbf(32, vertex);
}

// @address 0x15f930 ?get_structure_triangle@@YAXPBXJPBGJPAUs_uncompressed_triangle@@@Z
void get_structure_triangle(const void *vertices, long index, const unsigned short *indices, long, s_uncompressed_triangle *triangle)
{
    const unsigned short *vertex1 = &indices[index];
    const unsigned short *vertex0 = &indices[index - 2];
    unsigned short previous = vertex1[-1];
    unsigned short current = vertex1[0];
    extract_world_vertex_data((const rasterizer_vertex_world *)vertices, vertex0[0], 0, triangle);
    extract_world_vertex_data((const rasterizer_vertex_world *)vertices, previous, 1, triangle);
    extract_world_vertex_data((const rasterizer_vertex_world *)vertices, current, 2, triangle);
    __lwsync();
    __dcbf(0, vertex0);
    __dcbf(0, vertex1);
}



// @address 0x2b7c90 ?geometry_sampling_point_in_triangle3d@@YAMPBTreal_point3d@@000PAM1@Z
float geometry_sampling_point_in_triangle3d(
    const real_point3d *point, const real_point3d *vertex0, const real_point3d *vertex1, const real_point3d *vertex2, float *u, float *v)
{
    real_vector3d point_vector;
    vector_from_points3d(vertex0, point, &point_vector);
    real_vector3d edge1;
    vector_from_points3d(vertex0, vertex1, &edge1);
    real_vector3d edge2;
    vector_from_points3d(vertex0, vertex2, &edge2);
    real_vector3d normal;
    cross_product3d(&edge1, &edge2, &normal);
    short projection = projection_from_vector3d(&normal);
    bool sign = projection_sign_from_vector3d(&normal, projection);
    real_vector2d projected_edge1;
    project_vector3d(&edge1, projection, sign, &projected_edge1);
    real_vector2d projected_point;
    project_vector3d(&point_vector, projection, sign, &projected_point);
    float cross_point = cross_product2d(&projected_edge1, &projected_point);
    float result = cross_point;
    if (cross_point > rasterizer_states_mode_7_depth_bias)
    {
        real_vector2d projected_edge2;
        project_vector3d(&edge2, projection, sign, &projected_edge2);
        float cross_edge2 = cross_product2d(&projected_point, &projected_edge2);
        result = result > cross_edge2 ? cross_edge2 : result;
        if (cross_edge2 > rasterizer_states_mode_7_depth_bias)
        {
            float cross_area = cross_product2d(&projected_edge1, &projected_edge2);
            result = result > cross_area - (cross_point + cross_edge2) ? cross_area - (cross_point + cross_edge2) : result;
            if ((cross_area < rasterizer_states_mode_7_depth_bias || cross_area > 0.0001f) && cross_point + cross_edge2 <= cross_area + 0.0001f)
            {
                float inverse_area = 1.0f / cross_area;
                *u = cross_edge2 * inverse_area;
                *v = cross_point * inverse_area;
            }
            else
            {
                *u = 0.0f;
                *v = 0.0f;
            }
        }
        else
        {
            *u = 0.0f;
            *v = 0.0f;
        }
    }
    else
    {
        *u = 0.0f;
        *v = 0.0f;
    }
    return result;
}


static __forceinline s_geometry_sampling_probe *geometry_sampling_copy_vector3d(const real_vector3d *source, s_geometry_sampling_probe *result)
{
    result->direction = *source;
    return result;
}
// @address 0x15efc8 geometry_sampling_get_lightmap_probe
extern "C" long geometry_sampling_get_lightmap_probe(const real_point3d *point, long probe_index, s_geometry_sampling_probe *result)
{
    long status = 1;
    if (global_scenario->lightmap.index != -1)
    {
        const s_scenario_lightmap_definition *lightmap = (const s_scenario_lightmap_definition *)tag_get('sLdT', global_scenario->lightmap.index);
        if (probe_index < lightmap->airprobes.count && probe_index >= 0)
        {
            result->dominant_direction.i = 0.0f;
            result->dominant_direction.j = 0.0f;
            result->dominant_direction.k = 0.0f;
            geometry_sampling_copy_vector3d(&k_real_math_identity_matrix_storage.up, result);
            result->valid = true;
            result->position = *point;
            result->radius = 10.0f;
            const s_lightmap_airprobe *probes = (const s_lightmap_airprobe *)lightmap->airprobes.address;
            const s_lightmap_airprobe *probe = &probes[probe_index];
            result->key_color.i = half_to_real(probe->lightprobe.dominant_light_direction[0]);
            result->key_color.j = half_to_real(probe->lightprobe.dominant_light_direction[1]);
            result->key_color.k = half_to_real(probe->lightprobe.dominant_light_direction[2]);
            result->fill_color.i = half_to_real(probe->lightprobe.dominant_light_intensity[0]);
            result->fill_color.j = half_to_real(probe->lightprobe.dominant_light_intensity[1]);
            result->fill_color.k = half_to_real(probe->lightprobe.dominant_light_intensity[2]);
            for (long i = 0; i < 9; ++i)
            {
                result->coefficients[0][i] = half_to_real(probe->lightprobe.channel_coefficients[0][i]);
                result->coefficients[1][i] = half_to_real(probe->lightprobe.channel_coefficients[1][i]);
                result->coefficients[2][i] = half_to_real(probe->lightprobe.channel_coefficients[2][i]);
            }
            status = 0;
        }
    }
    return status;
}


static __forceinline float geometry_sampling_integer_power(float base, unsigned long exponent)
{
    float result = 1.0f;
    while (true)
    {
        if ((exponent & 1) != 0u) result *= base;
        exponent >>= 1;
        if (exponent == 0u) break;
        base *= base;
    }
    return result;
}
// @address 0x15f6e8 geometry_sampling_decode_packed_vector
extern "C" real_vector3d *geometry_sampling_decode_packed_vector(unsigned long packed, real_vector3d *result, long)
{
    float range = geometry_sampling_integer_power(2.0f, 2);
    float exponent_range = 127.0f / range;
    float scale = 1.0f;
    struct { float alpha; real_vector3d rgb; } color;
    color.alpha = (float)(signed char)(packed >> 24) * (1.0f / 127.0f);
    color.rgb.i = (float)(signed char)(packed >> 16) * (1.0f / 127.0f);
    color.rgb.j = (float)(signed char)(packed >> 8) * (1.0f / 127.0f);
    color.rgb.k = (float)(signed char)packed * (1.0f / 127.0f);
    scale = (float)pow(2.0, (double)(exponent_range * color.alpha));
    color.rgb.i = scale * color.rgb.i;
    color.rgb.j = scale * color.rgb.j;
    color.rgb.k = scale * color.rgb.k;
    *result = color.rgb;
    return result;
}


static __forceinline real_argb_color geometry_sampling_bitmap_pixel(long group, long bitmap, const real_point2d *point, long layer)
{
    real_argb_color result;
    return *bitmap_group_2d_get_real_pixel(&result, group, bitmap, point, layer, 0, true);
}

static inline real_argb_color geometry_sampling_decode_lightprobe(
    const real_argb_color *first, const real_argb_color *second, const real_rgb_color *first_scale, const real_rgb_color *second_scale)
{
    real_argb_color result;
    float scale = first_scale->red * first->alpha * second->alpha;
    result.red = first->red + second->red - 0.5f;
    result.green = first->green + second->green - 0.5f;
    result.blue = first->blue + second->blue - 0.5f;
    result.red = (result.red * 2.0f - 1.0f) * scale;
    result.green = (result.green * 2.0f - 1.0f) * scale;
    result.blue = (result.blue * 2.0f - 1.0f) * scale;
    return result;
}

// @address 0x2b7ec0 geometry_sampling_sample_lightprobe_bitmaps
extern "C" void geometry_sampling_sample_lightprobe_bitmaps(long bsp_index, const s_scenario_lightmap_bsp_data *bsp,
    const s_geometry_sampling_lightprobe_entity *entity, const real_point2d *point, float *red, float *green, float *blue,
    real_vector3d *direction, real_rgb_color *color, float *intensity)
{
    long linear_sh_index = bsp->coefficients_bitmap.index;
    long dominant_intensity_index = bsp->dominant_light_bitmap.index;
    if (entity->bitmap_index != -1 && linear_sh_index != -1 && dominant_intensity_index != -1)
    {
        memset(red, 0, sizeof(float) * 36);
        memset(green, 0, sizeof(float) * 36);
        memset(blue, 0, sizeof(float) * 36);
        const bitmap_group *linear_sh_group =
            (const bitmap_group *)tag_get('bitm', linear_sh_index);
        const bitmap_group *dominant_intensity_group =
            (const bitmap_group *)tag_get('bitm', dominant_intensity_index);
        long bitmap_index = entity->bitmap_index;
        const bitmap_data *linear_bitmap = &((bitmap_data *)linear_sh_group->bitmaps.address)[bitmap_index];
        const bitmap_data *dominant_bitmap = &((bitmap_data *)dominant_intensity_group->bitmaps.address)[bitmap_index];
        const unsigned long linear_coefficient_count = 4;
        const unsigned long dominant_coefficient_count = 1;
        const unsigned long packed_linear_depth = 8;
        const unsigned long packed_dominant_depth = 2;
        if (linear_bitmap->depth == packed_linear_depth)
        {
            for (unsigned long i = 0; i < linear_coefficient_count; ++i)
            {
                real_argb_color first = geometry_sampling_bitmap_pixel(linear_sh_index, bitmap_index, point, i * 2);
                real_argb_color decoded;
                real_argb_color second = geometry_sampling_bitmap_pixel(linear_sh_index, bitmap_index, point, i * 2 + 1);
                decoded = geometry_sampling_decode_lightprobe(&first, &second, &bsp->coefficients_map_scale[i].packed_coefficient_scale[0],
                    &bsp->coefficients_map_scale[i].packed_coefficient_scale[1]);
                red[i] = decoded.red;
                green[i] = decoded.green;
                blue[i] = decoded.blue;
            }
        }
        else if (linear_bitmap->depth == linear_coefficient_count)
        {
            for (long i = 0; i < linear_bitmap->depth; ++i)
            {
                real_argb_color sample = geometry_sampling_bitmap_pixel(linear_sh_index, bitmap_index, point, i);
                red[i] = sample.alpha;
                green[i] = sample.blue;
                blue[i] = sample.green;
            }
        }
        real_rgb_color *dominant_color;
        if (dominant_bitmap->depth == packed_dominant_depth)
        {
            real_argb_color decoded;
            real_argb_color first = geometry_sampling_bitmap_pixel(dominant_intensity_index, bitmap_index, point, 0);
            real_argb_color second = geometry_sampling_bitmap_pixel(dominant_intensity_index, bitmap_index, point, 1);
            decoded = geometry_sampling_decode_lightprobe(&first, &second, &bsp->coefficients_map_scale[4].packed_coefficient_scale[0],
                &bsp->coefficients_map_scale[4].packed_coefficient_scale[1]);
            // match: 0x2b7ec0: reading color here keeps the packed branch's load at 0x2b81ec and the other branch's load at 0x2b8264.
            real_rgb_color *const volatile &color_argument = color;
            dominant_color = color_argument;
            dominant_color->red = decoded.red;
            dominant_color->green = decoded.green;
            dominant_color->blue = decoded.blue;
        }
        else
        {
            dominant_color = color;
            if (dominant_bitmap->depth == dominant_coefficient_count)
            {
                real_argb_color sample = geometry_sampling_bitmap_pixel(dominant_intensity_index, bitmap_index, point, 0);
                dominant_color->red = sample.alpha;
                dominant_color->green = sample.blue;
                dominant_color->blue = sample.green;
            }
        }
        find_dominant_light_direction(red, green, blue, direction);
        reconstruct_quadratic_lightprobe_from_linear_and_intensity(red, green, blue, dominant_color, direction, intensity);
    }
}

struct s_scenario_lightmap_entity_data;
static __forceinline bool geometry_sampling_query_real_constant(
    const s_render_method_postprocess_definition *definition, long property, real_vector4d *constant)
{
    bool result = false;
    long index = definition->query_constants[property];
    if (index != -1)
    {
        *constant = definition->real_constants[index];
        result = true;
    }
    return result;
}
static __forceinline float geometry_sampling_uncompress_lightmap_word(unsigned short value)
{
    // match: 0x15f128: emits rldicl zero-extension instead of clrlwi; without it the texcoord offset loads (140h/148h) and fr30/fr31 swap in the caller.
    return (float)_rotl(value, 0) / 65535.0f;
}
static __forceinline float shade_scalar(float a, float b, float c, float u, float v)
{
    return a + v * (c - a) + u * (b - a);
}
static __forceinline unsigned long geometry_sampling_swizzle_rgbe(unsigned long value)
{
    s_geometry_sampling_byte_color color;
    color.alpha = (unsigned char)(value >> 24);
    color.red = (unsigned char)(value >> 16);
    color.green = (unsigned char)(value >> 8);
    color.blue = (unsigned char)value;
    return (color.alpha << 24) | (color.blue << 16) | (color.green << 8) | color.red;
}
static __forceinline const unsigned long *geometry_sampling_vertex_lightprobe_get(const void *vertex_lightprobes, long vertex_index, long count)
{
    return (const unsigned long *)vertex_lightprobes + vertex_index * count;
}
static __forceinline void real_rgb_lightprobe_from_half(const s_lightmap_half_rgb_probe *source, s_geometry_sampling_real_rgb_lightprobe *result)
{
    for (long i = 0; i < 27; i++)
        result->coefficients[i] = half_to_real(source->coefficients[i]);
}
// @address 0x15f128 geometry_sampling_compute_triangle_sample
extern "C" void geometry_sampling_compute_triangle_sample(const real_point3d *point, const s_uncompressed_triangle *triangle, float u, float v,
    long triangle_index,
    const void *lightmap_texcoords, const void *vertex_lightprobes, long, unsigned long vertex_lightprobe_stride, long lightprobe_index,
    const real_matrix4x3 *, s_geometry_test_result &result, long, const s_scenario_lightmap_bsp_data *lightmap_bsp)
{
    result.lightmap_texcoord.x = 0.0f;
    result.lightmap_texcoord.y = 0.0f;
    result.triangle_index = triangle_index;
    if (lightmap_texcoords)
    {
        const s_geometry_sampling_lightmap_texcoord *texcoords = (const s_geometry_sampling_lightmap_texcoord *)lightmap_texcoords;
        result.triangle_lightmap_texcoords[0].x = geometry_sampling_uncompress_lightmap_word(texcoords[triangle->indices[0]].u);
        result.triangle_lightmap_texcoords[0].y = geometry_sampling_uncompress_lightmap_word(texcoords[triangle->indices[0]].v);
        result.triangle_lightmap_texcoords[1].x = geometry_sampling_uncompress_lightmap_word(texcoords[triangle->indices[1]].u);
        result.triangle_lightmap_texcoords[1].y = geometry_sampling_uncompress_lightmap_word(texcoords[triangle->indices[1]].v);
        result.triangle_lightmap_texcoords[2].x = geometry_sampling_uncompress_lightmap_word(texcoords[triangle->indices[2]].u);
        result.triangle_lightmap_texcoords[2].y = geometry_sampling_uncompress_lightmap_word(texcoords[triangle->indices[2]].v);
        __lwsync();
        __dcbf(0, lightmap_texcoords);
        // match: 0x15f128: zero-bit rotate preserves 32-bit GPR value
        const unsigned long &texcoord_size = _rotl(_rotl(_rotl(_rotl(sizeof(s_geometry_sampling_lightmap_texcoord), 0), 0), 0), 0);
        __dcbf(texcoord_size, lightmap_texcoords);
        result.lightmap_texcoord.x = shade_scalar(result.triangle_lightmap_texcoords[0].x, result.triangle_lightmap_texcoords[1].x,
            result.triangle_lightmap_texcoords[2].x, u, v);
        result.lightmap_texcoord.y = shade_scalar(result.triangle_lightmap_texcoords[0].y, result.triangle_lightmap_texcoords[1].y,
            result.triangle_lightmap_texcoords[2].y, u, v);
    }
    result.position.x = shade_scalar(triangle->positions[0].x, triangle->positions[1].x, triangle->positions[2].x, u, v);
    result.position.y = shade_scalar(triangle->positions[0].y, triangle->positions[1].y, triangle->positions[2].y, u, v);
    result.position.z = shade_scalar(triangle->positions[0].z, triangle->positions[1].z, triangle->positions[2].z, u, v);
    result.texcoord.x = shade_scalar(triangle->texcoords[0].x, triangle->texcoords[1].x, triangle->texcoords[2].x, u, v);
    result.texcoord.y = shade_scalar(triangle->texcoords[0].y, triangle->texcoords[1].y, triangle->texcoords[2].y, u, v);
    result.normal.i = shade_scalar(triangle->normals[0].i, triangle->normals[1].i, triangle->normals[2].i, u, v);
    result.normal.j = shade_scalar(triangle->normals[0].j, triangle->normals[1].j, triangle->normals[2].j, u, v);
    result.normal.k = shade_scalar(triangle->normals[0].k, triangle->normals[1].k, triangle->normals[2].k, u, v);
    if (vertex_lightprobes)
    {
        long count = vertex_lightprobe_stride / sizeof(unsigned long);
        const unsigned long *vertex_data[3];
        vertex_data[0] = geometry_sampling_vertex_lightprobe_get(vertex_lightprobes, triangle->indices[0], count);
        vertex_data[1] = geometry_sampling_vertex_lightprobe_get(vertex_lightprobes, triangle->indices[1], count);
        vertex_data[2] = geometry_sampling_vertex_lightprobe_get(vertex_lightprobes, triangle->indices[2], count);
        __lwsync();
        __dcbf(0, geometry_sampling_vertex_lightprobe_get(vertex_lightprobes, triangle->indices[0], count));
        __dcbf(0, geometry_sampling_vertex_lightprobe_get(vertex_lightprobes, triangle->indices[1], count));
        __dcbf(0, geometry_sampling_vertex_lightprobe_get(vertex_lightprobes, triangle->indices[2], count));
        real_vector3d unpacked[3][5];
        for (long vertex = 0; vertex < 3; vertex++)
        {
            const unsigned long *data = vertex_data[vertex];
            for (long i = 0; i < count; i++)
                geometry_sampling_decode_packed_vector(geometry_sampling_swizzle_rgbe(data[i]), &unpacked[vertex][i], 1);
        }
        result.dominant_light_direction.k = shade_scalar(unpacked[0][0].k, unpacked[1][0].k, unpacked[2][0].k, u, v);
        result.dominant_light_direction.j = shade_scalar(unpacked[0][0].j, unpacked[1][0].j, unpacked[2][0].j, u, v);
        result.dominant_light_direction.i = shade_scalar(unpacked[0][0].i, unpacked[1][0].i, unpacked[2][0].i, u, v);
        long i = 0;
        do
        {
            result.linear_sh[2][i] = shade_scalar(unpacked[0][i + 1].k, unpacked[1][i + 1].k, unpacked[2][i + 1].k, u, v);
            result.linear_sh[1][i] = shade_scalar(unpacked[0][i + 1].j, unpacked[1][i + 1].j, unpacked[2][i + 1].j, u, v);
            result.linear_sh[0][i] = shade_scalar(unpacked[0][i + 1].i, unpacked[1][i + 1].i, unpacked[2][i + 1].i, u, v);
        } while (++i < 4);
    }
    if (lightmap_bsp && lightprobe_index != -1)
        real_rgb_lightprobe_from_half(&((const s_lightmap_half_rgb_probe *)lightmap_bsp->instanced_geometry_light_probes.address)[lightprobe_index],
            &result.lightprobe);
    result.triangle_positions[0] = triangle->positions[0];
    result.triangle_positions[1] = triangle->positions[1];
    result.triangle_positions[2] = triangle->positions[2];
    result.triangle_texcoords[0] = triangle->texcoords[0];
    result.triangle_texcoords[1] = triangle->texcoords[1];
    result.triangle_texcoords[2] = triangle->texcoords[2];
    result.u = u;
    result.v = v;
}

// @address 0x2b8310 ?sample_diffuse_textures@@YAXPBUs_geometry_test_result@@PATreal_vector3d@@@Z
void sample_diffuse_textures(const s_geometry_test_result *geometry, real_vector3d *diffuse)
{
    const structure_bsp *bsp = (const structure_bsp *)geometry_sampling_bsp_get(geometry->bsp_index);
    long render_method_index = ((geometry_material *)bsp->materials.address)[geometry->material_index].render_method.index;
    if (render_method_index != -1)
    {
        const c_render_method *method =
            (const c_render_method *)tag_get('rm  ', render_method_index);
        const s_render_method_postprocess_definition *definition = (const s_render_method_postprocess_definition *)method->postprocess.address;
        real_argb_color weights;
        real_argb_color color;
        if (definition->sample_texture(s_render_method_postprocess_definition::_runtime_queryable_property_diffuse_weights, geometry->texcoord, 3, &weights))
        {
            float weight = weights.red;
            float blue = 0.0f;
            float green = 0.0f;
            float red = 0.0f;
            float total = 0.0f;
            if (weight > 0.0f)
            {
                if (definition->sample_texture(s_render_method_postprocess_definition::_runtime_queryable_property_diffuse_map, geometry->texcoord, 0, &color))
                {
                    total = weight;
                    red = color.red * weight;
                    green = color.green * weight;
                    blue = color.blue * weight;
                }
            }
            weight = weights.green;
            if (weight > 0.0f)
            {
                if (definition->sample_texture(s_render_method_postprocess_definition::_runtime_queryable_property_diffuse_map_green,
                    geometry->texcoord, 0, &color))
                {
                    total += weight;
                    red += weight * color.red;
                    green += weight * color.green;
                    blue += weight * color.blue;
                }
            }
            weight = weights.blue;
            if (weight > 0.0f)
            {
                if (definition->sample_texture(s_render_method_postprocess_definition::_runtime_queryable_property_diffuse_map_blue,
                    geometry->texcoord, 0, &color))
                {
                    total += weight;
                    red += weight * color.red;
                    green += weight * color.green;
                    blue += weight * color.blue;
                }
            }
            weight = weights.alpha;
            if (weight > 0.0f)
            {
                if (definition->sample_texture(s_render_method_postprocess_definition::_runtime_queryable_property_diffuse_map_alpha,
                    geometry->texcoord, 0, &color))
                {
                    total += weight;
                    red += weight * color.red;
                    green += weight * color.green;
                    blue += weight * color.blue;
                }
            }
            total = total > 0.0001f ? total : 0.0001f;
            float inverse = 1.0f / total;
            diffuse->i = inverse * red;
            diffuse->j = inverse * green;
            diffuse->k = inverse * blue;
        }
        else
        {
            color.alpha = 1.0f;
            color.red = 1.0f;
            color.green = 1.0f;
            color.blue = 1.0f;
            definition->sample_texture(s_render_method_postprocess_definition::_runtime_queryable_property_diffuse_map, geometry->texcoord, 0, &color);
            real_vector4d constant;
            if (geometry_sampling_query_real_constant(definition, 4, &constant))
            {
                color.red *= constant.i;
                color.green *= constant.j;
                color.blue *= constant.k;
            }
            diffuse->i = color.red;
            diffuse->j = color.green;
            diffuse->k = color.blue;
        }
    }
}

static const float geometry_sampling_model_zero = 0.0f;
static const float geometry_sampling_model_one = 1.0f;


// @address 0x2b8ba8 geometry_sampling_sample_model_probes
extern "C" bool geometry_sampling_sample_model_probes(
    // match: 0x2b8ba8: pointer __restrict qualifier prevents alias reload
    const render_model_definition *model, const real_point3d *point, const void *, s_geometry_sampling_model_result *__restrict result)
{
    bool sampled = false;
    if (model)
    {
        float minimum_weight = (float)exp(-5.0);
        memset(result->coefficients, 0, sizeof(result->coefficients));
        if (model->volume_samples.count > 0)
        {
            float inverse = geometry_sampling_model_one;
            float total = geometry_sampling_model_zero;
            for (long i = 0; i < model->volume_samples.count; ++i)
            {
                // match: 0x2b8ba8: value range assumption guides branch lowering
                __assume(i >= 0);
                const s_geometry_sampling_model_probe *probe =
                    TAG_BLOCK_GET_ELEMENT(&model->volume_samples, i, const s_geometry_sampling_model_probe);
                real_vector3d delta;
                vector_from_points3d(&probe->position, point, &delta);
                float distance = sqrtf((delta.k * delta.k + delta.i * delta.i) + delta.j * delta.j);
                float weight = (float)exp(static_cast<double>(distance * -5.0f));
                if (weight > minimum_weight)
                {
                    for (long j = 0; j < 81; ++j)
                        result->coefficients[j] += probe->coefficients[j] * weight;
                    total += weight;
                }
            }
            inverse /= total;
            for (long j = 0; j < 81; ++j)
                result->coefficients[j] *= inverse;
            sampled = true;
        }
    }
    return sampled;
}


namespace {
static __forceinline const s_visibility_cached_object_render_state *geometry_sampling_cached_render_state_get(long index)
{
    return datum_get<s_visibility_cached_object_render_state>(g_cached_object_render_states, index);
}
static __forceinline long mesh_get_vertex_buffer_index_safe(const s_mesh *mesh, long index)
{
    long result = 0;
    if (index >= 0 && index < 8 && (unsigned short)mesh->vertex_buffer_indices[index] != 0xffff)
        result = (unsigned short)mesh->vertex_buffer_indices[index];
    return result;
}
static __forceinline c_rasterizer_vertex_buffer *scenario_lightmap_bsp_get_runtime_vertex_buffer(
    const s_scenario_lightmap_bsp_data *lightmap_bsp, long index)
{
    return render_geometry_get_vertex_buffer(&lightmap_bsp->render_geometry, index);
}
static __forceinline const unsigned short *geometry_sampling_index_buffer_read_lock(c_rasterizer_index_buffer *buffer)
{
    const void *result = 0;
    if (g_rasterizer_device)
    {
        if (restricted_region_locked_for_current_thread(2))
            result = D3DIndexBuffer_Lock(buffer->get_d3d_buffer(), 0, 0, 17);
        else
            result = (const void *)(UINT_PTR)buffer->get_d3d_buffer()->Address;
    }
    return (const unsigned short *)result;
}
static __forceinline bool instance_has_lighting_samples(long structure_bsp_index, long instance_index)
{
    const s_scenario_lightmap_bsp_data *lightmap_bsp = geometry_sampling_lightmap_bsp_get(structure_bsp_index);
    bool result = false;
    if (lightmap_bsp)
    {
        const s_structure_lighting_cluster *reference =
            &((const s_structure_lighting_cluster *)lightmap_bsp->instanced_geometry_lighting.address)[instance_index];
        if (reference->bucket_index != -1)
        {
            const s_lighting_cluster *bucket = &((const s_lighting_cluster *)lightmap_bsp->static_per_vertex_lighting.address)[reference->bucket_index];
            if (bucket->vertex_buffer_index != -1 &&
                scenario_lightmap_bsp_get_runtime_vertex_buffer(lightmap_bsp, bucket->vertex_buffer_index)->get_d3d_buffer())
                result = true;
        }
        else if (reference->lightmap_index != -1)
            result = true;
        else if (reference->lightprobe_index != -1)
            result = true;
    }
    return result;
}
}

// @address 0x15dd78 ?geometry_test_vector@c_geometry_sampler@@SA?AW4e_geometry_test_vector_result@@ABTreal_point3d@@ABTreal_vector3d@@_NJ222AAUs_geometry_test_result@@@Z
e_geometry_test_vector_result c_geometry_sampler::geometry_test_vector(const real_point3d &point, const real_vector3d &vector, bool test_objects,
    long ignore_object_index, bool ignore_lighting, bool test_back_facing, bool find_material, s_geometry_test_result &result)
{
    e_geometry_test_vector_result test_result = _geometry_test_vector_result_none;
    result.has_vertex_lightprobes = false;
    result.has_lightmap_texcoords = false;
    result.has_lightprobe = false;
    s_collision_test_flags flags;
    flags.collision_flags = 0x4811;
    flags.object_flags = 0;
    s_collision_test_flags test_flags = flags;
    test_flags.collision_flags |= 8;
    if (!test_objects)
    {
        s_collision_test_flags object_test_flags;
        object_test_flags.collision_flags = test_flags.collision_flags;
        object_test_flags.object_flags = test_flags.object_flags | 0x2181;
        test_flags = object_test_flags;
    }
    collision_result collision;
    real_point3d test_point = point;
    real_vector3d test_vector = vector;
    result.cluster_index = -1;
    result.instance_index = -1;
    result.object_index = -1;
    long second_ignore_object_index = -1;
    if (ignore_object_index != -1)
    {
        object_datum *object = (object_datum *)object_get(ignore_object_index);
        if ((unsigned short)object->map_variant_index != 0xffff)
        {
            s_variant_object_datum variant_object;
            game_engine_globals->map_info.get_variant_object_datum(object->map_variant_index, &variant_object);
            if (variant_object.helper_object_index != -1)
                second_ignore_object_index = variant_object.helper_object_index;
        }
    }
    if (collision_test_vector(test_flags, false, &test_point, &test_vector, ignore_object_index, second_ignore_object_index, -1, &collision))
    {
        bool collision_valid = false;
        if (geometry_test_collision_result(collision.structure_bsp_index, collision.object_index, collision.instance_index,
            collision.surface_index, &collision.point, false, true, collision_valid, result))
            test_result = _geometry_test_vector_result_sampled;
        else if (collision.object_index != -1)
            test_result = _geometry_test_vector_result_object;
        else if (collision.structure_bsp_index != -1)
            test_result = collision.instance_index != -1 ? _geometry_test_vector_result_instance : _geometry_test_vector_result_structure;
        else
            test_result = _geometry_test_vector_result_none;
        result.collision_point = collision.point;
        result.global_material = collision.global_material;
    }
    return test_result;
}

// @address 0x15df78?geometry_test_collision_result@c_geometry_sampler@@SA_NJJJJPBTreal_point3d@@_N1AA_NAAUs_geometry_test_result@@@Z
bool c_geometry_sampler::geometry_test_collision_result(long structure_bsp_index, long object_index, long instance_index, long surface_index,
    const real_point3d *point, bool ignore_lighting, bool find_material, bool &collision_valid, s_geometry_test_result &result)
{
    bool success = false;
    if (object_index != -1)
    {
        bool has_cached_lighting = false;
        long ultimate_parent_index = -1;
        long ultimate_parent_walker = object_index;
        while (ultimate_parent_walker != -1)
        {
            ultimate_parent_index = ultimate_parent_walker;
            ultimate_parent_walker = ((object_datum *)object_get(ultimate_parent_walker))->parent_object_index;
        }
        long cached_render_state_index = object_get_cached_render_state(ultimate_parent_index);
        if (cached_render_state_index && geometry_sampling_cached_render_state_get(cached_render_state_index)->valid)
            has_cached_lighting = true;
        if (has_cached_lighting || object_has_lighting_samples(object_index))
        {
            success = true;
            collision_valid = true;
            result.object_index = object_index;
        }
    }
    else if (structure_bsp_index != -1)
    {
        const s_scenario_lightmap_bsp_data *lightmap_bsp = geometry_sampling_lightmap_bsp_get(structure_bsp_index);
        const s_render_geometry *render_geometry = geometry_sampling_geometry_get(structure_bsp_index);
        if (render_geometry && (ignore_lighting || lightmap_bsp))
        {
            result.material_index = find_material ? find_render_method_index_from_collision(structure_bsp_index, instance_index, surface_index) : -1;
            result.bsp_index = structure_bsp_index;
            if (instance_index != -1)
            {
                if (ignore_lighting || instance_has_lighting_samples(structure_bsp_index, instance_index))
                {
                    structure_bsp *bsp = geometry_sampling_bsp_get(structure_bsp_index);
                    const structure_instanced_geometry_instance *instance =
                        &((structure_instanced_geometry_instance *)bsp->instanced_geometry_instances.address)[instance_index];
                    const structure_instanced_geometry_definition *definition =
                        &geometry_sampling_definitions_get(structure_bsp_index)[instance->definition_index];
                    const s_mesh *mesh = &((s_mesh *)render_geometry->meshes.address)[(unsigned short)definition->mesh_index];
                    long vertex_buffer_index = mesh_get_vertex_buffer_index_safe(mesh, 0);
                    if (vertex_buffer_index != -1)
                    {
                        c_rasterizer_vertex_buffer *vertex_buffer = render_geometry_get_vertex_buffer(render_geometry, vertex_buffer_index);
                        const void *vertices = c_rasterizer_vertex_buffer_factory_interface::read_lock(vertex_buffer, true);
                        c_rasterizer_vertex_buffer *lightmap_texcoord_buffer = 0;
                        const void *lightmap_texcoords = 0;
                        const void *vertex_lightprobes = 0;
                        long lightprobe_vertex_count = 0;
                        unsigned long lightprobe_vertex_stride = 0;
                        c_rasterizer_vertex_buffer *lightprobe_buffer = 0;
                        long lightprobe_index = -1;
                        if (!ignore_lighting)
                        {
                            switch (instance->lightmap_policy)
                            {
                            case _sampling_instance_lightmap_policy_per_pixel_separate:
                            case _sampling_instance_lightmap_policy_per_pixel_shared:
                                if (instance->lightmap_texcoords_index >= 0 &&
                                    instance->lightmap_texcoords_index < render_geometry->per_instance_lightmap_texcoords.count)
                                {
                                    const s_object_lighting_cluster *texcoords =
                                        &((s_object_lighting_cluster *)
                                            render_geometry->per_instance_lightmap_texcoords.address)[instance->lightmap_texcoords_index];
                                    if (texcoords->vertex_buffer_index != -1)
                                    {
                                        lightmap_texcoord_buffer = render_geometry_get_vertex_buffer(render_geometry, texcoords->vertex_buffer_index);
                                        lightmap_texcoords = c_rasterizer_vertex_buffer_factory_interface::read_lock(lightmap_texcoord_buffer, true);
                                        result.has_lightmap_texcoords = true;
                                        success = true;
                                    }
                                }
                                break;
                            case _sampling_instance_lightmap_policy_per_vertex:
                            {
                                const s_structure_lighting_cluster *reference =
                                    &((const s_structure_lighting_cluster *)lightmap_bsp->instanced_geometry_lighting.address)[instance_index];
                                if (reference->bucket_index != -1)
                                {
                                    lightprobe_buffer = scenario_lightmap_bsp_get_runtime_vertex_buffer(
                                        lightmap_bsp,
                                            ((const s_lighting_cluster *)
                                                lightmap_bsp->static_per_vertex_lighting.address)[reference->bucket_index].vertex_buffer_index);
                                    vertex_lightprobes = c_rasterizer_vertex_buffer_factory_interface::read_lock(lightprobe_buffer, true);
                                    lightprobe_vertex_count = lightprobe_buffer->get_vertex_count();
                                    lightprobe_vertex_stride = lightprobe_buffer->get_vertex_stride();
                                    result.has_vertex_lightprobes = true;
                                    success = true;
                                }
                                break;
                            }
                            case _sampling_instance_lightmap_policy_single_probe:
                            {
                                const s_structure_lighting_cluster *reference =
                                    &((const s_structure_lighting_cluster *)lightmap_bsp->instanced_geometry_lighting.address)[instance_index];
                                if (reference->lightprobe_index != -1)
                                {
                                    lightprobe_index = reference->lightprobe_index;
                                    result.has_lightprobe = true;
                                    success = true;
                                }
                                break;
                            }
                            }
                        }
                        if ((success || ignore_lighting) && (unsigned short)mesh->index_buffer_indices[0] != 0xffff)
                        {
                            c_rasterizer_index_buffer *index_buffer = render_geometry_get_index_buffer(render_geometry,
                                (unsigned short)mesh->index_buffer_indices[0]);
                            const unsigned short *indices = geometry_sampling_index_buffer_read_lock(index_buffer);
                            result.instance_index = instance_index;
                            const s_compression_info *compression =
                                &((s_compression_info *)render_geometry->compression_info.address)[(unsigned short)definition->compression_index];
                            collision_valid = true;
                            real_point3d instance_point;
                            matrix4x3_inverse_transform_point(&instance->transform, point, &instance_point);
                            bool found = false;
                            if (definition->surface_planes.count > 0)
                            {
                                const s_sampling_surface *surface = TAG_BLOCK_GET_ELEMENT(&definition->surface_planes,
                                    surface_index, const s_sampling_surface);
                                float best_match = -3.402823466e+38f;
                                long best_index = -1;
                                float best_u;
                                float best_v;
                                for (long i = 0; i < surface->count; i++)
                                {
                                    s_uncompressed_triangle triangle;
                                    get_instance_triangle(vertices, compression,
                                        TAG_BLOCK_GET_ELEMENT(&definition->surface_to_triangle_mapping, surface->first + i,
                                            s_sampling_surface_reference)->triangle,
                                        indices, vertex_buffer->get_vertex_count(), &triangle);
                                    float u;
                                    float v;
                                    float match = geometry_sampling_point_in_triangle3d(
                                        &instance_point, &triangle.positions[0], &triangle.positions[1], &triangle.positions[2], &u, &v);
                                    if (match > best_match)
                                    {
                                        best_index = i;
                                        best_u = u;
                                        best_v = v;
                                        best_match = match;
                                        if (match > -0.0001f)
                                            break;
                                    }
                                }
                                if (best_index != -1)
                                {
                                    const s_sampling_surface_reference *reference = TAG_BLOCK_GET_ELEMENT(
                                        &definition->surface_to_triangle_mapping, surface->first + best_index,
                                        const s_sampling_surface_reference);
                                    s_uncompressed_triangle triangle;
                                    get_instance_triangle(vertices, compression, reference->triangle, indices, vertex_buffer->get_vertex_count(), &triangle);
                                    geometry_sampling_compute_triangle_sample(&instance_point, &triangle, best_u, best_v,
                                        (reference->triangle - 2) / 3, lightmap_texcoords, vertex_lightprobes,
                                        lightprobe_vertex_count, lightprobe_vertex_stride, lightprobe_index, &instance->transform, result,
                                            structure_bsp_index, lightmap_bsp);
                                    found = true;
                                }
                            }
                            success = found;
                            c_rasterizer_index_buffer_factory_interface::unlock(index_buffer);
                        }
                        c_rasterizer_vertex_buffer_factory_interface::unlock(vertex_buffer);
                        if (lightmap_texcoord_buffer)
                            c_rasterizer_vertex_buffer_factory_interface::unlock(lightmap_texcoord_buffer);
                        if (lightprobe_buffer)
                            c_rasterizer_vertex_buffer_factory_interface::unlock(lightprobe_buffer);
                    }
                }
            }
            else
            {
                const scenario_structure_bsp_reference *bsp_reference = geometry_sampling_bsp_reference_get(structure_bsp_index);
                collision_valid = true;
                structure_bsp *bsp = geometry_sampling_bsp_reference_get_bsp(bsp_reference);
                if (bsp->structure_surfaces.count > 0)
                {
                    const s_sampling_surface *surface = TAG_BLOCK_GET_ELEMENT(&bsp->structure_surfaces, surface_index,
                        const s_sampling_surface);
                    float best_match = -3.402823466e+38f;
                    long best_index = -1;
                    float best_u;
                    float best_v;
                    for (long i = 0; i < surface->count; i++)
                    {
                        const s_sampling_surface_reference *reference = TAG_BLOCK_GET_ELEMENT(
                            &bsp->structure_surface_to_triangle_mapping, surface->first + i, const s_sampling_surface_reference);
                        const s_mesh *mesh =
                            &((s_mesh *)render_geometry->meshes.address)[((structure_cluster *)bsp->clusters.address)[reference->cluster].mesh_index];
                        long vertex_buffer_index = mesh_get_vertex_buffer_index_safe(mesh, 0);
                        if (vertex_buffer_index != -1)
                        {
                            c_rasterizer_vertex_buffer *vertex_buffer = render_geometry_get_vertex_buffer(render_geometry, vertex_buffer_index);
                            const void *vertices = c_rasterizer_vertex_buffer_factory_interface::read_lock(vertex_buffer, true);
                            c_rasterizer_index_buffer *index_buffer = render_geometry_get_index_buffer(render_geometry,
                                (unsigned short)mesh->index_buffer_indices[0]);
                            const unsigned short *indices = geometry_sampling_index_buffer_read_lock(index_buffer);
                            s_uncompressed_triangle triangle;
                            get_structure_triangle(vertices, reference->triangle, indices, vertex_buffer->get_vertex_count(), &triangle);
                            float u;
                            float v;
                            float match = geometry_sampling_point_in_triangle3d(
                                point, &triangle.positions[0], &triangle.positions[1], &triangle.positions[2], &u, &v);
                            if (match > best_match)
                            {
                                best_index = i;
                                best_u = u;
                                best_v = v;
                                best_match = match;
                                if (match > -0.0001f)
                                {
                                    c_rasterizer_vertex_buffer_factory_interface::unlock(vertex_buffer);
                                    c_rasterizer_index_buffer_factory_interface::unlock(index_buffer);
                                    break;
                                }
                            }
                            c_rasterizer_vertex_buffer_factory_interface::unlock(vertex_buffer);
                            c_rasterizer_index_buffer_factory_interface::unlock(index_buffer);
                        }
                    }
                    if (best_index != -1)
                    {
                        const s_sampling_surface_reference *reference = TAG_BLOCK_GET_ELEMENT(
                            &bsp->structure_surface_to_triangle_mapping, surface->first + best_index, const s_sampling_surface_reference);
                        const s_mesh *mesh =
                            &((s_mesh *)render_geometry->meshes.address)[((structure_cluster *)bsp->clusters.address)[reference->cluster].mesh_index];
                        c_rasterizer_vertex_buffer *vertex_buffer = render_geometry_get_vertex_buffer(
                            render_geometry, mesh_get_vertex_buffer_index_safe(mesh, 0));
                        const void *vertices = c_rasterizer_vertex_buffer_factory_interface::read_lock(vertex_buffer, true);
                        long lightmap_texcoord_buffer_index = mesh_get_vertex_buffer_index_safe(mesh, 1);
                        c_rasterizer_vertex_buffer *lightmap_texcoord_buffer = 0;
                        const void *lightmap_texcoords = 0;
                        if (lightmap_texcoord_buffer_index != -1)
                        {
                            lightmap_texcoord_buffer = render_geometry_get_vertex_buffer(render_geometry, lightmap_texcoord_buffer_index);
                            if (lightmap_texcoord_buffer)
                            {
                                lightmap_texcoords = c_rasterizer_vertex_buffer_factory_interface::read_lock(lightmap_texcoord_buffer, true);
                                result.has_lightmap_texcoords = true;
                            }
                        }
                        c_rasterizer_index_buffer *index_buffer = render_geometry_get_index_buffer(render_geometry,
                            (unsigned short)mesh->index_buffer_indices[0]);
                        const unsigned short *indices = geometry_sampling_index_buffer_read_lock(index_buffer);
                        result.cluster_index = reference->cluster;
                        s_uncompressed_triangle triangle;
                        get_structure_triangle(vertices, reference->triangle, indices, vertex_buffer->get_vertex_count(), &triangle);
                        geometry_sampling_compute_triangle_sample(point, &triangle, best_u, best_v, (reference->triangle - 2) / 3, lightmap_texcoords, 0,
                            0, 0, -1, 0, result, structure_bsp_index, lightmap_bsp);
                        success = true;
                        c_rasterizer_index_buffer_factory_interface::unlock(index_buffer);
                        c_rasterizer_vertex_buffer_factory_interface::unlock(vertex_buffer);
                        if (lightmap_texcoord_buffer)
                            c_rasterizer_vertex_buffer_factory_interface::unlock(lightmap_texcoord_buffer);
                    }
                }
            }
        }
    }
    return success;
}

extern "C" {
// @address 0x8c43b8 g_geometry_sampling_use_sky_lighting
#if !defined(_XBOX) || defined(__clang__)
// The sparse .bss pool that holds this flag cannot take a native definition on the
// Xbox link (wpl: shared BSS range), so this unit owns the one external definition for the port.
bool g_geometry_sampling_use_sky_lighting;
#endif
}

s_geometry_sample c_geometry_sampler::m_default_lightprobe_sample =
{
    { 0.0f, 0.0f, -1.0f },
    {
        1.89376f, -1.44314f, 0.19557f, 0.51599f, -0.30808f, -0.04173f,
        -0.46665f, 0.29195f, 0.10501f, -0.68593f, -0.28162f, -0.02457f,
        0.05725f, 0.00277f, 0.39065f, 0.29198f, -0.51806f, -0.40963f,
        -0.07886f, -0.28186f, -0.10771f, -0.08218f, -0.11293f, 0.00076f,
        -0.17254f, -0.51173f, -0.20854f, 0.04657f, 0.02307f, -0.00857f,
        -0.17355f, -0.28087f, -0.11882f, 0.17052f, -0.03682f, -0.11584f,
    },
    {
        2.13512f, -1.79217f, 0.15439f, 0.51479f, -0.27409f, 0.07048f,
        -0.62705f, 0.25751f, -0.01816f, -0.72972f, -0.26973f, 0.03089f,
        0.15399f, 0.01694f, 0.47631f, 0.36473f, -0.57259f, -0.45238f,
        -0.09771f, -0.37129f, -0.09276f, -0.09023f, -0.1829f, -0.04929f,
        -0.20565f, -0.55691f, -0.16243f, 0.15492f, 0.03688f, -0.0044f,
        -0.22628f, -0.31062f, -0.1244f, 0.18018f, -0.06977f, -0.09439f,
    },
    {
        2.25851f, -2.07371f, 0.05633f, 0.44103f, -0.19508f, 0.23114f,
        -0.7642f, 0.18839f, -0.22193f, -0.67869f, -0.2313f, 0.10002f,
        0.29664f, 0.0245f, 0.54792f, 0.41278f, -0.57895f, -0.47083f,
        -0.09933f, -0.47703f, -0.07234f, -0.07335f, -0.2495f, -0.11649f,
        -0.22986f, -0.55137f, -0.06692f, 0.27887f, 0.03516f, 0.01672f,
        -0.27793f, -0.31099f, -0.12964f, 0.18245f, -0.12532f, -0.06907f,
    },
};

extern "C" {
// @address 0x744468 g_geometry_sampling_diffuse_scale
float g_geometry_sampling_diffuse_scale = 1.5f;
}

static __forceinline const s_global_material_definition *geometry_sampling_global_get_material(c_global_material_type type)
{
    const s_global_material_definition *material = 0;
    if (type.m_index != -1 && type.m_index >= 0 && type.m_index < global_game_globals->materials.count())
        material = &((const s_global_material_definition *)global_game_globals->materials.address)[type.m_index];
    return material;
}
static __forceinline real_vector3d *geometry_sampling_transform_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    result->i = vector->i * matrix->forward.i + vector->j * matrix->left.i + vector->k * matrix->up.i;
    result->j = vector->i * matrix->forward.j + vector->j * matrix->left.j + vector->k * matrix->up.j;
    result->k = vector->i * matrix->forward.k + vector->j * matrix->left.k + vector->k * matrix->up.k;
    return result;
}
static __forceinline const s_visibility_cached_object_render_state *geometry_sampling_cached_lightprobe_get(long index)
{
    return (const s_visibility_cached_object_render_state *)geometry_sampling_cached_render_state_get(index);
}

// @address 0x15d3a0 geometry_sampling_sample_point_lighting
extern "C" e_geometry_sample_result geometry_sampling_sample_point_lighting(const real_point3d &point, const real_vector3d &vector, bool test_objects,
    long ignore_object_index, bool sample_lightprobe, bool sample_diffuse, s_geometry_sample &sample)
{
    e_geometry_sample_result result;
    sample.valid = true;
    if (global_scenario->lightmap.index != -1)
    {
        s_geometry_test_result geometry;
        e_geometry_test_vector_result test_result = c_geometry_sampler::geometry_test_vector(
            point, vector, test_objects, ignore_object_index, false, false, true, geometry);
        if (test_result == _geometry_test_vector_result_sampled)
        {
            if (geometry.object_index != -1)
            {
                object_datum *object = (object_datum *)object_get(geometry.object_index);
                long parent_index = -1;
                long ultimate_parent_walker = geometry.object_index;
                while (ultimate_parent_walker != -1)
                {
                    parent_index = ultimate_parent_walker;
                    ultimate_parent_walker = ((object_datum *)object_get(ultimate_parent_walker))->parent_object_index;
                }
                long cached_render_state_index = object_get_cached_render_state(parent_index);
                sample.position = geometry.collision_point;
                if (cached_render_state_index)
                {
                    const s_visibility_cached_object_render_state *render_state = geometry_sampling_cached_render_state_get(cached_render_state_index);
                    const s_global_material_definition *material = geometry_sampling_global_get_material(geometry.global_material);
                    if (material && (bool)((material->flags >> 2) & 1) && object_has_lighting_samples(geometry.object_index))
                    {
                        const s_object_definition *object_definition =
                            (const s_object_definition *)tag_get('obje', object->definition_index);
                        if (object_definition->model.index != -1)
                        {
                            const s_model_definition *model_definition =
                                (const s_model_definition *)tag_get('hlmt', object_definition->model.index);
                            if (model_definition->render_model_index != -1)
                            {
                                const render_model_definition *render_model =
                                    (const render_model_definition *)tag_get('mode', model_definition->render_model_index);
                                object_datum *parent = (object_datum *)object_get(parent_index);
                                const real_matrix4x3 *matrix = (const real_matrix4x3 *)((char *)parent + parent->node_matrices_offset);
                                real_point3d model_point;
                                matrix4x3_inverse_transform_point(matrix, &point, &model_point);
                                s_geometry_sampling_model_result model_result;
                                if (geometry_sampling_sample_model_probes(render_model, &model_point, matrix, &model_result) && cached_render_state_index != -1)
                                {
                                    const s_visibility_cached_object_render_state *lightprobe = geometry_sampling_cached_lightprobe_get(
                                        cached_render_state_index);
                                    float scale = MAX(model_result.coefficients[0] * 3.545f, 0.1f);
                                    scale = MIN(scale, 1.0f);
                                    sh_scale(sample.red, 3, lightprobe->shader_info.lightprobe_sample.red, scale);
                                    sh_scale(sample.green, 3, lightprobe->shader_info.lightprobe_sample.green, scale);
                                    sh_scale(sample.blue, 3, lightprobe->shader_info.lightprobe_sample.blue, scale);
                                    calculate_dominant_light_from_lightprobe(sample.red, sample.green, sample.blue,
                                        &sample.dominant_light_direction, &sample.dominant_light_color, &sample.dominant_light_intensity);
                                    sample.diffuse.i = 0.5f;
                                    sample.diffuse.j = 0.5f;
                                    sample.diffuse.k = 0.5f;
                                    sample.normal = k_real_math_identity_matrix_storage.up;
                                    sample.valid = true;
                                    result = _geometry_sample_result_sampled;
                                }
                            }
                        }
                    }
                    else if (render_state->valid == 1)
                    {
                        const s_visibility_cached_object_render_state *lightprobe = (const s_visibility_cached_object_render_state *)render_state;
                        memcpy(sample.red, lightprobe->shader_info.lightprobe_sample.red, sizeof(float) * 9);
                        memcpy(sample.green, lightprobe->shader_info.lightprobe_sample.green, sizeof(float) * 9);
                        memcpy(sample.blue, lightprobe->shader_info.lightprobe_sample.blue, sizeof(float) * 9);
                        sample.dominant_light_direction = lightprobe->shader_info.lightprobe_sample.dominant_light_direction;
                        sample.dominant_light_intensity = lightprobe->shader_info.lightprobe_sample.dominant_light_intensity;
                        sample.dominant_light_color = lightprobe->shader_info.lightprobe_sample.dominant_light_color;
                        sample.valid = true;
                        result = _geometry_sample_result_sampled;
                    }
                    else
                    {
                        result = _geometry_sample_result_no_geometry;
                    }
                }
            }
            else
            {
                long structure_bsp_index = geometry.bsp_index;
                const s_scenario_lightmap_bsp_data *lightmap_bsp = geometry_sampling_lightmap_bsp_get(structure_bsp_index);
                if (lightmap_bsp && scenario_structure_bsp_has_valid_lightmap_geometry(global_scenario_index, structure_bsp_index))
                {
                    memset(&sample, 0, sizeof(sample));
                    sample.valid = true;
                    if (geometry.instance_index == -1 && geometry.cluster_index == -1)
                    {
                        sample = c_geometry_sampler::m_default_lightprobe_sample;
                        result = _geometry_sample_result_no_lighting_data;
                    }
                    else
                    {
                        const s_geometry_sampling_lightprobe_entity *entity;
                        if (geometry.instance_index != -1)
                        {
                            entity =
                                (const s_geometry_sampling_lightprobe_entity *)
                                    &((const s_structure_lighting_cluster *)lightmap_bsp->instanced_geometry_lighting.address)[geometry.instance_index];
                            const structure_instanced_geometry_instance *instance =
                                &((structure_instanced_geometry_instance *)
                                    geometry_sampling_bsp_get(structure_bsp_index)->instanced_geometry_instances.address)[geometry.instance_index];
                            geometry_sampling_transform_vector(&instance->transform, &geometry.normal, &sample.normal);
                        }
                        else
                        {
                            entity =
                                &((const s_geometry_sampling_cluster_entity *)lightmap_bsp->cluster_static_per_vertex_lighting.address)[geometry.cluster_index];
                            sample.normal = geometry.normal;
                        }
                        if (sample_diffuse)
                        {
                            real_vector3d *diffuse = &sample.diffuse;
                            diffuse->i = diffuse->j = diffuse->k = 0.0f;
                            if (geometry.material_index != -1)
                                sample_diffuse_textures(&geometry, diffuse);
                        }
                        if (sample_lightprobe)
                        {
                            if (geometry.has_vertex_lightprobes)
                            {
                                for (long i = 0; i < 4; i++)
                                {
                                    sample.red[i] = geometry.linear_sh[0][i];
                                    sample.green[i] = geometry.linear_sh[1][i];
                                    sample.blue[i] = geometry.linear_sh[2][i];
                                }
                                sample.dominant_light_color = geometry.dominant_light_color;
                                find_dominant_light_direction(sample.red, sample.green, sample.blue, &sample.dominant_light_direction);
                                reconstruct_quadratic_lightprobe_from_linear_and_intensity(sample.red, sample.green, sample.blue,
                                    &sample.dominant_light_color, &sample.dominant_light_direction, &sample.dominant_light_intensity);
                            }
                            else if (geometry.has_lightmap_texcoords)
                            {
                                geometry_sampling_sample_lightprobe_bitmaps(structure_bsp_index, lightmap_bsp, entity, &geometry.lightmap_texcoord,
                                    sample.red, sample.green, sample.blue,
                                    &sample.dominant_light_direction, &sample.dominant_light_color, &sample.dominant_light_intensity);
                            }
                            else if (geometry.has_lightprobe)
                            {
                                for (long i = 0; i < 9; i++)
                                {
                                    sample.red[i] = geometry.lightprobe.coefficients[i];
                                    sample.green[i] = geometry.lightprobe.coefficients[9 + i];
                                    sample.blue[i] = geometry.lightprobe.coefficients[18 + i];
                                }
                                calculate_dominant_light_from_lightprobe(sample.red, sample.green, sample.blue,
                                    &sample.dominant_light_direction, &sample.dominant_light_color, &sample.dominant_light_intensity);
                            }
                            else
                            {
                                sample.red[0] = 1.0f;
                                sample.green[0] = 1.0f;
                                sample.blue[0] = 1.0f;
                            }
                            if (sample.diffuse.i > 0.0f || sample.diffuse.j > 0.0f || sample.diffuse.k > 0.0f)
                            {
                                real_vector3d direction;
                                direction.i = -geometry.normal.i;
                                direction.j = -geometry.normal.j;
                                direction.k = -geometry.normal.k;
                                float red_scale = sh_reconstruct_order_3(&geometry.normal, sample.red);
                                float green_scale = sh_reconstruct_order_3(&geometry.normal, sample.green);
                                float blue_scale = sh_reconstruct_order_3(&geometry.normal, sample.blue);
                                red_scale = MAX(red_scale, 0.0f);
                                green_scale = MAX(green_scale, 0.0f);
                                blue_scale = MAX(blue_scale, 0.0f);
                                if (g_geometry_sampling_use_sky_lighting)
                                {
                                    memset(sample.red, 0, sizeof(sample.red));
                                    memset(sample.green, 0, sizeof(sample.green));
                                    memset(sample.blue, 0, sizeof(sample.blue));
                                }
                                float red_sh[9];
                                float green_sh[9];
                                float blue_sh[9];
                                sh_eval_directional_light(3, &direction, sample.diffuse.i * red_scale * g_geometry_sampling_diffuse_scale,
                                    sample.diffuse.j * green_scale * g_geometry_sampling_diffuse_scale,
                                        sample.diffuse.k * blue_scale * g_geometry_sampling_diffuse_scale,
                                    red_sh, green_sh, blue_sh);
                                sh_add(sample.red, 3, sample.red, red_sh);
                                sh_add(sample.green, 3, sample.green, green_sh);
                                sh_add(sample.blue, 3, sample.blue, blue_sh);
                            }
                        }
                        result = _geometry_sample_result_sampled;
                    }
                }
                else
                {
                    lighting_interface_get_visible_sky_lighting(
                        sample.red, sample.green, sample.blue, &sample.dominant_light_direction, &sample.dominant_light_color);
                    sample.normal = k_real_math_identity_matrix_storage.up;
                    sample.dominant_light_intensity = 10.0f;
                    sample.diffuse.i = 1.0f;
                    sample.diffuse.j = 1.0f;
                    sample.diffuse.k = 1.0f;
                    result = _geometry_sample_result_no_lightmap;
                }
            }
        }
        else
        {
            result = test_result == _geometry_test_vector_result_none ? _geometry_sample_result_no_geometry : _geometry_sample_result_unlit_geometry;
        }
    }
    else
    {
        sample = c_geometry_sampler::m_default_lightprobe_sample;
        sample.valid = false;
        result = _geometry_sample_result_no_lightprobes;
    }
    return result;
}
