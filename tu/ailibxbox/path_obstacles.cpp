// ailibxbox/path_obstacles.obj: from the records 0xc7ce0 0xc7d90 0xc9240 0xcf148 0xd0228 0xd0528 0xd0840 0xd0980 0x5059d8 0x50b200 0x569990
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "effects/effect_location_data.h"
#include "ai/path_state_types.h"
#include "memory/data_types.h"
#include "memory/tls_data_arrays.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include <string.h>
#include "math/real_math.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_types.h"
#include "objects/object_datum.h"
#include "ai/ai_point3d.h"
#include "ai/sector_ref.h"
#include "ai/actor_datum.h"
#include "ai/path_obstacle_types.h"
#include "math/matrix_math.h"
#include <math.h>
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "ai/path_obstacles.h"
#include "ai/ai_globals.h"
#include "ai/path_obstacle_avoidance.h"
#include "ai/sector.h"
#include "effects/effects.h"
#include "memory/data.h"
#include "ai/special_movement_types.h"
#include "math/real_math_types.h"
#include "objects/object_reference_types.h"
#include "units/biped_definitions.h"
#include "ai/obstacle_profile.h"
#include "cseries/crt_xdk.h"
#include "effects/effect_datum.h"
#include "effects/effect_definitions.h"
#include "effects/effect_data.h"
#include "effects/effect_location_types.h"
#include "ai/hint_ref.h"
#include "ai/obstacle_path_types.h"
#include "ai/path_debug_storage.h"
#include "ai/path_types.h"
#include "ai/pathfinding_sector_types.h"
#include "ai/pathfinding_data.h"
#include "ai/ai_reference_frame.h"
#include "math/projection_math.h"
// @address 0x569990 ?obstacles_disc_neighborhood@@YAXPBUobstacles@@MFPAV?$c_static_flags_no_init@$0EA@@@@Z
// @address 0xc7ce0 obstacle_profile_initialize
// @address 0xc7d90 ?obstacle_profile_set_actor@@YAXJPBTreal_point3d@@PBTreal_vector3d@@F_NPAUs_obstacle_profile@@@Z
// @address 0xc9240 ?obstacles_add_disc@@YA_NPAUobstacles@@JFPBTreal_point3d@@M@Z
// @address 0xd0840 ?obstacles_recompute@@YAXPAUobstacles@@M@Z
// @address 0xd0980 ?obstacles_disc_tangents@@YAXPBUobstacles@@FPBTreal_point2d@@MPATreal_vector2d@@2PAM@Z

struct path_obstacle_cluster {
    short count;
    short padding;
    struct entry {
        short weight;
        short padding;
        real_point3d position;
        float radius;
    } entries[32];
};

struct effect_location_iterator {
    const effect_datum *effect;
    long next_index;
    __forceinline const effect_location_datum *next() {
        long index = effect_location_find_next_of_type(effect, next_index, 3);
        if (index == -1)
            return 0;
        const effect_location_datum *location = &((effect_location_datum *)g_effect_location_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)];
        next_index = location->next_location_index;
        return location;
    }
};

// flood fill: every disc that overlaps one already reached (from the seed on) is marked in the bit vector
void obstacles_disc_neighborhood(const obstacles *list, float margin, short seed_index,
                                 c_static_flags_no_init<64> *flags)
{
    memset(flags, 0, ((list->disc_count + 31) >> 5) * 4);
    if (seed_index != -1)
    {
        short stack_top = 0;
        short stack[256];
        flags->set(seed_index, true);
        stack[stack_top++] = seed_index;
        while (stack_top > 0)
        {
            --stack_top;
            const obstacle_disc *current = &list->discs[stack[stack_top]];
            for (short i = seed_index + 1; i < list->disc_count; i++)
            {
                if (!flags->test(i))
                {
                    const obstacle_disc *candidate = &list->discs[i];
                    if (point_in_circle(&current->center, &candidate->center,
                                              (current->radius + margin) + (candidate->radius + margin)))
                    {
                        flags->set(i, true);
                        stack[stack_top++] = i;
                    }
                }
            }
        }
    }
}

