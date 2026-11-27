// blamlibxbox/decals.obj: from the records 0x14c058 0x16e508 0x16e5f8 0x176d70 0x176f98 0x22d2f8 0x22d308 0x22d318 0x22d3a0 0x22d3f0 0x22d440 0x22d498 0x22d5c8 0x22d6c8 0x22d758 0x22d830 0x22d878 0x22d9d0 0x22db40 0x22dff0 0x22e0a8 0x22e550 0x22e650 0x22e788 0x22e820 0x22e8d8 0x22e9c0 0x22ea20 0x22eb18 0x22ebc0 0x22ec70 0x22ecd8 0x22efd8 0x22f148 0x22f218 0x22f760 0x22f808 0x22ff00 0x230060 0x230138 0x230150 0x2303d0 0x230530 0x230558 0x2305e0 0x2308c8 0x230a88 0x230b68 0x230d40 0x231038 0x231550 0x2319c8 0x231a40 0x231c70 0x2321c0 0x232580 0x232620 0x232638 0x232658 0x232708 0x232768 0x232858 0x2328f8 0x232910 0x232930 0x2329d0 0x2329f0 0x232a90 0x232ab0 0x232b08 0x232b88 0x2b4d10 0x36a178 0x428800 0x45a090
#include "geometry/geometry_definitions.h"
#include "memory/runtime_state_allocation.h"
#include "effects/decals_runtime.h"
#include "math/real_math_constants.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "rasterizer/rasterizer_depth_bias_globals.h"
#include "objects/leaf_system_tls.h"
#include "effects/effect_array_globals.h"
#include <xtl.h>
#include "memory/static_arrays.h"
#include "cseries/long_sort.h"
#include "tag_files/tag_index.h"
#include "tag_files/tag_groups.h"
#include "math/matrix_math_types.h"
#include "objects/object_reference_types.h"
#include "memory/data_types.h"
#include "cache/restricted_memory_regions.h"
#include "effects/decals.h"
#include "effects/effects.h"
#include "interface/first_person_weapons.h"
#include "math/collisions_simple.h"
#include "memory/lruv_cache.h"
#include "render_methods/render_method_submit.h"
#include "memory/data.h"
#include "multithreading/synchronized_value.h"
#include "physics/collision_types.h"
#include "physics/collision_test_flags.h"
#include "physics/collision_bsp_types.h"
#include "physics/structure_bsp_seam_link_types.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "structures/cluster_bit_vectors.h"
#include "effects/decals_globals.h"
#include "math/matrix_math_functions.h"
#include "math/real_math_functions.h"
#include "math/real_math_globals.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
#include "math/real_math.h"
#include "math/point_math.h"
#include "game/game_globals_storage.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "visibility/visibility_collection.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "memory/lruv_cache_types.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_index_buffer.h"
#include "scenario/scenario_tag.h"
#include "structures/structure_bsp_tag.h"
#include "rasterizer/rasterizer_states.h"
#include "rasterizer/rasterizer_geometry_types.h"
#include "scenario/scenario.h"
#include "render/dynamic_vertex_buffer_types.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "physics/collisions.h"
#include "structures/structure_seams.h"
#include <math.h>
#include <string.h>
#include "math/real_math_trigonometry.h"
#include "cache/restricted_memory_layout.h"
#include "render_methods/render_method.h"
#include "effects/decal_definition_types.h"
#include "effects/decal_system_types.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/message_queue.h"
#include "effects/decal_message_types.h"
#include "effects/decal_types.h"
#include "geometry/render_geometry_mesh_types.h"
#include "structures/structure_instanced_geometry.h"
#include "structures/structure_instanced_geometry_definition.h"
#include "effects/decals_allocators.h"
#include "structures/preplaced_decal_types.h"
#include "render/views/player_view_types.h"
#include "memory/lruv_block_types.h"
#include "geometry/render_geometry_parts.h"
#include "geometry/render_geometry_types.h"
#include "cseries/cseries.h"
#include "physics/collision_test_vector.h"
#include "structures/structures.h"
#include "math/random_math_range.h"
#include "memory/datum_index.h"
#include "cubemaps/cubemap_sample.h"
#include "objects/object_hierarchy.h"
#include "physics/collision_bsp_cache_types.h"
#include "scenario/cluster_reference_helpers.h"
#include "render_methods/render_method_data.h"
#include "math/random_math.h"
#include "render/shader_extern_info_types.h"
#include "game/game.h"
#include "tag_files/string_ids.h"
#include "scenario/scenario_resource_registry.h"

// @address 0x74b80c g_runtime_state_allocation
// @address 0x9e7ea0 g_visibility_globals
// @address 0x8c43a8 g_effect_budget_limits
// @address 0x3aa90 k_real_math_origin3d
// @address 0x3aa90 k_real_math_zero_vector3d_storage
// @address 0x81dab0 g_rasterizer_current_z_buffer_mode

// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @flags /TP /O2
// @address 0x5a7618 D3DResource_Release

// @address 0x2b4d10 ?__tls_set_g_decal_array_allocator@@YAXPAX@Z
// @address 0x36a178 ?__tls_set_g_decal_system_allocator@@YAXPAX@Z

// @address 0xa31558 g_decal_system_pvs
s_game_cluster_bit_vectors g_decal_system_pvs;


struct s_fold;
struct s_decal_projection_builder {
    real_matrix4x3 projection;
    float projection_angle;
    float projection_cosine;
    float edge_angle;
    float edge_cosine;
    unsigned long flags;
    s_decal_projection_builder() : flags(0) {}
    void initialize(const real_matrix4x3 *, float, float, bool);
    bool build_projection(const s_fold *fold);
    __forceinline bool get_left_handed() const { return ((flags >> 3) & 1) != 0; }
    __forceinline bool get_needs_renormalize() const { return ((flags >> 4) & 1) != 0; }
    __forceinline void set_initialized(bool value) { if (value) flags |= 1; else flags &= ~1; }
    __forceinline void set_built(bool value) { if (value) flags |= 2; else flags &= ~2; }
    __forceinline void set_left_handed(bool value) { if (value) flags |= 8; else flags &= ~8; }
    __forceinline void set_needs_renormalize(bool value) { if (value) flags |= 16; else flags &= ~16; }
};
struct s_fold {
    long surface;
    long instance_index;
    long neighbor;
    union
    {
        char data[36];
        struct
        {
            real_vector3d normal;
            union { real_point3d point; real_vector3d point_offset; };
            real_vector3d axis;
        };
    };
};
struct s_decal_neighbor_surface {
    long index;
    s_fold fold;
    s_decal_projection_builder projection;
};
struct s_decal_mesh_builder {
    long instance_index;
    bool floating;
    long count;
    s_decal_neighbor_surface surfaces[128];
    bool push_neighbor_surface(long, const s_fold *);
};
typedef char decal_mesh_builder_count_offset[(offsetof(s_decal_mesh_builder, count) == 8) ? 1 : -1];


typedef unsigned int uint;

struct s_decals_system_definition;
struct s_decal_collision_result;


typedef char decal_layout_check_1[(sizeof(c_decal) == 0xc4) ? 1 : -1];
typedef char decal_layout_check_2[(offsetof(c_decal, m_half_size) == 0x2c) ? 1 : -1];
typedef char decal_layout_check_3[(offsetof(c_decal, m_vertices) == 0x44) ? 1 : -1];
typedef char decal_layout_check_4[(sizeof(c_decal_system) == 0x50) ? 1 : -1];

// @address 0x7830c0 g_decal_preplaced_vertex_allocator
c_decal::c_vertex_allocator g_decal_preplaced_vertex_allocator(10240);
// @address 0x7830d8 g_decal_preplaced_index_allocator
c_decal::c_index_allocator g_decal_preplaced_index_allocator(40960);
// @address 0x7830f0 g_decal_runtime_vertex_allocator
c_decal::c_vertex_allocator g_decal_runtime_vertex_allocator(2048);
// @address 0x783108 g_decal_runtime_index_allocator
c_decal::c_index_allocator g_decal_runtime_index_allocator(8192);


struct s_decals_instance_bucket_element { long mesh_index; s_tag_block instances; };

struct s_decal_cylinder {
    real_matrix4x3 matrix;
    float radius;
    float height;
    __forceinline bool contains_point(const real_point3d *point) const
    {
        return collision_point_in_cylinder(point, &matrix, radius, height, 0);
    }
};

class c_instanced_geometry_iterator {
public:
    __forceinline c_instanced_geometry_iterator(const s_decal_cylinder *cylinder, const s_cluster_reference *cluster_references,
        long ignore_instance_index)
    {
        m_cylinder = cylinder;
        m_cluster_references = cluster_references;
        m_structure_bsp_index = -1;
        m_cluster_reference_index = 0;
        m_bucket_element_count = 0;
        m_bucket_element_index = 0;
        m_instance_count = 0;
        m_instance_reference_index = 0;
        m_instance_index = -1;
        m_instance = 0;
        m_instance_flags.clear();
        if (ignore_instance_index != -1)
            m_instance_flags.set(ignore_instance_index, true);
        next();
    }
    void next();
    __forceinline const structure_instanced_geometry_instance *get_datum() const { return m_instance; }
    __forceinline unsigned short get_structure_bsp_index() const { return (unsigned short)m_structure_bsp_index; }
    __forceinline unsigned short get_instance_index() const { return (unsigned short)m_instance_index; }
private:
    bool next_cluster();
    bool next_instance_bucket();
    bool next_instance();

    const s_decal_cylinder *m_cylinder;
    const s_cluster_reference *m_cluster_references;
    long m_structure_bsp_index;
    unsigned long m_cluster_reference_index;
    structure_bsp *m_structure_bsp;
    structure_cluster *m_cluster;
    s_mesh *m_bucket;
    unsigned long m_bucket_element_count;
    unsigned long m_bucket_element_index;
    s_decals_instance_bucket_element *m_bucket_element;
    c_static_flags_no_init<4096> m_instance_flags;
    unsigned long m_instance_count;
    unsigned long m_instance_reference_index;
    long m_instance_index;
    structure_instanced_geometry_instance *m_instance;
};

class c_collision_surface_edge_iterator
{
public:
    __forceinline c_collision_surface_edge_iterator(long structure_index, long instance_index, long surface_index);
    structure_bsp *structure;
    collision_bsp *bsp;
    long structure_index;
    long instance_index;
    long surface_index;
    collision_surface *surface;
    real_plane3d plane;
    long first_edge;
    bool get_opposing_surface_fold(const real_point3d *start, const real_point3d *end, s_fold *fold, bool coplanar, bool same_material) const;
};

struct c_collision_surface_vertex_iterator : c_collision_surface_edge_iterator
{
    c_collision_surface_vertex_iterator(long structure_index, long instance_index, long surface_index);
};

struct s_decal_collision_result : collision_result {
    __forceinline long get_type() const { return type; }
    __forceinline const real_point3d *get_point() const { return &point; }
    __forceinline const real_vector3d *get_normal() const { return &plane.n; }
    __forceinline const real_plane3d *get_plane() const { return &plane; }
    __forceinline long get_structure_bsp_index() const { return structure_bsp_index; }
    __forceinline long get_surface_index() const { return surface_index; }
    __forceinline long get_instanced_geometry_instance_index() const { return instance_index; }
    __forceinline const s_cluster_reference *get_cluster_reference() const { return &location.cluster_reference; }
    __forceinline unsigned short get_material_index() const { return material_index; }
};


struct s_decals_sort_item
{
    union { unsigned long key; const real_point3d *position; };
    real_point2d texcoord;
    real_vector3d normal, up;
};

struct s_decals_weld_group { unsigned short start; unsigned short count; };

struct s_decals_weld_sort {
    unsigned short order[1024];
    unsigned short start;
    unsigned short count;
    const void *context;
    bool (*compare)(short, short, const void *);
};

struct s_decals_weld_buffers {
    s_decals_sort_item items[1024];
    unsigned short indices[1024];
    long item_count, index_count;
    long vertex_count;
    long group_count;
    unsigned short order_area[1024];
    unsigned short range_start;
    unsigned short range_count;
    const void *sort_context;
    bool (*sort_compare)(short, short, const void *);
    unsigned short remap[1024];
    s_decals_weld_group groups[1024];
};

struct s_decals_weld_batch {
    const real_matrix4x3 *transform;
    const s_decal_collision_result *collision;
    bool offset;
    unsigned long start;
    unsigned long count;
    long index_start;
    long index_count;
    long vertex_base;
    long group_count;
    long group_base;
    long output_index_count;
};

struct s_collision_tester {
    s_collision_test_flags flags;
    real_point3d point;
    real_vector3d vector;
    float radius;
    long ignore_object_index;
    long structure_index;
    long instance_index;
    __forceinline void set_length(float length)
    {
        scale_vector3d(&vector, length / magnitude3d(&vector), &vector);
    }
    __forceinline void offset_by(const real_vector3d *direction, float distance)
    {
        point_from_line3d(&point, direction, distance, &point);
    }
    __forceinline void nudge_by(const real_vector3d *direction, float distance)
    {
        point_from_line3d(&vector, direction, distance, &vector);
    }
    __forceinline bool test_vector(s_decal_collision_result *collision) const;
};


// @address 0x22d3a0 ?get_vertex_allocator@c_decal@@ABAPAVc_vertex_allocator@1@XZ
#if defined(_XBOX) && !defined(__clang__)
// match: 0x2303d0: inline keeps both allocator lookups expanded in decal_destroy (8 bytes shorter without it)
inline
#endif
c_decal::c_vertex_allocator *c_decal::get_vertex_allocator() const
{
    s_data_array *array = g_decal_system_data;
    const c_decal_system *entry = (const c_decal_system *)(array->data + (unsigned short)m_system_index * sizeof(c_decal_system));
    if (entry->m_flags & 1)
        return &g_decal_preplaced_vertex_allocator;
    return &g_decal_runtime_vertex_allocator;
}

// @address 0x22d3f0 ?get_index_allocator@c_decal@@ABAPAVc_index_allocator@1@XZ
#if defined(_XBOX) && !defined(__clang__)
// match: 0x2321c0: inline keeps both allocator lookups expanded in decals_build_decal_mesh (39 words differ without it)
inline
#endif
c_decal::c_index_allocator *c_decal::get_index_allocator() const
{
    s_data_array *array = g_decal_system_data;
    const c_decal_system *entry = (const c_decal_system *)(array->data + (unsigned short)m_system_index * sizeof(c_decal_system));
    if (entry->m_flags & 1)
        return &g_decal_preplaced_index_allocator;
    return &g_decal_runtime_index_allocator;
}

// @address 0x22d2f8 ?__tls_set_g_decal_message_queue_allocator@@YAXPAX@Z
void __tls_set_g_decal_message_queue_allocator(void *address)
{
    g_decal_counts = (s_decals_pass_counts *)address;
}

// @address 0x22d308 ?create_buffer@c_vertex_allocator@c_decal@@UAAXXZ
void c_decal::c_vertex_allocator::create_buffer()
{
    m_buffer.create((c_rasterizer_vertex_buffer::e_vertex_buffer_declaration)1, m_maximum_count);
}

// @address 0x22d318 ?create_buffer@c_index_allocator@c_decal@@UAAXXZ
void c_decal::c_index_allocator::create_buffer()
{
    m_buffer.create(c_rasterizer_index_buffer::_primitive_type_triangle_strip, m_maximum_count);
}


