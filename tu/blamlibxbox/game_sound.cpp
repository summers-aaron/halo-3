// blamlibxbox/game_sound.obj: from the records 0xa6f80 0xf8248 0xf83a8 0xf8490 0xfc000 0xfd160 0xfd270 0xfd3b0 0xfd418 0xfe0b0 0x122958 0x126f80 0x1275a0 0x128328 0x128430 0x13b698 0x1fbe50 0x1fbeb0 0x1fbec8 0x1fbed8 0x1fbee8 0x1fc000 0x1fc020 0x1fc1e8 0x1fc268 0x1fc558 0x1fc628 0x1fc710 0x1fc7d0 0x1fc848 0x1fc968 0x1fca20 0x1fcb08 0x1fcbd8 0x1fcca0 0x1fcd08 0x1fce88 0x1fcff8 0x1fd070 0x1fd0c8 0x1fd138 0x1fd238 0x1fd398 0x1fd468 0x1fd770 0x1fd940 0x1fda88 0x1fdd60 0x1fde38 0x1fdf68 0x1fe038 0x1fe0f0 0x1fe2a8 0x1fe310 0x1fe398 0x1fe518 0x1fe5c0 0x1fec60 0x1fef30 0x1ff568 0x1ff678 0x1ff7f0 0x1ffc90 0x1ffd40 0x1ffee8 0x200008 0x200110 0x200190 0x2001c8 0x2001f8 0x2002e8 0x200358 0x200498 0x200528 0x2005d0 0x200690 0x200768 0x200850 0x2008f0 0x200a28 0x200a88 0x200b28 0x200b48 0x200be8 0x200c00 0x200c20 0x200cc0 0x200cd8 0x2d6d78 0x4fac20 0x4fb828 0x567dd8 0x707dc0 0x707de0 0x707df0 0x707e00 0x70b978
// @flags /TP /O2
#include "game/game_time_inline.h"
#include "camera/cinematic_scene_types.h"
#include "camera/cinematic_scene_music_types.h"
#include "interface/chud/cortana_effect_definition.h"
#include "memory/tls_data_arrays.h"
#include "structures/structure_portal_reference_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/real_constant_pool.h"
// @address 0x3aac8 k_real_math_identity_matrix_storage
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
// @address 0x1fbec8 ?__tls_set_g_game_sound_globals_allocator@@YAXPAX@Z
#include <string.h>
#include <ppcintrinsics.h>
#include "game/game_time_types.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "cache/restricted_memory_regions.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "sound/game_sound.h"
#include "sound/scripted_sound_time_inline.h"
#include "sound/game_sound_externs.h"
#include "sound/game_sound_types.h"
#include "sound/sound_manager_runtime_types.h"
#include "sound/game_looping_sound_data.h"
#include "sound/default_acoustics_definition.h"
#include "sound/looping_sound_definitions.h"
#include "sound/sound_datum.h"
#include "sound/sound_tracker.h"
#include "objects/damage_effect_definitions.h"
#include "sound/sound_globals_reference_types.h"
#include "items/item_types.h"
#include "cutscene/cinematics.h"
#include "effects/effects.h"
#include "hs/hs_runtime.h"
#include "interface/first_person_weapons.h"
#include "render/render_objects_static_lighting.h"
#include "scenario/scenario_definitions.h"
#include "sound/game_sound_deterministic.h"
#include "sound/game_sound_player_effects.h"
#include "sound/looping_sound_manager.h"
#include "sound/sound_classes.h"
#include "sound/sound_definitions.h"
#include "sound/sound_effect_manager.h"
#include "sound/sound_manager.h"
#include "sound/sound_playback_controller.h"
#include "sound/sound_playback_controller_types.h"
#include "structures/structures.h"
#include "ai/actor_perception.h"
#include "interface/chud/chud.h"
#include "math/integer_math.h"
#include "sound/sound_xaudio_xenon.h"
#include "units/motor_enums.h"
#include "objects/object_reference_types.h"
#include "game/game_globals.h"
#include "effects/effect_datum.h"
#include "effects/effect_data.h"
#include "ai/ai_player_tls.h"
#include "ai/ai_player.h"
#include "cache/restricted_memory.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "ai/ai_player_types.h"
#include "units/bipeds.h"
#include "sound/sound_manager_platform_playback.h"
#include "memory/data.h"
#include "camera/camera_scripting.h"
#include "physics/impact.h"
#include "effects/event_flags_types.h"
#include "game/sound_tracking_data_types.h"
#include "objects/cluster_reference.h"
#include "cache/cache_files.h"
#include "tag_files/string_ids.h"
#include "hs/hs_function.h"
#include "memory/data_inline.h"
#include "hs/hs_value_types.h"
#include "game/player_mapping_types.h"
#include "structures/structure_bsp_tag.h"
#include "objects/object_type_definition.h"
#include "cseries/data_mining_event_types.h"
#include "sound/sound_source.h"
#include "sound/game_sound_spatialization.h"
#include "math/matrix_math.h"
#include "motor/action_system.h"
#include "sound/sound_cache_file_gestalt.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include "sound/game_sound_impulse_parameters.h"
#include "sound/sound_cache_file_definitions.h"
#include "sound/sound_cache_file_gestalt_globals.h"
#include "sound/sound_class_definitions.h"
#include "structures/structure_seam_cluster_stack.h"
#include "structures/structure_cluster_portal.h"
#include "hs/hs_function_definition.h"
#include "objects/object_placement.h"
#include "objects/object_datum.h"
#include "units/unit_types.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "sound/game_sound_player_effects_types.h"
#include "sound/sound_manager_types.h"
#include "structures/structure_seams.h"
#include "cache/restricted_memory_layout.h"
#include <math.h>
#include "sound/game_sound_definitions.h"
#include "cseries/cseries.h"
#include "sound/game_sound_message_queue.h"
#include "cseries/crt_xdk.h"
#include "animations/animation_manager.h"
#include "animations/animation_manager_mixing_board_inline.h"
#include "animations/mixing_board/channels/facial_animation_channel_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "effects/effect_definitions.h"
#include "game/game_globals_storage.h"
#include "game/game_globals_inline.h"
#include "camera/director_globals_types.h"



#include "cache/restricted_memory_registration.h"
#include "multithreading/interlocked_slist_push.h"
#include "sound/game_sound_allocators.h"
#include "sound/scripted_looping_sound.h"
#include "sound/game_sound_deterministic_permutation.h"
#include "sound/sound_impulse_definition.h"
#include "sound/looping_sound_manager_dependencies.h"
#include "sound/game_sound_impulse.h"
#include "ai/mission_dialogue_types.h"
#include "ai/mission_dialogue_variant.h"
#include "sound/sound_mix_definition.h"
#include "ai/event_handling_sound_types.h"
#include "devices/machine_datum.h"
#include "scenario/scenario_zone_set_audibility.h"
#include "sound/game_sound_listener_definition.h"
#include "sound/game_sound_restore.h"
#include "objects/object_header_access.h"
#include "game/player_mapping_inline.h"
#include "cubemaps/cubemap_sample.h"
#include "scenario/scenario_access.h"
#include "hs/hs.h"
#include "hs/hs_macro_function.h"
#include "units/unit_scripting.h"
#include "objects/scenery.h"
#include "sound/sound_scenery.h"
struct game_looping_sound_datum;
struct impact_sound_flags {
    unsigned short m_storage;
    void clear() { m_storage = 0; }
    __forceinline void set(int bit, bool value) { if (value) m_storage |= 1 << bit; else m_storage &= ~(1 << bit); }
};
enum e_game_looping_sound_state { _game_looping_sound_state_initial = 3 };
enum e_game_looping_sound_type
{
    _game_looping_sound_type_unattached,
    _game_looping_sound_type_object,
    _game_looping_sound_type_impact,
    _game_looping_sound_type_effect,
    _game_looping_sound_type_music,
};
struct s_looping_sound_attachment_record {
    unsigned short identifier;
    c_enum<e_game_looping_sound_state, char, 0, 4> state;
    c_enum<e_game_looping_sound_type, char, 0, 5> type;
    impact_sound_flags flags;
    short attachment_index;
    float scale;
    long definition_index, sound_index;
    union {
        struct { long object_index; long identifier; } object;
        struct { union { long marker_name; real_decibel gain; }; s_cluster_reference cluster_reference; c_enum_no_init<long, char, -1,
            64> user_index; } unattached;
    } source;
};
struct s_music_identifier_generator {
    short salt;
    short index;
    __forceinline long next()
    {
        if ((unsigned short)salt == 0xffff)
            salt = (short)0x8000;
        short identifier_salt = salt;
        short identifier_index = index;
        long result = (identifier_salt << 16) | identifier_index;
        salt = (short)(identifier_salt + 1);
        index = (short)(identifier_index + 1) % 1021;
        return result;
    }
};
enum e_sound_spatialization_mode
{
    _sound_spatialization_mode_none = 0,
    _sound_spatialization_mode_unspatialized = 1,
    _sound_spatialization_mode_spatialized = 2
};
struct s_first_person_stereo_identifiers {
    __int64 identifiers[3];
};
struct s_sound_tracker_constant_pool {
    real_decibel default_additional_gain;
    real_decibel minimum_gain;
    long lowest_bit[37];
    unsigned long near_field_constants[2];
    s_sound_tracker object_trackers[3];
};
struct s_tracked_background_sound {
    float distance;
    float interior_volume;
    float portal_volume;
    float exterior_volume;
    short palette_index;
    s_cluster_reference cluster;
    long definition_index;
    float volume;
    real_decibel gain;
    bool play_alternate;
};
struct s_active_background_sound {
    long definition_index;
    float volume;
    float maximum_delta;
    real_decibel gain;
    long type;
    s_cluster_reference cluster;
    long palette_index;
    bool spatialize;
    bool play_alternate;
};
struct s_looping_sound_update_snapshot
{
    c_enum<e_game_looping_sound_type, char, 0, 5> type;
    unsigned short identifier;
    c_enum<e_game_looping_sound_state, char, 0, 4> state;
    unsigned short flags;
    short attachment_index;
    float scale;
    long definition_index, sound_index, object_index, source_identifier;
    __forceinline s_looping_sound_update_snapshot(const s_looping_sound_attachment_record &sound)
        : type(sound.type), identifier(sound.identifier), state(sound.state), flags(sound.flags.m_storage),
          attachment_index(sound.attachment_index), scale(sound.scale), definition_index(sound.definition_index),
          sound_index(sound.sound_index), object_index(sound.source.object.object_index), source_identifier(sound.source.object.identifier) {}
    __forceinline s_looping_sound_update_snapshot(const s_looping_sound_update_snapshot &sound)
        : type(sound.type), identifier(sound.identifier), state(sound.state), flags(sound.flags),
          attachment_index(sound.attachment_index), scale(sound.scale), definition_index(sound.definition_index),
          sound_index(sound.sound_index), object_index(sound.object_index), source_identifier(sound.source_identifier) {}
};


namespace {
static __forceinline long restricted_region_add_game_sound_globals_member(const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = g_restricted_regions[1].add_member(name, type, size, alignment_bits, __tls_set_g_game_sound_globals_allocator, 0, 0);
    void *address = restricted_region_get_member_address(1, member_index);
    g_game_sound_globals = (game_sound_globals *)address;
    return member_index;
}
static __forceinline long restricted_region_add_game_sound_scripted_impulses_member(const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = g_restricted_regions[1].add_member(name, type, size, alignment_bits, game_sound_set_scripted_impulses_address, 0, 0);
    void *address = restricted_region_get_member_address(1, member_index);
    g_game_sound_scripted_impulses = (s_scripted_impulse_sound *)address;
    return member_index;
}
}

static __forceinline int next_allocated_datum_index(s_data_array *array, int index)
{
    long absolute_index = ::data_next_absolute_index((const ::s_data_array *)(array), (index + 1) & 0xffff);
    int result = -1;
    if (absolute_index != -1)
        result = (*(unsigned short *)(array->data + absolute_index * array->size) << 16) | absolute_index;
    return result;
}

namespace {
static __forceinline void game_sound_reset_scripted_dialog_timer() { ((game_sound_globals *)g_game_sound_globals)->scripted_dialog_timer = 0; }
}


namespace {
static __forceinline s_portal_reference *game_sound_portal_reference_invalidate(s_portal_reference *reference)
{
    reference->value = -1;
    return reference;
}
static __forceinline const s_portal_reference *game_sound_portal_reference_set(s_portal_reference *reference, long bsp, long portal)
{
    reference->value = (short)((bsp << 10) | (portal & 1023));
    return reference;
}
static __forceinline void game_sound_class_set_gain(const char *name, float gain, float time)
{
    sound_class_set_gain_db(name, real_gain_to_decibels(gain), time, 0);
}
}




// 0xf8248: turns the sound location to the rotation that carries the identity forward direction onto the vector
// @address 0xf8248 game_sound_location_set_rotation_from_direction
extern "C" void game_sound_location_set_rotation_from_direction(s_sound_location *sound_location, const real_vector3d *vector)
{
    real_vector3d direction = *vector;
    normalize3d_with_default(&direction, &k_real_math_identity_matrix_storage.forward);
    quaternion_from_unit_vectors(&sound_location->rotation, &k_real_math_identity_matrix_storage.forward, &direction);
}

// @address 0xf8490 ?game_sound_dispose_from_old_map@@YAXXZ
void game_sound_dispose_from_old_map()
{
    if (game_looping_sound_data.get() != NULL && game_looping_sound_data->valid) {
        game_sound_clear(0);
        game_looping_sound_data->valid = false;
        game_sound_reset_scripted_dialog_timer();
    }
}

// @address 0xfd270 ?game_sound_clear@@YAXJ@Z
void game_sound_clear(long reason)
{
    for (long sound_index = data_next_index(game_looping_sound_data, -1); sound_index != -1;
        sound_index = next_allocated_datum_index(game_looping_sound_data, sound_index))
    {
        s_looping_sound_attachment_record *sound = datum_get<s_looping_sound_attachment_record>(game_looping_sound_data, sound_index);
        long scripted_sound_index = scripted_looping_sound_get_sound_index(sound->definition_index);
        if (scripted_sound_index == sound_index)
            scripted_looping_sound_reset_internal(sound->definition_index, -1);
    }
}

static __forceinline long game_sound_impulse_new_for_object(long object_index, long sound_index, const s_sound_location *location, float volume)
{
    long listener_index = -1;
    if (object_unattached_impulse_sound_new_internal(object_index, sound_index, &listener_index))
        return unattached_impulse_sound_new(sound_index, location, volume);
    return first_person_impulse_sound_new_internal(listener_index, object_index, sound_index, false, &location->position, volume);
}


// @address 0x122958 game_sound_new_material_effect_sounds
// 0x122958: plays up to 3 impulse sounds at a scaled offset position, unattached or first-person per listener
extern "C" bool game_sound_new_material_effect_sounds(long *definition_indices, const real_point3d *position, const real_vector3d *normal,
    const real_vector3d *direction, const long *location, long user_index, float volume)
{
    bool result = false;
    real_point3d offset_position;
    offset_position.x = direction->i * 0.01f + position->x;
    offset_position.y = direction->j * 0.01f + position->y;
    offset_position.z = direction->k * 0.01f + position->z;
    struct { s_sound_location sound; long location; } spatial;
    spatial.sound.position = offset_position;
    game_sound_location_set_rotation_from_direction(&spatial.sound, direction);
    spatial.sound.velocity = *global_zero_vector3d;
    spatial.location = *location;
    for (unsigned long i = 0; i < 3; i++) {
        if (definition_indices[i] != -1) {
            game_sound_impulse_new_for_object(user_index, definition_indices[i], &spatial.sound, volume);
            result = true;
        }
    }
    return result;
}



// 0x1275a0: the definition of the default block's tag, when at most one player is playing
// @address 0x1275a0 game_sound_try_and_get_default_acoustics_definition
extern "C" const s_default_acoustics_definition *game_sound_try_and_get_default_acoustics_definition(bool enabled)
{
    const s_default_acoustics_definition *definition = 0;
    if (tls_player_mapping_globals()->local_player_count <= 1 && enabled) {
        sound_globals_reference *block = ((s_game_globals *)global_game_globals)->sound_globals.count() != 0
            ? (sound_globals_reference *)((s_game_globals *)global_game_globals)->sound_globals.address
            : 0;
        if (block->default_acoustics.index != -1)
            definition = TAG_GET(block->default_acoustics.group_tag, const s_default_acoustics_definition, block->default_acoustics.index);
    }
    return definition;
}

// @address 0x13b698 ?game_sound_find_closest_portal@@YA?AUs_portal_reference@@PBTreal_point3d@@Us_cluster_reference@@PAM@Z
// match: 0x13b698: pointer __restrict qualifier prevents alias reload
s_portal_reference game_sound_find_closest_portal(const real_point3d *__restrict position, s_cluster_reference cluster, float *__restrict out_closest_distance)
{
    c_static_stack<s_cluster_reference, 64> clusters;
    *out_closest_distance = 3.402823466e+38f;
    s_portal_reference closest_portal;
    game_sound_portal_reference_invalidate(&closest_portal);
    s_cluster_stack connected_clusters;
    connected_clusters.m_count = 0;
    structure_seams_connected_cluster_references_get(global_scenario_index, cluster, true, &connected_clusters);
    for (long index = 0; index < connected_clusters.m_count; ++index)
        clusters.push_back(connected_clusters.m_elements[index]);
    for (long index = 0; index < clusters.count(); ++index) {
        s_cluster_reference reference = *clusters.get(index);
        structure_bsp *bsp = global_structure_bsp_get(reference.bsp_index);
        structure_cluster *bsp_cluster = &((structure_cluster *)bsp->clusters.address)[reference.cluster_index];
        for (long portal_number = 0; portal_number < bsp_cluster->portal_indices.count; ++portal_number) {
            long portal_index = ((short *)bsp_cluster->portal_indices.address)[portal_number];
            cluster_portal *portal = &((cluster_portal *)bsp->cluster_portals.address)[portal_index];
            float distance = distance3d(&portal->centroid, position);
            if (*out_closest_distance > distance) {
                *out_closest_distance = distance;
                game_sound_portal_reference_set(&closest_portal, reference.bsp_index, portal_index);
            }
        }
    }
    return closest_portal;
}

// @address 0x1fbe50 ?sound_source_quantize_cone_angle@@YAGM@Z
inline unsigned short sound_source_quantize_cone_angle(float angle)
{
    return (unsigned short)quantize_real(((((angle) > (0.0f) ? (angle) : (0.0f))) < (3.1415927f) ? (((angle) > (0.0f) ? (angle) : (0.0f))) : (3.1415927f)),
        0.0f, 3.1415927f, 16, false, false);
}
// @address 0x1fc628 ?game_sound_disable_at_game_finish@@YAXXZ
void game_sound_disable_at_game_finish()
{
    if (game_engine_running()) {
        game_sound_class_set_gain("", 0.0f, 1.0f);
        game_sound_class_set_gain("ambient_nature", 0.2f, 1.0f);
        game_sound_class_set_gain("ambient_machinery", 0.2f, 1.0f);
        game_sound_class_set_gain("ambient_computers", 0.2f, 1.0f);
    }
}

static __forceinline s_looping_sound_attachment_record *game_sound_attachment_datum_get(s_data_array *data, long index)
{
    long absolute_index = index & 0xffff;
    return (s_looping_sound_attachment_record *)(absolute_index * sizeof(s_looping_sound_attachment_record) + data->data);
}
// @address 0x1fc710 ?game_looping_sound_attachment_new@@YAJJJJJ@Z
long game_looping_sound_attachment_new(long object_index, long definition_index, long attachment_index, long identifier)
{
    long sound_index = -1;
    if (definition_index != -1) {
        sound_index = datum_new(game_looping_sound_data);
        if (sound_index != -1) {
            s_looping_sound_attachment_record *sound = game_sound_attachment_datum_get(
                game_looping_sound_data, sound_index);
            sound->definition_index = definition_index;
            sound->sound_index = -1;
            sound->state = _game_looping_sound_state_initial;
            sound->flags.clear();
            sound->flags.set(9, true);
            if (object_index != -1) {
                sound->type = _game_looping_sound_type_object;
                sound->source.object.object_index = object_index;
                sound->source.object.identifier = identifier;
                sound->attachment_index = (short)attachment_index;
            } else {
                sound->attachment_index = -1;
                sound->type = _game_looping_sound_type_unattached;
                sound->source.unattached.marker_name = 0;
                sound->source.unattached.user_index = -1;
                sound->source.unattached.cluster_reference = invalid_cluster_reference_get();
            }
        }
    }
    return sound_index;
}

// @address 0x1fc7d0 ?sound_definition_link_to_owner@@YA_NJJ@Z
bool sound_definition_link_to_owner(long tag_index, long object_index)
{
  bool result = false;
  if (tag_index != -1) {
    if (TAG_GET('snd!', s_cache_file_sound_definition, tag_index)->flags & 0x20) {
      if (object_index != -1) {
        if ((1 << object_header_get(object_index)->object_type) & 4)
          result = true;
      }
    }
  }
  return result;
}

// @address 0x1fca20 ?object_impulse_sound_initialize_attachment_data@@YAXJJFPBTreal_point3d@@PBTreal_vector3d@@PAUs_sound_attachment_data@@@Z
void object_impulse_sound_initialize_attachment_data(long definition_index, long object_index, short node_index,
    const real_point3d *position, const real_vector3d *forward, s_sound_attachment_data *attachment)
{
    attachment->position = *position;
    attachment->forward = *forward;
    if (node_index != -1)
        attachment->node_index = (unsigned char)node_index;
    else
        attachment->node_index = 0;
    attachment->object_index = -1;
    if (object_index != -1 && sound_definition_link_to_owner(definition_index, object_index) &&
        ((1 << object_header_get(object_index)->object_type) & 4))
    {
        item_datum *object = (item_datum *)object_header_get(object_index)->datum;
        if (object->owner_relation)
            attachment->object_index = object->unit_object_index;
    }
}

