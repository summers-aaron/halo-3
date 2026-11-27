// blamlibxbox/visibility_portal_traversal.obj: from the records 0x154b00 0x154f20 0x155020 0x1550d0 0x1551d8 0x2b0078 0x2b0110 0x2b09e0 0x2b0c68 0x2b0eb0 0x2b0f98 0x2b1180 0x2b11d8 0x2b13c0
// @flags /TP /O2

#include "visibility/visibility_portal_activation_types.h"
#include "structures/structure_runtime_resources.h"
#include <stddef.h>
#include "cseries/long_sort.h"
#include "visibility/visibility_projection_types.h"
#include "objects/object_reference_types.h"
#include "tag_files/tag_groups.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "visibility/visibility_portal_traversal.h"
#include "structures/structure_bsp_definitions.h"
#include "structures/structure_seams.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_definitions.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "math/convex_hulls.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "structures/cluster_bit_vectors.h"
#include "structures/structure_portal_reference_types.h"
#include "structures/structure_cluster_portal.h"
#include "structures/structure_bsp_tag.h"
#include "memory/bit_vectors.h"
#include "memory/static_arrays_flags.h"
#include <math.h>
#include "visibility/visibility_region_types.h"
#include "structures/structure_seam_cluster_stack.h"

enum
{
    _portal_facing_none = 0,
    _portal_facing_away,
    _portal_facing_toward,
    _portal_facing_both,
};

struct s_cluster_working_portals
{
    unsigned long cluster_bits[16][8];
    short first_portals[16][256];
    short next_portals[256];

    long get_first(s_cluster_reference cluster_reference);
    long get_next(s_cluster_reference cluster_reference, long portal_index);
    void insert(s_cluster_reference cluster_reference, long portal_index);
    s_cluster_working_portals();
};

struct s_working_portal_polygon
{
    real_rectangle2d bounds;
    short point_count;
    real_point2d *points;
};

struct s_transformed_portal
{
    long flags;
    unsigned char facing;
    short cluster_indices[2];
    float depth;
    s_working_portal_polygon polygon;
};

struct s_transformed_portal_stack
{
    long count;
    s_transformed_portal portals[512];

    __forceinline s_transformed_portal_stack() : count(0) {}

    __forceinline bool full() const { return count == 512; }

    __forceinline long push() { return count++; }
};

struct s_vertex_allocator
{
    long counts[4];
    real_point2d points[5120];

    __forceinline s_vertex_allocator() { counts[0] = 0; counts[1] = 0; counts[2] = 0; counts[3] = 0; }

    __forceinline long remaining() const { return 5120 - counts[0]; }

    __forceinline real_point2d *allocate(long count)
    {
        real_point2d *result = &points[counts[0]];
        counts[0] += (count + 1) & ~1;
        return result;
    }

    __forceinline void deallocate(real_point2d *allocation)
    {
        long index = allocation - points;
        counts[0] = index;
    }

    __forceinline real_point2d *reallocate(real_point2d *allocation, long count)
    {
        deallocate(allocation);
        return allocate(count);
    }
};

struct s_transformed_portal_cache
{
    const visibility_projection *projection;
    real_plane3d projection_plane;
    c_static_flags_no_init<512> bsp_cluster_flags[16];
    short portal_indices[16][512];
    s_transformed_portal_stack transformed_portals;
    __declspec(align(16)) s_vertex_allocator vertices;

    s_transformed_portal_cache(const visibility_projection *projection_in);
    s_transformed_portal *transform_portal(s_portal_reference portal_reference);
    bool projection_origin_in_portal(const cluster_portal *portal, const real_plane3d *plane);
};


struct s_working_portal
{
    short a;
    s_portal_reference portal_reference;
    s_cluster_reference c;
    short d;
    unsigned char e;
    s_working_portal_polygon polygon;
    float f;
    long merged_portal_index;
};

struct s_working_portal_stack
{
    long m_count;
    s_working_portal m_elements[256];
    __declspec(align(16)) s_vertex_allocator vertices;

    __forceinline s_working_portal_stack() : m_count(0) {}
    __forceinline bool full() const { return m_count == 256; }
    __forceinline s_working_portal *get(long index) { return &m_elements[index]; }
    __forceinline long push() { return m_count++; }
};

template <typename t_element, long k_maximum_count>
struct s_static_index_queue
{
    long m_head;
    long m_tail;
    t_element m_elements[k_maximum_count];

    __forceinline s_static_index_queue() : m_head(0), m_tail(0) {}
    __forceinline bool empty() { return m_head == m_tail; }
    __forceinline bool full() { return (m_tail + 1) % k_maximum_count == m_head; }
    __forceinline long increment(long &index)
    {
        long result = index;
        index++;
        if (index == k_maximum_count)
            index = 0;
        return result;
    }
    __forceinline void enqueue(t_element element) { m_elements[increment(m_tail)] = element; }
    __forceinline t_element dequeue() { return m_elements[increment(m_head)]; }
};

long s_cluster_working_portals::get_first(s_cluster_reference cluster_reference)
{
    if (!bit_vector_test(cluster_bits[cluster_reference.bsp_index], cluster_reference.cluster_index)) {
        return -1;
    }
    return first_portals[cluster_reference.bsp_index][cluster_reference.cluster_index];
}

long s_cluster_working_portals::get_next(s_cluster_reference cluster_reference, long portal_index)
{
    return next_portals[portal_index];
}

static __forceinline long vpt_active_structure_bsp_first_index_get(void)
{
    return 31 - _CountLeadingZeros(g_active_structure_bsp_mask & -g_active_structure_bsp_mask);
}

static __forceinline long vpt_active_structure_bsp_next_index_get(long index)
{
    unsigned long mask = g_active_structure_bsp_mask & ~(unsigned long)((1i64 << (index + 1)) - 1);
    return 31 - _CountLeadingZeros(-mask & mask);
}