// @address 0x22d758 ?next_cluster@c_instanced_geometry_iterator@@AAA_NXZ
bool c_instanced_geometry_iterator::next_cluster()
{
    if (m_cluster_reference_index < 4)
    {
        s_cluster_reference reference = m_cluster_references[m_cluster_reference_index];
        if (reference.bsp_index != -1)
        {
            m_structure_bsp_index = reference.bsp_index;
            m_structure_bsp = global_structure_bsp_get(m_structure_bsp_index);
            m_cluster = TAG_BLOCK_GET_ELEMENT(&m_structure_bsp->clusters, reference.cluster_index, structure_cluster);
            m_bucket = TAG_BLOCK_GET_ELEMENT(
                &m_structure_bsp->render_geometry.meshes, m_cluster->mesh_index, s_mesh);
            m_bucket_element_count = m_bucket->instance_buckets.count;
            m_cluster_reference_index++;
            return next_instance_bucket();
        }
    }
    return false;
}

// @address 0x22d830 ?next_instance_bucket@c_instanced_geometry_iterator@@AAA_NXZ
inline bool c_instanced_geometry_iterator::next_instance_bucket()
{
    if (m_bucket_element_index < m_bucket_element_count)
    {
        m_bucket_element = TAG_BLOCK_GET_ELEMENT(&m_bucket->instance_buckets, m_bucket_element_index, s_decals_instance_bucket_element);
        m_instance_count = m_bucket_element->instances.count;
        m_bucket_element_index++;
        return next_instance();
    }
    m_bucket_element_index = 0;
    return next_cluster();
}

// @address 0x22d878 ?next_instance@c_instanced_geometry_iterator@@AAA_NXZ
bool c_instanced_geometry_iterator::next_instance()
{
    while (m_instance_reference_index < m_instance_count)
    {
        m_instance_index = *TAG_BLOCK_GET_ELEMENT(&m_bucket_element->instances, m_instance_reference_index, unsigned short);
        if (m_instance_flags.test(m_instance_index))
        {
            m_instance_reference_index++;
            continue;
        }
        m_instance_flags.set(m_instance_index, true);
        break;
    }
    if (m_instance_reference_index < m_instance_count)
    {
        m_instance = TAG_BLOCK_GET_ELEMENT(&m_structure_bsp->instanced_geometry_instances,
            m_instance_index, structure_instanced_geometry_instance);
        m_instance_reference_index++;
        return true;
    }
    m_instance = 0;
    m_instance_index = -1;
    m_instance_reference_index = 0;
    return next_instance_bucket();
}

// @address 0x22d6c8 ?next@c_instanced_geometry_iterator@@QAAXXZ
void c_instanced_geometry_iterator::next()
{
    while (next_instance())
    {
        float sphere_radius = m_instance->world_bounding_sphere_radius;
        if (collision_point_in_cylinder(&m_instance->world_bounding_sphere_center, &m_cylinder->matrix, m_cylinder->radius + sphere_radius,
                m_cylinder->height + 2.0f * sphere_radius, 0))
            break;
    }
}

/* ---- collision surface vertex iterator ---- */

// @address 0x7e8c48 g_decals_scratch_plane
real_plane3d g_decals_scratch_plane;

struct s_collision_bsp_cache;
static __forceinline real_plane3d *decals_collision_plane_from_designator(collision_bsp *bsp, long designator, real_plane3d *result)
{
    real_plane3d *plane = TAG_BLOCK_GET_ELEMENT(&bsp->planes, designator & 0x7fff, real_plane3d);
    if (designator & 0x8000) {
        result->n.i = -plane->n.i;
        result->n.j = -plane->n.j;
        result->n.k = -plane->n.k;
        result->d = -plane->d;
    } else {
        *result = *plane;
    }
    return result;
}


static __forceinline bool decals_collision_surface_valid(const collision_surface *surface)
{
    return (surface->flags & 59) == 0 && surface->material != -1;
}

static __forceinline collision_bsp *decals_surface_query_collision_bsp(const s_decal_collision_result *query)
{
    collision_bsp *collision_bsp = 0;
    if (query->instance_index != -1)
    {
        structure_bsp *structure = global_structure_bsp_get(query->structure_bsp_index);
        short definition_index = TAG_BLOCK_GET_ELEMENT(
            &structure->instanced_geometry_instances, query->instance_index, structure_instanced_geometry_instance)
                                     ->definition_index;
        s_collision_bsp_cache_user *cache = &g_collision_bsp_cache.thread_users[get_current_thread_index()];
        structure_instanced_geometry_definition *instance
            = &((structure_instanced_geometry_definition *)cache->instanced_geometry_definitions[query->structure_bsp_index].address)[definition_index];
        if (!instance->render_bsp.count)
            collision_bsp = &instance->bsp;
    }
    else
    {
        s_collision_bsp_cache_user *cache = &g_collision_bsp_cache.thread_users[get_current_thread_index()];
        collision_bsp = const_cast<struct collision_bsp *>(static_cast<const struct collision_bsp *>(cache->collision_bsps[query->structure_bsp_index]));
    }
    return collision_bsp;
}

// @address 0x22d498 decals_query_has_valid_surface
extern "C" bool decals_query_has_valid_surface(const s_decal_collision_result *query)
{
    collision_bsp *collision_bsp;
    return query->type != 4
        && (collision_bsp = decals_surface_query_collision_bsp(query)) != 0
        && decals_collision_surface_valid(TAG_BLOCK_GET_ELEMENT(&collision_bsp->surfaces, query->surface_index, collision_surface));
}

__forceinline c_collision_surface_edge_iterator::c_collision_surface_edge_iterator(long structure_index, long instance_index, long surface_index)
{
    structure = global_structure_bsp_get(structure_index);
    collision_bsp *collision_bsp;
    if (instance_index != -1) {
        s_collision_bsp_cache_user *cache = &g_collision_bsp_cache.thread_users[get_current_thread_index()];
        collision_bsp = &((structure_instanced_geometry_definition *)cache->instanced_geometry_definitions[structure_index].address)[instance_index].bsp;
    } else {
        collision_bsp = const_cast<struct collision_bsp *>(static_cast<const struct collision_bsp *>(g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[structure_index]));
    }
    bsp = collision_bsp;
    this->structure_index = structure_index;
    this->instance_index = instance_index;
    this->surface_index = surface_index;
    surface = TAG_BLOCK_GET_ELEMENT(&collision_bsp->surfaces, surface_index, collision_surface);
    plane = *decals_collision_plane_from_designator(collision_bsp, surface->plane, &g_decals_scratch_plane);
    first_edge = surface->first_edge;
}

// @address 0x22d9d0 ??0c_collision_surface_vertex_iterator@@QAA@JJJ@Z
c_collision_surface_vertex_iterator::c_collision_surface_vertex_iterator(long structure_index, long instance_index, long surface_index)
    : c_collision_surface_edge_iterator(structure_index, instance_index, surface_index)
{
}

// @address 0x22dff0 ?initialize@s_decal_projection_builder@@QAAXPBUreal_matrix4x3@@MM_N@Z
void s_decal_projection_builder::initialize(const real_matrix4x3 *matrix, float angle, float edge, bool left_handed)
{
    projection = *matrix;
    projection_angle = angle;
    projection_cosine = cosine(projection_angle);
    edge_angle = edge;
    edge_cosine = cosine(edge_angle);
    set_initialized(true);
    set_built(false);
    set_left_handed(left_handed);
    set_needs_renormalize(true);
}

// @address 0x22e550 ?push_neighbor_surface@s_decal_mesh_builder@@QAA_NJPBUs_fold@@@Z
bool s_decal_mesh_builder::push_neighbor_surface(long index, const s_fold *fold)
{
    if ((unsigned long)count < 128) {
        bool found = false;
        for (long surface_index = 0; surface_index < count; ++surface_index)
            found = found || (surfaces[surface_index].fold.neighbor == fold->neighbor && surfaces[surface_index].fold.surface == fold->surface);
        if (!found) {
            s_decal_neighbor_surface *surface = &surfaces[count];
            ++count;
            surface->index = index;
            surface->fold = *fold;
            return true;
        }
    }
    return false;
}



__forceinline void c_decal::initialize_for_game()
{
    g_decal_array = data_new("decal", 704, sizeof(c_decal), 0, (c_allocation_base *)&g_decal_array_allocator);
    g_decal_preplaced_vertex_allocator.initialize_for_game();
    g_decal_preplaced_index_allocator.initialize_for_game();
    g_decal_runtime_vertex_allocator.initialize_for_game();
    g_decal_runtime_index_allocator.initialize_for_game();
}
// @address 0x22e650 ?initialize@c_decal_system@@SAXXZ
void c_decal_system::initialize()
{
    g_decal_system_data = data_new("decal_system", 704, sizeof(c_decal_system), 0, (c_allocation_base *)&g_decal_system_allocator);
    g_decal_counts =
        (s_decals_pass_counts *)((void *(*)(s_decals_restricted_allocation *, const char *, const char *, unsigned int,
            long))decals_reserve_counts_memory)(
            (s_decals_restricted_allocation *)&g_decal_counts_allocator, "decal counts", 0, 2 * sizeof(s_decals_pass_counts), 0);
    g_decal_message_queue = static_cast<t_message_queue<c_decal_message_union, 256> *>(
        g_decal_message_queue_allocator.reserve_memory("decal messaging queue", 0, 2084, 4));
    g_decal_message_queue->clear();
    c_decal::initialize_for_game();
    s_decals_pass_counts *counts = g_decal_counts;
    counts[0].system_count = 0;
    counts[0].child_count = 0;
    counts[0].first_cache_total = 0;
    counts[0].second_cache_total = 0;
    counts[1].system_count = 0;
    counts[1].child_count = 0;
    counts[1].first_cache_total = 0;
    counts[1].second_cache_total = 0;
}

// @address 0x22e788 ?initialize_for_new_map@c_decal_system@@SAXXZ
void c_decal_system::initialize_for_new_map()
{
    s_data_array *array = g_decal_system_data;
    array->valid = 1;
    data_delete_all(array);
    s_data_array *second = g_decal_array;
    second->valid = 1;
    data_delete_all(second);
    s_decals_pass_counts *words = g_decal_counts;
    words[0].system_count = 0;
    words[0].child_count = 0;
    words[0].first_cache_total = 0;
    words[0].second_cache_total = 0;
    words[1].system_count = 0;
    words[1].child_count = 0;
    words[1].first_cache_total = 0;
    words[1].second_cache_total = 0;
    memset(&g_decal_system_pvs, 0, sizeof(g_decal_system_pvs));
}


// @address 0x22e820 ?dispose_from_old_map@c_decal_system@@SAXXZ
void c_decal_system::dispose_from_old_map()
{
    ((s_data_array *)g_decal_array)->valid = 0;
    ::lruv_flush(g_decal_preplaced_index_allocator.get_cache());
    ::lruv_flush(g_decal_preplaced_vertex_allocator.get_cache());
    ::lruv_flush(g_decal_runtime_index_allocator.get_cache());
    ::lruv_flush(g_decal_runtime_vertex_allocator.get_cache());
    (g_decal_system_data)->valid = 0;
    s_decals_pass_counts *counts = g_decal_counts;
    counts[0].system_count = 0;
    counts[0].child_count = 0;
    counts[0].first_cache_total = 0;
    counts[0].second_cache_total = 0;
    counts[1].system_count = 0;
    counts[1].child_count = 0;
    counts[1].first_cache_total = 0;
    counts[1].second_cache_total = 0;
}

typedef c_decal::c_mesh_allocator<c_rasterizer_vertex_buffer, rasterizer_vertex_world> t_decal_vertex_allocator;
typedef c_decal::c_mesh_allocator<c_rasterizer_index_buffer, unsigned short> t_decal_index_allocator;


// @address 0x22ebc0 decal_system_destroy_all
extern "C" void decal_system_destroy_all(void)
{
    c_data_iterator<void> cursor;
    int empty_pvs[128];
    cursor.m_iterator.data = g_decal_system_data;
    cursor.m_iterator.absolute_index = -1;
    cursor.m_iterator.index = -1;
    for (cursor.m_datum = ::data_iterator_next(&cursor.m_iterator); cursor.m_datum;
        cursor.m_datum = ::data_iterator_next(&cursor.m_iterator))
        c_decal_system::destroy(cursor.m_iterator.index);
    memset(empty_pvs, 0, sizeof(empty_pvs));
    ((void (*)(const s_game_cluster_bit_vectors *, const s_game_cluster_bit_vectors *, long))
        c_decal_system::change_pvs)((const s_game_cluster_bit_vectors *)empty_pvs,
        (const s_game_cluster_bit_vectors *)(&game_globals->cluster_pvs_local), 1);
    memset(&g_decal_system_pvs, 0, sizeof(g_decal_system_pvs));
}

// @address 0x22e9c0 ?initialize_for_new_structure_bsp@c_decal_system@@SAXK@Z
void c_decal_system::initialize_for_new_structure_bsp(unsigned long activating_structure_bsp_mask)
{
    c_data_iterator<void> cursor;
    cursor.m_iterator.data = g_decal_system_data;
    cursor.m_iterator.absolute_index = -1;
    cursor.m_iterator.index = -1;
    for (cursor.m_datum = ::data_iterator_next(&cursor.m_iterator); cursor.m_datum;
        cursor.m_datum = ::data_iterator_next(&cursor.m_iterator))
        ;
}


// @address 0x14c058 ?frame_advance_all@c_decal_system@@SAXM@Z
void c_decal_system::frame_advance_all(float dt)
{
    const void *new_pvs = &game_globals->cluster_pvs_local;
    if (!g_decal_system_pvs_locked && memcmp(&g_decal_system_pvs, new_pvs, sizeof(g_decal_system_pvs)) != 0)
        ((void (*)(const s_game_cluster_bit_vectors *, const s_game_cluster_bit_vectors *, long))
            c_decal_system::change_pvs)(&g_decal_system_pvs,
            (const s_game_cluster_bit_vectors *)new_pvs, 1);
    if (dt <= visibility_projections_and_volumes_zero)
        return;
    c_data_iterator<void> cursor;
    cursor.m_iterator.data = g_decal_system_data;
    cursor.m_iterator.absolute_index = -1;
    cursor.m_iterator.index = -1;
    for (cursor.m_datum = ::data_iterator_next(&cursor.m_iterator); cursor.m_datum;
        cursor.m_datum = ::data_iterator_next(&cursor.m_iterator))
    {
        c_decal_system *parent = (c_decal_system *)cursor.m_datum;
        if (parent->m_flags & 1)
            continue;
        c_decal *prev = 0;
        long child_index = parent->m_first_child_index;
        while (child_index != -1) {
            c_decal *child = (c_decal *)(g_decal_array->data + (child_index & 0xffff) * sizeof(c_decal));
            child->m_age += dt;
            if (child->m_age > child->m_lifespan)
                child->m_flags &= ~1;
            long current_child = child_index;
            child_index = child->m_next_decal_index;
            if (child->m_flags & 1) {
                prev = child;
            } else {
                if (prev)
                    prev->m_next_decal_index = child->m_next_decal_index;
                else
                    parent->m_first_child_index = child->m_next_decal_index;
                decal_destroy(current_child);
            }
        }
        if (parent->m_first_child_index == -1)
            c_decal_system::destroy(cursor.m_iterator.index);
    }
}

