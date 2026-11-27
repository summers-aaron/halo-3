// ailibxbox/sector_geometry.obj: from the records 0xb1f00 0xc99d8 0xc9b58 0xc9c20 0xced68 0x11d918 0x51b5c0 0x51b658 0x51b830 0x51b8e8 0xc94a0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "physics/collision_test_flags.h"
#include "objects/object_reference_types.h"
#include "structures/structure_runtime_resources.h"
#include "ai/path_types.h"
#include "physics/collision_types.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include <math.h>
#include "memory/data.h"
#include "ai/sector_ref.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/cluster_reference_helpers.h"
#include "cubemaps/cubemap_sample.h"
#include "ai/ai_hint_definitions.h"
#include "ai/ai_reference_frame.h"
#include "ai/sector.h"
#include "ai/sector_environment_objects.h"
#include "ai/sector_surface_bsp.h"
#include "ai/path_obstacle_types.h"
#include "math/matrix_math_types.h"
#include "physics/collision_models.h"
#include "physics/collision_bsp_types.h"
#include "physics/collision_model_types.h"
#include "physics/collision_model_definition_types.h"
#include "structures/structure_bsp_tag.h"
#include "structures/structure_instanced_geometry.h"
#include "objects/object_datum.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "cache/cache_files.h"
#include "ai/pathfinding_sector_types.h"
#include "ai/pathfinding_geometry.h"
#include "ai/pathfinding_data.h"
#include "ai/pathfinding_collision.h"
#include "ai/sector_geometry.h"
#include "math/real_math_globals.h"
#include "math/real_math_functions.h"
#include "math/real_math.h"
#include "ai/pathfinding_object_reference.h"
#include "structures/structure_instanced_geometry_definition.h"
#include "ai/pathfinding_pill_result.h"
#include "ai/ai_point3d.h"
#include "physics/collision_vector_query.h"
#include "render/render_objects_globals.h"
#include "math/projection_math.h"
#include "objects/object_header_access.h"
// @address 0x11d918 ?pathfinding_test_line@@YA_NPBTreal_point3d@@Vc_sector_ref@@01PBUpath_error_info@@PAUpath_collision_result@@@Z
// @address 0x51b5c0 ?pill2d_verify_collision_helper@@YA_NPBTreal_point3d@@Vc_sector_ref@@01PBUpath_error_info@@@Z
// @address 0x51b8e8 ?giant_sector_from_point@@YA?AVc_sector_ref@@PBTreal_point3d@@@Z
// @address 0x9e680 ?pathfinding_data_get@@YAPAUpathfinding_data@@F@Z
// @address 0xb2420 ?recursive_traverse_sector_bsp2d_node@@YAJPBUpathfinding_data@@JPBTreal_point2d@@@Z
// @address 0x9e788 ?scenario_cluster_reference_from_leaf_index@@YA?AUs_cluster_reference@@JJ@Z
// @address 0x9bb88 ?scenario_leaf_index_from_point@@YAJPBTreal_point3d@@PAJ@Z
// @address 0xb36b8 ?valid@c_sector_ref@@QBA_NXZ


struct obstacles_test_pill_result
{
    float distance;
    short disc_index;
    short obstacle_index;
};

static __forceinline s_cluster_reference sector_geometry_cluster_reference_from_point(const real_point3d *point)
{
    s_cluster_reference reference;
    long structure_bsp_index;
    long leaf_index = scenario_leaf_index_from_point(point, &structure_bsp_index);
    if (leaf_index != NONE)
    {
        reference = scenario_cluster_reference_from_leaf_index(structure_bsp_index, leaf_index);
    }
    else
    {
        cluster_reference_invalidate(&reference);
    }
    return reference;
}

static __forceinline const obstacle_disc *sector_geometry_obstacles_get_disc(const obstacles *list, short index)
{
    return &list->discs[index];
}

