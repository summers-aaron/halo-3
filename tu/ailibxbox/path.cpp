// ailibxbox/path.obj: from the records 0xcd5f0 0xcd758 0xcd968 0xce180 0x11d450 0x5508c8 0x550a78 0x5510c0 0x551310 0x551f78 0x552058 0x552180 0x552c50
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "ai/pathfinding_collision.h"
#include "ai/path_state_types.h"
#include "ai/behavior_stimulus_types.h"
#include "memory/data.h"
#include "memory/tls_globals.h"
#include <stdlib.h>
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "ai/ai_point3d.h"
#include "ai/sector_ref.h"
#include "ai/special_movement_types.h"
// @address 0xcd758 ?path_state_find@@YA_NPAUpath_state@@@Z
// @address 0xce180 ?path_node_from_hash_table@@YAFPAUpath_state@@Vc_sector_ref@@@Z
// @address 0x551f78 ?path_heap_bubble_up@@YAXPAUpath_state@@F@Z
// @address 0x552058 ?path_heap_bubble_down@@YAXPAUpath_state@@F@Z
// @address 0x552c50 ?closest_point_to_attractor@@YAXPBTreal_point3d@@00PAT1@@Z

#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include <math.h>
#include <string.h>
#include "ai/actors.h"
#include "ai/ai_hint_definitions.h"
#include "ai/path.h"
#include "ai/path_flying.h"
#include "ai/path_smoothing.h"
#include "ai/sector.h"
#include "ai/path_point_types.h"
struct path_state;
struct path_debug_storage;
struct path_state;


#include "ai/path_types.h"
#include "ai/actor_datum.h"
#include "ai/pathfinding_geometry.h"
#include "ai/pathfinding_data.h"
#include "ai/hint_ref.h"
#include "ai/ai_reference_frame.h"
#include "ai/actor_stimulus.h"
struct path_debug_storage;
#include "ai/pathfinding_sector_types.h"


#include "ai/path_result.h"


// @address 0xcd5f0 ?path_state_new@@YAXPBUpath_input@@PAUpath_state@@PAUpath_error_info@@PAUpath_debug_storage@@PAUspecial_movement@@PBUactor_moving_reserved_hints@@@Z
#include "ai/ai_hint_jump.h"
#include "ai/path_obstacles.h"

void path_state_new(struct path_input const *input, struct path_state *state, struct path_error_info *error,
    struct path_debug_storage *debug, struct special_movement *movement, struct actor_moving_reserved_hints const *cache)
{
 state->has_goal=false; state->best_node_index=-1; state->node_count=0; state->heap_count=0;
 if(error) state->error=*error;
 else { state->error.a=0; state->error.b=0; memset(state->error.data,0,sizeof(state->error.data)); }
 memcpy(&state->input,input,sizeof(*input));
 state->debug=0; state->another_flag=false;
 if(movement) state->movement=*movement;
 else memset(&state->movement,0,sizeof(state->movement));
 if(cache) state->cache=*cache;
 else memset(&state->cache,0,sizeof(state->cache));
 state->flight_cache.count=0;
}

static __forceinline void path_state_reset_for_find(path_state *state)
{
    state->node_count = 0;
    state->heap_count = 1;
    memset(state->hash_buckets, -1, sizeof(state->hash_buckets));
    state->best_x98 = 3.4028234663852886e+38f;
    state->best_node_index = -1;
    state->best_x9c = 3.4028234663852886e+38f;
}

static __forceinline void path_error_stage_advance_for_find(path_error_info *error)
{
    error->a++;
    if (error->a > 32)
        error->a = 0;
}


bool path_state_find(path_state *state)
{
    bool result = false;
    path_state_reset_for_find(state);
    if (path_state_begin(state))
        result = path_state_traverse(state);
    if (!result)
    {
        if ((unsigned short)state->error.a == 0)
            state->error.a = 32;
        else
            path_error_stage_advance_for_find(&state->error);
    }
    return result;
}

short path_node_from_hash_table(path_state *table, c_sector_ref key)
{
    short a = key.structure_bsp_index;
    unsigned short b = key.sector_index;
    short slot = (b << 3) & 0x1ff8;
    bool mismatch;
    unsigned short index;
    do {
        short current = slot;
        slot = (current + 1) & 0x1fff;
        index = table->hash_buckets[current];
        if ((short)index == -1)
            return index;
        mismatch = !(table->node_list[(short)index].sector_ref.structure_bsp_index == a && table->node_list[(short)index].sector_ref.sector_index == b);
    } while (mismatch);
    return index;
}

static __forceinline path_node *path_state_get_node(path_state *state, short node_index)
{
    path_node *nodes = state->node_list;
    return &nodes[node_index];
}

// @address 0x5508c8 ?path_state_approach_point@@YA_NPAUpath_state@@PBVc_ai_point3d@@Vc_sector_ref@@PA_NPAV2@@Z
// Walks the parent chain from sector_ref's node toward the start while each segment to the parent is unobstructed.
// Reports the start point (*p3 = true) when the chain reaches the root.
bool path_state_approach_point(path_state *state, const c_ai_point3d *ai_point, c_sector_ref sector_ref, bool *p3, c_ai_point3d *ai_point2)
{
    short node_index = path_node_from_hash_table(state, sector_ref);
    bool found = false;
    if (node_index != -1)
    {
        path_node *node = &state->node_list[node_index];
        for (short parent_index = node->parent_node_index; parent_index != -1;
            node = &state->node_list[node->parent_node_index], parent_index = node->parent_node_index)
        {
            path_node *parent = &state->node_list[parent_index];
            if ((unsigned short)ai_point->location[0] != (unsigned short)parent->point.location[0])
                break;

            real_vector3d direction;
            vector_from_points3d(&ai_point->point, &parent->point.point, &direction);
            float length = sqrtf((direction.k * direction.k + direction.i * direction.i) + direction.j * direction.j);
            if (!(fabs(length) < 0.0001f))
            {
                float inverse = 1.0f / length;
                direction.i = inverse * direction.i;
                direction.j = inverse * direction.j;
                direction.k = inverse * direction.k;
            }
            else
            {
                length = 0.0f;
            }
            path_collision_result collision;
            if (pathfinding_test_vector(&ai_point->point, sector_ref, parent->sector_ref, &direction, length, 0, &collision))
                break;
        }

        if ((unsigned short)node->parent_node_index == 0xffff)
        {
            *p3 = true;
            *ai_point2 = state->input.start;
        }
        else
        {
            *p3 = false;
            *ai_point2 = node->point;
        }
        found = true;
    }
    return found;
}