// @address 0x22ea20 ?dispose_from_old_structure_bsp@c_decal_system@@SAXK@Z
void c_decal_system::dispose_from_old_structure_bsp(unsigned long structure_mask)
{
    c_data_iterator<void> cursor;
    cursor.m_iterator.data = g_decal_system_data;
    cursor.m_iterator.absolute_index = -1;
    cursor.m_iterator.index = -1;
    for (cursor.m_datum = ::data_iterator_next(&cursor.m_iterator); cursor.m_datum;
        cursor.m_datum = ::data_iterator_next(&cursor.m_iterator))
    {
        c_decal_system *parent = (c_decal_system *)cursor.m_datum;
        s_cluster_reference *cluster_reference = &parent->m_cluster_references[0];
        if ((parent->m_flags & 1) && (((1 << cluster_reference->bsp_index) & structure_mask) != 0))
            c_decal_system::destroy(cursor.m_iterator.index);
    }
    for (long index = 0; index < 16; index++) {
        if (structure_mask & (1 << index)) {
            unsigned long *bits = g_decal_system_pvs.bit_vectors[index].get_writeable_bits_direct();
            for (long word = 0; word < 8; word++)
                bits[word] = 0;
        }
    }
}

// @address 0x22eb18 decal_system_remove_invalid_definitions
extern "C" void decal_system_remove_invalid_definitions(const s_game_non_bsp_zone_set *, const s_game_non_bsp_zone_set *,
    c_scenario_resource_registry *pending_zone_registry)
{
    c_data_iterator<void> cursor;
    cursor.m_iterator.data = g_decal_system_data;
    cursor.m_iterator.absolute_index = -1;
    cursor.m_iterator.index = -1;
    for (cursor.m_datum = ::data_iterator_next(&cursor.m_iterator); cursor.m_datum;
        cursor.m_datum = ::data_iterator_next(&cursor.m_iterator))
    {
        c_decal_system *parent = (c_decal_system *)cursor.m_datum;
        long definition_index = datum_index_to_absolute_index(parent->m_definition_index);
        if (!pending_zone_registry->is_tag_available(definition_index))
            c_decal_system::destroy(cursor.m_iterator.index);
    }
}

// @address 0x22ec70 ?adopt_old_render_message_queue@c_decal_system@@SAXPAX@Z
void c_decal_system::adopt_old_render_message_queue(void *old_queue)
{
    g_decal_message_queue->try_to_prepend_message_queue(static_cast<t_message_queue<c_decal_message_union, 256> *>(old_queue));
}

// @address 0x9d0d0 ?matrix3x3_rotation_from_quaternion@@YAPAUreal_matrix3x3@@PAU1@PBUreal_quaternion@@@Z

// @address 0x22efd8 ?add_cluster_to_pvs@c_decal_system@@SAXPBUs_cluster_reference@@@Z
void c_decal_system::add_cluster_to_pvs(const s_cluster_reference *cluster_reference)
{
    structure_bsp *structure_bsp = global_structure_bsp_get(cluster_reference->bsp_index);
    structure_cluster *cluster = TAG_BLOCK_GET_ELEMENT(&structure_bsp->clusters, cluster_reference->cluster_index, structure_cluster);
    for (long index = 0; index < (unsigned short)cluster->runtime_decal_count; index++) {
        s_tag_reference *palette = (s_tag_reference *)global_scenario->decal_palette.address;
        s_preplaced_decal *decal = ((s_preplaced_decal *)structure_bsp->runtime_decals.address) + cluster->runtime_first_decal_index + index;
        long definition_index = palette[decal->palette_index].index;
        if (definition_index != -1) {
            real_orientation *orientation = &decal->orientation;
            real_matrix3x3 matrix;
            matrix3x3_rotation_from_quaternion(&matrix, &orientation->rotation);
            real_vector3d up;
            up.i = -matrix.up.i;
            up.j = -matrix.up.j;
            up.k = -matrix.up.k;
            real_vector3d forward;
            forward.i = -matrix.forward.i;
            forward.j = -matrix.forward.j;
            forward.k = -matrix.forward.k;
            decal_system_create(0, -1, definition_index, -1, true, &orientation->translation, &up, &forward);
        }
    }
    g_decal_system_pvs.bit_vectors[cluster_reference->bsp_index].set(cluster_reference->cluster_index, true);
}

// @address 0x22f148 decal_system_remove_cluster_from_pvs
extern "C" void decal_system_remove_cluster_from_pvs(const s_cluster_reference *cluster_reference)
{
    c_data_iterator<void> cursor;
    cursor.m_iterator.data = g_decal_system_data;
    cursor.m_iterator.absolute_index = -1;
    cursor.m_iterator.index = -1;
    for (cursor.m_datum = ::data_iterator_next(&cursor.m_iterator); cursor.m_datum;
        cursor.m_datum = ::data_iterator_next(&cursor.m_iterator))
    {
        c_decal_system *parent = (c_decal_system *)cursor.m_datum;
        if ((parent->m_flags & 1) && (parent->m_cluster_references[0].value() == cluster_reference->value()))
            c_decal_system::destroy(cursor.m_iterator.index);
    }
    g_decal_system_pvs.bit_vectors[cluster_reference->bsp_index].set(cluster_reference->cluster_index, false);
}

// @address 0x22f760 ?destroy@c_decal_system@@SAXJ@Z
void c_decal_system::destroy(long parent_index)
{
    if (parent_index != -1) {
        c_decal_system *parent =
            (c_decal_system *)((g_decal_system_data)->data + (parent_index & 0xffff) * sizeof(c_decal_system));
        long child_index = parent->m_first_child_index;
        while (child_index != -1) {
            long deleted_index = child_index;
            c_decal *child = (c_decal *)(g_decal_array->data + (child_index & 0xffff) * sizeof(c_decal));
            child_index = child->m_next_decal_index;
            decal_destroy(deleted_index);
        }
        (g_decal_counts + (parent->m_flags & 1))->system_count--;
        ::datum_delete((s_data_array *)(g_decal_system_data), parent_index);
    }
}

#define DECALS_MAX(a,b) ((a)>(b)?(a):(b))
#define DECALS_MIN(a,b) ((a)<(b)?(a):(b))

// @address 0x230a88 ?line_segment_intersects_unit_texture_tile@@YA_NPBTreal_point2d@@0@Z
bool line_segment_intersects_unit_texture_tile(const real_point2d *p1, const real_point2d *p2)
{
    real_point2d a = { p1->x - 0.5f, p1->y - 0.5f };
    real_point2d b = { p2->x - 0.5f, p2->y - 0.5f };
    real_vector2d delta = { b.x - a.x, b.y - a.y };
    float t;
    if (magnitude_squared2d(&delta) > 0.0f) {
        float result;
        if (delta.i * delta.j >= 0.0f) result = -(a.y + a.x) / (delta.j + delta.i);
        else result = -(a.y - a.x) / (delta.j - delta.i);
        t = result;
    } else t = 0.0f;
    t = DECALS_MIN(DECALS_MAX(t, 0.0f), 1.0f);
    real_point2d point = { a.x * (1.0f - t) + b.x * t, a.y * (1.0f - t) + b.y * t };
    bool result = DECALS_MAX(fabsf(point.x), fabsf(point.y)) < 0.5f;
    return result;
}


// @address 0x2319c8 decals_compare_weld_items
extern "C" bool decals_compare_weld_items(short left, short right, const void *context)
{
    const s_decals_sort_item *items = (const s_decals_sort_item *)context;
    const s_decals_sort_item *a = &items[left];
    const s_decals_sort_item *b = &items[right];
    if (a->key > b->key) return true;
    if (a->key < b->key) return false;
    if (a->texcoord.x > b->texcoord.x) return true;
    if (a->texcoord.x < b->texcoord.x) return false;
    if (a->texcoord.y > b->texcoord.y) return true;
    return false;
}


// @address 0x231a40 decals_weld_vertices
extern "C" void decals_weld_vertices(s_decals_weld_buffers *buffers, s_decals_weld_batch *batch)
{
    batch->vertex_base = buffers->vertex_count;
    batch->group_base = buffers->group_count;
    batch->group_count = 0;
    batch->output_index_count = 0;
    buffers->range_start = (unsigned short)batch->start;
    buffers->range_count = (unsigned short)batch->count;
    s_decals_weld_sort *sort = (s_decals_weld_sort *)buffers->order_area;
    for (long index = 0; index < (sort->count > 1024 ? 1024 : sort->count); ++index) {
        long position = sort->start + index;
        sort->order[position] = position;
    }
    if (sort->count >= 2)
        templated_sort<short>::qsort((short *)&sort->order[sort->start], sort->count, sort->compare, sort->context);
    unsigned long cursor = batch->start;
    while (cursor < batch->start + batch->count) {
        unsigned long first = cursor;
        unsigned short representative = buffers->order_area[cursor];
        unsigned short vertex = (unsigned short)(batch->group_count + batch->vertex_base);
        ++cursor;
        buffers->remap[representative] = vertex;
        const s_decals_sort_item *item = &buffers->items[representative];
        while (cursor < batch->start + batch->count) {
            unsigned short other_index = buffers->order_area[cursor];
            const s_decals_sort_item *other = &buffers->items[other_index];
            if (other->key != item->key)
                break;
            if (fabsf(other->texcoord.x - item->texcoord.x) >= 0.02f)
                break;
            if (fabsf(other->texcoord.y - item->texcoord.y) >= 0.02f)
                break;
            ++cursor;
            buffers->remap[other_index] = vertex;
        }
        buffers->groups[vertex].start = (unsigned short)first;
        buffers->groups[vertex].count = (unsigned short)(cursor - first);
        ++batch->group_count;
    }
    batch->output_index_count = batch->index_count;
    buffers->vertex_count += batch->group_count;
    buffers->group_count += batch->output_index_count;
}


// @address 0x232580 decals_reserve_counts_memory
extern "C" void *decals_reserve_counts_memory(s_decals_restricted_allocation *allocation, const char *name, const char *type, unsigned int size)
{
    long member_index = g_restricted_regions[4].add_member(name, type, size, 0, __tls_set_g_decal_message_queue_allocator, 0, 0);
    void *address = restricted_region_get_member_address(4, member_index);
    g_decal_counts = (s_decals_pass_counts *)address;
    allocation->m_member_index = member_index;
    allocation->m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(4, member_index);
}


// @address 0x232620 ?allocate@?$t_restricted_allocation_manager@$03$0A@$0A@$1?__tls_set_g_decal_message_queue_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_message_queue_allocator>::allocate(unsigned int size, const char *name)
{
    return ((void *(*)(s_decals_restricted_allocation *, const char *, const char *, unsigned int, long))decals_reserve_counts_memory)(
        (s_decals_restricted_allocation *)this, name, "", size, 0);
}

// @address 0x232638 ?deallocate@?$t_restricted_allocation_manager@$03$0A@$0A@$1?__tls_set_g_decal_message_queue_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_message_queue_allocator>::deallocate(void *allocation)
{
    long m_member_index = this->m_member_index;
    g_decal_counts = 0;
    restricted_region_free_member(4, m_member_index);
}


// @address 0x232708 decals_dispose_gpu_buffer
extern "C" void decals_dispose_gpu_buffer(t_decal_vertex_allocator *buffer)
{
    s_lruv_cache *block = buffer->get_cache();
    c_allocation_base *allocator = block->allocator;
    data_dispose((s_data_array *)block->header.block_data);
    allocator->deallocate(block);
    buffer->clear_cache();
    if (buffer->get_buffer()->m_d3d_buffer)
        D3DResource_Release((D3DResource *)buffer->get_buffer()->m_d3d_buffer);
}

template<class t_buffer, class t_vertex>
__forceinline void c_decal::c_mesh_allocator<t_buffer, t_vertex>::dispose_gpu_block(s_lruv_cache *block)
{
    c_allocation_base *allocator = block->allocator;
    data_dispose((s_data_array *)block->header.block_data);
    allocator->deallocate(block);
    clear_cache();
}

// @address 0x22e8d8 ?dispose_from_game@c_decal@@SAXXZ
void c_decal::dispose_from_game()
{
    g_decal_array = 0;
    g_decal_preplaced_index_allocator.dispose_gpu_block(g_decal_preplaced_index_allocator.get_cache());
    decals_dispose_gpu_buffer(&g_decal_preplaced_vertex_allocator);
    g_decal_runtime_index_allocator.dispose_gpu_block(g_decal_runtime_index_allocator.get_cache());
    decals_dispose_gpu_buffer(&g_decal_runtime_vertex_allocator);
    s_decals_pass_counts *words = g_decal_counts;
    g_decal_counts = 0;
    words[0].system_count = 0;
    words[0].child_count = 0;
    words[0].first_cache_total = 0;
    words[0].second_cache_total = 0;
    words[1].system_count = 0;
    words[1].child_count = 0;
    words[1].first_cache_total = 0;
    words[1].second_cache_total = 0;
    g_decal_system_data = 0;
}

// @address 0x232858 ?reserve_memory@?$t_restricted_allocation_manager@$02$1?adopt_old_render_message_queue@c_decal_system@@SAXPAX@Z$0A@$1?__tls_set_g_decal_message_queue_allocator@@YAX0@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, &c_decal_system::adopt_old_render_message_queue, static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_message_queue_allocator>::reserve_memory(const char *name, const char *type, unsigned int size, long alignment)
{
    m_member_index = restricted_region_register_member(3, name, type, size, alignment, &__tls_set_g_first_person_weapon_orientations_allocator,
        &c_decal_system::adopt_old_render_message_queue, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x232ab0 ?clear@?$t_message_queue@Vc_decal_message_union@@$0BAA@@@QAAXXZ
template<class t_message, long k_count> void t_message_queue<t_message, k_count>::clear()
{
    message_count.set(0);
    in_use.set_array(0);
}
#if !defined(_XBOX) || defined(__clang__)
template void t_message_queue<c_decal_message_union, 256>::clear();
#endif

void __tls_set_g_decal_array_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_contrail_location_data, (s_data_array *)object);
}

void __tls_set_g_decal_system_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_leaf_system_data, (s_data_array *)object);
}

// @address 0x428800 ?usage_block_proc@?$c_mesh_allocator@Vc_rasterizer_vertex_buffer@@Urasterizer_vertex_world@@@c_decal@@KAEPAXJ@Z
template<> unsigned char c_decal::c_mesh_allocator<c_rasterizer_vertex_buffer, rasterizer_vertex_world>::usage_block_proc(void *block, long size)
{
    return 255;
}