void __tls_set_g_game_looping_sound_data_allocator(void *allocation)
{
    data_set_new_base_address(
        &game_looping_sound_data.get_restricted_data_array_address(),
        (s_data_array *)allocation);
}

// @address 0x1fbec8 ?__tls_set_g_game_sound_globals_allocator@@YAXPAX@Z
void __tls_set_g_game_sound_globals_allocator(void *allocation)
{
    g_game_sound_globals = (game_sound_globals *)allocation;
}

// @address 0x1fbed8 game_sound_set_scripted_impulses_address
extern "C" void game_sound_set_scripted_impulses_address(void *address)
{
    g_game_sound_scripted_impulses = (s_scripted_impulse_sound *)address;
}


__forceinline void game_sound_render_message_queue::initialize(long message_count)

{
    pending_list = 0;
    free_list = 0;
    for (unsigned long index = 0; index < 128; index++)
        messages[index].next = 0;
    valid = true;
    count = message_count;
    if (count != 0) {
        for (long index = count - 1; index >= 0; index--) {
            __lwsync();
            InterlockedPushEntrySList(&free_list, &messages[index]);
        }
    }
}




// @address 0x1fbee8 ?game_sound_initialize@@YAXXZ
void game_sound_initialize()
{
    g_game_sound_message_queue.initialize(128);
    game_looping_sound_data = data_new("object looping sounds", 1024, 28, 0, (c_allocation_base *)&g_game_looping_sound_data_allocator);
    g_game_sound_globals = (game_sound_globals *)g_game_sound_globals_allocator.reserve_memory("game sound globals", 0, 344, 0);
    g_game_sound_scripted_impulses = (s_scripted_impulse_sound *)g_game_sound_scripted_impulses_allocator.reserve_memory(
        "game sound scripted impulses", 0, sizeof(s_scripted_impulse_sound) * k_maximum_scripted_impulse_sounds, 0);
}

void game_sound_dispose()
{
    if (game_looping_sound_data.get() != NULL)
        game_looping_sound_data = 0;
}

