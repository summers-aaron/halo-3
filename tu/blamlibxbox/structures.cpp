// blamlibxbox/structures.obj: from the records 0x9bce8 0x9e700 0xa1150 0xb0b70 0xb0c08 0xb0c90 0x127c50 0x1571b0 0x229758 0x2299d0
#include "objects/object_reference_types.h"
#include "tag_files/tag_groups.h"
#include "math/real_math_types.h"
#include "math/real_math_globals.h"
#include "tag_files/tag_types.h"
#include <string.h>
#include <math.h>
#include <ppcintrinsics.h>
#include "structures/structures.h"
#include "structures/structure_seams.h"
#include "structures/cluster_bit_vectors.h"
#include "structures/structure_globals.h"
#include "structures/structure_seam_cluster_stack.h"
#include "multithreading/thread_tls.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "structures/structure_bsp_tag.h"
#include "structures/structure_cluster_portal.h"
#include "scenario/scenario.h"
#include "structures/structure_runtime_resources.h"
#include "math/real_math_functions.h"
#include <xtl.h>
#include <xboxmath.h>
#include "math/real_math.h"
#include "math/projection_math.h"
#include "memory/static_arrays.h"
#include "memory/datum_index.h"
#include "scenario/cluster_reference_helpers.h"

// @flags /TP /O2 /Oc
// @address 0xb0c08 ?element_exists@?$c_additive_hash@$0CAA@$0BAAA@$0MOL@$02@@QAAPAJJJG@Z
// @address 0xb0c90 ?add_element@?$c_additive_hash@$0CAA@$0BAAA@$0MOL@$02@@QAA_NJJGJ@Z

template <long bucket_count, long capacity, long first_multiplier, long second_multiplier>
long *c_additive_hash<bucket_count, capacity, first_multiplier, second_multiplier>::element_exists(long first_hash, long second_hash,
    unsigned short key)
{
    long *result = 0;
    short index = buckets[hash_function(first_hash, second_hash)];
    while (index != -1) {
        if (data[index].key == key) {
            result = &data[index].value;
            index = -1;
        } else {
            index = data[index].next;
        }
    }
    return result;
}

template <long bucket_count, long capacity, long first_multiplier, long second_multiplier>
bool c_additive_hash<bucket_count, capacity, first_multiplier, second_multiplier>::add_element(long first_hash, long second_hash,
    unsigned short key, long value)
{
    bool result = false;
    if (count < capacity) {
        short bucket = hash_function(first_hash, second_hash);
        short head = buckets[bucket];
        long index = count;
        count++;
        buckets[bucket] = (short)index;
        data[index].key = key;
        data[index].value = value;
        data[index].next = head;
        result = true;
    }
    return result;
}


// match: 0x9bce8: calling the out-of-line canonical emits bl plus fpr/gpr save-restore instead of the inlined arithmetic.
static __forceinline bool structures_sphere_intersects_cone3d(const real_point3d *point, float radius,
    const real_point3d *cone_point, const real_vector3d *direction, float height, float sine, float cosine)
{
    real_vector3d vector;
    vector.i = point->x - cone_point->x;
    vector.j = point->y - cone_point->y;
    vector.k = point->z - cone_point->z;
    float distance_along = direction->i * vector.i + (direction->k * vector.k + direction->j * vector.j);
    if (distance_along < -radius || distance_along > radius + height)
        return false;
    if ((radius * sine * 2.0f + distance_along) * distance_along + radius * radius >=
        (vector.i * vector.i + (vector.k * vector.k + vector.j * vector.j)) * cosine * cosine)
        return true;
    return false;
}

static __forceinline s_structure_globals *get_structure_globals()
{
    int current_thread = get_current_thread_index();
    return &thread_structure_globals[current_thread - 1];
}


