// blamlibxbox/animation_definitions.obj: from the records 0xe0028 0xe01e0 0xe0520 0xe09e8 0xe2080 0xe20f8 0xe2180 0xe8850 0xe88e0 0xe9830 0xeda90 0xeddc0 0x101fd0 0x1021d0 0x10a9f8 0x10f2f8 0x10f950 0x120980 0x120a90 0x1214b0 0x2fc010 0x2fc408 0x2fc468 0x2fc4c8 0x2fc658 0x2fc740 0x2fc828 0x2fc8d0 0x2fcb30 0x2fcbb8 0x2fcc58
// @compiler xdk6130
// @flags /TP /O2
#include "math/matrix_math_types.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "animations/animation_definition_storage.h"
// @address 0x74b898 g_geometry_globals
// @address 0x2251c k_real_zero_storage
// @address 0x3aaa8 k_real_math_identity_orientation_storage
// @address 0x3aac8 k_real_math_identity_matrix_storage
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include <string.h>
#include "math/real_math.h"
#include "math/random_math_range.h"
#include "animations/animation_definitions_internal.h"
#include "animations/animation_definition_types.h"
#include "animations/mixing_board/channels/channel_base.h"
#include "math/matrix_math.h"
#include "animations/animation_id.h"
#include "cache/tag_resource_cache_thread_lock.h"
#include "animations/mixing_board/channels/anim_computation_flags_types.h"
#include "geometry/geometry_block_types.h"
// @rdata 0x8203af7c
static const float animation_definitions_frame_rate = 30.0f;
// @rdata 0x8203aed4
static const float animation_definitions_minimum_time = 0.033333335f;
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0xe2220 ?valid@c_animation_data@@QBA_NXZ
// @address 0x9d068 ?matrix4x3_from_point_and_quaternion@@YAXPAUreal_matrix4x3@@PBTreal_point3d@@PBUreal_quaternion@@@Z
// @address 0xe8a20 ?get_next_weapon_type@c_animation_search_path_iterator@@QAAPBVc_weapon_type@@PAJ00@Z
#include "cseries/crt_xdk.h"
#include "cseries/cseries.h"
#include <math.h>

#include "tag_files/tag_groups.h"
#include "memory/datum_index.h"
#include "animations/animation_inheritence.h"
struct s_animation_event_reference { s_tag_reference reference; unsigned short flags, pad2; };
class c_weapon_class;
class c_animation_mode;
class c_weapon_type;
class ik_point_iterator;
struct animation_aiming_screen_bounds;
class c_model_animation_tag_resource_member;
class c_animation_data;
#include "animations/animation_manager.h"

#include "animations/animation_graph_node_types.h"
struct render_model_definition;
#include "animations/animation_node_orientations.h"
#include "animations/animation_search_path_iterator.h"
struct s_animation_blend_screen { long name; animation_aiming_screen_bounds aiming_screen; };
struct s_foot_tracking_member {
    long foot_marker;
    real_bounds foot_ik_range;
    long ankle_marker;
    real_bounds ankle_ik_range;
    short defaults;
    short pad26;
};
struct s_mode_foot_tracking { short member; short defaults; };

struct s_transition_destination {
    long name;
    long mode;
    long state;
    char type;
    char pad13;
    unsigned char direction;
    unsigned char region;
    c_animation_id animation;
};
struct s_transition_source {
    long name;
    long state;
    short pad8;
    unsigned char direction;
    unsigned char region;
    s_tag_block destinations;
};

struct s_ik_point { long marker; long attach_to_marker; };
#include "animations/ik_point_iterator.h"

#include "animations/animation_data.h"
#include "math/random_math.h"
class c_model_animation_tag_resource_member
{
public:
    long name;
    long checksum;
    short frame_count;
    unsigned char node_count;
    signed char movement_data_type;
    c_animation_data_sizes data_sizes;
    s_tag_data animation_data;
};
typedef char animation_resource_member_size[(sizeof(c_model_animation_tag_resource_member) == 0x30) ? 1 : -1];
typedef char animation_resource_member_data_offset[(offsetof(c_model_animation_tag_resource_member, animation_data) == 0x1c) ? 1 : -1];
class c_model_animation_tag_resource_group { public: long member_count; long resource_handle; long definition; };
struct s_animation_resource_data { s_tag_block members; };

// Damage-type index to animation-name string id.
// @address 0x73e708 damage_type_string_id_list
long damage_type_string_id_list[4] = { 0x93, 0x94, 0x95, 0x96 };

static __forceinline const c_model_animation_graph *definition_get_child(const c_model_animation_graph *definition, long index)
{
  if (index == -1)
    return definition;
  c_model_animation_graph *result = 0;
  if (index < definition->m_inheritence_list.count) {
    long tag_index = (*TAG_BLOCK_GET_ELEMENT(&definition->m_inheritence_list, index, s_animation_inheritence)).reference.index;
    if (tag_index != -1)
      result = (c_model_animation_graph *)tag_get(0, tag_index);
  }
  return result;
}
static __forceinline const c_model_animation *definition_get_part(const c_model_animation_graph *definition, long index)
{
  c_model_animation *result = 0;
  if (index != -1)
    result = &((c_model_animation *)definition->m_animations.address)[index];
  return result;
}