static __forceinline bool path_traverse_sector_matches(const c_sector_ref &reference, short structure_bsp_index, unsigned short sector_index)
{
    return reference.structure_bsp_index == structure_bsp_index && reference.sector_index == sector_index;
}

static __forceinline bool path_traverse_source_equal(const path_step_source *link, const path_step_source *source)
{
    return link->type == source->type
        && link->structure_bsp_index == source->structure_bsp_index
        && link->index == source->index;
}

static __forceinline short path_traverse_heap_remove(path_state *state)
{
    short node_index = NONE;
    if (state->heap_count > 1)
    {
        node_index = state->heap[1].node_index;
        state->node_list[node_index].heap_location = NONE;
        state->heap_count--;
        if (state->heap_count > 1)
        {
            state->heap[1] = state->heap[state->heap_count];
            path_heap_bubble_down(state, 1);
        }
    }
    return node_index;
}

static __forceinline float path_traverse_distance(const c_ai_point3d *a, const c_ai_point3d *b)
{
    return sqrtf(ai_distance_squared3d(a, b));
}

// @rdata 0x820242d8 path_traverse_two
static const float path_traverse_two = 2.0f;
// @rdata 0x82021694 path_begin_max_cost
static const float path_begin_max_cost = 3.402823466e+38f;

static __forceinline float path_traverse_constant(const float *value)
{
    return *value;
}

static __forceinline float path_traverse_minimum_width_squared(float radius, float width_scale)
{
    float diameter = width_scale * radius;
    return diameter * diameter;
}

static __forceinline float path_traverse_infinite_distance()
{
    return path_traverse_constant(&path_begin_max_cost);
}

static inline float path_traverse_attractor_weight(
    const path_state *state, const c_ai_point3d *start, const c_ai_point3d *end, float *distance, float maximum_distance)
{
    float weight = 0.0f;
    float nearest_distance = maximum_distance;
    real_point3d local_end;
    ai_reference_frame_inverse_transform_point(start->reference_frame(), end->get_point(), &local_end);
    real_point3d local_attractor;
    ai_reference_frame_inverse_transform_point(start->reference_frame(), &state->input.attractor, &local_attractor);
    real_point3d closest;
    closest_point_to_attractor(start->get_point(), &local_end, &local_attractor, &closest);
    real_vector3d delta;
    vector_from_points3d(&local_attractor, &closest, &delta);
    float distance_squared = magnitude_squared3d(&delta);
    if (distance_squared < state->input.attractor_radius * state->input.attractor_radius)
    {
        nearest_distance = sqrtf(distance_squared);
        weight = (1.0f - nearest_distance / state->input.attractor_radius) * state->input.attractor_weight;
    }
    *distance = nearest_distance;
    return weight;
}