static __forceinline structure_bsp *vpt_structure_bsp_get(long index)
{
    scenario *current_scenario = global_scenario;
    // Direct subscript: routing this through the shared tag_get inline flips
    // downstream scheduling in two callers (see REPORT "Needs a rule exemption").
    return (structure_bsp *)global_tag_instances[
        DATUM_INDEX_TO_ABSOLUTE_INDEX(((scenario_structure_bsp_reference *)current_scenario->structure_bsps.address)[index].structure_bsp.index)].definition;
}

// @address 0x154f20 ??0s_transformed_portal_cache@@QAA@PBUvisibility_projection@@@Z
s_transformed_portal_cache::s_transformed_portal_cache(const visibility_projection *projection_in)
    : projection(projection_in)
{
    projection_plane.n.i = visibility_projections_and_volumes_zero;
    projection_plane.n.j = visibility_projections_and_volumes_zero;
    projection_plane.n.k = visibility_projections_and_volumes_minus_one;
    projection_plane.d = visibility_projections_and_volumes_zero;
    for (long bsp_index = vpt_active_structure_bsp_first_index_get(); bsp_index != -1; bsp_index = vpt_active_structure_bsp_next_index_get(bsp_index)) {
        bsp_cluster_flags[bsp_index].fill(vpt_structure_bsp_get(bsp_index)->cluster_portals.count, 0);
    }
}

void s_cluster_working_portals::insert(s_cluster_reference cluster_reference, long portal_index)
{
    if (!bit_vector_test(cluster_bits[cluster_reference.bsp_index], cluster_reference.cluster_index)) {
        next_portals[portal_index] = -1;
        first_portals[cluster_reference.bsp_index][cluster_reference.cluster_index] = (short)portal_index;
        bit_vector_set(cluster_bits[cluster_reference.bsp_index], cluster_reference.cluster_index);
    }
    else {
        next_portals[portal_index] = first_portals[cluster_reference.bsp_index][cluster_reference.cluster_index];
        first_portals[cluster_reference.bsp_index][cluster_reference.cluster_index] = (short)portal_index;
    }
}

static __forceinline long vpt_bit_count(unsigned long bits)
{
    bits = ((bits >> 1) & 0x55555555) + (bits & 0x55555555);
    bits = ((bits >> 2) & 0x33333333) + (bits & 0x33333333);
    bits = ((bits >> 4) & 0x0F0F0F0F) + (bits & 0x0F0F0F0F);
    bits = ((bits >> 8) & 0x00FF00FF) + (bits & 0x00FF00FF);
    return (bits >> 16) + (bits & 0xFFFF);
}

static __forceinline s_scenario_bsp_pvs *vpt_pvs_bsp_get(s_cluster_reference cluster_reference)
{
    scenario *current_scenario = global_scenario;
    s_scenario_zone_set *zone_set = &((s_scenario_zone_set *)current_scenario->zone_sets.address)[global_zone_set_index];
    long bsp_slot = vpt_bit_count(((1 << cluster_reference.bsp_index) - 1) & zone_set->bsp_active_mask);
    s_scenario_zone_set_visibility *visibility = &((s_scenario_zone_set_visibility *)current_scenario->zone_set_pvs.address)[zone_set->pvs_index];
    return &((s_scenario_bsp_pvs *)visibility->structure_bsp_pvs.address)[bsp_slot];
}

// @address 0x1550d0 vpt_pvs_get_visible_clusters
// match: 0x1550d0: pointer __restrict qualifier prevents alias reload
extern "C" short *vpt_pvs_get_visible_clusters(s_cluster_reference cluster_reference, long *__restrict portal_count)
{
    s_scenario_cluster_portals *cluster = &((s_scenario_cluster_portals *)
        vpt_pvs_bsp_get(cluster_reference)->cluster_portals.address)[cluster_reference.cluster_index];
    *portal_count = cluster->portal_indices.count;
    return (short *)cluster->portal_indices.address;
}

static __forceinline float vpt_plane3d_distance_to_point(const real_plane3d *plane, const real_point3d *point)
{
    return point->x * plane->n.i + point->y * plane->n.j + point->z * plane->n.k - plane->d;
}

static __forceinline short vpt_projection_from_vector3d(const real_vector3d *vector)
{
    float abs_i = fabs(vector->i);
    float abs_j = fabs(vector->j);
    float abs_k = fabs(vector->k);
    if (abs_k >= abs_j && abs_k >= abs_i)
        return 2;
    else if (abs_j >= abs_i)
        return 1;
    else
        return 0;
}

static __forceinline bool vpt_projection_sign_from_vector3d(const real_vector3d *vector, short projection)
{
    return (&vector->i)[projection] > 0.0f;
}

static __forceinline real_point2d *vpt_project_point3d(const real_point3d *point, short projection, bool sign, real_point2d *result)
{
    set_real_point2d(result, point->n[global_projection3d_mappings[projection][sign][0]], point->n[global_projection3d_mappings[projection][sign][1]]);
    return result;
}

// @address 0x2b13c0 ?projection_origin_in_portal@s_transformed_portal_cache@@QAA_NPBUcluster_portal@@PBUreal_plane3d@@@Z
bool s_transformed_portal_cache::projection_origin_in_portal(const cluster_portal *portal_reference, const real_plane3d *plane)
{
    const cluster_portal *portal = (const cluster_portal *)portal_reference;
    short projection_axis = vpt_projection_from_vector3d(&plane->n);
    bool projection_sign = (unsigned char)vpt_projection_sign_from_vector3d(&plane->n, projection_axis) != 0;
    real_point3d plane_origin;
    real_point2d projected_origin;
    point_from_line3d(&projection->basis_to_world.position, &plane->n,
        -vpt_plane3d_distance_to_point(plane, &projection->basis_to_world.position), &plane_origin);
    vpt_project_point3d(&plane_origin, projection_axis, projection_sign, &projected_origin);
    real_point2d projected_points[128];
    for (long point_index = 0; point_index < portal->vertices.s_tag_block::count; point_index++)
        vpt_project_point3d(&portal->vertices[point_index], projection_axis, projection_sign, &projected_points[point_index]);
    return (unsigned char)convex_hull2d_test_point((short)portal->vertices.s_tag_block::count,
        projected_points, &projected_origin, 0.0001f) != 0;
}