__forceinline const c_model_animation_graph *c_model_animation_graph::get_inherited_graph(long index) const
{
    const c_model_animation_graph *result = 0;
    if (index < m_inheritence_list.count) {
        const s_animation_inheritence *child = TAG_BLOCK_GET_ELEMENT(&m_inheritence_list, index, s_animation_inheritence);
        if (child->reference.index != -1)
            result = (const c_model_animation_graph *)tag_get(0, child->reference.index);
    }
    return result;
}
__forceinline const c_model_animation *c_model_animation_graph::get_local_animation(long index) const
{
    const c_model_animation *result = 0;
    if (index != -1)
        result = &((const c_model_animation *)m_animations.address)[index];
    return result;
}
__forceinline const animation_aiming_screen_bounds *c_model_animation_graph::get_local_blend_screen_bounds(long index) const
{
    const s_animation_blend_screen *screen = TAG_BLOCK_GET_ELEMENT(&m_blend_screens, index, s_animation_blend_screen);
    return &screen->aiming_screen;
}
static __forceinline const s_animation_resource_data *animation_resource_get(long handle) {
    s_geometry_storage_member *member = &g_geometry_globals.manager->m_resource_cache.m_geometry_storage;
    s_geometry_second_base *base = member;
    const c_tag_resource_cache_file_access_cache *cache = base->m_per_thread_access_cache[get_current_thread_index()];
    return static_cast<const s_animation_resource_data *>(cache->m_resource_values[datum_index_to_absolute_index(handle)]);
}

void c_model_animation_graph::get_graph_and_animation(
    c_animation_id reference, const c_model_animation_graph **out_definition, const c_model_animation **out_part) const
{
  const c_model_animation *part = 0;
  const c_model_animation_graph *child = 0;
  if (reference.m_animation_index >= 0) {
    child = definition_get_child(this, reference.m_graph_index);
    if (child)
      part = definition_get_part(child, reference.m_animation_index);
  }
  if (out_definition)
    *out_definition = child;
  if (out_part)
    *out_part = part;
}

// @address 0xe01e0 ?compute_relative_frame_index@@YAMPBVc_model_animation@@0M@Z
float compute_relative_frame_index(const c_model_animation *source, const c_model_animation *destination, float frame)
{
    float frame_ratio = (float)destination->frame_count / (float)source->frame_count;
    float last_frame = destination->get_last_frame_index();
    float result = MIN(MAX(frame * frame_ratio, k_real_zero), last_frame);
    return result;
}

float c_model_animation::get_playback_ratio_from_frame_position(float frame) const
{
    float result = k_real_one;
    float last_frame = (float)(frame_count - 1);
    if (last_frame > k_real_zero)
        result = MIN(MAX(frame / last_frame, k_real_zero), k_real_one);
    return result;
}