static __forceinline s_structure_globals *structures_get_globals()
{
    long current_thread = get_current_thread_index();
    s_structure_globals *globals = thread_structure_globals + current_thread;
    return globals - 1;
}
static __forceinline void structures_cluster_marker_begin()
{
    s_structure_globals *globals = structures_get_globals();
    globals->cluster_marker_sequence++;
    globals->cluster_marker_initialized = true;
}
static __forceinline void structures_cluster_marker_end()
{
    structures_get_globals()->cluster_marker_initialized = false;
}
static __forceinline bool structures_cluster_unmarked(s_cluster_reference reference)
{
    s_structure_globals *globals = structures_get_globals();
    return globals->cluster_markers[reference.bsp_index][reference.cluster_index] != globals->cluster_marker_sequence;
}
static __forceinline bool structures_cluster_mark(s_cluster_reference reference)
{
    s_structure_globals *globals = structures_get_globals();
    bool result = globals->cluster_markers[reference.bsp_index][reference.cluster_index] != globals->cluster_marker_sequence;
    globals->cluster_markers[reference.bsp_index][reference.cluster_index] = globals->cluster_marker_sequence;
    return result;
}
static __forceinline structure_bsp *structures_bsp_get(long index)
{
    scenario *current_scenario = (scenario *)global_scenario;
    return TAG_GET('sbsp', structure_bsp,
        ((const scenario_structure_bsp_reference *)current_scenario->structure_bsps.address)[index].structure_bsp.index);
}

// @address 0x9bce8 ?structure_clusters_in_cone@@YAJUs_cluster_reference@@PBTreal_point3d@@PBTreal_vector3d@@MMMJPAU1@PAJ@Z
long structure_clusters_in_cone(s_cluster_reference reference, const real_point3d *point, const real_vector3d *direction,
    float height, float sine, float cosine, long maximum_count, s_cluster_reference *references, long *total_count)
{
    // match: 0x9bce8: value range assumption guides branch lowering
    __assume(maximum_count == 256);
    // match: 0x9bce8: value range assumption guides branch lowering
    __assume(total_count == 0);
    long count = 0;
    structures_cluster_marker_begin();
    c_static_stack<s_cluster_reference, 256> pending;
    s_cluster_stack connected;
    connected.m_count = 0;
    structure_seams_connected_cluster_references_get(global_scenario_index, reference, true, &connected);
    for (long i = 0; i < connected.m_count; ++i) {
        s_cluster_reference connected_reference = connected.m_elements[i];
        structures_cluster_mark(connected_reference);
        pending.push_back(connected_reference);
    }
    while (!pending.empty()) {
        s_cluster_reference current = *pending.get_top();
        structure_bsp *bsp = structures_bsp_get(current.bsp_index);
        structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[current.cluster_index];
        pending.pop();
        if (count < maximum_count)
            references[count] = current;
        ++count;
        for (long i = 0; i < cluster->portal_indices.count; ++i) {
            long portal_index = *TAG_BLOCK_GET_ELEMENT(&cluster->portal_indices, i, short);
            cluster_portal *portal = &((cluster_portal *)bsp->cluster_portals.address)[portal_index];
            s_cluster_reference adjacent;
            adjacent.bsp_index = current.bsp_index;
            adjacent.cluster_index = (unsigned char)(portal->cluster_indices[0] == current.cluster_index ? portal->cluster_indices[1]
                                                                                                         : portal->cluster_indices[0]);
            if (structures_cluster_unmarked(adjacent) &&
                structures_sphere_intersects_cone3d(&portal->centroid, portal->bounding_radius, point, direction, height, sine,
                    cosine))
            {
                s_cluster_stack connected;
                connected.m_count = 0;
                structure_seams_connected_cluster_references_get(global_scenario_index, adjacent, true, &connected);
                for (long j = 0; j < connected.m_count; ++j) {
                    s_cluster_reference connected_reference = connected.m_elements[j];
                    if (structures_cluster_unmarked(connected_reference)) {
                        structures_cluster_mark(connected_reference);
                        pending.push_back(connected_reference);
                    }
                }
            }
        }
    }
    structures_cluster_marker_end();
    if (total_count)
        *total_count = count;
    return count > maximum_count ? maximum_count : count;
}