// @address 0x2b0078 copy_working_portal
extern "C" short copy_working_portal(s_working_portal_stack *stack, short source_index, short c, char e)
{
    short result = -1;
    if (!stack->full())
    {
        s_working_portal *source = stack->get(source_index);
        result = (short)stack->push();
        s_working_portal *destination = stack->get(result);
        destination->a = source->a;
        destination->portal_reference.value = source->portal_reference.value;
        destination->c.value() = c;
        destination->d = source->d;
        destination->polygon = source->polygon;
        destination->f = source->f;
        destination->merged_portal_index = source->merged_portal_index;
        destination->e = e;
    }
    return result;
}

static __forceinline float vpt_real_min(float a, float b)
{
    return a <= b ? a : b;
}

static __forceinline float vpt_real_max(float a, float b)
{
    return a > b ? a : b;
}

static __forceinline void vpt_rectangle2d_union(const real_rectangle2d *a, const real_rectangle2d *b, real_rectangle2d *out)
{
    // Union the horizontal and vertical bounds.
    for (long axis = 0; axis < 2; ++axis)
    {
        out->bounds[axis].lower = vpt_real_min(a->bounds[axis].lower, b->bounds[axis].lower);
        out->bounds[axis].upper = vpt_real_max(a->bounds[axis].upper, b->bounds[axis].upper);
    }
}

static __forceinline float vpt_rectangle2d_area(const real_rectangle2d *rectangle)
{
    return (rectangle->y1 - rectangle->y0) * (rectangle->x1 - rectangle->x0);
}

// @address 0x2b0eb0 build_working_portal_stack_build_region
extern "C" void build_working_portal_stack_build_region(s_cluster_working_portals *table, s_working_portal_stack *stack, short (*map)[256],
    const visibility_projection *projections, long type, s_visibility_region *region)
{
    long portal_count = stack->m_count;
    for (long portal_index = 0; portal_index < portal_count; portal_index++)
    {
        s_cluster_reference cluster_reference = stack->m_elements[portal_index].c;
        long first_portal_index;
        if (bit_vector_test(table->cluster_bits[cluster_reference.bsp_index], cluster_reference.cluster_index))
            first_portal_index = table->first_portals[cluster_reference.bsp_index][cluster_reference.cluster_index];
        else
            first_portal_index = -1;
        if (portal_index == first_portal_index)
        {
            if (!build_working_portal_stack_add_working_portal_to_region(table, stack, map, projections, type, region, portal_index, cluster_reference))
                break;
        }
    }
}

// @address 0x2b1180 ?compare_region_cluster_indices@@YA_NJJPBUs_visibility_region@@@Z
static bool compare_region_cluster_indices(long first_cluster_index, long second_cluster_index, const s_visibility_region *region)
{
    const s_cluster_reference *first = &region->clusters[first_cluster_index].cluster_reference;
    const s_cluster_reference *second = &region->clusters[second_cluster_index].cluster_reference;
    if (first->bsp_index != second->bsp_index)
        return first->bsp_index > second->bsp_index;
    return first->cluster_index > second->cluster_index;
}

static __forceinline void vpt_qsort_4byte(long *base, unsigned long count, bool (*compare)(long, long, const void *), const void *context)
{
    if (count >= 2)
        templated_sort<long>::qsort(base, count, compare, context);
}

// @address 0x2b11d8 ?visibility_build_region_from_pvs@@YAXPBUvisibility_projection@@Us_cluster_reference@@PAUs_visibility_region@@@Z
void visibility_build_region_from_pvs(const visibility_projection *projection, s_cluster_reference source, s_visibility_region *region)
{
    region->projections[0] = *projection;
    region->projection_count = 1;
    region->cluster_count = 0;
    region->volume_count = 0;
    if (source.bsp_index != -1) {
        s_game_cluster_bit_vectors pvs;
        structure_bsp_compute_cluster_active_pvs(source, &pvs);
        for (long bsp = vpt_active_structure_bsp_first_index_get(); bsp != -1; bsp = vpt_active_structure_bsp_next_index_get(bsp)) {
            structure_bsp *structure = vpt_structure_bsp_get(bsp);
            for (long index = 0; index < structure->clusters.count; ++index) {
                if (bit_vector_test(pvs.bit_vectors[bsp].m_storage, index) && region->cluster_count < 128) {
                    visibility_cluster *cluster = &region->clusters[region->cluster_count];
                    region->cluster_indices[region->cluster_count] = region->cluster_count;
                    cluster->cluster_reference.bsp_index = (signed char)bsp;
                    cluster->cluster_reference.cluster_index = (unsigned char)index;
                    cluster->volume_count[0] = 0;
                    cluster->first_volume[0] = -1;
                    ++region->cluster_count;
                    if (region->volume_count < 512) {
                        short volume_index = region->volume_count++;
                        visibility_volume_build(projection, 0, &projection->volume.rectangle, &region->volumes[volume_index]);
                        ++cluster->volume_count[0];
                        if (cluster->first_volume[0] == -1) cluster->first_volume[0] = volume_index;
                    }
                }
            }
        }
        vpt_qsort_4byte(region->cluster_indices, region->cluster_count,
            reinterpret_cast<bool (*)(long, long, const void *)>(compare_region_cluster_indices), region);
    }
}