static __forceinline const s_animation_event_reference *animation_definitions_sound_reference(const c_model_animation_graph *graph, long index)
{
  const s_animation_event_reference *result = 0;
  if (index != -1 && index >= 0 && index < graph->m_sound_references.count)
    result = ((const s_animation_event_reference *)graph->m_sound_references.address) + index;
  return result;
}
static __forceinline const s_animation_event_reference *animation_definitions_effect_reference(const c_model_animation_graph *graph, long index)
{
  const s_animation_event_reference *result = 0;
  if (index != -1 && index >= 0 && index < graph->m_effect_references.count)
    result = ((const s_animation_event_reference *)graph->m_effect_references.address) + index;
  return result;
}
// @address 0xe0520 check_animation_frame_range_for_events
unsigned long check_animation_frame_range_for_events(const c_model_animation_graph *g, const c_model_animation *a,
    s_animation_playback_flags *play, float start, float end, s_animation_event_callback callback, long context)
{
  unsigned long mask = 0;
  // match: 0xe0520: preserve the write summary so callers reload playback flags.
  play->storage = play->storage;
  float lower = start > end ? end : start;
  float upper = start > end ? start : end;
  if (!play->test(7)) {
    long total = a->frame_events.begin() ? a->frame_events.count() : 0;
    for (long i = 0; i < total; ++i) {
      const s_frame_event *event = a->frame_events.begin() + i;
      float frame = (float)event->frame;
      if (frame >= lower && frame < upper) {
        mask |= 1 << event->type;
        if (callback) {
          s_animation_range_event_data data;
          data.type = 0;
          data.frame = event->frame;
          data.u.key.index = (short)i;
          data.u.key.type = event->type;
          callback(&data, context);
        }
      }
    }
  }
  if (!play->test(8)) {
    long total = (const s_animation_asset_event *)a->sound_events.address ? a->sound_events.count : 0;
    for (long i = 0; i < total; ++i) {
      const s_animation_asset_event *event = (const s_animation_asset_event *)a->sound_events.address + i;
      float frame = (float)event->frame;
      if (frame >= lower && frame < upper) {
        const s_animation_event_reference *ref = animation_definitions_sound_reference(g, event->reference);
        if (ref && ref->reference.index != -1 && callback) {
          if ((!(ref->flags & 2) || play->test(13)) && (!(ref->flags & 4) || play->test(12))) {
            bool valid = true;
            if ((ref->flags & 16) && start > end)
              valid = false;
            if ((ref->flags & 32) && start < end)
              valid = false;
            if (valid) {
              s_animation_range_event_data data;
              data.type = 1;
              data.frame = event->frame;
              data.u.asset.marker = event->marker;
              data.u.asset.flags = ref->flags;
              data.u.asset.asset = ref->reference.index;
              callback(&data, context);
            }
          }
        }
      }
    }
    if (callback) {
      long total_dialogues = (const s_animation_dialogue_event *)a->dialogue_events.address ? a->dialogue_events.count : 0;
      for (long i = 0; i < total_dialogues; ++i) {
        const s_animation_dialogue_event *event = (const s_animation_dialogue_event *)a->dialogue_events.address + i;
        float frame = (float)event->frame;
        if (frame >= lower && frame < upper) {
          s_animation_range_event_data data;
          data.type = 3;
          data.frame = event->frame;
          data.u.dialogue = event->dialogue;
          callback(&data, context);
        }
      }
    }
  }
  if (!play->test(9)) {
    long total = (const s_animation_asset_event *)a->effect_events.address ? a->effect_events.count : 0;
    for (long i = 0; i < total; ++i) {
      const s_animation_asset_event *event = (const s_animation_asset_event *)a->effect_events.address + i;
      float frame = (float)event->frame;
      if (frame >= lower && frame < upper) {
        const s_animation_event_reference *ref = animation_definitions_effect_reference(g, event->reference);
        if (ref && ref->reference.index != -1) {
          if ((!(ref->flags & 2) || play->test(13)) && (!(ref->flags & 4) || play->test(12))) {
            bool valid = true;
            if ((ref->flags & 16) && start > end)
              valid = false;
            if ((ref->flags & 32) && start < end)
              valid = false;
            if (valid && callback) {
              s_animation_range_event_data data;
              data.type = 2;
              data.frame = event->frame;
              data.u.asset.marker = event->marker;
              data.u.asset.flags = ref->flags;
              data.u.asset.asset = ref->reference.index;
              callback(&data, context);
            }
          }
        }
      }
    }
  }
  return mask;
}

// 0xe2080
// @address 0xe2080 animation_definitions_try_and_get_data
bool animation_definitions_try_and_get_data(const c_channel_base *channel, c_animation_data *out_data)
{
    bool result = false;
    if (channel->valid()) {
        const c_model_animation_graph *definition = (const c_model_animation_graph *)tag_get(0, channel->get_graph_index());
        if (definition)
            result = definition->get_animation_data(channel->get_animation_id(), out_data);
    }
    return result;
}

// @address 0xe20f8 ?get_animation_data@c_model_animation_graph@@QBA_NVc_animation_id@@PAVc_animation_data@@@Z
bool c_model_animation_graph::get_animation_data(c_animation_id reference, c_animation_data *out_data) const
{
    bool result = false;
    if (reference.valid()) {
        const c_model_animation_graph *graph = 0;
        const c_model_animation *animation = 0;
        get_graph_and_animation(reference, &graph, &animation);
        if (graph && animation)
            result = graph->get_local_animation_data(animation, out_data);
    }
    return result;
}

// @address 0xe2180 ?get_local_animation_data@c_model_animation_graph@@QBA_NPBVc_model_animation@@PAVc_animation_data@@@Z
bool c_model_animation_graph::get_local_animation_data(const c_model_animation *animation, c_animation_data *out_data) const
{
    bool result = false;
    const c_model_animation_tag_resource_member *member = get_local_resource_group_member(animation->resource_group, animation->resource_group_member);
    if (member && member->animation_data.address) {
        out_data->setup((const unsigned char *)member->animation_data.address, animation->node_count,
            (signed char)animation->movement_type, (short)animation->frame_count,
            member ? &member->data_sizes : 0);
        result = out_data->valid();
    }
    return result;
}

// binary search of a tag block sorted by the int at the start of each element
void *find_named_block_entry_with_size(const s_tag_block *block, long key, long element_size)
{
    void *result = 0;
    int low = 0;
    bool done = false;
    int high = block->count - 1;
    while (!result) {
        if (done || high < low)
            break;
        int middle = (high + low) >> 1;
        done = low == high;
        int *element = (int *)tag_block_get_element_with_size(block, middle, element_size);
        if (*element == key)
            result = element;
        else if (*element > key)
            high = middle - 1;
        else if (*element < key)
            low = middle + 1;
    }
    return result;
}

// 0xe88e0
c_animation_id c_model_animation_graph::find_action(
    long mode, long weapon_class, long weapon_type, long action, long *out_mode, long *out_class, long *out_type) const
{
    c_animation_id result;
    result.clear();
    if (weapon_class != -1) {
        long result_mode = -1;
        long result_class = -1;
        long result_type = -1;
        c_animation_search_path_iterator iterator;
        iterator.begin(this, mode, weapon_class, weapon_type);
        const c_weapon_type *type = iterator.get_next_weapon_type(&result_mode, &result_class, &result_type);
        while (!result.valid() && type) {
            result = type->find_action(action);
            if (!result.valid())
                type = iterator.get_next_weapon_type(&result_mode, &result_class, &result_type);
        }
        if (out_mode)
            *out_mode = result_mode;
        if (out_class)
            *out_class = result_class;
        if (out_type)
            *out_type = result_type;
    }
    return result;
}