// match: 0xc7ce0: __restrict on profile keeps the no-alias schedule; without it the prologue spills an extra GPR and the copy loads recolor
extern "C" void obstacle_profile_initialize(s_obstacle_profile * __restrict profile, const real_point3d *position, float radius,
                             long flags, profile_source *source, const real_vector3d *direction,
                             long index1, long index2)
{
    real_vector3d direction_copy = *direction;
    real_vector3d direction_copy2 = direction_copy;
    real_point3d position_copy = *position;
    profile->radius = radius;
    profile->flags = flags;
    profile->position = position_copy;
    profile->lower = -3.4028234663852886e+38f;
    profile->movement = (special_movement *)source;
    profile->upper = 3.4028234663852886e+38f;
    profile->biped = 0;
    profile->ignore_size = 0;
    profile->step_size = 0;
    profile->minimum_push_size = 0;
    profile->maximum_push_size = 0;
    profile->vault_allowed = true;
    profile->mount_allowed = true;
    profile->in_vehicle = false;
    profile->forward = direction_copy;
    profile->direction = direction_copy2;
    profile->ignore_object_index = index1;
    profile->ignore_object_index2 = index2;
}

void obstacle_profile_set_actor(long actor_index, union real_point3d const *position, union real_vector3d const *direction, short type,
    bool enabled, struct s_obstacle_profile *profile)
    {
        actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
        object_datum *object_data = (object_datum *)object_get_and_verify_type(actor->unit_index, NONE);
        if (object_data->type == 0)
        {
            profile->biped = (biped_datum *)object_data;
            if (object_data->parent == -1 && type == 2 && enabled)
            {
                biped_definition *unit_definition =
                    (biped_definition *)tag_get('bipd', object_data->definition);
                if (position)
                {
                    profile->lower =
                        profile->position.z > position->z ? position->z : profile->position.z;
                    profile->upper =
                        profile->position.z > position->z ? profile->position.z : position->z;
                    profile->upper += unit_definition->physics.height_standing;
                }
            }
        }
        if (direction)
        {
            float dot = dot_product3d(&profile->forward, direction);
            profile->direction = *direction;
            if (actor->flags & 2)
            {
                profile->vault_allowed = dot > 0.95f;
                profile->mount_allowed = dot > 0.8660254f;
            }
            else
            {
                profile->vault_allowed = true;
                profile->mount_allowed = true;
            }
        }
        profile->in_vehicle = actor->root_vehicle_index != -1;
        if (profile->movement)
        {
            if (profile->flags & 1)
                profile->ignore_size = profile->movement->obstacle_ignore_size;
            if (profile->flags & 4)
                profile->step_size = profile->movement->obstacle_smashable_size;
            if (profile->flags & 2)
            {
                profile->minimum_push_size = profile->movement->obstacle_leap_minimum_size;
                profile->maximum_push_size = profile->movement->obstacle_leap_maximum_size;
            }
        }
    }

// appends an entry unless the list is full
bool obstacles_add_disc(obstacles *list, long object_index, short flags, const real_point3d *point,
                        float radius)
{
    short count = list->disc_count;
    if (count == 256)
        return false;
    obstacle_disc *entry = &list->discs[count];
    list->disc_count = count + 1;
    if (flags & 1)
        list->flag1_count++;
    if (flags & 8)
        list->flag8_count++;
    entry->flags = flags;
    entry->object_index = object_index;
    entry->obstacle_index = -1;
    set_real_point2d(&entry->center, point->x, point->y);
    entry->radius = radius;
    return true;
}

// claims the spawn slots: frees every slot to -1, then for each still-free slot takes its neighborhood and tags it with the slot's new group id
void obstacles_recompute(struct obstacles * list, float margin)
{
    list->obstacle_count = 0;
    for (short i = 0; i < list->disc_count; i++)
        list->discs[i].obstacle_index = -1;

    for (short i = 0; i < list->disc_count; i++)
    {
        if ((unsigned short)list->discs[i].obstacle_index == 0xffff)
        {
            unsigned short old = list->obstacle_count;
            list->obstacle_count = old + 1;
            c_static_flags_no_init<256> mask;
            obstacles_disc_neighborhood(list, margin, i, (c_static_flags_no_init<64> *)&mask);
            for (short j = 0; j < list->disc_count; j++)
            {
                if (mask.test(j))
                    list->discs[j].obstacle_index = old;
            }
        }
    }
}

// tangent directions from a point to a disc; the tangent length is 0 when the point is on the disc centre