template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x200b48: value range assumption emits the alignment zeroing (li r7,0)
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_game_sound_globals_member(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_scripted_impulses_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x200c20: value range assumption emits the alignment zeroing (li r7,0)
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_game_sound_scripted_impulses_member(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

void __tls_set_g_game_sound_scripted_impulses_allocator(void *allocation)
{
    tls_ai_players_set((ai_player_state *)allocation);
}

static __forceinline long game_sound_scripted_impulse_find(long definition_index)
{
    long result = -1;
    for (long index = 0; index < k_maximum_scripted_impulse_sounds; index++) {
        if (g_game_sound_scripted_impulses[index].definition_index == definition_index) {
            result = index;
            break;
        }
    }
    return result;
}
struct s_game_sound_deterministic_permutation;
// @address 0x23dcf8 ?game_sound_deterministic_pick_permutation@@YAXJPAUs_game_sound_deterministic_permutation@@PA_N@Z
namespace {
static __forceinline long game_sound_get_scripting_time(long definition_index)
{
    s_cache_file_sound_definition *sound = TAG_GET('snd!', s_cache_file_sound_definition, definition_index);
    return hs_seconds_to_ticks((float)sound->maximum_play_time / 1000.0f);
}

}
// 0x1fcbd8: finds the listener that hears an unattached impulse sound for a user, and whether it plays at all
// @address 0x1fcbd8 object_unattached_impulse_sound_new_internal
extern "C" bool object_unattached_impulse_sound_new_internal(long user_index, long definition_index, long *out_listener_index)
{
    bool result = true;
    long listener_index = -1;
    if (user_index != -1) {
        char spatialization = 1;
        if (!(TAG_GET('snd!', s_cache_file_sound_definition, definition_index)->flags & 2)) {
            update_spatialization_if_first_person(user_index, &spatialization, &listener_index);
        }
        result = spatialization - 1 == 0;
        if (!result && listener_index != -1)
            result = !((s_sound_manager_globals *)g_sound_manager_globals)->listeners[listener_index].valid;
    }
    *out_listener_index = listener_index;
    return result;
}

// @address 0x1fdd60 ?scripted_sound_time_legacy_do_not_use@@YAJJ@Z
long scripted_sound_time_legacy_do_not_use(long definition_index)
{
    long result = 0;
    if (definition_index != -1)
        result = scripted_sound_time_explicit_hs_ticks(definition_index, game_sound_get_scripting_time(definition_index));
    return result;
}


// @address 0x1fde38 ?scripted_sound_language_time_explicit_hs_ticks@@YAJJ@Z
long scripted_sound_language_time_explicit_hs_ticks(long definition_index)
{
    float seconds = 0.0f;
    if (definition_index != -1) {
        long impulse_index = game_sound_scripted_impulse_find(definition_index);
        if (impulse_index != -1) {
            s_scripted_impulse_sound *impulse = &g_game_sound_scripted_impulses[impulse_index];
            float playing_time = game_sound_deterministic_get_permutation_playing_time(
                definition_index, (const s_game_sound_deterministic_permutation *)&impulse->permutation);
            long ticks = game_seconds_to_ticks_round(playing_time) - game_time_get() + impulse->start_time;
            seconds = game_ticks_to_seconds((float)ticks);
            if (!(seconds > 0.0f))
                seconds = 0.0f;
        }
    }
    return hs_seconds_to_ticks(seconds);
}


long scripted_sound_time_explicit_game_ticks(long definition_index, long hs_ticks)
{
    long result = 0;
    if (definition_index != -1) {
        long impulse_index = game_sound_scripted_impulse_find(definition_index);
        if (impulse_index != -1) {
            s_scripted_impulse_sound *impulse = &g_game_sound_scripted_impulses[impulse_index];
            float seconds = hs_ticks_to_seconds(hs_ticks);
            long ticks = game_seconds_to_ticks_round(seconds) - (game_time_get() - impulse->start_time);
            result = ticks > 0 ? ticks : 0;
        }
    }
    return result;
}


// @address 0x1fe038 ?scripted_sound_stop@@YAXJ@Z
void scripted_sound_stop(long definition_index)
{
    if (definition_index == -1)
        return;
    data_mine_insert_sp_audio_event("scripted sound stop", definition_index, -1, -1.0f);
    long impulse_index = game_sound_scripted_impulse_find(definition_index);
    if (impulse_index != -1) {
        s_scripted_impulse_sound *impulse = &g_game_sound_scripted_impulses[impulse_index];
        sound_stop_impulse(impulse->sound_index);
        impulse->sound_index = -1;
        impulse->start_time = -1;
        impulse->definition_index = -1;
    }
}


// @address 0x7e3834 g_game_sound_identifier_generator
s_music_identifier_generator g_game_sound_identifier_generator;
// @address 0x1fe398 ?music_looping_sound_start@@YAJJM@Z
long music_looping_sound_start(long definition_index, float scale)
{
    long sound_index = datum_new(game_looping_sound_data);
    if (sound_index != -1) {
        looping_sound_definition *definition =
            TAG_GET('lsnd', looping_sound_definition, definition_index);
        game_looping_sound_datum *sound = datum_get<game_looping_sound_datum>(game_looping_sound_data, sound_index);
        sound->type = _game_looping_sound_type_music;
        sound->state = _game_looping_sound_state_initial;
        sound->flags.clear();
        sound->flags.set(9, true);
        sound->scale = scale;
        sound->definition_index = definition_index;
        sound->object_index = -1;
        sound->location = -1;
        sound->flags.set(0, true);
        sound->unique_identifier = g_game_sound_identifier_generator.next();
        if (definition->fade_in_time > 0.0f)
            sound->timeout = game_time_get() + game_seconds_to_ticks_round(definition->fade_in_time);
        else
            sound->timeout = -1;
    }
    return sound_index;
}

// @address 0x1fe518 ?impact_looping_sound_start@@YAJJJ@Z
long impact_looping_sound_start(long definition_index, long impact_index)
{
    int sound_index;
    sound_index = -1;
    if (definition_index != -1) {
        sound_index = datum_new(game_looping_sound_data);
        if (sound_index != -1) {
            game_looping_sound_datum *sound = datum_get<game_looping_sound_datum>(game_looping_sound_data, sound_index);
            sound->type = _game_looping_sound_type_impact;
            sound->state = _game_looping_sound_state_initial;
            sound->flags.clear();
            sound->flags.set(9, true);
            sound->scale = 0.0f;
            sound->definition_index = definition_index;
            sound->object_index = -1;
            sound->location = (short)-1;
            sound->timeout = impact_index;
            sound->flags.set(0, true);
        }
    }
    return sound_index;
}
static __forceinline const s_sound_override_playback_location *game_sound_definition_try_and_get_override_location(s_cache_file_sound_definition *sound) {
 s_sound_playback_parameters *parameters =
     &g_sound_cache_file_gestalt.data->playback_parameters[sound->playback_index];
 const s_sound_override_playback_location *location = &parameters->override_location;
 const s_sound_override_playback_location *result = 0; if ((unsigned char)((parameters->override_location.flags & 7) != 0)) result = location; return result;
}
// @address 0x1ffee8 update_impulse_sound_output
extern "C" bool update_impulse_sound_output(
    long object_index, long sound_index, const s_sound_attachment_data *attachment, s_sound_output_parameters *output)
{
 if (attachment->flags & 1) {
  const s_sound_override_playback_location *location = game_sound_definition_try_and_get_override_location(TAG_GET('snd!', s_cache_file_sound_definition,
      sound_index));
  if (location) {
   if (location->flags & 1) output->flags |= 2;
   if ((unsigned char)((location->flags >> 1) & 1)) output->gain_bits = location->gain_bits;
   if ((unsigned char)((location->flags >> 2) & 1)) { output->flags |= 1; output->distance_bits = location->distance_bits; }
  }
 }
 if (attachment->flags & 2) {
  output->flags = 1;
  output->distance_bits = 0;
  output->gain_bits = 0xc2800000;
  output->range = attachment->additional_gain_range;
  if (attachment->flags & 4) output->flags = 65;
 }
 return attachment->flags != 0;
}

static __forceinline void game_sound_animation_manager_handle_lipsync_data_ending(c_animation_manager *manager, long sound)
{
    c_facial_animation_channel *channel = manager->get_mixing_board_facial_animation_channel();
    if (channel && channel->m_sound_index == sound)
        channel->m_state_flags.m_storage &= ~4;
}
// @address 0x200008 on_stop_object_impulse_sound
extern "C" void on_stop_object_impulse_sound(long object, long sound, long reason)
{
    object_datum *datum = (object_datum *)object_try_and_get_and_verify_type(object, ~0u);
    if (game_looping_sound_data->valid && datum && object_get(object)->animation.offset != -1)
    {
        c_animation_manager *manager = (c_animation_manager *)((char *)datum + datum->animation.offset);
        if (manager)
        {
            sound_datum *data = datum_get<sound_datum>(g_sound_data, sound);
            game_sound_animation_manager_handle_lipsync_data_ending(manager, data->definition_index);
        }
    }
}
// 0x200110: stops the object's impulse sound (when there is an object), then clears the scripted impulse that played this sound
// @address 0x200110 on_stop_scripted_impulse_sound
extern "C" void on_stop_scripted_impulse_sound(long object_index, long sound_index, long reason)
{
    if (object_index != -1)
        on_stop_object_impulse_sound(object_index, sound_index, reason);
    for (long impulse_index = 0; impulse_index < k_maximum_scripted_impulse_sounds; impulse_index++)
    {
        s_scripted_impulse_sound *impulse = &g_game_sound_scripted_impulses[impulse_index];
        if (impulse->sound_index == sound_index)
        {
            impulse->sound_index = -1;
            return;
        }
    }
}
// @address 0x200190 are_sound_attachments_equivalent
extern "C" bool are_sound_attachments_equivalent(const s_sound_attachment_data *a, const s_sound_attachment_data *b)
{
    return memcmp(a, b, sizeof(s_sound_attachment_data)) == 0;
}
// @address 0x2001c8 ?object_identifiers_are_equal@@YA_NJPBXJ0@Z
bool object_identifiers_are_equal(long object_index_a, const void *identifier_a, long object_index_b, const void *identifier_b)
{
    const s_sound_attachment_data *a = (const s_sound_attachment_data *)identifier_a;
    const s_sound_attachment_data *b = (const s_sound_attachment_data *)identifier_b;
    return a->object_index != -1 && b->object_index != -1 && a->object_index == b->object_index;
}


static __forceinline void game_sound_lipsync_flag_set(unsigned char *flags) { *flags |= 4; }
static __forceinline void game_sound_animation_manager_set_lipsync_data(
    c_animation_manager *manager, long sound_index, long permutation_index, float time)
{
    c_facial_animation_channel *channel = manager->get_mixing_board_facial_animation_channel();
    if (channel)
    {
        channel->m_frame = time;
        channel->m_sound_index = sound_index;
        channel->m_lipsync_index = permutation_index;
        game_sound_lipsync_flag_set(&channel->m_state_flags.m_storage);
    }
}
// @address 0x2001f8 ?game_sound_set_lipsync_data@@YAXJJPBUs_cache_file_sound_permutation@@M@Z
void game_sound_set_lipsync_data(long object_index, long sound_index, const s_cache_file_sound_permutation *permutation, float time)
{
    object_datum *datum = (object_datum *)object_try_and_get_and_verify_type(object_index, ~0u);
    if (game_looping_sound_data->valid && datum && object_get(object_index)->animation.offset != -1)
    {
        c_animation_manager *manager = (c_animation_manager *)((char *)datum + datum->animation.offset);
        if (manager)
            game_sound_animation_manager_set_lipsync_data(manager, sound_index, permutation->dialogue_info_index, time);
    }
}


// 0x2002e8: the offset plus the sizes of the first count items of the selected group
// @address 0x2002e8 game_sound_compute_group_item_offset
extern "C" long game_sound_compute_group_item_offset(long count, long offset)
{
    long total = 0;
    long selection_index = global_zone_set_index;
    for (long i = 0; i < count; i++)
        total += ((const s_scenario_bsp_machine_door_mapping *)((const s_scenario_zone_set_visibility *)global_scenario->zone_set_pvs.address)
            [((const s_scenario_zone_set *)global_scenario->zone_sets.address)[selection_index].pvs_index].machine_door_mappings.address)[i]
            .machine_door_portal_references.count;
    return total + offset;
}

// @address 0x200498 ?sound_attachment_is_sky@@YA_NJ@Z
bool sound_attachment_is_sky(long object_index)
{
  bool result = false;
  long root = -1;
  long current = object_index;
  if (current != -1) {
    do {
      root = current;
      current = object_header_get(current)->datum->parent_object_index;
    } while (current != -1);
  }
  unsigned short index = (unsigned short)root;
  s_object_header *header = object_header_get(index);
  if ((1 << header->object_type) & 0x40) {
    if (((const s_scenery_object *)header->datum)->flags & 4)
      result = true;
  }
  return result;
}

// @address 0x200528 ?scripted_looping_sound_get_sound_index@@YAJJ@Z
long scripted_looping_sound_get_sound_index(long owner)
{
    int index;
    for (index = data_next_index(game_looping_sound_data, -1); index != -1; index = next_allocated_datum_index(game_looping_sound_data,
        index)) {
        s_looping_sound_attachment_record *datum = datum_get<s_looping_sound_attachment_record>(game_looping_sound_data, index);
        if (datum->definition_index == owner) {
            unsigned char flag = (datum->flags.m_storage >> 5) & 1;
            if (flag)
                break;
        }
    }
    return index;
}
// @address 0x2005d0 ?scripted_looping_sound_reset_internal@@YAXJJ@Z
inline void scripted_looping_sound_reset_internal(long definition_index, long sound_index)
{
    long old_sound_index = ::scripted_looping_sound_get_sound_index(definition_index);
    if (old_sound_index != -1) {
        s_looping_sound_attachment_record *sound = datum_get<s_looping_sound_attachment_record>(game_looping_sound_data, old_sound_index);
        if (sound->definition_index == definition_index)
            sound->flags.set(5, false);
    }
    if (sound_index != -1) {
        s_looping_sound_attachment_record *sound = datum_get<s_looping_sound_attachment_record>(game_looping_sound_data, sound_index);
        if (sound->definition_index == definition_index) {
            sound->flags.set(5, true);
            sound->flags.set(6, true);
        }
    }
}


namespace {
static __forceinline void game_sound_unattached_looping_sound_stop(long sound_index)
{
    s_looping_sound_attachment_record *sound = datum_get<s_looping_sound_attachment_record>(game_looping_sound_data, sound_index);
    sound->flags.set(1, true);
}
}
// @address 0x200690 scripted_looping_sound_stop_datum
extern "C" void scripted_looping_sound_stop_datum(s_looping_sound_attachment_record *datum, long)
{
    long definition_index = datum->definition_index;
    if (definition_index != -1) {
        long sound_index = ::scripted_looping_sound_get_sound_index(definition_index);
        data_mine_insert_sp_audio_event("looping sound stop", datum->definition_index, -1, -1.0f);
        if (sound_index != -1) {
            s_looping_sound_attachment_record *sound = datum_get<s_looping_sound_attachment_record>(game_looping_sound_data, sound_index);
            sound->flags.set(1, true);
            long other_owner = datum->definition_index;
            long other_index = ::scripted_looping_sound_get_sound_index(other_owner);
            if (other_index != -1) {
                s_looping_sound_attachment_record *other = datum_get<s_looping_sound_attachment_record>(game_looping_sound_data, other_index);
                if (other->definition_index == other_owner)
                    other->flags.set(5, false);
            }
        }
        datum->flags.set(5, false);
        datum->flags.set(2, true);
    }
}
// @address 0x200768 ?scripted_looping_sound_stop_internal@@YAXJW4e_looping_sound_fadeout_type@@@Z
void scripted_looping_sound_stop_internal(long definition_index, e_looping_sound_fadeout_type fadeout_type)
{
    if (definition_index != -1) {
        long sound_index = ::scripted_looping_sound_get_sound_index(definition_index);
        data_mine_insert_sp_audio_event("looping sound stop", definition_index, -1, -1.0f);
        if (sound_index != -1) {
            s_looping_sound_attachment_record *sound = datum_get<s_looping_sound_attachment_record>(game_looping_sound_data, sound_index);
            sound->flags.set(5, false);
            game_sound_unattached_looping_sound_stop(sound_index);
            scripted_looping_sound_reset_internal(definition_index, -1);
            if (fadeout_type == _looping_sound_fadeout_type_immediate)
                sound->flags.set(3, true);
        }
    }
}


// @address 0x1fe0f0 ?scripted_looping_sound_start_with_effect@@YAXJJMJ@Z
void scripted_looping_sound_start_with_effect(long definition_index, long object_index, float scale, long effect_label)
{
    if (definition_index == -1)
        return;

    looping_sound_definition *definition = TAG_GET('lsnd', looping_sound_definition, definition_index);
    data_mine_insert_sp_audio_event("looping sound start", definition_index, object_index, scale);
    scripted_looping_sound_stop_internal(definition_index, _looping_sound_fadeout_type_default);
    if (definition->flags & 4) {
        for (long sound_index = ::data_next_index(game_looping_sound_data,
            -1); sound_index != -1; sound_index = next_allocated_datum_index(game_looping_sound_data, sound_index)) {
            s_looping_sound_attachment_record *sound = datum_get<s_looping_sound_attachment_record>(game_looping_sound_data, sound_index);
            if ((sound->type == _game_looping_sound_type_unattached || sound->type == _game_looping_sound_type_music) &&
                looping_sound_definition_is_music(sound->definition_index))
                scripted_looping_sound_stop_datum(sound, 1);
        }
    }

    bool music = looping_sound_definition_is_music(definition_index);
    long sound_index;
    if (music)
        sound_index = music_looping_sound_start(definition_index, scale);
    else
        sound_index = unattached_looping_sound_start_with_effect(definition_index, object_index, scale, effect_label);
    if (sound_index != -1) {
        scripted_looping_sound_reset_internal(definition_index, sound_index);
        if (music) {
            bool in_progress = false;
            if (cinematic_globals)
                in_progress = cinematic_globals->in_progress;
            if (in_progress) {
                s_looping_sound_attachment_record *sound = game_sound_attachment_datum_get(game_looping_sound_data, sound_index);
                sound->flags.set(7, true);
            }
        }
    }
}

// @address 0x200850 ?looping_sound_definition_is_music@@YA_NJ@Z
namespace {


static __forceinline looping_sound_definition *game_sound_music_check_definition_tag_get(long index)
{
    return TAG_GET('lsnd', looping_sound_definition, index);
}
static __forceinline s_cache_file_sound_definition *game_sound_music_check_sound_definition_tag_get(long index)
{
    return TAG_GET('snd!', s_cache_file_sound_definition, index);
}
static __forceinline bool game_sound_music_check_sound_class_is_music(short sound_class) { return sound_class == 32 || sound_class == 38; }
}
bool looping_sound_definition_is_music(long definition_index)
{
    bool result = false;
    looping_sound_definition *definition = game_sound_music_check_definition_tag_get(definition_index);
    for (short i = 0; i < definition->tracks.count; i++)
    {
        looping_sound_track_definition *element = &((looping_sound_track_definition *)definition->tracks.address)[i];
        if (element->loop.index != -1)
        {
            s_cache_file_sound_definition *sound = game_sound_music_check_sound_definition_tag_get(element->loop.index);
            if (game_sound_music_check_sound_class_is_music(sound->sound_class))
            {
                result = true;
                break;
            }
        }
    }
    return result;
}


namespace {
static __forceinline bool game_sound_first_person_spatialization(long user_index)
{
    return ((1 << tls_director_globals()->slots[user_index].perspective) & 3) != 0;
}

}
// @address 0x2008f0 ?update_spatialization_if_first_person@@YAXJPADPAJ@Z
void update_spatialization_if_first_person(long object_index, char *spatialization, long *user_index)
{
    bool first_person = false;
    if (user_index)
        *user_index = -1;
    if ((unsigned char)*spatialization == 1) {
        for (long user = 0; user < 4; ++user) {
            if (players_user_is_active(user) && game_sound_first_person_spatialization(user) && user != -1) {
                long unit_index = tls_player_mapping_globals()->unit_indices[user];
                if (unit_index != -1 && (object_owns_object(unit_index, object_index) || object_owns_object(object_index, unit_index))) {
                    first_person = true;
                    if (user_index) {
                        *user_index = user;
                        break;
                    } else {
                        break;
                    }
                }
            }
        }
    }
    if (first_person)
        *spatialization = 0;
}

// @address 0x200a28 ?data_mine_insert_sp_audio_event@@YAXPBDJJM@Z
void data_mine_insert_sp_audio_event(const char *event_name, long definition_index, long object_index, float scale)
{
    if (game_is_campaign() && definition_index != -1) {
        c_datamine datamine(0, event_name, 2, "sound", "campaign");
    }
}

// @address 0x200a88 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_game_looping_sound_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_looping_sound_data_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[1].add_member(name, "", size, 0, __tls_set_g_game_looping_sound_data_allocator, 0, 0);
    unsigned int offset = g_restricted_regions[1].get_member_offset(member_index);
    data_set_new_base_address(&game_looping_sound_data.get_restricted_data_array_address(), (s_data_array *)(g_restricted_address[1] + offset));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return g_restricted_address[1] + g_restricted_regions[1].get_member_offset(member_index);
}
// @address 0x200b28 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_game_looping_sound_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_looping_sound_data_allocator>::deallocate(void *allocation)
{
    game_looping_sound_data = 0;
    restricted_region_free_member(1, m_member_index);
}
// @address 0x200be8 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_game_sound_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
// @address 0x200c00 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_game_sound_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_globals_allocator>::deallocate(void *allocation)
{
    int value = m_member_index;
    g_game_sound_globals = 0;
    restricted_region_free_member(1, value);
}
// @address 0x200cc0 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_game_sound_scripted_impulses_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_scripted_impulses_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
// @address 0x200cd8 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_game_sound_scripted_impulses_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_scripted_impulses_allocator>::deallocate(void *allocation)
{
    g_game_sound_scripted_impulses = 0;
    restricted_region_free_member(1, m_member_index);
}







// @address 0x2d6d78 scripted_music_stop_all
extern "C" void scripted_music_stop_all(short function_index, long thread_index, bool initialize)
{
    hs_value *arguments = (hs_value *)hs_macro_function_evaluate(function_index, thread_index, initialize);
    if (arguments) {
        long sound_index = arguments[3].long_value;
        long shot_index = arguments[2].long_value;
        s_cinematic_scene *scene = scripted_camera_get_scene(arguments[0].long_value, arguments[1].long_value);
        if (scene && shot_index >= 0 && shot_index < scene->shots.count) {
            hs_cinematic_shot *shot = (hs_cinematic_shot *)tag_block_get_element_with_size(&scene->shots, shot_index, sizeof(hs_cinematic_shot));
            if (shot && sound_index >= 0 && sound_index < shot->music.count) {
                s_cinematic_scene_music *sound = (s_cinematic_scene_music *)tag_block_get_element_with_size(
                    &shot->music, sound_index, sizeof(s_cinematic_scene_music));
                if (sound->sound.group_tag == 'lsnd') {
                    scripted_looping_sound_stop_internal(sound->sound.index, _looping_sound_fadeout_type_default);
                }
            }
        }
        hs_return(thread_index, 0);
    }
}

// @address 0x4fb828 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_ai_players_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void ai_player_allocation_manager::deallocate(void *allocation)
{
    int value = m_member_index;
    tls_ai_players_set(0);
    restricted_region_free_member(1, value);
}

// @address 0x707dc0 game_sound_register_shared_empty_destructor
void game_sound_register_shared_empty_destructor(void)
{
    atexit(sound_xaudio_xenon_noop_callback);
}





static __forceinline long game_sound_looping_sound_get_object_index(const s_looping_sound_attachment_record *looping_sound)
{
    return looping_sound->type.m_value == _game_looping_sound_type_object ? looping_sound->source.object.object_index : -1;
}

static __forceinline long game_sound_looping_sound_source_index(long looping_sound_index, long object_index)
{
    if (object_index == -1)
        return looping_sound_index | 0x2000;
    return object_index;
}

static __forceinline long game_sound_looping_sound_identifier(long looping_sound_index)
{
    s_looping_sound_attachment_record *looping_sound = game_sound_attachment_datum_get(game_looping_sound_data, looping_sound_index);
    long identifier = looping_sound_index;
    if (looping_sound->type.m_value == _game_looping_sound_type_unattached && looping_sound->source.unattached.user_index.m_value != -1)
        identifier = looping_sound->definition_index | 0x8000;
    else if (looping_sound->type.m_value == _game_looping_sound_type_music)
        identifier = looping_sound->source.object.identifier | 0xa000;
    return identifier;
}

// @address 0x128328 ?unattached_looping_sound_get_amplitude@@YAMJ@Z
float unattached_looping_sound_get_amplitude(long looping_sound_index)
{
    s_looping_sound_attachment_record *looping_sound = game_sound_attachment_datum_get(game_looping_sound_data, looping_sound_index);
    looping_sound_definition *definition =
        TAG_GET('lsnd', looping_sound_definition, looping_sound->definition_index);
    long source_index = -1;
    if (((definition->flags & 0x10) && looping_sound->type.m_value == _game_looping_sound_type_object) || (definition->flags & 0x20))
        source_index = game_sound_looping_sound_source_index(looping_sound_index, game_sound_looping_sound_get_object_index(looping_sound));
    s_sound_source source;
    source.flags = 0;
    game_looping_sound_get_source(looping_sound_index, &source);
    return looping_sound_try_to_get_mouth_apeture(
        looping_sound->definition_index, game_sound_looping_sound_identifier(looping_sound_index), source_index, &source);
}
struct s_sound_tracker;
// @address 0xf8500 ?sound_class_get@@YAPBUsound_class_definition@@F@Z
// @address 0x23e0f0 ?game_sound_deterministic_permutation_valid@@YA_NJPBUs_game_sound_deterministic_permutation@@@Z
// @address 0xf8550 ?sound_effect_manager_create_impulse@@YAJJPAUs_sound_impulse_definition@@@Z
static __forceinline bool game_sound_5_class_always_use_speakers(const sound_class_definition *definition) { return (bool)((definition->flags >> 7) & 1); }
static __forceinline bool game_sound_5_class_is_scripted_speech(const sound_class_definition *definition)
{
    return (bool)((definition->internal_flags >> 1) & 1) && (bool)(((definition->internal_flags >> 2) & 0x3fff) & 1);
}
static __forceinline bool game_sound_5_source_test(unsigned short flags, long bit) { return (bool)((flags >> bit) & 1); }
// @address 0x1fda88 ?game_sound_impulse_new_internal@@YAJJPAUs_game_sound_impulse_internal_parameters@@@Z
long game_sound_impulse_new_internal(long sound_index, s_game_sound_impulse_internal_parameters *parameters)
{
    long result = -1;
    if (sound_index != -1) {
        const s_cache_file_sound_definition *sound =
            TAG_GET('snd!', const s_cache_file_sound_definition, sound_index);
        long source_type;
        if (parameters->additional_override_flags & 1)
            source_type = parameters->source_type;
        else if ((parameters->source.spatialization & 0xf0) && !(sound->flags & 0x400))
            source_type = 3;
        else
            source_type = 1;
        s_sound_impulse_definition impulse;
        impulse.flags = 5;
        if (parameters->tracking) impulse.flags |= 32; else impulse.flags &= ~32;
        if (parameters->object_index != -1) impulse.flags |= 8; else impulse.flags &= ~8;
        if (parameters->playback_label != -1) impulse.flags |= 256; else impulse.flags &= ~256;
        impulse.playback_flags = source_type;
        memcpy(&impulse.source, &parameters->source, sizeof(impulse.source));
        short sound_class = sound->sound_class;
        impulse.object_index = parameters->object_index;
        impulse.callbacks = parameters->tracking;
        impulse.playback_index = parameters->playback_label;
        if (game_sound_5_class_always_use_speakers(sound_class_get(sound_class)) && !(sound->flags & 128))
            impulse.source.flags |= 2048;
        if (parameters->tracking_data) {
            impulse.tracking_data = *parameters->tracking_data;
            impulse.event_size = 40;
        } else {
            memset(&impulse.event_parameters, 0, sizeof(impulse.event_parameters));
            impulse.event_size = 0;
        }
        if (parameters->permutation && game_sound_deterministic_permutation_valid(sound_index, parameters->permutation)) {
            impulse.flags |= 1024;
            unsigned short pitch_range_index = parameters->permutation->pitch_range;
            short permutation_index = parameters->permutation->permutation;
            impulse.pitch_range = pitch_range_index;
            impulse.permutation = permutation_index;
        }
        result = sound_effect_manager_create_impulse(sound_index, &impulse);
        if (result != -1) {
            if (game_sound_5_class_is_scripted_speech(sound_class_get(sound->sound_class))) {
                long ticks = game_seconds_to_ticks_round((float)sound->maximum_play_time * 0.001f);
                long time = game_time_get() + ticks + 10;
                if (game_sound_5_source_test(impulse.source.flags, 8) || game_sound_5_source_test(impulse.source.flags, 11) ||
                    !(impulse.source.spatialization & 0xf0))
                {
                    game_sound_globals *globals = (game_sound_globals *)g_game_sound_globals;
                    globals->scripted_dialog_timer = time > globals->scripted_dialog_timer ? time : globals->scripted_dialog_timer;
                }
            }
        }
    }
    return result;
}

extern "C" {
// @address 0x35fec k_scripted_impulse_sound_tracker
}
// @address 0x2879a8 ?game_sound_find_platform_playback_by_label@@YAJJ@Z
// @address 0xfcf30 ?internal_object_get_markers_by_string_id@@YAFJJPAUobject_marker@@F_N@Z
// @address 0x1fcb08 ?object_impulse_sound_new_internal@@YAJJJW4e_sound_spatialization_mode@@PBUs_sound_attachment_data@@PBUs_sound_tracker@@MJPBUs_game_sound_deterministic_permutation@@@Z
long object_impulse_sound_new_internal(long object_index, long sound_index, e_sound_spatialization_mode spatialization,
    const s_sound_attachment_data *attachment, const s_sound_tracker *tracker, float volume, long playback_index,
    const s_game_sound_deterministic_permutation *permutation)
{
    int spatialization_flags[4];
    s_game_sound_impulse_internal_parameters request;
    long result;
    *(unsigned char *)spatialization_flags = (unsigned char)spatialization;
    request.source.flags = 0;
    request.additional_override_flags = 0;
    result = -1;
    update_spatialization_if_first_person(object_index, (char *)spatialization_flags, 0);
    signed char spatialization_byte = *(unsigned char *)spatialization_flags;
    request.source.volume = volume;
    void *handler = tracker->callbacks[0];
    int spatialization_nibbles = spatialization_byte;
    request.source.volume_type = 0;
    request.source.flags = 0;
    request.source.name = 0;
    location_invalidate(&request.source.location);
    request.permutation = permutation;
    request.source.spatialization = (spatialization_nibbles & 15) | (spatialization_nibbles << 4);
    if ((unsigned char)((long (*)(long, long, const s_sound_attachment_data *, s_sound_source *, float))handler)(
            object_index, sound_index, attachment, &request.source, volume) != 0) {
        request.object_index = object_index;
        request.tracking_data = (const s_sound_tracking_data *)attachment;
        request.playback_label = playback_index;
        request.tracking = tracker;
        result = game_sound_impulse_new_internal(sound_index, &request);
    }
    return result;
}
// @address 0x1fd770 scripted_impulse_sound_start_at_location
extern "C" void scripted_impulse_sound_start_at_location(long definition_index, const s_sound_location *location, float scale,
    real_decibel gain, real_decibel distance, long playback_label, float *playing_time)
{
    long sound_index;
    if (playing_time) *playing_time = 0.0f;
    if (definition_index != -1) {
        long scripted_sound_index = scripted_sound_create_internal(definition_index, 0);
        if (scripted_sound_index != -1) {
            s_scripted_impulse_sound *sound = &g_game_sound_scripted_impulses[scripted_sound_index];
            if (playing_time)
                *playing_time = game_sound_deterministic_get_permutation_playing_time(
                    definition_index, (const s_game_sound_deterministic_permutation *)&sound->permutation);
            const s_sound_override_playback_location *override_location = game_sound_definition_try_and_get_override_location(
                TAG_GET('snd!', s_cache_file_sound_definition, definition_index));
            bool spatialized = override_location && (override_location->flags & 1);
            data_mine_insert_sp_audio_event("scripted sound start", definition_index, -1, 1.0f);
            if (spatialized) {
                s_sound_attachment_data attachment;
                attachment.flags = 1;
                sound_index = object_impulse_sound_new_internal(-1, definition_index, _sound_spatialization_mode_spatialized, &attachment,
                    &k_scripted_impulse_sound_tracker, 1.0f, game_sound_find_platform_playback_by_label(0),
                    (const s_game_sound_deterministic_permutation *)&sound->permutation);
            } else {
                s_game_sound_impulse_internal_parameters parameters;
                parameters.additional_override_flags = spatialized;
            parameters.source.flags = 1024;
            parameters.source.volume_type = 0;
                memcpy(&parameters.source.position, location, sizeof(game_sound_location_block));
                parameters.source.spatialization = 17;
                parameters.source.volume = 1.0f;
                parameters.source.name = 0;
                parameters.object_index = -1;
                parameters.playback_label = game_sound_find_platform_playback_by_label(0);
                parameters.tracking_data = 0;
                parameters.tracking = &k_scripted_impulse_sound_tracker;
                parameters.permutation = (const s_game_sound_deterministic_permutation *)&sound->permutation;
                sound_index = game_sound_impulse_new_internal(definition_index, &parameters);
            }
            g_game_sound_scripted_impulses[scripted_sound_index].sound_index = sound_index;
        }
    }
}
// @address 0x1fd940 ?scripted_sound_create_internal@@YAJJPBUs_game_sound_deterministic_permutation@@@Z
long scripted_sound_create_internal(long sound_index, const s_game_sound_deterministic_permutation *permutation)
{
    long existing_index = -1;
    long free_index = -1;
    for (long index = 0; index < k_maximum_scripted_impulse_sounds; index++) {
        s_scripted_impulse_sound *impulse = &g_game_sound_scripted_impulses[index];
        if (impulse->definition_index == sound_index) {
            if (impulse->unit_source == 0) {
                if (impulse->sound_index != -1) {
                    sound_stop_impulse(impulse->sound_index);
                    impulse->sound_index = -1;
                }
            }
            existing_index = index;
        } else if (free_index == -1) {
            if (impulse->definition_index != -1) {
                float playing_time = game_sound_deterministic_get_permutation_playing_time(
                    impulse->definition_index, (const s_game_sound_deterministic_permutation *)&impulse->permutation);
                if (game_seconds_to_ticks_round(playing_time) + impulse->start_time > game_time_get()) continue;
                free_index = index;
            } else {
                free_index = index;
            }
        }
    }
    if (existing_index == -1 && free_index != -1) existing_index = free_index;
    if (existing_index != -1) {
        s_scripted_impulse_sound *impulse = &g_game_sound_scripted_impulses[existing_index];
        game_sound_deterministic_pick_permutation(sound_index, (s_game_sound_deterministic_permutation *)&impulse->permutation, 0);
        impulse->unit_source = 0;
        long time = game_time_get();
        impulse->definition_index = sound_index;
        impulse->sound_index = -1;
        impulse->start_time = time;
    }
    return existing_index;
}
// @address 0x200a28 ?data_mine_insert_sp_audio_event@@YAXPBDJJM@Z

static __forceinline int game_sound_sound_datum_valid(int handle)
{
    if (handle == -1) return 0;
    int index = handle & 65535;
    s_data_array *array = g_sound_data;
    if (index >= array->first_unallocated) return 0;
    short *datum = (short *)(array->size * index + array->data);
    short &salt = *datum;
    if (salt == 0) return 0;
    if (salt != handle >> 16) return 0;
    return 1;
}


// @address 0x1fc558 ?game_sound_reset_at_round_finish@@YAXXZ
void game_sound_reset_at_round_finish()
{
    if (game_engine_running() && ((s_sound_manager_globals *)g_sound_manager_globals)->initialized) {
        sound_effect_manager_stop_all();
        for (long sound_index = ::data_next_index((::s_data_array *)g_sound_data, -1); sound_index != -1;
            sound_index = next_allocated_datum_index(g_sound_data, sound_index))
        {
            sound_datum *sound = (sound_datum *)((sound_datum *)g_sound_data->data) + (sound_index & 0xffff);
            if (!sound->type)
                sound_stop_and_unlock(sound_index, 10);
        }
    }
}
// @address 0x1fd0c8 ?submit_unspatialized_sound@c_game_sound_impulse_parameters@@QAAJXZ
long c_game_sound_impulse_parameters::submit_unspatialized_sound()
{
    s_game_sound_impulse_internal_parameters internal;
    internal.source.volume = scale;
    internal.additional_override_flags = 0;
    internal.source.volume_type = 0;
    internal.source.flags = 0;
    internal.source.spatialization = 0;
    internal.source.name = 0;
    internal.object_index = -1;
    internal.playback_label = game_sound_find_platform_playback_by_label(sound_impulse_index);
    internal.tracking_data = 0;
    internal.tracking = 0;
    internal.permutation = deterministic;
    return game_sound_impulse_new_internal(definition_index, &internal);
}
// 0x1fd398: no dev-build name; flags the sound of a scripted impulse (0x400) unless the impulse is flagged or its sound is active
// @address 0x1fd398 scripted_sound_set_outro_flag
extern "C" void scripted_sound_set_outro_flag(long definition_index)
{
    long impulse_index = -1;
    s_scripted_impulse_sound *impulses = g_game_sound_scripted_impulses;
    for (long index = 0; index < k_maximum_scripted_impulse_sounds; index++)
    {
        if (impulses[index].definition_index == definition_index)
        {
            impulse_index = index;
            break;
        }
    }
    if (impulse_index == -1) return;
    s_scripted_impulse_sound *impulse = &impulses[impulse_index];
    if (impulse->unit_source) return;
    int handle = impulse->sound_index;
    if (game_sound_sound_datum_valid(handle))
    {
        sound_datum *datum = (sound_datum *)((sound_datum *)g_sound_data->data) + (handle & 65535);
        if (!datum->type) datum->flags.m_flags |= 0x400;
    }
}
// @address 0x1fd468 ?scripted_sound_new_internal@@YAXJJMUreal_decibel@@0JPAM@Z
void scripted_sound_new_internal(long definition_index, long object_index, float scale, real_decibel gain, real_decibel distance,
    long playback_label, float *playing_time)
{
    long sound_index = -1;
    if (playing_time) *playing_time = 0.0f;
    if (definition_index != -1) {
        long scripted_sound_index = scripted_sound_create_internal(definition_index, 0);
        if (scripted_sound_index != -1) {
            s_scripted_impulse_sound *sound = &g_game_sound_scripted_impulses[scripted_sound_index];
            if (playing_time)
                *playing_time = game_sound_deterministic_get_permutation_playing_time(
                    definition_index, (const s_game_sound_deterministic_permutation *)&sound->permutation);
            const s_sound_override_playback_location *override_location = game_sound_definition_try_and_get_override_location(
                TAG_GET('snd!', s_cache_file_sound_definition, definition_index));
            bool spatialized = override_location && (override_location->flags & 1);
            data_mine_insert_sp_audio_event("scripted sound start", definition_index, object_index, scale);
            scale = ((((scale) > (0.0f) ? (scale) : (0.0f))) < (1.0f) ? (((scale) > (0.0f) ? (scale) : (0.0f))) : (1.0f));
            if (object_index == -1 && !spatialized) {
                s_game_sound_impulse_internal_parameters parameters;
                parameters.additional_override_flags = 0;
                parameters.source.flags = 1024;
                parameters.source.volume_type = 0;
                parameters.source.spatialization = 0;
                parameters.source.volume = scale;
                parameters.source.name = 0;
                parameters.object_index = -1;
                parameters.playback_label = game_sound_find_platform_playback_by_label(playback_label);
                parameters.tracking_data = 0;
                parameters.tracking = &k_scripted_impulse_sound_tracker;
                parameters.permutation = (const s_game_sound_deterministic_permutation *)&sound->permutation;
                sound_index = game_sound_impulse_new_internal(definition_index, &parameters);
            } else {
                s_sound_attachment_data attachment;
                attachment.flags = 0;
                if (object_index != -1) {
                    object_marker marker;
                    internal_object_get_markers_by_string_id(object_index, _string_id_head, &marker, 1, false);
                    object_impulse_sound_initialize_attachment_data(definition_index, object_index, marker.node_index,
                        &marker.node_to_marker.position, &marker.node_to_marker.forward, &attachment);
                }
                if (override_location) attachment.flags |= 1; else attachment.flags &= ~1;
                sound_index = object_impulse_sound_new_internal(object_index, definition_index,
                    (e_sound_spatialization_mode)((spatialized != 0) + 1), &attachment, &k_scripted_impulse_sound_tracker, scale,
                    game_sound_find_platform_playback_by_label(playback_label),
                    (const s_game_sound_deterministic_permutation *)&sound->permutation);
            }
            g_game_sound_scripted_impulses[scripted_sound_index].sound_index = sound_index;
        }
        if (object_index != -1) {
            object_type_definition *definition = object_type_definitions[object_header_get(object_index)->datum->type];
            for (short index = 0; definition->parts[index]; index++) {
                if (definition->parts[index]->scripted_sound)
                    definition->parts[index]->scripted_sound(object_index, definition_index, sound_index);
            }
        }
    }
}
// @address 0x1a35d0 ?sound_stop_impulse@@YAXJ@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x1ec160 ?data_new@@YAPAUs_data_array@@PBDJJJPAVc_allocation_base@@@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x1fb270 ?sound_class_set_gain_db@@YAXPBDUreal_decibel@@MJ@Z
// @address 0x23dff8 ?game_sound_deterministic_get_permutation_playing_time@@YAMJPBUs_game_sound_deterministic_permutation@@@Z
// @address 0x24dac0 ?hs_arguments_evaluate@@YAPAJJFPBF_N@Z
// @address 0xa2b80 ?quantize_real@@YAJMMMJ_N0@Z
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0xab8a0 ?hs_return@@YAXJJ@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xf5268 ?game_engine_running@@YA_NXZ
// @address 0xf9528 ?real_gain_to_decibels@@YA?AUreal_decibel@@M@Z
// @address 0xd93f8 ?normalize3d_with_default@@YAMPATreal_vector3d@@PBT1@@Z
// @address 0xfd350 ?object_owns_object@@YA_NJJ@Z
// @address 0x73e758 ?hs_function_table@@3PAPBUhs_function_definition@@A

// @address 0x1fd070 ?unspatialized_impulse_sound_new@@YAJJM@Z
long unspatialized_impulse_sound_new(long sound_index, float scale)
{
    s_game_sound_impulse_internal_parameters parameters;
    parameters.additional_override_flags = 0;
    parameters.source.volume_type = 0;
    parameters.source.flags = 0;
    parameters.source.spatialization = 0;
    parameters.source.volume = scale;
    parameters.source.name = 0;
    parameters.object_index = -1;
    parameters.playback_label = -1;
    parameters.tracking_data = 0;
    parameters.tracking = 0;
    parameters.permutation = 0;
    return game_sound_impulse_new_internal(sound_index, &parameters);
}

// @address 0x1fe310 ?unattached_looping_sound_start@@YAJJJM@Z
long unattached_looping_sound_start(long definition_index, long object_index, float scale)
{
    long sound_index = -1;
    if (definition_index != -1) {
        sound_index = game_looping_sound_attachment_new(object_index, definition_index, -1, 0);
        if (sound_index != -1) {
            s_looping_sound_attachment_record *sound = game_sound_attachment_datum_get(
                game_looping_sound_data, sound_index);
            sound->flags.set(0, true);
            sound->scale = scale;
        }
    }
    return sound_index;
}

// @address 0x1f66e0 ?sound_permutation_get_real_mouth_aperture@@YAMJFFF@Z
static __forceinline bool game_sound_5_class_is_speech(const sound_class_definition *definition) { return (bool)((definition->internal_flags >> 1) & 1); }
// @address 0x1ffd40 ?update_unit_impulse_sound@@YAXJJJJM@Z
void update_unit_impulse_sound(long object_index, long sound_index, long pitch_range_index, long permutation_index, float time)
{
    const s_cache_file_sound_definition *sound =
        TAG_GET('snd!', const s_cache_file_sound_definition, sound_index);
    if (!(sound->flags & 0x100) && game_sound_5_class_is_speech(sound_class_get(sound->sound_class))) {
        if (sound->extra_info_index != -1) {
            const s_tag_block *extra_info = (const s_tag_block *)g_sound_cache_file_gestalt.data->extra_infos.address +
                sound->extra_info_index;
            if (extra_info != 0 && extra_info->count > 0) {
                const s_sound_cache_file_gestalt_definition *gestalt = (const s_sound_cache_file_gestalt_definition *)g_sound_cache_file_gestalt.data;
                const s_cache_file_sound_pitch_range *permutation_range =
                    ((const s_cache_file_sound_pitch_range *)g_sound_cache_file_gestalt.data->pitch_ranges.address) +
                    (sound->pitch_range_index + pitch_range_index);
                if (game_looping_sound_data->valid && object_try_and_get_and_verify_type(object_index, 4099) != 0) {
                    unit_datum *datum = (unit_datum *)object_header_get(object_index)->datum;
                    float previous = datum->unit.mouth_aperture;
                    float delta = sound_permutation_get_real_mouth_aperture(
                                      sound_index, (short)pitch_range_index, (short)permutation_index, (short)(long)(time * 30.0f + 0.5f)) -
                        previous;
                    if (delta <= -0.3f)
                        delta = -0.3f;
                    else if (delta >= 0.3f)
                        delta = 0.3f;
                    datum->unit.mouth_aperture += delta;
                }
                long first = permutation_range->first_permutation_index;
                game_sound_set_lipsync_data(object_index, sound_index,
                    ((const s_cache_file_sound_permutation *)gestalt->permutations.address) + (first + permutation_index), time);
            }
        }
    }
}
static __forceinline const s_cache_file_sound_pitch_range *game_sound_notify_pitch_range(const s_cache_file_sound_definition *definition, long index)
{
    return &((const s_cache_file_sound_pitch_range *)g_sound_cache_file_gestalt.data->pitch_ranges.address)[definition->pitch_range_index + index];
}
static __forceinline const s_cache_file_sound_permutation *game_sound_notify_permutation(const s_cache_file_sound_pitch_range *range)
{
    return &((const s_cache_file_sound_permutation *)g_sound_cache_file_gestalt.data->permutations.address)[range->first_permutation_index];
}
// @address 0x1ffc90 ?notify_object_impulse_sound@@YAXJPBUs_sound_attachment_data@@JJPBUs_cache_file_sound_permutation@@M@Z
void notify_object_impulse_sound(long object_index, const s_sound_attachment_data *attachment, long sound_index, long pitch_range_index,
    const s_cache_file_sound_permutation *permutation, float time)
{
    if (object_index != -1 && permutation && object_try_and_get_and_verify_type(object_index, 0x1003)) {
        const s_cache_file_sound_definition *sound =
            TAG_GET('snd!', const s_cache_file_sound_definition, sound_index);
        const s_cache_file_sound_pitch_range *permutation_range = game_sound_notify_pitch_range(sound, pitch_range_index);
        update_unit_impulse_sound(object_index, sound_index, pitch_range_index, (long)(permutation - game_sound_notify_permutation(permutation_range)), time);
    }
}

// 0x1ff568: advances scripted impulses, clearing the ones whose object is gone or whose playing time elapsed
// @address 0x1ff568 ?game_sound_impulses_update@@YAXXZ
void game_sound_impulses_update(void)
{
    for (long index = 0; index < k_maximum_scripted_impulse_sounds; index++) {
        s_scripted_impulse_sound *impulse = &g_game_sound_scripted_impulses[index];
        if (impulse->unit_source != 0) {
            bool valid = true;
            if (object_try_and_get_and_verify_type(impulse->sound_index, 0x1003) != 0) {
                float elapsed = (float)(double)(game_time_get() - impulse->start_time) * game_tick_length();
                float playing_time = game_sound_deterministic_get_permutation_playing_time(
                    impulse->definition_index, (const s_game_sound_deterministic_permutation *)&impulse->permutation);
                update_unit_impulse_sound(impulse->sound_index, impulse->definition_index, (long)impulse->pitch_range_index,
                    (long)impulse->permutation_index, elapsed);
                valid = elapsed > game_tick_length() * 10.0f + playing_time;
            }
            if (valid) {
                impulse->unit_source = 0;
                impulse->sound_index = -1;
            }
        }
    }
}

// @address 0xf83a8 ?unattached_impulse_sound_new@@YAJJPBUs_sound_location@@M@Z
long unattached_impulse_sound_new(long handle, const s_sound_location *location, float volume)
{
    s_game_sound_impulse_internal_parameters parameters;
    parameters.additional_override_flags = 0;
    parameters.source.volume_type = 0;
    parameters.source.flags = 0;
    memcpy(&parameters.source.position, location, sizeof(game_sound_location_block));
    long result;
    parameters.source.volume = volume;
    parameters.source.name = 0;
    parameters.source.spatialization = 17;
    parameters.tracking_data = 0;
    parameters.tracking = 0;
    parameters.permutation = 0;
    parameters.object_index = -1;
    parameters.playback_label = -1;
    if (g_restricted_address[1] != 0) {
        result = game_sound_impulse_new_internal(handle, &parameters);
    } else {
        game_sound_render_message *message = (game_sound_render_message *)InterlockedPopEntrySList(&g_game_sound_message_queue.free_list);
        __lwsync();
        if (message != 0) {
            message->handle = handle;
            message->location = *(const game_sound_location_block *)location;
            message->value = volume;
            __lwsync();
            InterlockedPushEntrySList(&g_game_sound_message_queue.pending_list, message);
        }
        result = -1;
    }
    return result;
}

// @address 0xa6f80 game_sound_update_timeline
extern "C" void game_sound_update_timeline(void)
{
    chud_dispose_state *state = g_chud_cinematic_state;
    if (state->definition_index != -1) {
        s_cortana_effect_definition *definition =
            TAG_GET('crte', s_cortana_effect_definition, state->definition_index);
        long ticks = (long)(30.0f * ((float)(double)(game_time_get() - state->start_time) * game_tick_length()) + 0.5f);
        long elapsed = ticks - state->last_ticks;
        state->last_ticks = ticks;
        if (elapsed > 0) {
            long count = definition->sounds.count();
            if (count > 0) {
                float scale = 1.0f;
                long offset = 0;
                long remaining = count;
                do {
                    s_cortana_effect_sound *event = (s_cortana_effect_sound *)((char *)definition->sounds.address + offset);
                    long sound_index = event->sound.index;
                    if (sound_index != -1) {
                        long time = event->time;
                        if (time <= g_chud_cinematic_state->last_ticks && time > g_chud_cinematic_state->last_ticks - elapsed) {
                            unspatialized_impulse_sound_new(sound_index, scale);
                        }
                    }
                    remaining--;
                    offset += sizeof(s_cortana_effect_sound);
                } while ((unsigned int)remaining != 0);
            }
        }
        state = g_chud_cinematic_state;
        if (state->last_ticks >= state->end_ticks && state->definition_index != -1) {
            chud_stop_screen_effect();
        }
    }
}
// @address 0x35f08 k_scripted_sound_default_gain
// @address 0x35f08 scripted_sound_constant_pool_anchor
// The gain aliases the first member of the pooled tracker constants below.

// Extend the existing gain window through the neighboring tracker tables.
// @rdata 0x82035f08..0x8203602c
static const s_sound_tracker_constant_pool k_game_sound_tracker_prefix = {
    { 0.0f }, { -64.0f }, {
    32, 0, 1, 26, 2, 23, 27, 0, 3, 16, 24, 30, 28, 11, 0, 13, 4, 7, 17,
    0, 25, 22, 31, 15, 29, 10, 12, 6, 0, 21, 14, 9, 5, 20, 8, 19, 18
    }, { 0x42df18dc, 0x3d088889 }, {
    { { (void *)track_object_impulse_sound, (void *)object_impulse_playing_in_channel,
        (void *)notify_object_impulse_sound, 0, (void *)on_stop_object_impulse_sound, 0, (void *)are_sound_attachments_equivalent, 0 } },
    { { (void *)track_object_impulse_sound, (void *)object_impulse_playing_in_channel,
        (void *)notify_object_impulse_sound, 0, (void *)on_stop_object_impulse_sound, 0, (void *)are_sound_attachments_equivalent,
            (void *)object_identifiers_are_equal
        }
        }
        ,
    { { (void *)track_scripted_impulse_sound, (void *)object_impulse_playing_in_channel,
        (void *)notify_object_impulse_sound, (void *)update_impulse_sound_output, (void *)on_stop_scripted_impulse_sound, 0, 0, 0 } }
    }
};
static const s_sound_tracker k_game_sound_first_person_tracker = {
    { (void *)track_scripted_impulse_sound, (void *)object_impulse_playing_in_channel,
      (void *)notify_object_impulse_sound, (void *)update_impulse_sound_output, (void *)on_stop_object_impulse_sound, 0, 0, 0 }
};
// @rdata-end

#if !defined(_XBOX) || defined(__clang__)
// The Xbox image places these names inside the pooled block above; the PC link needs them as objects.
const real_decibel k_scripted_sound_default_gain = { 0.0f };
const real_decibel k_scripted_sound_default_decibels = { 0.0f };
const s_sound_tracker k_object_impulse_sound_trackers[2] = {
    { { (void *)track_object_impulse_sound, (void *)object_impulse_playing_in_channel,
        (void *)notify_object_impulse_sound, 0, (void *)on_stop_object_impulse_sound, 0, (void *)are_sound_attachments_equivalent, 0 } },
    { { (void *)track_object_impulse_sound, (void *)object_impulse_playing_in_channel,
        (void *)notify_object_impulse_sound, 0, (void *)on_stop_object_impulse_sound, 0, (void *)are_sound_attachments_equivalent,
        (void *)object_identifiers_are_equal } }
};
const s_sound_tracker k_scripted_impulse_sound_tracker = {
    { (void *)track_scripted_impulse_sound, (void *)object_impulse_playing_in_channel,
        (void *)notify_object_impulse_sound, (void *)update_impulse_sound_output, (void *)on_stop_scripted_impulse_sound, 0, 0, 0 }
};
#endif

static __forceinline float game_sound24_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + (a->k * b->k + a->j * b->j);
}
static __forceinline float game_sound24_dot(const real_point3d *a, const real_vector3d *b)
{
    return a->x * b->i + (a->z * b->k + a->y * b->j);
}
static __forceinline void game_sound24_stereo_side(const s_sound_listener *const &listener, const real_point3d *point, float *facing)
{
    real_vector3d left = listener->matrix.left;
    float side = game_sound24_dot(point, &left);
    float aligned = game_sound24_dot(&listener->matrix.position, &left);
    *facing = side - aligned;
}
static __forceinline void game_sound24_stereo_identifier(const s_first_person_stereo_identifiers *identifiers, long listener_index,
    bool force_center, const real_point3d *point, __int64 *identifier)
{
    if (force_center)
    {
        *identifier = identifiers->identifiers[1];
        return;
    }
    float facing;
    game_sound24_stereo_side(&((const s_sound_manager_globals *)g_sound_manager_globals)->listeners[listener_index], point, &facing);
    if (fabs(facing) <= 0.01f)
    {
        *identifier = identifiers->identifiers[1];
        return;
    }
    if (facing > 0.0f)
    {
        *identifier = identifiers->identifiers[0];
        return;
    }
    *identifier = identifiers->identifiers[2];
}
const s_sound_tracker *game_sound_get_first_person_tracker()
{
    return &k_game_sound_first_person_tracker;
}
// @address 0x1fcd08 ?first_person_impulse_sound_new_internal@@YAJJJJ_NPBTreal_point3d@@M@Z
long first_person_impulse_sound_new_internal(
    long listener_index, long object_index, long sound_index, bool force_center, const real_point3d *point, float volume)
{
    s_sound_attachment_data attachment;
    attachment.flags = 0;
    object_impulse_sound_initialize_attachment_data(sound_index, object_index, -1, global_origin3d, global_forward3d, &attachment);
    sound_globals_reference *block = ((s_game_globals *)global_game_globals)->sound_globals.count()
        ? (sound_globals_reference *)((s_game_globals *)global_game_globals)->sound_globals.address
        : 0;
    long stereo_identifier_tag_index = block->first_person_stereo_identifiers.index;
    if (stereo_identifier_tag_index != -1) {
        s_first_person_stereo_identifiers *identifiers =
            TAG_GET('snmx', s_first_person_stereo_identifiers, stereo_identifier_tag_index);
        game_sound24_stereo_identifier(identifiers, listener_index, force_center, point, (__int64 *)attachment.identifier);
        attachment.flags |= 2;
    }
    return object_impulse_sound_new_internal(
        object_index, sound_index, _sound_spatialization_mode_none, &attachment, game_sound_get_first_person_tracker(), volume, -1, 0);
}
// @address 0x1fcca0 object_impulse_sound_new_at_location
extern "C" long object_impulse_sound_new_at_location(long object_index, long sound_index, const s_sound_location *location, float volume)
{
    long listener_index = -1;
    if (object_unattached_impulse_sound_new_internal(object_index, sound_index, &listener_index))
        return unattached_impulse_sound_new(sound_index, location, volume);
    return first_person_impulse_sound_new_internal(listener_index, object_index, sound_index, false, &location->position, volume);
}
// @address 0x1fcff8 object_impulse_sound_new_at_marker
extern "C" long object_impulse_sound_new_at_marker(long object_index, long sound_index, long marker_name, const s_sound_location *location, float volume)
{
    long listener_index = -1;
    if (object_unattached_impulse_sound_new_internal(object_index, sound_index, &listener_index))
        return unattached_impulse_sound_new(sound_index, location, volume);
    return first_person_impulse_sound_new_internal(listener_index, object_index, sound_index, marker_name == 157, &location->position, volume);
}