// @address 0x2b09e0 ?build_working_portal_stack_merge_working_portals@@YAXPAUs_cluster_working_portals@@PAUs_working_portal_stack@@@Z
void build_working_portal_stack_merge_working_portals(s_cluster_working_portals *cluster_portals, s_working_portal_stack *stack)
{
    for (long bsp_index = vpt_active_structure_bsp_first_index_get(); bsp_index != -1; bsp_index = vpt_active_structure_bsp_next_index_get(bsp_index))
    {
        s_cluster_reference cluster_reference;
        cluster_reference.bsp_index = (signed char)bsp_index;
        structure_bsp *bsp = vpt_structure_bsp_get(bsp_index);
        for (cluster_reference.cluster_index = 0; cluster_reference.cluster_index < bsp->clusters.count; ++cluster_reference.cluster_index)
        {
            for (long portal_index = cluster_portals->get_first(cluster_reference); portal_index != -1;
                portal_index = cluster_portals->get_next(cluster_reference, portal_index))
            {
                s_working_portal *portal = stack->get(portal_index);
                for (long merge_index = cluster_portals->get_next(cluster_reference, portal_index); merge_index != -1;
                    merge_index = cluster_portals->get_next(cluster_reference, merge_index))
                {
                    s_working_portal *merge_portal = stack->get(merge_index);
                    real_rectangle2d bounds = merge_portal->polygon.bounds;
                    vpt_rectangle2d_union(&bounds, &portal->polygon.bounds, &bounds);
                    if (vpt_rectangle2d_area(&bounds) < (vpt_rectangle2d_area(&merge_portal->polygon.bounds)
                        + vpt_rectangle2d_area(&portal->polygon.bounds)) * 1.5f)
                    {
                        portal->merged_portal_index = merge_index;
                        merge_portal->f = vpt_real_min(portal->f, merge_portal->f);
                        merge_portal->polygon.bounds = bounds;
                        break;
                    }
                }
            }
        }
    }
}

static __forceinline s_scenario_bsp_pvs *vpt_neighbor_pvs_get(s_cluster_reference reference)
{
 scenario *current = global_scenario;
 
 s_scenario_zone_set *zone = &((s_scenario_zone_set *)current->zone_sets.address)[global_zone_set_index];
 long slot = vpt_bit_count((1i64 << reference.bsp_index) - 1 & zone->bsp_active_mask);
 s_scenario_zone_set_visibility *visibility = &((s_scenario_zone_set_visibility *)current->zone_set_pvs.address)[zone->pvs_index];
 return &((s_scenario_bsp_pvs *)visibility->structure_bsp_pvs.address)[slot];
}
// @address 0x2b0c68 ?build_working_portal_stack_add_working_portal_to_region@@YA_NPAUs_cluster_working_portals@@PAUs_working_portal_stack@@QAY0BAA@FPBUvisibility_projection@@JPAUs_visibility_region@@JUs_cluster_reference@@@Z
bool build_working_portal_stack_add_working_portal_to_region(s_cluster_working_portals *table,
    // match: 0x2b0c68: pointer __restrict qualifier prevents alias reload
    s_working_portal_stack *stack, short (*const __restrict map)[256], const visibility_projection *projections,
    long group, s_visibility_region *region, long portal_index, s_cluster_reference reference)
{
    visibility_cluster *cluster = 0;
    bool success = true;
    long cluster_index = map[reference.bsp_index][reference.cluster_index];
    if (cluster_index != -1) {
        cluster = &region->clusters[cluster_index];
    } else if (region->cluster_count < 128) {
        cluster_index = region->cluster_count;
        ++region->cluster_count;
        region->cluster_indices[cluster_index] = cluster_index;
        map[reference.bsp_index][reference.cluster_index] = cluster_index;
        cluster = &region->clusters[cluster_index];
        cluster->cluster_reference = reference;
        memset(cluster->volume_count, 0, sizeof(cluster->volume_count));
        memset(cluster->first_volume, -1, sizeof(cluster->first_volume));
    } else {
        success = false;
    }
    if (success) {
        do {
            if (region->volume_count >= 512) {
                success = false;
                break;
            }
            s_working_portal *portal = &stack->m_elements[portal_index];
            if (portal->merged_portal_index == -1) {
                short volume_index = region->volume_count;
                ++region->volume_count;
                visibility_volume *volume = &region->volumes[volume_index];
                if (visibility_volume_build(&projections[group], group, &portal->polygon.bounds, volume)) {
                    volume->cluster_index = cluster_index;
                    volume->distance = portal->f;
                    s_portal_reference reference = portal->portal_reference;
                    portal->d = volume_index;
                    long bsp_index = reference.bsp_index;
                    if (bsp_index != -1) {
                        long structure_portal_index = reference.portal_index;
                        structure_bsp *bsp = vpt_structure_bsp_get(reference.bsp_index);
                        volume->portal_flags = ((cluster_portal *)bsp->cluster_portals.address)[structure_portal_index].plane_index;
                        volume->portal_side = portal->portal_reference.bsp_index;
                    } else {
                        volume->portal_flags = -1;
                        volume->portal_side = -1;
                    }
                    if ((unsigned short)cluster->first_volume[group] == 0xffff) cluster->first_volume[group] = volume_index;
                    ++cluster->volume_count[group];
                }
            }
            portal_index = table->next_portals[portal_index];
        } while (portal_index != -1);
    }
    return success;
}

// @address 0x2b0f98 vpt_cluster_find_neighbor_outside_pvs
extern "C" s_cluster_reference vpt_cluster_find_neighbor_outside_pvs(s_cluster_reference reference)
{
    const unsigned long *pvs = (const unsigned long *)vpt_neighbor_pvs_get(reference)->cluster_visibility.address;
    if (bit_vector_test_unsigned(pvs, reference.cluster_index)) {
        structure_bsp *bsp = vpt_structure_bsp_get(reference.bsp_index);
        const structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[reference.cluster_index];
        for (long portal_index = 0; portal_index < cluster->portal_indices.count; ++portal_index) {
            const cluster_portal *portal = &((cluster_portal *)bsp->cluster_portals.address)[((short *)cluster->portal_indices.address)[portal_index]];
            bool back_side = portal->cluster_indices[1] == reference.cluster_index;
            if (!bit_vector_test(pvs, portal->cluster_indices[!back_side])) {
                reference.cluster_index = (unsigned char)portal->cluster_indices[!back_side];
                break;
            }
        }
    }
    return reference;
}

// @rdata 0x82021694 vpt_maximum_depth
static const float vpt_maximum_depth = 3.402823466e+38f;