// @address 0x2328f8 ?allocate@?$t_restricted_allocation_manager@$02$1?adopt_old_render_message_queue@c_decal_system@@SAXPAX@Z$0A@$1?__tls_set_g_decal_message_queue_allocator@@YAX0@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, &c_decal_system::adopt_old_render_message_queue, static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_message_queue_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x232910 ?deallocate@?$t_restricted_allocation_manager@$02$1?adopt_old_render_message_queue@c_decal_system@@SAXPAX@Z$0A@$1?__tls_set_g_decal_message_queue_allocator@@YAX0@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, &c_decal_system::adopt_old_render_message_queue, static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_message_queue_allocator>::deallocate(void *allocation)
{
    g_decal_message_queue = NULL;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x230138 __tls_set_g_decal_data_allocator
void __tls_set_g_decal_data_allocator(void *object)
{
    data_set_new_base_address(&g_decal_array, (s_data_array *)object);
}

// @address 0x2303d0 decal_destroy
extern "C" void decal_destroy(long child_index)
{
    c_decal *child = (c_decal *)(g_decal_array->data + (child_index & 0xffff) * sizeof(c_decal));
    c_decal_system *parent;
    s_decals_pass_counts *counts;
    if (child->m_first_cache_handle != -1) {
        lruv_block_delete(((c_decal::c_mesh_allocator<c_rasterizer_vertex_buffer,
            rasterizer_vertex_world> *)child->get_vertex_allocator())->get_cache(), child->m_first_cache_handle, false);
        parent = (c_decal_system *)((g_decal_system_data)->data + (child->m_system_index & 0xffff) * sizeof(c_decal_system));
        counts = g_decal_counts + (parent->m_flags & 1);
        counts->first_cache_total -= child->m_first_cache_count;
    }
    if (child->m_second_cache_handle != -1) {
        lruv_block_delete(((c_decal::c_mesh_allocator<c_rasterizer_index_buffer,
            unsigned short> *)child->get_index_allocator())->get_cache(), child->m_second_cache_handle, false);
        parent = (c_decal_system *)((g_decal_system_data)->data + (child->m_system_index & 0xffff) * sizeof(c_decal_system));
        counts = g_decal_counts + (parent->m_flags & 1);
        counts->second_cache_total -= child->m_second_cache_count;
    }
    parent = (c_decal_system *)((g_decal_system_data)->data + (child->m_system_index & 0xffff) * sizeof(c_decal_system));
    counts = g_decal_counts + (parent->m_flags & 1);
    counts->child_count -= 1;
    ::datum_delete((s_data_array *)(g_decal_array), child_index);
}

// @address 0x232b88 ?try_to_append_message_queue@?$t_message_queue@Vc_decal_message_union@@$0BAA@@@QAAJPAV1@@Z
template<> long t_message_queue<c_decal_message_union, 256>::try_to_append_message_queue(t_message_queue *source_queue)
{
    long message_count = source_queue->message_count.peek();
    long failed_count = 0;
    for (long index = 0; index < message_count; index++) {
        if (source_queue->in_use.test_bit(index)) {
            if (!try_add_message(&source_queue->messages[index]))
                failed_count++;
        }
    }
    return failed_count;
}

// @address 0x232930 ?allocate@?$t_restricted_allocation_manager@$03$0A@$0A@$1?__tls_set_g_effect_lightprobe_slot_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
__forceinline void *t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_lightprobe_slot_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment)
{
    m_member_index = restricted_region_register_member(4, name, type, size, alignment, &__tls_set_g_effect_lightprobe_slot_data_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(4, m_member_index);
}
template<> void *t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_lightprobe_slot_data_allocator>::allocate(unsigned int size,
    const char *name)
{
    return reserve_memory(name, "", size, 0);
}
template<> __forceinline void t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_lightprobe_slot_data_allocator>::free_memory()
{
    g_decal_system_data = 0;
    restricted_region_free_member(4, m_member_index);
}
// @address 0x2329d0 ?deallocate@?$t_restricted_allocation_manager@$03$0A@$0A@$1?__tls_set_g_effect_lightprobe_slot_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_lightprobe_slot_data_allocator>::deallocate(void *allocation)
{
    free_memory();
}

// @address 0x2329f0 ?allocate@?$t_restricted_allocation_manager@$03$0A@$0A@$1?__tls_set_g_decal_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
__forceinline void *t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment)
{
    m_member_index = restricted_region_register_member(4, name, type, size, alignment, &__tls_set_g_decal_data_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(4, m_member_index);
}
template<> void *t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
template<> __forceinline void t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_data_allocator>::free_memory()
{
    g_decal_array = 0;
    restricted_region_free_member(4, m_member_index);
}
// @address 0x232a90 ?deallocate@?$t_restricted_allocation_manager@$03$0A@$0A@$1?__tls_set_g_decal_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_data_allocator>::deallocate(void *allocation)
{
    free_memory();
}

// @address 0x22d440 ?get_fade@c_decal@@ABAMXZ
float c_decal::get_fade() const
{
    return DECALS_MIN(DECALS_MAX((m_lifespan - m_age) / m_fade_time, 0.0f), 1.0f);
}

__forceinline const s_decals_system_definition *c_decal_system::get_definition() const
{
    return s_decals_system_definition::get(m_definition_index);
}
// @address 0x22ff00 ?check_overlap@c_decal_system@@AAA_NPBTreal_point3d@@@Z
bool c_decal_system::check_overlap(const real_point3d *point)
{
    const s_decals_system_definition *definition = s_decals_system_definition::get(m_definition_index);
    if ((float)definition->overlap_max_count > 0.0f) {
        float radius = definition->radius_max;
        float overlap_radius = radius - definition->overlap_amount;
        long count = 0;
        c_data_iterator<void> cursor;
        cursor.m_iterator.data = g_decal_system_data;
        cursor.m_iterator.absolute_index = -1;
        cursor.m_iterator.index = -1;
        for (cursor.m_datum = ::data_iterator_next(&cursor.m_iterator); cursor.m_datum;
            cursor.m_datum = ::data_iterator_next(&cursor.m_iterator))
        {
            const c_decal_system *other = (const c_decal_system *)cursor.m_datum;
            if (definition == s_decals_system_definition::get(other->m_definition_index)) {
                if ((unsigned char)point_in_sphere(point, other->get_center(), overlap_radius + radius))
                    count++;
            }
        }
        return (float)count < (float)definition->overlap_max_count;
    }
    return true;
}

// @address 0x745cd8 g_decal_distance_inverse
float g_decal_distance_inverse = 1.0f;
static __forceinline float decals_fade_ratio(const s_decals_system_definition *definition, float distance)
{
    return DECALS_MIN(DECALS_MAX((definition->far_distance - distance) / (definition->far_distance - definition->near_distance), 0.0f), 1.0f);
}
static __forceinline float decals_camera_distance(const real_point3d *a, const real_point3d *b)
{
    real_vector3d delta;
delta.j=b->y-a->y;
delta.i=b->x-a->x;
delta.k=b->z-a->z;
    return sqrtf(delta.j*delta.j + delta.k*delta.k + delta.i*delta.i);
}
// @address 0x230060 ?decals_system_get_distance_fade@@YAMPBVc_decal_system@@@Z
float decals_system_get_distance_fade(const c_decal_system *system)
{
    const s_decals_system_definition *definition = s_decals_system_definition::get(system->m_definition_index);
    float distance = decals_camera_distance(&system->m_projection.position,
        &c_player_view::get_current()->rasterizer_camera.position) * g_decal_distance_inverse;
    return decals_fade_ratio(definition, distance);
}

template<class t_buffer, class t_vertex>
static __forceinline unsigned long decals_block_offset(const c_decal::c_mesh_allocator<t_buffer, t_vertex> *allocator, long block_index)
{
    const s_lruv_cache *cache = allocator->get_cache();
    const s_lruv_block *block = &((s_lruv_block *)((s_data_array *)cache->header.block_data)->data)[datum_index_to_absolute_index(block_index)];
    return block->page_index << cache->page_size_bits;
}
// @address 0x230558 ?get_vertex_start@c_decal@@ABAJXZ
long c_decal::get_vertex_start() const
{
    const t_decal_vertex_allocator *allocator = get_vertex_allocator();
    return decals_block_offset(allocator, m_first_cache_handle) >> 5;
}
// @address 0x2305e0 ?get_index_start@c_decal@@ABAJXZ
long c_decal::get_index_start() const
{
    const t_decal_index_allocator *allocator = get_index_allocator();
    return decals_block_offset(allocator, m_second_cache_handle) >> 1;
}


// @address 0x5bec98 ?delete_block_proc@?$c_mesh_allocator@Vc_rasterizer_vertex_buffer@@Urasterizer_vertex_world@@@c_decal@@KAXPAXJ_N@Z
// @address 0x5bec98 ?delete_block_proc@?$c_mesh_allocator@Vc_rasterizer_index_buffer@@G@c_decal@@KAXPAXJ_N@Z
// @address 0x42d380 ?locked_block_proc@?$c_mesh_allocator@Vc_rasterizer_vertex_buffer@@Urasterizer_vertex_world@@@c_decal@@KA_NPAXJ@Z
// @address 0x42d380 ?locked_block_proc@?$c_mesh_allocator@Vc_rasterizer_index_buffer@@G@c_decal@@KA_NPAXJ@Z
// @address 0x232658 ?initialize_for_game@?$c_mesh_allocator@Vc_rasterizer_vertex_buffer@@Urasterizer_vertex_world@@@c_decal@@QAAXXZ
template<> void c_decal::c_mesh_allocator<c_rasterizer_vertex_buffer, rasterizer_vertex_world>::initialize_for_game()
{
    create_buffer();
    m_locked_data = c_rasterizer_vertex_buffer_factory_interface::write_lock((c_rasterizer_vertex_buffer *)(&m_buffer));
    c_rasterizer_vertex_buffer_factory_interface::unlock((c_rasterizer_vertex_buffer *)(&m_buffer));
    m_cache = lruv_new("decal cache", m_maximum_count << 5, 0, 0x2c0, 0, delete_block_proc, locked_block_proc, usage_block_proc,
        (c_allocation_base *)&g_runtime_state_allocation, -1);
}
// match: write_lock is inlined (out of line in March's rasterizer_geometry.cpp; the tree has no out-of-line body)
// into initialize_for_game below.
__forceinline unsigned short *c_rasterizer_index_buffer::write_lock()
{
    unsigned short *locked = 0;
    if (c_rasterizer::get_device()) {
        if (g_restricted_address[2]) locked = (unsigned short *)D3DIndexBuffer_Lock(m_d3d_buffer, 0, 0, 0);
        else locked = (unsigned short *)(UINT_PTR)m_d3d_buffer->Address;
    }
    return locked;
}
// @address 0x232768 ?initialize_for_game@?$c_mesh_allocator@Vc_rasterizer_index_buffer@@G@c_decal@@QAAXXZ
template<> void c_decal::c_mesh_allocator<c_rasterizer_index_buffer, unsigned short>::initialize_for_game()
{
    create_buffer();
    m_locked_data = m_buffer.write_lock();
    c_rasterizer_index_buffer_factory_interface::unlock((c_rasterizer_index_buffer *)(&m_buffer));
    m_cache = lruv_new("decal cache", m_maximum_count << 1, 0, 0x2c0, 0, delete_block_proc, locked_block_proc,
        c_decal::c_mesh_allocator<c_rasterizer_vertex_buffer, rasterizer_vertex_world>::usage_block_proc,
            (c_allocation_base *)&g_runtime_state_allocation,
        -1);
}

// @address 0x232b08 ?add_message@?$t_message_queue@Vc_decal_message_union@@$0BAA@@@QAAXPBVc_decal_message_union@@@Z
template<> void t_message_queue<c_decal_message_union, 256>::add_message(const c_decal_message_union *message_union)
{
    __lwsync();
    long index = InterlockedIncrement((long *)&message_count) - 1;
    ((c_synchronized_bitvector<512> *)&in_use)->set_bit(index, false);
    messages[index] = *message_union;
    ((c_synchronized_bitvector<512> *)&in_use)->set_bit(index, true);
}

/* ---- geometry draw (0x45a090) ---- */
typedef char decal_system_size[(sizeof(c_decal_system) == 80) ? 1 : -1];
typedef char decal_system_definition_offset[(offsetof(c_decal_system, m_definition_index) == 4) ? 1 : -1];


// @address 0x8c29d8 g_decal_preplaced_render_list
const c_decal *g_decal_preplaced_render_list[2][0x700 / 4];
// @address 0x8c37d8 g_decal_preplaced_render_counts
long g_decal_preplaced_render_counts[2];
// @address 0x8c37e0 g_decal_runtime_render_list
const c_decal *g_decal_runtime_render_list[2][0x400 / 4];
// @address 0x8c3fe0 g_decal_runtime_render_counts
long g_decal_runtime_render_counts[2];


// @address 0x176f98 decal_system_render_queued_decals
extern "C" void decal_system_render_queued_decals(long index)
{
    for (long i = 0; i < g_decal_preplaced_render_counts[index & 0x3fffffff]; i++)
        g_decal_preplaced_render_list[index][i]->render();
    for (long i = 0; i < g_decal_runtime_render_counts[index & 0x3fffffff]; i++)
        g_decal_runtime_render_list[index & 0x3fffff][i]->render();
}


// @address 0x45a090 ?render_mesh_part_no_material@@YAXPBUs_render_geometry@@JJPAK@Z
void render_mesh_part_no_material(const s_render_geometry *container, long item_index, long group_index, unsigned long *mask)
{
    s_render_geometry *geometry = (s_render_geometry *)container;
    s_mesh *item = &((s_mesh *)geometry->meshes.address)[item_index];
    s_part *group = &((s_part *)item->parts.address)[group_index];
    const c_rasterizer_index_buffer *buffer = render_geometry_get_index_buffer(container, (unsigned short)item->index_buffer_indices[0]);
    const c_rasterizer_vertex_buffer *info = (const c_rasterizer_vertex_buffer *)render_geometry_get_vertex_buffer(
        (const s_render_geometry *)container, (unsigned short)item->vertex_buffer_indices[0]);
    if (c_rasterizer::set_vertex_stream((c_rasterizer_vertex_buffer const *)(info))) {
        if (!mask) {
            c_rasterizer::draw_indexed_primitive((c_rasterizer_index_buffer const *)(buffer), 0, info->m_vertex_count, group->start, group->count);
        } else {
            long start = -1;
            long end = -1;
            for (long i = group->first_subpart; i < group->first_subpart + group->subpart_count; i++) {
                if (mask[i >> 5] & (1 << (i & 31))) {
                    const s_subpart *range = &((s_subpart *)item->subparts.address)[i];
                    if (start == -1) {
                        start = range->index_start;
                        end = range->index_start + range->index_count - 1;
                    } else if (range->index_start != end + 1) {
                        c_rasterizer::draw_indexed_primitive((c_rasterizer_index_buffer const *)(buffer), 0, info->m_vertex_count, start,
                            end - start + 1);
                        start = range->index_start;
                        end = range->index_start + range->index_count - 1;
                    } else {
                        end = range->index_start + range->index_count - 1;
                    }
                }
            }
            if (start != -1)
                c_rasterizer::draw_indexed_primitive((c_rasterizer_index_buffer const *)(buffer), 0, info->m_vertex_count, start, end - start + 1);
        }
    }
}


// the visibility globals keep a pointer to the collection input just past the 200304-byte input block
static __forceinline const unsigned long *decals_visibility_collection_words()
{
    return (const unsigned long *)g_visibility_globals.collection_input;
}
// @address 0x16e508 decals_add_visible_system_decals
extern "C" void decals_add_visible_system_decals(const s_decals_cluster_request *request)
{
    const c_decal_system *system = (const c_decal_system *)request;
    bool visible = false;
    for (unsigned long index = 0; index < 4; index++) {
        const s_cluster_reference *entry = &system->m_cluster_references[index];
        if (entry->bsp_index != -1) {
            unsigned long word = decals_visibility_collection_words()[(entry->cluster_index >> 5) + (entry->bsp_index << 3) + 0xbf1c];
            if (word & (1 << (entry->cluster_index & 31))) {
                visible = true;
                break;
            }
        }
    }
    if (visible) {
        for (long child_index = system->m_first_child_index; child_index != -1; ) {
            const c_decal *child = (const c_decal *)(g_decal_array->data + (child_index & 0xffff) * sizeof(c_decal));
            decals_add_decal_to_render_list((const c_decal *)child);
            child_index = child->m_next_decal_index;
        }
    }
}

// @address 0x16e5f8 decals_add_decal_to_render_list
extern "C" void decals_add_decal_to_render_list(const c_decal *decal)
{
    c_decal_system *datum =
        (c_decal_system *)((g_decal_system_data)->data + (unsigned short)decal->m_system_index * sizeof(c_decal_system));
    float fade = DECALS_MIN(DECALS_MAX((decal->m_lifespan - decal->m_age) / decal->m_fade_time, 0.0f), 1.0f);
    if (decals_system_get_distance_fade((const c_decal_system *)datum) * fade > 0.0f) {
        const c_decal_system *recomputed = (const c_decal_system *)((g_decal_system_data)->data +
            (unsigned short)decal->m_system_index * sizeof(c_decal_system));
        const s_decals_system_definition *definition
            = (const s_decals_system_definition *)global_tag_instances[datum_index_to_absolute_index(recomputed->m_definition_index)].definition;
        long visual_index = definition->decals[decal->m_definition_index].visual_index;
        if (datum->m_flags & 1) {
            long count = g_decal_preplaced_render_counts[visual_index];
            g_decal_preplaced_render_counts[visual_index] = count + 1;
            g_decal_preplaced_render_list[visual_index][count] = decal;
        } else {
            long count = g_decal_runtime_render_counts[visual_index];
            g_decal_runtime_render_counts[visual_index] = count + 1;
            g_decal_runtime_render_list[visual_index][count] = decal;
        }
    }
}

// @address 0x22ecd8 ?change_pvs@c_decal_system@@SAXPBUs_game_cluster_bit_vectors@@0_N@Z
void c_decal_system::change_pvs(const s_game_cluster_bit_vectors *old_pvs, const s_game_cluster_bit_vectors *new_pvs, bool update)
{
    if (update == true) {
    const bool &locked = restricted_region_locked_for_current_thread(4);
    if (locked != g_decal_system_pvs_locked) {
        scenario *scenario = global_scenario_get();
        if (scenario) {
            long active_mask = global_structure_bsp_active_mask_get();
            c_decal_message_union message;
            long structure_bsp_index;
            for (structure_bsp_index = 0; structure_bsp_index < scenario->structure_bsps.count; structure_bsp_index++) {
                if (active_mask & (1 << structure_bsp_index)) {
                    scenario_structure_bsp_reference *reference =
                        TAG_BLOCK_GET_ELEMENT(
                            &((struct scenario *)global_scenario)->structure_bsps, structure_bsp_index, scenario_structure_bsp_reference);
                    const long &tag_index = datum_index_to_absolute_index(reference->structure_bsp.index);
                    structure_bsp *structure_bsp = (struct structure_bsp *)global_tag_instances[tag_index].definition;
                    for (short cluster_index = 0; cluster_index < structure_bsp->clusters.count; cluster_index++) {
                        bool was_visible = old_pvs->bit_vectors[structure_bsp_index].test(cluster_index);
                        bool is_visible = new_pvs->bit_vectors[structure_bsp_index].test(cluster_index);
                        bool added = !was_visible && is_visible;
                        bool removed = was_visible && !is_visible;
                        if (removed) {
                            s_cluster_reference cluster_reference;
                            cluster_reference_set(&cluster_reference, structure_bsp_index, cluster_index);
                            message.type = 1;
                            message.cluster_reference = cluster_reference;
                            if (restricted_region_locked_for_current_thread(4))
                                decal_system_remove_cluster_from_pvs(&message.cluster_reference);
                            else
                                g_decal_message_queue->add_message(&message);
                        }
                    }
                }
            }
            for (structure_bsp_index = 0; structure_bsp_index < scenario->structure_bsps.count; structure_bsp_index++) {
                if (active_mask & (1 << structure_bsp_index)) {
                    structure_bsp *structure_bsp = global_structure_bsp_get(structure_bsp_index);
                    for (short cluster_index = 0; cluster_index < structure_bsp->clusters.count; cluster_index++) {
                        bool was_visible = old_pvs->bit_vectors[structure_bsp_index].test(cluster_index);
                        bool is_visible = new_pvs->bit_vectors[structure_bsp_index].test(cluster_index);
                        bool added = !was_visible && is_visible;
                        bool removed = was_visible && !is_visible;
                        if (added) {
                            s_cluster_reference cluster_reference;
                            cluster_reference_set(&cluster_reference, structure_bsp_index, cluster_index);
                            message.type = 0;
                            message.cluster_reference = cluster_reference;
                            if (restricted_region_locked_for_current_thread(4))
                                c_decal_system::add_cluster_to_pvs(&message.cluster_reference);
                            else
                                g_decal_message_queue->add_message(&message);
                        }
                    }
                }
            }
        }
    }
    }
}
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x1ec288 ?data_dispose@@YAXPAUs_data_array@@@Z
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x22e628 ?__tls_set_g_first_person_weapon_orientations_allocator@@YAXPAX@Z
// @address 0x2865e0 ?lruv_new@@YAPAUs_lruv_cache@@PBDJJJPAXP6AX1J_N@ZP6A_N1J@ZP6AE1J@ZPAVc_allocation_base@@J@Z
// @address 0x23cc98 ?set_bit@?$c_synchronized_bitvector@$0CAA@@@QAAXJ_N@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A
// @address 0x1ec160 ?data_new@@YAPAUs_data_array@@PBDJJJPAVc_allocation_base@@@Z


__forceinline bool s_render_method_postprocess_definition::query_sprite_bitmap(const s_render_method_texture **reference) const
{
    bool result = false;
    if (query_constants[1] != NONE)
    {
        *reference = &((s_render_method_texture *)textures.address)[query_constants[1]];
        result = true;
    }
    return result;
}
template<class T> static __forceinline T *decals_bitmap_get_element(const s_tag_block *block, long index)
{
    return &((T *)block->address)[index];
}

// match: 0x230150, 0x22e0a8: _rotl intrinsic selects the rotlwi instruction form
#pragma intrinsic(_rotl)
// Extract the radius sample from the updated seed word.
static __forceinline float decals_real_random_range_rotl(unsigned long *seed, float lower, float upper)
{
    random_seed_next(seed);
    unsigned long random = _rotl(*seed, 16) & 0xffff;
    return lower + (upper - lower) * ((float)random * 1.5259022e-05f);
}

static __forceinline c_decal_system *decals_get_system(long system_index)
{
    return &((c_decal_system *)(g_decal_system_data)->data)[(unsigned short)system_index];
}
static __forceinline const c_decal_definition *decals_get_decal_definition(long system_index, long definition_index)
{
    const c_decal_system *system = decals_get_system(system_index);
    const s_decals_system_definition *system_definition =
        (const s_decals_system_definition *)global_tag_instances[datum_index_to_absolute_index(system->m_definition_index)].definition;
    return &system_definition->decals[definition_index];
}
static __forceinline unsigned long *decals_get_current_count_modifiable(bool preplaced)
{
    return &g_decal_counts[preplaced].child_count;
}
static __forceinline unsigned long decals_get_max_count(bool preplaced)
{
    return preplaced ? 448 : 256;
}

// @address 0x230150 ?create@c_decal@@SAJEJJ@Z
long c_decal::create(unsigned char type, long system_index, long definition_index)
{
    bool preplaced = (decals_get_system(system_index)->m_flags & 1) != 0;
    unsigned long count = *decals_get_current_count_modifiable(preplaced);
    if (count < decals_get_max_count(preplaced) && (preplaced || count + 1 <= g_effect_budget_limits->table->blocks[14].limits[type].maximum_count))
    {
        (*decals_get_current_count_modifiable(preplaced))++;
        long decal_index = datum_new(g_decal_array);
        c_decal *decal = &((c_decal *)g_decal_array->data)[datum_index_to_absolute_index(decal_index)];
        decal->m_system_index = system_index;
        decal->m_definition_index = definition_index;
        const c_decal_definition *definition = decals_get_decal_definition(system_index, definition_index);
        decal->m_next_decal_index = NONE;
        decal->m_flags = 0;
        decal->m_age = 0.0f;
        decal->set_alive(true);
        decal->m_radius = decals_real_random_range_rotl(get_local_random_seed_address(), definition->radius_min, definition->radius_max);
        decal->m_width = decal->m_radius * definition->bitmap_aspect;
        decal->m_fade_time = _real_random_range(get_local_random_seed_address(), definition->decay_min, definition->decay_max);
        decal->m_lifespan = _real_random_range(get_local_random_seed_address(), definition->lifespan_min, definition->lifespan_max);
        decal->m_first_cache_handle = NONE;
        decal->m_second_cache_handle = NONE;
        decal->m_texture_origin = k_real_math_origin3d.xy;
        decal->m_texture_size.y = 1.0f;
        decal->m_texture_size.x = 1.0f;
        decal->choose_sprite();
        g_decal_allocation_available[type] = true;
        return decal_index;
    }
    g_decal_allocation_available[type] = false;
    return NONE;
}
// @address 0x2308c8 ?choose_sprite@c_decal@@QAAXXZ
void c_decal::choose_sprite()
{
    const c_decal_system *system = &((c_decal_system *)(g_decal_system_data)->data)[(unsigned short)m_system_index];
    const s_decals_system_definition *system_definition =
        (const s_decals_system_definition *)global_tag_instances[datum_index_to_absolute_index(system->m_definition_index)].definition;
    const c_decal_definition *definition = &system_definition->decals[m_definition_index];
    if (definition->get_has_sprite())
    {
        const s_render_method_postprocess_definition *postprocess = definition->shader.get_postprocess_definition();
        const s_render_method_texture *reference = 0;
        if (postprocess->query_sprite_bitmap(&reference) && reference->bitmap.index != NONE)
        {
            const bitmap_group *bitmap = (const bitmap_group *)global_tag_instances[datum_index_to_absolute_index(reference->bitmap.index)].definition;
            const bitmap_sequence *sequence = 0;
            if ((long)bitmap->manual_sequences.count > 0)
                sequence = decals_bitmap_get_element<bitmap_sequence>(
                    &bitmap->manual_sequences, _random_range(get_local_random_seed_address(), 0, 0, 0, 0, (unsigned long)bitmap->manual_sequences.count));
            else if ((long)bitmap->sequences.count > 0)
                sequence = decals_bitmap_get_element<bitmap_sequence>(
                    &bitmap->sequences, _random_range(get_local_random_seed_address(), 0, 0, 0, 0, (unsigned long)bitmap->sequences.count));
            if (sequence)
            {
                const bitmap_sprite *sprite = decals_bitmap_get_element<bitmap_sprite>(
                    &sequence->sprites, _random_range(get_local_random_seed_address(), 0, 0, 0, 0, (unsigned long)sequence->sprites.count));
                m_texture_origin.x = sprite->bounds.x0;
                m_texture_origin.y = sprite->bounds.y0;
                m_texture_size.x = sprite->bounds.x1 - sprite->bounds.x0;
                m_texture_size.y = sprite->bounds.y1 - sprite->bounds.y0;
            }
        }
    }
}

// match: noalias declaration preserves caller live registers.


static __forceinline real_vector3d *decals_transform_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *out)
{
float x = vector->i;
float y = vector->j;
float z = vector->k;
float i = matrix->rotation.forward.i * x + matrix->rotation.left.i * y + matrix->rotation.up.i * z;
float j = matrix->rotation.forward.j * x + matrix->rotation.left.j * y + matrix->rotation.up.j * z;
float k = matrix->rotation.forward.k * x + matrix->rotation.left.k * y + matrix->rotation.up.k * z;
out->i=i;out->j=j;out->k=k;return out;
}
static __forceinline real_vector3d *decals_transform_reference(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *out)
{
float x = vector->i;
float y = vector->j;
float pre_i = matrix->rotation.forward.i * x;
float pre_j = matrix->rotation.up.j * vector->k;
float pre_k = matrix->rotation.up.k * vector->k;
float i = pre_i + matrix->rotation.left.i * y + matrix->rotation.up.i * vector->k;
float j = matrix->rotation.forward.j * x + matrix->rotation.left.j * y + pre_j;
float k = matrix->rotation.forward.k * x + matrix->rotation.left.k * y + pre_k;
out->i=i;out->j=j;out->k=k;return out;
}
static __forceinline void decals_rotate_reference(const real_matrix4x3 *matrix, real_vector3d *vector)
{
    float i = matrix->rotation.forward.i * vector->i + matrix->rotation.up.i * vector->k + matrix->rotation.left.i * vector->j;
    float j = matrix->rotation.up.j * vector->k + matrix->rotation.forward.j * vector->i + matrix->rotation.left.j * vector->j;
    float k = matrix->rotation.up.k * vector->k + matrix->rotation.forward.k * vector->i + matrix->rotation.left.k * vector->j;
    vector->i = i; vector->j = j; vector->k = k;
}
static __forceinline void decals_cross_product(const real_vector3d *a, const real_vector3d *b, real_vector3d *out)
{
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    out->i = i; out->j = j; out->k = k;
}
// @address 0x230b68 decals_compute_vertex_basis
extern "C" void decals_compute_vertex_basis(
    rasterizer_vertex_world *vertex, const real_matrix4x3 *matrix, const real_vector3d *normal, const real_vector3d *up, bool offset)
{
    const real_vector3d *basis = normal;
    real_vector3d binormal = *up;
    real_vector3d tangent;
    real_vector3d transformed_normal;
    if (matrix)
    {
        decals_transform_vector(matrix, normal, &transformed_normal);
        basis = &transformed_normal;
        decals_transform_reference(matrix, &binormal, &binormal);
    }
    decals_cross_product(basis, &binormal, &tangent);
    normalize3d(&tangent);
    decals_cross_product(&tangent, basis, &binormal);
    normalize3d(&binormal);
    vertex->normal = rasterizer_pack_vector_dhenn3(basis->n);
    vertex->tangent = rasterizer_pack_vector_dhenn3(tangent.n);
    vertex->binormal = rasterizer_pack_vector_dhenn3(binormal.n);
    if (offset)
    {
        vertex->position.x += basis->i * 0.001f;
        vertex->position.y += basis->j * 0.001f;
        vertex->position.z += basis->k * 0.001f;
    }
}