// @address 0x551310 ?path_state_traverse@@YA_NPAUpath_state@@@Z
bool path_state_traverse(path_state *state)
{
    float radius = state->input.radius < 0.2f ? 0.2f : state->input.radius;
    bool result = true;
    float hint_cost = state->input.hint_cost;
    float goal_distance;
    float width_scale = path_traverse_constant(&path_traverse_two);
    float maximum_distance = path_traverse_infinite_distance();
    for (;;)
    {
        short cheapest_node_index = path_traverse_heap_remove(state);
        if (cheapest_node_index == NONE)
            break;
        path_node *cheapest_node = path_state_get_node(state, cheapest_node_index);
        if (state->has_goal)
        {
            if (cheapest_node->sector_ref == state->goal.sector)
            {
                state->best_node_index = cheapest_node_index;
                state->best_x98 = 0.0f;
                state->best_point = state->goal.point;
                break;
            }
            if (cheapest_node->total_cost_estimate > state->best_x9c + 10.0f * (state->best_x98 < 5.0f ? 5.0f : state->best_x98))
                break;
        }
        path_link links[100];
        short link_count = build_path_links_for_sector(cheapest_node, links, state);
        for (short link_index = 0; link_index < link_count; link_index++)
        {
            path_link *link = &links[link_index];
            short structure_bsp_index = link->sector_ref.structure_bsp_index;
            if (link->sector_ref == cheapest_node->link_sector_ref)
                continue;
            short sector_flags = link->sector_flags;
            if (!(sector_flags & 1))
                continue;
            if (cheapest_node->flag_c && (sector_flags & 0x1000))
                continue;
            if (state->error.a > 0)
            {
                bool failed = false;
                path_error_info *error = &state->error;
                for (short index = 0; index < error->b; index++)
                {
                    if ((error->a & (1 << index)) && path_traverse_source_equal(&link->source, &error->links[index]))
                    {
                        failed = true;
                        break;
                    }
                }
                if (failed)
                    continue;
            }
            if (!state->input.allow_broken_sectors && (sector_flags & 2))
            {
                c_sector_ref reference = link->sector_ref;
                pathfinding_data *data = pathfinding_data_get(reference.structure_bsp_index);
                if (sector_broken(reference.structure_bsp_index, data,
                        &((struct pathfinding_sector *)data->sectors.address)[(short)reference.sector_index]))
                    continue;
            }
            float length_squared = magnitude_squared3d(&link->vector);
            if (link->narrow && !link->allow_narrow && !state->input.allow_narrow_links
                && length_squared < path_traverse_minimum_width_squared(radius, width_scale))
                continue;
            short node_index = NONE;
            long reference_frame = link->point.reference_frame();
            real_point3d midpoint;
            c_ai_point3d point;
            point_from_line3d(link->point.get_point(), &link->vector, 0.5f, &midpoint);
            point.set(&midpoint, reference_frame);
            unsigned long has_goal = state->has_goal;
            if (has_goal && length_squared > 16.0f && length_squared > path_traverse_minimum_width_squared(radius, width_scale))
            {
                float length = sqrtf(length_squared);
                real_vector3d direction;
                ai_vector_from_points3d(&link->point, &state->goal.point, &direction);
                const real_vector3d *projection_vector = &link->vector;
                float fraction = dot_product3d(&direction, projection_vector) / magnitude_squared3d(&link->vector);
                fraction = MIN(MAX(fraction, radius / length), 1.0f - radius / length);
                point_from_line3d(link->point.get_point(), &link->vector, fraction, &point.point);
            }
            float distance;
            switch (link->hint_type)
            {
            case 1:
            {
                pathfinding_data *data = pathfinding_data_get(cheapest_node->sector_ref.structure_bsp_index);
                pathfinding_hint *hint = TAG_BLOCK_GET_ELEMENT(&data->hints, link->index, pathfinding_hint);
                if (!(hint->fields.flags & 2))
                {
                    long start_index = hint->data.data[0] & 0xffff;
                    const real_point3d *start = TAG_BLOCK_GET_ELEMENT(&data->vertices, start_index, real_point3d);
                    const real_point3d *end = TAG_BLOCK_GET_ELEMENT(&data->vertices, hint->fields.index0, real_point3d);
                    real_vector3d edge;
                    vector_from_points3d(start, end, &edge);
                    float length = normalize3d(&edge);
                    real_vector3d relative;
                    vector_from_points3d(start, cheapest_node->point.get_point(), &relative);
                    float fraction = dot_product3d(&relative, &edge);
                    fraction = MIN(MAX(fraction, 0.0f), length);
                    real_point3d closest;
                    point_from_line3d(start, &edge, fraction, &closest);
                    c_ai_point3d source_point;
                    source_point.set(&closest, (short)hint->fields.reference_frame);
                    distance = distance3d(cheapest_node->point.get_point(), source_point.get_point()) + path_traverse_distance(&source_point, &point);
                    break;
                }
            }
            case 6:
            {
                pathfinding_data *data = pathfinding_data_get(cheapest_node->sector_ref.structure_bsp_index);
                pathfinding_hint *hint = TAG_BLOCK_GET_ELEMENT(&data->hints, link->index, pathfinding_hint);
                long reference_frame = hint->fields.reference_frame;
                const real_point3d *start = TAG_BLOCK_GET_ELEMENT(&data->vertices, hint->data.data[0] & 0xffff, real_point3d);
                c_ai_point3d source_point;
                source_point.set(start, reference_frame);
                distance = distance3d(cheapest_node->point.get_point(), source_point.get_point()) + path_traverse_distance(&source_point, &point);
                break;
            }
            default:
                distance = path_traverse_distance(&cheapest_node->point, &point);
                break;
            }
            float total_distance = cheapest_node->x30 + distance;
            float attractor_distance;
            float cost;
            if (state->input.use_attractor)
            {
                cost = (1.0f + path_traverse_attractor_weight(state, &cheapest_node->point, &point, &attractor_distance, maximum_distance)) * distance;
                attractor_distance = cheapest_node->x2c > attractor_distance ? attractor_distance : cheapest_node->x2c;
            }
            else
            {
                attractor_distance = 0.0f;
                cost = distance;
            }
            if ((state->movement.hint_types & 4) && !((unsigned short)sector_flags & 0x8000))
                cost *= width_scale;
            if (hint_cost > 0.0f && link->hint_type != NONE)
            {
                switch (link->hint_type)
                {
                case 1:
                case 5:
                case 6:
                    cost += hint_cost;
                    break;
                }
            }
            if (link->hint_type == 5 && state->cache.count > 0)
            {
                for (short index = 0; index < 4; index++)
                {
                    const actor_moving_reserved_hint *entry = &state->cache.hints[index];
                    if (entry->structure_bsp_index == link->structure_bsp_index && entry->hint_index == link->index)
                    {
                        if (entry->count > 0)
                            cost += (float)entry->count * 3.0f;
                        else if (entry->count < 0)
                            cost -= hint_cost;
                        break;
                    }
                }
            }
            float total_cost = cheapest_node->x34 + cost;
            float total_cost_estimate = total_cost;
            if (has_goal)
            {
                goal_distance = path_traverse_distance(&point, &state->goal.point);
                total_cost_estimate += goal_distance;
            }
            long quantized_cost = (long)(total_cost_estimate * 10.0f);
            if (quantized_cost < 32767)
            {
                if (!(state->input.bounded_search && total_distance > state->input.maximum_distance))
                {
                    short bucket = (link->sector_ref.sector_index << 3) & 0x1ff8;
                    for (;; bucket = (bucket + 1) & 0x1fff)
                    {
                        short existing_index = state->hash_buckets[bucket];
                        if (existing_index != NONE)
                        {
                            path_node *existing = path_state_get_node(state, existing_index);
                            if (path_traverse_sector_matches(existing->sector_ref, structure_bsp_index, link->sector_ref.sector_index))
                            {
                                if (quantized_cost < existing->quantized_cost_estimate && existing->heap_location != NONE)
                                    node_index = existing_index;
                                break;
                            }
                        }
                        else
                        {
                            if (state->node_count < 2048)
                            {
                                node_index = state->node_count++;
                                state->hash_buckets[bucket] = node_index;
                                state->node_list[node_index].heap_location = NONE;
                            }
                            break;
                        }
                    }
                }
            }
            if (node_index != NONE)
            {
                path_node *node = path_state_get_node(state, node_index);
                node->parent_node_index = cheapest_node_index;
                node->link_sector_ref = cheapest_node->sector_ref;
                node->sector_ref = link->sector_ref;
                node->point = point;
                node->x28 = distance;
                node->x2c = attractor_distance;
                node->x30 = total_distance;
                node->x34 = total_cost;
                node->total_cost_estimate = total_cost_estimate;
                node->quantized_cost_estimate = (short)quantized_cost;
                node->flag_c = (sector_flags & 0x1000) != 0;
                node->flag_d = false;
                node->source = link->source;
                switch ((short)node->source.type)
                {
                case 1:
                case 5:
                case 6:
                    node->x3e = cheapest_node->x3e + 2;
                    break;
                default:
                    node->x3e = cheapest_node->x3e + 1;
                    break;
                }
                if (node->heap_location == NONE)
                {
                    if (state->heap_count < 2048)
                    {
                        short heap_location = state->heap_count++;
                        state->heap[heap_location].node_index = node_index;
                        state->heap[heap_location].cost_estimate = (short)quantized_cost;
                        path_heap_bubble_up(state, heap_location);
                    }
                }
                else
                {
                    state->heap[node->heap_location].cost_estimate = (short)quantized_cost;
                    path_heap_bubble_up(state, node->heap_location);
                }
                if (state->has_goal)
                {
                    c_ai_point3d best_point = node->point;
                    if (state->goal.distance > 0.0f && goal_distance < state->best_x98)
                    {
                        state->best_node_index = node_index;
                        state->best_x98 = goal_distance;
                        state->best_point = best_point;
                        state->best_x9c = total_cost_estimate;
                    }
                }
            }
        }
    }
    if (state->has_goal)
        result = state->best_x98 <= state->goal.distance;
    return result;
}