static __forceinline const real_plane3d *vpt_structure_bsp_get_plane(const structure_bsp *bsp, long plane_index)
{
    const s_structure_bsp_resource *resource = &bsp->resource;
    const s_structure_bsp_resources *resources = resource->use_resource_items
        ? resource->collision_resource_interface.get_resources()
        : (s_structure_bsp_resources *)resource->raw_resources.address;
    return &((real_plane3d *)resources->collision_bsp->planes.address)[plane_index];
}

static __inline real_rectangle2d *vpt_rectangle2d_enclose_point(real_rectangle2d *rectangle, const real_point2d *point)
{
    if (rectangle->x0 > point->x)
        rectangle->x0 = point->x;
    if (rectangle->x1 < point->x)
        rectangle->x1 = point->x;
    if (rectangle->y0 > point->y)
        rectangle->y0 = point->y;
    if (rectangle->y1 < point->y)
        rectangle->y1 = point->y;
    return rectangle;
}

static __forceinline real_rectangle2d *vpt_rectangle2d_expand(const real_rectangle2d *rectangle, float amount, real_rectangle2d *result)
{
    result->x0 = rectangle->x0 - amount;
    result->x1 = rectangle->x1 + amount;
    result->y0 = rectangle->y0 - amount;
    result->y1 = rectangle->y1 + amount;
    return result;
}

static __forceinline long vpt_polygon2d_from_bounds(const real_rectangle2d *bounds, real_point2d *points)
{
    points[0].x = bounds->x0;
    points[0].y = bounds->y0;
    points[1].x = bounds->x1;
    points[1].y = bounds->y0;
    points[2].x = bounds->x1;
    points[2].y = bounds->y1;
    points[3].x = bounds->x0;
    points[3].y = bounds->y1;
    return 4;
}


static __forceinline long vpt_compute_portal_facing(s_transformed_portal_cache *cache, const cluster_portal *portal,
    const real_plane3d *plane, long point_count, bool *origin_in_portal)
{
    long facing;
    float distance = vpt_plane3d_distance_to_point(plane, &cache->projection->basis_to_world.position);
    if (fabsf(distance) > 0.015625f)
    {
        *origin_in_portal = false;
        if (point_count <= 0)
            facing = _portal_facing_none;
        else if (point_count < 3)
            facing = _portal_facing_both;
        else if (distance < 0.0f)
            facing = _portal_facing_away;
        else
            facing = _portal_facing_toward;
    }
    else
    {
        *origin_in_portal = cache->projection_origin_in_portal((const cluster_portal *)portal, plane);
        if (*origin_in_portal || point_count > 0)
            facing = _portal_facing_both;
        else
            facing = _portal_facing_none;
    }
    return facing;
}

// @address 0x1551d8 ?transform_portal@s_transformed_portal_cache@@QAAPAUs_transformed_portal@@Us_portal_reference@@@Z
s_transformed_portal *s_transformed_portal_cache::transform_portal(s_portal_reference portal_reference)
{
    s_transformed_portal *transformed_portal = 0;
    if (!bsp_cluster_flags[portal_reference.bsp_index].test(portal_reference.portal_index))
    {
        if (!transformed_portals.full() && vertices.remaining() >= 128)
        {
        long transformed_portal_index = transformed_portals.push();
        structure_bsp *bsp = vpt_structure_bsp_get(portal_reference.bsp_index);
        cluster_portal *portal = &((cluster_portal *)bsp->cluster_portals.address)[portal_reference.portal_index];
        const real_plane3d *plane = vpt_structure_bsp_get_plane(bsp, portal->plane_index);
        transformed_portal = &transformed_portals.portals[transformed_portal_index];
        real_point3d transformed_points[128];
        real_point3d clipped_points[128];
        matrix4x3_transform_points(&projection->world_to_basis, portal->vertices.s_tag_block::count, portal->vertices.begin(), transformed_points);
        long clipped_point_count = plane3d_clip_polygon(&projection_plane, true, 0.0078125f,
            portal->vertices.s_tag_block::count, transformed_points, 128, clipped_points);
        bool origin_in_portal;
        transformed_portal->facing = vpt_compute_portal_facing(this, portal, plane, clipped_point_count, &origin_in_portal);
        transformed_portal->cluster_indices[0] = portal->cluster_indices[0];
        transformed_portal->cluster_indices[1] = portal->cluster_indices[1];
        transformed_portal->depth = vpt_maximum_depth;
        transformed_portal->polygon.bounds = *global_null_rectangle2d;
        transformed_portal->flags = portal->flags;
        if (transformed_portal->facing != _portal_facing_none)
        {
            long point_index = transformed_portal->facing == _portal_facing_away ? clipped_point_count - 1 : 0;
            long point_step = transformed_portal->facing == _portal_facing_away ? -1 : 1;
            transformed_portal->polygon.points = vertices.allocate(128);
            transformed_portal->polygon.point_count = (short)clipped_point_count;
            if (clipped_point_count > 0)
            for (long clipped_index = 0; clipped_index != clipped_point_count; clipped_index++, point_index += point_step)
            {
                const real_point3d *clipped_point = &clipped_points[clipped_index];
                real_point2d *point = &transformed_portal->polygon.points[point_index];
                float scale = -1.0f / clipped_point->z;
                point->x = clipped_point->x * scale;
                point->y = clipped_point->y * scale;
                vpt_rectangle2d_enclose_point(&transformed_portal->polygon.bounds, point);
                transformed_portal->depth = vpt_real_min(transformed_portal->depth, -clipped_point->z);
            }
            if (transformed_portal->facing == _portal_facing_both)
            {
                if (origin_in_portal)
                {
                    transformed_portal->depth = (float)(transformed_portal->depth <= 0.015625 ? transformed_portal->depth : 0.015625);
                    transformed_portal->polygon.bounds = projection->volume.rectangle;
                    transformed_portal->polygon.point_count = (short)projection->polygon_count;
                    memcpy(transformed_portal->polygon.points, projection->polygon, projection->polygon_count * sizeof(real_point2d));
                }
                else
                {
                    vpt_rectangle2d_expand(&transformed_portal->polygon.bounds, 0.00390625f, &transformed_portal->polygon.bounds);
                    transformed_portal->polygon.point_count = (short)vpt_polygon2d_from_bounds(
                        &transformed_portal->polygon.bounds, transformed_portal->polygon.points);
                }
            }
            if (projection->far_plane_valid && transformed_portal->depth > projection->far_distance)
                transformed_portal->facing = _portal_facing_none;
            vertices.reallocate(transformed_portal->polygon.points, transformed_portal->polygon.point_count);
        }
        else
        {
            transformed_portal->polygon.points = 0;
            transformed_portal->polygon.point_count = 0;
        }
        bsp_cluster_flags[portal_reference.bsp_index].set(portal_reference.portal_index, true);
        portal_indices[portal_reference.bsp_index][portal_reference.portal_index] = (short)transformed_portal_index;
    }
    }
    else
    {
        transformed_portal = &transformed_portals.portals[portal_indices[portal_reference.bsp_index][portal_reference.portal_index]];
    }
    return transformed_portal;
}