void structure_instanced_geometry_marker_begin()
{
    s_structure_globals *structure_globals = get_structure_globals();
    structure_globals->instanced_geometry_marker_sequence++;
    structure_globals->instanced_geometry_marker_initialized = true;
    structure_globals->marker.clear();
}

bool structure_instanced_geometry_mark(long bsp_index, long instanced_geometry_index)
{
    s_structure_globals *structure_globals = get_structure_globals();
    long key = (bsp_index << 12) | instanced_geometry_index;
    if (!structure_globals->marker.element_exists(instanced_geometry_index, bsp_index, (unsigned short)key)) {
        bool result = structure_globals->marker.add_element(instanced_geometry_index, bsp_index, (unsigned short)key, -1);
        return result;
    } else {
        return false;
    }
}

// @address 0x1571b0 structure_instanced_geometry_mark_get_user_data
unsigned char structure_instanced_geometry_mark_get_user_data(int target_bsp_index, int target_instanced_geometry_index,
    int *user_data)
{
    bool result = false;
    c_additive_hash<512, 4096, 3307, 3> *hash =
        &(thread_structure_globals - 1)[get_current_thread_index()].marker;
    long *r = hash->element_exists(
        target_instanced_geometry_index, target_bsp_index,
            (unsigned short)((target_bsp_index << 12) | target_instanced_geometry_index));
    if (r) {
        result = true;
        *user_data = *r;
    }
    return result;
}

template long *c_additive_hash<512, 4096, 3307, 3>::element_exists(long, long, unsigned short);
template bool c_additive_hash<512, 4096, 3307, 3>::add_element(long, long, unsigned short, long);


// @address 0x127c50 ?structure_distance_to_portal@@YAMPBUstructure_bsp@@PBUcluster_portal@@PBTreal_point3d@@@Z
float structure_distance_to_portal(const structure_bsp *bsp, const cluster_portal *portal, const real_point3d *point)
{
    const s_structure_bsp_resource *resource = &bsp->resource;
    const c_structure_bsp_resource_interface *resource_interface = &resource->collision_resource_interface;
    const s_structure_bsp_resources *resources = resource->use_resource_items
        ? resource_interface->get_resources() : (s_structure_bsp_resources *)resource->raw_resources.address;
    const real_plane3d *plane = &((real_plane3d *)resources->collision_bsp->planes.address)[portal->plane_index];
    real_vector3d to_point;
    vector_from_points3d(&portal->centroid, point, &to_point);
    real_vector3d perpendicular;
    real_vector3d parallel;
    component_vectors_from_normal3d(&to_point, &plane->n, &parallel, &perpendicular);
    float distance_along_normal = dot_product3d(&parallel, &plane->n);
    float perpendicular_distance_squared = magnitude_squared3d(&perpendicular);
    if (perpendicular_distance_squared <= portal->bounding_radius * portal->bounding_radius)
        return (float)fabs((double)distance_along_normal);
    float edge_distance = (float)sqrt((double)perpendicular_distance_squared) - portal->bounding_radius;
    return (float)sqrt((double)(distance_along_normal * distance_along_normal + edge_distance * edge_distance));
}


static __forceinline long structures_first_active_bsp()
{
    return 31 - _CountLeadingZeros(-g_active_structure_bsp_mask & g_active_structure_bsp_mask);
}
static __forceinline long structures_next_active_bsp(long index)
{
    unsigned long mask = g_active_structure_bsp_mask & ~(unsigned long)((1i64 << (index + 1)) - 1);
    return 31 - _CountLeadingZeros(-mask & mask);
}
static __forceinline structure_bsp *structures_active_bsp_get(long index)
{
    const scenario *current_scenario = (const scenario *)global_scenario;
    return TAG_GET('sbsp', structure_bsp,
        ((const scenario_structure_bsp_reference *)current_scenario->structure_bsps.address)[index].structure_bsp.index);
}