// @address 0xe9830 animation_definitions_compute_blend_and_playback
// match: 0x2fc8d0: preserve the axis-dot accumulation order.
static __forceinline float animation_definitions_dot_product3d_kfirst(const real_vector3d *a, const real_vector3d *b)
{
    float t = a->j * b->j;
    return a->k * b->k + t + a->i * b->i;
}
extern "C" void animation_definitions_compute_blend_and_playback(const c_model_animation_graph *graph, c_animation_id first_id,
    // match: 0xe9830: keeps the second_id save (mr r29,r5 + stw) above the first_id validity lhz and pins the tail *playback stores
    c_animation_id second_id, const real_vector3d *velocity, bool limited, float * blend, float *__restrict playback)
{
    *blend = 0.0f;
    *playback = 1.0f;
    if (first_id.valid() && second_id.valid())
    {
        const c_model_animation *first = graph->get_animation(first_id);
        const c_model_animation *second = graph->get_animation(second_id);
        if (first->movement_type && second->movement_type)
        {
            float slow_speed = second->motion_distance * 30.0f;
            float fast_speed = first->motion_distance * 30.0f;
            if (slow_speed > 0.0001f && fast_speed > slow_speed + 0.0001f)
            {
                float speed = (float)fabs(dot_product3d(velocity, &first->motion_direction));
                if (limited)
                    speed = PIN(speed, slow_speed * 0.15f, slow_speed);
                else
                    speed = PIN(speed, slow_speed * 0.15f, fast_speed * 1.2f);
                float frame_ratio = (float)first->frame_count / (float)second->frame_count;
                float blend_ratio;
                float playback_ratio;
                if (speed <= slow_speed)
                {
                    blend_ratio = 1.0f;
                    playback_ratio = frame_ratio * (speed / slow_speed);
                }
                else if (speed <= fast_speed)
                {
                    float t = 1.0f - (speed - slow_speed) / (fast_speed - slow_speed);
                    blend_ratio = t * t;
                    playback_ratio = 1.0f - (1.0f - frame_ratio) * blend_ratio;
                }
                else
                {
                    blend_ratio = 0.0f;
                    playback_ratio = speed / fast_speed;
                }
                playback_ratio = PIN(playback_ratio, 0.25, 1.2f);
                blend_ratio = PIN(blend_ratio, 0.0, 1.0f);
                *blend = blend_ratio;
                *playback = playback_ratio;
            }
        }
    }
}

float c_model_animation::get_frame_position_from_playback_ratio(float fraction) const
{
    float last = (float)(frame_count - 1);
    float scaled = last * fraction;
    return MIN(MAX(scaled, k_real_zero_storage), last);
}

// 0xeddc0
const animation_aiming_screen_bounds *c_model_animation_graph::get_blend_screen_bounds(c_animation_id reference) const
{
    const animation_aiming_screen_bounds *result = 0;
    const c_model_animation_graph *graph = this;
    if (reference.valid()) {
        if (reference.m_graph_index != -1)
            graph = get_inherited_graph(reference.m_graph_index);
        const c_model_animation *animation = graph->get_local_animation(reference.m_animation_index);
        if (animation->blend_screen != -1)
            result = graph->get_local_blend_screen_bounds(animation->blend_screen);
    }
    return result;
}

__forceinline const c_model_animation_tag_resource_member *c_model_animation_graph::get_resource_group_member(c_animation_id reference) const
{
    const c_model_animation *animation = 0;
    const c_model_animation_graph *graph = 0;
    get_graph_and_animation(reference, &graph, &animation);
    return graph->get_local_resource_group_member(animation->resource_group, animation->resource_group_member);
}

// 0x1021d0
// @address 0x1021d0 animation_definitions_animation_has_variation_chain
bool animation_definitions_animation_has_variation_chain(const c_model_animation_graph *definition, c_animation_id reference)
{
    bool result = false;
    if (reference.valid()) {
        const c_model_animation *animation = definition->get_animation(reference);
        result = animation->parent_animation_index != 0xffff || animation->next_animation_index != 0xffff;
    }
    return result;
}