// @address 0x3aa90 k_real_math_zero_vector3d_storage

// @address 0x1fc968 ?submit_unattached_object_sound@c_game_sound_impulse_parameters@@QAAJJJ@Z
long c_game_sound_impulse_parameters::submit_unattached_object_sound(long object_index, long marker_name)
{
    object_marker marker;
    internal_object_get_markers_by_string_id(object_index, _string_id_head, &marker, 1, false);
    struct { s_sound_location sound; s_location location; } spatial;
    spatial.sound.position = marker.matrix.position;
    matrix3x3_rotation_to_quaternion((const real_matrix3x3 *)&marker.matrix.forward, &spatial.sound.rotation);
    spatial.sound.velocity = k_real_math_zero_vector3d_storage;
    object_get_location(object_index, &spatial.location);
    return object_impulse_sound_new_at_marker(object_index, definition_index, _string_id_head, &spatial.sound, scale);
}

namespace {
static __forceinline unsigned short game_sound_quantize_occlusion(float occlusion)
{
    return (unsigned short)quantize_real(occlusion, 0.0f, 1.0f, 16, false, false);
}
}


// @address 0x1fc1e8 ?game_sound_initialize_for_new_structure_bsp@@YAXK@Z
void game_sound_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask)
{
    if (g_game_sound_globals != 0)
    {
        game_sound_update_listeners(0.0f);
        ((game_sound_globals *)g_game_sound_globals)->occlusion.set_all(game_sound_quantize_occlusion(0.0f));
    }
}

// March body is empty; the release image folds it into the shared bare blr at 0x5bec98.
// @address 0x5bec98 ?game_sound_dispose_from_old_structure_bsp@@YAXK@Z
void game_sound_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask)
{
}

// @address 0x1fc020 ?game_sound_initialize_for_new_map@@YAXXZ
void game_sound_initialize_for_new_map()
{
    if (game_looping_sound_data != 0) {
        data_make_valid(game_looping_sound_data);
        game_sound_globals *globals = (game_sound_globals *)g_game_sound_globals;
        s_game_sound_listener listener;
        listener.cluster_loop_index = -1;
        listener.active_fog_definition_index = -1;
        listener.tracked_as_underwater = false;
        ((game_sound_globals *)g_game_sound_globals)->listeners[0] = listener;
        memcpy(&globals->listeners[1], &globals->listeners[0], sizeof(globals->listeners) - sizeof(s_game_sound_listener));
        globals->scripted_dialog_timer = 0;
        globals->occlusion.set_all(game_sound_quantize_occlusion(0.0f));
        memset(g_game_sound_scripted_impulses, -1, sizeof(s_scripted_impulse_sound) * k_maximum_scripted_impulse_sounds);
        globals->environment.global_environment_values[0] = 1.0f;
        globals->environment.global_environment_values[1] = 1.0f;
        globals->environment.global_environment_values[2] = 1.0f;
        globals->environment.global_environment_values[3] = 1.0f;
        long &global_environment_index = globals->environment.global_environment_index;
        global_environment_index = 8000;
        globals->environment.global_environment_value = 0.0f;
        globals->environment.timeline_duck = 0.0f;
    }
    strncpy((char *)&g_game_sound_identifier_generator, "music", 2);
    byte_swap_memory(&g_game_sound_identifier_generator.salt, 1, -2);
    g_game_sound_identifier_generator.index = 0;
    g_game_sound_identifier_generator.salt |= 0x8000;
    if (game_engine_running()) {
        game_sound_class_set_gain("", 0.0f, 0.0f);
        game_sound_class_set_gain("", 1.0f, 2.0f);
        game_sound_class_set_gain("scripted_dialog_player", 1.0f, 0.0f);
    }
}