void path_heap_bubble_up(struct path_state * _sym_p0, short heap_location)
{
    short node_index = _sym_p0->heap[heap_location].node_index;
    short node_cost = _sym_p0->heap[heap_location].cost_estimate;

    while (heap_location > 1)
    {
        short parent_location = heap_location >> 1;
        short parent_node_index = _sym_p0->heap[parent_location].node_index;
        short parent_cost_estimate = _sym_p0->heap[parent_location].cost_estimate;

        if (node_cost < parent_cost_estimate)
        {
            _sym_p0->heap[heap_location].node_index = parent_node_index;
            _sym_p0->heap[heap_location].cost_estimate = parent_cost_estimate;
            _sym_p0->node_list[parent_node_index].heap_location = heap_location;
            heap_location = parent_location;
        }
        else
        {
            break;
        }
    }

    _sym_p0->heap[heap_location].node_index = node_index;
    _sym_p0->heap[heap_location].cost_estimate = node_cost;
    _sym_p0->node_list[node_index].heap_location = heap_location;
}

void path_heap_bubble_down(struct path_state * _sym_p0, short heap_location) {
 heap_location=1;
 short node_index=_sym_p0->heap[heap_location].node_index;
 short node_cost=_sym_p0->heap[heap_location].cost_estimate;
 while(true) {
  short best_location=heap_location;
  short best_node=node_index;
  short best_cost=node_cost;
  for(short child=0;child<2;child++) {
   short child_location=heap_location*2+child;
   if(child_location>=_sym_p0->heap_count) break;
   short child_node=_sym_p0->heap[child_location].node_index;
   short child_cost=_sym_p0->heap[child_location].cost_estimate;
   if(child_cost<best_cost) { best_location=child_location; best_node=child_node; best_cost=child_cost; }
  }
  if(best_location==heap_location) break;
  _sym_p0->heap[heap_location].node_index=best_node;
  _sym_p0->heap[heap_location].cost_estimate=best_cost;
  _sym_p0->node_list[best_node].heap_location=heap_location;
  heap_location=best_location;
 }
 _sym_p0->heap[heap_location].node_index=node_index;
 _sym_p0->heap[heap_location].cost_estimate=node_cost;
 _sym_p0->node_list[node_index].heap_location=heap_location;
}

void closest_point_to_attractor(const real_point3d *start, const real_point3d *end, const real_point3d *attractor, real_point3d *result)
{
    real_vector3d direction;
    vector_from_points3d(start, end, &direction);
    float magnitude_squared = magnitude_squared3d(&direction);
    if (magnitude_squared > 0.0f)
    {
        real_vector3d to_start;
        vector_from_points3d(attractor, start, &to_start);
        float fraction = dot_product3d(&direction, &to_start) / magnitude_squared;
        if (fraction < 0.0f || fraction > 1.0f)
        {
            *result = *end;
        }
        else
        {
            point_from_line3d(start, &direction, fraction, result);
        }
    }
    else
    {
        *result = *start;
    }
}





static __forceinline pathfinding_sector *path_start_sector(c_sector_ref reference)
{
    pathfinding_data *data = pathfinding_data_get(reference.structure_bsp_index);
    return &((struct pathfinding_sector *)data->sectors.address)[(short)reference.sector_index];
}

static __forceinline void path_begin_heap_insert(path_state *state, short node_index, short cost)
{
    if (state->heap_count < 2048)
    {
        short location = state->heap_count++;
        state->heap[location].node_index = node_index;
        state->heap[location].cost_estimate = cost;
        ::path_heap_bubble_up(state, location);
    }
}

// @rdata 0x8202251c path_begin_zero
static const float path_begin_zero = 0.0f;
// @rdata 0x82022850 path_begin_cost_scale
static const float path_begin_cost_scale = 10.0f;

static __forceinline float path_begin_get_zero()
{
    return path_begin_zero;
}

// @address 0x11d450 ?path_state_estimated_distance@@YA_NPAUpath_state@@PBVc_ai_point3d@@Vc_sector_ref@@PAM3PATreal_vector3d@@@Z
bool path_state_estimated_distance(
    path_state *state, const c_ai_point3d *point, c_sector_ref sector_ref, float *distance, float *deviation, real_vector3d *direction)
{
    bool success;
    short node_index = path_node_from_hash_table(state, sector_ref);
    if (node_index != -1)
    {
        path_node *node = &state->node_list[node_index];
        float total_distance = 0.0f;
        bool estimated = false;
        path_collision_result collision;
        pathfinding_test_line(&state->input.start.point, state->input.start_sector_ref, &point->point, sector_ref, 0, &collision);
        if (!collision.collision)
        {
            estimated = true;
            total_distance = distance3d(&state->input.start.point, &point->point);
        }
        if (!estimated)
        {
            short index = node_index;
            c_ai_point3d current = *point;
            float margin = state->input.radius * 1.2f;
            while (index != -1)
            {
                bool handled = false;
                estimated = true;
                path_node *step = &state->node_list[index];
                if (index == 0)
                {
                    total_distance += ai_distance3d(&current, &state->input.start);
                    handled = true;
                }
                else if ((short)step->source.type == -1 && step->source.index != -1 &&
                    step->source.index != 0xffff && (short)step->source.structure_bsp_index != -1)
                {
                    pathfinding_data *data = pathfinding_data_get((short)step->source.structure_bsp_index);
                    sector_link *link = &((sector_link *)data->links.address)[step->source.index];
                    const real_point3d *vertex0 = TAG_BLOCK_GET_ELEMENT(&data->vertices, link->start_vertex, real_point3d);
                    const real_point3d *vertex1 = TAG_BLOCK_GET_ELEMENT(&data->vertices, link->end_vertex, real_point3d);
                    real_point3d position;
                    ai_point_get_position(&current, &position);
                    real_vector3d edge;
                    vector_from_points3d(vertex0, vertex1, &edge);
                    real_vector3d offset;
                    vector_from_points3d(vertex0, &position, &offset);
                    float edge_length = normalize3d(&edge);
                    float t = dot_product3d(&edge, &offset);
                    t = PIN(t, margin, edge_length - margin);
                    real_point3d closest;
                    point_from_line3d(vertex0, &edge, t, &closest);
                    total_distance += distance3d(&position, &closest);
                    current.point = closest;
                    handled = true;
                }
                if (!handled)
                {
                    total_distance += ai_distance3d(&current, &step->point);
                    current = step->point;
                }
                index = step->parent_node_index;
            }
        }
        if (!estimated)
            total_distance = ai_distance3d(&node->point, point) + node->x30;
        *distance = total_distance;
        if (deviation)
        {
            float attractor_distance = 0.0f;
            if (state->input.use_attractor)
            {
                real_point3d start;
                ai_point_get_position(&node->point, &start);
                real_point3d end;
                ai_point_get_position(point, &end);
                real_point3d closest;
                closest_point_to_attractor(&start, &end, &state->input.attractor, &closest);
                attractor_distance = distance3d(&state->input.attractor, &closest);
                if (attractor_distance > node->x2c)
                    attractor_distance = node->x2c;
            }
            *deviation = attractor_distance;
        }
        if (direction)
        {
            short index = node_index;
            float accumulated = 0.0f;
            short previous = -1;
            path_node *step;
            do
            {
                step = &state->node_list[index];
                short parent = step->parent_node_index;
                step->pad0 = previous;
                previous = index;
                index = parent;
            } while (index != -1);
            index = previous;
            while (index != -1 && accumulated < 0.8f)
            {
                step = &state->node_list[index];
                index = step->pad0;
                accumulated += step->x28;
            }
            const c_ai_point3d *end = point;
            if (index != -1)
                end = &step->point;
            ai_vector_from_points3d(&state->input.start, end, direction);
            normalize3d(direction);
        }
        success = true;
    }
    else
    {
        if (deviation)
            *deviation = path_begin_max_cost;
        if (direction)
            *direction = *global_zero_vector3d;
        *distance = path_begin_max_cost;
        success = false;
    }
    return success;
}