static __forceinline c_animation_id animation_definitions_find_top_of_variation_chain(const c_model_animation_graph *definition, c_animation_id reference)
{
    if (animation_definitions_animation_has_variation_chain(definition, reference)) {
        const c_model_animation *animation = (const c_model_animation *)definition->get_animation(reference);
        if (animation->parent_animation_index != 0xffff)
            reference.m_animation_index = (short)animation->parent_animation_index;
    }
    return reference;
}
// @address 0x101fd0 animation_definitions_find_random_variation
c_animation_id *animation_definitions_find_random_variation(
    c_animation_id *out, const c_model_animation_graph *definition, c_animation_id reference, bool deterministic)
{
    if (reference.valid()) {
        if (animation_definitions_animation_has_variation_chain(definition, reference)) {
            const c_model_animation_graph *local_graph = definition;
            if (reference.m_graph_index != -1)
                local_graph = definition->get_inherited_graph(reference.m_graph_index);
            reference = animation_definitions_find_top_of_variation_chain(definition, reference);
            const c_model_animation *animation = (const c_model_animation *)local_graph->get_local_animation(reference.m_animation_index);
            if (animation->weight < 1.0f) {
                float random;
                if (deterministic)
                    random = (float)(random_seed_next(tls_random_seed()) >> 16) * 1.5259022e-05f;
                else
                    random = (float)(random_seed_next(get_local_random_seed_address()) >> 16) * 1.5259022e-05f;
                while (animation->next_animation_index != 0xffff) {
                    if (random <= animation->weight)
                        break;
                    reference.m_animation_index = (short)animation->next_animation_index;
                    animation = (const c_model_animation *)local_graph->get_local_animation((short)animation->next_animation_index);
                }
            }
        }
    }
    *out = reference;
    return out;
}

// 0x10a9f8
bool c_model_animation_graph::animation_contains_movement_data(c_animation_id reference) const
{
    const c_model_animation_tag_resource_member *resource_member = get_resource_group_member(reference);
    return resource_member->data_sizes.contains_movement_data();
}

// 0x10f2f8
bool c_model_animation_graph::find_next_mode_ik_point(ik_point_iterator *iterator) const
{
    long next_index = iterator->index + 1;
    bool done = false;
    while (!done) {
        const c_animation_mode *mode = 0;
        while (iterator->mode < 2 && !mode) {
            switch (iterator->mode) {
            case 0: mode = find_mode(iterator->mode_name); break;
            case 1: mode = find_mode(200); break;
            }
            if (!mode) {
                ++iterator->mode;
                next_index = 0;
            }
        }
        if (mode) {
            if (next_index < mode->m_ik_points.count) {
                const s_ik_point *point = TAG_BLOCK_GET_ELEMENT(&mode->m_ik_points, next_index, s_ik_point);
                iterator->marker = point->marker;
                iterator->attach_to_marker = point->attach_to_marker;
                iterator->index = (short)next_index;
                return true;
            } else {
                ++iterator->mode;
            }
        } else {
            done = true;
        }
    }
    return false;
}

// @address 0x10f950 ?find_next_weapon_ik_point@c_model_animation_graph@@QBA_NPAVik_point_iterator@@@Z
// match: 0x10f950: the first three search cases inline; the fallback keeps the shared call.
bool c_model_animation_graph::find_next_weapon_ik_point(ik_point_iterator *iterator) const
{
    long next_index = iterator->index + 1;
    bool done = false;
    while (!done) {
        const c_weapon_class *weapon_class = 0;
        while (iterator->mode < 4 && !weapon_class) {
            switch (iterator->mode) {
            case 0:
            {
                const c_weapon_class *found_class = 0;
                const c_animation_mode *mode = find_mode(iterator->mode_name);
                if (mode)
                    found_class = mode->find_class(iterator->weapon_class_name);
                weapon_class = found_class;
                break;
            }
            case 1:
            {
                const c_weapon_class *found_class = 0;
                const c_animation_mode *mode = find_mode(iterator->mode_name);
                if (mode)
                    found_class = mode->find_class(200);
                weapon_class = found_class;
                break;
            }
            case 2:
            {
                const c_weapon_class *found_class = 0;
                const c_animation_mode *mode = find_mode(200);
                if (mode)
                    found_class = mode->find_class(iterator->weapon_class_name);
                weapon_class = found_class;
                break;
            }
            case 3: weapon_class = find_weapon_class(200, 200); break;
            }
            if (!weapon_class) {
                ++iterator->mode;
                next_index = 0;
            }
        }
        if (weapon_class) {
            if (next_index < weapon_class->m_ik_points.count) {
                const s_ik_point *point = TAG_BLOCK_GET_ELEMENT(&weapon_class->m_ik_points, next_index, s_ik_point);
                iterator->marker = point->marker;
                iterator->attach_to_marker = point->attach_to_marker;
                iterator->index = (short)next_index;
                return true;
            } else {
                ++iterator->mode;
            }
        } else {
            done = true;
        }
    }
    return false;
}

// @address 0x120980 ?find_transition@c_model_animation_graph@@QBA?AVc_animation_id@@JJJJDDJJDDD@Z
c_animation_id c_model_animation_graph::find_transition(long mode, long weapon_class, long weapon_type, long state, char direction,
    char region, long destination_mode, long destination_state, char destination_direction, char destination_region, char type) const
{
    c_animation_id result;
    result.clear();
    if (weapon_class != -1) {
        long result_mode = -1;
        long result_class = -1;
        long result_type = -1;
        c_animation_search_path_iterator iterator;
        iterator.begin(this, mode, weapon_class, weapon_type);
        const c_weapon_type *weapon = iterator.get_next_weapon_type(&result_mode, &result_class, &result_type);
        while (!result.valid() && weapon) {
            result = weapon->find_transition(state, -1, -1, destination_mode, destination_state, -1, -1, type);
            if (!result.valid())
                weapon = iterator.get_next_weapon_type(&result_mode, &result_class, &result_type);
        }
    }
    return result;
}