// @address 0x1fce88 first_person_impulse_sound_new_for_weapon
extern "C" long first_person_impulse_sound_new_for_weapon(long effect_index, long object_index, long sound_index, float volume)
{
    s_sound_attachment_data attachment;
    attachment.flags = 0;
    // match: 0x1fce88: the global math pointers expose owned static storage for sharing the tracker base.
    object_impulse_sound_initialize_attachment_data(
        sound_index, object_index, -1, global_origin3d, global_forward3d, &attachment);
    s_game_globals *globals = (s_game_globals *)global_game_globals;
    sound_globals_reference *block = globals->sound_globals.count() != 0 ? (sound_globals_reference *)globals->sound_globals.address : 0;
    long stereo_identifier_tag_index = block->first_person_stereo_identifiers.index;
    if (stereo_identifier_tag_index != -1) {
        s_first_person_stereo_identifiers *identifiers =
            TAG_GET('snmx', s_first_person_stereo_identifiers, stereo_identifier_tag_index);
        if (effect_index != -1) {
            long player_index = tls_player_mapping_globals()->unit_indices[effect_index];
            if (player_index != -1) {
                unit_datum *object = (unit_datum *)object_header_get(player_index)->datum;
                long first_identifier = -1;
                if (object->unit.current_weapon_indices[0] != -1)
                    first_identifier = object->unit.weapon_object_indices[object->unit.current_weapon_indices[0]];
                long second_identifier = -1;
                if (object->unit.current_weapon_indices[1] != -1)
                    second_identifier = object->unit.weapon_object_indices[object->unit.current_weapon_indices[1]];
                if (object_index == second_identifier && first_identifier != -1)
                    *( __int64 *)attachment.identifier = identifiers->identifiers[0];
                else if (object_index == first_identifier && second_identifier != -1)
                    *( __int64 *)attachment.identifier = identifiers->identifiers[2];
                else
                    *( __int64 *)attachment.identifier = identifiers->identifiers[1];
                attachment.flags |= 6;
            }
        }
    }
    // match: 0x1fce88: the named tracker shares the math constants base without a second lis.
    const s_sound_tracker *tracker = &k_game_sound_first_person_tracker;
    return object_impulse_sound_new_internal(
        object_index, sound_index, _sound_spatialization_mode_none, &attachment, tracker, volume, -1, 0);
}

// ---- 0x1fd238 scripted_sound_new_3d ----

static __forceinline const real_quaternion *game_sound_new_3d_identity_quaternion() { return global_identity_quaternion; }

static __forceinline long game_sound_new_3d_players_first_active_user()
{
    s_user_globals *players = tls_player_mapping_globals();
    long result = -1;
    long user_index = 0;
    do {
        if (players->player_indices[user_index] != -1) {
            result = user_index;
            break;
        }
        user_index++;
    } while (user_index < 4);
    return result;
}

static __forceinline const s_sound_listener *game_sound_new_3d_manager_get_listener(long listener_index)
{
    return &((s_sound_manager_globals *)g_sound_manager_globals)->listeners[listener_index];
}
// @address 0x1fd238 ?scripted_sound_new_3d@@YAXJMM@Z
void scripted_sound_new_3d(long sound_index, float angle, float volume)
{
    if (sound_index != -1) {
        long user_index = game_sound_new_3d_players_first_active_user();
        if (user_index != -1) {
            const s_sound_listener *listener = game_sound_new_3d_manager_get_listener(user_index);
            if (listener->valid) {
                union {
                    real_vector3d direction;
                    real_point3d position;
                } offset;
                vector3d_from_angle(&offset.direction, angle * 0.017453292f);
                scale_vector3d(&offset.direction, sound_definition_get_minimum_distance(sound_index), &offset.direction);
                game_sound_location_block location;
                matrix4x3_transform_point(&listener->matrix, &offset.position, &location.sound.position);
                location.sound.rotation = *game_sound_new_3d_identity_quaternion();
                location.sound.velocity = *global_zero_vector3d;
                scenario_location_from_point(&location.location, &location.sound.position);
                unattached_impulse_sound_new(sound_index, &location.sound, volume);
            }
        }
    }
}


// @address 0x9e6c60 ?g_combined_pas@@3V?$c_static_flags@$0BAA@@@A
c_static_flags<256> g_combined_pas;
// @address 0x9e4c28 ?g_valid_skies@@3V?$c_static_flags@$0CA@@@A
c_static_flags<32> g_valid_skies;

static __forceinline void game_sound_pas_update_listeners(
    const scenario *scenario_definition, const structure_bsp *bsp, long zone_set_index, const s_sound_manager_globals *manager)
{
    for (unsigned long remaining = 4; remaining != 0; --remaining) {
        const s_sound_listener *state = &manager->listeners[4 - remaining];
        if (state->valid && state->location.cluster_reference.bsp_index != -1) {
            for (long cluster = 0; cluster < bsp->clusters.count; ++cluster)
                g_combined_pas.set(cluster, true);
            s_cluster_reference reference = state->location.cluster_reference;
            long sky = scenario_zone_set_structure_bsp_cluster_visible_sky_index_get(
                scenario_definition, zone_set_index, reference.bsp_index, reference.cluster_index);
            if (sky != -1)
                g_valid_skies.set(sky, true);
        }
    }
}
static __forceinline long game_sound_pas_active_bsp_index()
{
    return 31 - _CountLeadingZeros(g_active_structure_bsp_mask & -g_active_structure_bsp_mask);
}
static __forceinline const structure_bsp *game_sound_pas_active_bsp()
{
    long tag_index = ((const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[game_sound_pas_active_bsp_index()].structure_bsp.index;
    return TAG_GET('sbsp', const structure_bsp, tag_index);
}

// @address 0x200358 ?compute_combined_pas@@YAXXZ
// 0x200358: combined portal-accessibility state for the active structure bsp
void compute_combined_pas(void)
{
    const structure_bsp *bsp = game_sound_pas_active_bsp();
    bit_vector_clear_range(g_combined_pas.get_writeable_bits_direct(), bsp->clusters.count);
    g_valid_skies.clear();
    game_sound_pas_update_listeners(global_scenario, bsp, global_zone_set_index, g_sound_manager_globals);
}


// @address 0x567dd8 scripted_sound_new_mission_dialogue_at_point
extern "C" float scripted_sound_new_mission_dialogue_at_point(
    const real_point3d *position, const real_vector3d *direction, long dialogue_id, long variant_id)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x567dd8: 0.0f and 1.0f load off one 0x820221fc GPR base
    float zero = k_real_constant_pool[200];
#else
    float zero = 0.0f;
#endif
    struct { float playing_time; long playback_label; long pad[2]; } dialogue_result;
    struct { s_sound_location sound; s_location location; } spatial;
    s_sound_location &sound_location = spatial.sound;
    s_location &location = spatial.location;
    dialogue_result.playing_time = zero;
    dialogue_result.playback_label = 0;
    s_ai_mission_dialogue_variant *variant = ai_mission_dialogue_try_and_get_variant(dialogue_id, variant_id, &dialogue_result.playback_label);
    if (variant && variant->sound.index != -1) {
        scenario_location_from_point(&location, position);
        if (location.leaf_index != -1) {
            sound_location.position = *position;
            game_sound_location_set_rotation_from_direction(&sound_location, direction);
#if defined(_XBOX) && !defined(__clang__)
            // match: 0x567dd8: use one base for the gain word load at offset 0 and the velocity address at offset 0x4b88.
            const float *pool = (const float *)scripted_sound_constant_pool_anchor;
            real_decibel gain;
            memcpy(&gain, pool, sizeof(gain));
            memcpy(&sound_location.velocity, &pool[4834], sizeof(real_vector3d));
#else
            real_decibel gain = { 0.0f };
            sound_location.velocity.i = 0.0f;
            sound_location.velocity.j = 0.0f;
            sound_location.velocity.k = 0.0f;
#endif
            scripted_impulse_sound_start_at_location(variant->sound.index, &sound_location, k_real_constant_pool[0], gain, gain, 0,
                &dialogue_result.playing_time);
            return dialogue_result.playing_time;
        }
    }
    return zero;
}

static __forceinline float game_sound_flyby_magnitude_squared(const real_vector3d *vector)
{
    return vector->i * vector->i + (vector->j * vector->j + vector->k * vector->k);
}
static __forceinline float game_sound_flyby_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->j * b->j + (a->k * b->k + a->i * b->i);
}
static __forceinline void game_sound_flyby_components(const real_vector3d *vector, const real_vector3d *direction,
    real_vector3d *parallel, real_vector3d *perpendicular)
{
    float magnitude_squared = game_sound_flyby_magnitude_squared(direction);
    if (magnitude_squared != 0.0f) {
        float projection = direction->i * vector->i;
        float direction_k = direction->k;
        float direction_j = direction->j;
        float lateral_projection = direction_k * vector->k + direction_j * vector->j;
        projection += lateral_projection;
        projection /= magnitude_squared;
        parallel->i = direction->i * projection;
        parallel->j = direction_j * projection;
        parallel->k = direction_k * projection;
        perpendicular->i = vector->i - parallel->i;
        perpendicular->j = vector->j - parallel->j;
        perpendicular->k = vector->k - parallel->k;
    } else {
        parallel->i = 0.0f;
        parallel->j = 0.0f;
        parallel->k = 0.0f;
        *perpendicular = *vector;
    }
}
static __forceinline long game_sound_flyby_player_index(long user_index)
{
    long player_index = -1;
    if (user_index != -1)
        player_index = tls_player_mapping_globals()->player_indices[user_index];
    return player_index;
}
// @address 0xfc000 ?projectile_flyby_sound_try_new@@YA_NJJJPBTreal_point3d@@PBTreal_vector3d@@PBUs_location@@@Z
bool projectile_flyby_sound_try_new(long user_index, long owner_player_index, long definition_index,
    const real_point3d *projectile_position, const real_vector3d *travel_distance, const s_location *projectile_location)
{
    bool result = false;
    const s_sound_listener *listener = game_sound_new_3d_manager_get_listener(user_index);
    if (listener->valid && (!game_sound_first_person_spatialization(user_index)
        || owner_player_index != game_sound_flyby_player_index(user_index))) {
        const real_point3d *listener_position = &listener->matrix.position;
        float maximum_distance = sound_definition_get_maximum_distance(definition_index);
        real_vector3d to_listener;
        vector_from_points3d(projectile_position, listener_position, &to_listener);
        real_vector3d parallel;
        real_vector3d perpendicular;
        game_sound_flyby_components(&to_listener, travel_distance, &parallel, &perpendicular);
        float distance_along_path = game_sound_flyby_dot(&parallel, travel_distance);
        if (distance_along_path >= 0.0f && distance_along_path < game_sound_flyby_magnitude_squared(travel_distance)) {
            float distance_squared = game_sound_flyby_magnitude_squared(&perpendicular);
            if (distance_squared < maximum_distance * maximum_distance) {
                struct { s_sound_location sound; s_location location; } spatial;
                float distance = -sqrtf(distance_squared);
                spatial.sound.position.x = listener_position->x + perpendicular.i * distance;
                spatial.sound.position.y = listener_position->y + perpendicular.j * distance;
                spatial.sound.position.z = listener_position->z + perpendicular.k * distance;
                real_vector3d direction = *travel_distance;
                normalize3d(&direction);
                game_sound_location_set_rotation_from_direction(&spatial.sound, &direction);
                spatial.sound.velocity = *global_zero_vector3d;
                if (projectile_location->cluster_reference.bsp_index != -1)
                    spatial.location = *projectile_location;
                else
                    spatial.location = listener->location;
                unattached_impulse_sound_new(definition_index, &spatial.sound, 1.0f);
                result = true;
            }
        }
    }
    return result;
}

// @address 0x35fac ?k_object_impulse_sound_trackers@@3QBUs_sound_tracker@@B
static __forceinline const s_sound_tracker *object_get_sound_tracker(long definition_index, long object_index)
{
    return sound_definition_link_to_owner(definition_index, object_index) ? &k_object_impulse_sound_trackers[1] : &k_object_impulse_sound_trackers[0];
}
// @address 0x1fc848 ?submit_attached_sound@c_game_sound_impulse_parameters@@QAAJJFPBTreal_point3d@@PBTreal_vector3d@@@Z
long c_game_sound_impulse_parameters::submit_attached_sound(long object_index, short node_index, const real_point3d *point, const real_vector3d *forward)
{
    long result = -1;
    s_sound_attachment_data attachment;
    attachment.flags = 0;
    s_game_sound_impulse_internal_parameters parameters;
    parameters.additional_override_flags = 0;
    char spatialization = 1;
    update_spatialization_if_first_person(object_index, &spatialization, 0);
    object_impulse_sound_initialize_attachment_data(definition_index, object_index, node_index, point, forward, &attachment);
    parameters.source.volume_type = 0;
    parameters.source.flags = 0;
    long spatialization_value = spatialization;
    parameters.source.spatialization = (spatialization_value & 15) | (spatialization_value << 4);
    parameters.source.name = 0;
    location_invalidate(&parameters.source.location);
    parameters.source.volume = scale;
    parameters.permutation = deterministic;
    if (track_object_impulse_sound(object_index, definition_index, &attachment, &parameters.source)) {
        parameters.object_index = object_index;
        parameters.tracking_data = (const s_sound_tracking_data *)&attachment;
        parameters.playback_label = game_sound_find_platform_playback_by_label(sound_impulse_index);
        parameters.tracking = object_get_sound_tracker(definition_index, object_index);
        result = game_sound_impulse_new_internal(definition_index, &parameters);
    }
    return result;
}
// @address 0xfd3b0 ?submit_attached_sound_from_marker@c_game_sound_impulse_parameters@@QAAJJJ@Z
long c_game_sound_impulse_parameters::submit_attached_sound_from_marker(long object_index, long marker_name)
{
    object_marker marker;
    internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false);
    return submit_attached_sound(object_index, marker.node_index, &marker.node_to_marker.position, &marker.node_to_marker.forward);
}
static __forceinline bool game_sound_class_is_deterministic(const sound_class_definition *definition) { return (bool)((definition->flags >> 5) & 1); }
// match: 0x1fd138: scripted_sound_trigger has the marker lookup inlined; calling the out-of-line member (0xfd3b0) shrinks it 36 bytes.
static __forceinline long inl_submit_attached_sound_from_marker(c_game_sound_impulse_parameters *parameters, long object_index, long marker_name)
{
    object_marker marker;
    internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false);
    return parameters->submit_attached_sound(object_index, marker.node_index, &marker.node_to_marker.position, &marker.node_to_marker.forward);
}
// @address 0x1fd138 ?scripted_sound_trigger@@YAXJJMJ@Z
void scripted_sound_trigger(long definition_index, long object_index, float scale, long count)
{
    if (definition_index != -1) {
        const s_cache_file_sound_definition *sound =
            TAG_GET('snd!', const s_cache_file_sound_definition, definition_index);
        bool deterministic = game_sound_class_is_deterministic(sound_class_get(sound->sound_class));
        while (count--) {
            s_game_sound_deterministic_permutation permutation;
            c_game_sound_impulse_parameters parameters(definition_index, scale);
            if (deterministic) {
                game_sound_deterministic_pick_permutation(definition_index, &permutation, 0);
                if (game_sound_deterministic_permutation_valid(definition_index, &permutation))
                    parameters.deterministic = &permutation;
            }
            if (object_index != -1)
                inl_submit_attached_sound_from_marker(&parameters, object_index, 0);
            else
                parameters.submit_unspatialized_sound();
        }
    }
}
static __forceinline bool game_sound_object_is_first_person_weapon(long user_index, long object_index)
{
    bool first_person_weapon = false;
    if (user_index != -1 && first_person_weapon_get_weapon_slot(user_index, object_index) != -1)
        first_person_weapon = true;
    return first_person_weapon;
}
// @address 0xfd160 ?object_impulse_sound_new@@YAJJJFPBTreal_point3d@@PBTreal_vector3d@@M@Z
long object_impulse_sound_new(long object_index, long definition_index, short node_index, const real_point3d *point, const real_vector3d *forward, float volume)
{
    long user_index = -1;
    bool first_person = !object_unattached_impulse_sound_new_internal(object_index, definition_index, &user_index) &&
        game_sound_object_is_first_person_weapon(user_index, object_index);
    if (first_person)
        return first_person_impulse_sound_new_for_weapon(user_index, object_index, definition_index, volume);
    s_sound_attachment_data attachment;
    attachment.flags = 0;
    object_impulse_sound_initialize_attachment_data(definition_index, object_index, node_index, point, forward, &attachment);
    return object_impulse_sound_new_internal(object_index, definition_index, _sound_spatialization_mode_unspatialized, &attachment,
        object_get_sound_tracker(definition_index, object_index), volume, -1, 0);
}
// @address 0xfe0b0 ?game_sound_get_object_velocity@@YAXJPATreal_vector3d@@@Z
void game_sound_get_object_velocity(long object_index, real_vector3d *out_velocity)
{
    long object_type = object_header_get(object_index)->object_type;
    object_get_velocities(object_index, out_velocity, 0);
    real_vector3d root_velocity;
    if (((1 << object_type) & 0x1003) && unit_is_playing_custom_animation(object_index) &&
        biped_get_animation_root_velocity(object_index, &root_velocity))
    {
        out_velocity->i = out_velocity->i + root_velocity.i;
        out_velocity->j = out_velocity->j + root_velocity.j;
        out_velocity->k = out_velocity->k + root_velocity.k;
    }
}
// @address 0x1fe2a8 ?unattached_looping_sound_start_with_effect@@YAJJJMJ@Z
long unattached_looping_sound_start_with_effect(long definition_index, long object_index, float scale, long effect_label)
{
    long sound_index = unattached_looping_sound_start(definition_index, object_index, scale);
    if (sound_index != -1) {
        s_looping_sound_attachment_record *sound = game_sound_attachment_datum_get(
            game_looping_sound_data, sound_index);
        sound->sound_index = game_sound_find_platform_playback_by_label(effect_label);
    }
    return sound_index;
}


static __forceinline bool game_sound_looping_source_flag(unsigned short flags, long bit) { return ((flags >> bit) & 1) != 0; }
static __forceinline const s_cluster_reference *game_sound_looping_source_cluster_reference_set(
    s_cluster_reference *reference, long bsp_index, long cluster_index)
{
    reference->bsp_index = (signed char)bsp_index;
    reference->cluster_index = (unsigned char)cluster_index;
    return reference;
}
static __forceinline const s_location *game_sound_looping_source_location_set(
    s_location *location, long bsp_index, long cluster_index, long leaf_index)
{
    game_sound_looping_source_cluster_reference_set(&location->cluster_reference, bsp_index, cluster_index);
    location->leaf_index = (unsigned short)leaf_index;
    return location;
}


static __forceinline real_decibel game_sound_looping_source_additional_gain(const s_looping_sound_attachment_record *datum)
{
    return *(datum->type.m_value == _game_looping_sound_type_unattached
        ? &datum->source.unattached.gain : &k_game_sound_tracker_prefix.default_additional_gain);
}
static __forceinline long game_sound_looping_source_object_index(const s_looping_sound_attachment_record *datum)
{
    long result;
    if (datum->type.m_value == _game_looping_sound_type_object)
        result = datum->source.object.object_index;
    else
        result = -1;
    return result;
}
static __forceinline long game_sound_looping_source_marker_name(const s_looping_sound_attachment_record *datum)
{
    long result = 0;
    if (datum->type.m_value == _game_looping_sound_type_object && datum->attachment_index != -1)
        result = object_get_attachment_marker_name(game_sound_looping_source_object_index(datum), datum->attachment_index);
    return result;
}
static __forceinline long game_sound_looping_source_object_get_type(long object_index) { return object_header_get(object_index)->object_type; }
static __forceinline s_sound_scenery_datum *game_sound_looping_source_object_get(long object_index)
{
    return (s_sound_scenery_datum *)object_header_get(object_index)->datum;
}
static __forceinline bool game_sound_looping_source_gain_less(const real_decibel *a, const real_decibel *b) { return a->value < b->value; }

static __forceinline const real_point3d *game_sound_looping_source_origin() { return global_origin3d; }
static __forceinline const real_vector3d *game_sound_looping_source_up() { return global_up3d; }