static __forceinline const s_cluster_reference *vpt_cluster_reference_set(s_cluster_reference *reference, long bsp_index, long cluster_index)
{
    reference->bsp_index = (signed char)bsp_index;
    reference->cluster_index = (unsigned char)cluster_index;
    return reference;
}

static __forceinline bool vpt_rectangle_intersects(const real_rectangle2d *a, const real_rectangle2d *b)
{
    return a->x0 <= b->x1 && a->x1 >= b->x0 && a->y0 <= b->y1 && a->y1 >= b->y0;
}

static __forceinline short vpt_working_portal_find_portal(s_working_portal_stack *stack, short index, short portal_reference)
{
    while (index != -1)
    {
        s_working_portal *portal = stack->get(index);
        if (portal->portal_reference.value == portal_reference)
            break;
        index = portal->a;
    }
    return index;
}

static __forceinline short vpt_working_portal_find_cluster(s_working_portal_stack *stack, short index, short cluster_reference)
{
    while (index != -1)
    {
        s_working_portal *portal = stack->get(index);
        if (portal->c.value() == cluster_reference)
            break;
        index = portal->a;
    }
    return index;
}

static __forceinline bool vpt_portal_is_active(s_portal_reference portal_reference)
{
    long portal_index = portal_reference.portal_index;
    return (g_active_portal_bitvectors[portal_reference.bsp_index * 16 + (portal_index >> 5)] & (1 << (portal_index & 31))) != 0;
}

static __forceinline bool vpt_polygon_clip(const s_working_portal_polygon *a, const s_working_portal_polygon *b, s_working_portal_polygon *out)
{
    out->point_count = convex_hull2d_intersect_new(a->point_count, a->points, b->point_count, b->points, 128, out->points, 0.0001f);
    if (out->point_count == -1)
    {
        out->point_count = a->point_count;
        memcpy(out->points, a->points, a->point_count * sizeof(real_point2d));
    }
    return out->point_count >= 3;
}

static __forceinline structure_bsp *vpt_scenario_structure_bsp_get(scenario *current_scenario, long index)
{
    return (structure_bsp *)tag_get('sbsp',
        ((scenario_structure_bsp_reference *)current_scenario->structure_bsps.address)[index].structure_bsp.index);
}

static __forceinline s_scenario_bsp_pvs *vpt_zone_set_pvs_bsp_get(scenario *current_scenario, s_cluster_reference reference)
{
    s_scenario_zone_set *zone_set = &((s_scenario_zone_set *)current_scenario->zone_sets.address)[global_zone_set_index];
    long bsp_slot = vpt_bit_count((1i64 << reference.bsp_index) - 1 & zone_set->bsp_active_mask);
    s_scenario_zone_set_visibility *visibility = &((s_scenario_zone_set_visibility *)current_scenario->zone_set_pvs.address)[zone_set->pvs_index];
    return &((s_scenario_bsp_pvs *)visibility->structure_bsp_pvs.address)[bsp_slot];
}