static inline float obstacle_disc_tangent_normalize2d(real_vector2d *v)
{
    float length = sqrtf(v->i * v->i + v->j * v->j);
    if (fabsf(length) >= k_real_epsilon)
    {
        float inverse = k_real_one / length;
        v->i *= inverse;
        v->j *= inverse;
        return length;
    }
    return k_real_zero;
}

static inline real_vector2d *obstacle_disc_tangent_rotate2d(const real_vector2d *v, float sine, float cosine, real_vector2d *result)
{
    result->i = cosine * v->i - sine * v->j;
    result->j = sine * v->i + cosine * v->j;
    return result;
}

static inline real_vector2d *obstacle_disc_tangent_set2d(real_vector2d *v, float i, float j)
{
    v->i = i;
    v->j = j;
    return v;
}

static void obstacle_disc_tangent_split(const real_vector2d *direction, float center_distance, float radius, real_vector2d *tangent0,
    real_vector2d *tangent1, float *tangent_length)
{
    float sine = 1.0f;
    if (center_distance > 0.0f)
    {
        sine = radius / center_distance;
        sine = sine > 1.0f ? 1.0f : sine;
    }
    float cosine = sqrtf(1.0f - sine * sine);
    obstacle_disc_tangent_rotate2d(direction, -sine, cosine, tangent0);
    obstacle_disc_tangent_rotate2d(direction, sine, cosine, tangent1);
    *tangent_length = center_distance * cosine;
}

void obstacles_disc_tangents(const obstacles *obs, short disc_index, const real_point2d *point, float radius, real_vector2d *tangent0,
    // match: 0xd0980: __restrict on distance keeps its store hoisted above the tangent stores; without it the store sinks and tangents recolor to fr12
    real_vector2d *tangent1, float *__restrict distance)
{
    const float k_tangent_epsilon = 0.00390625f;
    const obstacle_disc *disc = &obs->discs[disc_index];
    real_vector2d direction;
    vector_from_points2d(point, &disc->center, &direction);
    float length = obstacle_disc_tangent_normalize2d(&direction);
    if (length > 0.0f)
    {
        obstacle_disc_tangent_split(&direction, length, disc->radius + radius + k_tangent_epsilon, tangent0, tangent1, distance);
    }
    else
    {
        obstacle_disc_tangent_set2d(tangent0, 0.0f, -1.0f);
        obstacle_disc_tangent_set2d(tangent1, 0.0f, 1.0f);
        *distance = 0.0f;
    }
}

#define PATH_OBSTACLES_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define PATH_OBSTACLES_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define PATH_OBSTACLES_PIN(v, lo, hi) PATH_OBSTACLES_MIN(PATH_OBSTACLES_MAX(v, lo), hi)
static __forceinline real_vector3d *path_obstacles_cluster_vector(const real_point3d *a, const real_point3d *b, real_vector3d *v)
{
    v->i = b->x - a->x;
    v->j = b->y - a->y;
    v->k = b->z - a->z;
    return v;
}
static __forceinline float path_obstacles_cluster_distance(const real_point3d *a, const real_point3d *b)
{
    real_vector3d v;
    path_obstacles_cluster_vector(a, b, &v);
    return sqrtf(v.k * v.k + (v.j * v.j + v.i * v.i));
}
static const float k_path_obstacles_cluster_one = 1.0f;
static const float k_path_obstacles_cluster_ten = 10.0f;
static const float k_path_obstacles_cluster_two = 2.0f;
static const float k_path_obstacles_cluster_limit = 3.0f;
static const float k_path_obstacles_cluster_zero = 0.0f;
static __forceinline const float &path_obstacles_cluster_zero()
{
    return k_path_obstacles_cluster_zero;
}
static __forceinline float path_obstacles_cluster_score(const path_obstacle_cluster::entry *entry, const real_point3d *point)
{
    float distance = path_obstacles_cluster_distance(point, &entry->position);
    float score = path_obstacles_cluster_zero();
    if (distance < k_path_obstacles_cluster_one)
        score = k_path_obstacles_cluster_one;
    else if (distance < entry->radius)
        score = PATH_OBSTACLES_PIN((float)entry->weight / distance * k_path_obstacles_cluster_ten, 
            path_obstacles_cluster_zero(), k_path_obstacles_cluster_one);
    return score;
}
// @address 0x5059d8 obstacles_cluster_add_point
extern "C" void obstacles_cluster_add_point(path_obstacle_cluster *clusters, const real_point3d *point, float radius)
{
    short best = -1;
    float best_score = path_obstacles_cluster_zero();
    for (short i = 0; i < clusters->count; ++i) {
        float score = path_obstacles_cluster_score(&clusters->entries[i], point);
        if (score > best_score) {
            best = i;
            best_score = score;
        }
    }
    if (best != -1)
        clusters->entries[best].weight++;
    else if ((unsigned short)clusters->count < 32) {
        short index = clusters->count++;
        clusters->entries[index].weight = 1;
        clusters->entries[index].position = *point;
        float limit = radius * k_path_obstacles_cluster_two;
        clusters->entries[index].radius = limit > k_path_obstacles_cluster_limit ? k_path_obstacles_cluster_limit : limit;
    }
}