// @address 0x5510c0 ?path_state_begin@@YA_NPAUpath_state@@@Z
bool path_state_begin(path_state *state)
{
    bool result = false;
    if (state->input.start_sector_ref.valid() && state->input.start.point.z > -1000.0f)
    {
        float distance;
        long cost;
        switch (0)
        {
        case 0:
            if (state->has_goal)
            {
                distance = sqrtf(ai_distance_squared3d(&state->input.start, &state->goal.point));
                cost = (long)(distance * path_begin_cost_scale);
                if (cost >= 32767 || ai_reference_frames_are_incompatible(state->input.start.location[0], state->goal.point.location[0]))
                    break;
            }
            else
            {
                distance = path_begin_get_zero();
                cost = 0;
            }
            pathfinding_sector *start_sector = path_start_sector(state->input.start_sector_ref);
            short node_index = state->node_count++;
            path_node *node = &state->node_list[node_index];
            node->parent_node_index = -1;
            node->link_sector_ref.clear();
            node->sector_ref = state->input.start_sector_ref;
            node->point = state->input.start;
            node->x28 = path_begin_get_zero();
            node->x30 = path_begin_get_zero();
            node->x2c = path_begin_max_cost;
            node->x34 = path_begin_get_zero();
            node->total_cost_estimate = distance;
            node->quantized_cost_estimate = (short)cost;
            node->x3e = 0;
            node->flag_c = (start_sector->flags & 0x1000) != 0;
            node->flag_d = (start_sector->flags & 0x3c0) || (start_sector->flags & 0x20);
            node->source.type = NONE;
            node->source.index = -1;
            if (state->has_goal)
            {
                state->best_node_index = node_index;
                state->best_x98 = distance;
                state->best_point = state->input.start;
                state->best_x9c = distance;
            }
            state->hash_buckets[(node->sector_ref.sector_index & 0x3ff) << 3] = node_index;
            path_begin_heap_insert(state, node_index, (short)cost);
            result = true;
            break;
        }
    }
    return result;
}


static __forceinline pathfinding_sector *path_sector_get(const pathfinding_data *data, short index)
{
    return &((struct pathfinding_sector *)data->sectors.address)[index];
}

static __forceinline bool path_sector_walkable(short flags)
{
    return (flags & 0x10) == 0 && (flags & 0x1000) == 0;
}

static __forceinline void path_link_set_segment(path_link *link, const real_point3d *start, const real_point3d *end, short reference_frame)
{
    link->point.point = *start;
    link->point.location[0] = reference_frame;
    link->point.location[1] = NONE;
    vector_from_points3d(start, end, &link->vector);
}