static __forceinline long game_sound_looping_source_definition_index(const s_looping_sound_attachment_record *datum)
{
    long result = -1;
    if ((long)datum->type.m_value == _game_looping_sound_type_object) {
        const looping_sound_definition *object = (const looping_sound_definition *)object_try_and_get_and_verify_type(
            datum->source.object.object_index, 0xffffffff);
        if (object)
            result = object->flags;
    }
    return result;
}
// @address 0x128430 ?game_looping_sound_get_source@@YAXJPAUs_sound_source@@@Z
void game_looping_sound_get_source(long looping_sound_index, s_sound_source *sound_source_param)
{
    s_sound_source *sound_source = (s_sound_source *)sound_source_param;
    real_vector3d effect_vector;
    object_marker marker;
    s_data_array *sound_data = game_looping_sound_data;
    s_looping_sound_attachment_record *datum = datum_get<s_looping_sound_attachment_record>(sound_data, looping_sound_index);
    const looping_sound_definition *definition =
        TAG_GET('lsnd', const looping_sound_definition, datum->definition_index);
    sound_source->source_flags.clear();
    sound_source->gain = game_sound_looping_source_additional_gain(datum);
    sound_source->volume_mode = 0;
    switch (datum->type.m_value) {
    case _game_looping_sound_type_impact:
        sound_source->current_spatialization = 1;
        if (!game_sound_looping_source_flag(datum->flags.m_storage, 1)) {
            c_impact *impact = datum_get<c_impact>(tls_impact_data(), datum->source.object.object_index);
            impact->update_looping_sound((s_sound_location *)&sound_source->position, &sound_source->volume);
        }
        else {
            sound_source->volume = 0.0f;
            sound_source->source_flags.set(1, true);
            memset(&sound_source->position, 0, sizeof(game_sound_location_block));
            scenario_location_from_point((s_location *)&sound_source->location, game_sound_looping_source_origin());
            game_sound_location_set_rotation_from_direction((s_sound_location *)&sound_source->position, game_sound_looping_source_up());
        }
        break;
    case _game_looping_sound_type_effect: {
        bool valid = true;
        sound_source->current_spatialization = 1;
        if (!game_sound_looping_source_flag(datum->flags.m_storage, 1)) {
            real_point3d *position = &sound_source->position;
            valid = !effect_update_attached_sound(datum->source.object.object_index, position, &effect_vector, &sound_source->volume);
            if (!valid) {
                game_sound_location_set_rotation_from_direction((s_sound_location *)position, &effect_vector);
                const effect_datum *effect = datum_get<effect_datum>(effect_data, datum->source.object.object_index);
                const real_vector3d *velocity = &effect->velocity;
                sound_source->velocity = *velocity;
                scenario_location_from_point((s_location *)&sound_source->location, position);
            }
        }
        if (valid) {
            sound_source->volume = 0.0f;
            sound_source->source_flags.set(1, true);
            memset(&sound_source->position, 0, sizeof(game_sound_location_block));
            scenario_location_from_point((s_location *)&sound_source->location, game_sound_looping_source_origin());
            game_sound_location_set_rotation_from_direction((s_sound_location *)&sound_source->position, game_sound_looping_source_up());
        }
        break;
    }
    case _game_looping_sound_type_unattached: {
        s_cluster_reference cluster_reference = datum->source.unattached.cluster_reference;
        if (cluster_reference.bsp_index != -1) {
            game_sound_looping_source_location_set(&sound_source->location, cluster_reference.bsp_index, cluster_reference.cluster_index, -1);
            sound_source->source_flags.set(12, true);
        }
    }
    default:
        if (!game_sound_looping_source_flag(datum->flags.m_storage, 0)) {
            if ((1 << game_sound_looping_source_object_get_type(game_sound_looping_source_object_index(datum))) & _object_mask_sound_scenery) {
                s_sound_scenery_datum *object = game_sound_looping_source_object_get(datum->source.object.object_index);
                sound_source->volume_mode = (unsigned char)object->volume_type;
                sound_source->length = object->height;
                if (object->minimum_distance > 0.0f) {
                    sound_source->source_flags.set(2, true);
                    sound_source->minimum_distance = object->minimum_distance;
                }
                if (object->maximum_distance > 0.0f) {
                    sound_source->source_flags.set(3, true);
                    sound_source->maximum_distance = object->maximum_distance;
                }
                if (object->inner_cone_angle > 0.0f) {
                    sound_source->source_flags.set(4, true);
                    sound_source->inner_cone_angle = sound_source_quantize_cone_angle(object->inner_cone_angle);
                }
                if (object->outer_cone_angle > 0.0f) {
                    sound_source->source_flags.set(5, true);
                    sound_source->outer_cone_angle = sound_source_quantize_cone_angle(object->outer_cone_angle);
                }
                real_decibel zero_gain = { 0.0f };
                bool override_gain = game_sound_looping_source_gain_less(&object->outer_cone_gain, &zero_gain);
                if (override_gain) {
                    sound_source->source_flags.set(6, true);
                    sound_source->outer_cone_gain = object->outer_cone_gain;
                }
            }
            long definition_index = game_sound_looping_source_definition_index(datum);
            bool deterministic;
            object_get_function_value(game_sound_looping_source_object_index(datum), datum->source.object.identifier, definition_index,
                &sound_source->volume, &deterministic);
        }
        else {
            sound_source->volume = datum->scale;
        }
        if (datum->type.m_value == _game_looping_sound_type_object) {
            internal_object_get_markers_by_string_id(
                game_sound_looping_source_object_index(datum), game_sound_looping_source_marker_name(datum), &marker, 1, false);
            const real_point3d *marker_position = &marker.matrix.position;
            sound_source->position = *marker_position;
            matrix3x3_rotation_to_quaternion((const real_matrix3x3 *)&marker.matrix.forward, &sound_source->orientation);
            game_sound_get_object_velocity(game_sound_looping_source_object_index(datum), &sound_source->velocity);
            object_get_location(game_sound_looping_source_object_index(datum), (s_location *)&sound_source->location);
            long object_index_for_spatialization = game_sound_looping_source_object_index(datum);
            char spatialization = 1;
            update_spatialization_if_first_person(object_index_for_spatialization, &spatialization, 0);
            sound_source->current_spatialization = spatialization;
            if ((definition->flags & (1 << 3)) && sound_source->current_spatialization == 0) {
                sound_source->source_flags.set(0, true);
                sound_source->current_spatialization = 1;
            }
            if (sound_attachment_is_sky(game_sound_looping_source_object_index(datum)))
                sound_source->source_flags.set(13, true);
        }
        else {
            sound_source->current_spatialization = 0;
        }
        if (game_sound_looping_source_flag(datum->flags.m_storage, 6))
            sound_source->source_flags.set(10, true);
        break;
    }
    sound_source->initial_spatialization = sound_source->current_spatialization;
}
#define GAME_SOUND_SET_BIT(flags, bit, value) ((value) ? ((flags) |= (1 << (bit))) : ((flags) &= ~(1 << (bit))))
static __forceinline bool game_sound_looping_source_within_listener_range(const s_sound_source *source, float maximum_distance)
{
    bool result = false;
    float maximum_distance_squared = maximum_distance * maximum_distance;
    for (long listener_index = 0; listener_index < 4; listener_index++) {
        if (((s_sound_manager_globals *)g_sound_manager_globals)->listeners[listener_index].valid &&
            source_distance_squared(listener_index, source) <= maximum_distance_squared)
        {
            result = true;
            break;
        }
    }
    return result;
}
static __forceinline s_game_globals *game_sound_looping_game_globals() { return (s_game_globals *)global_game_globals; }
static __forceinline float game_sound_looping_default_fade_time()
{
    float fade_time = 4.0f;
    sound_globals_reference *block = game_sound_looping_game_globals()->sound_globals.count() != 0
        ? (sound_globals_reference *)game_sound_looping_game_globals()->sound_globals.address
        : 0;
    if (block->sound_mix.index != -1)
        fade_time = (TAG_GET('snmx', sound_manager_mix_definition, block->sound_mix.index))
                        ->global.default_looping_fade_time;
    return fade_time;
}
static __forceinline bool game_sound_looping_manager_available(const s_sound_manager_globals *manager)
{
    return manager->initialized && manager->enabled && manager->active;
}
static __forceinline void game_sound_looping_playback_controller_refresh(long owner_identifier, const s_sound_source *source)
{
    const s_sound_manager_globals *manager = (const s_sound_manager_globals *)g_sound_manager_globals;
    if (game_sound_looping_manager_available(manager)) {
        long controller_index = sound_playback_controller_find_existing(owner_identifier);
        if (controller_index != -1) {
            c_sound_playback_controller *controller = (c_sound_playback_controller *)datum_try_and_get(g_sound_playback_controller_data, controller_index);
            if (controller)
                controller->update_controller_source(source, (manager->update_flags & 1) != 0);
        }
    }
}
static __forceinline bool game_sound_looping_cinematic_in_progress()
{
    bool in_progress = false;
    if (cinematic_globals)
        in_progress = cinematic_globals->in_progress;
    return in_progress;
}
static __forceinline void game_sound_looping_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->j * b->k - b->j * a->k;
    result->j = a->k * b->i - a->i * b->k;
    result->k = a->i * b->j - a->j * b->i;
}
static __forceinline void game_sound_looping_matrix3x3_from_forward_and_up(real_matrix3x3 *matrix, const real_vector3d *forward, const real_vector3d *up)
{
    matrix->forward = *forward;
    game_sound_looping_cross_product3d(up, forward, &matrix->left);
    matrix->up = *up;
}
static __forceinline bool game_looping_sound_is_attached_to_object(const s_looping_sound_attachment_record *sound)
{
    return sound->type == _game_looping_sound_type_object;
}
static __forceinline long game_looping_sound_get_object_index(const s_looping_sound_attachment_record *sound)
{
    return game_looping_sound_is_attached_to_object(sound) ? sound->source.object.object_index : -1;
}
static __forceinline long game_looping_sound_get_object_definition_index(const s_looping_sound_attachment_record *sound)
{
    long result = -1;
    if (game_looping_sound_is_attached_to_object(sound)) {
        const looping_sound_definition *object = (const looping_sound_definition *)object_try_and_get_and_verify_type(
            game_looping_sound_get_object_index(sound), 0xffffffff);
        if (object)
            result = object->flags;
    }
    return result;
}
static __forceinline long game_looping_sound_get_unattached_background_sound_palette_index(const s_looping_sound_attachment_record *sound)
{
    return sound->type == _game_looping_sound_type_unattached ? (long)sound->source.unattached.user_index.m_value : -1;
}
static __forceinline long game_looping_sound_get_identifier(long looping_sound_index)
{
    s_looping_sound_attachment_record *sound = game_sound_attachment_datum_get(game_looping_sound_data, looping_sound_index);
    long identifier = looping_sound_index;
    long palette_index = game_looping_sound_get_unattached_background_sound_palette_index(sound);
    if (palette_index != -1)
        identifier = sound->definition_index | 0x8000;
    else if (sound->type == _game_looping_sound_type_music)
        identifier = sound->source.object.identifier | 0xa000;
    return identifier;
}
// @address 0x1fe5c0 ?update_potentially_audible_looping_sound@@YAXJ@Z
void update_potentially_audible_looping_sound(long looping_sound_index)
{
    s_looping_sound_attachment_record *looping_sound = datum_get<s_looping_sound_attachment_record>(game_looping_sound_data, looping_sound_index);
    s_looping_sound_attachment_record *record = (s_looping_sound_attachment_record *)looping_sound;
    const looping_sound_definition *definition =
        TAG_GET('lsnd', const looping_sound_definition, looping_sound->definition_index);
    float scale = looping_sound->scale;
    bool was_audible = game_sound_looping_source_flag(looping_sound->flags.m_storage, 9);
    bool deterministic;
    bool audible = !game_sound_looping_source_flag(looping_sound->flags.m_storage, 0)
        ? object_get_function_value(game_looping_sound_get_object_index(record), looping_sound->source.object.identifier,
              game_looping_sound_get_object_definition_index(record), &scale, &deterministic)
        : !game_sound_looping_source_flag(looping_sound->flags.m_storage, 1) && !game_sound_looping_source_flag(looping_sound->flags.m_storage, 11);
    long identifier = game_looping_sound_get_identifier(looping_sound_index);
    long source_index = -1;
    if (((definition->flags & 0x10) && game_looping_sound_is_attached_to_object(record)) || (definition->flags & 0x20))
        source_index = game_sound_looping_sound_source_index(looping_sound_index, game_looping_sound_get_object_index(record));
    if (audible || ((e_game_looping_sound_state)record->state != _game_looping_sound_state_initial && was_audible)) {
        unsigned long playback_flags = 0;
        GAME_SOUND_SET_BIT(playback_flags, 0, !(definition->flags & 8));
        GAME_SOUND_SET_BIT(playback_flags, 1, ((1 << (e_game_looping_sound_type)record->type) & 0xe) != 0 && !(definition->flags & 0x40));
        s_sound_source source;
        source.flags = 0;
        game_looping_sound_get_source(looping_sound_index, (s_sound_source *)&source);
        float maximum_distance = game_sound_looping_source_flag(source.flags, 3) ? source.maximum_distance : definition->maximum_distance;
        unsigned long refresh_flags = 0;
        unsigned short flags = looping_sound->flags.m_storage;
        GAME_SOUND_SET_BIT(refresh_flags, 0, game_sound_looping_source_flag(flags, 4));
        GAME_SOUND_SET_BIT(refresh_flags, 2, game_sound_looping_source_flag(flags, 8));
        e_game_looping_sound_type type = record->type;
        GAME_SOUND_SET_BIT(refresh_flags, 4, type == _game_looping_sound_type_music);
        if (!(playback_flags & 2) || game_sound_looping_source_flag(source.flags, 1) ||
            game_sound_looping_source_within_listener_range((s_sound_source *)&source, maximum_distance))
        {
            if (audible) {
                bool forced = false;
                if (game_sound_looping_cinematic_in_progress() && type == _game_looping_sound_type_object &&
                    ((1 << game_sound_looping_source_object_get_type(
                          type == _game_looping_sound_type_object ? record->source.object.object_index : -1)) &
                        2))
                    forced = true;
                e_looping_sound_refresh_state refresh_state;
                if (forced || ((1 << (e_game_looping_sound_state)record->state) & 3)) {
                    refresh_state = _looping_sound_refresh_audible;
                    record->state.m_value = 1;
                } else {
                    refresh_state = _looping_sound_refresh_inaudible;
                    record->state.m_value = 0;
                }
                if (sound_refresh_looping(looping_sound->definition_index, playback_flags, identifier, source_index, record->sound_index,
                        (s_sound_source *)&source, refresh_state, refresh_flags, 0.0f))
                {
                    if (definition->flags & 2)
                        record->flags.set(1, true);
                    record->state.m_value = 3;
                } else if (source_index != -1) {
                    s_sound_source source_copy;
                    memcpy(&source_copy, &source, sizeof(source_copy));
                    if (game_looping_sound_is_attached_to_object(record)) {
                        long object_index = game_looping_sound_get_object_index(record);
                        object_get_origin(object_index, &source_copy.position);
                        real_vector3d forward;
                        real_vector3d up;
                        object_get_orientation(object_index, &forward, &up);
                        real_matrix3x3 matrix;
                        game_sound_looping_matrix3x3_from_forward_and_up(&matrix, &forward, &up);
                        matrix3x3_rotation_to_quaternion(&matrix, &source.orientation);
                        object_get_velocities(game_looping_sound_get_object_index(record), &source_copy.velocity, 0);
                    }
                    game_sound_looping_playback_controller_refresh(source_index, (s_sound_source *)&source_copy);
                }
            } else {
                float fade = game_sound_looping_source_flag(flags, 2) ? game_sound_looping_default_fade_time()
                                                                      : (game_sound_looping_source_flag(flags, 3) ? 0.01f : 0.0f);
                if (was_audible &&
                    !sound_refresh_looping(looping_sound->definition_index, playback_flags, identifier, source_index, record->sound_index,
                        (s_sound_source *)&source, _looping_sound_refresh_stopped, refresh_flags, fade))
                    record->state.m_value = 2;
                else
                    record->state.m_value = 3;
            }
            record->flags.set(9, true);
        } else {
            record->flags.set(9, false);
        }
    } else if ((e_game_looping_sound_state)record->state != _game_looping_sound_state_initial) {
        record->state = _game_looping_sound_state_initial;
    }
    if (record->state == _game_looping_sound_state_initial && (looping_sound->flags.m_storage & 0xe) != 0 && (looping_sound->flags.m_storage & 1) != 0) {
        if (scripted_looping_sound_get_sound_index(looping_sound->definition_index) == looping_sound_index)
            scripted_looping_sound_reset_internal(looping_sound->definition_index, -1);
        datum_delete(game_looping_sound_data, looping_sound_index);
    }
}

// @rdata 0x820221fc
static const float k_azimuth_one = 1.0f;
// @rdata 0x8202251c
static const float k_azimuth_zero = 0.0f;
// @rdata 0x820249c8
static const float k_azimuth_two_pi = 6.2831855f;
// @rdata 0x82022dc0
static const float k_azimuth_half = 0.5f;
static __forceinline const float *game_sound_azimuth_one() { return &k_azimuth_one; }
static __forceinline const float *game_sound_azimuth_zero() { return &k_azimuth_zero; }
static __forceinline const float *game_sound_azimuth_two_pi() { return &k_azimuth_two_pi; }
static __forceinline const float *game_sound_azimuth_half() { return &k_azimuth_half; }
static __forceinline real_vector3d *game_sound_azimuth_project(const real_matrix3x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    float i = vector->i;
    float j = vector->j;
    float k = vector->k;
    result->i = i * matrix->forward.i + j * matrix->forward.j + k * matrix->forward.k;
    result->j = i * matrix->left.i + j * matrix->left.j + k * matrix->left.k;
    result->k = i * matrix->up.i + j * matrix->up.j + k * matrix->up.k;
    return result;
}
static __forceinline float *game_sound_azimuth_store_volume(float *destination, float volume) { *destination = volume; return destination; }
// @address 0x1fec60 ?game_sound_calculate_azimuth_image@@YAXPBUs_sound_listener@@PBTreal_point3d@@PAUstructure_bsp@@JJM_NPAUs_sound_azimuth_image@@@Z
void game_sound_calculate_azimuth_image(const s_sound_listener *listener, const real_point3d *point, structure_bsp *bsp, long unused,
    long portal_index, float distance, bool inside, s_sound_azimuth_image *image)
{
    bool valid = false;
    float one = *game_sound_azimuth_one();
    float zero = *game_sound_azimuth_zero();
    float two_pi = *game_sound_azimuth_two_pi();
    if (portal_index != -1)
    {
        const cluster_portal *portal = &((const cluster_portal *)(const void *)bsp->cluster_portals.address)[portal_index];
        real_vector3d direction = { point->x - portal->centroid.x, point->y - portal->centroid.y, point->z - portal->centroid.z };
        float portal_distance = structure_distance_to_portal(bsp, portal, point);
        normalize3d(&direction);
        game_sound_azimuth_project(&listener->matrix.rotation, &direction, &direction);
        float forward = direction.i;
        float left = direction.j;
        float up = direction.k;
        if (portal_distance < distance)
        {
            if (inside) portal_distance = -portal_distance;
            else { forward = -forward; left = -left; up = -up; }
            if (!(distance > 0.001f)) distance = 0.001f;
            float ratio = portal_distance / distance;
            ratio = ((((ratio) > (-1.0f) ? (ratio) : (-1.0f))) < (1.0f) ? (((ratio) > (-1.0f) ? (ratio) : (-1.0f))) : (1.0f));
            float angle = arccosine(ratio);
            float radius_angle = (float)atan2(portal->bounding_radius, portal_distance);
            float azimuth = (float)atan2(left, forward);
            float width;
            if (inside)
                width = angle;
            else
            {
                float scale = ((((one - (float)fabs(ratio)) > (0.0f) ? (one - (float)fabs(ratio)) : (0.0f)))
                    < (1.0f) ? (((one - (float)fabs(ratio)) > (0.0f) ? (one - (float)fabs(ratio)) : (0.0f))) : (1.0f));
                width = ((radius_angle) > (angle) ? (radius_angle) : (angle)) * scale;
            }
            image->azimuth.lower = azimuth - width;
            image->azimuth.upper = azimuth + width;
            if (image->azimuth.lower < -6.2831855f)
            { image->azimuth.lower += two_pi; image->azimuth.upper += two_pi; }
            else if (image->azimuth.upper > two_pi)
            { image->azimuth.lower -= two_pi; image->azimuth.upper -= two_pi; }
            float volume;
            if (inside)
                volume = ((float)fabs(ratio) + one) * *game_sound_azimuth_half();
            else
                volume = (one - (float)fabs(ratio)) * *game_sound_azimuth_half();
            game_sound_azimuth_store_volume(&image->base_volume, volume);
            valid = true;
            image->base_volume = (float)fabs(up) * volume;
            image->spread = one - (float)fabs(up);
        }
    }
    if (!valid)
    {
        image->azimuth.lower = zero;
        image->azimuth.upper = two_pi;
        image->spread = inside ? one : zero;
        image->base_volume = zero;
    }
}


typedef c_game_sound_structure_palette_helper<s_scenario_acoustics_palette_entry,
    get_acoustic_ambience_background_sound, get_acoustic_ambience_background_cutoff, 28, 264, 30, 588>
    game_sound_background_palette_helper;


// match: 0xfd418: returning the ambience member rather than the whole palette entry avoids an extra move and keeps the float math order.
static __forceinline const s_scenario_acoustics_ambience *structure_bsp_get_ambience_palette_entry(const structure_bsp *bsp, long index)
{
    return &static_cast<const s_scenario_acoustics_palette_entry *>(global_scenario->acoustics_palette.address)[index].ambience;
}
static __forceinline const s_scenario_acoustics_palette_entry *game_sound_background_bsp_palette(const structure_bsp *bsp, long index)
{
    const s_scenario_acoustics_palette_entry *entry = 0;
    if (index != -1)
        entry = ((s_scenario_acoustics_palette_entry *)bsp->acoustics_palette.address) + index;
    return entry;
}
static __forceinline long game_sound_background_reference(const structure_bsp *bsp, long index, bool active)
{
    const s_scenario_acoustics_palette_entry *entry = game_sound_background_bsp_palette(bsp, index);
    return entry ? (active ? entry->ambience.inside_cluster_sound.index : entry->ambience.background_sound.index) : -1;
}
static __forceinline long game_sound_background_parent(long object_index)
{
    long root = -1;
    if (object_index != -1) {
        do {
            root = object_index;
            object_index = object_header_get(object_index)->datum->parent_object_index;
        } while (object_index != -1);
    }
    return root;
}
// @rdata 0x8202490c
static const float k_game_sound_background_epsilon = 0.0001f;
// @rdata 0x82021694
static const float k_game_sound_reverb_attribute_distance = 3.4028234663852886e38f;
static const real_decibel k_game_sound_background_minimum_gain = { -64.0f };
static __forceinline long game_sound_background_user_object(long user_index)
{
    long result = -1;
    if (user_index != -1)
        result = tls_player_mapping_globals()->unit_indices[user_index];
    return result;
}
static __forceinline float game_sound_background_normalize(float lower, float upper, float value, bool reverse)
{
    float normalized;
    if (fabsf(lower - upper) >= k_game_sound_background_epsilon)
        normalized = (value - lower) / (upper - lower);
    else
        normalized = ((value < lower) == reverse) ? 0.0f : 1.0f;
    return ((((normalized) > (0.0f) ? (normalized) : (0.0f))) < (1.0f) ? (((normalized) > (0.0f) ? (normalized) : (0.0f))) : (1.0f));
}
static __forceinline float game_sound_background_ease(float ratio)
{
    float square = ratio * ratio;
    float result = 3.0f * square - 2.0f * square * ratio;
    return ((((result) > (0.0f) ? (result) : (0.0f))) < (1.0f) ? (((result) > (0.0f) ? (result) : (0.0f))) : (1.0f));
}
static __forceinline float game_sound_background_fade_time(const structure_bsp *bsp, long index, float delta)
{
    const s_scenario_acoustics_ambience *ambience = structure_bsp_get_ambience_palette_entry(bsp, index);
    float time = fabsf(ambience->interpolation_speed) < k_game_sound_background_epsilon ? 2.0f : ambience->interpolation_speed;
    return delta / time;
}
static __forceinline unsigned long game_sound_background_loop_palette_index(const s_looping_sound_attachment_record *datum)
{
    return datum->type.m_value == _game_looping_sound_type_unattached ? datum->source.unattached.user_index.m_value : -1;
}
static __forceinline float game_sound_background_gain(real_decibel gain)
{
    return (float)exp((double)(gain.to_real() * 0.115129254758358f));
}
static __forceinline void game_sound_background_interpolate(float *value, float *step, float target, float maximum_delta)
{
    *step = ((((target - *value) > (-maximum_delta) ? (target - *value) : (-maximum_delta)))
        < (maximum_delta) ? (((target - *value) > (-maximum_delta) ? (target - *value) : (-maximum_delta))) : (maximum_delta));
    *value += *step;
}
static __forceinline void game_sound_background_interpolate(float *value, float target, float maximum_delta)
{
    *value += ((((target - *value) > (-maximum_delta) ? (target - *value) : (-maximum_delta)))
        < (maximum_delta) ? (((target - *value) > (-maximum_delta) ? (target - *value) : (-maximum_delta))) : (maximum_delta));
}
static __forceinline long game_looping_sound_attachment_new_(long object_index, long definition_index, long attachment_index, long identifier)
{
    long sound_index = -1;
    if (definition_index != -1) {
        sound_index = datum_new(game_looping_sound_data);
        if (sound_index != -1) {
            s_looping_sound_attachment_record *sound = game_sound_attachment_datum_get(
                game_looping_sound_data, sound_index);
            sound->definition_index = definition_index;
            sound->sound_index = -1;
            sound->state = _game_looping_sound_state_initial;
            sound->flags.clear();
            sound->flags.set(9, true);
            if (object_index != -1) {
                sound->type = _game_looping_sound_type_object;
                sound->source.object.object_index = object_index;
                sound->source.object.identifier = identifier;
                sound->attachment_index = (short)attachment_index;
            } else {
                sound->attachment_index = -1;
                sound->type = _game_looping_sound_type_unattached;
                sound->source.unattached.marker_name = 0;
                sound->source.unattached.user_index = -1;
                sound->source.unattached.cluster_reference = invalid_cluster_reference_get();
            }
        }
    }
    return sound_index;
}
static __forceinline long unattached_looping_sound_start_(long definition_index, long object_index, float scale)
{
    long sound_index = -1;
    if (definition_index != -1) {
        sound_index = game_looping_sound_attachment_new_(object_index, definition_index, -1, 0);
        if (sound_index != -1) {
            s_looping_sound_attachment_record *sound = game_sound_attachment_datum_get(
                game_looping_sound_data, sound_index);
            sound->flags.set(0, true);
            sound->scale = scale;
        }
    }
    return sound_index;
}
static __forceinline void game_sound_background_loop_stop(long sound_index)
{
    s_looping_sound_attachment_record *sound = game_sound_attachment_datum_get(
        game_looping_sound_data, sound_index);
    sound->flags.set(1, true);
}