// @address 0x2b0110 ?build_working_portal_stack_traverse_portals@@YA_NFPAUs_working_portal_stack@@PAUs_cluster_working_portals@@PAU?$s_static_index_queue@F$0BAA@@@PAUs_transformed_portal_cache@@PBUvisibility_projection@@@Z
bool build_working_portal_stack_traverse_portals(short root_portal_index, s_working_portal_stack *working_portals,
    s_cluster_working_portals *cluster_portals, s_static_index_queue<short, 256> *queue,
    s_transformed_portal_cache *transformed_portal_cache, const visibility_projection *projection)
{
    scenario *current_scenario = global_scenario;
    bool success = true;
    bool stack_overflow = false;
    bool queue_overflow = false;
    bool cache_overflow = false;
    while (!queue->empty())
    {
        short working_portal_index = queue->dequeue();
        s_working_portal *working_portal = working_portals->get(working_portal_index);
        if (!working_portal->e)
        {
            s_cluster_reference cluster_reference = working_portal->c;
            structure_bsp *bsp = vpt_scenario_structure_bsp_get(current_scenario, cluster_reference.bsp_index);
            s_scenario_bsp_pvs *pvs_bsp = vpt_zone_set_pvs_bsp_get(current_scenario, cluster_reference);
            structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[cluster_reference.cluster_index];
            for (long cluster_portal_index = 0; cluster_portal_index < cluster->portal_indices.count; cluster_portal_index++)
            {
                long portal_reference_value = 0;
                portal_reference_value |= (cluster_reference.bsp_index & 0x3f) << 10;
                portal_reference_value |= ((short *)cluster->portal_indices.address)[cluster_portal_index] & 0x3ff;
                s_portal_reference portal_reference;
                portal_reference.value = (short)portal_reference_value;
                long portal_index = portal_reference.portal_index;
                cluster_portal *portal = &((cluster_portal *)bsp->cluster_portals.address)[portal_index];
                unsigned long *cluster_bits = (unsigned long *)pvs_bsp->cluster_visibility.address;
                bool portal_above = portal->cluster_indices[1] == cluster_reference.cluster_index;
                bool other_side_visible = bit_vector_test(cluster_bits, portal->cluster_indices[!portal_above]);
                if ((((cluster_portal *)bsp->cluster_portals.address)[portal_index].flags & 8) == 0 &&
                    (!other_side_visible || (other_side_visible && bit_vector_test(cluster_bits, portal->cluster_indices[portal_above]))))
                {
                    s_transformed_portal *transformed_portal = transformed_portal_cache->transform_portal(portal_reference);
                    if (transformed_portal)
                    {
                        bool one_way = (transformed_portal->flags & 2) != 0;
                        bool one_way_reversed = (transformed_portal->flags & 16) != 0;
                        bool above = transformed_portal->cluster_indices[1] == cluster_reference.cluster_index;
                        if ((transformed_portal->facing == _portal_facing_away && !above && !one_way)
                            || (transformed_portal->facing == _portal_facing_toward && above && !one_way_reversed)
                            || (transformed_portal->facing == _portal_facing_both
                                && ((!one_way && !one_way_reversed) || (one_way && above) || (one_way_reversed && !above))))
                        {
                            if (vpt_rectangle_intersects(&working_portal->polygon.bounds, &transformed_portal->polygon.bounds)
                                && vpt_working_portal_find_portal(working_portals, working_portal_index, portal_reference.value) == -1
                                && vpt_portal_is_active(portal_reference))
                            {
                                if (!working_portals->full() && working_portals->vertices.remaining() >= 128)
                                {
                                    short new_working_portal_index = (short)working_portals->push();
                                    s_working_portal *new_working_portal = working_portals->get(new_working_portal_index);
                                    new_working_portal->polygon.points = working_portals->vertices.allocate(128);
                                    if (vpt_polygon_clip(&working_portal->polygon,
                                            &transformed_portal->polygon, &new_working_portal->polygon))
                                    {
                                        s_cluster_reference new_cluster_reference;
                                        vpt_cluster_reference_set(&new_cluster_reference, cluster_reference.bsp_index,
                                            transformed_portal->cluster_indices[0] == cluster_reference.cluster_index
                                                ? transformed_portal->cluster_indices[1] : transformed_portal->cluster_indices[0]);
                                        new_working_portal->a = working_portal_index;
                                        new_working_portal->portal_reference = portal_reference;
                                        new_working_portal->c = new_cluster_reference;
                                        new_working_portal->merged_portal_index = -1;
                                        new_working_portal->d = -1;
                                        new_working_portal->polygon.bounds = *global_null_rectangle2d;
                                        new_working_portal->f = transformed_portal->depth;
                                        new_working_portal->e = 0;
                                        working_portals->vertices.reallocate(new_working_portal->polygon.points, new_working_portal->polygon.point_count);
                                        real_rectangle2d_enclose_points(&new_working_portal->polygon.bounds,
                                            new_working_portal->polygon.point_count, new_working_portal->polygon.points);
                                        cluster_portals->insert(new_cluster_reference, new_working_portal_index);
                                        if (!queue->full())
                                            queue->enqueue(new_working_portal_index);
                                        else
                                            queue_overflow = true;
                                        s_cluster_stack seam_cluster_references;
                                        if (structure_seams_connected_cluster_references_get(
                                                global_scenario_index, new_cluster_reference, false, &seam_cluster_references))
                                        {
                                            for (long seam_index = 0; seam_index < seam_cluster_references.count(); seam_index++)
                                            {
                                                s_cluster_reference seam_cluster_reference = seam_cluster_references.m_elements[seam_index];
                                                long seam_working_portal_index = copy_working_portal(
                                                    working_portals, new_working_portal_index, seam_cluster_reference.value(), 0);
                                                if (seam_working_portal_index != -1)
                                                {
                                                    cluster_portals->insert(seam_cluster_reference, seam_working_portal_index);
                                                    if (!queue->full())
                                                        queue->enqueue((short)seam_working_portal_index);
                                                    else
                                                        queue_overflow = true;
                                                }
                                                else
                                                {
                                                    stack_overflow = true;
                                                }
                                            }
                                        }
                                        long pvs_cluster_count;
                                        unsigned char *pvs_clusters = (unsigned char *)vpt_pvs_get_visible_clusters(new_cluster_reference, &pvs_cluster_count);
                                        for (long pvs_index = 0; pvs_index < pvs_cluster_count; pvs_index++)
                                        {
                                            s_cluster_reference pvs_cluster_reference;
                                            vpt_cluster_reference_set(&pvs_cluster_reference, cluster_reference.bsp_index, pvs_clusters[pvs_index]);
                                            if (vpt_working_portal_find_cluster(
                                                    working_portals, new_working_portal_index, pvs_cluster_reference.value()) == -1)
                                            {
                                                long pvs_working_portal_index = copy_working_portal(
                                                    working_portals, new_working_portal_index, pvs_cluster_reference.value(), 1);
                                                if (pvs_working_portal_index != -1)
                                                {
                                                    cluster_portals->insert(pvs_cluster_reference, pvs_working_portal_index);
                                                    if (!queue->full())
                                                        queue->enqueue((short)pvs_working_portal_index);
                                                    else
                                                        queue_overflow = true;
                                                }
                                                else
                                                {
                                                    stack_overflow = true;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        working_portals->vertices.deallocate(new_working_portal->polygon.points);
                                        working_portals->m_count--;
                                    }
                                }
                                else
                                {
                                    stack_overflow = true;
                                }
                            }
                        }
                    }
                    else
                    {
                        cache_overflow = true;
                    }
                }
            }
        }
    }
    if (queue_overflow)
        success = false;
    if (stack_overflow)
        success = false;
    if (cache_overflow)
        success = false;
    return success;
}

__forceinline s_cluster_working_portals::s_cluster_working_portals()
{
    for (long bsp_index = vpt_active_structure_bsp_first_index_get(); bsp_index != -1; bsp_index = vpt_active_structure_bsp_next_index_get(bsp_index))
        memset(cluster_bits[bsp_index], 0, ((vpt_structure_bsp_get(bsp_index)->clusters.count + 31) >> 5) * sizeof(unsigned long));
}

static __forceinline bool vpt_cluster_reference_valid(const s_cluster_reference *reference)
{
    return reference->bsp_index != -1;
}

static __forceinline short build_working_portal_stack_add_projection(
    s_working_portal_stack *stack, s_cluster_reference cluster_reference, const visibility_projection *projection)
{
    short portal_index = (short)stack->push();
    s_working_portal *portal = stack->get(portal_index);
    portal->a = -1;
    portal->portal_reference.value = -1;
    portal->d = -1;
    portal->merged_portal_index = -1;
    portal->c = cluster_reference;
    portal->polygon.bounds = projection->volume.rectangle;
    portal->polygon.points = stack->vertices.allocate(projection->polygon_count);
    const unsigned long &point_bytes = projection->polygon_count * sizeof(real_point2d);
    memcpy(portal->polygon.points, projection->polygon, point_bytes);
    portal->polygon.point_count = (short)projection->polygon_count;
    portal->f = 0.0f;
    portal->e = 0;
    return portal_index;
}

// @address 0x154b00 ?visibility_build_region_from_projections@@YAXJPBUvisibility_projection@@Us_cluster_reference@@_NPAUs_visibility_region@@2@Z
void visibility_build_region_from_projections(long projection_count, const visibility_projection *projections, s_cluster_reference cluster,
    bool projections_empty, s_visibility_region *region, bool use_pvs)
{
    region->projection_count = (short)projection_count;
    if (projections != region->projections)
        memcpy(region->projections, projections, projection_count * sizeof(visibility_projection));
    region->cluster_count = 0;
    region->volume_count = 0;
    if (vpt_cluster_reference_valid(&cluster))
    {
        s_cluster_reference root_cluster = vpt_cluster_find_neighbor_outside_pvs(cluster);
        short cluster_portal_map[16][256];
        memset(cluster_portal_map, -1, sizeof(cluster_portal_map));
        for (long projection_index = 0; projection_index < projection_count; projection_index++)
        {
            const visibility_projection *projection = &projections[projection_index];
            s_working_portal_stack working_portals;
            s_transformed_portal_cache transformed_portal_cache(projection);
            s_cluster_working_portals cluster_portals;
            s_static_index_queue<short, 256> queue;
            short root_portal_index = build_working_portal_stack_add_projection(&working_portals, root_cluster, projection);
            cluster_portals.insert(root_cluster, root_portal_index);
            queue.enqueue(root_portal_index);
            s_cluster_stack seam_cluster_references;
            if (structure_seams_connected_cluster_references_get(global_scenario_index, root_cluster, false, &seam_cluster_references))
            {
                for (long seam_index = 0; seam_index < seam_cluster_references.count(); seam_index++)
                {
                    short seam_portal_index = copy_working_portal(
                        &working_portals, root_portal_index, seam_cluster_references.m_elements[seam_index].value(), 0);
                    cluster_portals.insert(seam_cluster_references.m_elements[seam_index], seam_portal_index);
                    long seam_queue_index = seam_portal_index;
                    queue.enqueue((short)seam_queue_index);
                }
            }
            long pvs_cluster_count;
            unsigned char *pvs_clusters = (unsigned char *)vpt_pvs_get_visible_clusters(root_cluster, &pvs_cluster_count);
            s_cluster_reference pvs_cluster_reference;
            for (long pvs_index = 0; pvs_index < pvs_cluster_count; pvs_index++)
            {
                vpt_cluster_reference_set(&pvs_cluster_reference, root_cluster.bsp_index, pvs_clusters[pvs_index]);
                long pvs_portal_index = copy_working_portal(&working_portals, root_portal_index, pvs_cluster_reference.value(), 1);
                cluster_portals.insert(pvs_cluster_reference, pvs_portal_index);
                queue.enqueue((short)pvs_portal_index);
            }
            if (!use_pvs)
                build_working_portal_stack_traverse_portals(
                    (short)projection_index, &working_portals, &cluster_portals, &queue, &transformed_portal_cache, projections);
            build_working_portal_stack_merge_working_portals(&cluster_portals, &working_portals);
            build_working_portal_stack_build_region(&cluster_portals, &working_portals, cluster_portal_map, projections, projection_index, region);
        }
    }
    vpt_qsort_4byte(region->cluster_indices, region->cluster_count,
        reinterpret_cast<bool (*)(long, long, const void *)>(compare_region_cluster_indices), region);
}

#if !defined(_WIN64) && !defined(__LP64__)
typedef char working_portal_polygon_offset[(offsetof(s_working_portal, polygon) == 0xc) ? 1 : -1];
typedef char visibility_projection_depth_offset[(offsetof(visibility_projection, far_distance) == 0x88) ? 1 : -1];
typedef char visibility_region_projections_offset[(offsetof(s_visibility_region, projections) == 0x10) ? 1 : -1];
typedef char visibility_region_clusters_offset[(offsetof(s_visibility_region, clusters) == 0xd32) ? 1 : -1];
typedef char visibility_region_cluster_indices_offset[(offsetof(s_visibility_region, cluster_indices) == 0x1a34) ? 1 : -1];
typedef char visibility_region_volumes_offset[(offsetof(s_visibility_region, volumes) == 0x1c40) ? 1 : -1];
typedef char transformed_portal_size[(sizeof(s_transformed_portal) == 0x28) ? 1 : -1];
typedef char transformed_portal_points_offset[(offsetof(s_transformed_portal, polygon) + offsetof(s_working_portal_polygon, points) == 0x24) ? 1 : -1];
typedef char structure_bsp_portal_flags_offset[(offsetof(cluster_portal, flags) == 0x18) ? 1 : -1];
typedef char visibility_cluster_size[(sizeof(visibility_cluster) == 26) ? 1 : -1];
#endif