static __forceinline bool sector_geometry_disc_test_ray(const real_point2d *center, float radius, const real_point2d *point,
    const real_vector2d *vector, float *distance, float *distance_squared)
{
    bool result = false;
    real_vector2d offset;
    vector_from_points2d(point, center, &offset);
    float along = dot_product2d(&offset, vector);
    if (along > 0.0f)
    {
        float offset_squared = magnitude_squared2d(&offset);
        float outside = offset_squared - radius * radius;
        *distance_squared = offset_squared;
        if (outside <= 0.0f)
        {
            *distance = 0.0f;
            result = true;
        }
        else
        {
            float discriminant = along * along - outside;
            if (discriminant >= 0.0f)
            {
                *distance = along - sqrtf(discriminant);
                result = true;
            }
        }
    }
    return result;
}

// @address 0xc99d8 ?obstacles_test_pill@@YA_NPBUobstacles@@FPBTreal_point2d@@PBTreal_vector2d@@MM_NPAUobstacles_test_pill_result@@@Z
bool obstacles_test_pill(const obstacles *list, short ignore_index, const real_point2d *point, const real_vector2d *vector, float radius,
    float max_distance, bool skip_flagged, obstacles_test_pill_result *result)
{
    long count = list->disc_count;
    result->distance = max_distance;
    result->disc_index = NONE;
    result->obstacle_index = NONE;
    float best_distance_squared = 3.402823466e+38f;
    float distance;
    float distance_squared;
    for (short disc_index = 0; disc_index < count; disc_index++)
    {
        if (disc_index != ignore_index)
        {
            // match: 0xc99d8: pointer __restrict qualifier prevents alias reload
            const obstacle_disc *__restrict disc = sector_geometry_obstacles_get_disc(list, disc_index);
            if ((!skip_flagged || (disc->flags & 1) == 0) &&
                sector_geometry_disc_test_ray(&disc->center, disc->radius + radius, point, vector, &distance, &distance_squared))
            {
                bool closer = false;
                if (result->distance > distance)
                    closer = true;
                else if (result->distance == distance && distance_squared < best_distance_squared)
                    closer = true;
                if (closer)
                {
                    result->obstacle_index = disc->obstacle_index;
                    result->distance = distance;
                    result->disc_index = disc_index;
                    best_distance_squared = distance_squared;
                }
            }
        }
    }
    return result->disc_index + 1 != 0;
}

// @address 0xc9b58 pathfinding_test_vector
bool pathfinding_test_vector(const real_point3d *point, c_sector_ref sector_ref, c_sector_ref sector_ref2, const real_vector3d *vector,
    float length, const path_error_info *error_info, path_collision_result *result)
{
    unsigned long flags = 0;
    if (sector_ref.valid())
    {
        pathfinding_sector *start_sector = pathfinding_get_sector(sector_ref);
        flags = start_sector->flags & 0x3f0;
        if (sector_ref2.valid())
        {
            pathfinding_sector *end_sector = pathfinding_get_sector(sector_ref2);
            flags |= end_sector->flags & 0x3f0;
            if (end_sector->flags & 0x1000)
                flags |= 16;
        }
    }
    return pathfinding_test_vector_with_flags(point, sector_ref, sector_ref2, vector, length, flags, error_info, result);
}

// @address 0x11d918 ?pathfinding_test_line@@YA_NPBTreal_point3d@@Vc_sector_ref@@01PBUpath_error_info@@PAUpath_collision_result@@@Z
bool pathfinding_test_line(const real_point3d *point, c_sector_ref sector_ref, const real_point3d *point2, c_sector_ref sector_ref2,
    const path_error_info *error_info, path_collision_result *result)
{
    real_vector3d vector;
    vector_from_points3d(point, point2, &vector);
    float length = normalize3d(&vector);
    return pathfinding_test_vector(point, sector_ref, sector_ref2, &vector, length, error_info, result);
}

bool pill2d_verify_collision_helper(const real_point3d *point, c_sector_ref sector_ref, const real_point3d *point2,
    c_sector_ref sector_ref2, const path_error_info *error_info)
{
    real_vector3d vector;
    vector_from_points3d(point, point2, &vector);
    float length = normalize3d(&vector);
    if (length > 0.0f)
    {
        path_collision_result collision;
        return pathfinding_test_vector(point, sector_ref, sector_ref2, &vector, length, error_info, &collision);
    }
    return false;
}