static __forceinline void path_link_set_stationary(path_link *link, short &link_count)
{
    link->vector = *global_zero_vector3d;
    link->narrow = true;
    link->allow_narrow = true;
    link_count++;
}
// @address 0x552180 ?build_path_links_for_sector@@YAFPBUpath_node@@PAUpath_link@@PAUpath_state@@@Z
short build_path_links_for_sector(const path_node *node, path_link *links, path_state *state)
{
    short structure_bsp_index = node->sector_ref.structure_bsp_index;
    pathfinding_data *data = pathfinding_data_get(structure_bsp_index);
    pathfinding_sector *sectors = (struct pathfinding_sector *)data->sectors.address;
    c_sector_ref sector_ref = node->sector_ref;
    short reference_frame = node->point.location[0];
    short link_count = 0;
    pathfinding_sector *current_sector = path_start_sector(sector_ref);
    bool walkable = path_sector_walkable(current_sector->flags);

    short hint_index = current_sector->first_hint_index;
    if (data->hints.count > 0)
    {
        while (hint_index != NONE && link_count < 100)
        {
            pathfinding_hint *hint = &((pathfinding_hint *)data->hints.address)[hint_index];
            if ((state->movement.hint_types & (1 << hint->type)) != 0)
            {
                switch (hint->type)
                {
                case 0:
                {
                    long link_index = hint->data.data[0];
                    sector_link *link = &((sector_link *)data->links.address)[link_index];
                    bool right_side = sector_ref.sector_index == link->sectors[1];
                    unsigned short adjacent_sector_index = link->sectors[!right_side];
                    long sector_index = adjacent_sector_index;
                    if (sector_index != NONE && sector_index != 0xffff)
                    {
                        path_link *plink = &links[link_count];
                        plink->index = link_index;
                        plink->sector_ref.sector_index = adjacent_sector_index;
                        plink->hint_type = NONE;
                        plink->structure_bsp_index = sector_ref.structure_bsp_index;
                        plink->sector_ref.structure_bsp_index = sector_ref.structure_bsp_index;
                        plink->sector_flags = sectors[sector_index].flags;
                        const real_point3d *start = TAG_BLOCK_GET_ELEMENT(&data->vertices, link->start_vertex, real_point3d);
                        const real_point3d *end = TAG_BLOCK_GET_ELEMENT(&data->vertices, link->end_vertex, real_point3d);
                        plink->point.set(start, reference_frame);
                        vector_from_points3d(start, end, &plink->vector);
                        plink->narrow = TEST_BIT(link->flags, 5);
                        plink->allow_narrow = true;
                        link_count++;
                    }
                    break;
                }
                case 9:
                {
                    short marker_structure_bsp_index = NONE;
                    long element_index = NONE;
                    long marker_sector_index = NONE;
                    if (pathfinding_find_seam_link(structure_bsp_index, hint_index, &marker_structure_bsp_index, &element_index, &marker_sector_index)
                        && marker_structure_bsp_index != NONE
                        && element_index != NONE && element_index != 0xffff
                        && marker_sector_index != NONE && marker_sector_index != 0xffff)
                    {
                        pathfinding_data *marker_data = pathfinding_data_get(marker_structure_bsp_index);
                        sector_link *link = &((sector_link *)marker_data->links.address)[element_index];
                        path_link *plink = &links[link_count];
                        plink->hint_type = NONE;
                        plink->index = element_index;
                        plink->structure_bsp_index = marker_structure_bsp_index;
                        unsigned short sector_index = (unsigned short)marker_sector_index;
                        plink->sector_ref.set(marker_structure_bsp_index, sector_index);
                        plink->sector_flags = path_sector_get(marker_data, (short)marker_sector_index)->flags;
                        const real_point3d *start = TAG_BLOCK_GET_ELEMENT(&marker_data->vertices, link->start_vertex, real_point3d);
                        const real_point3d *end = TAG_BLOCK_GET_ELEMENT(&marker_data->vertices, link->end_vertex, real_point3d);
                        plink->point.set(start, reference_frame);
                        vector_from_points3d(start, end, &plink->vector);
                        plink->narrow = false;
                        plink->allow_narrow = false;
                        link_count++;
                    }
                    break;
                }
                case 1:
                case 6:
                {
                    hint_jump_data *jump_data = &hint->data;
                    long vertex_index = hint->fields.index1;
                    if (vertex_index != NONE
                        && (state->movement.infinite_jump || (hint->fields.heights & state->movement.jump_heights) > 0)
                        && hint->fields.index4 != NONE && hint->fields.index4 != 0xffff)
                    {
                        long flags = hint->fields.flags;
                        if ((!(flags & 0x20) || (state->movement.hint_types & 8) != 0)
                            && (state->input.allow_vehicle_hints || !(flags & 8)))
                        {
                            bool valid = true;
                            real_point3d start;
                            real_point3d end;
                            c_sector_ref destination_sector_ref;
                            short hint_reference_frame;
                            short destination_reference_frame;
                            if (!ai_hint_get_jump_destination_rail(structure_bsp_index, jump_data, &node->point.point, &start, &end,
                                    &destination_sector_ref, &hint_reference_frame))
                            {
                                destination_reference_frame = hint_reference_frame;
                                valid = false;
                            }
                            else
                            {
                                destination_reference_frame = hint_reference_frame;
                                if (!(flags & 1) && (short)hint->fields.reference_frame != destination_reference_frame)
                                {
                                    real_point3d source_point;
                                    real_point3d destination_point;
                                    ai_reference_frame_transform_point((short)hint->fields.reference_frame,
                                        TAG_BLOCK_GET_ELEMENT(&data->vertices, vertex_index, real_point3d), &source_point);
                                    ai_reference_frame_transform_point(destination_reference_frame, &start, &destination_point);
                                    valid = false;
                                    for (short height = 0; height < 6; height++)
                                    {
                                        if ((state->movement.jump_heights & (1 << height)) != 0 &&
                                            ai_hint_test_jump(&source_point, 0, &destination_point, 0, height))
                                        {
                                            valid = true;
                                            break;
                                        }
                                        if ((hint->fields.heights & (1 << height)) != 0)
                                            break;
                                    }
                                }
                            }
                            if (valid)
                            {
                                path_link *plink = &links[link_count];
                                pathfinding_sector *destination_sector = path_start_sector(destination_sector_ref);
                                plink->hint_type = hint->type;
                                plink->index = hint_index;
                                plink->structure_bsp_index = structure_bsp_index;
                                plink->sector_ref = destination_sector_ref;
                                plink->sector_flags = destination_sector->flags;
                                plink->sector_flags |= 1;
                                plink->narrow = true;
                                plink->allow_narrow = true;
                                plink->point.set(&start, destination_reference_frame);
                                if (!(hint->fields.flags & 2))
                                    vector_from_points3d(&start, &end, &plink->vector);
                                else
                                    plink->vector = *global_zero_vector3d;
                                link_count++;
                            }
                        }
                    }
                    break;
                }
                case 5:
                {
                    if (hint->fields.index0 != NONE && hint->fields.index3 != NONE && hint->fields.index2 != NONE
                        && hint->fields.index1 != NONE && hint->fields.index1 != 0xffff
                        && (((state->movement.movement_hints & 0x400) && (hint->fields.flags & 1))
                            || ((state->movement.movement_hints & 0x800) && (hint->fields.flags & 2))
                            || ((state->movement.movement_hints & 0x1000) && (hint->fields.flags & 4))))
                    {
                        const real_point3d *point = TAG_BLOCK_GET_ELEMENT(&data->vertices, hint->fields.index3, real_point3d);
                        path_link *plink = &links[link_count];
                        plink->hint_type = 5;
                        plink->index = hint_index;
                        plink->structure_bsp_index = sector_ref.structure_bsp_index;
                        plink->sector_ref.set(sector_ref.structure_bsp_index, (unsigned short)hint->data.data[0]);
                        plink->sector_flags = sectors[(unsigned short)hint->data.data[0]].flags;
                        plink->point.set(point, reference_frame);
                        path_link_set_stationary(plink, link_count);
                    }
                    break;
                }
                case 8:
                {
                    if (hint->fields.index1 != NONE && hint->fields.index1 != 0xffff && (unsigned short)((unsigned long)hint->data.data[0] >> 16) != 0xffff)
                    {
                        const real_point3d *point = TAG_BLOCK_GET_ELEMENT(&data->vertices, hint->fields.index0, real_point3d);
                        path_link *plink = &links[link_count];
                        plink->hint_type = 8;
                        plink->index = hint_index;
                        plink->structure_bsp_index = sector_ref.structure_bsp_index;
                        plink->sector_ref.set(sector_ref.structure_bsp_index, (unsigned short)hint->data.data[0]);
                        plink->sector_flags = sectors[(unsigned short)hint->data.data[0]].flags;
                        plink->point.set(point);
                        path_link_set_stationary(plink, link_count);
                    }
                    break;
                }
                }
            }
            hint_index = hint->next_hint_index;
        }
    }

    if (link_count < 100
        && ((current_sector->flags & 1) || (current_sector->flags & 0x8000) || (link_count == 0 && node->parent_node_index == NONE)))
    {
        long link_index = current_sector->first_link_index;
        if (link_index != NONE)
        {
            do
            {
                sector_link *link = &((sector_link *)data->links.address)[link_index];
                bool right_side = sector_ref.sector_index == link->sectors[1];
                long adjacent_sector_index = link->sectors[!right_side];
                if (adjacent_sector_index != NONE && adjacent_sector_index != 0xffff
                    && ((unsigned short)link->hint_index == 0xffff || ai_link_hint_traversable(data, link_index)))
                {
                    path_link *plink = &links[link_count];
                    pathfinding_sector *adjacent_sector = &sectors[adjacent_sector_index];
                    plink->hint_type = NONE;
                    plink->index = link_index;
                    plink->structure_bsp_index = sector_ref.structure_bsp_index;
                    plink->sector_ref.set(sector_ref.structure_bsp_index, adjacent_sector_index);
                    plink->sector_flags = adjacent_sector->flags;
                    if (state->movement.hint_types & 4)
                    {
                        bool adjacent_walkable = (plink->sector_flags & 0x8000) && (adjacent_sector->flags & 0x10) == 0 && (plink->sector_flags & 0x1000) == 0;
                        if (adjacent_walkable == walkable)
                            plink->sector_flags |= 1;
                        else
                            plink->sector_flags &= ~1;
                    }
                    plink->narrow = TEST_BIT(link->flags, 5);
                    plink->allow_narrow = false;
                    const real_point3d *start = TAG_BLOCK_GET_ELEMENT(&data->vertices, link->start_vertex, real_point3d);
                    const real_point3d *end = TAG_BLOCK_GET_ELEMENT(&data->vertices, link->end_vertex, real_point3d);
                    plink->point.set(start, reference_frame);
                    vector_from_points3d(start, end, &plink->vector);
                    link_count++;
                }
                if (link_count == 100)
                    break;
                link_index = link->links[right_side];
            } while (link_index != current_sector->first_link_index);
        }
    }
    return link_count;
}