static __forceinline long path_obstacles_effect_next_index(s_data_array *array, long index)
{
    long absolute_index = data_next_absolute_index(array, (index + 1) & 0xffff);
    long result = -1;
    if (absolute_index != -1)
        result = (*(unsigned short *)(array->data + absolute_index * array->size) << 16) | absolute_index;
    return result;
}
static __forceinline float path_obstacles_transform_axis(float x, float y, float z, float forward, float left, float up, float position)
{
    return x * forward + (y * left + z * up) + position;
}
static __forceinline real_point3d *path_obstacles_effect_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
    const float & up_i = matrix->up.i;
    float x = point->x;
    float y = point->y;
    float z = point->z;
    float scale = matrix->scale;
    x *= scale;
    y *= scale;
    z *= scale;
    result->x = x * matrix->forward.i + (y * matrix->left.i + z * up_i) + matrix->position.x;
    result->y = path_obstacles_transform_axis(x, y, z, matrix->forward.j, matrix->left.j, matrix->up.j, matrix->position.y);
    result->z = x * matrix->forward.k + (y * matrix->left.k + z * matrix->up.k) + matrix->position.z;
    return result;
}
static __forceinline real_point3d *path_obstacles_effect_position(
    const effect_datum *effect, const effect_location_datum *location, real_point3d *position)
{
                        if ((short)location->node_index != -1) {
                            real_matrix4x3 matrix;
                            effect_get_node_matrix(effect, (short)location->node_index, &matrix);
                            path_obstacles_effect_transform_point(&matrix, &location->matrix.position, position);
                        } else {
                            *position = location->matrix.position;
                        }
return position;
}
// @address 0xd0228 obstacles_compute_effect_clusters
extern "C" void obstacles_compute_effect_clusters(path_obstacle_cluster *clusters)
{
    for (long effect_index = data_next_index(effect_data, -1); effect_index != -1;
         effect_index = path_obstacles_effect_next_index(effect_data, effect_index)) {
        const effect_datum *effect = &((effect_datum *)effect_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(effect_index)];
        const s_effect_definition *definition =
            TAG_GET('effe', const s_effect_definition, effect->definition_index);
        if (!definition->flags.test(_effect_definition_first_person_visibility_bit))
            return;
        if (!effect->flags.test(_effect_stopping_bit) && definition->danger_radius != 0.0f) {
            if (effect->object_index != -1) {
                // match: 0xd0228: manual header-table read; accessor calls here shift numbering and flip a downstream fmuls in this function
                object_datum *object = (object_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(effect->object_index)].datum;
                if ((1 << object->type) & 0x1003)
                    continue;
                if (object->parent != -1) {
                    object_datum *parent = (object_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(object->parent)].datum;
                    if ((1 << parent->type) & 0x1003)
                        continue;
                }
            }
            for (long index = 0; index < definition->locations.count; ++index) {
                effect_location_iterator iterator;
                iterator.effect = effect;
                iterator.next_index = effect->location_indices[index];
                if (iterator.next_index != -1) {
                    const effect_location_datum *location;
                    while ((location = iterator.next()) != 0) {
                        real_point3d position;
                        obstacles_cluster_add_point(clusters, path_obstacles_effect_position(effect, location, &position), definition->danger_radius);
                    }
                }
            }
        }
    }
}