// @address 0x51b830 ?sector_from_point_and_sector@@YA_NPBTreal_point3d@@Vc_sector_ref@@0PAV2@@Z
bool sector_from_point_and_sector(const real_point3d *point, c_sector_ref sector_ref, const real_point3d *point2, c_sector_ref *out_sector_ref)
{
    real_vector3d vector;
    vector_from_points3d(point, point2, &vector);
    vector.k = 0.0f;
    bool result;
    float length = normalize3d(&vector);
    if (length > 0.0f)
    {
        path_collision_result collision;
        result = !pathfinding_test_vector(point, sector_ref, g_sector_none, &vector, length, 0, &collision);
        if (result)
            *out_sector_ref = collision.sector;
    }
    else
    {
        result = true;
        *out_sector_ref = sector_ref;
    }
    return result;
}

c_sector_ref giant_sector_from_point(const real_point3d *point)
{
    real_point3d raised;
    raised.x = point->x;
    raised.y = point->y;
    raised.z = point->z + 0.2f;

    c_sector_ref sector;
    sector.clear();

    s_cluster_reference cluster = sector_geometry_cluster_reference_from_point(&raised);
    if (cluster_reference_valid(&cluster))
    {
        pathfinding_data *data = pathfinding_data_get(cluster.bsp_index);
        if (data->bsp2d_roots.count > 0)
        {
            long root = *(long *)data->bsp2d_roots.address;
            if (root != NONE)
            {
                long sector_index = recursive_traverse_sector_bsp2d_node(data, root, &point->xy);
                sector.set(cluster.bsp_index, (unsigned short)sector_index);
            }
        }
    }
    return sector;
}
static __forceinline s_structure_bsp_resources *sector_geometry_surface_resources(const s_structure_bsp_resource *data)
{
    return data->use_resource_items ? data->collision_resource_interface.get_resources() : (s_structure_bsp_resources *)data->raw_resources.address;
}
static __forceinline real_matrix4x3 *sector_geometry_surface_node_matrices(long object_index, long *count)
{
    object_datum *object = datum_get<s_object_header>(tls_object_header_data(), object_index)[0].datum;
    *count = object->node_matrices_size / sizeof(real_matrix4x3);
    return (real_matrix4x3 *)((char *)object + object->node_matrices_offset);
}
// @address 0xb1f00 ?collision_surface_get_sector@@YA?AVc_sector_ref@@FJJJKPBTreal_point3d@@@Z
c_sector_ref collision_surface_get_sector(short bsp, long surface, long instance_index, long object_index, unsigned long flags, const real_point3d *point)
{
    long sector_index = -1;
    long type;
    if (object_index != -1) {
        type = 4;
        if (bsp == -1) bsp = datum_get<s_object_header>(tls_object_header_data(), object_index)[0].cluster_reference.bsp_index;
    } else { if (instance_index != -1) type = 3; else type = 1; }
    if (bsp != -1) {
        pathfinding_data *data = pathfinding_data_get(bsp);
        if (data) {
            pathfinding_data *view = data;
            switch (type) {
            case 1: {
#ifdef HALO3_64BIT
                const collision_bsp *bsp_data = ((const TAG_POINTER(const collision_bsp) *)(&g_collision_bsp_cache))[get_current_thread_index() * 64 + bsp];
#else
                const collision_bsp *bsp_data = ((const collision_bsp **)(&g_collision_bsp_cache))[get_current_thread_index() * 64 + bsp];
#endif
                if (bsp_data && surface != -1) sector_index = sector_bsp2d_traverse(data, bsp_data, surface, &data->refs, point, 0);
            break; }
            case 3: {
                if (instance_index >= 0 && instance_index < view->instanced_geometry_refs.count) {
                    short index = ((short *)view->instanced_geometry_refs.address)[instance_index * 2];
                    if (index >= 0 && index < view->object_refs.count) {
                        pathfinding_object_bsp_reference *entry = ((pathfinding_object_reference *)view->object_refs.address)[index].entries;
                        scenario_structure_bsp_reference *reference = &((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[bsp];
                        structure_bsp *definition = TAG_GET('sbsp', structure_bsp, reference->structure_bsp.index);
                        if (instance_index < definition->instanced_geometry_instances.count) {
                            structure_instanced_geometry_instance *instance =
                                &((structure_instanced_geometry_instance *)definition->instanced_geometry_instances.address)[instance_index];
                            s_structure_bsp_resources *resources = sector_geometry_surface_resources(&definition->resource);
                            const collision_bsp *bsp_data =
                                &((structure_instanced_geometry_definition *)resources->instanced_geometry_definitions.address)[instance->definition_index].bsp;
                            sector_index = sector_bsp2d_traverse(data, bsp_data, surface, &entry->nodes, point, &instance->transform);
                        }
                    }
                }
            break; }
            case 4: {
                short index = pathfinding_object_get_object_ref_index(object_index, bsp);
                if (index >= 0 && index < view->object_refs.count) {
                    pathfinding_object_reference *ref = &((pathfinding_object_reference *)view->object_refs.address)[index];
                    for (short i = 0; i < ref->count; ++i) {
                        pathfinding_object_bsp_reference *entry = &ref->entries[i];
                        if (entry->bsp_index == flags) {
                            collision_model_instance model;
                            if (collision_model_instance_new(&model, object_index)) {
                                short node = (short)(flags & 255);
                                long node_count;
                                real_matrix4x3 *matrices = sector_geometry_surface_node_matrices(object_index, &node_count);
                                if (node >= 0 && node < node_count) {
                                    s_collision_model_region *region =
                                        &((s_collision_model_region *)model.collision_model->regions.address)[(flags >> 24) & 255];
                                    s_collision_model_permutation *permutation =
                                        &((s_collision_model_permutation *)region->permutations.address)[(flags >> 16) & 255];
                                    s_collision_model_bsp *entries = (s_collision_model_bsp *)permutation->bsps.address;
                                    const collision_bsp *bsp_data =
                                &entries[(flags >> 8) & 255].bsp;
                                    if (bsp_data) sector_index = sector_bsp2d_traverse(data, bsp_data, surface, &entry->nodes, point, &matrices[node]);
                                }
                            }
                            break;
                        }
                    }
                }
            break; }
            }
        }
    }
    c_sector_ref result;
    result.set(bsp, sector_index);
    return result;
}

// @address 0x51b658 pathfinding_find_ground_sector
extern "C" c_sector_ref pathfinding_find_ground_sector(
    const real_point3d *point, const real_vector3d *up, c_ai_point3d *ai_point, collision_result *collision, float offset)
{
    c_sector_ref sector_ref;
    sector_ref.clear();
    real_point3d origin;
    real_vector3d vector;
    point_from_line3d(point, up, offset, &origin);
    scale_vector3d(up, -4.0f, &vector);
    if (collision_test_vector(k_biped_ground_collision_flags, false, &origin, &vector, NONE, NONE, NONE, collision))
    {
        sector_ref = collision_surface_get_sector((short)collision->structure_bsp_index, collision->surface_index,
            collision->instance_index, collision->object_index, collision->bsp_reference, &collision->point);
        if (ai_point)
        {
            if (collision->object_index == NONE)
            {
                ai_point->set(&collision->point, NONE, (short)collision->structure_bsp_index);
            }
            else
            {
                short reference_frame = ai_reference_frame_get_by_node_index(collision->object_index, collision->node_index);
                if (reference_frame == NONE)
                {
                    ai_point->set(&collision->point, NONE, (short)collision->structure_bsp_index);
                }
                else if (ai_reference_frame_inverse_transform_point(reference_frame, &collision->point, &ai_point->point))
                {
                    ai_point->location[0] = reference_frame;
                    ai_point->location[1] = (short)collision->structure_bsp_index;
                }
                else
                {
                    ai_point->set(&collision->point, NONE, (short)collision->structure_bsp_index);
                }
            }
        }
    }
    else if (ai_point)
    {
        ai_point->set(point, NONE, scenario_structure_index_from_point(&origin));
    }
    return sector_ref;
}


static __forceinline real_point2d *project_point2d(const real_point3d *point, short projection, bool sign, real_point2d *result)
{
    set_real_point2d(result,
        point->coordinates[global_projection3d_mappings[projection][sign][0]],
        point->coordinates[global_projection3d_mappings[projection][sign][1]]);
    return result;
}

static __forceinline real_plane2d *plane2d_from_point_and_normal(real_plane2d *plane, const real_point2d *point, const real_vector2d *normal)
{
    plane->normal = *normal;
    plane->d = point->x * plane->normal.i + point->y * plane->normal.j;
    return plane;
}

static __forceinline bool path_error_info_contains_link(const path_error_info *error_info, long link_index)
{
    bool found = false;
    for (short index = 0; index < error_info->b; index++)
    {
        if ((unsigned short)error_info->links[index].type == 0xffff && error_info->links[index].index == link_index && (error_info->a & (1 << index)))
        {
            found = true;
            break;
        }
    }
    return found;
}


// @address 0xc9c20 pathfinding_test_vector_with_flags
bool pathfinding_test_vector_with_flags(const real_point3d *point, c_sector_ref sector_ref, c_sector_ref sector_ref2,
    const real_vector3d *vector, float length, unsigned long flags, const path_error_info *error_info, path_collision_result *result)
{
    bool success = false;
    c_sector_ref current_sector = sector_ref;
    short iterations = 0;
    c_sector_ref previous_sector;
    previous_sector.clear();
    result->collision = false;
    result->sector.clear();
    result->link_index = 0xffff;
    result->t = length;
    result->entered_non_floor_sector = false;
    result->non_floor_sector_t = 3.402823466e+38f;
    float previous_t = rasterizer_states_mode_5_slope_scale_bias;
    if (sector_ref != sector_ref2 && sector_ref.valid())
    {
        pathfinding_sector *sector = &((pathfinding_sector *)pathfinding_data_get(sector_ref.structure_bsp_index)
                ->sectors.address)[(short)sector_ref.sector_index];
        real_vector3d normal;
        if (sector_ref2.valid())
            get_sector_normal(&((pathfinding_sector *)pathfinding_data_get(sector_ref2.structure_bsp_index)
                                      ->sectors.address)[(short)sector_ref2.sector_index],
                &normal, true);
        else
            get_sector_normal(sector, &normal, true);
        short projection = projection_from_vector3d(&normal);
        bool sign = projection_sign_from_vector3d(&normal, projection);
        real_vector3d cross;
        cross.i = normal.j * vector->k - normal.k * vector->j;
        cross.j = normal.k * vector->i - normal.i * vector->k;
        cross.k = normal.i * vector->j - normal.j * vector->i;
        // j-first association
        if (sqrtf((cross.j * cross.j + cross.i * cross.i) + cross.k * cross.k) > 0.00001)
        {
            if (flags & 0x10)
                flags |= 0x1000;
            else if (sector->flags & 0x1000)
                flags |= 0x10;
            real_vector2d projected_vector;
            project_vector3d(vector, projection, sign, &projected_vector);
            real_vector2d projected_cross;
            project_vector3d(&cross, projection, sign, &projected_cross);
            real_point3d projected_point;
            project_point3d(point, projection, sign, &projected_point);
            length *= normalize2d(&projected_vector);
            real_plane2d plane;
            plane2d_from_point_and_normal(&plane, &projected_point.xy, &projected_cross);
            pathfinding_data *data = pathfinding_data_get(sector_ref.structure_bsp_index);
            c_sector_ref next_sector = sector_ref;
            real_point2d p1;
            real_point2d p2;
            float side1;
            float side2;
            while (next_sector != NONE)
            {
                bool first = true;
                bool advanced = false;
                pathfinding_data *current_data = data;
                previous_sector = current_sector;
                current_sector = next_sector;
                sector = &((pathfinding_sector *)current_data->sectors.address)[current_sector.sector_index];
                if (!(sector->flags & 0x10) && !result->entered_non_floor_sector)
                {
                    result->entered_non_floor_sector = true;
                    result->non_floor_sector_t = previous_t;
                }
                if (current_sector == sector_ref2)
                    break;
                next_sector.clear();
                long link_index = sector->first_link_index;
                for (long count = 1; count <= 2000; count++)
                {
                    if (link_index == NONE || link_index == 0xffff)
                        break;
                    sector_link *link = &((sector_link *)current_data->links.address)[link_index];
                    long current_link_index = link_index;
                    bool forward;
                    long vertex_index;
                    long next_link;
                    if (link->sectors[1] == current_sector.sector_index)
                    {
                        next_link = link->next_links[1];
                        forward = true;
                        vertex_index = link->vertices[0];
                        if (next_link == sector->first_link_index)
                            link_index = 0xffff;
                        else
                            link_index = next_link;
                    }
                    else
                    {
                        next_link = link->next_links[0];
                        forward = false;
                        vertex_index = link->vertices[1];
                        if (next_link == sector->first_link_index)
                            link_index = 0xffff;
                        else
                            link_index = next_link;
                    }
                    if (first)
                    {
                        project_point2d(&current_data->vertices[forward ? link->vertices[1] : link->vertices[0]],
                            projection, sign, &p1);
                        side1 = plane2d_distance_to_point(&plane, &p1);
                        first = false;
                    }
                    else
                    {
                        side1 = side2;
                        p1 = p2;
                    }
                    project_point2d(&current_data->vertices[vertex_index], projection, sign, &p2);
                    side2 = plane2d_distance_to_point(&plane, &p2);
                    if (!(side1 > 0.0f) && !(side2 < 0.0f))
                    {
                        sector_geometry_cross_link(current_data, link, current_sector, flags, current_link_index, &projected_point, &projected_vector, &plane,
                            &next_sector, &data, forward, &p1, &p2, result);
                        advanced = true;
                        point_from_line3d(point, vector, result->t, &result->point);
                        if (!result->collision && next_sector == previous_sector)
                        {
                            advanced = false;
                        }
                        else
                        {
                            if (result->t - previous_t < -0.1 && iterations > 0)
                            {
                                success = false;
                                goto miss;
                            }
                            previous_t = result->t;
                            if (result->t - length > 0.0001)
                            {
                                if (sector_ref2 == NONE || sector_ref2 == result->sector)
                                {
                                    result->collision = false;
                                    result->link_index = 0xffff;
                                    result->t = length;
                                    point_from_line3d(point, vector, length, &result->point);
                                    success = false;
                                }
                                else if (next_sector == sector_ref2 && result->t - length < 0.01)
                                {
                                    result->collision = false;
                                    result->sector = next_sector;
                                    result->link_index = 0xffff;
                                    result->t = length;
                                    point_from_line3d(point, vector, length, &result->point);
                                    success = false;
                                }
                                else
                                {
                                    result->collision = true;
                                    result->link_index = 0xffff;
                                    result->t = 0.0f;
                                    point_from_line3d(point, vector, length, &result->point);
                                    success = true;
                                }
                            }
                            else if (result->collision)
                            {
                                success = true;
                            }
                            else if ((error_info && (link->flags & 0x20) && path_error_info_contains_link(error_info, current_link_index)) ||
                                ((unsigned short)link->hint_index != 0xffff && !ai_link_hint_traversable(current_data, current_link_index)))
                            {
                                success = true;
                                result->collision = true;
                            }
                            else
                            {
                                if (iterations > 200)
                                {
                                    success = false;
                                    goto miss;
                                }
                                iterations++;
                                break;
                            }
                            goto done;
                        }
                    }
                    else
                    {
                        advanced = false;
                    }
                }
                if (!advanced)
                {
                    result->collision = true;
                    result->link_index = 0xffff;
                    result->sector = current_sector;
                    result->t = length;
                    point_from_line3d(point, vector, length, &result->point);
                    success = true;
                    goto done;
                }
            }
        }
    }
miss:
    result->collision = false;
    result->link_index = 0xffff;
    result->sector = current_sector;
    result->t = length;
    point_from_line3d(point, vector, length, &result->point);
done:
    return success;
}

// @address 0xced68 sector_geometry_cross_link
extern "C" inline void sector_geometry_cross_link(pathfinding_data *data, sector_link *link, c_sector_ref sector_ref,
    unsigned long flags, long link_index, const real_point3d *point, const real_vector2d *vector,
    // match: 0xced68: pointer __restrict qualifier prevents alias reload
    const void *plane, c_sector_ref *__restrict next_sector, pathfinding_data **__restrict next_data, bool forward,
    const real_point2d *p1, const real_point2d *p2, path_collision_result *result)
{
    *next_data = data;
    if (link->flags & 0x1000)
    {
        next_sector->clear();
        if (link->hint_index != NONE)
        {
            short bsp_index;
            long element;
            long sector_index;
            if (pathfinding_find_seam_link(sector_ref.structure_bsp_index, link->hint_index, &bsp_index, &element, &sector_index)
                && bsp_index != NONE && sector_index != NONE && sector_index != 0xffff)
            {
                data = pathfinding_data_get(bsp_index);
                *next_data = data;
                next_sector->set(bsp_index, (unsigned short)sector_index);
            }
        }
    }
    else
    {
        if (link->hint_index != NONE)
        {
            pathfinding_hint *hint = &((pathfinding_hint *)data->hints.address)[link->hint_index];
            if (hint->type == 0)
            {
                link = &((sector_link *)data->links.address)[hint->link_index];
                forward = link->sectors[1] == sector_ref.sector_index;
            }
        }
        next_sector->set(sector_ref.structure_bsp_index, link->sectors[!forward]);
    }
    if (next_sector->sector_index == NONE || next_sector->sector_index == 0xffff)
        result->collision = true;
    else
    {
        pathfinding_sector *sector = &((pathfinding_sector *)data->sectors.address)[(short)next_sector->sector_index];
        short sector_flags = sector->flags;
        result->collision = (!(sector_flags & 1) && !(sector_flags & 0x8000))
            || ((sector_flags & 2) && sector_broken(next_sector->structure_bsp_index, data, sector))
            || !(sector_flags & flags);
    }
    result->sector = sector_ref;
    result->link_index = link_index;
    real_vector2d edge;
    real_vector2d relative;
    vector_from_points2d(p2, p1, &edge);
    vector_from_points2d(p2, &point->xy, &relative);
    float denominator = cross_product2d(vector, &edge);
    float numerator = cross_product2d(&edge, &relative);
    if (denominator != 0.0f)
    {
        result->t = numerator / denominator;
        result->t = result->t > 0.0f ? result->t : 0.0f;
    }
    else
    {
        result->t = 0.0f;
        result->point = *point;
    }
}

// @address 0xc94a0 ?pathfinding_test_pill2d@@YA_N_NPBUobstacles@@FF0PBTreal_point3d@@Vc_sector_ref@@3PBTreal_vector3d@@MM000PBUpath_error_info@@PAUpathfinding_test_pill2d_result@@@Z
bool __cdecl pathfinding_test_pill2d(bool, const obstacles *list, short ignore_index, short projection_axis, bool projection_sign,
    const real_point3d *point, c_sector_ref sector, c_sector_ref sector2, const real_vector3d *vector, float radius, float max_distance,
    bool skip_rays, bool subtract_radius, bool skip_flagged, const path_error_info *error_info, pathfinding_test_pill2d_result *result)
{
    bool success = false;
    result->distance = max_distance;
    result->sector.clear();
    result->link_index = 0xffff;
    result->structure_bsp_index = NONE;
    result->disc_index = NONE;
    result->obstacle_index = NONE;
    result->left_side_blocked = false;
    result->right_side_blocked = false;
    float limit = subtract_radius ? max_distance - radius : max_distance;
    if (!skip_rays)
    {
        if (!sector.valid())
        {
            success = true;
        }
        else
        {
            c_sector_ref none;
            none.clear();
            pathfinding_sector *start_sector = pathfinding_get_sector(sector);
            real_vector3d normal;
            get_sector_normal(start_sector, &normal, true);
            unsigned long flags = start_sector->flags & 0x3f0;
            real_point3d end;
            point_from_line3d(point, vector, max_distance, &end);
            if (sector2.valid())
            {
                pathfinding_sector *end_sector = pathfinding_get_sector(sector2);
                flags |= end_sector->flags & 0x3f0;
                if (end_sector->flags & 0x1000)
                    flags |= 16;
            }
            path_collision_result first;
            if (pathfinding_test_vector_with_flags(point, sector, sector2, vector, max_distance, flags, error_info, &first))
            {
                if (first.t < limit || (sector2 != NONE && first.sector != sector2))
                {
                    success = true;
                    result->distance = first.t;
                    result->link_index = first.link_index;
                    result->structure_bsp_index = first.sector.structure_bsp_index;
                }
            }
            real_vector3d perp;
            cross_product3d(&normal, vector, &perp);
            path_collision_result perp_collision;
            pathfinding_test_vector(point, sector, none, &perp, radius, 0, &perp_collision);
            {
                path_collision_result side;
                if (pathfinding_test_vector_with_flags(&perp_collision.point, perp_collision.sector, none, vector, max_distance, flags, error_info, &side))
                {
                    float t = side.t;
                    if (!(t >= limit))
                    {
                        result->left_side_blocked = true;
                        if (!(t >= result->distance))
                        {
                            bool verify = true;
                            if (!first.collision)
                                verify = pill2d_verify_collision_helper(&side.point, side.sector, &end, sector2, error_info);
                            if (verify)
                            {
                                result->distance = t;
                                result->link_index = side.link_index;
                                result->structure_bsp_index = side.sector.structure_bsp_index;
                                success = true;
                            }
                        }
                    }
                }
            }
            scale_vector3d(&perp, -1.0f, &perp);
            pathfinding_test_vector(point, sector, none, &perp, radius, 0, &perp_collision);
            {
                path_collision_result side;
                if (pathfinding_test_vector_with_flags(&perp_collision.point, perp_collision.sector, none, vector, max_distance, flags, error_info, &side))
                {
                    float t = side.t;
                    if (!(t >= limit))
                    {
                        result->right_side_blocked = true;
                        if (!(t >= result->distance))
                        {
                            bool verify = true;
                            if (!first.collision)
                                verify = pill2d_verify_collision_helper(&side.point, side.sector, &end, sector2, error_info);
                            if (verify)
                            {
                                result->distance = t;
                                result->link_index = side.link_index;
                                result->structure_bsp_index = side.sector.structure_bsp_index;
                                success = true;
                            }
                        }
                    }
                }
            }
        }
    }
    if (list)
    {
        real_point2d proj_point;
        project_point2d(point, projection_axis, projection_sign, &proj_point);
        real_vector2d proj_vector;
        project_vector3d(vector, projection_axis, projection_sign, &proj_vector);
        obstacles_test_pill_result pill;
        if (obstacles_test_pill(list, ignore_index, &proj_point, &proj_vector, radius, max_distance, skip_flagged, &pill))
        {
            if (pill.distance < result->distance && pill.distance < limit)
            {
                result->distance = pill.distance;
                result->link_index = 0xffff;
                result->structure_bsp_index = NONE;
                result->disc_index = pill.disc_index;
                result->obstacle_index = pill.obstacle_index;
                success = true;
            }
        }
    }
    path_collision_result final_collision;
    pathfinding_test_vector(point, sector, sector2, vector, result->distance, error_info, &final_collision);
    result->sector = final_collision.sector;
    return success;
}