static __forceinline void decals_transform_quad_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *out)
{
    float x = matrix->scale;
    x *= point->x;
    float y = point->y * matrix->scale;
    float z = point->z * matrix->scale;
    out->x = matrix->rotation.forward.i * x + matrix->rotation.left.i * y + matrix->rotation.up.i * z + matrix->position.x;
    out->y = matrix->rotation.forward.j * x + matrix->rotation.left.j * y + matrix->rotation.up.j * z + matrix->position.y;
    out->z = matrix->rotation.forward.k * x + matrix->rotation.left.k * y + matrix->rotation.up.k * z + matrix->position.z;
}
static __forceinline void decals_project_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *out)
{
    float x = point->x * matrix->scale;
    float y = point->y * matrix->scale;
    float z = point->z * matrix->scale;
    float matrix_left_i = matrix->rotation.left.i;
    out->x = matrix->rotation.forward.i * x + matrix_left_i * y + matrix->rotation.up.i * z + matrix->position.x;
    out->y = matrix->rotation.forward.j * x + matrix->rotation.left.j * y + matrix->rotation.up.j * z + matrix->position.y;
    out->z = matrix->rotation.forward.k * x + matrix->rotation.left.k * y + matrix->rotation.up.k * z + matrix->position.z;
}
static __forceinline real_point3d *decals_quad_point_from_line(const real_point3d *point, const real_vector3d *vector, float distance,
    real_point3d *out)
{
    out->x = point->x + vector->i * distance;
    out->y = point->y + vector->j * distance;
    out->z = point->z + vector->k * distance;
    return out;
}
static __forceinline void decals_project_quad_point(const real_plane3d *plane, const real_matrix4x3 *matrix, real_point3d *point)
{
    float denominator = dot_product3d(&plane->n, &matrix->rotation.forward);
    float distance;
    if (denominator != 0.0f)
        distance = -(plane->n.i * point->x + (plane->n.k * point->z + plane->n.j * point->y) - plane->d) / denominator;
    else
        distance = 0.0f;
    decals_quad_point_from_line(point, &matrix->rotation.forward, distance, point);
}
static __forceinline bool decals_definition_needs_normals(const c_decal_definition *definition) { return ((definition->flags >> 1) & 1) != 0; }
// @address 0x230d40 decals_build_floating_quad
extern "C" void decals_build_floating_quad(
    c_decal *decal, const real_plane3d *plane, const real_matrix4x3 *transform, const real_matrix4x3 *projection)
{
    const c_decal_system *system = &((c_decal_system *)(g_decal_system_data)->data)[(unsigned short)decal->m_system_index];
    const s_decals_system_definition *system_definition =
        (const s_decals_system_definition *)global_tag_instances[(unsigned short)system->m_definition_index].definition;
    const c_decal_definition *definition = &system_definition->decals[decal->m_definition_index];
    const real_point2d *half_size = &decal->m_half_size;
    for (long i = 0; i < 4; ++i)
    {
        rasterizer_vertex_world *vertex = &decal->m_vertices[i];
        vertex->texcoord.y = (float)(i & 1);
        vertex->texcoord.x = (float)(long)((unsigned long)i >> 1);
        real_point3d point = { 0.0f, (vertex->texcoord.x - 0.5f) * half_size->x * 2.0f, (vertex->texcoord.y - 0.5f) * half_size->y * 2.0f };
        decals_project_transform_point(projection, &point, &vertex->position);
        decals_project_quad_point(plane, projection, &vertex->position);
        vertex->position.x += plane->n.i * 0.001f;
        vertex->position.y += plane->n.j * 0.001f;
        vertex->position.z += plane->n.k * 0.001f;
        if (transform)
            decals_transform_quad_point(transform, &vertex->position, &vertex->position);
        if (decals_definition_needs_normals(definition))
            decals_compute_vertex_basis(vertex, transform, &plane->n, &projection->rotation.up, false);
    }
}