// @address 0x550a78 ?build_raw_path@@YAFJPAUpath_state@@PAUpath_result@@FFPAUpath_step@@PA_NPAFPAUpath_step_source@@F@Z
short build_raw_path(long unit_index, path_state *state, path_result *result, short node_index, short max_steps, path_step *steps,
    // match: 0x550a78: pointer __restrict qualifier prevents alias reload
    bool *out_finished, short *out_source_count, path_step_source *__restrict sources, short max_sources)
{
    (void)max_steps;
    (void)max_sources;
    c_sector_ref sector;
    real_vector3d diff;
    real_vector3d edge;
    short link_indices[40];
    long link_values[40];
    short prev_type = -1;
    path_node *prev = 0;
    short count = 0;
    bool finished = true;
    short source_count = 0;
    long depth = (long)state->node_list[node_index].x3e + 1;
    if (depth > 40)
        depth = 40;
    short step_count = (short)depth;
    if (step_count > 0)
    {
        short i = 0;
        do
        {
            steps[i].type = -1;
            steps[i].hint.clear();
            steps[i].flags = 0;
            link_values[i] = -1;
            link_indices[i] = -1;
            i++;
        } while (i < step_count);
    }
    count = 0;
    if (node_index != -1)
    {
        path_node *node;
        do
        {
            node = &state->node_list[node_index];
            if (source_count < 10)
            {
                if (node->sector_ref != NONE)
                {
                    long link_index = NONE;
                    pathfinding_data *data = pathfinding_data_get(node->sector_ref.structure_bsp_index);
                    bool has_link = false;
                    if ((unsigned short)node->source.type != 0xffff)
                        has_link = true;
                    else
                    {
                        link_index = node->source.index;
                        if (link_index >= 0 && link_index < data->links.count)
                            has_link = (_rotl((unsigned short)TAG_BLOCK_GET_ELEMENT(&data->links, link_index, sector_link)->flags, 27) & 1);
                    }
                    if (has_link)
                    {
                        sources[source_count] = node->source;
                        source_count++;
                    }
                }
            }
            if (node->x3e >= 40)
                finished = false;
            else
            {
                if (count == 0)
                    count = (short)(node->x3e + 1);
                steps[node->x3e].sector = node->sector_ref;
                if (node->sector_ref != NONE)
                {
                    sector = node->sector_ref;
                    pathfinding_data *data = pathfinding_data_get(sector.structure_bsp_index);
                    if ((TAG_BLOCK_GET_ELEMENT(&data->sectors, (short)sector.sector_index, pathfinding_sector)->flags & 0x10) == 0)
                        steps[node->x3e].flags |= 2;
                }
                if (prev_type == -1)
                    steps[node->x3e].point = result->destination.point;
                else
                {
                    steps[node->x3e].point = prev->point;
                    if ((unsigned short)prev->source.type == 0xffff)
                    {
                        link_values[node->x3e] = prev->source.index;
                        link_indices[node->x3e] = (unsigned short)prev->source.structure_bsp_index;
                    }
                }
                short hint = node->source.type;
                if (hint == 1 || hint == 6 || hint == 5)
                {
                    path_node *parent = &state->node_list[node->parent_node_index];
                    steps[node->x3e - 1].type = hint;
                    steps[node->x3e - 1].hint.set(node->source.structure_bsp_index, (short)node->source.index);
                    ai_hint_get_maneuver_point(unit_index, state->input.radius, node->source.index, &parent->point, parent->sector_ref,
                        &node->point, &parent->sector_ref, &steps[node->x3e - 1].point, &steps[node->x3e - 1].sector);
                }
                else if (hint == 8)
                {
                    steps[node->x3e - 1].type = 8;
                    steps[node->x3e - 1].hint.set(node->source.structure_bsp_index, (short)node->source.index);
                }
            }
            prev_type = node_index;
            prev = node;
            node_index = node->parent_node_index;
        } while (node_index != -1);
    }
    bool giant = unit_index != -1 ? actor_is_giant(datum_get<actor_datum>(tls_actor_data(), unit_index)) : false;
    float margin = state->input.radius * 1.2f;
    float limit;
    if (giant)
        limit = 300.0f;
    else
        limit = 40.0f;
    short total = count;
    if (total > 0)
    {
        short i = 0;
        do
        {
            pathfinding_data *data = 0;
            long link;
            unsigned short bsp;
            real_point3d *prev_point;
            c_ai_point3d *current_point;
            sector_link *edge_link;
            real_point3d *start_vertex;
            real_point3d *end_vertex;
            float length;
            float dot;
            float tmax;
            float hi;
            float t;
            link = link_values[i];
            if (link != -1 && link != 0xffff)
            {
                bsp = link_indices[i];
                if (bsp != 0xffff)
                {
                    if (i > 0)
                        prev_point = &steps[i - 1].point.point;
                    else
                        prev_point = &state->input.start.point;
                    current_point = &steps[i].point;
                    data = pathfinding_data_get((short)bsp);
                    edge_link = TAG_BLOCK_GET_ELEMENT(&data->links, link, sector_link);
                    if (edge_link->start_vertex != 0xffff && edge_link->end_vertex != 0xffff)
                    {
                        start_vertex = TAG_BLOCK_GET_ELEMENT(&data->vertices, edge_link->start_vertex, real_point3d);
                        end_vertex = TAG_BLOCK_GET_ELEMENT(&data->vertices, edge_link->end_vertex, real_point3d);
                        vector_from_points3d(start_vertex, end_vertex, &edge);
                        vector_from_points3d(start_vertex, prev_point, &diff);
                        length = normalize3d(&edge);
                        if (length > margin * 2.0f)
                        {
                            dot = dot_product3d(&edge, &diff);
                            tmax = dot > margin ? dot : margin;
                            hi = length - margin;
                            if (tmax < hi)
                            {
                                if (dot > margin)
                                    t = dot;
                                else
                                    t = margin;
                            }
                            else
                                t = hi;
                            point_from_line3d(start_vertex, &edge, t, &current_point->point);
                        }
                    }
                    if (ai_distance_squared3d(&state->input.start, current_point) > limit * limit && i < total - 1)
                    {
                        finished = false;
                        count = (short)(i + 1);
                    }
                }
            }
            total = count;
            i = (short)(i + 1);
        } while (i < total);
    }
    if (out_finished)
        *out_finished = finished;
    if (out_source_count)
        *out_source_count = source_count;
    return count;
}