// @address 0xd0528 obstacles_add_nearby_discs
extern "C" void obstacles_add_nearby_discs(long p0, long p1, short p2, bool p3, special_movement *movement, obstacles *obs, obstacles *obs2,
    const real_point3d *point, const real_point3d *actor_position, float radius, const real_vector3d *vect, const real_vector3d *vect2,
    long p11, long p12)
{
    s_location location;
    real_point3d point_copy;
    s_obstacle_profile profile;
    long indices[512];
    obstacle_profile_initialize(&profile, point, radius, p1, (profile_source *)movement, vect, p11, p12);
    if (p0 != -1)
        ::obstacle_profile_set_actor(
            p0, actor_position, vect2, p2, p3, &profile);
    if (p11 != -1) {
        s_object_header *element = object_header_get(p11);
        const object_datum *object_data = (const object_datum *)element->datum;
        location = object_data->location;
    } else {
        point_copy.x = point->x;
        point_copy.y = point->y;
        point_copy.z = point->z + 0.05f;
        scenario_location_from_point(&location, &point_copy);
    }
    short count = objects_in_sphere(1, 0x14cb, &location, point, radius, indices, 0x200);
    if (count > 0) {
        short i = 0;
        do {
            path_obstacle_avoidance_add_object_discs(p0, indices[i], &profile, p2, p3, obs, obs2);
            i = (short)(i + 1);
        } while (i < count);
    }
}

// @address 0x50b200 ?get_sector_flags_normal@@YAXJPATreal_vector3d@@@Z
// sector.h inline (COMDAT, `f i` in the March map): out-of-line copy, register-opaque to every caller
// match: 0xcf148: inline lets cl expand this into obstacles_find_avoidance_path; plain, 4 words of its normal loads change
#if defined(_XBOX) && !defined(__clang__)
inline void get_sector_flags_normal(long flags, real_vector3d *normal)
#else
void get_sector_flags_normal(long flags, real_vector3d *normal)
#endif
{
    if (flags & 16)
        set_real_vector3d(normal, 0.0f, 0.0f, 1.0f);
    else if (flags & 32)
        set_real_vector3d(normal, 0.0f, 0.0f, -1.0f);
    else if (flags & 64)
        set_real_vector3d(normal, 0.0f, 1.0f, 0.0f);
    else if (flags & 128)
        set_real_vector3d(normal, 0.0f, -1.0f, 0.0f);
    else if (flags & 256)
        set_real_vector3d(normal, 1.0f, 0.0f, 0.0f);
    else if (flags & 512)
        set_real_vector3d(normal, -1.0f, 0.0f, 0.0f);

}

static __forceinline void path_obstacles_new(obstacles *list)
{
    list->obstacle_count = 0;
    list->disc_count = 0;
    list->flag1_count = 0;
    list->flag8_count = 0;
    list->unused = 0;
}
// Inline expansion of normalize3d; the out-of-line copy lives in real_math.obj.
static __forceinline float normalize3d_inline(real_vector3d *vector)
{
    float length = sqrtf(vector->i * vector->i + (vector->j * vector->j + vector->k * vector->k));
    if (fabsf(length) >= k_real_epsilon)
    {
        scale_vector3d(vector, 1.0f / length, vector);
        return length;
    }
    return 0.0f;
}
// Inline expansion of normalize2d; the out-of-line copy lives in real_math.obj.
static __forceinline float normalize2d_inline(real_vector2d *vector)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j);
    if (!(fabsf(length - 0.0f) < k_real_epsilon))
    {
        float scale = 1.0f / length;
        vector->i = vector->i * scale;
        vector->j = vector->j * scale;
    }
    else
    {
        length = 0.0f;
    }
    return length;
}
// Local spelling of projection_sign_from_vector3d; the shared copy lives in projection_math.h.
static __forceinline bool projection_sign_from_vector3d_inline(const real_vector3d *vector, short projection)
{
    return vector->n[projection] > 0.0f;
}
static __forceinline bool path_obstacles_process_step(const path_step *step)
{
    return (unsigned short)step->type == 0xffff && step->sector.valid();
}
static __forceinline obstacle_node *path_obstacles_get_step(obstacle_path *path, short index)
{
    return &path->nodes[index];
}
// Local spelling of point_in_sphere summing i+(k+j); real_math.h sums i+j+k via distance_squared3d.
static __forceinline bool point_in_sphere_inline(const real_point3d *point, const real_point3d *center, float radius)
{
    real_vector3d delta;
    delta.i = point->x - center->x;
    delta.j = point->y - center->y;
    delta.k = point->z - center->z;
    bool result = delta.i * delta.i + (delta.k * delta.k + delta.j * delta.j) <= radius * radius;
    return result;
}