static __forceinline real_vector3d *decals_add_vectors(const real_vector3d *a, const real_vector3d *b, real_vector3d *out)
{
    out->i = a->i + b->i;
    out->j = a->j + b->j;
    out->k = a->k + b->k;
    return out;
}

static __forceinline void decals_set_texture_coordinates(real_point2d *out, float x, float y)
{
    out->x = x;
    out->y = y;
}

static __forceinline void decals_set_point(real_point3d *out, float x, float y, float z)
{
    out->x = x;
    out->y = y;
    out->z = z;
}

static __forceinline real_vector3d *decals_scale_weld_vector(const real_vector3d *vector, float scale, real_vector3d *out)
{
    out->i = scale * vector->i;
    out->j = scale * vector->j;
    out->k = scale * vector->k;
    return out;
}

static __forceinline void decals_normalize_weld_normal(real_vector3d *normal)
{
    float length = sqrtf(normal->j * normal->j + normal->k * normal->k + normal->i * normal->i);
    if (!(fabsf(length) < 0.0001f))
    {
        decals_scale_weld_vector(normal, 1.0f / length, normal);
    }
}

// @address 0x231c70 decals_write_welded_mesh
extern "C" void decals_write_welded_mesh(const c_decal *decal, rasterizer_vertex_world *vertices,
                             unsigned short *indices, const s_decals_weld_buffers *buffers, const s_decals_weld_batch *batch)
{
    unsigned short system_index = (unsigned short)decal->m_system_index;
    const s_data_array *systems = g_decal_system_data;
    const c_decal_system *system = &((const c_decal_system *)systems->data)[system_index];
    long system_definition_index = datum_index_to_absolute_index(system->m_definition_index);
    const s_decals_system_definition *system_definition =
        (const s_decals_system_definition *)global_tag_instances[system_definition_index].definition;
    const c_decal_definition *definition = &system_definition->decals[decal->m_definition_index];
    unsigned long vertex_index = batch->vertex_base;
    unsigned long group_index = vertex_index;
    if (vertex_index < batch->vertex_base + batch->group_count)
    {
        do
        {
            rasterizer_vertex_world *vertex = &vertices[vertex_index];
            const s_decals_weld_group *group = &buffers->groups[group_index];
            const s_decals_sort_item *representative = &buffers->items[buffers->order_area[group->start]];
            decals_set_texture_coordinates(&vertex->texcoord, representative->texcoord.x, representative->texcoord.y);
            if (batch->transform)
                decals_project_transform_point(batch->transform, representative->position, &vertex->position);
            else
                decals_set_point(&vertex->position, representative->position->x, representative->position->y, representative->position->z);
            bool offset = g_decal_force_offset || batch->offset;
            if (decals_definition_needs_normals(definition) || offset)
            {
                unsigned long member_count = group->count;
                real_vector3d normal = *global_zero_vector3d;
                real_vector3d up = *global_zero_vector3d;
                for (unsigned long member_index = 0; member_index < member_count; ++member_index)
                {
                    unsigned short order_index = (unsigned short)(group->start + member_index);
                    const s_decals_sort_item *member = &buffers->items[buffers->order_area[order_index]];
                    decals_add_vectors(&member->normal, &normal, &normal);
                    decals_add_vectors(&up, &member->up, &up);
                }
                decals_normalize_weld_normal(&normal);
                decals_compute_vertex_basis(vertex, batch->transform, &normal, &up, offset);
            }
            ++vertex_index;
            ++group_index;
        } while (vertex_index < batch->vertex_base + batch->group_count);
    }
    for (unsigned long index = batch->group_base; index < batch->group_base + batch->output_index_count; ++index)
    {
        unsigned long vertex = buffers->indices[index];
        if (vertex != 0xffff)
            vertex = buffers->remap[vertex];
        indices[index] = (unsigned short)vertex;
    }
}


// @address 0x8c29d0 ?x_currently_submitting_decal@c_decal@@0PBV1@B
const c_decal *c_decal::x_currently_submitting_decal;

// @address 0x230530 ?shader_function_callback@c_decal@@SAMJJJ@Z
float c_decal::shader_function_callback(long object_index, long function_name, long definition_index)
{
    if (function_name != _string_id_holiday)
        return x_currently_submitting_decal->m_age / x_currently_submitting_decal->m_lifespan;
    return game_time_get_holiday();
}


// @rdata 0x820254e4
static const float decals_decal_slope_scale_bias = -0.5f;
// @rdata 0x820254e8
static const float decals_decal_depth_bias = -0.000005f;
static __forceinline void decals_set_z_buffer_mode_decal(c_rasterizer::e_z_buffer_mode z_buffer_mode)
{
    unsigned char use_floating_point_z_buffer = g_rasterizer_use_floating_point_z_buffer;
    float scale = 1.0f;
    unsigned long z_function;
    if (g_rasterizer_use_floating_point_z_buffer) {
        z_function = 6;
        scale = -1.0f;
    } else {
        z_function = 3;
    }

    if (z_buffer_mode != g_rasterizer_current_z_buffer_mode || use_floating_point_z_buffer != g_rasterizer_current_z_buffer_floating_point) {
        g_rasterizer_current_z_buffer_mode = z_buffer_mode;
        g_rasterizer_current_z_buffer_floating_point = use_floating_point_z_buffer;
        D3DDevice_SetRenderState_ZEnable(c_rasterizer::get_device(), 1);
        D3DDevice_SetRenderState_ZWriteEnable(c_rasterizer::get_device(), 0);
        D3DDevice_SetRenderState_ZFunc(c_rasterizer::get_device(), z_function);
        union { float as_float; unsigned long as_dword; } slope_scale_bias = { decals_decal_slope_scale_bias };
        union { float as_float; unsigned long as_dword; } depth_bias = { decals_decal_depth_bias };
        slope_scale_bias.as_float = scale * slope_scale_bias.as_float;
        depth_bias.as_float = scale * depth_bias.as_float;
        D3DDevice_SetRenderState_SlopeScaleDepthBias(c_rasterizer::get_device(), slope_scale_bias.as_dword);
        D3DDevice_SetRenderState_DepthBias(c_rasterizer::get_device(), depth_bias.as_dword);
    }
}
// @address 0x35548 ?x_shader_extern_info@c_decal@@2Us_shader_extern_info@@B
// @rdata 0x82035548..0x82035798
const s_shader_extern_info c_decal::x_shader_extern_info = { (long)0xdeadc0de, c_decal::shader_function_callback };
// @rdata-end
// @address 0x176d70 ?render_all@c_decal_system@@SAXW4e_pass@c_decal_definition@@@Z
void c_decal_system::render_all(c_decal_definition::e_pass pass)
{
    decals_set_z_buffer_mode_decal(c_rasterizer::_z_buffer_mode_6);
    c_rasterizer::begin_high_quality_blend();
    D3DDevice_SetRenderState_PrimitiveResetEnable(c_rasterizer::get_device(), TRUE);
    D3DDevice_SetRenderState_PrimitiveResetIndex(c_rasterizer::get_device(), 0xffff);
    if (pass == c_decal_definition::_pass_default)
        c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)19);
    else
        D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(),
            D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
    render_method_submit_set_extern_info(global_structure_bsp_first_active_index_get(), const_cast<s_shader_extern_info *>(&c_decal::x_shader_extern_info));
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_stipple_read);
    decal_system_render_queued_decals(pass);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), D3DCULL_CW);
    c_rasterizer::end_high_quality_blend();
    c_rasterizer::set_separate_alpha_blend_mode(c_rasterizer::_separate_alpha_blend_off);
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_normal);
    c_rasterizer::set_alpha_blend_mode(c_rasterizer::_alpha_blend_additive);
    c_rasterizer::set_alpha_blend_mode(c_rasterizer::_alpha_blend_opaque);
    c_rasterizer::set_render_target(1, c_rasterizer::_surface_none);
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(),
        D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
    c_render_method_data::m_valid = false;
    render_method_submit_valid();
}
// match: 0x22e0a8, 0x231c70: the cross product computes i, k, j into separate temporaries before storing.
static __forceinline real_vector3d *decals_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result) {
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}

static __forceinline real_vector3d *decals_projection_cross(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float bi = b->i, bj = b->j, bk = b->k;
    float i = bk * a->j - bj * a->k;
    float k = bj * a->i - bi * a->j;
    float j = bi * a->k - bk * a->i;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}
static __forceinline void decals_projection_dot_axis(const real_vector3d *a, const real_vector3d *b, float *out)
{
    float ai = a->i;
    *out = ai * b->i + a->j * b->j + a->k * b->k;
}
static __forceinline float decals_projection_pin_unit(float value)
{
    return (value > 0.0f ? value : 0.0f) < 1.0f ? (value <= 0.0f ? 0.0f : value) : 1.0f;
}
static __forceinline void decals_projection_negate_vector(const real_vector3d *source, real_vector3d *result)
{
    result->i = -source->i;
    result->j = -source->j;
    result->k = -source->k;
}
static __forceinline real_vector3d *decals_projection_offset_point(const real_vector3d *point, float distance,
    const real_vector3d *direction, real_vector3d *result)
{
    result->i = point->i + direction->i * distance;
    result->j = point->j + direction->j * distance;
    result->k = point->k + distance * direction->k;
    return result;
}
// @address 0x22e0a8 ?build_projection@s_decal_projection_builder@@QAA_NPBUs_fold@@@Z
bool s_decal_projection_builder::build_projection(const s_fold *fold)
{
    float incidence_cosine = dot_product3d(&fold->normal, &projection.rotation.forward);
    if (!(-incidence_cosine >= projection_cosine))
        return false;
    if (get_left_handed())
    {
        decals_cross_product3d(&projection.rotation.forward, &projection.rotation.up, &projection.rotation.left);
        normalize3d(&projection.rotation.left);
        decals_cross_product3d(&projection.rotation.left, &projection.rotation.forward, &projection.rotation.up);
    }
    else
    {
        decals_cross_product3d(&projection.rotation.up, &projection.rotation.forward, &projection.rotation.left);
        normalize3d(&projection.rotation.left);
        decals_cross_product3d(&projection.rotation.forward, &projection.rotation.left, &projection.rotation.up);
    }
    if (-incidence_cosine < edge_cosine)
    {
        flags |= 4;
        real_vector3d projected;
        float along_axis;
        decals_projection_dot_axis(&fold->axis, &projection.rotation.forward, &along_axis);
        scale_vector3d(&fold->axis, along_axis, &projected);
        decals_projection_offset_point(&projected, -edge_cosine, &fold->normal, &projected);
        real_vector3d side;
        decals_cross_product3d(&fold->normal, &fold->axis, &side);
        float sign = dot_product3d(&side, &projection.rotation.forward) < 0.0f ? -1.0f : 1.0f;
        float displacement = sqrtf(1.0f - decals_projection_pin_unit(magnitude_squared3d(&projected))) * sign;
        point_from_line3d(&projected, &side, displacement, &projected);
        real_vector3d perpendicular;
        decals_cross_product3d(&fold->axis, &projection.rotation.forward, &perpendicular);
        real_vector3d target_perpendicular;
        decals_projection_cross(&fold->axis, &projected, &target_perpendicular);
        float rotation_cosine = dot_product3d(&perpendicular, &target_perpendicular) / magnitude_squared3d(&perpendicular);
        float rotation_sine = -(sqrtf(1.0f - decals_projection_pin_unit(rotation_cosine * rotation_cosine)) * sign);
        real_matrix4x3 rotation;
        matrix4x3_rotation_from_axis_and_angle(&rotation, &fold->axis, rotation_sine, rotation_cosine);
        point_from_line3d(&projection.position, &fold->point_offset, -1.0f, &projection.position);
        matrix4x3_compute_product(&rotation, &projection, &projection);
        add_points3d(&projection.position, &fold->point, &projection.position);
        if (get_needs_renormalize())
        {
            normalize3d(&projection.rotation.forward);
            decals_cross_product3d(&projection.rotation.up, &projection.rotation.forward, &projection.rotation.left);
            normalize3d(&projection.rotation.left);
            decals_cross_product3d(&projection.rotation.forward, &projection.rotation.left, &projection.rotation.up);
            normalize3d(&projection.rotation.up);
            if (get_left_handed())
            {
                decals_projection_negate_vector(&projection.rotation.left, &projection.rotation.left);
            }
        }
    }
    set_built(true);
    return true;
}


// @address 0xa31758 ?k_decal_vector_test_mask@@3Us_collision_test_flags@@A
// match: other collisions.h includers still use erased C bridge signatures.


static __forceinline s_collision_test_flags decals_collision_flags_intersect(s_collision_test_flags flags, const s_collision_test_flags &mask)
{
    flags.collision_flags &= mask.collision_flags;
    flags.object_flags &= mask.object_flags;
    return flags;
}
static __forceinline bool decals_collision_flags_empty(s_collision_test_flags flags)
{
    return flags.collision_flags == 0 && flags.object_flags == 0;
}
static __forceinline bool decals_vector_test(s_collision_test_flags flags, const real_point3d *point, const real_vector3d *vector,
    long ignore_object_index, long structure_index, long instance_index, collision_result *collision)
{
    bool result = false;
    if (!decals_collision_flags_empty(decals_collision_flags_intersect(flags, k_decal_vector_test_mask))) {
        if (instance_index == -1)
            result = collision_test_vector(flags, false, point, vector, ignore_object_index, -1, -1, collision);
        else if ((_rotl(flags.collision_flags, 29) & 1) != 0)
            result = collision_test_instanced_geometry_instance(structure_index, instance_index, flags.collision_flags, point, vector, collision);
        if (result)
            result = decals_query_has_valid_surface((const s_decal_collision_result *)collision);
    }
    return result;
}
// @address 0x22d5c8 decals_test_vector
extern "C" bool decals_test_vector(const s_collision_tester *query, collision_result *collision)
{
    return decals_vector_test(
        query->flags, &query->point, &query->vector, query->ignore_object_index, query->structure_index, query->instance_index, collision);
}
__forceinline bool s_collision_tester::test_vector(s_decal_collision_result *collision) const
{
    return decals_test_vector(this, (collision_result *)collision);
}