// 0x120a90
c_animation_id c_weapon_type::find_transition(long state, char direction, char region, long destination_mode, long destination_state,
    char destination_direction, char destination_region, char type) const
{
    c_animation_id result;
    result.clear();
    for (long source_index = 0; source_index < m_transitions.count; ++source_index) {
        const s_transition_source *source = TAG_BLOCK_GET_ELEMENT(&m_transitions, source_index, s_transition_source);
        if (source->state == state && source->direction == 255 && source->region == 255) {
            for (long destination_index = 0; destination_index < source->destinations.count; ++destination_index) {
                const s_transition_destination *destination = TAG_BLOCK_GET_ELEMENT(&source->destinations, destination_index, s_transition_destination);
                if (destination->mode == destination_mode && destination->state == destination_state && destination->direction == 255 &&
                    destination->region == 255 && destination->type == type)
                {
                    return destination->animation;
                }
            }
        }
    }
    return result;
}

// @address 0x1214b0 ?find_overlay@c_model_animation_graph@@QBA?AVc_animation_id@@JJJJPAJ00@Z
c_animation_id c_model_animation_graph::find_overlay(
    long mode, long weapon_class, long weapon_type, long overlay, long *out_mode, long *out_class, long *out_type) const
{
    c_animation_id result;
    result.clear();
    if (mode == -1)
        mode = 200;
    if (weapon_class == -1)
        weapon_class = 200;
    if (weapon_type == -1)
        weapon_type = 200;
    long result_mode = -1;
    long result_class = -1;
    long result_type = -1;
    c_animation_search_path_iterator iterator;
    iterator.begin(this, mode, weapon_class, weapon_type);
    const c_weapon_type *type = iterator.get_next_weapon_type(&result_mode, &result_class, &result_type);
    while (!result.valid() && type) {
        result = type->find_overlay(overlay);
        if (!result.valid())
            type = iterator.get_next_weapon_type(&result_mode, &result_class, &result_type);
    }
    return result;
}

// @address 0x2fc408 ?get_animation@c_model_animation_graph@@QBAPBVc_model_animation@@Vc_animation_id@@@Z
const c_model_animation *c_model_animation_graph::get_animation(c_animation_id reference) const
{
  const c_model_animation *part = 0;
  if (reference.valid())
    get_graph_and_animation(reference, 0, &part);
  return part;
}
// 0x2fc468
const c_weapon_class *c_model_animation_graph::find_weapon_class(long mode_name, long class_name) const
{
    const c_weapon_class *result = 0;
    const c_animation_mode *mode = find_mode(mode_name);
    if (mode)
        result = mode->find_class(class_name);
    return result;
}

// @address 0x2fc4c8 ?find_damage_animation@c_model_animation_graph@@QBA?AVc_animation_id@@JJJJJJPAJ00@Z
c_animation_id c_model_animation_graph::find_damage_animation(long mode, long weapon_class, long weapon_type, long damage, long direction,
    long region, long *out_mode, long *out_class, long *out_type) const
{
    c_animation_id result;
    result.clear();
    if (weapon_class != -1) {
        long result_mode = -1;
        long result_class = -1;
        long result_type = -1;
        c_animation_search_path_iterator iterator;
        iterator.begin(this, mode, weapon_class, weapon_type);
        const c_weapon_type *type = iterator.get_next_weapon_type(&result_mode, &result_class, &result_type);
        while (!result.valid() && type) {
            result = type->find_damage_animation(damage, direction, region);
            if (!result.valid())
                type = iterator.get_next_weapon_type(&result_mode, &result_class, &result_type);
        }
        if (out_mode)
            *out_mode = result_mode;
        if (out_class)
            *out_class = result_class;
        if (out_type)
            *out_type = result_type;
    }
    return result;
}

// 0x2fc658
 e_foot_tracking_state c_model_animation_graph::find_foot_tracking_default_state(long member_index, long mode_name) const
{
    long defaults = 0;
    e_foot_tracking_state result = _foot_tracking_none;
    long search_mode = mode_name;
    bool found = false;
    for (long attempt = 0; !found && attempt < 2; ++attempt) {
        const c_animation_mode *mode = find_mode(search_mode);
        if (mode) {
            for (long index = 0; !found && index < mode->m_foot_tracking.count; ++index) {
                const s_mode_foot_tracking *entry = TAG_BLOCK_GET_ELEMENT(&mode->m_foot_tracking, index, s_mode_foot_tracking);
                if (entry->member == member_index) {
                    found = true;
                    defaults = entry->defaults;
                }
            }
        }
        search_mode = 200;
    }
    if (!found) {
        const s_foot_tracking_member *member = TAG_BLOCK_GET_ELEMENT(&m_foot_tracking, member_index, s_foot_tracking_member);
        defaults = member->defaults;
    }
    if (defaults == 1)
        result = _foot_tracking_on;
    return result;
}