static __forceinline bool structures_convex_hull2d_test_circle(short count, const real_point2d *points, const real_point2d *center,
    float radius)
{
    bool result = true;
    float radius_squared = radius * radius;
    real_vector2d to_center, edge;
    float cross;
    for (short i = 0; i < count; i++)
    {
        const real_point2d *a = points + i;
        const real_point2d *b = points + ((i + 1 < count) ? (i + 1) : 0);
        vector_from_points2d(a, b, &edge);
        vector_from_points2d(a, center, &to_center);
        float edge_length_squared = magnitude_squared2d(&edge);
        if (edge_length_squared != 0.0f)
        {
            cross = cross_product2d(&to_center, &edge);
            if (cross > 0.0f && cross * cross > radius_squared * edge_length_squared)
            {
                result = false;
                break;
            }
        }
    }
    return result;
}

// @address 0x229758 sphere_intersects_cluster_portal
extern "C" bool sphere_intersects_cluster_portal(const cluster_portal * portal,
    const collision_bsp * bsp3d, const real_point3d * point, float radius)
{
    bool result = false;
    const real_plane3d *plane = &((real_plane3d *)bsp3d->planes.address)[portal->plane_index];
    float distance = point->x * plane->n.i + point->y * plane->n.j + point->z * plane->n.k - plane->d;
    if (fabsf(distance) < radius)
    {
        float sum = portal->bounding_radius + radius;
        if (distance_squared3d(point, &portal->centroid) < sum * sum)
        {
            short axis = projection_from_vector3d(&plane->n);
            bool sign = projection_sign_from_vector3d(&plane->n, axis);
            real_point3d projected3d;
            point_from_line3d(point, &plane->n, -distance, &projected3d);
            real_point2d center2d;
            project_point3d(&projected3d, axis, sign, &center2d);
            real_point2d points2d[128];
            short i;
            for (i = 0; i < portal->vertices.count(); i++)
                project_point3d(&portal->vertices[i], axis, sign, &points2d[i]);
            float circle_radius = sqrtf(radius * radius - distance * distance);
            if (structures_convex_hull2d_test_circle((short)portal->vertices.count(), points2d, &center2d, circle_radius))
                result = true;
        }
    }
    return result;
}


