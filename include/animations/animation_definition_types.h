#ifndef ANIMATIONS_ANIMATION_DEFINITION_TYPES_H
#define ANIMATIONS_ANIMATION_DEFINITION_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
#include "animations/animation_id.h"

void *find_named_block_entry_with_size(const s_tag_block *block, long key, long element_size);

class c_model_animation_graph;
class c_weapon_type;

struct s_animation_entry { long name; c_animation_id animation; };
struct s_animation_damage_entry { long name; s_tag_block directions; };
struct s_animation_damage_direction { s_tag_block regions; };

struct s_vehicle_suspension {
    long name;
    c_animation_id animation;
    long marker;
    float extension;
    float compression;
    float rate;
    long region;
    float scale;
    float offset;
    float limit;
};

struct s_object_overlay {
    long name;
    c_animation_id animation;
    short pad8;
    short mode;
    long function_name;
    long pad10;
};

template <class element_type>
static __forceinline const element_type *animation_definitions_tag_block_get_element(const s_tag_block *block, long index)
{
    return &((const element_type *)block->address)[index];
}

class c_weapon_type {
public:
    __forceinline c_animation_id find_action(long name) const {
        c_animation_id result;
        result.clear();
        const s_animation_entry *entry = (const s_animation_entry *)find_named_block_entry_with_size(&m_actions, name, sizeof(s_animation_entry));
        if (entry)
            result = entry->animation;
        return result;
    }
    __forceinline c_animation_id find_overlay(long name) const {
        c_animation_id result;
        result.clear();
        const s_animation_entry *entry = (const s_animation_entry *)find_named_block_entry_with_size(&m_overlays, name, sizeof(s_animation_entry));
        if (entry)
            result = entry->animation;
        return result;
    }
    __forceinline c_animation_id find_damage_animation(long name, long direction, long region) const {
        c_animation_id result;
        result.clear();
        const s_animation_damage_entry *entry = (const s_animation_damage_entry *)
            find_named_block_entry_with_size(&m_damage, name, sizeof(s_animation_damage_entry));
        if (entry && direction >= 0 && direction < entry->directions.count) {
            const s_animation_damage_direction *damage_direction =
                animation_definitions_tag_block_get_element<s_animation_damage_direction>(&entry->directions, direction);
            if (region >= 0 && region < damage_direction->regions.count) {
                const c_animation_id *animation = animation_definitions_tag_block_get_element<c_animation_id>(&damage_direction->regions, region);
                result = *animation;
            }
        }
        return result;
    }
    c_animation_id find_transition(long state, char direction, char region, long destination_mode, long destination_state,
        char destination_direction, char destination_region, char type) const;
    long m_name;
    s_tag_block m_actions;
    s_tag_block m_overlays;
    s_tag_block m_damage;
    s_tag_block m_transitions;
};

struct s_frame_event { short type; short frame; };
struct s_animation_asset_event { short reference, frame; long marker; };
struct s_animation_dialogue_event { short dialogue, frame; };

struct s_object_space_correction;

struct s_foot_tracking_cycle {
    short start;
    short plant;
    short unplant;
    short end;
    real_point3d lock_point;
};
struct s_foot_tracking_foot {
    short type;
    short pad;
    long cycle_count;
    TAG_POINTER(s_foot_tracking_cycle) cycles;
    long definition;
};

class c_model_animation
{
public:
    float get_playback_ratio_from_frame_position(float frame) const;
    float get_frame_position_from_playback_ratio(float fraction) const;
    __forceinline short get_loop_frame_index() const { return loop_frame_index; }
    __forceinline short get_frame_count() const { return frame_count; }
    __forceinline float get_last_frame_index() const { return (float)(frame_count - 1); }

    __forceinline short find_first_key_of_type(long type) const
    {
        for (long key_index = 0; key_index < frame_events.count(); key_index++)
        {
            const s_frame_event *frame_event = &frame_events[key_index];
            if (frame_event->type == type)
                return frame_event->frame;
        }
        return -1;
    }
    __forceinline short find_next_key_of_type(long type, long after) const
    {
        for (long key_index = 0; key_index < frame_events.count(); ++key_index)
        {
            const s_frame_event *key = &frame_events[key_index];
            if (key->type == type && key->frame > after)
                return key->frame;
        }
        return -1;
    }
    __forceinline short get_primary_key_frame_index() const { return find_first_key_of_type(0); }
    __forceinline short get_secondary_key_frame_index() const { return find_first_key_of_type(1); }
    // March 82bfeeb8 reads the world-relative bit at +0x16.
    __forceinline bool animation_is_world_relative() const { return ((internal_flags >> 1) & 1) != 0; }