// 0x2fc740
c_animation_id c_model_animation_graph::find_animation(long name) const
{
    c_animation_id result;
    result.clear();
    const c_model_animation_graph *graph = this;
    long graph_index = -1;
    while (graph && !result.valid()) {
        for (long index = 0; index < graph->m_animations.count; ++index) {
            const c_model_animation *animation = graph->get_local_animation(index);
            if (animation->name == name) {
                result.m_graph_index = (short)graph_index;
                result.m_animation_index = (short)index;
                break;
            }
        }
        if (!result.valid()) {
            ++graph_index;
            if (graph_index < m_inheritence_list.count)
                graph = get_inherited_graph(graph_index);
            else
                graph = 0;
        }
    }
    return result;
}

// 0x2fcb30
const c_model_animation_tag_resource_member *c_model_animation_graph::get_local_resource_group_member(long group_index, long member_index) const
{
    const c_model_animation_tag_resource_group *group = 0;
    if (group_index != -1)
        group = TAG_BLOCK_GET_ELEMENT(&m_resource_groups, group_index, c_model_animation_tag_resource_group);
    const s_animation_resource_data *resource = animation_resource_get(group->resource_handle);
    const c_model_animation_tag_resource_member *member = TAG_BLOCK_GET_ELEMENT(&resource->members, member_index, c_model_animation_tag_resource_member);
    return member;
}

// @address 0x2fcbb8 animation_definitions_get_animation_root_matrix
extern "C" void animation_definitions_get_animation_root_matrix(long graph_index, c_animation_id reference, float frame,
    const render_model_definition *model, real_matrix4x3 *matrix)
{
    real_orientation orientation = k_real_math_identity_orientation_storage;
    c_flags<e_anim_computation_flags, unsigned long, 1> computation_flags(1);
    animation_compute_node_orientations(graph_index, reference, 1, frame * 30.0f, 1.0f, 0, &orientation, computation_flags, false, 0.0f, 0.0f);
    matrix4x3_from_point_and_quaternion(matrix, &orientation.translation, &orientation.rotation);
}

static __forceinline short animation_definitions_get_key_frame(const c_model_animation *animation)
{
    long index = 0;
    if (animation->frame_events.count() > 0)
    {
        const s_frame_event *event = animation->frame_events.begin();
        do
        {
            if (event->type == 0)
                return event->frame;
            ++index;
            ++event;
        } while (index < animation->frame_events.count());
    }
    return -1;
}

template<long k_stride>
static __forceinline void animation_definitions_accumulate_movement(
    const float *&movement, long component, long count, long key_frame, float &total, float &at_key)
{
    for (long frame = 0; frame < count; ++frame, movement += k_stride)
    {
        total = movement[component] + total;
        if (frame == key_frame)
            at_key = total;
    }
}

// @address 0x2fc010 animation_definitions_compute_movement_totals
extern "C" void animation_definitions_compute_movement_totals(
    const c_model_animation_graph *graph, c_animation_id reference, short component, float *at_key, float *total)
{
    float total_movement = 0.0f;
    float key_movement = total_movement;
    const c_model_animation *animation = graph->get_animation(reference);
    c_animation_data data;
    data.m_data = 0;
    data.m_sizes = 0;
    data.m_node_count = 0;
    data.m_movement_data_type = 0;
    if (graph->get_animation_data(reference, &data) && data.m_sizes->contains_movement_data())
    {
        const c_animation_data_sizes *sizes = data.m_sizes;
        const float *movement = (const float *)(data.m_data + sizes->m_default_data_size + sizes->m_animated_node_flags_data_size +
            sizes->m_compressed_data_size + sizes->m_static_node_flags_data_size);
        long key_frame = animation_definitions_get_key_frame(animation);
        switch ((signed char)animation->movement_type)
        {
        case 1:
            if (component < 2)
                animation_definitions_accumulate_movement<2>(movement, component, animation->frame_count, key_frame, total_movement, key_movement);
            break;
        case 2:
            if (component < 2)
                animation_definitions_accumulate_movement<3>(movement, component, animation->frame_count, key_frame, total_movement, key_movement);
            break;
        case 3:
            animation_definitions_accumulate_movement<4>(movement, component, animation->frame_count, key_frame, total_movement, key_movement);
            break;
        }
    }
    if (total)
        *total = total_movement;
    if (at_key)
        *at_key = key_movement;
}

__forceinline void c_model_animation_graph::get_animation_root_orientation(
    long graph_index, c_animation_id animation_id, float animation_time, const render_model_definition *render_model,
    real_orientation *orientation)
{
    c_flags<e_anim_computation_flags, unsigned long, 1> computation_flags(1);
    animation_compute_node_orientations(graph_index, animation_id, 1, animation_time *animation_definitions_frame_rate, 1.0f, 0, orientation,
        computation_flags, false, 0.0f, 0.0f);
}