static __forceinline actor_datum *path_build_actor_get(long actor_index)
{
    return datum_get<actor_datum>(tls_actor_data(), actor_index);
}

static __forceinline long path_build_seconds_to_ticks_round(float seconds)
{
    return (long)((float)tls_game_time_globals()->ticks_per_second * seconds + 0.5f);
}

static __forceinline bool path_build_flag_test(short flags, long bit)
{
    return (flags & (1 << bit)) != 0;
}

// @address 0xcd968 ?path_state_build_path@@YA_NJPAUpath_state@@PAUpath_result@@@Z
bool path_state_build_path(long actor_index, path_state *state, path_result *result)
{
    actor_datum *actor = 0;
    if (actor_index != NONE)
        actor = path_build_actor_get(actor_index);
    result->success = false;
    result->error = state->error;
    result->x2 = false;
    if (state->has_goal)
    {
        short node_index = path_node_from_hash_table(state, state->goal.sector);
        if (node_index != NONE)
        {
            result->destination = state->goal;
            result->destination.distance = 0.0f;
        }
        else if (state->best_x98 < state->goal.distance)
        {
            node_index = state->best_node_index;
            path_node *node = path_state_get_node(state, node_index);
            result->destination.set(&state->best_point, &node->sector_ref, state->best_x98);
        }
        if (node_index != NONE)
        {
            path_step_source sources[10];
            short avoided_step_count;
            bool steps_finish_path = true;
            bool success = true;
            path_step avoided_steps[4];
            short raw_step_count = 0;
            path_step smoothed_steps[4];
            path_step raw_steps[40];
            short source_count;
            short smoothed_step_count = 0;
            avoided_step_count = 0;
            source_count = 0;
            raw_step_count = build_raw_path(actor_index, state, result, node_index, 40, raw_steps, &steps_finish_path, &source_count, sources, 10);
            path_smooth(state, raw_step_count, raw_steps, &smoothed_step_count, smoothed_steps, &steps_finish_path);
            if (actor && (actor_is_giant(path_build_actor_get(actor_index)) || actor->ignore_obstacles))
            {
                avoided_step_count = (short)(smoothed_step_count > 4 ? 4 : smoothed_step_count);
                memcpy(avoided_steps, smoothed_steps, avoided_step_count * sizeof(path_step));
            }
            else
            {
                long ignorable_object_index = NONE;
                short ignorable_flags = 0;
                bool allow_partial = state->another_flag;
                if (!allow_partial && result->error.a > 0)
                    allow_partial = (result->error.a / (1 << result->error.b)) % 2 == 1;
                success = obstacles_find_avoidance_path(actor_index, state, smoothed_step_count, smoothed_steps, allow_partial,
                    &avoided_step_count, avoided_steps, &steps_finish_path, &ignorable_object_index, &ignorable_flags, &result->x2);
                if (success)
                {
                    if (path_build_flag_test(ignorable_flags, 7))
                    {
                        if (actor)
                        {
                            actor->movement_obstacle_index = ignorable_object_index;
                            actor->movement_obstacle_ticks = (short)path_build_seconds_to_ticks_round(1.0f);
                        }
                        result->ignorable_object_index = NONE;
                        result->x3c = 0;
                    }
                    else
                    {
                        result->ignorable_object_index = ignorable_object_index;
                        result->x3c = ignorable_flags;
                    }
                    if (ignorable_object_index != NONE && actor_index != NONE && path_build_flag_test(ignorable_flags, 6))
                        stimulus_register(actor_index, _behavior_stimulus_destructible_obstacle, (short)path_build_seconds_to_ticks_round(0.2f), 3);
                }
                else
                {
                    result->ignorable_object_index = NONE;
                    result->x3c = 0;
                }
            }
            if (result->error.a == 0)
            {
                if (source_count > 0)
                    result->error.links[0] = sources[0];
                if (source_count > 1)
                    result->error.links[1] = sources[source_count - 1];
                if (source_count > 2)
                    result->error.links[2] = sources[source_count / 2];
                result->error.b = (short)MIN(MAX(source_count, 0), 3);
            }
            if (success)
            {
                result->success = true;
                result->final_point = steps_finish_path;
                result->x3f = 0;
                result->count = (char)avoided_step_count;
                result->start_point = state->input.start;
                memcpy(result->points, avoided_steps, avoided_step_count * sizeof(path_step));
                if (result->final_point)
                {
                    if (result->count > 0)
                    {
                        result->destination.point = result->points[result->count - 1].point;
                        result->destination.sector = result->points[result->count - 1].sector;
                    }
                    else
                    {
                        result->destination.point = state->input.start;
                        result->destination.sector = state->input.start_sector_ref;
                    }
                    result->destination.distance = sqrtf(ai_distance_squared3d(&result->destination.point, &state->goal.point));
                }
            }
            else
                path_error_stage_advance_for_find(&result->error);
        }
    }
    return result->success;
}