/* ---- decal fold (0x22db40) ---- */


static __forceinline collision_bsp *decals_fold_bsp(long index)
{
    return const_cast<collision_bsp *>(static_cast<const collision_bsp *>(g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[index]));
}
static __forceinline const collision_bsp *decals_fold_iterator_bsp(const c_collision_surface_edge_iterator *iterator)
{
    return (const collision_bsp *)iterator->bsp;
}
static __forceinline real_plane3d *decals_fold_plane(const collision_bsp *bsp, long designator, real_plane3d *result)
{
    const real_plane3d *plane = TAG_BLOCK_GET_ELEMENT(&bsp->planes, designator & 0x7fff, real_plane3d);
    if (designator & 0x8000) {
        result->n.i = -plane->n.i;
        result->n.j = -plane->n.j;
        result->n.k = -plane->n.k;
    } else {
        *result = *plane;
    }
    return result;
}
static __forceinline real_vector3d *decals_fold_vector(const real_point3d *start, const real_point3d *end, real_vector3d *vector)
{
    vector->i = end->x - start->x;
    vector->j = end->y - start->y;
    vector->k = end->z - start->z;
    return vector;
}
static __forceinline const collision_edge *decals_fold_edge(const c_collision_surface_edge_iterator *iterator)
{
    return iterator->first_edge == -1 ? 0 : TAG_BLOCK_GET_ELEMENT(&decals_fold_iterator_bsp(iterator)->edges,
        iterator->first_edge, collision_edge);
}
static __forceinline void decals_fold_opposing_surface(const c_collision_surface_edge_iterator *iterator, long *opposite)
{
    const collision_edge *edge = decals_fold_edge(iterator);
    *opposite = edge->surfaces[edge->surfaces[1] != iterator->surface_index];
}
static __forceinline const s_structure_bsp_seam_link *decals_fold_material(const structure_bsp *structure, long material)
{
    return TAG_BLOCK_GET_ELEMENT(&structure->collision_materials, material, const s_structure_bsp_seam_link);
}
// @address 0x22db40 ?get_opposing_surface_fold@c_collision_surface_edge_iterator@@QBA_NPBTreal_point3d@@0PAUs_fold@@_N2@Z
bool c_collision_surface_edge_iterator::get_opposing_surface_fold(
    const real_point3d *start, const real_point3d *end, s_fold *fold, bool coplanar, bool same_material) const
{
    bool result = false;
    fold->surface = -1;
    fold->instance_index = -1;
    fold->neighbor = -1;
    fold->normal = k_real_math_zero_vector3d_storage;
    memcpy(&fold->point, &k_real_math_zero_vector3d_storage, sizeof(real_point3d));
    fold->axis = k_real_math_zero_vector3d_storage;
    long opposite_surface;
    decals_fold_opposing_surface(this, &opposite_surface);
    short material = TAG_BLOCK_GET_ELEMENT(&this->bsp->surfaces, opposite_surface, collision_surface)->material;
    if (material != -1) {
        if (this->instance_index == -1 && decals_fold_material((const structure_bsp *)this->structure, material)->seam_index != -1) {
            long other_structure, other_edge;
            if (structure_seams_try_and_get_connected_edge(this->structure_index, this->first_edge, &other_structure, &other_edge)) {
                structure_bsp *structure = global_structure_bsp_get(other_structure);
                collision_bsp *bsp = decals_fold_bsp(other_structure);
                const collision_edge *connected_edge = TAG_BLOCK_GET_ELEMENT(&bsp->edges, other_edge, collision_edge);
                for (long i = 0; i < 2; ++i) {
                    long surface = connected_edge->surfaces[i];
                    long other_material = TAG_BLOCK_GET_ELEMENT(&bsp->surfaces, surface, collision_surface)->material;
                    if (other_material != -1 && decals_fold_material(structure, other_material)->seam_index == -1) {
                        fold->surface = other_structure;
                        fold->instance_index = -1;
                        fold->neighbor = surface;
                        break;
                    }
                }
            }
        } else {
            fold->surface = this->structure_index;
            fold->instance_index = this->instance_index;
            fold->neighbor = opposite_surface;
        }
    }
    if (fold->neighbor != -1) {
        structure_bsp *structure = fold->surface != this->structure_index ? global_structure_bsp_get(fold->surface)
                                                                                         : (structure_bsp *)this->structure;
        collision_bsp *bsp = fold->surface != this->structure_index ? decals_fold_bsp(fold->surface) : (collision_bsp *)this->bsp;
        collision_surface *surface = TAG_BLOCK_GET_ELEMENT(&bsp->surfaces, fold->neighbor, collision_surface);
        if (decals_collision_surface_valid(surface) &&
            (!same_material ||
                decals_fold_material(structure, surface->material)->global_material_index ==
                    decals_fold_material((const structure_bsp *)this->structure, this->surface->material)->global_material_index))
        {
            real_plane3d plane;
            decals_fold_plane(bsp, surface->plane, &plane);
            if (!coplanar ||
                (fabsf(plane.n.i - this->plane.n.i) < 0.0001f && fabsf(plane.n.j - this->plane.n.j) < 0.0001f &&
                    fabsf(plane.n.k - this->plane.n.k) < 0.0001f))
            {
                fold->normal = plane.n;
                fold->point = *start;
                normalize3d(decals_fold_vector(start, end, &fold->axis));
                result = true;
            }
        }
    }
    return result;
}

/* ---- decal mesh build (0x231038) ---- */
static __forceinline const collision_vertex *decals_vertex_iterator_get_datum(const c_collision_surface_vertex_iterator *iterator)
{
    if (iterator->first_edge == -1)
        return 0;
    const collision_bsp *bsp = (const collision_bsp *)iterator->bsp;
    const collision_edge *edge = TAG_BLOCK_GET_ELEMENT(&bsp->edges, iterator->first_edge, collision_edge);
    if (!edge)
        return 0;
    return TAG_BLOCK_GET_ELEMENT(&bsp->vertices,
        (unsigned short)edge->vertices[edge->surfaces[1] == iterator->surface_index], collision_vertex);
}
static __forceinline void decals_vertex_iterator_advance(c_collision_surface_vertex_iterator *iterator)
{
    const collision_bsp *bsp = (const collision_bsp *)iterator->bsp;
    const collision_edge *edge = TAG_BLOCK_GET_ELEMENT(&bsp->edges, iterator->first_edge, collision_edge);
    iterator->first_edge = (unsigned short)edge->edges[edge->surfaces[1] == iterator->surface_index];
    if (iterator->first_edge == iterator->surface->first_edge)
        iterator->first_edge = -1;
}
static __forceinline const c_decal_system *decals_quad_get_system(const c_decal *decal)
{
    const s_data_array *systems = g_decal_system_data;
    return &((const c_decal_system *)systems->data)[(unsigned short)decal->m_system_index];
}
static __forceinline bool decals_system_get_left_handed(const c_decal_system *system)
{
    return (((system->m_flags >> 2) ^ (system->m_flags >> 1)) & 1) != 0;
}

// @rdata 0x82024fd4
static const float decals_degrees_to_radians = 0.017453292f;
// @address 0x231038 decals_build_surface_mesh
extern "C" bool decals_build_surface_mesh(const c_decal *decal, s_decals_weld_buffers *buffers, s_decal_mesh_builder *builder)
{
    const c_decal_system *system = decals_quad_get_system(decal);
    const s_decals_system_definition *system_definition =
        (const s_decals_system_definition *)global_tag_instances[datum_index_to_absolute_index(system->m_definition_index)].definition;
    const c_decal_definition *definition = &(
        (const s_decals_system_definition *)global_tag_instances[datum_index_to_absolute_index(system->m_definition_index)].definition)
                                                ->decals[decal->m_definition_index];
    if (flags_test(system_definition->flags, 3))
        return true;

    for (long surface_index = 0; surface_index < builder->count; surface_index++)
    {
        s_decal_neighbor_surface *surface = &builder->surfaces[surface_index];
        const s_fold *fold = &surface->fold;
        const s_decal_projection_builder *projection = &surface->projection;
        long first_vertex = buffers->item_count;
        c_collision_surface_vertex_iterator iterator(fold->surface, builder->instance_index, fold->neighbor);
        while (const collision_vertex *vertex = decals_vertex_iterator_get_datum(&iterator))
        {
            if ((unsigned long)buffers->item_count >= 1024)
                return false;
            s_decals_sort_item *item = &buffers->items[buffers->item_count++];
            item->position = &vertex->point;
            set_real_vector3d(&item->normal, fold->normal.i, fold->normal.j, fold->normal.k);
            set_real_vector3d(&item->up, projection->projection.rotation.up.i, projection->projection.rotation.up.j,
                projection->projection.rotation.up.k);
            real_point3d local_point;
            matrix4x3_inverse_transform_point(&projection->projection, item->position, &local_point);
            item->texcoord.x = local_point.y / (decal->m_half_size.x * 2.0f) + 0.5f;
            item->texcoord.y = local_point.z / (decal->m_half_size.y * 2.0f) + 0.5f;
            decals_vertex_iterator_advance(&iterator);
        }

        long vertex_count = buffers->item_count - first_vertex;
        if ((unsigned long)(buffers->index_count + vertex_count + 1) >= 1024)
            return false;
        unsigned short *index = &buffers->indices[buffers->index_count];
        *index++ = (unsigned short)first_vertex;
        for (long i = 1; i < vertex_count; i++)
            *index++ = (unsigned short)(((i & 1) ? (i + 1) >> 1 : vertex_count - (i >> 1)) + first_vertex);
        *(short *)index++ = NONE;
        buffers->index_count = index - buffers->indices;

        unsigned long vertex_index = first_vertex;
        c_collision_surface_vertex_iterator edge_iterator(fold->surface, builder->instance_index, fold->neighbor);
        while (decals_vertex_iterator_get_datum(&edge_iterator))
        {
            s_decals_sort_item start = buffers->items[vertex_index];
            ++vertex_index;
            s_decals_sort_item end = buffers->items[vertex_index < (unsigned long)buffers->item_count ? vertex_index : first_vertex];
            if (line_segment_intersects_unit_texture_tile(&start.texcoord, &end.texcoord))
            {
                builder->floating = false;
                s_fold neighbor_fold;
                if (edge_iterator.get_opposing_surface_fold(start.position, end.position,
                    &neighbor_fold,
                        flags_test(system_definition->flags, 4), flags_test(system_definition->flags, 5))
                    && builder->push_neighbor_surface(surface_index, &neighbor_fold))
                {
                    s_decal_projection_builder *neighbor_projection = &builder->surfaces[builder->count - 1].projection;
                    neighbor_projection->initialize(&projection->projection, definition->cull_angle * decals_degrees_to_radians,
                        definition->clamp_angle * decals_degrees_to_radians,
                        decals_system_get_left_handed(decals_quad_get_system(decal)));
                    if (!neighbor_projection->build_projection(&neighbor_fold))
                        builder->count--;
                }
            }
            decals_vertex_iterator_advance(&edge_iterator);
        }
    }
    return true;
}

/* ---- decal mesh build entry (0x231550) ---- */
static __forceinline const c_decal_system *decals_quad_get_parent_system(const c_decal *decal)
{
    const s_data_array *systems = g_decal_system_data;
    return &((const c_decal_system *)systems->data)[datum_index_to_absolute_index(decal->m_system_index)];
}
// @address 0x231550 decals_build_collision_batch
extern "C" bool decals_build_collision_batch(c_decal *decal, s_decals_weld_buffers *buffers, s_decals_weld_batch *batch, bool allow_floating)
{
    bool result = true;
    const c_decal_system *system = decals_quad_get_parent_system(decal);
    const c_decal_definition *definition = &(
        (const s_decals_system_definition *)global_tag_instances[datum_index_to_absolute_index(system->m_definition_index)].definition)
                                                ->decals[decal->m_definition_index];
    const s_decal_collision_result *collision = batch->collision;
    batch->start = buffers->item_count;
    batch->index_start = buffers->index_count;

    real_matrix4x3 projection;
    memcpy(&projection, &system->m_projection, sizeof(projection));
    s_decal_mesh_builder builder;
    batch->transform = 0;
    batch->offset = false;
    builder.instance_index = -1;
    builder.floating = true;
    builder.count = 1;
    builder.surfaces[0].index = -1;
    s_fold *fold = &builder.surfaces[0].fold;
    fold->surface = collision->get_structure_bsp_index();
    fold->neighbor = collision->get_surface_index();
    fold->normal = *collision->get_normal();
    fold->point = *collision->get_point();
    real_plane3d plane = *collision->get_plane();
    if (collision->get_type() == 3)
    {
        structure_bsp *structure = global_structure_bsp_get(collision->get_structure_bsp_index());
        const structure_instanced_geometry_instance *instance = TAG_BLOCK_GET_ELEMENT(
            &structure->instanced_geometry_instances, collision->get_instanced_geometry_instance_index(), structure_instanced_geometry_instance);
        batch->transform = &instance->transform;
        batch->offset = ((instance->flags >> 4) & 1) != 0;
        builder.instance_index = instance->definition_index;
        projection.scale = 1.0f / instance->transform.scale;
        matrix4x3_inverse_transform_point(batch->transform, &projection.position, &projection.position);
        matrix4x3_inverse_transform_normal(batch->transform, &projection.rotation.forward, &projection.rotation.forward);
        matrix4x3_inverse_transform_normal(batch->transform, &projection.rotation.up, &projection.rotation.up);
        matrix4x3_inverse_transform_point(batch->transform, &fold->point, &fold->point);
        matrix4x3_inverse_transform_normal(batch->transform, &fold->normal, &fold->normal);
        normalize3d(&fold->normal);
        matrix4x3_inverse_transform_plane(batch->transform, &plane, &plane);
        normalize3d(&plane.n);
    }

    const real_vector3d *forward = &projection.rotation.forward;
    decals_cross_product(forward, &fold->normal, &fold->axis);
    normalize3d(&fold->axis);
    if (!valid_real_normal3d(&fold->axis))
    {
        decals_cross_product(&projection.rotation.up, &fold->normal, &fold->axis);
        normalize3d(&fold->axis);
    }

    s_decal_projection_builder *projection_builder = &builder.surfaces[0].projection;
    projection_builder->initialize(&projection, definition->cull_angle * decals_degrees_to_radians,
        definition->clamp_angle * decals_degrees_to_radians,
        decals_system_get_left_handed(system));
    if (projection_builder->build_projection(fold))
    {
        result = decals_build_surface_mesh(decal, buffers, &builder);
        if (result && builder.floating && allow_floating)
        {
            decal->m_flags |= 2;
            decals_build_floating_quad(decal, &plane, batch->transform, &projection_builder->projection);
            buffers->item_count = batch->start;
            buffers->index_count = batch->index_start;
        }
    }
    batch->count = buffers->item_count - batch->start;
    batch->index_count = buffers->index_count - batch->index_start;
    return result;
}