// @address 0xcf148 obstacles_find_avoidance_path
extern "C" bool obstacles_find_avoidance_path(long actor_index, const path_state *state, short point_count,
    // match: 0xcf148: pointer __restrict qualifier prevents alias reload
    const path_step *points, bool allow_partial, short *output_count, path_step *__restrict output_points,
    bool *final_point, long *ignorable_object_index, short *ignorable_flags, bool *blocked)
{
    bool result = true;
    actor_datum *actor = 0;
    float radius = PATH_OBSTACLES_MAX(state->input.radius, 0.2f);
    float obstacle_radius = 4.0f;
    short previous_output_count = 0;
    long obstacle_flags = 0;
    bool final_point_flag = *final_point;
    path_obstacle_cluster clusters;
    clusters.count = 0;
    memset(clusters.entries, 0, sizeof(clusters.entries));
    c_ai_point3d previous_point;
    c_sector_ref previous_sector;
    obstacles ignorable_obstacles;

    if (actor_index != -1)
    {
        actor = datum_get<actor_datum>(actor_data, actor_index);
        if (actor->root_vehicle_index != -1)
            obstacle_radius = 20.0f;
    }
    if (ignorable_object_index)
        *ignorable_object_index = -1;
    if (state->debug && !state->debug->locked)
        state->debug->path_count = 0;
    if (blocked)
        *blocked = false;
    if (actor)
    {
        obstacle_flags |= 0x1;
        obstacle_flags |= 0x80;
        if (actor->root_vehicle_index == -1)
        {
            obstacle_flags |= 0x2;
            if (actor->combat_status >= 3 || allow_partial)
            {
                obstacle_flags |= 0x4;
                if ((state->movement.hint_types & 0x8) && (state->movement.movement_hints & 0x3))
                    obstacle_flags |= 0x8;
                if ((state->movement.hint_types & 0x10) && (state->movement.movement_hints & 0xe0))
                    obstacle_flags |= 0x10;
                if ((state->movement.hint_types & 0x20) && (state->movement.movement_hints & 0x1800))
                    obstacle_flags |= 0x20;
            }
        }
    }
    obstacles_compute_effect_clusters(&clusters);

    for (short point_index = 0; point_index < point_count; point_index++)
    {
        obstacles local_obstacles;
        obstacle_path local_path;
        obstacles *obstacle_list = &local_obstacles;
        obstacle_path *path = &local_path;
        const path_step *point = &points[point_index];
        short reference_frame = point->point.reference_frame();
        bool final_segment = point_index == point_count - 1 && final_point_flag;
        unsigned short partial_obstacle = 0xffff;
        bool sector_point = path_obstacles_process_step(point);
        bool direct = false;
        long flags = obstacle_flags;
        short projection = 2;
        bool projection_sign = true;
        if (!sector_point)
        {
            direct = points[point_index].type == 6;
            if (direct)
                flags = 1;
        }
        if (*output_count >= 4)
        {
            *final_point = false;
            break;
        }
        if (point->sector.valid())
        {
            c_sector_ref sector = point->sector;
            // match: 0xcf148: pathfinding_get_sector takes a short, sign-extending the sector index.
            const pathfinding_sector *sector_data = &(
                (pathfinding_sector *)pathfinding_data_get(sector.structure_bsp_index)->sectors.address)[(short)sector.sector_index];
            real_vector3d normal;
            if (sector_data->flags & 0x1000)
                normal = *global_up3d;
            else
                get_sector_flags_normal(sector_data->flags, &normal);
            projection = projection_from_vector3d(&normal);
            projection_sign = projection_sign_from_vector3d_inline(&normal, projection);
        }
        c_ai_point3d start_point;
        c_sector_ref start_sector;
        if (point_index > 0)
        {
            start_sector = previous_sector;
            start_point = previous_point;
        }
        else
        {
            start_sector = state->input.start_sector_ref;
            start_point = state->input.start;
        }
        real_point3d start;
        ai_point_get_position(&start_point, &start);
        ai_reference_frame_inverse_transform_point(reference_frame, &start, &start);
        project_point3d(&start, projection, projection_sign, &start);
        real_point3d goal;
        project_point3d(&point->point.point, projection, projection_sign, &goal);
        c_sector_ref goal_sector = point->sector;
        if (sector_point || direct)
        {
            if (state->debug)
            {
                obstacle_list = &state->debug->obstacles[point_index];
                path = &state->debug->paths[point_index];
            }
            real_vector3d direction;
            ai_vector_from_points3d(&start_point, &point->point, &direction);
            normalize3d_inline(&direction);
            real_vector3d actor_facing;
            if (actor)
                actor_facing = actor->facing_vector;
            real_point3d start_position;
            ai_point_get_position(&start_point, &start_position);
            real_point3d goal_position;
            ai_point_get_position(&point->point, &goal_position);
            short cluster_count = clusters.count;
            path_obstacles_new(obstacle_list);
            path_obstacles_new(&ignorable_obstacles);
            for (short cluster_index = 0; cluster_index < cluster_count; cluster_index++)
            {
                path_obstacle_cluster::entry *cluster = &clusters.entries[cluster_index];
                if (point_in_sphere_inline(&start_position, &cluster->position, cluster->radius + obstacle_radius))
                    obstacles_add_disc(obstacle_list, -1, 0, &cluster->position, cluster->radius);
            }
            obstacles_add_nearby_discs(actor_index, flags, projection, projection_sign, (special_movement *)&state->movement, obstacle_list,
                &ignorable_obstacles, &start_position, &goal_position, obstacle_radius, &direction,
                actor ? &actor_facing : 0, state->input.unit_index, state->input.destination_object_index);
            for (short disc_index = 0; disc_index < obstacle_list->disc_count; disc_index++)
            {
                obstacle_disc *entry = &obstacle_list->discs[disc_index];
                real_point3d center;
                center.x = entry->center.x;
                center.y = entry->center.y;
                center.z = 0.0f;
                ai_reference_frame_inverse_transform_point(reference_frame, &center, &center);
                entry->center.x = center.x;
                entry->center.y = center.y;
            }
            for (short disc_index = 0; disc_index < ignorable_obstacles.disc_count; disc_index++)
            {
                obstacle_disc *entry = &ignorable_obstacles.discs[disc_index];
                real_point3d center;
                center.x = entry->center.x;
                center.y = entry->center.y;
                center.z = 0.0f;
                ai_reference_frame_inverse_transform_point(reference_frame, &center, &center);
                entry->center.x = center.x;
                entry->center.y = center.y;
            }
            if (state->input.use_attractor && state->input.pad47)
            {
                real_point3d ignore_position;
                ai_reference_frame_inverse_transform_point(reference_frame, &state->input.attractor, &ignore_position);
                obstacles_add_disc(obstacle_list, state->input.attractor_object_index, 1, &ignore_position, state->input.attractor_strength);
            }
            ::obstacles_recompute(obstacle_list, radius);
            if (state->debug && !state->debug->locked)
                state->debug->path_count++;
        }
        if (sector_point && obstacle_list->disc_count > 0)
        {
            bool found = path_find(state, path, state->input.allow_broken_sectors, obstacle_list, radius, &start, start_sector, &goal, goal_sector,
                projection,
                projection_sign, final_segment, false);
            if (!found && !allow_partial && obstacle_list->flag1_count > 0)
                found = path_find(state, path, state->input.allow_broken_sectors, obstacle_list, radius, &start, start_sector, &goal, goal_sector,
                projection,
                    projection_sign, final_segment, true);
            if (!found && allow_partial &&
                ((obstacle_list->flag8_count > 0 && path->partial != 0xffff) || obstacle_list->flag1_count > 0))
            {
                for (short disc_index = 0; disc_index < obstacle_list->disc_count; disc_index++)
                {
                    obstacle_disc *entry = &obstacle_list->discs[disc_index];
                    if ((unsigned short)entry->obstacle_index == path->partial && (entry->flags & 0x8))
                        entry->flags |= 1;
                    else
                        entry->flags &= ~1;
                }
                partial_obstacle = path->partial;
                if (point_index > 0)
                {
                    project_point3d(&points[point_index - 1].point.point, projection, projection_sign, &start);
                    start_sector = points[point_index - 1].sector;
                }
                found = path_find(state, path, state->input.allow_broken_sectors, obstacle_list, radius, &start, start_sector, &goal, goal_sector,
                projection,
                    projection_sign, final_segment, true);
            }
            if (found)
            {
                bool overflow = false;
                short step_count = 0;
                path_step steps[64];
                if (path->complete)
                {
                    inverse_project_point3d(&goal, projection, projection_sign, &previous_point.point);
                    previous_point.location[0] = reference_frame;
                    previous_sector = goal_sector;
                }
                else
                {
                    obstacle_node *last = path_obstacles_get_step(path, path->last_node);
                    real_point3d last_point;
                    last_point.x = last->point.x;
                    last_point.y = last->point.y;
                    last_point.z = goal.z;
                    inverse_project_point3d(&last_point, projection, projection_sign, &previous_point.point);
                    previous_point.location[0] = reference_frame;
                    previous_sector = last->sector;
                }
                short node_index = path->last_node;
                while (node_index != 0)
                {
                    path_step *step = &steps[step_count++];
                    obstacle_node *node = path_obstacles_get_step(path, node_index);
                    step->sector = node->sector;
                    step->type = -1;
                    step->hint.clear();
                    step->flags = 0;
                    real_point3d node_position;
                    inverse_project_point3d(&node->point, projection, projection_sign, &node_position);
                    step->point.set(&node_position, points[point_index].point.reference_frame());
                    node_index = node->previous;
                    if (step_count >= 64)
                    {
                        overflow = true;
                        break;
                    }
                }
                short count = *output_count;
                for (short index = step_count - 1; index >= 0; index--)
                {
                    if (count >= 4)
                    {
                        overflow = true;
                        break;
                    }
                    output_points[count] = steps[index];
                    count++;
                }
                *output_count = count;
                if (overflow)
                {
                    *final_point = false;
                    break;
                }
            }
            else
            {
                if (point_index == point_count - 1)
                {
                    for (short disc_index = 0; disc_index < obstacle_list->disc_count; disc_index++)
                    {
                        if ((obstacle_list->discs[disc_index].flags & 0x2) && blocked)
                            *blocked = true;
                    }
                }
                result = false;
                break;
            }
        }
        else
        {
            bool clear = true;
            if (direct && obstacle_list->disc_count > 0)
            {
                for (short disc_index = 0; disc_index < obstacle_list->disc_count; disc_index++)
                {
                    const obstacle_disc *entry = &obstacle_list->discs[disc_index];
                    float overlap = entry->radius + radius - distance2d(&goal.xy, &entry->center);
                    if (overlap > radius)
                    {
                        clear = false;
                        break;
                    }
                }
            }
            if (clear)
            {
                path_step *output = &output_points[*output_count];
                *output = points[point_index];
                (*output_count)++;
                previous_point = output->point;
                previous_sector = output->sector;
            }
            else
            {
                result = false;
                break;
            }
        }
        if (result && sector_point && ignorable_object_index && *ignorable_object_index == -1 &&
            (ignorable_obstacles.disc_count > 0 || partial_obstacle != 0xffff))
        {
            short partial_index = (short)partial_obstacle;
            if (partial_index != -1)
            {
                for (short disc_index = 0; disc_index < obstacle_list->disc_count; disc_index++)
                {
                    const obstacle_disc *entry = &obstacle_list->discs[disc_index];
                    if (entry->obstacle_index == partial_index)
                    {
                        real_point3d center;
                        center.x = entry->center.x;
                        center.y = entry->center.y;
                        center.z = 0.0f;
                        obstacles_add_disc(&ignorable_obstacles, entry->object_index, entry->flags, &center, entry->radius);
                    }
                }
            }
            for (short index = previous_output_count; index < *output_count; index++)
            {
                const path_step *output = &output_points[index];
                if ((unsigned short)output->type == 0xffff)
                {
                    const real_point3d *from;
                    c_sector_ref from_sector;
                    if (index == 0)
                    {
                        from = &state->input.start.point;
                        from_sector = state->input.start_sector_ref;
                    }
                    else
                    {
                        const path_step *previous = &output_points[index - 1];
                        from = &previous->point.point;
                        from_sector = previous->sector;
                    }
                    real_vector3d direction;
                    vector_from_points3d(from, &output->point.point, &direction);
                    float distance = normalize2d_inline(&direction.xy);
                    direction.k = 0.0f;
                    if (final_segment && index == *output_count - 1)
                        distance -= 0.25f;
                    if (distance > 0.0f &&
                        ::actor_test_ignorable_obstacles(actor_index, &ignorable_obstacles, projection, projection_sign,
                            from, from_sector, &direction, radius, distance, ignorable_object_index, ignorable_flags))
                    {
                        break;
                    }
                }
            }
        }
        previous_output_count = *output_count;
    }
    return result;
}