    long name;
    float weight;
    short loop_frame_index;
    unsigned short animation_flags;
    char blend_screen;
    signed char desired_compression_setting;
    signed char current_compression_setting;
    unsigned char node_count;
    short frame_count;
    signed char animation_type;
    unsigned char movement_type;
    unsigned short production_flags;
    unsigned short internal_flags;
    unsigned long node_list_checksum;
    unsigned long production_checksum;
    unsigned short importer_version;
    unsigned short compressor_version;
    unsigned short parent_animation_index;
    unsigned short next_animation_index;
    short resource_group;
    short resource_group_member;
    c_typed_tag_block<s_frame_event> frame_events;
    s_tag_block sound_events;
    s_tag_block effect_events;
    s_tag_block dialogue_events;
    c_typed_tag_block<s_object_space_correction> object_space_corrections;
    c_typed_tag_block<s_foot_tracking_foot> feet;
    real_vector3d motion_direction;
    float motion_distance;
    float average_pivot_magnitude;
};

typedef char foot_tracking_lock_point_offset[(offsetof(s_foot_tracking_cycle, lock_point) == 8) ? 1 : -1];
typedef char foot_tracking_cycle_size[(sizeof(s_foot_tracking_cycle) == 0x14) ? 1 : -1];
typedef char model_animation_size[(sizeof(c_model_animation) == 0x88) ? 1 : -1];
typedef char model_animation_checksum_offset[(offsetof(c_model_animation, node_list_checksum) == 0x18) ? 1 : -1];
typedef char model_animation_pivot_offset[(offsetof(c_model_animation, average_pivot_magnitude) == 0x84) ? 1 : -1];
typedef char model_animation_internal_flags_offset[(offsetof(c_model_animation, internal_flags) == 0x16) ? 1 : -1];

typedef char model_animation_frame_events_offset[(offsetof(c_model_animation, frame_events) == 0x2c) ? 1 : -1];
typedef char model_animation_frame_event_size[(sizeof(s_frame_event) == 4) ? 1 : -1];

class c_weapon_class
{
public:
    const c_weapon_type *find_type(long name) const
    {
        return (const c_weapon_type *)find_named_block_entry_with_size(&m_weapon_types, name, sizeof(c_weapon_type));
    }

    long m_name;
    s_tag_block m_weapon_types;
    s_tag_block m_ik_points;
};

class c_animation_mode
{
public:
    __forceinline const c_weapon_class *find_class(long name) const
    {
        return (const c_weapon_class *)find_named_block_entry_with_size(&m_weapon_classes, name, sizeof(c_weapon_class));
    }

    long m_name;
    s_tag_block m_weapon_classes;
    s_tag_block m_ik_points;
    s_tag_block m_foot_tracking;
};

class c_animation_search_path_iterator
{
public:
    __forceinline void begin(const c_model_animation_graph *graph, long mode, long weapon_class, long weapon_type)
    {
        m_graph = graph;
        m_state = 0;
        m_mode = 0;
        m_weapon_class = 0;
        m_mode_name = mode;
        m_weapon_class_name = weapon_class;
        m_weapon_type_name = weapon_type;
    }
    const c_weapon_type *get_next_weapon_type(long *result_mode, long *result_weapon_class, long *result_weapon_type);

    const c_model_animation_graph *m_graph;
    long m_mode_name;
    long m_weapon_class_name;
    long m_weapon_type_name;
    unsigned long m_state;
    const c_animation_mode *m_mode;
    const c_weapon_class *m_weapon_class;
};

typedef char model_animation_sound_events_offset[(offsetof(c_model_animation, sound_events) == 0x38) ? 1 : -1];
typedef char model_animation_effect_events_offset[(offsetof(c_model_animation, effect_events) == 0x44) ? 1 : -1];
typedef char model_animation_dialogue_events_offset[(offsetof(c_model_animation, dialogue_events) == 0x50) ? 1 : -1];

#endif // ANIMATIONS_ANIMATION_DEFINITION_TYPES_H