// @address 0x2321c0 decals_build_decal_mesh
extern "C" bool decals_build_decal_mesh(
    c_decal *decal, unsigned char type, const c_static_sized_dynamic_array<s_decal_collision_result, 16> *collisions)
{
    bool result = true;
    s_decals_weld_batch batches[16];
    s_decals_weld_buffers buffers;
    buffers.item_count = 0;
    buffers.index_count = 0;
    buffers.vertex_count = 0;
    buffers.group_count = 0;
    buffers.sort_context = buffers.items;
    buffers.sort_compare = decals_compare_weld_items;
    for (long index = 0; index < collisions->count(); index++)
    {
        s_decals_weld_batch *batch = &batches[index];
        batch->collision = &(*collisions)[index];
        if (!decals_build_collision_batch(decal, &buffers, batch, collisions->count() == 1))
        {
            result = false;
            break;
        }
        decals_weld_vertices(&buffers, batch);
    }
    if (result && !flags_test(decal->m_flags, 1))
    {
        decal->m_first_cache_count = (unsigned short)buffers.vertex_count;
        decal->m_second_cache_count = (unsigned short)buffers.group_count;
        if (decal->m_first_cache_count > 0 && decal->m_second_cache_count > 0)
        {
            bool preplaced = (decals_quad_get_system(decal)->m_flags & 1) != 0;
            if (preplaced
                || (g_decal_counts[preplaced].first_cache_total + decal->m_first_cache_count
                    <= g_effect_budget_limits->table->blocks[15].limits[type].maximum_count
                    && g_decal_counts[preplaced].second_cache_total
                        + decal->m_second_cache_count <= g_effect_budget_limits->table->blocks[16].limits[type].maximum_count))
            {
                decal->m_first_cache_handle = ((c_decal::c_mesh_allocator<c_rasterizer_vertex_buffer,
                    rasterizer_vertex_world> *)decal->get_vertex_allocator())
                                          ->new_elements(decal->m_first_cache_count);
                decal->m_second_cache_handle = ((c_decal::c_mesh_allocator<c_rasterizer_index_buffer, unsigned short> *)decal->get_index_allocator())
                                         ->new_elements(decal->m_second_cache_count);
                if (decal->m_first_cache_handle != NONE)
                    g_decal_counts[preplaced].first_cache_total += decal->m_first_cache_count;
                if (decal->m_second_cache_handle != NONE)
                    g_decal_counts[preplaced].second_cache_total += decal->m_second_cache_count;
                if (decal->m_first_cache_handle != NONE && decal->m_second_cache_handle != NONE)
                {
                    const c_decal_system *system = &((c_decal_system *)g_decal_system_data->data)[(unsigned short)decal->m_system_index];
                    const t_decal_vertex_allocator *vertex_allocator =
                        system->m_flags & 1 ? &g_decal_preplaced_vertex_allocator : &g_decal_runtime_vertex_allocator;
                    rasterizer_vertex_world *vertices = (rasterizer_vertex_world *)vertex_allocator->get_locked_data() +
                        (decals_block_offset(vertex_allocator, decal->m_first_cache_handle) >> 5);
                    const t_decal_index_allocator *index_allocator =
                        system->m_flags & 1 ? &g_decal_preplaced_index_allocator : &g_decal_runtime_index_allocator;
                    unsigned short *indices = (unsigned short *)index_allocator->get_locked_data() + (decals_block_offset(index_allocator,
                        decal->m_second_cache_handle) >> 1);
                    for (long index = 0; index < collisions->count(); index++)
                    {
                        if ((unsigned long)batches[index].group_count > 0)
                            decals_write_welded_mesh(decal, vertices, indices, &buffers, &batches[index]);
                    }
                }
                else
                {
                    result = false;
                }
            }
            else
            {
                result = false;
            }
        }
        else
        {
            result = false;
        }
    }
    return result;
}


// @address 0x22f808 ?collide@c_decal_system@@AAA_NPBTreal_point3d@@PBTreal_vector3d@@JPAV?$c_static_sized_dynamic_array@Us_decal_collision_result@@$0BA@@@@Z
bool c_decal_system::collide(const real_point3d *point, const real_vector3d *vector, long ignore_object_index,
    c_static_sized_dynamic_array<s_decal_collision_result, 16> *collisions)
{
    const unsigned long &collision_flags = k_collision_test_all_objects.collision_flags | k_collision_test_structure_and_water.collision_flags;
    const unsigned long &object_flags = k_collision_test_all_objects.object_flags | k_collision_test_structure_and_water.object_flags;
    s_collision_test_flags flags;
    flags.collision_flags = collision_flags | FLAG(14);
    flags.object_flags = object_flags | FLAG(0);
    s_collision_tester tester;
    tester.flags = flags;
    tester.point = *point;
    tester.vector = *vector;
    const s_decals_system_definition *definition = get_definition();
    tester.radius = definition->radius_max;
    tester.ignore_object_index = ignore_object_index;
    tester.structure_index = NONE;
    tester.instance_index = NONE;
    if (definition->get_no_structure_collision())
        tester.flags.collision_flags &= ~FLAG(0);
    if (definition->get_no_instance_collision())
        tester.flags.collision_flags &= ~FLAG(3);
    tester.set_length(1.0f);
    tester.offset_by(&tester.vector, -0.01f);
    s_decal_collision_result *collision = collisions->end();
    if (tester.test_vector(collision) && (collision->get_type() == 1 || collision->get_type() == 3) && check_overlap(collision->get_point()))
    {
        collisions->add_element();
        set_center(collision->get_point());
        m_cluster_references[0] = *collision->get_cluster_reference();
        s_decal_cylinder cylinder;
        memcpy(&cylinder.matrix, get_projection(), sizeof(real_matrix4x3));
        cylinder.radius = definition->radius_max;
        cylinder.height = definition->radius_max;
        long cluster_count = 0;
        structure_clusters_in_sphere(m_cluster_references[0], &cylinder.matrix.position,
            DECALS_MAX(cylinder.radius, cylinder.height / 2.0f), NUMBEROF(m_cluster_references), m_cluster_references.begin(),
            &cluster_count);
        const unsigned long &first_collision_only = definition->get_first_collision_only();
        if (!first_collision_only && !definition->get_no_secondary_collisions() && !definition->get_no_instance_secondary_collisions())
        {
            if (collision->get_type() == 3)
            {
                s_decal_collision_result *secondary = collisions->end();
                s_collision_tester back_tester = {tester.flags, *collision->get_point(), *collision->get_normal(), tester.radius,
                    tester.ignore_object_index, tester.structure_index, tester.instance_index};
                back_tester.flags.collision_flags &= ~FLAG(3);
                back_tester.set_length(-cylinder.height);
                back_tester.offset_by(&back_tester.vector, -0.5f);
                if (back_tester.test_vector(secondary) && cylinder.contains_point(secondary->get_point()))
                {
                    if (!definition->get_same_material_only() || collision->get_material_index() == secondary->get_material_index())
                        collisions->add_element();
                }
            }
            s_collision_tester instance_tester = {tester.flags, *collision->get_point(), *collision->get_normal(), tester.radius,
                tester.ignore_object_index, tester.structure_index, tester.instance_index};
            instance_tester.flags.collision_flags &= ~(FLAG(0) | FLAG(1));
            instance_tester.set_length(-cylinder.height);
            instance_tester.offset_by(&instance_tester.vector, -0.5f);
            long ignore_instance_index = collision->get_type() == 3 ? collision->get_instanced_geometry_instance_index() : NONE;
            const structure_instanced_geometry_instance *instance;
            for (c_instanced_geometry_iterator iterator(&cylinder, m_cluster_references.begin(), ignore_instance_index);
                (instance = iterator.get_datum()) != 0; iterator.next())
            {
                if (!collisions->full())
                {
                    instance_tester.structure_index = iterator.get_structure_bsp_index();
                    instance_tester.instance_index = iterator.get_instance_index();
                    s_decal_collision_result *instance_collision = collisions->end();
                    real_vector3d to_center;
                    vector_from_points3d(collision->get_point(), &instance->world_bounding_sphere_center, &to_center);
                    float t = dot_product3d(&instance_tester.vector, &to_center) / magnitude3d(&instance_tester.vector);
                    real_vector3d direction;
                    point_from_line3d(&to_center, &instance_tester.vector, t, &direction);
                    normalize3d(&direction);
                    instance_tester.offset_by(&direction, tester.radius * 0.5f);
                    instance_tester.nudge_by(&direction, tester.radius * 0.5f);
                    if (instance_tester.test_vector(instance_collision))
                    {
                        if (!definition->get_same_material_only() || collision->get_material_index() == instance_collision->get_material_index())
                            collisions->add_element();
                    }
                    instance_tester.offset_by(&direction, tester.radius * -0.5f);
                    instance_tester.nudge_by(&direction, tester.radius * -0.5f);
                }
            }
        }
        return true;
    }
    return false;
}


static __forceinline unsigned long *decals_get_current_system_count(bool preplaced)
{
    return &g_decal_counts[preplaced].system_count;
}
static __forceinline float decals_sine(float angle)
{
    return (float)sin((double)angle);
}
static __forceinline void decals_cluster_reference_initialize(s_cluster_reference *reference, s_cluster_reference value, bool invalidate)
{
    if (invalidate)
        *reference = value;
}

// @address 0x22f218 decal_system_create
extern "C" long decal_system_create(unsigned char type, long system_index, long definition_index, long ignore_object_index, bool preplaced,
    const real_point3d *point, const real_vector3d *forward, const real_vector3d *up)
{
    const s_decals_system_definition *definition = s_decals_system_definition::get(definition_index);
    system_index = NONE;
    unsigned long count = *decals_get_current_system_count(preplaced);
    if (count < (preplaced ? 448UL : 256UL)
        && (preplaced || count + 1 <= g_effect_budget_limits->table->blocks[13].limits[(unsigned char)type].maximum_count))
    {
        (*decals_get_current_system_count(preplaced))++;
        system_index = datum_new(g_decal_system_data);
        c_decal_system *system = &((c_decal_system *)(g_decal_system_data)->data)[(unsigned short)system_index];
        system->m_definition_index = definition_index;
        system->m_first_child_index = NONE;
        system->m_flags = 0;
        system->set_preplaced(preplaced);
        s_cluster_reference invalid_reference = invalid_cluster_reference_get();
        unsigned long remaining = 4;
        do {
            decals_cluster_reference_initialize(&system->m_cluster_references[4 - remaining], invalid_reference, true);
        } while (--remaining);
        system->m_projection.scale = 1.0f;
        system->set_projection_forward(forward);
        normalize3d_with_default(&system->m_projection.rotation.forward, global_forward3d);
        if (!up)
            system->set_projection_up(fabsf(dot_product3d(global_up3d,
                &system->m_projection.rotation.forward)) > 0.9999f ? global_left3d : global_up3d);
        else
        {
            system->set_projection_up(up);
            normalize3d(&system->m_projection.rotation.up);
        }
        decals_cross_product3d(&system->m_projection.rotation.up, &system->m_projection.rotation.forward, &system->m_projection.rotation.left);
        normalize3d(&system->m_projection.rotation.left);
        decals_cross_product3d(&system->m_projection.rotation.forward, &system->m_projection.rotation.left, &system->m_projection.rotation.up);
        system->m_rotation = 0.0f;
        if (TEST_BIT(definition->flags, 0))
            system->m_rotation = _real_random_range(get_local_random_seed_address(), 0.0f, 6.2831855f);
        if ((bool)((definition->flags >> 1) & 1))
        {
            random_seed_next(get_local_random_seed_address());
            if (TEST_BIT(*get_local_random_seed_address(), 31))
                system->m_flags |= 2;
        }
        if ((bool)((definition->flags >> 2) & 1))
        {
            random_seed_next(get_local_random_seed_address());
            if (TEST_BIT(*get_local_random_seed_address(), 31))
                system->m_flags |= 4;
        }
        real_matrix4x3 rotation = *global_identity4x3;
        matrix4x3_rotation_from_axis_and_angle(&rotation, global_forward3d, decals_sine(system->m_rotation), cosine(system->m_rotation));
        if ((bool)((system->m_flags >> 1) & 1))
            decals_projection_negate_vector(&rotation.rotation.left, &rotation.rotation.left);
        if ((bool)((system->m_flags >> 2) & 1))
            decals_projection_negate_vector(&rotation.rotation.up, &rotation.rotation.up);
        matrix4x3_compute_product(&system->m_projection, &rotation, &system->m_projection);
        bool success = false;
        c_static_sized_dynamic_array<s_decal_collision_result, 16> collisions;
        if (system->collide(point, &system->m_projection.rotation.forward, ignore_object_index, &collisions))
        {
            c_decal *last_decal = 0;
            for (long index = 0; index < definition->decal_count; index++)
            {
                long decal_index = c_decal::create((unsigned char)type, system_index, index);
                if (decal_index != NONE)
                {
                    if (last_decal)
                        last_decal->m_next_decal_index = decal_index;
                    else
                        system->m_first_child_index = decal_index;
                    last_decal = &((c_decal *)g_decal_array->data)[(unsigned short)decal_index];
                }
            }
            bool built = true;
            for (long decal_index = system->m_first_child_index; decal_index != NONE; )
            {
                c_decal *decal = &((c_decal *)g_decal_array->data)[(unsigned short)decal_index];
                built &= decals_build_decal_mesh((c_decal *)decal, (unsigned char)type, &collisions);
                decal_index = decal->m_next_decal_index;
            }
            if (built)
            {
                success = true;
                g_decal_last_created_system = system;
            }
        }
        if (!success)
        {
            c_decal_system::destroy(system_index);
            system_index = NONE;
        }
        g_decal_system_allocation_available[(unsigned char)type] = true;
    }
    else
        g_decal_system_allocation_available[(unsigned char)type] = false;
    return system_index;
}

static inline s_collision_test_flags decal_collision_test_flags_new(unsigned long collision_flags, unsigned long object_flags)
{
    s_collision_test_flags result = { collision_flags, object_flags };
    return result;
}

t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_message_queue_allocator> g_decal_counts_allocator;
// @address 0x708038 ??__Ek_decal_vector_test_mask@@YAXXZ
s_collision_test_flags k_decal_vector_test_mask = decal_collision_test_flags_new(11, 1);
t_restricted_allocation_manager<3, &c_decal_system::adopt_old_render_message_queue, static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_message_queue_allocator> g_decal_message_queue_allocator;
t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_lightprobe_slot_data_allocator> g_decal_system_allocator;
t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_data_allocator> g_decal_array_allocator;


// @address 0x8c29cc g_decal_system_pvs_locked
bool g_decal_system_pvs_locked;
// @address 0x8c29cd g_decal_force_offset
bool g_decal_force_offset;
// @address 0x8c29d4 g_decal_last_created_system
const c_decal_system *g_decal_last_created_system;
// @address 0x119a418 g_decal_system_allocation_available
bool g_decal_system_allocation_available[2];
// @address 0x119a41c g_decal_allocation_available
unsigned char g_decal_allocation_available[2];

// @address 0x11df7a8 g_decal_counts
__declspec(thread) s_decals_pass_counts *g_decal_counts;

// @address 0x11df84c g_decal_array
__declspec(thread) s_data_array *g_decal_array;

// @address 0x11df7b4 g_decal_system_data
__declspec(thread) s_data_array *g_decal_system_data;

// @address 0x11df874 g_decal_message_queue
__declspec(thread) t_message_queue<c_decal_message_union, 256> *g_decal_message_queue;

// @address 0x8c46bc g_rasterizer_use_floating_point_z_buffer