static __forceinline const collision_bsp *structures_bsp_collision_get(const structure_bsp *bsp)
{
    const s_structure_bsp_resource *resource = &bsp->resource;
    const c_structure_bsp_resource_interface *resource_interface = &resource->collision_resource_interface;
    const s_structure_bsp_resources *resources = resource->use_resource_items
        ? resource_interface->get_resources() : (s_structure_bsp_resources *)resource->raw_resources.address;
    return resources->collision_bsp;
}
// @address 0xa1150 ?structure_clusters_in_sphere@@YAJUs_cluster_reference@@PBTreal_point3d@@MJPAU1@PAJ@Z
long structure_clusters_in_sphere(s_cluster_reference cluster_reference, const real_point3d *sphere_center, float sphere_radius,
    long maximum_cluster_count, s_cluster_reference *cluster_references, long *cluster_count)
{
    s_cluster_reference reference = cluster_reference;
    const real_point3d *center = sphere_center;
    float radius = sphere_radius;
    long maximum_count = maximum_cluster_count;
    s_cluster_reference *references = cluster_references;
    long *total_count = cluster_count;
    long count = 0;
    if (reference.bsp_index != -1) {
        s_cluster_stack connected;
        connected.m_count = 0;
        long scenario_index = global_scenario_index;
        structure_seams_connected_cluster_references_get(scenario_index, reference, true, &connected);
        if (radius > 0.0f) {
            structures_cluster_marker_begin();
            structure_bsp *bsp;
            const collision_bsp *bsp3d;
            c_static_stack<s_cluster_reference, 4096> pending;
            long bsp_index = -1;
            for (long i = 0; i < connected.m_count; ++i) {
                s_cluster_reference connected_reference = connected.m_elements[i];
                structures_cluster_mark(connected_reference);
                pending.push_back(connected_reference);
            }
            while (!pending.empty()) {
                s_cluster_reference current = *pending.get_top();
                pending.pop();
                if (current.bsp_index != bsp_index) {
                    bsp_index = current.bsp_index;
                    bsp = structures_bsp_get(bsp_index);
                    bsp3d = structures_bsp_collision_get(bsp);
                }
                structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[current.cluster_index];
                if (count < maximum_count)
                    references[count] = current;
                ++count;
                if (cluster->seam_indices.count > 0) {
                    connected.m_count = 0;
                    if (structure_seams_connected_cluster_references_get(scenario_index, current, false, &connected)) {
                        for (long i = 0; i < connected.m_count; ++i) {
                            s_cluster_reference connected_reference = connected.m_elements[i];
                            if (structures_cluster_unmarked(connected_reference)) {
                                structures_cluster_mark(connected_reference);
                                pending.push_back(connected_reference);
                            }
                        }
                    }
                }
                for (long i = 0; i < cluster->portal_indices.count; ++i) {
                    long portal_index = *TAG_BLOCK_GET_ELEMENT(&cluster->portal_indices, i, short);
                    cluster_portal *portal = &((cluster_portal *)bsp->cluster_portals.address)[portal_index];
                    s_cluster_reference adjacent;
                    cluster_reference_set(&adjacent, current.bsp_index,
                        portal->cluster_indices[0] == current.cluster_index ? portal->cluster_indices[1] : portal->cluster_indices[0]);
                    if (structures_cluster_unmarked(adjacent) &&
                        sphere_intersects_cluster_portal((const cluster_portal *)portal, bsp3d, center, radius))
                    {
                        structures_cluster_mark(adjacent);
                        pending.push_back(adjacent);
                    }
                }
            }
            structures_cluster_marker_end();
        } else {
            for (long i = 0; i < connected.m_count && i < maximum_count; ++i) {
                references[i] = connected.m_elements[i];
                ++count;
            }
        }
    }
    if (total_count)
        *total_count = count;
    return count > maximum_count ? maximum_count : count;
}
// @address 0x2299d0 ?structure_clusters_in_bitvector@@YAJPBUs_game_cluster_bit_vectors@@JPAUs_cluster_reference@@PAJ@Z
long structure_clusters_in_bitvector(const s_game_cluster_bit_vectors *vectors, long maximum_count, s_cluster_reference *references,
    long *total_count)
{
    // match: 0x2299d0: value range assumption guides branch lowering
    __assume(maximum_count == 256);
    long count = 0;
    *total_count = 0;
    for (long bsp_index = structures_first_active_bsp(); bsp_index != -1; bsp_index = structures_next_active_bsp(bsp_index)) {
        structure_bsp *bsp = structures_active_bsp_get(bsp_index);
        for (long cluster_index = 0; cluster_index < bsp->clusters.count; ++cluster_index) {
            if (vectors->bit_vectors[bsp_index].test(cluster_index)) {
                if (count < maximum_count) {
                    s_cluster_reference *reference = &references[count];
                    cluster_reference_set(reference, bsp_index, cluster_index);
                    ++count;
                }
                ++*total_count;
            }
        }
    }
    return count;
}

s_structure_globals thread_structure_globals[2];


// @address 0x5bec98 ?structures_initialize@@YAXXZ
void structures_initialize(void)
{
}

// @address 0x5bec98 ?structures_dispose@@YAXXZ
void structures_dispose(void)
{
}

// @address 0x5bec98 ?structures_initialize_for_new_map@@YAXXZ
void structures_initialize_for_new_map(void)
{
}

// @address 0x5bec98 ?structures_dispose_from_old_map@@YAXXZ
void structures_dispose_from_old_map(void)
{
}

// @address 0x5bec98 ?structures_initialize_for_new_structure_bsp@@YAXK@Z
void structures_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask)
{
}

// @address 0x5bec98 ?structures_dispose_from_old_structure_bsp@@YAXK@Z
void structures_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask)
{
}