// @address 0xfd418 game_sound_update_background_sounds
extern "C" void game_sound_update_background_sounds(const s_cluster_reference &cluster, long user_index, const real_matrix4x3 *matrix,
    long, const s_default_acoustics_definition *override_sound, float delta)
{
    c_static_stack<s_active_background_sound, 3> active;
    long active_palette_index = -1;
    const structure_bsp *bsp = (const structure_bsp *)global_structure_bsp_get(cluster.bsp_index);
    long cluster_index = cluster.cluster_index;
    const real_point3d *position = &matrix->position;
    if (cluster_index != -1) {
        long count = 0;
        c_static_array<long, 64> palette_indices;
        c_static_array<real_decibel, 64> gains;
        c_static_array<long, 64> portal_indices;
        c_static_array<long, 64> cluster_indices;
        long object_index = game_sound_background_user_object(user_index);
        game_sound_find_cluster_attributes<game_sound_background_palette_helper, 64>(global_scenario_index, cluster, matrix,
            game_sound_background_parent(object_index), 20.0f, &palette_indices, &gains, &portal_indices, &cluster_indices, &count);
        c_static_stack<s_tracked_background_sound, 2> tracked;
        for (long index = 0; index < count && !tracked.full(); index++) {
            long palette_index = palette_indices[index];
            long definition_index = game_sound_background_reference(bsp, palette_index, false);
            if (definition_index != -1) {
                bool found = false;
                for (long i = 0; i < tracked.count() && !found; i++)
                    found = tracked.get(i)->definition_index == definition_index;
                if (!found) {
                    tracked.push();
                    s_tracked_background_sound *sound = tracked.get_top();
                    const s_scenario_acoustics_ambience *entry = structure_bsp_get_ambience_palette_entry(bsp, palette_index);
                    sound->definition_index = definition_index;
                    sound->palette_index = (short)palette_index;
                    sound->gain = gains[index];
                    sound->cluster.bsp_index = cluster.bsp_index;
                    sound->cluster.cluster_index = (unsigned char)cluster_indices[index];
                    if (entry->scale_flags & 1) {
                        sound->interior_volume = entry->interior_scale;
                        sound->portal_volume = entry->portal_scale;
                        sound->exterior_volume = entry->exterior_scale;
                    } else {
                        sound->interior_volume = 1.0f;
                        sound->portal_volume = 1.0f;
                        sound->exterior_volume = 1.0f;
                    }
                    if (entry->scale_flags & 8) {
                        float scale = 0.0f;
                        sound->play_alternate = false;
                        sound->interior_volume *= scale;
                        sound->portal_volume *= scale;
                        sound->exterior_volume *= scale;
                    } else {
                        sound->play_alternate = false;
                    }
                    if (portal_indices[index] != -1)
                        sound->distance = structure_distance_to_portal((const structure_bsp *)bsp,
                            (const cluster_portal *)((char *)bsp->cluster_portals.address + portal_indices[index] * 40), position);
                    else
                        sound->distance = 0.0f;
                }
            }
        }
        bool inside = cluster_index == cluster_indices[0];
        if (count > 1 && !tracked.empty() && inside) {
            s_tracked_background_sound *sound = tracked.get(0);
            const s_scenario_acoustics_ambience *entry = structure_bsp_get_ambience_palette_entry(bsp, sound->palette_index);
            for (long index = 1; index < count; index++) {
                long palette_index = palette_indices[index];
                long definition_index = game_sound_background_reference(bsp, palette_index, false);
                if (palette_index != -1 && definition_index == sound->definition_index) {
                    sound->distance = structure_distance_to_portal((const structure_bsp *)bsp,
                        (const cluster_portal *)((char *)bsp->cluster_portals.address + portal_indices[index] * 40), position);
                    if ((entry->scale_flags & 4) || (entry->scale_flags & 2)) {
                        const s_scenario_acoustics_ambience *other = structure_bsp_get_ambience_palette_entry(bsp, palette_index);
                        if (entry->scale_flags & 4)
                            sound->exterior_volume = other->scale_flags & 1 ? other->interior_scale : 1.0f;
                        if (entry->scale_flags & 2)
                            sound->portal_volume = other->scale_flags & 1 ? other->interior_scale : 1.0f;
                    }
                    break;
                }
            }
        }
        for (long index = 0, count = tracked.count(); index < count; index++) {
            s_tracked_background_sound *sound = tracked.get(index);
            const s_scenario_acoustics_ambience *entry = structure_bsp_get_ambience_palette_entry(bsp, sound->palette_index);
            float ratio = game_sound_background_ease(game_sound_background_normalize(0.0f, entry->cutoff_distance, sound->distance, true));
            float target = inside ? sound->interior_volume : sound->exterior_volume;
            sound->volume = (target - sound->portal_volume) * ratio + sound->portal_volume;
            active.push();
            s_active_background_sound *background = active.get_top();
            background->definition_index = sound->definition_index;
            background->gain = sound->gain;
            background->volume = sound->volume;
            background->spatialize = index == 0 && (!cinematic_in_progress() || g_game_sound_globals->environment.ambience_details_enabled);
            background->type = index;
            background->cluster = sound->cluster;
            background->palette_index = sound->palette_index;
            background->maximum_delta = game_sound_background_fade_time(bsp, background->palette_index, delta);
            background->play_alternate = sound->play_alternate;
        }
        if (inside && !tracked.empty())
            active_palette_index = tracked.get(0)->palette_index;
    }
    if (override_sound) {
        for (long index = 0; index < active.count(); index++)
            active.get(index)->gain += override_sound->background_gain;
        if (override_sound->background_sound.index != -1) {
            active.push();
            s_active_background_sound *background = active.get_top();
            background->definition_index = override_sound->background_sound.index;
            background->gain = override_sound->gain;
            background->volume = 1.0f;
            background->maximum_delta = 1.0f;
            background->palette_index = -1;
            background->cluster = invalid_cluster_reference_get();
            background->type = 2;
            background->spatialize = true;
            background->play_alternate = false;
        }
    }
    long *active_cluster_sound = &g_game_sound_globals->listeners[user_index].cluster_loop_index;
    c_static_stack<long, 64> old_loops;
    c_static_flags<64> used_old_loop;
    used_old_loop.clear();
    c_data_iterator<s_looping_sound_attachment_record> iterator;
    iterator.begin(game_looping_sound_data);
    while (iterator.next()) {
        s_looping_sound_attachment_record *sound = iterator.m_datum;
        if (game_sound_background_loop_palette_index(sound) != -1 || game_sound_5_source_test(sound->flags.m_storage, 12)) {
            old_loops.push_back(iterator.m_iterator.index);
            sound->flags.set(10, false);
        }
    }
    for (long index = 0; index < active.count(); index++) {
        const s_active_background_sound *background = active.get(index);
        long sound_index = -1;
        for (long old_index = 0; old_index < old_loops.count(); old_index++) {
            long old_sound_index = *old_loops.get(old_index);
            if (old_sound_index != -1 &&
                game_sound_attachment_datum_get(
                    game_looping_sound_data, old_sound_index)
                        ->definition_index == background->definition_index)
            {
                sound_index = old_sound_index;
                used_old_loop.set(old_index, true);
                break;
            }
        }
        if (sound_index == -1) {
            sound_index = unattached_looping_sound_start_(background->definition_index, -1, background->volume);
            if (sound_index != -1)
                game_sound_attachment_datum_get(
                    game_looping_sound_data, sound_index)
                    ->source.unattached.gain = k_game_sound_background_minimum_gain;
        }
        if (sound_index != -1) {
            float step = 0.0f;
            s_looping_sound_attachment_record *sound = game_sound_attachment_datum_get(
                game_looping_sound_data, sound_index);
            float gain = game_sound_background_gain(sound->source.unattached.gain);
            float maximum_delta = background->maximum_delta;
            float target_gain = game_sound_background_gain(background->gain);
            game_sound_background_interpolate(&gain, &step, target_gain, maximum_delta);
            sound->source.unattached.gain = real_gain_to_decibels(gain);
            sound->flags.set(10, true);
            sound->flags.set(8, !background->spatialize);
            sound->flags.set(4, background->play_alternate);
            if (background->type == 2)
                sound->flags.set(12, true);
            sound->source.unattached.cluster_reference = background->cluster;
            game_sound_background_interpolate(&sound->scale, &step, background->volume, background->maximum_delta);
            sound->source.unattached.user_index = background->palette_index;
        }
    }
    iterator.begin(game_looping_sound_data);
    while (iterator.next()) {
        s_looping_sound_attachment_record *sound = iterator.m_datum;
        long palette_index = game_sound_background_loop_palette_index(sound);
        if ((palette_index != -1 || game_sound_5_source_test(sound->flags.m_storage, 12)) && !game_sound_5_source_test(sound->flags.m_storage, 10)) {
            float maximum_delta = game_sound_5_source_test(sound->flags.m_storage, 12) ? 1.0f : game_sound_background_fade_time(bsp, palette_index, delta);
            float gain = game_sound_background_gain(sound->source.unattached.gain);
            game_sound_background_interpolate(&gain, 0.0f, maximum_delta);
            game_sound_background_interpolate(&sound->scale, 0.0f, maximum_delta);
            if (fabsf(gain) < k_game_sound_background_epsilon)
                game_sound_background_loop_stop(iterator.m_iterator.index);
            else
                sound->source.unattached.gain = real_gain_to_decibels(gain);
        }
    }
    long active_definition = game_sound_background_reference(bsp, active_palette_index, true);
    if (*active_cluster_sound != -1) {
        if (game_sound_attachment_datum_get(
                game_looping_sound_data, *active_cluster_sound)
                ->definition_index != active_definition)
        {
            game_sound_background_loop_stop(*active_cluster_sound);
            *active_cluster_sound = -1;
        }
    }
    if (*active_cluster_sound == -1 && active_definition != -1)
        *active_cluster_sound = unattached_looping_sound_start_(active_definition, -1, 1.0f);
}












static __forceinline bool game_sound_tracking_dead(const object_datum *object) { return ((object->damage_flags >> 2) & 1) != 0; }
static __forceinline bool game_sound_tracking_stop_when_dead(const sound_class_definition *definition) { return ((definition->flags >> 11) & 1) != 0; }
static __forceinline bool game_sound_tracking_node_valid(unsigned char node) { return !((long)node >= 255); }

static __forceinline real_vector3d *game_sound_tracking_transform_normal(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    float x = vector->i;
    float y = vector->j;
    float z = vector->k;
    result->i = (y * matrix->left.i + z * matrix->up.i) + x * matrix->forward.i;
    result->j = (y * matrix->left.j + z * matrix->up.j) + x * matrix->forward.j;
    result->k = (y * matrix->left.k + z * matrix->up.k) + x * matrix->forward.k;
    return result;
}

static __forceinline const long *game_sound_tracking_user_indices() { return tls_player_mapping_globals()->player_indices; }
static __forceinline long game_sound_tracking_first_user()
{
    const long *indices = game_sound_tracking_user_indices();
    long result = -1;
    long user = 0;
    do {
        if (indices[user] != -1) { result = user; break; }
        user++;
    } while (user < 4);
    return result;
}
static __forceinline long game_sound_tracking_audible_cluster(const audibility_table *table, s_cluster_reference reference)
{
    return table->bsps[reference.bsp_index].values[reference.cluster_index];
}
static __forceinline short game_sound_tracking_door_portal_reference(long zone_set_index, long bsp_index, short portal_index)
{
    const scenario *definition = (const scenario *)global_scenario;
    const s_scenario_zone_set *zone_set = &((const s_scenario_zone_set *)definition->zone_sets.address)[zone_set_index];
    const s_scenario_zone_set_visibility *block = &((const s_scenario_zone_set_visibility *)definition->zone_set_pvs.address)[zone_set->pvs_index];
    const s_scenario_bsp_machine_door_mapping *mapping = &((const s_scenario_bsp_machine_door_mapping *)block->machine_door_mappings.address)[bsp_index];
    short portal = ((const unsigned short *)mapping->machine_door_portal_references.address)[portal_index];
    return (short)((bsp_index << 10) | (portal & 1023));
}

// @address 0x1ff7f0 ?track_object_impulse_sound@@YA_NJJPBUs_sound_attachment_data@@PAUs_sound_source@@@Z
bool track_object_impulse_sound(long object_index, long sound_index, const s_sound_attachment_data *attachment, s_sound_source *source)
{
    bool result = false;
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, ~0UL);
    if (object && !object_is_hidden(object_index)
        && (!game_sound_tracking_dead(object)
            || !game_sound_tracking_stop_when_dead(sound_class_get(TAG_GET('snd!', s_cache_file_sound_definition, sound_index)->sound_class))))
    {
        s_location location;
        long tracked_object_index = object_index;
        object_get_location(tracked_object_index, &location);
        if (location.cluster_reference.bsp_index != -1)
        {
            unsigned short node_index = attachment->node_index;
            s_object_header *header = object_header_get(tracked_object_index);
            long object_type = header->object_type;
            node_index = game_sound_tracking_node_valid(attachment->node_index) ? attachment->node_index : 0;
            object_datum *datum = header->datum;
            const real_matrix4x3 *node = (const real_matrix4x3 *)((const char *)datum + datum->node_matrices.offset) + (short)node_index;
            s_sound_location *sound_location = (s_sound_location *)&source->position;
            matrix4x3_transform_point(node, &attachment->position, &sound_location->position);
            real_vector3d forward;
            game_sound_tracking_transform_normal(node, &attachment->forward, &forward);
            game_sound_location_set_rotation_from_direction(sound_location, &forward);
            game_sound_get_object_velocity(tracked_object_index, &source->velocity);
            if ((1 << object_type) & 0x1003)
            {
                if (unit_is_playing_custom_animation(tracked_object_index))
                    scenario_location_from_point(&location, &sound_location->position);
            }
            else if (object_type == 7)
            {
                const s_machine_datum *device = (const s_machine_datum *)object_header_get(tracked_object_index)->datum;
                if (device->pathfinding_object_index != -1 && game_sound_tracking_first_user() != -1)
                {
                    long user_index = game_sound_tracking_first_user();
                    const s_sound_listener *listener = &((const s_sound_manager_globals *)g_sound_manager_globals)->listeners[user_index];
                    if (listener && listener->valid && listener->location.cluster_reference.bsp_index != -1)
                    {
                        audibility_table table;
                        long zone_set_index = global_zone_set_index;
                        actor_perception_initialize_audibility_table(&table, global_scenario_index, zone_set_index);
                        const scenario_zone_set_audibility *audibility = (const scenario_zone_set_audibility *)table.audibility;
                        if (audibility)
                        {
                            s_cluster_reference reference = listener->location.cluster_reference;
                            long cluster = game_sound_tracking_audible_cluster(&table, reference);
                            long portal_offset = game_sound_compute_group_item_offset(device->pathfinding_bsp_index, (device->pathfinding_object_index)) * 2;
                            long word_count = (audibility->room_count + 31) >> 5;
                            long word_index = cluster >> 5;
                            unsigned long mask = 1 << (cluster & 31);
                            long side;
                            if (((unsigned int *)audibility->encoded_door_pas.address)[word_count * portal_offset + word_index] & mask)
                                side = 0;
                            else if (((unsigned int *)audibility->encoded_door_pas.address)[(portal_offset + 1) * word_count + word_index] & mask)
                                side = 1;
                            else
                                side = -1;
                            if (side != -1)
                            {
                                const scenario *scenario_definition = (const scenario *)global_scenario;
                                short packed_portal = game_sound_tracking_door_portal_reference(zone_set_index, device->pathfinding_bsp_index,
                                    (device->pathfinding_object_index));
                                long structure_index = (packed_portal << 16) >> 26;
                                if (structure_index != -1)
                                {
                                    location.cluster_reference.bsp_index = (signed char)structure_index;
                                    location.leaf_index = -1;
                                    long structure_portal = (packed_portal << 22) >> 22;
                                    long tag_index = ((const scenario_structure_bsp_reference *)scenario_definition->structure_bsps.address)
                                        [structure_index].structure_bsp.index;
                                    const structure_bsp *bsp =
                                        TAG_GET('sbsp', const structure_bsp, tag_index);
                                    location.cluster_reference.cluster_index = (unsigned char)((const cluster_portal *)bsp->cluster_portals.address)
                                        [structure_portal].cluster_indices[side];
                                }
                            }
                        }
                    }
                }
            }
            source->location = location;
            signed char spatialization = source->initial_spatialization;
            if (!(*(const unsigned short *)TAG_GET('snd!', s_cache_file_sound_definition, sound_index) & 2))
                update_spatialization_if_first_person(tracked_object_index, (char *)&spatialization, 0);
            source->current_spatialization = spatialization;
            result = true;
        }
    }
    return result;
}
// @address 0x1ff678 ?track_scripted_impulse_sound@@YA_NJJPBUs_sound_attachment_data@@PAUs_sound_source@@@Z
bool track_scripted_impulse_sound(long object_index, long definition_index, const s_sound_attachment_data *attachment, s_sound_source *source)
{
    bool result = (object_index == -1 || (unsigned char)track_object_impulse_sound(object_index, definition_index, attachment, source) != 0);
    if (object_index == -1 || result) {
        const s_sound_override_playback_location *location = game_sound_definition_try_and_get_override_location(TAG_GET('snd!', s_cache_file_sound_definition,
            definition_index));
        
        
        if (location && (location->flags & 1)) {
            source->flags |= 1;
            real_vector3d *velocity = &source->velocity;
            real_point3d *position = &source->position;
            *velocity = *global_zero_vector3d;
            memcpy(position, global_forward3d, sizeof(real_point3d));
            float cos_angle = (float)cos(location->azimuth);
            float sin_f = (float)sin(location->azimuth);
            result = true;
            float a = (position->x) * k_real_zero_storage;
            float b = (position->z) * k_real_zero_storage;
            float c = (position->y) * k_real_zero_storage - a;
            a = b - (position->y);
            b = (position->x) - b;
            position->x = (position->x) * cos_angle + a * sin_f;
            position->z = (position->z) * cos_angle + c * sin_f;
            position->y = (position->y) * cos_angle + b * sin_f;
        }
    }
    return result;
}

typedef c_game_sound_structure_palette_helper<s_scenario_acoustics_palette_entry,
    get_acoustic_reverb, get_acoustic_reverb_cutoff, 28, 264, 30, 588> game_sound_reverb_palette_helper;