// @address 0x2fcc58 ?get_animation_root_velocity@c_model_animation_graph@@SAXJVc_animation_id@@MMPBUrender_model_definition@@PATreal_vector3d@@@Z
void c_model_animation_graph::get_animation_root_velocity(long graph_index, c_animation_id animation_id, float animation_time,
    float playback_rate, const render_model_definition *render_model, real_vector3d *velocity)
{
    if (animation_time <= animation_definitions_minimum_time)
        animation_time = animation_definitions_minimum_time;
    real_orientation previous_orientation = k_real_math_identity_orientation_storage;
    real_orientation current_orientation = k_real_math_identity_orientation_storage;
    get_animation_root_orientation(graph_index, animation_id, animation_time - animation_definitions_minimum_time, render_model,
        &previous_orientation);
    get_animation_root_orientation(graph_index, animation_id, animation_time, render_model, &current_orientation);
    float scale = playback_rate * animation_definitions_frame_rate;
    velocity->i = (current_orientation.translation.x - previous_orientation.translation.x) * scale;
    velocity->j = (current_orientation.translation.y - previous_orientation.translation.y) * scale;
    velocity->k = (current_orientation.translation.z - previous_orientation.translation.z) * scale;
}

// @address 0x2fc828 ?internal_node_matrices_from_orientations@c_model_animation_graph@@ABAXJJPIAUreal_matrix4x3@@PIBUreal_orientation@@PIBU2@@Z
void c_model_animation_graph::internal_node_matrices_from_orientations(long start_index, long count, real_matrix4x3 * matrices,
    const real_orientation * orientations, const real_matrix4x3 * base_matrices) const
{
    if (count > 0)
    {
        if (start_index == 0)
        {
            matrix4x3_multiply_by_orientation(base_matrices, orientations, matrices);
            start_index = 1;
            --count;
        }
        if (count > 0)
        {
            const animation_graph_node *node = &((animation_graph_node *)m_nodes.address)[start_index];
            for (long node_index = 0; node_index < count; ++node_index)
            {
                long parent_node_index = node[node_index].parent_node_index;
                matrix4x3_multiply_by_orientation(&matrices[parent_node_index], &orientations[start_index + node_index], &matrices[start_index + node_index]);
            }
        }
    }
}

// @address 0x2fc8d0 ?node_matrices_from_orientations_with_gun_hand_swap@c_model_animation_graph@@QBAXPIAUreal_matrix4x3@@PIBUreal_orientation@@PIBU2@JJ@Z
void c_model_animation_graph::node_matrices_from_orientations_with_gun_hand_swap(real_matrix4x3 * matrices,
    const real_orientation * orientations, const real_matrix4x3 * base_matrices, long weapon_root_index,
    long opposite_hand_node_index) const
{
    real_matrix4x3 * matrices_loc = matrices;
    long max_index;
    if (weapon_root_index > opposite_hand_node_index)
        max_index = weapon_root_index;
    else
        max_index = opposite_hand_node_index;
    long max_node_index = max_index + 1;
    internal_node_matrices_from_orientations(0, max_node_index, matrices_loc, orientations, base_matrices);
    const real_matrix4x3 *base = base_matrices;
    animation_graph_node *node = &((animation_graph_node *)m_nodes.address)[weapon_root_index];
    long parent_index = node->parent_node_index;
    if (weapon_root_index != 0 && parent_index != -1)
        base = &matrices_loc[parent_index];
    const real_matrix4x3 *identity = &k_real_math_identity_matrix_storage;
    real_matrix4x3 result;
    matrix4x3_inverse(base, &result);
    real_matrix4x3 *weapon_matrix = &matrices_loc[weapon_root_index];
    real_matrix4x3 temp;
    matrix4x3_compute_product(&result, weapon_matrix, &temp);
    memcpy(&result, identity, sizeof(result));
    result.left.i = -0.0f;
    result.left.j *= -1.0f;
    result.left.k *= -1.0f;
    real_matrix4x3 *opposite_matrix = &matrices_loc[opposite_hand_node_index];
    real_matrix4x3 swapped;
    matrix4x3_compute_product(opposite_matrix, &result, &swapped);
    matrix4x3_compute_product(&swapped, &temp, &result);
    float forward_dot = animation_definitions_dot_product3d_kfirst(&result.forward, &opposite_matrix->left);
    float left_dot = animation_definitions_dot_product3d_kfirst(&result.left, &opposite_matrix->left);
    float up_dot = animation_definitions_dot_product3d_kfirst(&result.up, &opposite_matrix->left);
    forward_dot *= forward_dot;
    left_dot *= left_dot;
    up_dot *= up_dot;
    memcpy(&temp, identity, sizeof(temp));
    if (forward_dot >= left_dot && forward_dot >= up_dot) {
        scale_vector3d(&temp.forward, -1.0f, &temp.forward);
        temp.forward.i = -1.0f;
    } else if (left_dot >= forward_dot && left_dot >= up_dot) {
        scale_vector3d(&temp.left, -1.0f, &temp.left);
        temp.left.i = -0.0f;
    } else {
        scale_vector3d(&temp.up, -1.0f, &temp.up);
        temp.up.i = -0.0f;
    }
    matrix4x3_compute_product(&result, &temp, weapon_matrix);
    internal_node_matrices_from_orientations(weapon_root_index + 1, m_nodes.count - weapon_root_index - 1, matrices_loc, orientations, base_matrices);
}