static __forceinline long game_sound_update_listeners_reverb_index(const structure_bsp *bsp, long palette_index)
{
    if (palette_index != -1) {
        // match: 0x1fef30: const reference binding preserves temporary lifetime
        const long &index = (long)_rotl((unsigned long)palette_index, 0);
        const s_scenario_acoustics_palette_entry *entry = &((const s_scenario_acoustics_palette_entry *)bsp->acoustics_palette.address)[index];
        if (entry)
            return entry->reverb.sound_environment.index;
    }
    return -1;
}
static __forceinline bool game_sound_user_is_active(long user_index)
{
    bool active = false;
    if (user_index != -1)
        active = tls_player_mapping_globals()->player_indices[user_index] != -1;
    return active;
}
static __forceinline void game_sound_update_listeners_fill(long *destination, long value, long size)
{
    for (unsigned long count = (unsigned long)size / sizeof(long); count != 0; count--)
        *destination++ = value;
}
static __forceinline long game_sound_update_listeners_impulse(long sound_index)
{
    s_game_sound_impulse_internal_parameters parameters;
    parameters.additional_override_flags = 0;
    parameters.source.volume_type = 0;
    parameters.source.flags = 0;
    parameters.source.spatialization = 0;
    parameters.source.volume = 1.0f;
    parameters.source.name = 0;
    parameters.object_index = -1;
    parameters.playback_label = -1;
    parameters.tracking_data = 0;
    parameters.tracking = 0;
    parameters.permutation = 0;
    return game_sound_impulse_new_internal(sound_index, &parameters);
}

// @address 0x1fef30 ?game_sound_update_listeners@@YAXM@Z
void game_sound_update_listeners(float delta)
{
    long fog_definition_indices[4];
    memset(fog_definition_indices, -1, sizeof(fog_definition_indices));
    long count;
    c_static_array<long, 2> palette_indices;
    c_static_array<long, 2> portal_indices;
    c_static_array<long, 2> cluster_indices;
    s_sound_manager_reverbs reverbs;
    s_game_sound_impulse_internal_parameters exit_parameters;
    s_game_sound_impulse_internal_parameters enter_parameters;
    for (long user_index = 0; user_index < 4; user_index++) {
        if (!game_sound_user_is_active(user_index))
            continue;
        const s_sound_listener *listener = (const s_sound_listener *)game_sound_new_3d_manager_get_listener(user_index);
        if (user_index == players_first_active_user() && listener->valid) {
            real_vector3d listener_basis_lengths;
            listener_basis_lengths.i = game_sound24_dot(&listener->matrix.forward, &listener->matrix.forward);
            listener_basis_lengths.j = game_sound24_dot(&listener->matrix.left, &listener->matrix.left);
            listener_basis_lengths.k = game_sound24_dot(&listener->matrix.up, &listener->matrix.up);
            const s_cluster_reference *cluster = &listener->location.cluster_reference;
            signed char structure_bsp_index = listener->location.cluster_reference.bsp_index;
            if (structure_bsp_index != -1 && structure_bsp_is_active(structure_bsp_index)) {
                long cluster_index = listener->location.cluster_reference.cluster_index;
                const real_matrix4x3 *matrix = &listener->matrix;
                real_point3d position = listener->matrix.position;
                const structure_bsp *bsp = (const structure_bsp *)global_structure_bsp_get(structure_bsp_index);
                game_sound_update_background_sounds(*cluster, user_index, matrix, -1,
                    game_sound_try_and_get_default_acoustics_definition(listener->default_acoustics), delta);

                if (cluster_index != -1) {
                    long object_index = game_sound_background_user_object(user_index);
                    game_sound_find_cluster_attributes<game_sound_reverb_palette_helper, 2>(global_scenario_index, *cluster, matrix,
                        game_sound_background_parent(object_index), k_game_sound_reverb_attribute_distance, &palette_indices, 0, &portal_indices,
                            &cluster_indices, &count);
                    if (count <= 1) {
                        for (long index = count; index < 2; index++) {
                            palette_indices.m_storage[index] = -1;
                            portal_indices.m_storage[index] = -1;
                            cluster_indices.m_storage[index] = -1;
                        }
                        count = 2;
                    }
                    if (portal_indices.m_storage[0] == -1)
                        portal_indices.m_storage[0] = portal_indices.m_storage[1];
                }

                reverbs.count = 0;
                const s_default_acoustics_definition *definition =
                    game_sound_try_and_get_default_acoustics_definition(listener->default_acoustics);
                if (definition && definition->reverb.index != -1) {
                    s_sound_manager_reverb reverb;
                    reverb.definition_index = definition->reverb.index;
                    reverb.inverse_cutoff = 10000.0f;
                    reverb.scale = 1.0f;
                    reverb.image.azimuth.lower = 0.0f;
                    reverb.image.azimuth.upper = 6.2831855f;
                    reverb.image.spread = 1.0f;
                    reverb.image.base_volume = 0.0f;
                    long slot = reverbs.count++;
                    reverbs.reverbs[slot] = reverb;
                }
                for (long index = 0; index < count; index++) {
                    if (reverbs.count == 2)
                        break;
                    long palette_index = palette_indices[index];
                    long reverb_index = game_sound_update_listeners_reverb_index(bsp, palette_index);
                    float inverse_cutoff = 2.0f;
                    if (palette_index != -1) {
                        float cutoff = ((const s_scenario_acoustics_palette_entry *)global_scenario->acoustics_palette.address)
                            [palette_index].reverb.interpolation_speed;
                        if (!(fabs(cutoff) < k_game_sound_background_epsilon))
                            inverse_cutoff = 1.0f / cutoff;
                    }
                    s_sound_manager_reverb reverb;
                    reverb.inverse_cutoff = inverse_cutoff;
                    reverb.definition_index = reverb_index;
                    reverb.scale = 1.0f;
                    s_sound_azimuth_image image;
                    if (palette_index != -1) {
                        game_sound_calculate_azimuth_image(listener, &position, (structure_bsp *)bsp, cluster_index, portal_indices[index],
                            ((const s_scenario_acoustics_palette_entry *)global_scenario->acoustics_palette.address)[palette_index].reverb.cutoff_distance,
                                cluster_index == cluster_indices[index], &image);
                    } else {
                        image.azimuth.lower = 0.0f;
                        image.azimuth.upper = 6.2831855f;
                        image.spread = reverb_index == -1 ? 0.0f : 1.0f;
                        image.base_volume = 0.0f;
                    }
                    reverb.image = image;
                    long slot = reverbs.count++;
                    reverbs.reverbs[slot] = reverb;
                }
                ((s_sound_manager_globals *)g_sound_manager_globals)->reverbs = reverbs;
            }
        }

        s_game_sound_listener *state = &((game_sound_globals *)g_game_sound_globals)->listeners[user_index];
        const long &fog_definition_index = fog_definition_indices[user_index];
        if (state->active_fog_definition_index != fog_definition_index) {
            if (state->active_fog_definition_index != -1) {
                const s_planar_fog_definition *fog = TAG_GET('fog ', const s_planar_fog_definition, state->active_fog_definition_index);
                long exit_sound = fog->sound_events[1].index;
                if (exit_sound != -1)
                    unspatialized_impulse_sound_new(exit_sound, 1.0f);
            }
            if (fog_definition_index != -1) {
                const s_planar_fog_definition *fog = TAG_GET('fog ', const s_planar_fog_definition, fog_definition_index);
                long enter_sound = fog->sound_events[0].index;
                if (enter_sound != -1)
                    unspatialized_impulse_sound_new(enter_sound, 1.0f);
            }
            state->active_fog_definition_index = fog_definition_index;
        }
        if (listener->default_acoustics != state->tracked_as_underwater) {
            const s_default_acoustics_definition *entered =
                game_sound_try_and_get_default_acoustics_definition(listener->default_acoustics);
            const s_default_acoustics_definition *exited =
                game_sound_try_and_get_default_acoustics_definition(state->tracked_as_underwater);
            if (exited) {
                exit_parameters.additional_override_flags = 0;
                exit_parameters.source.volume_type = 0;
                exit_parameters.source.flags = 0;
                exit_parameters.source.spatialization = 0;
                exit_parameters.source.volume = 1.0f;
                exit_parameters.source.name = 0;
                exit_parameters.object_index = -1;
                exit_parameters.playback_label = -1;
                exit_parameters.tracking_data = 0;
                exit_parameters.tracking = 0;
                exit_parameters.permutation = 0;
                game_sound_impulse_new_internal(exited->exit_sound.index, (s_game_sound_impulse_internal_parameters *)&exit_parameters);
            }
            if (entered) {
                enter_parameters.additional_override_flags = 0;
                enter_parameters.source.volume_type = 0;
                enter_parameters.source.flags = 0;
                enter_parameters.source.spatialization = 0;
                enter_parameters.source.volume = 1.0f;
                enter_parameters.source.name = 0;
                enter_parameters.object_index = -1;
                enter_parameters.playback_label = -1;
                enter_parameters.tracking_data = 0;
                enter_parameters.tracking = 0;
                enter_parameters.permutation = 0;
                game_sound_impulse_new_internal(entered->enter_sound.index, &enter_parameters);
            }
            state->tracked_as_underwater = listener->default_acoustics;
        }
    }
}

static __forceinline void game_sound_restore_player_effects()
{
    for (long index = 0; index < 2; index++) {
        s_player_effect *effect = &g_player_effects_globals->effects[index];
        if (effect->active) {
            switch ((unsigned long)index) {
            case 0: {
                const s_damage_effect_definition *definition =
                    TAG_GET('jpt!', const s_damage_effect_definition, effect->definition_index);
                game_sound_find_platform_playback_by_label(
                    definition->player_responses[effect->value.effect_index].sound_effect.playback_label);
                break;
            }
            case 1:
                game_sound_find_platform_playback_by_label(effect->definition_index);
                break;
            default:
                // match: 0x1fef30: unreachable hint; removal flips RA ties in game_sound_restore
                __assume(0);
                break;
            }
        }
    }
    game_sound_player_effects_update(0.0f);
}
static __forceinline long game_sound_restore_index_from_absolute(long first_absolute_index)
{
    s_data_array *array = game_looping_sound_data;
    long absolute_index = data_next_absolute_index(array, first_absolute_index);
    long result = -1;
    if (absolute_index != -1)
        result = (*(unsigned short *)(array->data + absolute_index * array->size) << 16) | absolute_index;
    return result;
}
static __forceinline s_looping_sound_attachment_record *game_sound_restore_datum_get(s_data_array *data, const long &index)
{
    return datum_get<s_looping_sound_attachment_record>(data, index);
}

// @address 0x1fc268 ?game_sound_restore@@YAXJ@Z
void game_sound_restore(long flags)
{
    bool restore_music = (flags & (1 << 5)) != 0;
    for (long index = ::data_next_index(game_looping_sound_data, -1); index != -1; index = game_sound_restore_index_from_absolute((index + 1) & 0xffff)) {
        s_looping_sound_attachment_record *sound = game_sound_restore_datum_get(game_looping_sound_data, index);
        bool scripted = game_sound_5_source_test(sound->flags.m_storage, 5);
        if (scripted) {
            if (looping_sound_definition_is_music(sound->definition_index) && restore_music)
                continue;
            const looping_sound_definition *definition =
                TAG_GET('lsnd', const looping_sound_definition, sound->definition_index);
            bool discard = game_sound_5_source_test(sound->flags.m_storage, 7);
            if (discard || (definition->flags & 2)) {
                datum_delete(game_looping_sound_data, index);
            } else if ((1 << (signed char)sound->state) & 3) {
                scripted_looping_sound_reset_internal(sound->definition_index, index);
            }
        }
    }
    if (restore_music) {
        game_sound_restore_iterator iterator;
        game_sound_restore_state state;
        iterator.index = -1;
        while (looping_sound_iterate_scripted(&iterator, &state)) {
            long definition_index = state.definition_index;
            long sound_index = scripted_looping_sound_get_sound_index(definition_index);
            long mode = state.mode;
            if (sound_index == -1) {
                if (mode != _looping_sound_refresh_stop) {
                    if (scripted_looping_sound_get_sound_index(definition_index) != -1 || definition_index == -1)
                        continue;
                    sound_index = music_looping_sound_start(definition_index, state.volume);
                    if (sound_index == -1)
                        continue;
                    scripted_looping_sound_reset_internal(definition_index, sound_index);
                } else {
                    if (definition_index == -1)
                        continue;
                    sound_index = music_looping_sound_start(definition_index, state.volume);
                }
                if (sound_index == -1)
                    continue;
            }
            s_looping_sound_attachment_record *sound = game_sound_restore_datum_get(game_looping_sound_data, sound_index);
            sound->flags.set(4, state.alternate);
            sound->flags.set(7, true);
            sound->source.object.identifier = state.sound_identifier;
            switch ((unsigned long)mode) {
            case _looping_sound_refresh_start:
                sound->state = (e_game_looping_sound_state)_looping_sound_refresh_start;
                break;
            case _looping_sound_refresh_update:
                sound->state = (e_game_looping_sound_state)_looping_sound_refresh_update;
                break;
            default:
                sound->state = (e_game_looping_sound_state)_looping_sound_refresh_stop;
                sound->flags.set(1, true);
                break;
            }
        }
    }
    sound_reset_promotions();
    game_sound_restore_player_effects();
    refresh_listener(false);
    if ((flags & (1 << 4)) && !g_game_sound_globals->environment.suppress_ambience_update_on_revert)
        game_sound_update_listeners(k_game_sound_reverb_attribute_distance);
    g_game_sound_globals->environment.suppress_ambience_update_on_revert = false;
}

struct event_datum;


static const effect_datum *game_sound_update_effect_get(long index, s_data_array *data)
{
    long absolute_index = index & 0xffff;
    const effect_datum *datum = (const effect_datum *)(absolute_index * sizeof(effect_datum) + data->data);
    return datum;
}

static __forceinline void game_sound_update_set_ducker(long ducker)
{
    s_sound_manager_globals *manager = (s_sound_manager_globals *)g_sound_manager_globals;
    if ((long)ducker != manager->ducker) {
        if ((long)ducker != -1 && (long)ducker == manager->previous_ducker) {
            manager->ducker = manager->previous_ducker;
            manager->ducker_time = manager->previous_ducker_time;
            manager->previous_ducker_time = 0.0f;
        } else {
            manager->ducker_time = 0.0f;
            manager->previous_ducker = manager->ducker;
            manager->ducker = ducker;
            manager->previous_ducker_time = 0.0f;
        }
    }
}
static __forceinline long game_sound_update_event_flags_looping(c_flags_no_init<e_event_flags, unsigned char, 4> flags)
{
    return flags.test(_event_started_bit) && !flags.test(_event_finished_bit);
}
static __forceinline long game_sound_update_event_looping(const event_datum *event)
{
    return event && game_sound_update_event_flags_looping(event->flags);
}
static __forceinline unsigned char game_sound_update_effect_looping(long effect_index, unsigned long &result)
{
    const effect_datum *effect = game_sound_update_effect_get(effect_index, effect_data);
    if (effect) {
        long event_index =
            (signed char)(TAG_GET('effe', const s_effect_definition, effect->definition_index))
                ->sound_scale_source;
        if (event_index != -1)
            result = game_sound_update_event_looping((const event_datum *)effect_get_event((const effect_datum *)effect, event_index));
        else
            result = 1;
    }
    return result;
}
static __forceinline long game_sound_update_music_stop_tick(const s_looping_sound_attachment_record *sound)
{
    return sound->type == _game_looping_sound_type_music ? sound->source.object.object_index : -1;
}
static __forceinline unsigned char game_sound_update_effect_sound_looping(
    const s_looping_sound_attachment_record *source, s_looping_sound_update_snapshot sound, unsigned long &fallback)
{
    return game_sound_update_effect_looping(source->source.object.object_index, fallback);
}
static __forceinline void game_sound_update_set_flag(s_looping_sound_attachment_record *sound, long bit, bool enabled)
{
    ((c_flags_no_init<long, unsigned short, 14> *)&sound->flags)->set(bit, enabled);
}
static __forceinline long game_sound_update_root(long object_index)
{
    long root = -1;
    while (object_index != -1) {
        root = object_index;
        object_index = object_header_get(object_index)->datum->parent_object_index;
    }
    return root;
}
static __forceinline bool game_sound_update_location_valid(const s_location *location)
{
    return (unsigned char)location->cluster_reference.bsp_index != 255;
}
static __forceinline const s_sound_listener *game_sound_update_listener_get(long index)
{
    return &((s_sound_manager_globals *)g_sound_manager_globals)->listeners[index];
}
static __forceinline bool game_sound_update_location_audible(const s_location *location)
{
    bool audible = false;
    if (game_sound_update_location_valid(location)) {
        long index = 0;
        while (!audible) {
            if (index >= 4)
                break;
            const s_sound_listener *listener = game_sound_update_listener_get(index);
            if (listener->valid && game_sound_update_location_valid(&listener->location)) {
                audible = scenario_test_pvs(listener->location.cluster_reference, *(const s_cluster_reference *)location);
            }
            index++;
        }
    }
    return audible;
}
static __forceinline bool game_sound_update_sky_audible(long object_index)
{
    long root = game_sound_update_root(object_index);
    long sky_identifier = object_header_get(root)->datum->object_identifier.get_sky_identifier();
    return sky_identifier != -1 && g_valid_skies.test(sky_identifier);
}
static __forceinline float game_sound_update_pin(float value, const float &minimum, const float &maximum)
{
    return (value > minimum ? value : minimum) < maximum ? (value <= minimum ? minimum : value) : maximum;
}
static __forceinline float *game_sound_update_timeline_value()
{
    return &((game_sound_globals *)g_game_sound_globals)->environment.timeline_duck;
}

// @address 0x126f80 ?game_sound_update@@YAXM@Z
void game_sound_update(float elapsed_time)
{
    refresh_listener(false);
    compute_combined_pas();
    game_sound_update_listeners(elapsed_time);
    game_sound_player_effects_update(elapsed_time);
    game_sound_impulses_update();
    long ducker = -1;
    if (g_chud_cinematic_state->definition_index != -1)
        ducker = 2;
    else if (game_sound_looping_cinematic_in_progress())
        ducker = 0;
    else if (((game_sound_globals *)g_game_sound_globals)->scripted_dialog_timer >= game_time_get())
        ducker = 1;
    game_sound_update_set_ducker(ducker);
    if (g_chud_cinematic_state->definition_index != -1)
        *game_sound_update_timeline_value() += elapsed_time;
    else
        *game_sound_update_timeline_value() -= elapsed_time;
    *game_sound_update_timeline_value() = game_sound_update_pin(*game_sound_update_timeline_value(), 0.0f, 1.0f);
    ((s_sound_manager_globals *)g_sound_manager_globals)->timeline_duck = *game_sound_update_timeline_value();
    c_data_iterator<s_looping_sound_attachment_record> iterator;
    iterator.begin(game_looping_sound_data);
    while (iterator.next()) {
        s_looping_sound_attachment_record *sound = iterator.get_datum();
        long sound_index = iterator.get_index();
        switch ((e_game_looping_sound_type)sound->type) {
        case _game_looping_sound_type_music: {
            long stop_tick = game_sound_update_music_stop_tick(sound);
            if (stop_tick != -1 && stop_tick <= game_time_get())
                game_sound_background_loop_stop(sound_index);
            break;
        }
        case _game_looping_sound_type_effect: {
            const unsigned long &sound_flags = sound->flags.m_storage;
            // match: 0x126f80: const reference binding preserves temporary lifetime
            const unsigned long &stop_state = (unsigned char)((unsigned short)sound_flags >> 1);
            unsigned long stopping = stop_state & 1;
            bool looping = !stopping && game_sound_update_effect_sound_looping(sound, s_looping_sound_update_snapshot(*sound), stopping);
            game_sound_update_set_flag(sound, 11, !looping);
            break;
        }
        }
        if (!game_looping_sound_is_attached_to_object(sound)) {
            update_potentially_audible_looping_sound(sound_index);
        } else if ((sound->flags.m_storage & 1) && !object_try_and_get_and_verify_type(game_looping_sound_get_object_index(sound), ~0ul)) {
            if (scripted_looping_sound_get_sound_index(sound->definition_index) == sound_index)
                scripted_looping_sound_reset_internal(sound->definition_index, -1);
            datum_delete(game_looping_sound_data, sound_index);
        } else if (object_is_connected_to_map(game_looping_sound_get_object_index(sound))) {
            bool audible = false;
            bool sky = sound_attachment_is_sky(game_looping_sound_get_object_index(sound));
            if (!sky) {
                long root = game_sound_update_root(game_looping_sound_get_object_index(sound));
                audible = (object_header_get(root)->flags & 1) != 0;
                if (!audible) {
                    s_location location;
                    object_get_location(root, &location);
                    audible = game_sound_update_location_audible(&location);
                }
            } else {
                audible = game_sound_update_sky_audible(game_looping_sound_get_object_index(sound));
            }
            if (audible)
                update_potentially_audible_looping_sound(sound_index);
            else
                game_sound_update_set_flag(sound, 9, false);
        }
    }
    if (game_time_get() > 1)
        ((s_sound_manager_globals *)g_sound_manager_globals)->started = true;
    ((s_sound_manager_globals *)g_sound_manager_globals)->update_frame = true;
    sound_effects_update();
}

// @address 0x11df604 ?game_looping_sound_data@@3V?$c_smart_data_array@Ugame_looping_sound_datum@@@@A
__declspec(thread) c_smart_data_array<game_looping_sound_datum> game_looping_sound_data;

// @address 0x11df700 g_game_sound_globals
extern "C" {
__declspec(thread) game_sound_globals *g_game_sound_globals;
}


__forceinline game_sound_render_message_queue::~game_sound_render_message_queue() {}

// @address 0x707de0 ??__Eg_game_sound_message_queue@@YAXXZ
extern "C" {
game_sound_render_message_queue g_game_sound_message_queue;
}
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_looping_sound_data_allocator> g_game_looping_sound_data_allocator;
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_globals_allocator> g_game_sound_globals_allocator;
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_sound_scripted_impulses_allocator> g_game_sound_scripted_impulses_allocator;

// @address 0x11df7c0 g_game_sound_scripted_impulses
extern "C" {
__declspec(thread) s_scripted_impulse_sound *g_game_sound_scripted_impulses;
}
