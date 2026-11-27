// objectslibxbox/damage.obj: from the records 0xc1dc8 0xdd9e0 0xde6a0 0xf4318 0xf43c8 0xf4ee8 0xf5460 0xf56d8 0xf5770 0xf5930 0xf5a58 0xf5af0 0x120488 0x1d1d38 0x2503e8 0x250408 0x3362d8 0x336398 0x336708 0x3367c0 0x336950 0x336ba0 0x336bc0 0x337350 0x337520 0x337638 0x337698 0x3376a8 0x337798 0x337c40 0x337df0 0x337f80 0x3387d0 0x338d48 0x338e98 0x339050 0x3394d0 0x3396b8 0x339c80 0x339f48 0x339fb0 0x33a1f0 0x33a4e0 0x33a668 0x33a760 0x33a980 0x33b3a8 0x33b5c8 0x33b630 0x33bc50 0x33bfa8 0x33c260 0x33c5d0 0x33c698 0x33c760 0x33c850 0x33c980 0x33ccd8 0x33ce18 0x33d180 0x33d2a8 0x33d3f0 0x33d538 0x33d9d8 0x33dc00 0x33e310
// @flags /TP /O2 /Oc
#include "math/real_math_constants.h"
#include "physics/physics_model_types.h"
#include "effects/effect_creation.h"
#include "objects/damage.h"
#include "objects/damage_reporting_types.h"
#include "models/model_types.h"
#include "game/materials_definitions.h"
#include "objects/object_header.h"
#include "objects/damage_section_response_types.h"
#include "objects/damage_globals.h"
#include "objects/object_reference_types.h"
#include "physics/physics_model_constraint_types.h"
#include "game/game_achievements_types.h"
#include "physics/collision_types.h"
#include "math/real_math.h"
#include "math/random_math_range.h"
#include "memory/data_types.h"
#include "math/matrix_math_types.h"
#include "cseries/cseries.h"
#include "cseries/cseries_wrapped.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "objects/object_types.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
#include "game/game_skulls.h"
#include "game/player_traits.h"
#include "game/player_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "units/unit_passenger_iterator.h"
#include "units/motor_task.h"
#include "units/unit_seat.h"
#include "units/action_request.h"
#include "units/unit_types.h"
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x7832dc ?k_global_material_type_invalid@@3Vc_global_material_type@@B
// @address 0xf58a8 ?bsearch_elements@@YAJPBX0JJP6AJ000@Z0@Z
// @address 0x21a048 ?game_difficulty_get_value_by_difficulty@@YAMFF@Z
// @address 0xfcf30 ?internal_object_get_markers_by_string_id@@YAFJJPAUobject_marker@@F_N@Z
// @address 0x262f60 ?model_find_region_by_name@@YAJJJ@Z
// @address 0x263810 ?model_find_region_by_physics_region_index@@YAJJJ@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xde6a0 ?object_get_damage_info@@YAPBUs_model_damage_info@@J@Z
// @address 0x250408 ?get_rigid_body_index@c_runtime_node_to_physics_model_rigid_body_index_mapping@@QBAJJJ@Z
// @address 0x33b5c8 ?damage_section_damage_all_constraints@@YAXPBUs_model_damage_info@@PBUs_instantaneous_damage_response@@GPAG@Z
#include <stdlib.h>
#include "units/unit_seat_flags.h"
#include "physics/collision_bsp_types.h"
#include "objects/object_type_definition.h"
#include "math/real_math_types.h"
#include "objects/damage_aftermath_types.h"
#include <string.h>
#include <float.h>
#include <ppcintrinsics.h>
#include <new>
#include "game/game.h"
#include "game/game_engine.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "ai/ai.h"
#include "creatures/creatures.h"
#include "effects/effects.h"
#include "game/game_engine_sandbox.h"
#include "game/game_engine_team.h"
#include "game/player_training.h"
#include "hs/object_lists.h"
#include "items/equipment.h"
#include "items/items.h"
#include "items/projectiles.h"
#include "items/weapons.h"
#include "math/integer_math.h"
#include "models/models.h"
#include "objects/damage_owner.h"
#include "physics/havok.h"
#include "physics/havok_utilities.h"
#include "render/render_objects_static_lighting.h"
#include "game/game_allegiance.h"
#include "game/game_engine_spawn_influencer.h"
#include "physics/physics_model_definitions.h"
#include "units/vehicle_utilities.h"
#include "ai/character_definitions.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "cseries/sort.h"
#include "game/game_globals.h"
#include "game/game_globals_inline.h"
#include "objects/object_list_header_types.h"
#include "objects/scenery.h"
#include "physics/runtime_node_rigid_body_indices_types.h"
#include "objects/list_reference_types.h"
#include "objects/objects_tracked_object_types.h"
#include "objects/object_acceleration_info_types.h"
#include "cache/cache_file_globals.h"
#include "scenario/scenario.h"
#include "physics/collisions.h"
#include "physics/havok_component.h"
#include "effects/effect_vector_types.h"
#include "models/model_damage_info.h"

#include "memory/tls_data_arrays.h"
#include "items/item_types.h"
#include "units/unit_definitions.h"
#include "models/model_target_types.h"
#include "models/model_material.h"
#include "physics/runtime_node_to_physics_model_rigid_body_index_mapping.h"
#include "physics/havok_component_class.h"
#include "objects/damage_data.h"
#include "objects/damage_effect_definitions.h"
#include <math.h>
#include "objects/damage_proximity_types.h"
#include "objects/damage_response_data.h"
#include "objects/damage_dependencies.h"
#include "units/biped_definitions.h"
#include "ai/character_behavior_properties.h"
#include "physics/physics_model_constraint_reference.h"
#include "physics/physics_model_constraint_bodies_iterator.h"
#include "objects/object_markers_inline.h"
#include "game/falling_damage_types.h"
#include "physics/havok_contact_point_iterator.h"
#include "physics/havok_rigid_body_state.h"
#include "units/vehicle_datum.h"
#include "effects/player_effects.h"
#include "game/game_engine_scoring_globals.h"
#include "objects/damage_inline.h"
#include "physics/breakable_surface_globals.h"
#include "tag_files/tag_index.h"
#include "ai/actor_datum.h"
#include "units/unit_passenger_iterator_types.h"
#include "objects/damage_section_response.h"
#include "units/giant_datum.h"
#include "render/render_cameras.h"
#include "physics/havok_constants_types.h"
#include "physics/collision_vector_query.h"
#include "items/equipment_definitions.h"
#include "game/player_training_externs.h"
#include "hs/object_list_inlines.h"

struct s_damage_data;
struct s_model_damage_info;
struct s_instantaneous_damage_response;
struct s_damage_seat_info;
struct s_damage_seat_region_state_setting;


struct s_damage_model_constraint {
    long name;
    long group_name;
    long key8;
    float weight;
    short type;
    short index;
};
struct s_damage_seat_info;


struct s_damage_object_datum : object_datum {
    union {
        _unit_datum unit;
        long projectile_flags;
    };
};


struct s_damage_armor_modifier {
    long armor_type;
    float modifier;
};
struct s_damage_material_modifiers {
    long material_type;
    long count;
    TAG_POINTER(s_damage_armor_modifier) modifiers;
    long definition;
};
enum { k_damage_object_flag_unknown2 = 1 << 2, k_damage_object_flag_vitality_pinned = 1 << 16 };
struct s_damage_seat_region_state_setting{float a,b,c;};
struct transfer {long name;short region;short pad;s_damage_seat_region_state_setting damaged,destroyed,dead;};
struct s_damage_seat_info{long flags;float a;union {long pad;float radius;};
    float b,c;long count;TAG_POINTER(transfer) settings;long definition;};
struct s_damage_decay_settings { float current_delay, current_time, recent_delay, recent_time; };
struct s_damage_best_choice { long object_index; float weight; };
enum { MAXIMUM_MODEL_TARGETS_PER_MODEL = 32 };
struct s_area_of_effect_object { float distance_squared; long object_index; bool low_priority; };

c_global_material_type object_get_material(long object_index, long material_index)
{
    c_global_material_type material = k_global_material_type_invalid;
    if (object_index != -1) {
        s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
        unit_definition *definition = (unit_definition *)tag_get('obje', object->definition_index);
        if (definition->model.index != 0xffffffff) {
            s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
            s_damage_model_material *model_material = TAG_BLOCK_GET_ELEMENT(&model->materials, material_index, s_damage_model_material);
            material = model_material->material;
        }
    }
    return material;
}

const s_model_damage_info *object_get_damage_info(long object_index)
{
    const s_model_damage_info *result = 0;
    if (object_index != -1) {
        const s_damage_object_datum *object =
            (const s_damage_object_datum *)object_header_get(object_index)->datum;
        const unit_definition *definition =
            (const unit_definition *)tag_get('obje', object->definition_index);
        unsigned long model_index = definition->model.index;
        if (model_index != 0xffffffff) {
            const s_model_definition *model =
                (const s_model_definition *)tag_get('hlmt', model_index);
            if (model->damage_info.count > 0)
                result = (const s_model_damage_info *)model->damage_info.address;
        }
    }
    return result;
}


namespace {
static __forceinline bool damage_game_skull_is_active_primary(long skull)
{
    unsigned long active_skulls = game_globals->options.game_mode == 1 ? game_globals->options.primary_skulls : 0;
    bool result = ((active_skulls >> skull) & 1) && true;
    return result;
}
}

// @address 0xf5770 ?damage_armor_table_lookup@@YAMJJJJ@Z
float damage_armor_table_lookup(long material_type_a, long material_type_b, long armor_type_a, long armor_type_b)
{
    float scale = 1.0f;
    s_tag_block *table = TAG_BLOCK_GET_ELEMENT(
        &global_game_globals->damage_tables, damage_game_skull_is_active_primary(7) ? 1 : 0, s_tag_block);
    long material_types[2] = { material_type_a, material_type_b };
    long armor_types[2] = { armor_type_a, armor_type_b };
    for (unsigned long material_index = 0; material_index < 2; material_index++) {
        long material_type = material_types[material_index];
        long element_index = bsearch_elements(&material_type, table->address, table->count, sizeof(s_damage_material_modifiers),
            armor_modifier_bsearch_proc, 0);
        if (element_index != -1) {
            s_damage_material_modifiers *material = TAG_BLOCK_GET_ELEMENT(table, element_index, s_damage_material_modifiers);
            for (unsigned long armor_index = 0; armor_index < 2; armor_index++) {
                long armor_type = armor_types[armor_index];
                long modifier_index = bsearch_elements(&armor_type, material->modifiers, material->count, sizeof(s_damage_armor_modifier),
                    armor_modifier_bsearch_proc, 0);
                if (modifier_index != -1)
                    scale *= material->modifiers[modifier_index].modifier;
            }
        }
    }
    return scale;
}

// @address 0xf4ee8 ?object_scripting_vitality_pinned@@YA_NJ@Z
namespace {
static __forceinline bool object_damage_vitality_pinned(const s_damage_object_datum *object)
{
    return (object->damage_flags_words[BIG_ENDIAN_VIEW_INDEX(0, 2)] & 1) != 0;
}
}
bool object_scripting_vitality_pinned(long object_index)
{
    bool result = false;
    if (object_index != -1) {
        const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
        const s_model_damage_info *damage_info = object_get_damage_info(object_index);
        if ((object_damage_vitality_pinned(object) ||
             damage_info && (damage_info->flags & 128) != 0) &&
            object->body_vitality <= 0.0002f)
            result = true;
    }
    return result;
}


// @address 0xf56d8 ?object_get_maximum_shield_vitality@@YAMJ_N@Z
float object_get_maximum_shield_vitality(long object_index, bool ignore_difficulty)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    float maximum_shield_vitality = object->maximum_shield_vitality;
    if (!ignore_difficulty && ((1 << object->object_type) & 0x1003))
        return ::game_difficulty_get_team_value(2, (e_game_team)object->unit.team) * maximum_shield_vitality;
    return maximum_shield_vitality;
}

// @address 0xf5a58 ?object_get_maximum_body_vitality@@YAMJ_N@Z
float object_get_maximum_body_vitality(long object_index, bool ignore_difficulty)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    float maximum_body_vitality = object->maximum_body_vitality;
    if (!ignore_difficulty && ((1 << object->object_type) & 0x1003))
        return ::game_difficulty_get_team_value(1, (e_game_team)object->unit.team) * maximum_body_vitality;
    return maximum_body_vitality;
}


inline long c_runtime_node_to_physics_model_rigid_body_index_mapping::get_rigid_body_count(long node_index) const
{
    long result;
    if (node_index == -1)
        result = entries[256].count;
    else
        result = entries[node_index].count;
    return result;
}

inline long c_runtime_node_to_physics_model_rigid_body_index_mapping::get_rigid_body_index(long node_index, long rigid_body_index) const
{
    if (node_index == -1) {
        return entries[256].indices[rigid_body_index];
    }
    return entries[node_index].indices[rigid_body_index];
}

// @address 0x3362d8 ?object_initialize_vitality@@YAXJPAM0@Z
void object_initialize_vitality(long object_index, float *body_override, float *shield_override)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    const s_model_damage_info *damage_info = object_get_damage_info(object_index);
    float body_vitality = 0.0f;
    float shield_vitality = 0.0f;
    if (damage_info) {
        body_vitality = damage_info->maximum_body_vitality;
        shield_vitality = damage_info->maximum_shield_vitality;
    }
    if (body_override)
        body_vitality = *body_override;
    if (shield_override)
        shield_vitality = *shield_override;
    object->maximum_body_vitality = body_vitality;
    object->maximum_shield_vitality = shield_vitality;
    object->body_vitality = body_vitality > 0.0f ? 1.0f : 0.0f;
    object->shield_vitality = shield_vitality > 0.0f ? 1.0f : 0.0f;
}

namespace {
}
// @address 0x33c5d0 ?object_constraint_is_broken@@YA_NJW4e_physics_model_constraint_type@@J@Z
bool object_constraint_is_broken(long object_index, e_physics_model_constraint_type constraint_type, long constraint_index)
{
    const s_model_damage_info *damage_info = object_get_damage_info(object_index);
    const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    bool result = false;
    if (damage_info) {
        for (long index = 0; index < damage_info->constraints.count; index++) {
            const s_damage_model_constraint *constraint = TAG_BLOCK_GET_ELEMENT(&damage_info->constraints, index, s_damage_model_constraint);
            if (constraint->index == constraint_index && constraint->type == constraint_type) {
                result = (object->broken_constraints & (1 << index)) != 0;
                break;
            }
        }
    }
    return result;
}

// @address 0x33c698 ?object_constraint_is_loosened@@YA_NJW4e_physics_model_constraint_type@@J@Z
bool object_constraint_is_loosened(long object_index, e_physics_model_constraint_type constraint_type, long constraint_index)
{
    const s_model_damage_info *damage_info = object_get_damage_info(object_index);
    const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    bool result = false;
    if (damage_info) {
        for (long index = 0; index < damage_info->constraints.count; index++) {
            const s_damage_model_constraint *constraint = TAG_BLOCK_GET_ELEMENT(&damage_info->constraints, index, s_damage_model_constraint);
            if (constraint->index == constraint_index && constraint->type == constraint_type) {
                result = (object->loosened_constraints & (1 << index)) != 0;
                break;
            }
        }
    }
    return result;
}

static __forceinline bool game_uses_predicted_simulation()
{
    bool result = false;
    if (game_globals->options.simulation_type == 4)
        result = true;
    return result;
}

// @address 0x33c760 ?object_damage_handle_resurrection@@YAXJ@Z
void object_damage_handle_resurrection(long object_index)
{
    if (!game_uses_predicted_simulation()) {
        s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
        const s_model_damage_info *damage_info = object_get_damage_info(object_index);
        object->damage_flags = 0;
        if (damage_info) {
            s_damage_object_section_state *states = (s_damage_object_section_state *)((char *)object + object->damage_sections.offset);
            for (long section_index = 0; section_index < damage_info->sections.count; section_index++) {
                const s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&damage_info->sections, section_index, s_damage_section);
                if (section->flags & 16) {
                    states[section_index].flags = 0;
                    if (section->model_state != -1)
                        ::object_set_model_state(object_index, section->model_state, 0, true);
                }
            }
        }
    }
}

// @address 0x33c850 ?damage_handle_vehicle_exit@@YAXJJ@Z
void damage_handle_vehicle_exit(long object_index, long vehicle_index)
{
    s_object_header *headers = ((s_object_header *)object_header_data->data);
    s_damage_object_datum *object = (s_damage_object_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
    if (!((unsigned char)((object->damage_flags >> 2) & 1))) {
        bool empty = true;
        c_multi_unit_passenger_iterator iterator;
        iterator.begin(vehicle_index);
        while (iterator.next()) {
            s_damage_object_datum *passenger = (s_damage_object_datum *)headers[datum_index_to_absolute_index(iterator.m_passenger_index)].datum;
            if ((1 << passenger->object_type) & 1)
                empty = false;
        }
        if (empty) {
            s_damage_object_datum *vehicle = (s_damage_object_datum *)headers[datum_index_to_absolute_index(vehicle_index)].datum;
            vehicle->m_damage_flags.set(_object_hanging_on_to_life_bit, false);
            if (vehicle->body_vitality < 0.25f)
                vehicle->body_vitality = 0.25f;
            long count = (unsigned int)(short)object->damage_sections.size / 8;
            s_damage_object_section_state *states = (s_damage_object_section_state *)((char *)object + object->damage_sections.offset);
            for (long i = 0; i < count; ++i) {
                if (states[i].damage > 0) {
                    states[i].damage = 0;
                    states[i].recharge_delay_ticks = 0;
                }
            }
        }
    }
}




// match: 0x33b3a8: external storage preserves damage_section_damage_one_constraint code; owner: __real@37800080
// @address 0xb34 k_random_word_normalization

// @address 0x33b3a8 ?damage_section_damage_one_constraint@@YAXPBUs_model_damage_info@@PBUs_instantaneous_damage_response@@GPAG@Z
void damage_section_damage_one_constraint(struct s_model_damage_info const *_sym_p0, struct s_instantaneous_damage_response const *_sym_p1,
    unsigned short mask, unsigned short *chosen)
{
    float opfix_c;
    float total = 0.0f;
    long i;
    for (i = 0; i < _sym_p0->constraints.count; i++) {
        s_damage_model_constraint *choice = TAG_BLOCK_GET_ELEMENT(&_sym_p0->constraints, i, s_damage_model_constraint);
        if (((1 << i) & mask) == 0 && choice->key8 == _sym_p1->constraint_name)
            total += choice->weight;
    }
    float sum = 0.0f;
    opfix_c = total;
    float threshold = (float)(random_seed_next(TLS_RANDOM_SEED) >> 16) * k_random_word_normalization * opfix_c;
    for (i = 0; i < _sym_p0->constraints.count; i++) {
        s_damage_model_constraint *choice = TAG_BLOCK_GET_ELEMENT(&_sym_p0->constraints, i, s_damage_model_constraint);
        if (((1 << i) & mask) == 0 && choice->key8 == _sym_p1->constraint_name) {
            sum += choice->weight;
            if (sum >= threshold) {
                *chosen |= 1 << i;
                return;
            }
        }
    }
}

// @address 0x336ba0 damage_compare_float
extern "C" long damage_compare_float(const float *a, const float *b)
{
    if (*a > *b)
        return 1;
    return -1;
}



// @address 0x337520 ?object_model_targets_destroyed@@YAFJJ@Z
short object_model_targets_destroyed(long object_index, long marker_name)
{
    short targets_destroyed = 0;
    long current_object_index = object_index;

    if (object_index != -1) {
        const cache_file_tag_instance *tag_instances = global_tag_instances;
        object_marker marker;

        do {
            s_damage_object_datum *object = (s_damage_object_datum *)object_try_and_get_and_verify_type(current_object_index, 0xffffffff);
            if (object != 0) {
                if (object_index == current_object_index || object->m_object_flags.test(_object_data_flag_26)) {
                    long model_index =
                        ((const unit_definition *)tag_instances[datum_index_to_absolute_index(object->definition_index)].definition)->model.index;
                    if (model_index != -1) {
                        const s_model_definition *model =
                            (const s_model_definition *)tag_instances[datum_index_to_absolute_index(model_index)].definition;
                        long target_count = model->targets.count;
                        for (long target_index = 0; target_index < target_count; target_index++) {
                                long target_marker_name = TAG_BLOCK_GET_ELEMENT(&model->targets, target_index, s_model_target)->marker_name;
                                if (target_marker_name == marker_name) {
                                    if (internal_object_get_markers_by_string_id(current_object_index, target_marker_name, &marker, 1, false) == 0) {
                                        targets_destroyed = (short)(targets_destroyed + 1);
                                    }
                                }
                        }
                    }
                }
            }
            if (current_object_index != -1) {
                if (current_object_index == object_index) {
                    current_object_index = object->first_child_object_index;
                } else {
                    current_object_index = object->next_object_index;
                }
            }
        } while (current_object_index != -1);
    }

    return targets_destroyed;
}

// @address 0x337698 ?armor_modifier_bsearch_proc@@YAJPBX00@Z
long armor_modifier_bsearch_proc(const void *a, const void *b, const void *context)
{
    return *(const long *)a - *(const long *)b;
}

namespace {
static __forceinline bool damage_object_flag2_set(unsigned long object_index)
{
    const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    return (_rotl(object->damage_flags, 30) & 1) != 0;
}
}
// @address 0x337638 object_set_cannot_die
extern "C" void object_set_cannot_die(unsigned long object_index, bool pinned)
{
    if (object_index == 0xffffffff)
        return;
    if (damage_object_flag2_set(object_index))
        return;
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    if (pinned)
        object->damage_flags |= k_damage_object_flag_vitality_pinned;
    else
        object->damage_flags &= ~k_damage_object_flag_vitality_pinned;
}

namespace {
static __forceinline bool damage_object_type_handle_parent_destroyed(long object_index)
{
    const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    const object_type_definition *definition = object_type_definitions[object->object_type];
    bool result = false;
    for (short index = 0; definition->parts[index]; index++) {
        if (definition->parts[index]->handle_parent_destroyed &&
            definition->parts[index]->handle_parent_destroyed(object_index))
            result = true;
    }
    return result;
}
}
// @address 0x3376a8 ?object_destroy_notify_children@@YAXJ@Z
void object_destroy_notify_children(long object_index)
{
    const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    long child_index = object->first_child_object_index;
    while (child_index != -1) {
        const s_damage_object_datum *child = OBJECT_GET(child_index, s_damage_object_datum, ~0u);
        long next_sibling = child->next_object_index;
        if (!damage_object_type_handle_parent_destroyed(child_index))
            object_destroy_notify_children(child_index);
        child_index = next_sibling;
    }
}


// @address 0x33a760 ?damage_section_response_is_valid@@YA_NPBUs_model_damage_info@@JPBUs_damage_owner@@JJ@Z
bool damage_section_response_is_valid(const s_model_damage_info *model_damage_info, long object_index, const s_damage_owner *damage_owner,
    long section_index, long response_index)
{
    const s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&model_damage_info->sections, section_index, s_damage_section);
    const s_instantaneous_damage_response *response = TAG_BLOCK_GET_ELEMENT(&section->responses, response_index, s_instantaneous_damage_response);
    bool valid = true;
    long region_count;
    char *region_a;
    char *region_b;
    object_region_information *regions;
    ::object_get_region_information(object_index, &region_count, &region_a, &region_b,
        &regions);
    if (response->region_a >= 0 && response->region_a < region_count &&
        regions[response->region_a].state >= response->state_a)
        valid = false;
    if (response->region_b >= 0 && response->region_b < region_count &&
        regions[response->region_b].state >= response->state_b)
        valid = false;
    return valid;
}



void damage_section_damage_all_constraints(const s_model_damage_info *model_damage_info,
    const s_instantaneous_damage_response *instantaneous_damage_response, unsigned short mask, unsigned short *chosen)
{
    for (long index = 0; index < model_damage_info->constraints.count; index++) {
        const s_damage_model_constraint *constraint = TAG_BLOCK_GET_ELEMENT(&model_damage_info->constraints, index, s_damage_model_constraint);
        if (constraint->key8 == instantaneous_damage_response->constraint_name) {
            long bit = 1 << index;
            if (((unsigned short)mask & bit) == 0)
                *chosen = (unsigned short)(*chosen | bit);
        }
    }
}





extern "C" {
// object_list_header_data

// list_object_data

}

// @address 0x120570 ?object_list_get_first@@YAJJPAJ@Z

static __forceinline long list_iterator_next(long *iterator)
{
    long result;
    s_data_array *array = g_list_object_reference_data;
    if (*iterator != -1) {
        s_list_reference *reference = (s_list_reference *)(array->data + (unsigned short)*iterator * array->size);
        long next_index = reference->next_index;
        if (next_index != -1)
            __dcbt(0, array->data + (unsigned short)next_index * array->size);
        result = reference->value;
        *iterator = next_index;
    } else {
        result = -1;
    }
    return result;
}

// @address 0x120488 object_set_damage_taking_internal
long object_set_damage_taking_internal(long list_index, short count)
{
    long iterator;
    long first = -1;
    if (list_index != -1) {
        long absolute = list_index & 0xffff;
        s_object_list_header *header = &((s_object_list_header *)g_object_list_header_data->data)[absolute];
        iterator = header->first_list_reference;
        first = object_list_get_first(list_index, &iterator);
    }
    long result = first;
    while (count > 0) {
        if (result == -1)
            break;
        result = list_iterator_next(&iterator);
        count--;
    }
    return result;
}




static __forceinline c_havok_component *damage_component_get(long component_index)
{
    return datum_get<c_havok_component>(g_havok_component_data, component_index);
}
// @address 0x33ccd8 ?object_find_constraint_search_initial_region@@YAJJ@Z
long object_find_constraint_search_initial_region(long object_index) {
 s_damage_object_datum *o=OBJECT_GET(object_index, s_damage_object_datum, ~0u);
 unit_definition *def=(unit_definition*)tag_get('obje',o->definition_index);
 long initial_region=0x7fffffff; long minimum_node=0x7fffffff;
 if(o->component_index!=-1 && def->model.index!=-1) {
   c_havok_component *c=damage_component_get(o->component_index);
  if(c->is_physics_model_havok_component()) {
   s_model_definition *m=(s_model_definition*)tag_get('hlmt',def->model.index);
   if(m->physics_model_index!=-1) {
    s_physics_model_definition *physics=(s_physics_model_definition*)tag_get('phmo',m->physics_model_index);
    for(long i=0;i<c->get_rigid_body_count();i++) {
     long index_count=c->get_physics_model_rigid_body_index_count(i);
     for(long j=0;j<index_count;j++) {
      long physics_index=c->get_physics_model_rigid_body_index(i,j);
      s_physics_model_rigid_body *body=&((s_physics_model_rigid_body *)physics->rigid_bodies.address)[physics_index];
      if(body->node<minimum_node) { minimum_node=body->node; initial_region=model_find_region_by_physics_region_index(def->model.index,body->region); }
     }
    }
   }
  }
 }
 return initial_region;
}

// @address 0x33d538 ?damage_seat_get_transfer_info@@YAXJPAUs_damage_seat_info@@PAUs_damage_seat_region_state_setting@@@Z
// match: 0x33d538: pointer __restrict qualifier prevents alias reload
void damage_seat_get_transfer_info(long index, struct s_damage_seat_info * info, struct s_damage_seat_region_state_setting * __restrict result){
 result->a=info->a;result->c=info->c;result->b=info->b;
 if(info->count>0){
  transfer *setting=&info->settings[0];
  long state=-1;
  long count;char *a,*b;object_region_information *regions;
  ::object_get_region_information(
      index, &count, &a, &b, &regions);
  if(setting->region>=0 && setting->region<count)state=regions[setting->region].state;
  else if(setting->region==-1 && setting->name!=0){
   s_damage_object_datum *object=OBJECT_GET(index, s_damage_object_datum, -1);
   if(object->parent!=-1){
    s_damage_object_datum *parent=OBJECT_GET(object->parent, s_damage_object_datum, -1);
    unit_definition *definition=(unit_definition *)tag_get(0x6f626a65,parent->definition_index);
    if(definition->model.index!=-1){
     long region=model_find_region_by_name(definition->model.index,setting->name);
     object_region_information *parent_regions;char *parent_a,*parent_b;long parent_count;
     ::object_get_region_information(object->parent, &parent_count, &parent_a,
         &parent_b, &parent_regions);
     if(region>=0 && region<parent_count)state=parent_regions[region].state;
    }
   }
  }
  if(state!=-1){switch(state){case 1:*result=setting->damaged;break;case 2:*result=setting->destroyed;break;case 3:*result=setting->dead;break;}}
 }
}

struct damage_proximity_object;
// @address 0x33d3f0 ?object_determine_rider_damage_scale@@YAMPAUs_damage_data@@JJ@Z
float object_determine_rider_damage_scale(struct s_damage_data * damage, long rider_index, long vehicle_index)
{
    float result = 1.0f;
    s_damage_effect_definition *effect = (s_damage_effect_definition *)tag_get(0, damage->effect);
    s_damage_object_datum *rider = OBJECT_GET(rider_index, s_damage_object_datum, ~0u);
    s_damage_object_datum *vehicle = OBJECT_GET(vehicle_index, s_damage_object_datum, ~0u);
    const s_model_damage_info *info = object_get_damage_info(vehicle_index);
    if (info && ((1 << rider->object_type) & 0x1003) != 0 &&
        ((1 << vehicle->object_type) & 0x1003) != 0 && !(effect->damage.flags & (1 << 13)))
    {
        long seat_index = rider->unit.parent_seat_index;
        if (seat_index != -1)
        {
            unit_definition *definition = (unit_definition *)tag_get(0, vehicle->definition_index);
            unit_seat *seat = &((unit_seat *)definition->seats.address)[seat_index];
            for (long i = 0; i < info->seats.count; ++i)
            {
                s_damage_seat_info *damage_seat_info = TAG_BLOCK_GET_ELEMENT(&info->seats, i, s_damage_seat_info);
                if (damage_seat_info->flags == seat->label)
                {
                    s_damage_seat_region_state_setting settings;
                    ::damage_seat_get_transfer_info(
                        rider_index, damage_seat_info, &settings);
                    result = settings.a * effect->damage.rider_direct_damage_scale;
                    break;
                }
            }
        }
    }
    return result;
}


static __forceinline long damage_game_difficulty_get() {
    if (game_globals->options.game_mode == 1) return game_globals->options.campaign_difficulty;
    return 1;
}
// @address 0x337df0 ?area_effect_infection_form_damage@@YA_NJPAUs_damage_data@@PA_N@Z
// match: 0x337df0: pointer __restrict qualifier prevents alias reload
bool area_effect_infection_form_damage(long index, struct s_damage_data * data, bool * __restrict damage_object) {
    s_damage_object_datum *datum = OBJECT_GET(index, s_damage_object_datum, ~0u);
    bool result = false;
    s_damage_effect_definition *def = (s_damage_effect_definition *)tag_get('jpt!', data->effect);
    if (*damage_object && (def->damage.flags & 0x1000)) {
        *damage_object = false;
        if ((1 << datum->object_type) & 0x800) {
            unit_definition *creature = (unit_definition *)tag_get('crea', datum->definition_index);
            if ((bool)((creature->flags >> 1) & 1) && index != data->owner_data.object_index) {
                float chance = game_difficulty_get_value_by_difficulty(8, (short)damage_game_difficulty_get());
                *damage_object = true;
                if ((def->damage.flags & 0x400) && (data->flags & 64)) *damage_object = false;
                if (chance > 0.0f) {
                    if ((float)(random_seed_next(TLS_RANDOM_SEED) >> 16) * k_random_word_normalization < chance / 2.0f) *damage_object = false;
                }
                if (*damage_object) result = true;
            }
        }
    }
    return result;
}





static __forceinline float damage_proximity_saturate(float value) {
    return ((value > 0.0f ? value : 0.0f) < 1.0f) ? (value > 0.0f ? value : 0.0f) : 1.0f;
}

// @address 0x33e310 damage_proximity_find_nearest
extern "C" bool damage_proximity_find_nearest(damage_proximity_object *object, const real_point3d *point, real_point3d *nearest_point, float *out_weight) {
    bool result = false;
    if (object->selected >= 0 && object->selected < object->limit) {
        float nearest_distance = 1000.0f;
        float total_weight = 0.0f;
        for (long i = 0; i < object->count; i++) {
            if (object->entries[i].index == object->selected) {
                float dx = point->x - object->entries[i].position.x;                float dy = point->y - object->entries[i].position.y;

                float dz = point->z - object->entries[i].position.z;
                float squared = dx * dx;
                squared = dz * dz + squared;
                squared = dy * dy + squared;
                float distance = sqrtf(squared);
                if (distance < object->max_distance) {
                    if (distance < nearest_distance) {
                        nearest_distance = distance;
                        *nearest_point = object->entries[i].position;
                    }
                    float range = object->max_distance - object->min_distance;
                    float weight;
                    if (range > 0.0f) {
                        weight = 1.0f - (distance - object->min_distance) / range;
                        weight = damage_proximity_saturate(weight);
                    } else {
                        weight = 1.0f;
                    }
                    total_weight += weight;
                }
            }
        }
        total_weight = damage_proximity_saturate(total_weight);
        *out_weight = total_weight;
        result = total_weight > 0.0f;
    }
    return result;
}


// @address 0xac7c0 ?datum_try_and_get@@YAPAXPBUs_data_array@@J@Z
// @address 0x265cf8 ?get_damage_resistance_percentage@c_player_trait_shield_vitality@@QBAMXZ

static __forceinline bool damage_game_engine_vehicles_indestructible() {
    return ((game_globals->options.game_variant.variant_flags >> 1) & 1) != 0;
}
static __forceinline float damage_magnitude_squared3d(const real_vector3d *const &v) {
    return v->i * v->i + (v->j * v->j + v->k * v->k);
}
static __forceinline s_variant_object_datum *damage_variant_object_get(long index) {
    s_variant_object_datum *result = 0;
    if ((unsigned long)index <= 639) result = &game_engine_globals->map_info.objects[index];
    return result;
}


// @address 0x338d48 ?damage_set_object_list_flag@@YAXJE@Z
// 0x338d48: matched with its callee tree (ltcg_component record)
void damage_set_object_list_flag(long owner, unsigned char flag) {
    if (owner == -1) return;
    long iterator = datum_get<s_object_list_header>(g_object_list_header_data, owner)->first_list_reference;
    for (long object_index = object_list_get_first(owner, &iterator); object_index != -1; object_index = object_list_get_next(owner, &iterator)) {
        s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
        if (flag == 0) object->damage_flags |= 0x100; else object->damage_flags &= 0xfffffeff;
        long child_index = object->first_child_object_index;
        while (child_index != -1) {
            s_damage_object_datum *child = OBJECT_GET(child_index, s_damage_object_datum, ~0u);
            if (_rotl(child->m_object_flags.m_storage, 6) & 1) {
                unsigned long child_flags = child->damage_flags;
                if (flag == 0) child_flags |= 0x100; else child_flags &= 0xfffffeff;
                child->damage_flags = child_flags;
            }
            child_index = child->next_object_index;
        }
    }
}

// @address 0x339f48 damage_get_object_scale
float damage_get_object_scale(int object_index)
{
    object_datum *object = object_header_get(object_index)->datum;
    int definition_index = object->definition_index;
    int flags = object->physics_flags;
    float scale = ((const unit_definition *)global_tag_instances[(unsigned short)definition_index].definition)->acceleration_scale;
    if ((_rotl(flags, 25) & 1) != 0)
        return scale;
    if ((_rotl(flags, 28) & 1) != 0)
        return scale;
    object_index = object->parent_object_index;
    if (object_index == -1)
        return scale;
    scale = damage_get_object_scale(object_index);
    return scale;
}

// @address 0x73d314 g_damage_acceleration_threshold
float g_damage_acceleration_threshold = 0.05f;
// @address 0x73d40c g_damage_acceleration_threshold_biped
float g_damage_acceleration_threshold_biped = 0.3f;

// @address 0x339fb0 damage_handle_acceleration
extern "C" void damage_handle_acceleration(long object_index, long damage_effect_index, long node_index, const s_damage_owner *damage_owner, float damage_scale,
    const real_vector3d *direction, const real_point3d *point, bool apply_acceleration, bool ignore_acceleration, bool ignore_node_index)
{
    s_damage_effect_definition *effect = (s_damage_effect_definition *)tag_get('jpt!', damage_effect_index);
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    float object_scale = damage_get_object_scale(object_index);
    if (object_scale > 0.001f && apply_acceleration)
    {
        real_vector3d acceleration_direction = *direction;
        float acceleration_scale = effect->damage.instantaneous_acceleration * object_scale;
        if (!ignore_acceleration && acceleration_scale > 0.001f)
        {
            long object_type = object->object_type;
            bool accelerate = true;
            object_set_damage_owner(object_index, damage_owner, false);
            long object_type_mask = 1 << object_type;
            if ((object_type_mask & 0x1003) != 0)
                accelerate = !(object->unit.unit_flags & (1 << 18));
            if (accelerate)
            {
                bool unused = damage_scale > 0.5f && (effect->damage.flags & (1 << 5)) != 0;
                bool set_biped_flag = (effect->damage.flags & (1 << 22)) != 0;
                if (game_secondary_skull_is_active(2))
                {
                    bool apply_skull = true;
                    if ((object_type_mask & 0x1003) != 0 &&
                        (object->unit.player_index != -1 || vehicle_has_passengers(object_index, true, true, -1, 0, 0)))
                        apply_skull = false;
                    if (apply_skull)
                        acceleration_scale *= 2.0f;
                }
                real_vector3d acceleration;
                acceleration.i = acceleration_direction.i * acceleration_scale;
                acceleration.j = acceleration_direction.j * acceleration_scale;
                acceleration.k = acceleration_direction.k * acceleration_scale;
                if ((object_type_mask & 0x1cdf) != 0)
                {
                    float threshold = (object_type_mask & 1) ? g_damage_acceleration_threshold_biped : g_damage_acceleration_threshold;
                    if (acceleration_scale <= threshold)
                        return;
                }
                object_submit_acceleration(object_index, ignore_node_index, node_index, unused, set_biped_flag, point, &acceleration);
            }
        }
    }
}


static __forceinline s_global_material_definition *damage_global_get_material(short index)
{
    s_global_material_definition *result = 0;
    if (index != -1 && index >= 0 && index < global_game_globals->material_count)
        result = TAG_BLOCK_GET_ELEMENT(&global_game_globals->materials, index, s_global_material_definition);
    return result;
}

static __forceinline bool damage_vehicles_indestructible_variant()
{
    game_options *options = &game_globals->options;
    const c_game_variant *variant = options->game_mode == 2 ? &options->game_variant : 0;
    return ((variant->variant_flags >> 1) & 1) != 0;
}

// @address 0x3394d0 ?compute_adjusted_body_damage@@YAMJPBUdamage_definition@@PBUs_damage_data@@PAUs_damage_response_data@@@Z
float compute_adjusted_body_damage(long object_index, const damage_definition *damage, const s_damage_data *data, s_damage_response_data *info)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);

    float body_damage = info->body_damage;
    float resistance_multiplier = 1.0f;
    unsigned long damage_flags = damage->flags;
    short material_index = data->material_type.m_index;
    if (!(damage_flags & 0x100000) && game_is_multiplayer()) {
        if (object->object_type == 0 && object->unit.player_index != -1) {
            player_datum *player = (player_datum *)datum_try_and_get(player_data, object->unit.player_index);
            if (player) {
                float resistance = player->traits.m_shield_vitality_traits.get_damage_resistance_percentage();
                if (resistance > 0.0001f) {
                    if (resistance >= 1000.0f)
                        resistance_multiplier = 0.0f;
                    else
                        resistance_multiplier = resistance_multiplier / resistance;
                }
            }
        } else if (object->object_type == 1 && damage_vehicles_indestructible_variant()) {
            body_damage = 0.0f;
        }
    }
    if ((unsigned char)((object->damage_flags >> 8) & 1))
        body_damage = 0.0f;
    if (((long)(info->definition->flags & 32)) && !(damage_flags & 32))
        body_damage = 0.0f;
    if (material_index == -1 && info->definition)
        material_index = info->definition->indirect_material_type.m_index;
    if (!(data->flags & 256) && damage_global_get_material(material_index)) {
        s_global_material_definition *material = damage_global_get_material(material_index);
        body_damage *= damage_armor_table_lookup(damage->general_damage, damage->specific_damage, material->armor_type_a, material->armor_type_b);
    }
    body_damage *= resistance_multiplier;
    return body_damage;
}

extern "C" {
// @address 0x336398 object_kill

// @address 0x5002b8 actor_new_child_from_object
// @address 0x3309e0 object_activation_regions_update_for_object
}
// @address 0xfc560 ?object_delete@@YAXJ@Z
namespace {
static __forceinline void damage_destroy_unit_grenades(long object_index)
{
    s_damage_object_datum *unit = OBJECT_GET(object_index, s_damage_object_datum, 0x1003);
    if (unit->unit.berserk_charges > 0) {
        action_submit_event(object_index, 47);
        action_submit_event(object_index, 46);
        unit->unit.berserk_charges = 0;
    }
}
static __forceinline void damage_destroy_biped_children(long object_index)
{
    biped_definition *definition = (biped_definition *)tag_get(
        'bipd', OBJECT_GET(object_index, s_damage_object_datum, 1)->definition_index);
    long effect = definition->death_spawn_character.index;
    if (effect != -1 && (unsigned char)((definition->flags >> 8) & 1)) {
        real_point3d origin;
        object_get_origin(object_index, &origin);
        actor_new_child_from_object(object_index, -1, effect, definition->death_spawn_marker, true, &origin);
    }
}
}

// @address 0x336398 object_kill
extern "C" void object_kill(long object_index, const s_damage_data *damage_data, const s_damage_owner *owner, s_damage_response_data *response,
    bool silent, unsigned char reporting_info)
{
    s_damage_response_data * response_loc = response;
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    if (!((unsigned char)((object->damage_flags >> 2) & 1))) {
        if (damage_data) {
            if (damage_data->flags & (1 << 17)) {
                response_loc->effect_index = 0;
                response_loc->response_metadata = 2;
            }
            if (damage_data->flags & (1 << 8)) {
                response_loc->effect_index = 0;
                response_loc->response_metadata = 4;
            }
        }
        object->body_vitality = 0.0f;
        response_loc->flags |= 1;
        object_deplete_shield_internal(object_index, owner, (s_damage_response_data *)response_loc);
        object->damage_flags |= 4;
        simulation_action_object_update(object_index, 1);
        object_update_collision_culling(object_index);
        if (!game_uses_predicted_simulation()) {
            const s_model_damage_info *info = object_get_damage_info(object_index);
            if (info) {
                for (long section_index = 0; section_index < info->sections.count; section_index++) {
                    if (TAG_BLOCK_GET_ELEMENT(&info->sections, section_index, s_damage_section)->flags & 2)
                        damage_section_deplete(info, object_index, owner, reporting_info, section_index, (s_damage_response_data *)response_loc);
                }
            }
        }
        if (!game_uses_predicted_simulation() && object->object_type == 1) {
            long child_index = object->first_child_object_index;
            while (child_index != -1) {
                s_damage_object_datum *child = OBJECT_GET(child_index, s_damage_object_datum, ~0u);
                if (child->object_type == 0) {
                    unsigned short seat_index = child->unit.parent_seat_index;
                    if (seat_index != 0xffff && !unit_seat_test_flag(child->parent, seat_index, _unit_seat_boarding_seat_bit))
                        object_cause_default_damage(child_index, 0, 0, 0, owner, reporting_info);
                }
                child_index = child->next_object_index;
            }
        }
        if (!game_uses_predicted_simulation()) {
            long parent_index = object->parent;
            if (parent_index != -1 && silent) {
                s_damage_object_datum *parent = OBJECT_GET(parent_index, s_damage_object_datum, ~0u);
                if (parent->m_damage_flags.test(_object_hanging_on_to_life_bit) && ((1 << parent->object_type) & 2)) {
                    bool all_passengers_dead = true;
                    c_multi_unit_passenger_iterator iterator;
                    iterator.begin(parent_index);
                    while (iterator.next()) {
                        if ((unsigned short)iterator.seat_index != 0xffff && iterator.passenger_index != object_index)
                            all_passengers_dead = false;
                    }
                    if (all_passengers_dead) {
                        parent->m_damage_flags.set(_object_die_from_no_riders_bit, true);
                        object_wake(object->parent);
                    }
                }
            }
        }
        if (response_loc->broken_constraints || response_loc->loosened_constraints)
            object_damage_constraints(object_index, response_loc->broken_constraints, response_loc->loosened_constraints);
        if ((response_loc->flags & 4) && !game_uses_predicted_simulation())
            object_delete(object_index);
    }
}

// @address 0x336708 object_set_shield_depleted
extern "C" void object_set_shield_depleted(long object_index, bool depleted)
{
    if (depleted)
    {
        s_damage_response_data response;
        memset(&response, 0, sizeof(response));
        object_deplete_shield_internal(object_index, &k_damage_owner_unknown, &response);
    }
    else
    {
        s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
        bool shield_depleted = (object->damage_flags >> 3) & 1;
        if (shield_depleted)
        {
            object->m_damage_flags.set(_damage_shield_depleted, false);
            object_set_model_state_property_per_region(object_index, -1, 2, 0, true);
        }
    }
}
// @address 0x336950 ?object_destroy_internal@@YAXJJJEE@Z
void object_destroy_internal(long object_index, long owner_player_index, long damage_owner_object_index, unsigned char argument, unsigned char silent)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    s_damage_response_data damage_response;
    memset(&damage_response, 0, sizeof(damage_response));
    if (silent)
        damage_response.flags |= 1024;
    else
        damage_response.flags &= ~1024UL;
    object_kill(object_index, 0, &k_damage_owner_unknown, &damage_response, true, argument);
    const s_model_damage_info *info = object_get_damage_info(object_index);
    memset(&damage_response, 0, sizeof(damage_response));
    if (silent)
        damage_response.flags |= 1024;
    else
        damage_response.flags &= ~1024UL;
    if (info) {
        for (long section_index = 0; section_index < info->sections.count; section_index++) {
            const s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&info->sections, section_index, s_damage_section);
            if (section->flags & 8)
                damage_section_deplete(info, object_index, &k_damage_owner_unknown, 0, section_index, (s_damage_response_data *)&damage_response);
        }
    }
    if ((1 << object->object_type) & 0x1003) {
        damage_destroy_unit_grenades(object_index);
        if ((1 << object->object_type) & 1)
            damage_destroy_biped_children(object_index);
    }
    object_destroy_notify_children(object_index);
    if ((unsigned char)((object->physics_flags >> 9) & 1))
        object_activation_regions_update_for_object(object_index, false);
    object_delete(object_index);
}

// @address 0x1d1d38 compute_total_damage
extern "C" void compute_total_damage(long index) {
 s_variant_object_datum *object=damage_variant_object_get(index);
 if(object->helper_object_index!=-1) {
  real_point3d position=object->position;
  real_vector3d forward=object->forward;
  real_vector3d up=object->up;
  map_variant_get_object_transform(&game_engine_globals->map_info,index,&position,&forward,&up);
  if(object->object_index!=-1) {
   if(!game_uses_predicted_simulation() && ::game_engine_is_object_being_edited(object->object_index, (long *)(0))) {
    long object_index = object->object_index;
    object_get_origin(object_index,&position);
    ::object_get_orientation(object_index, &forward, &up);
   } else {
    real_vector3d object_forward,object_up;
    ::object_get_orientation(object->object_index, &object_forward, &object_up);
    if(damage_magnitude_squared3d(&forward)<k_real_math_epsilon_storage) forward=object_forward;
    if(damage_magnitude_squared3d(&up)<k_real_math_epsilon_storage) up=object_up;
   }
   ::object_set_position_direct(object->helper_object_index, &position, &forward,
       &up, 0, false);
  }
 }
}


// @address 0x33d9d8 object_submit_acceleration
extern "C" void object_submit_acceleration(
    long object_index, bool ignore_node_index, long node_index, bool unused, bool set_biped_flag, const real_point3d *point, const real_vector3d *acceleration)
{
    s_object_acceleration_info info;
    memset(&info, 0, sizeof(info));
    bool queued = false;
    info.object_index = object_index;
    info.node_index = node_index;
    memcpy(&info.point, point, sizeof(info.point));
    memcpy(&info.acceleration, acceleration, sizeof(info.acceleration));
    bool valid_type = ((1 << object_header_get(object_index)->object_type) & 0x1003) != 0;
    if (ignore_node_index)
        info.flags |= 1;
    else
        info.flags &= ~1;
    if (unused)
        info.flags |= 2;
    else
        info.flags &= ~2;
    if (set_biped_flag)
        info.flags |= 4;
    else
        info.flags &= ~4;
    s_objects_tracked_object_globals *globals = g_tracked_object_globals;
    if (globals->active) {
        if (globals->count < 64) {
            unsigned long *changed_flags = globals->changed_flags.m_storage;
            if (!valid_type)
                changed_flags[globals->count >> 5] |= 1 << (globals->count & 31);
            else
                changed_flags[globals->count >> 5] &= ~(1 << (globals->count & 31));
            memcpy(&globals->objects[globals->count], &info, sizeof(info));
            globals->count++;
            queued = true;
        }
        if (!queued && valid_type) {
            long index = bit_vector_lowest_bit_set(globals->changed_flags.m_storage, 64);
            if (index != -1) {
                object_apply_acceleration_info((const s_object_acceleration_info *)&globals->objects[index]);
                s_objects_tracked_object_globals *restored = g_tracked_object_globals;
                memcpy(&restored->objects[index], &info, sizeof(info));
                queued = true;
                unsigned long *restored_flags = restored->changed_flags.m_storage;
                restored_flags[index >> 5] &= ~(1 << (index & 31));
            }
        }
    }
    if (!queued)
        object_apply_acceleration_info(&info);
}

// @address 0x33dc00 damage_handle_kill_achievements
extern "C" void damage_handle_kill_achievements(int object_index, int player_index, int other_team, unsigned char damage_type, unsigned char friendly)
{
    int team;
    team = -1;
    s_damage_object_datum *object;
    object = (s_damage_object_datum *)object_header_get(object_index)->datum;
    int value;
    int result;
    int source_index;
    if (object_index != -1) {
        value = game_globals->options.game_mode;
        if (value == 1) {
            team = object->unit.team;
        } else if (value == 2) {
            source_index = object->unit.player_index;
            if (source_index != -1) {
                const player_datum *source_player = DATUM_TRY_AND_GET_CONST(player_data, player_datum, source_index);
                if (source_player != 0) {
                    team = source_player->configuration.host.team_index;
                }
            }
        }
    }
    int datum_index;
    datum_index = player_index & 65535;
    s_data_array *players = player_data;
    if (datum_index < players->first_unallocated) {
        player_datum *player;
        player = (player_datum *)(players->data + players->size * datum_index);
        value = player->identifier;
        if (value != 0) {
            if (value == player_index >> 16) {
                if ((unsigned char)game_team_is_enemy((e_game_team)team, (e_game_team)other_team) != 0) {
                    memmove(player->achievement_damage_info + 1, player->achievement_damage_info,
            sizeof(player->achievement_damage_info) - sizeof(player->achievement_damage_info[0]));
                    player->achievement_damage_info[0] = damage_type;
                    int detail;
                    detail = damage_type & 3;
                    value = (damage_type >> 2) - 7;
                    int parent_index;
                    switch (value) {
                    case 0:
                        value = player->achievement_kill_counters[0] + 1;
                        if (value >= 255) {
                            value = 255;
                        }
                        value = value & 255;
                        player->achievement_kill_counters[0] = (unsigned char)value;
                        if ((unsigned int)value >= 5) {
                            game_results_try_and_grant_achievement(player_index, _achievement_up_close_and_personal);
                            return;
                        } else {
                            return;
                        }
                        break;
                    case _damage_reporting_type_missile_launcher - 7:
                    case _damage_reporting_type_rocket_launcher - 7:
                    case _damage_reporting_type_spartan_laser - 7:
                        if (object != 0) {
                            short seat_index = object->unit.parent_seat_index;
                            if (seat_index != -1) {
                                parent_index = object->parent_object_index;
                                if (parent_index != -1) {
                                    if (!::unit_seat_test_flag(parent_index, seat_index, _unit_seat_boarding_seat_bit)) {
                                        value = parent_index;
                                        do {
                                            result = value;
                                            value = object_header_get(value)->datum->parent_object_index;
                                        } while (value != -1);
                                        if ((unsigned char)vehicle_has_element_value(result, 863) != 0) {
                                            game_results_try_and_grant_achievement(player_index, _achievement_fear_the_pink_mist);
                                            return;
                                        } else {
                                            return;
                                        }
                                    } else {
                                        return;
                                    }
                                } else {
                                    return;
                                }
                            } else {
                                return;
                            }
                        } else {
                            return;
                        }
                        break;
                    case _damage_reporting_type_sniper_rifle - 7:
                    case _damage_reporting_type_beam_rifle - 7:
                        if (detail == 1) {
                            value = player->achievement_kill_counters[1] + 1;
                            if (value >= 255) {
                                value = 255;
                            }
                            value = value & 255;
                            player->achievement_kill_counters[1] = (unsigned char)value;
                            if ((unsigned int)value >= 10) {
                                game_results_try_and_grant_achievement(player_index, _achievement_headshot_honcho);
                                return;
                            }
                        }
                        break;
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 9:
                    default:
                        return;
                    }
                } else {
                    if (friendly == 0) {
                        if ((unsigned char)game_team_is_friendly((e_game_team)team, (e_game_team)other_team) != 0) {
                            memset(player->achievement_damage_info, 0, sizeof(player->achievement_damage_info));
                        }
                    }
                    return;
                }
            } else {
                return;
            }
        } else {
            return;
        }
    } else {
        return;
    }
}

static __forceinline bool object_has_hidden_flag(long object_index)
{
    const s_object_header *header = object_header_get(object_index);
    bool hidden = (header->flags & 16) || (unsigned char)(header->datum->m_object_flags.m_storage & 1);
    return hidden;
}

static __forceinline long damage_player_index_from_unit(long object_index)
{
    const s_damage_object_datum *unit = (const s_damage_object_datum *)object_try_and_get_and_verify_type(object_index, 0x1003);
    long player_index = -1;
    if (unit)
        player_index = unit->unit.player_index;
    return player_index;
}



static __forceinline long damage_seconds_to_ticks_round(float seconds, float half)
{
    return (long)((float)game_time_globals->ticks_per_second * seconds + half);
}
static __forceinline float damage_tick_rate()
{
    return (float)game_time_globals->ticks_per_second;
}
static __forceinline void damage_decay_amount(float delay, float duration, signed char *timer, float *amount, float zero, float one, float half, float epsilon)
{
    if (*amount > zero && *timer >= damage_seconds_to_ticks_round(delay, half)) {
        if (duration > epsilon) {
            *amount -= one / (damage_tick_rate() * duration);
            *amount = *amount < zero ? zero : *amount;
        } else
            *amount = zero;
    }
}




static __forceinline e_game_team damage_object_team(long object_index)
{
    e_game_team team = _game_team_none;
    if (object_index != -1) {
        const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
        if (game_globals->options.game_mode == 1)
            team = (e_game_team)object->unit.team;
        else if (game_globals->options.game_mode == 2) {
            if (object->unit.player_index != -1)
                team = (e_game_team)game_allegiance_get_player_team(object->unit.player_index);
            else
                team = _game_team_none;
        }
    }
    return team;
}
static __forceinline bool damage_teams_are_enemies(e_game_team first, e_game_team second)
{
    bool enemies;
    if ((unsigned long)first <= 15 && (unsigned long)second <= 15)
        enemies = game_team_is_enemy(first, second);
    else
        enemies = false;
    return enemies;
}

// @address 0x33bfa8 vehicle_has_passengers
extern "C" bool vehicle_has_passengers(long object_index, bool players_only, bool include_protected_seats, long player_index,
    bool *driver_is_enemy, long *enemy_passenger_count)
{
    const long object_type = object_header_get(object_index)->object_type;
    long enemy_count = 0;
    e_game_team team;
    if (player_index != -1)
        team = (e_game_team)game_allegiance_get_player_team(player_index);
    else
        team = _game_team_none;
    bool has_passenger = false;
    if (driver_is_enemy) {
        *driver_is_enemy = false;
        if ((1 << object_type) & 2) {
            long seat_index = ::unit_get_driver_seat(object_index);
            if (seat_index != -1) {
                long driver_index = ::unit_seat_get_occupant(object_index, (short)seat_index);
                if (driver_index != -1)
                    *driver_is_enemy = damage_teams_are_enemies(team, damage_object_team(driver_index));
            }
        }
    }
    if ((1 << object_type) & 2) {
        c_multi_unit_passenger_iterator iterator;
        iterator.begin(object_index);
        while (iterator.next()) {
            if (iterator.m_seat_index != -1) {
                const s_damage_object_datum *parent = OBJECT_GET(iterator.m_parent_index, s_damage_object_datum, ~0u);
                const unit_definition *definition =
                    (const unit_definition *)tag_get('obje', parent->definition_index);
                if (include_protected_seats || !((unsigned char)((((unit_seat *)definition->seats.address)[iterator.m_seat_index].flags >> 11) & 1))) {
                    long passenger_index = iterator.m_passenger_index;
                    if (!players_only || damage_player_index_from_unit(passenger_index) != -1) {
                        has_passenger = true;
                        if (!enemy_passenger_count)
                                break;
                        if (passenger_index != -1) {
                            e_game_team passenger_team = damage_object_team(passenger_index);
                            if ((unsigned long)team <= 15 && (unsigned long)passenger_team <= 15 && game_team_is_enemy(team, passenger_team))
                                ++enemy_count;
                        }
                    }
                }
            }
        }
    }
    if (enemy_passenger_count)
        *enemy_passenger_count = enemy_count;
    return has_passenger;
}



static __forceinline long damage_model_find_physics_region(long model_index, long physics_region)
{
    const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', model_index);
    long result = -1;
    for (long i = 0; i < model->regions.count; ++i) {
        if (((const s_model_region *)model->regions.address)[i].physics_region_index == physics_region) {
            result = i;
            break;
        }
    }
    return result;
}
static __forceinline bool damage_region_mask_test(unsigned long mask, long region)
{
    return (mask & (1 << region)) != 0;
}


static __forceinline long damage_mapping_body_count(const c_runtime_node_to_physics_model_rigid_body_index_mapping *, const long &);
static __forceinline long damage_mapping_body_index(const c_runtime_node_to_physics_model_rigid_body_index_mapping *, const long &, long);
static __forceinline const s_physics_model_rigid_body *damage_physics_get_rigid_body(const s_tag_block *bodies, long body_index, long size)
{
    return &((const s_physics_model_rigid_body *)bodies->address)[body_index];
}
// @address 0x33c980 damage_compute_connected_regions
// match: 0x33c980: noalias declaration preserves caller live registers
extern "C" __declspec(noalias) long damage_compute_connected_regions(long object_index, long region_index, long *__restrict unattached_regions)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    unit_definition *definition = (unit_definition *)tag_get('obje', object->definition_index);
    long connected = 1 << region_index;
    *unattached_regions = 0;
    if (definition->model.index != -1)
    {
    s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
    if (model->physics_model_index != -1)
    {
    c_runtime_node_to_physics_model_rigid_body_index_mapping mapping(object_index, true);
    s_physics_model_definition *physics = (s_physics_model_definition *)tag_get('phmo', model->physics_model_index);
    connected |= 1 << region_index;
    long region_count = model->regions.count;
    *unattached_regions = 0;
    bool done = false;
    for (long index = 0; index < region_count; index++)
    {
        if ((unsigned char)((const s_model_region *)model->regions.address)[index].physics_region_index == 255)
            *unattached_regions |= 1 << index;
        else
            *unattached_regions &= ~(1 << index);
    }
    do
    {
        bool changed = false;
        c_physics_model_constraint_bodies_iterator iterator((s_physics_model_definition *)tag_get('phmo', model->physics_model_index));
        s_physics_model_constraint_bodies *bodies = iterator.get();
        while (bodies)
        {
            if (!object_constraint_is_broken(object_index, iterator.get_constraint_type(), iterator.get_constraint_type_index()))
            {
                if (bodies->node_a != -1)
                {
                    if (bodies->node_b != -1)
                    {
                        for (long i = 0; i < damage_mapping_body_count(&mapping, bodies->node_a); i++)
                        {
                            for (long j = 0; j < damage_mapping_body_count(&mapping, bodies->node_b); j++)
                            {
                                long body_a = damage_mapping_body_index(&mapping, bodies->node_a, i);
                                long body_b = damage_mapping_body_index(&mapping, bodies->node_b, j);
                                if (body_a != -1)
                                {
                                    if (body_b != -1)
                                    {
                                        const s_physics_model_rigid_body *rigid_body_a = damage_physics_get_rigid_body(
                                            &physics->rigid_bodies, body_a, sizeof(s_physics_model_rigid_body));
                                        const s_physics_model_rigid_body *rigid_body_b = damage_physics_get_rigid_body(
                                            &physics->rigid_bodies, body_b, sizeof(s_physics_model_rigid_body));
                                        long region_a = damage_model_find_physics_region(definition->model.index, rigid_body_a->region);
                                        long region_b = damage_model_find_physics_region(definition->model.index, rigid_body_b->region);
                                        if (region_a != -1 && region_b != -1)
                                        {
                                            long bit_b = 1 << region_b;
                                            long bit_a = 1 << region_a;
                                            if (((bit_a & connected) != 0) ^ ((bit_b & connected) != 0))
                                            {
                                                changed = true;
                                                connected |= bit_a | bit_b;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            iterator.next();
            bodies = iterator.get();
        }
        if (!changed)
            done = true;
    } while (!done);
    return connected;
    }
    }
    return connected;
}

const float damage_real_one = 1.0f;


static __forceinline long damage_mapping_body_count(const c_runtime_node_to_physics_model_rigid_body_index_mapping *mapping, const long &node)
{
    long count;
    if (node == -1)
        count = mapping->entries[256].count;
    else
        count = mapping->entries[node].count;
    return count;
}
static __forceinline long damage_mapping_body_index(const c_runtime_node_to_physics_model_rigid_body_index_mapping *mapping, const long &node, long index)
{
    signed char body;
    if (node == -1)
        body = mapping->entries[256].indices[index];
    else
        body = mapping->entries[node].indices[index];
    return body;
}


// @address 0x338e98 ?object_damage_update_decay@@YA_NPBUcharacter_vitality_properties@@_NPAM2PAD@Z
bool object_damage_update_decay(const character_vitality_properties *properties, bool dead, float *current_damage, float *recent_damage, char *decay_timer)
{
    bool result = false;
    if (*decay_timer != -1) {
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x338e98: 0.0f, 2.0f, 0.0001f, 0.5f and 1.0f load off one 0x820221fc base register
        const float zero = k_real_constant_pool[200];
        float current_time = k_real_constant_pool[2103];
        float recent_delay = k_real_constant_pool[2103];
#else
        const float zero = 0.0f;
        float current_time = 2.0f;
        float recent_delay = 2.0f;
#endif
        float recent_time = current_time;
        float current_delay = zero;
        if (properties) {
            current_delay = properties->current_damage_decay_delay;
            current_time = properties->current_damage_decay_time;
            recent_delay = properties->recent_damage_decay_delay;
            recent_time = properties->recent_damage_decay_time;
        }
        long timer = *decay_timer + 1;
        if (timer > 127)
            timer = 127;
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x338e98: shared constant-pool base register (spelled literals reload a base per constant)
        const float epsilon = k_real_constant_pool[2500];
        const float half = k_real_constant_pool[753];
        const float one = k_real_constant_pool[0];
#else
        const float epsilon = 0.0001f;
        const float half = 0.5f;
        const float one = 1.0f;
#endif
        *decay_timer = (char)timer;
        if (*current_damage > zero && *decay_timer >= damage_seconds_to_ticks_round(current_delay, half)) {
            if (current_time > epsilon) {
                *current_damage -= one / (damage_tick_rate() * current_time);
                *current_damage = *current_damage >= zero ? *current_damage : zero;
            } else
                *current_damage = zero;
        }
        if (*recent_damage > zero && *decay_timer >= damage_seconds_to_ticks_round(recent_delay, half)) {
            if (recent_time > epsilon) {
                *recent_damage -= one / (damage_tick_rate() * recent_time);
                *recent_damage = *recent_damage >= zero ? *recent_damage : zero;
            } else
                *recent_damage = zero;
        }
        if (dead) {
            *current_damage = zero;
            if (*recent_damage == zero)
                *decay_timer = -1;
        } else if (*current_damage == zero && *recent_damage == zero)
            *decay_timer = -1;
        result = true;
    }
    return result;
}
// @address 0x337c40 ?area_effect_damage_consider_object@@YA_NJPBUs_damage_data@@@Z
bool area_effect_damage_consider_object(long object_index, const s_damage_data *damage_data)
{
    const s_damage_effect_definition *effect = (const s_damage_effect_definition *)tag_get('jpt!', damage_data->effect);
    const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    bool result = false;
    if ((!damage_data->damage_identifier || damage_data->damage_identifier != object->damage_identifier) && !object_has_hidden_flag(object_index)) {
        if (effect->effect_flags & 2) {
            result = damage_player_index_from_unit(object_index) != -1;
        } else {
            const s_object_header *header = object_header_get(object_index);
            if ((!(effect->damage.flags & 1) || object_index != damage_data->owner_data.object_index) &&
                (!((1 << header->object_type) & 64) || !(((const s_scenery_object *)header->datum)->flags & 4)) &&
                (!((1 << object->object_type) & 0x1003) ||
                 ((!(effect->damage.flags & 0x8000) || game_globals->options.game_mode != 1 || object->unit.player_index == -1) &&
                  (!(effect->damage.flags & 8) || game_team_is_enemy((e_game_team)object->unit.team, (e_game_team)damage_data->owner_data.team)))))
                result = true;
        }
    }
    return result;
}


static __forceinline long damage_unit_get_player_index(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_try_and_get_and_verify_type(unit_index, 0x1003);
    long player_index = -1;
    if (unit)
        player_index = unit->unit.player_index;
    return player_index;
}
// @address 0x33a1f0 damage_compute_amount
extern "C" float damage_compute_amount(s_damage_data *data, const s_damage_effect_definition *effect, const damage_definition *bounds,
    long object_index, bool *difficulty_scaled)
{
    float minimum = bounds->damage_upper_bound.lower;
    float maximum = bounds->damage_upper_bound.upper;
    *difficulty_scaled = false;
    float value;
    if (minimum == maximum)
        value = minimum;
    else {
        float fraction = (float)(random_seed_next(TLS_RANDOM_SEED) >> 16) * 1.5259022e-5f;
        value = minimum + fraction * (maximum - minimum);
    }
    float damage = data->amount * value + (1.0f - data->amount) * bounds->damage_lower_bound;
    if (data->close) damage += effect->aoe_spike_damage_bump;
    damage *= data->multiplier;
    if (!(data->flags & 0x100))
    {
        ai_adjust_damage(data->owner.object_index, object_index, (s_damage_data *)data, &damage);
        if (damage > 0.0f && data->owner.object_index != -1 && game_globals->options.game_mode == 1)
        {
            unit_datum *owner = (unit_datum *)object_try_and_get_and_verify_type(data->owner.object_index, 0x1003);
            if (owner)
            {
                s_damage_object_datum *object = (s_damage_object_datum *)object_header_get(object_index)->datum;
                if (owner->unit.gunner_object_index != -1) owner = (unit_datum *)object_header_get(owner->unit.gunner_object_index)->datum;
                unit_datum *unit = (unit_datum *)object;
                if (((1 << object->object_type) & 0x1003) && (bool)((unit->unit.unit_flags >> 29) & 1) && owner->unit.team != 1)
                    damage = 0.0f;
            }
        }
        if (game_engine_running())
        {
            long player_index = -1;
            for (long current = object_index; current != -1; current = object_header_get(current)->datum->parent)
            {
                if (object_try_and_get_and_verify_type(current, 0x1003))
                {
                    player_index = damage_unit_get_player_index(current);
                    break;
                }
            }
            float multiplier = game_engine_spawn_influencers_add_ally_bias(data->owner.player_index, player_index);
            damage *= multiplier;
            if (multiplier == 0.0f) data->in_range = 0;
        }
        else if (data->owner.team != -1 && game_globals->options.game_mode == 1 && game_team_is_enemy((e_game_team)data->owner.team, _game_team_player))
        {
            short difficulty = game_globals->options.game_mode == 1 ? game_globals->options.campaign_difficulty : 1;
            damage *= game_difficulty_get_value_by_difficulty(0, difficulty);
            *difficulty_scaled = true;
        }
    }
    if (::game_engine_is_object_being_edited(object_index, (long *)(0))) damage = 0.0f;
    return damage;
}

// @address 0xf4318 ?damage_data_new@@YAXPAUs_damage_data@@J@Z
void damage_data_new(s_damage_data *damage, long definition_index)
{
    memset(damage, 0, sizeof(*damage));
    const s_damage_owner *owner = damage_owner_unknown();
    damage->effect = definition_index;
    damage->material_type = k_global_material_type_invalid;
    damage->model_region_index = -1;
    damage->owner_data = *owner;
    damage->damage_owner_object_index = -1;
    location_invalidate(&damage->location);
    (damage->scale)[0] = 1.0f;
    (damage->scale)[1] = 1.0f;
    damage->object_index = -1;
    (damage->scale)[2] = 1.0f;
    damage->ultimate_parent_index = -1;
    damage->shot_id = -1;
    damage->reporting_info = 0;
}

typedef long (c_runtime_node_to_physics_model_rigid_body_index_mapping::*damage_mapping_count_member)(long) const;
typedef long (c_runtime_node_to_physics_model_rigid_body_index_mapping::*damage_mapping_index_member)(long, long) const;
damage_mapping_count_member damage_mapping_count_odr_use() { return &c_runtime_node_to_physics_model_rigid_body_index_mapping::get_rigid_body_count; }
damage_mapping_index_member damage_mapping_index_odr_use() { return &c_runtime_node_to_physics_model_rigid_body_index_mapping::get_rigid_body_index; }

// @address 0x33a668 ?damage_spawn_damage_effect_from_marker_name@@YAXJJPBUs_damage_owner@@JE@Z
void damage_spawn_damage_effect_from_marker_name(
    long damage_effect_index, long object_index, const s_damage_owner *owner, long marker_name, unsigned char reporting_info)
{
    object_marker marker;
    long marker_count = internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false);
    if (marker_name == -1 || marker_name == 0 || marker_count != 0)
    {
        s_damage_data damage_data;
        damage_data_new(&damage_data, damage_effect_index);
        damage_data.owner_data = *owner;
        damage_data.scale[0] = 1.0f;
        damage_data.reporting_info = reporting_info;
        object_get_location(object_index, &damage_data.location);
        damage_data.position = marker.matrix.position;
        damage_data.origin = damage_data.position;
        damage_data.direction = marker.matrix.forward;
        damage_data.object_index = object_index;
        area_of_effect_cause_damage(&damage_data, -1, -1);
    }
}

// @address 0x33d180 object_cause_default_damage
extern "C" void object_cause_default_damage(
    long object_index, unsigned char a, unsigned char b, unsigned char c, const s_damage_owner *owner, unsigned char reporting_info)
{
    if ((unsigned char)((OBJECT_GET(object_index, s_damage_object_datum, ~0u)->damage_flags >> 2) & 1))
        return;

    s_falling_damage_definition *options = (s_falling_damage_definition *)global_game_globals->falling_damage.address;
    if (options->falling_damage.index == -1)
        return;

    s_damage_data damage_data;
    damage_data_new(&damage_data, options->falling_damage.index);
    damage_data.reporting_info = reporting_info;
    damage_data.scale[0] = 1.0f;
    if (owner)
        damage_data.owner_data = *owner;
    damage_data.flags |= 4;
    if (a)
        damage_data.flags |= 0x10;
    else
        damage_data.flags &= ~0x10;
    if (b)
        damage_data.flags |= 0x80;
    else
        damage_data.flags &= ~0x80;
    if (c)
        damage_data.flags |= 0x800;
    else
        damage_data.flags &= ~0x800;
    object_cause_damage(&damage_data, object_index, -1, -1, -1, 0);
}


// match: noalias declaration preserves caller live registers
static __forceinline void damage_placement_data_set_location(object_placement_data *placement, const s_location *location)
{
    placement->location = *location;
    placement->placed_by_script = true;
}

// @address 0x33ce18 ?object_damage_constraints@@YAXJGG@Z
void object_damage_constraints(long object_index, unsigned short broken_constraints, unsigned short loosened_constraints)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    unit_definition *definition = (unit_definition *)tag_get('obje', object->definition_index);
    const s_model_damage_info *damage_info = object_get_damage_info(object_index);
    object_set_at_rest(object_index, false);
    if (damage_info) {
        long constraint_count = damage_info->constraints.count;
        for (long index = 0; index < constraint_count; index++) {
            long bit = 1 << index;
            if ((broken_constraints & bit) != 0)
                object->broken_constraints = (unsigned short)(object->broken_constraints | bit);
            if ((loosened_constraints & bit) != 0)
                object->loosened_constraints = (unsigned short)(object->loosened_constraints | bit);
        }
    }
    long initial_region = object_find_constraint_search_initial_region(object_index);
    long unattached_regions;
    long connected_regions = damage_compute_connected_regions(object_index, initial_region, &unattached_regions);
    if (definition->model.index != 0xffffffff && object->component_index != -1) {
        long component_index = object->component_index;
        c_havok_component *havok_component = (c_havok_component *)damage_component_get(component_index);
        s_havok_component_rigid_body_state rigid_body_state;
        havok_component->save_rigid_body_state(&rigid_body_state);
        if (broken_constraints != 0) {
            char *region_a;
            object_region_information *regions;
            long region_count;
            long disconnected_regions = 0;
            object_get_region_information(object_index, &region_count, &region_a, 0, &regions);
            long count = region_count;
            object_region_information *region = regions;
            for (long index = 0; index < count; index++) {
                if (region_a[index] != -1) {
                    long bit = 1 << index;
                    if ((bit & connected_regions) == 0 && (bit & unattached_regions) == 0) {
                        region_a[index] = -1;
                        disconnected_regions |= bit;
                        region->variant_index = -1;
                        region->state = 0;
                        region->properties = 0;
                        region->effect = -1;
                    }
                }
                region++;
            }
            if (disconnected_regions != 0 && definition->crate_object.index != -1) {
                do {
                    long unconnected_region = -1;
                    for (long index = 0; index < count; index++) {
                        if (((1 << index) & disconnected_regions) != 0) {
                            unconnected_region = index;
                                break;
                        }
                    }
                    long unattached;
                    long remaining = damage_compute_connected_regions(object_index, unconnected_region, &unattached);
                    disconnected_regions &= ~remaining;
                    if (game_globals->options.game_mode != 2) {
                        object_placement_data placement;
                        object_placement_data_new(&placement, definition->crate_object.index, object_index, 0);
                        damage_placement_data_set_location(&placement, &((object_datum *)object)->location);
                        object_get_origin(object_index, &placement.position);
                        placement.up = ((object_datum *)object)->up;
                        placement.forward = ((object_datum *)object)->forward;
                        object_get_velocities(object_index, &placement.linear_velocity, &placement.angular_velocity);
                        placement.unconnected_region_mask = ~remaining;
                        placement.broken_constraints = object->broken_constraints;
                        placement.loosened_constraints = object->loosened_constraints;
                        long new_object = object_new(&placement);
                        if (new_object != -1) {
                            s_damage_object_datum *new_object_datum = OBJECT_GET(new_object, s_damage_object_datum, ~0u);
                            if (new_object_datum->component_index != -1) {
                                long new_component_index = new_object_datum->component_index;
                                ((c_havok_component *)damage_component_get(new_component_index))->restore_rigid_body_state(&rigid_body_state);
                            }
                            long rate = game_time_globals->ticks_per_second;
                            object_set_garbage(new_object, true, (rate + (rate << 2)) << 1);
                        }
                    }
                } while (disconnected_regions != 0);
            }
        }
        havok_object_rebuild(object_index, 0);
        havok_component->restore_rigid_body_state(&rigid_body_state);
    }
}

// @address 0x337350 ?damage_effect_new_on_weapon@@YAXJJ@Z
void damage_effect_new_on_weapon(long weapon_index, long damage_effect)
{
    s_damage_object_datum *weapon = OBJECT_GET(weapon_index, s_damage_object_datum, 4);
    long unit_index = -1;
    if (((const item_datum *)weapon)->owner_relation)
        unit_index = weapon->unit.actor_index;
    if (damage_effect != -1 && unit_index != -1)
    {
        s_damage_object_datum *unit = OBJECT_GET(unit_index, s_damage_object_datum, 0x2003);
        unsigned long group_tag = tag_get_group_tag(damage_effect);
        real_vector3d direction;
        negate_vector3d(&unit->unit.aiming_vector, &direction);
        if (group_tag == 'jpt!' && !game_uses_predicted_simulation())
        {
            s_damage_data damage_data;
            damage_data_new(&damage_data, damage_effect);
            damage_data.flags |= 8;
            damage_data.direction = direction;
            damage_data.position = unit->bounding_center;
            damage_data.origin = damage_data.position;
            object_cause_damage(&damage_data, unit_index, -1, -1, -1, 0);
        }
        else
        {
            s_damage_owner owner;
            owner.player_index = -1;
            owner.object_index = -1;
            owner.team = -1;
            unsigned char *owner_bytes = reinterpret_cast<unsigned char *>(&owner);
            memset(owner_bytes + offsetof(s_damage_owner, reporting_info), 0, sizeof(owner) - offsetof(s_damage_owner, reporting_info));
            player_effect_start(unit_index, -1, damage_effect, &owner, &direction, 1.0f, 1.0f, 0, false);
        }
    }
}
#ifndef PIN
#endif
static const float k_damage_cone_epsilon = 0.0001f;

// @address 0x337f80 damage_handle_area_effect
extern "C" void damage_handle_area_effect(s_damage_data *damage, long object_index, long exclude_index, bool is_child, s_damage_best_choice *best)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    bool consider = area_effect_damage_consider_object(object_index, damage);
    bool infected = area_effect_infection_form_damage(object_index, damage, &consider);
    if (consider) {
        const s_model_damage_info *damage_info = object_get_damage_info(object_index);
        float proximity_weight = 1.0f;
        bool in_cone = false;
        bool discard = false;
        const s_damage_effect_definition *effect_definition = (const s_damage_effect_definition *)tag_get('jpt!', damage->effect);
        bool overlapping = (object->physics_flags >> 9) & 1;
        real_point3d position;
        float bounding_radius;
        object_get_bounding_sphere(object_index, &position, &bounding_radius);
        real_point3d center;
        object_get_center_of_mass(object_index, &center);
        real_point3d original_center = center;
        float radius = bounding_radius;
        if (object->component_index != -1) {
            real_rectangle3d bounds;
            if (((c_havok_component *)damage_component_get(object->component_index))->calculate_physics_bounding_box(&bounds))
                radius = (bounds.z1 - bounds.z0) * 0.5f;
            c_havok_contact_point_iterator iterator(object_index, false);
            overlapping |= iterator.valid();
        }
        const s_object_header *header = object_header_get(object_index);
        if (((1 << header->object_type) & 2) != 0)
            overlapping |= ((const vehicle_datum *)header->datum)->vehicle.braking_ticks != 0;
        float height;
        if ((((object->physics_flags >> 4) & 1) | overlapping) != 0)
            height = radius * 0.75f;
        else
            height = radius * 0.05f;
        original_center.z = center.z + height;
        if (exclude_index != object_index)
            normalize3d_with_default(vector_from_points3d(&damage->position, &original_center, &damage->direction), global_up3d);
        damage->initial_direction = damage->direction;
        damage_proximity_object *proximity = damage->proximity;
        if (proximity != 0) {
            real_point3d origin;
            object_get_origin(object_index, &origin);
            if (!damage_proximity_find_nearest(proximity, &origin, &damage->position, &proximity_weight)) {
                discard = true;
                proximity_weight = 0.0f;
            }
        }
        float dist;
        real_point3d closest;
        real_vector3d normal;
        if (object->component_index != -1 &&
            calculate_closest_point_and_normal_on_havok_component(object->component_index, &damage->position, &closest, &normal))
        {
            dist = sqrtf(distance_squared3d(&damage->position, &closest));
            damage->distance = dist;
            if (!(dist > 0.0f))
                dist = 0.0f;
            damage->position = closest;
        }
        else {
            dist = sqrtf(distance_squared3d(&damage->position, &position)) - bounding_radius;
            damage->distance = dist;
            if (!(dist > 0.0f))
                dist = 0.0f;
        }
        damage->distance = dist;
        if (damage->proximity == 0) {
            if (effect_definition->damage.dmg_outer_cone_angle != 0.0f) {
                if (fabsf(1.0f - magnitude_squared3d(&damage->cone_direction)) > k_damage_cone_epsilon) {
                    in_cone = true;
                }
                else {
                    float angle = arccosine(dot_product3d(&damage->cone_direction, &damage->direction));
                    if (angle < effect_definition->damage.dmg_inner_cone_angle + k_damage_cone_epsilon) {
                        in_cone = true;
                    }
                    else if (angle < effect_definition->damage.dmg_outer_cone_angle) {
                        in_cone = true;
                        float t = 1.0f -
                            (angle - effect_definition->damage.dmg_inner_cone_angle) /
                                (effect_definition->damage.dmg_outer_cone_angle - effect_definition->damage.dmg_inner_cone_angle);
                        damage->cone_amount = PIN(t, 0.0f, 1.0f);
                    }
                    else {
                        discard = true;
                        proximity_weight = 0.0f;
                    }
                }
            }
            else {
                in_cone = true;
            }
        }
        if (in_cone) {
            float falloff = 1.0f;
            float range = effect_definition->radius.upper - effect_definition->radius.lower;
            if (object_index == damage->ultimate_parent_index)
                damage->distance = 0.0f;
            if (range > 0.0f) {
                falloff = 1.0f - (damage->distance - effect_definition->radius.lower) / range;
                if (!(falloff >= 0.0f))
                    discard = true;
                falloff = PIN(falloff, 0.0f, 1.0f);
            }
            proximity_weight = falloff * proximity_weight;
            if (!(damage->distance >= effect_definition->aoe_spike_radius))
                damage->close = 1;
        }
        damage->weight = proximity_weight;
        damage->shield_amount = proximity_weight;
        if (discard) {
            damage->cone_amount = 0.0f;
            damage->flags |= 0x2000;
        }
        if ((effect_definition->effect_flags & 1) == 0)
            damage->amount = proximity_weight;
        if (effect_definition->shake_radius > 0.0f) {
            s_damage_object_datum *unit = (s_damage_object_datum *)object_try_and_get_and_verify_type(object_index, 0x1003);
            if ((unit == 0 || unit->unit.player_index == -1) && !vehicle_has_passengers(object_index, true, true, -1, 0, 0))
                damage->shield_amount = 0.0f;
            else {
                float t = damage->distance / effect_definition->shake_radius;
                damage->shield_amount = 1.0f - PIN(t, 0.0f, 1.0f);
            }
        }
        bool in_range = damage->distance < effect_definition->emp_radius;
        damage->in_range = in_range;
        if (damage->weight > 0.0f || in_range || damage->shield_amount > 0.0f) {
            bool blocked = false;
            if (!is_child)
                blocked = area_effect_damage_test_obstruction(object_index, &center, damage);
            if (!blocked) {
                long owner_index = damage->object_index;
                bool cinematic_owner = false;
                if (owner_index != -1 && object_try_and_get_and_verify_type(owner_index, ~0u) != 0) {
                    const s_damage_object_datum *owner = (const s_damage_object_datum *)object_header_get(owner_index)->datum;
                    cinematic_owner = (owner->flags_d6 >> 1) & 1;
                }
                float final_weight = damage->weight;
                if (final_weight > 0.0f) {
                    if (final_weight > best[0].weight) {
                        best[0].weight = final_weight;
                        best[0].object_index = object_index;
                    }
                    if (((1 << object_header_get(object_index)->object_type) & 0x1003) != 0) {
                        if (final_weight > best[1].weight) {
                            best[1].weight = final_weight;
                            best[1].object_index = object_index;
                        }
                        s_damage_object_datum *unit = (s_damage_object_datum *)object_try_and_get_and_verify_type(object_index, 0x1003);
                        if (unit != 0 && unit->unit.player_index != -1 && final_weight > best[2].weight) {
                            best[2].weight = final_weight;
                            best[2].object_index = object_index;
                        }
                    }
                }
                bool in_editor = game_uses_predicted_simulation();
                if (!in_editor && !cinematic_owner) {
                    if ((effect_definition->effect_flags & 0x10000) == 0) {
                        object_cause_damage(damage, object_index, -1, -1, -1, 0);
                        if (damage->damage_identifier != 0)
                            object->damage_identifier = damage->damage_identifier;
                    }
                    if (infected)
                        damage->flags |= 64;
                    if (damage_info != 0 && (damage_info->flags & 8) != 0) {
                        long child_index = object->first_child_object_index;
                        if (child_index != -1)
                            damage_handle_area_effect(damage, child_index, exclude_index, true, best);
                    }
                }
                else {
                    if (((1 << object->object_type) & 0x1003) != 0 && damage->effect != -1)
                        player_effect_start(object_index, -1, damage->effect, damage_owner_unknown(), &damage->direction, 1.0f, 1.0f, 0, false);
                }
            }
        }
    }
    if (is_child) {
        long sibling_index = object->next_object_index;
        if (sibling_index != -1)
            damage_handle_area_effect(damage, sibling_index, exclude_index, true, best);
    }
}

// @rdata 0x82021694
static const float damage_fatal_maximum_body_damage = FLT_MAX;

// @address 0x33d2a8 ?object_cause_fatal_damage_aftermath@@YAXJ@Z
void object_cause_fatal_damage_aftermath(long object_index)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    if ((unsigned char)((object->damage_flags >> 2) & 1))
        return;

    const s_falling_damage_definition *options = (s_falling_damage_definition *)global_game_globals->falling_damage.address;
    if (options->falling_damage.index == -1)
        return;

    long damage_effect_index = options->falling_damage.index;
    bool unit_was_dying = ((1 << object->object_type) & 0x1003) ? unit_is_dying(object_index) : false;
    s_damage_data damage_data;
    damage_data_new(&damage_data, damage_effect_index);
    damage_data.scale[0] = 1.0f;
    damage_data.flags |= 4;

    s_damage_response_data response;
    memset(&response, 0, sizeof(response));
    response.body_damage_scale = 1.0f;
    response.shield_damage_scale = 1.0f;
    response.node_index = -1;
    response.maximum_body_damage = damage_fatal_maximum_body_damage;
    response.flags |= 64;
    object_kill(object_index, &damage_data, damage_owner_unknown(),
        (s_damage_response_data *)&response, true, 0);

    s_damage_aftermath_result_data aftermath;
    object_compute_damage_aftermath(object_index, &damage_data, &response, &aftermath);
    if (unit_was_dying)
        aftermath.ping_type = 0;
    object_apply_damage_aftermath(object_index, &aftermath);
}



static __forceinline bool damage_game_primary_skull_test(long skull)
{
    unsigned long active_skulls = game_globals->options.game_mode == 1 ? game_globals->options.primary_skulls : 0;
    bool result = (unsigned char)((active_skulls >> skull) & 1);
    return result;
}

static __forceinline void creature_damage_aftermath_apply(long object_index, const s_damage_aftermath_result_data *result)
{
    if ((result->flags & 1) != 0)
    {
        s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
        c_motor_task *motor_task = (c_motor_task *)((char *)object + object->unit.motor_tasks.offset);
        if (motor_task && motor_task->valid() && !motor_task->program_default())
            motor_task_interrupt(object_index, 15, 0);
    }
}

// @address 0xf5af0 ?object_apply_damage_aftermath@@YAXJPBUs_damage_aftermath_result_data@@@Z
void object_apply_damage_aftermath(long object_index, const s_damage_aftermath_result_data *result)
{
    long damage_definition_index = result->damage_definition_index;
    s_damage_effect_definition *definition = (s_damage_effect_definition *)tag_get('jpt!', damage_definition_index);
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    unsigned long flags = result->flags;
    float body_damage = result->body_damage;

    if ((flags & 1) != 0)
        body_damage = MAX(body_damage, 1.0f);

    if (body_damage > k_damage_cone_epsilon)
    {
        object->body_damage = (object->body_damage + body_damage > 1.0f) ? 1.0f : object->body_damage + body_damage;
        object->current_body_damage = (object->current_body_damage + body_damage > 1.0f) ? 1.0f : object->current_body_damage + body_damage;
        object->body_damage_reset = 0;
    }

    if (game_uses_predicted_simulation())
    {
        damage_handle_acceleration(object_index, damage_definition_index, (short)result->node_index, &result->owner, result->scale,
            &result->direction, &result->epicenter, (flags & (1UL << 7)) != 0, (flags & (1UL << 8)) != 0,
            (flags & (1UL << 2)) != 0);
    }

    long damage_owner_object_index = result->damage_owner_object_index;
    if (damage_owner_object_index != -1)
    {
        s_damage_object_datum *damage_owner = (s_damage_object_datum *)object_try_and_get_and_verify_type(damage_owner_object_index, 0x1003);
        if (damage_owner)
        {
            if ((result->flags & 1) != 0)
            {
                s_damage_object_datum *owner_object = OBJECT_GET(damage_owner_object_index, s_damage_object_datum, ~0u);
                signed char weapon_slot = owner_object->unit.current_weapon_index;
                if (weapon_slot != -1)
                {
                    long weapon_index = owner_object->unit.weapon_object_indices[weapon_slot];
                    if (weapon_index != -1 && damage_reporting_info_get_type(result->reporting_info) == 25)
                        weapon_report_kill(weapon_index, object_index);
                }
            }
            if (damage_owner->unit.player_index != -1)
            {
                if (damage_game_primary_skull_test(1))
                {
                    unsigned long reporting_type = damage_reporting_info_get_type(result->reporting_info);
                    if (reporting_type == 3 || reporting_type == 25 || reporting_type == 26)
                    {
                        bool is_enemy = false;
                        int object_type = object_header_get(object_index)->object_type;
                        if (object_type == 0 || object_type == 1)
                        {
                            if (object->unit.actor_index != -1)
                            {
                                long team = object->unit.team;
                                if (team != -1 && game_team_is_enemy((e_game_team)team, _game_team_player))
                                    is_enemy = true;
                            }
                        }
                        if (is_enemy)
                            damage_owner->damage_recharge_flags = 0;
                    }
                }
            }
        }
    }

    float shield_damage = result->shield_damage;
    float shield_damage_value = result->shield_damage;
    if ((unsigned char)((object->damage_flags >> 13) & 1))
        shield_damage_value = 0.0f;
    else if ((result->flags & 2) != 0)
        shield_damage_value = MAX(shield_damage_value, 1.0f);

    if (shield_damage_value > k_damage_cone_epsilon)
    {
        object->shield_damage = (object->shield_damage + shield_damage_value > 1.0f) ? 1.0f : object->shield_damage + shield_damage_value;
        object->current_shield_damage =
            (object->current_shield_damage + shield_damage_value > 1.0f) ? 1.0f : object->current_shield_damage + shield_damage_value;
        object->shield_damage_reset = 0;
    }

    if (!game_uses_predicted_simulation() && (result->flags & (1UL << 11)) == 0)
    {
        float total_damage = result->body_damage + shield_damage;
        bool friendly_fire = ((definition->damage.flags >> 18) & 1) != 0;
        if (((1 << object->object_type) & 0x1003) != 0 && (result->flags & 1) != 0)
        {
            if (result->owner_player_index != -1)
                damage_handle_kill_achievements(object_index, result->owner_player_index, result->owner.team, result->reporting_info, friendly_fire);
            if (game_globals->options.game_mode == 1)
                campaign_metagame_handle_kill(object_index, result->owner_player_index, result->damage_owner_object_index,
                    result->owner.team, result->reporting_info, total_damage, friendly_fire);
            c_game_statborg *statborg = game_engine_get_statborg();
            if (statborg && statborg->m_stats_started)
                statborg->record_kill(object_index, result->owner_player_index, (e_game_team)result->owner.team, result->reporting_info,
                    total_damage, friendly_fire);
            long owner_player_index = result->owner_player_index;
            if (owner_player_index != -1 && datum_try_and_get(player_data, owner_player_index) != 0)
                player_training_notify_kill(owner_player_index, object_index, result->reporting_info);
        }
    }

    long object_type_mask = 1 << object->object_type;
    if ((object_type_mask & 0x1003) != 0)
        unit_damage_aftermath_apply(object_index, result);
    else if ((object_type_mask & (1 << 11)) != 0)
        creature_damage_aftermath_apply(object_index, result);

    if ((result->flags & (1UL << 9)) != 0)
        object_destroy_internal(object_index, result->owner_player_index, result->damage_owner_object_index, result->reporting_info, 0);

    if (((1 << object->object_type) & 0x1003) != 0)
    {
        if (result->shield_damage > 0.0f || result->body_damage > 0.0f)
            OBJECT_GET(object_index, s_damage_object_datum, ~0u)->unit.last_damage_time = game_time_globals->time;
    }

    simulation_action_damage_aftermath(object_index, result);
}

// @address 0x3387d0 ?area_of_effect_cause_damage_to_object_targets@@YAXPAUs_damage_data@@MJPAUs_damage_response_data@@@Z
void area_of_effect_cause_damage_to_object_targets(s_damage_data *damage_data, float scale, long object_index, s_damage_response_data *damage_response_data)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    unit_definition *object_definition = (unit_definition *)tag_get('obje', object->definition_index);
    const s_model_damage_info *damage_info = object_get_damage_info(object_index);
    s_damage_effect_definition *effect = (s_damage_effect_definition *)tag_get('jpt!', damage_data->effect);
    damage_definition *damage = &effect->damage;

    if ((unsigned char)((object->damage_flags >> 8) & 1)) {
        damage_data->scale[2] = 0.0f;
        scale = 0.0f;
    }

    if (damage_info && object_definition->model.index != -1)
    {
        const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', object_definition->model.index);
        long affected_target_count = 0;
        bool difficulty_scaled;
        real_vector3d offset;
        unsigned char target_indices[MAXIMUM_MODEL_TARGETS_PER_MODEL];
        object_marker marker;
        float target_weights[MAXIMUM_MODEL_TARGETS_PER_MODEL];
        for (long target_index = 0; target_index < model->targets.count; target_index++)
        {
            const s_model_target *target = &((const s_model_target *)model->targets.address)[target_index];
            s_damage_section *damage_section = 0;
            if (target->damage_section_index >= 0 && target->damage_section_index < damage_info->sections.count)
                damage_section = TAG_BLOCK_GET_ELEMENT(&damage_info->sections, target->damage_section_index, s_damage_section);
            if (damage_section && damage_section->vitality <= 0.0f)
                continue;

            if (object_get_markers_by_string_id(object_index, target->marker_name, &marker, 1) != 1)
                continue;

            vector_from_points3d(&marker.matrix.position, &damage_data->origin, &offset);
            float distance = normalize3d(&offset) - target->size;
            if (!(distance > 0.0f))
                distance = 0.0f;
            if (target->maximum_distance != 0.0f && target->maximum_distance < target->size + distance)
                continue;
            if (distance > effect->radius.upper)
                continue;

            float angle = arccosine(dot_product3d(&offset, &marker.matrix.forward));
            if (angle >= target->cone_angle)
                continue;

            float range = effect->radius.upper - effect->radius.lower;
            float falloff;
            if (range > 0.0f)
            {
                falloff = 1.0f - (distance - effect->radius.lower) / range;
                falloff = PIN(falloff, 0.0f, 1.0f);
            }
            else
                falloff = 1.0f;

            // match: 0x3387d0: value range assumption guides branch lowering
            __assume(affected_target_count < MAXIMUM_MODEL_TARGETS_PER_MODEL);
            target_indices[affected_target_count] = (unsigned char)target_index;
            target_weights[affected_target_count] = falloff;
            affected_target_count++;
        }

        if (affected_target_count > 0)
        {
            difficulty_scaled = false;
            float damage_amount = damage_compute_amount((s_damage_data *)damage_data, (const s_damage_effect_definition *)effect,
                (const damage_definition *)damage, object_index, &difficulty_scaled);
            if (damage_global_get_material(damage_data->material_type.m_index))
            {
                damage_amount = damage_armor_table_lookup(damage->general_damage, damage->specific_damage,
                                    damage_global_get_material(damage_data->material_type.m_index)->armor_type_a,
                                    damage_global_get_material(damage_data->material_type.m_index)->armor_type_b) *
                    damage_amount;
            }

            if (damage_amount > 0.0f)
            {
                float total_weight = 0.0f;
                float maximum_body_vitality = object_get_maximum_body_vitality(object_index, difficulty_scaled);
                float inverse_vitality = maximum_body_vitality > 0.0f ? 1.0f / maximum_body_vitality : 0.0f;
                float scaled_damage = inverse_vitality * damage_amount;
                long index;
                for (unsigned long weight_index = 0; (long)weight_index < affected_target_count; ++weight_index)
                    total_weight += target_weights[weight_index];

                for (index = 0; index < affected_target_count; index++)
                {
                    const s_model_target *target = &((const s_model_target *)model->targets.address)[target_indices[index]];
                    float target_damage = 0.0f;
                    if (total_weight > k_damage_cone_epsilon)
                        target_damage = target_weights[index] / total_weight * scaled_damage;
                    if (target->damage_section_index >= 0 && target->damage_section_index < damage_info->sections.count)
                    {
                        s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&damage_info->sections, target->damage_section_index, s_damage_section);
                        damage_update_section_damage(damage_info, object_index, &damage_data->owner_data, damage_data->reporting_info,
                            target->damage_section_index, target_damage / section->vitality, damage->custom_response_label, damage_response_data);
                    }
                }
            }
        }
    }

    long child_index = object->first_child_object_index;
    while (child_index != -1)
    {
        s_damage_object_datum *child = OBJECT_GET(child_index, s_damage_object_datum, ~0u);
        if (child->m_object_flags.test(_object_data_flag_26) && !(unsigned char)((child->damage_flags >> 8) & 1))
        {
            s_damage_response_data response;
            memset(&response, 0, sizeof(response));
            area_of_effect_cause_damage_to_object_targets(damage_data, scale, child_index, &response);

            s_damage_aftermath_result_data aftermath;
            object_compute_damage_aftermath(child_index, damage_data, &response, &aftermath);
            if (game_is_predicted())
                aftermath.flags &= ~1u;
            object_apply_damage_aftermath(child_index, &aftermath);
            if ((response.flags >> 2) & 1)
            {
                if (!game_is_predicted())
                    object_delete(child_index);
            }
        }
        child_index = child->next_object_index;
    }
}



// @rdata 0x82024078
static const float damage_section_maximum_damage = 100000.0f;

static __forceinline float damage_section_get_maximum_damage()
{
    return damage_section_maximum_damage;
}

// @address 0xf5930 ?object_damage_damage_section@@YAXJJM@Z
void object_damage_damage_section(long object_index, long section_name, float damage)
{
    if (object_index == -1)
        return;
    const s_model_damage_info *damage_info = object_get_damage_info(object_index);
    const s_damage_owner *owner = damage_owner_unknown();
    float maximum_damage = damage_section_get_maximum_damage();
    if (damage > maximum_damage)
        damage = maximum_damage;
    if (!damage_info || section_name == 0 || section_name == -1)
        return;
    long section_index = 0;
    if (damage_info->sections.count > 0) {
        const s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&damage_info->sections, 0, s_damage_section);
        do {
            if (section->name == section_name)
                break;
            section_index++;
            section++;
        } while (section_index < damage_info->sections.count);
    }
    if (section_index == damage_info->sections.count)
        return;
    s_damage_response_data response;
    memset(&response, 0, sizeof(response));
    damage_update_section_damage(damage_info, object_index, owner, 0, section_index, damage, 0, &response);
    if (response.first_constraint != 0 || response.second_constraint != 0)
        object_damage_constraints(object_index, response.first_constraint, response.second_constraint);
    if ((response.flags & 4) != 0)
        object_destroy_internal(object_index, -1, -1, 0, 0);
}


// Squared distance between two points.
static __forceinline float damage_area_distance_squared3d(const real_point3d *a, const real_point3d *b)
{
    float j = 0.0f, k = 0.0f, i = 0.0f;
    i = b->x - a->x;
    j = b->y - a->y;
    k = b->z - a->z;
    return i * i + j * j + k * k;
}
static __forceinline bool damage_area_point_in_sphere(const real_point3d *point, const real_point3d *center, float radius)
{
    return damage_area_distance_squared3d(point, center) <= radius * radius;
}
static __forceinline void *try_get_object_of_type(long object_index, unsigned long type_mask)
{
    s_data_array *data = object_header_data;
    void *result = 0;
    if (object_index != -1) {
        long absolute_index = object_index & 0xffff;
        if (absolute_index < data->first_unallocated) {
            s_object_header *header = (s_object_header *)(data->data + data->size * absolute_index);
            long identifier = header->identifier;
            if (identifier != 0 && identifier == (object_index >> 16) && ((1 << header->object_type) & type_mask) != 0)
                result = header->datum;
        }
    }
    return result;
}
static __forceinline long damage_player_index_from_unit_index(long unit_index)
{
    const s_damage_object_datum *unit = (const s_damage_object_datum *)try_get_object_of_type(unit_index, 0x1003);
    long player_index = -1;
    if (unit)
        player_index = unit->unit.player_index;
    return player_index;
}

// @address 0x336bc0 ?area_of_effect_cause_damage@@YAJPAUs_damage_data@@JJ@Z
long area_of_effect_cause_damage(s_damage_data *damage, long owner_index, long ignore_index)
{
    long low_priority_damaged = 0;
    long high_priority_count = 0;
    const s_damage_effect_definition *definition = (const s_damage_effect_definition *)tag_get('jpt!', damage->effect);
    const long maximum_object_count = 64;
    float radius = MAX(definition->radius.upper, MAX(definition->ai_stun_radius, definition->emp_radius));
    unsigned long type_mask = (definition->effect_flags & 2) ? 0x1003 : 0;
    if (damage->proximity)
        radius = damage->proximity->radius;
    long object_indices[maximum_object_count];
    s_damage_best_choice best[3];
    long object_count = objects_in_sphere(0, type_mask, &damage->location, &damage->origin, radius, object_indices, maximum_object_count);
    memset(best, 0, sizeof(best));
    damage->flags |= 0x10000;
    damage->flags |= 1;
    best[0].object_index = -1;
    best[1].object_index = -1;
    best[2].object_index = -1;
    if (definition->shake_radius > radius) {
        c_data_iterator<player_datum> iterator;
        long initial_count = object_count;
        iterator.begin(player_data);
        while (iterator.next()) {
            player_datum *player = iterator.get_datum();
            if (player->unit_index != -1) {
                long object_index = inl_object_get_ultimate_parent(player->unit_index);
                s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
                if (object_count < maximum_object_count) {
                    float bounding_radius = object->bounding_radius;
                    if (!damage_area_point_in_sphere(&damage->origin, &object->bounding_center, bounding_radius + radius)
                        && damage_area_point_in_sphere(&damage->origin, &object->bounding_center, definition->shake_radius + bounding_radius)) {
                        long i;
                        for (i = initial_count; i < object_count; i++) {
                            if (object_indices[i] == object_index)
                                break;
                        }
                        if (i == object_count)
                            object_indices[object_count++] = object_index;
                    }
                }
            }
        }
    }
    s_area_of_effect_object objects[maximum_object_count];
    for (long i = 0; i < object_count; i++) {
        long object_index = object_indices[i];
        real_point3d center;
        float bounding_radius;
        object_get_bounding_sphere(object_index, &center, &bounding_radius);
        objects[i].object_index = object_index;
        objects[i].distance_squared = damage_area_distance_squared3d(&center, &damage->origin);
        const s_object_header *header = object_header_get(object_index);
        const s_damage_object_datum *object = (const s_damage_object_datum *)header->datum;
        bool low_priority = (unsigned char)((object->damage_flags >> 2) & 1)
            | (unsigned char)(((unsigned long)(1 << header->object_type) >> 2) & 1)
            | (unsigned char)((object->recycling_flags >> 1) & 1);
        if (((1 << header->object_type) & 8) != 0) {
            low_priority = (unsigned char)(low_priority | (equipment_active_fraction(object_index) == 0.0f));
        }
        objects[i].low_priority = low_priority;
        if (!low_priority)
            high_priority_count++;
    }
    long low_priority_maximum = PIN(20 - high_priority_count, 5, 20);
    qsort(objects, object_count, sizeof(s_area_of_effect_object), (int (__cdecl *)(const void *, const void *))damage_compare_float);
    for (long i = 0; i < object_count; i++) {
        long object_index = objects[i].object_index;
        bool low_priority = objects[i].low_priority;
        if ((definition->effect_flags & 2) != 0 && damage_player_index_from_unit_index(object_index) == -1)
            continue;
        if (object_index == ignore_index)
            continue;
        if (low_priority && low_priority_damaged >= low_priority_maximum)
            continue;
        const s_object_header *header = object_header_get(object_index);
        s_damage_data object_damage = *damage;
        damage_handle_area_effect(&object_damage, object_index, owner_index, false, best);
        if (low_priority)
            low_priority_damaged++;
        long child_index = header->datum->first_child_object_index;
        while (child_index != -1) {
            s_damage_object_datum *child = OBJECT_GET(child_index, s_damage_object_datum, ~0u);
            long next_child_index = child->next_object_index;
            if ((unsigned char)((child->physics_flags >> 4) & 1)) {
                real_point3d center;
                float bounding_radius;
                object_get_bounding_sphere(child_index, &center, &bounding_radius);
                if (damage_area_point_in_sphere(&damage->origin, &center, bounding_radius + radius)) {
                    s_damage_data child_damage = *damage;
                    damage_handle_area_effect(&child_damage, child_index, owner_index, false, best);
                }
            }
            if (((1 << child->object_type) & 0x1003) != 0
                && (unsigned char)((((const unit_definition *)tag_get('unit', child->definition_index))->flags >> 19) & 1)) {
                real_point3d center;
                float bounding_radius;
                object_get_bounding_sphere(child_index, &center, &bounding_radius);
                if (point_in_sphere(&damage->origin, &center, bounding_radius + radius)) {
                    s_damage_data child_damage = *damage;
                    damage_handle_area_effect(&child_damage, child_index, owner_index, false, best);
                }
            }
            child_index = next_child_index;
        }
    }
    if (!game_uses_predicted_simulation() && *(unsigned char *)g_breakable_surface_globals) {
        const s_damage_effect_definition *breakable_definition = (const s_damage_effect_definition *)tag_get('jpt!', damage->effect);
        if (breakable_definition->damage.damage_upper_bound.lower != 0.0f || breakable_definition->damage.damage_upper_bound.upper != 0.0f)
            breakable_surface_damage_area_of_effect(&damage->location, &damage->position, breakable_definition->radius.upper);
    }
    long result = best[2].object_index;
    if (result == -1) {
        result = best[1].object_index;
        if (result == -1)
            result = best[0].object_index;
    }
    return result;
}

struct effect_vector_field;

// @address 0x3367c0 ?object_deplete_shield_internal@@YAXJPBUs_damage_owner@@PAUs_damage_response_data@@@Z
void object_deplete_shield_internal(long object_index, const s_damage_owner *damage_owner, s_damage_response_data *response)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    if (!((unsigned char)((object->damage_flags >> 3) & 1))) {
        const s_model_damage_info *info = object_get_damage_info(object_index);
        float zero = 0.0f;
        object->shield_vitality = zero;
        object->current_shield_damage = zero;
        object->m_damage_flags.set(_damage_shield_depleted, true);
        response->flags |= 8;
        if (info) {
            if (object->maximum_shield_vitality > zero && !response->silent)
                effect_new_from_object(info->shield_depletion_effect.index, damage_owner_unknown(), 0, object_index, zero, zero, 0, 0, 1);
            if (info->flags & (1 << 9))
                object->m_damage_flags.set(_damage_shield_destroyed, true);
            if (!game_uses_predicted_simulation()) {
                for (long section_index = 0; section_index < info->sections.count; section_index++) {
                    if (TAG_BLOCK_GET_ELEMENT(&info->sections, section_index, s_damage_section)->flags & (1 << 9))
                        damage_section_deplete(info, object_index, damage_owner, 0, section_index, response);
                }
            }
        }
        object_set_model_state_property_per_region(object_index, -1, 2, -1, false);
    }
}





static __forceinline float damage_object_get_actual_body_vitality(long object_index)
{
    float body_vitality = OBJECT_GET(object_index, s_damage_object_datum, ~0u)->body_vitality;
    return object_get_maximum_body_vitality(object_index, false) * body_vitality;
}

static __forceinline bool damage_player_is_immune_to_headshot(long player_index)
{
    bool immune = false;
    if (game_globals->options.game_mode == 1)
        immune = true;
    else if (game_globals->options.game_mode == 2)
        immune = datum_get<player_datum>(player_data, player_index)->traits.m_shield_vitality_traits.m_headshot_immunity.m_value == 1;
    return immune;
}

static __forceinline float apply_body_damage_to_damage_section(const s_model_damage_info *info,
    const s_damage_model_material *material, long object_index, const s_damage_owner *owner, unsigned char reporting_info,
    float damage, float inverse_vitality, long definition, s_damage_response_data *damage_response_data)
{
    float scaled_damage = inverse_vitality * damage;
    long section_index = info->indirect_damage_section;
    if (material)
        section_index = material->damage_section;
    if (section_index >= 0 && section_index < info->sections.count)
    {
        s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&info->sections, section_index, s_damage_section);
        if (section->vitality > 0.0f)
            damage_update_section_damage(info, object_index, owner, reporting_info, section_index, scaled_damage / section->vitality,
                definition, damage_response_data);
        if (((section->flags) & (1 << 0)))
            damage = 0.0f;
    }
    damage_response_data->scaled_body_damage = scaled_damage;
    return damage;
}

// @address 0x339050 ?object_damage_body@@YAXJPBUdamage_definition@@PAUs_damage_data@@PAUs_damage_response_data@@@Z
void object_damage_body(long object_index, const damage_definition *damage, s_damage_data *damage_data, s_damage_response_data *damage_response_data)
{
    const s_model_damage_info *info = (const s_model_damage_info *)damage_response_data->definition;
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    float body_damage = compute_adjusted_body_damage(object_index, damage, damage_data, damage_response_data);

    if (body_damage > 0.0f)
    {
        float maximum_body_vitality = object_get_maximum_body_vitality(object_index, damage->category == 1);
        float inverse_vitality = maximum_body_vitality > 0.0f ? 1.0f / maximum_body_vitality : 0.0f;

        const s_damage_model_material *material = damage_response_data->section_ref;
        bool headshot = false;
        if (info && material)
        {
            short section_index = material->damage_section;
            if (section_index >= 0 && section_index < info->sections.count &&
                ((TAG_BLOCK_GET_ELEMENT(&info->sections, section_index, s_damage_section)->flags) & (1 << 7)))
                headshot = true;
        }
        if (headshot && ((damage->flags) & (1 << 1)))
        {
            bool immune = false;
            if (object->object_type == 0 && object->unit.player_index != -1)
                immune = damage_player_is_immune_to_headshot(object->unit.player_index);
            if (!immune)
            {
                damage_data->reporting_info = (damage_data->reporting_info & ~3) | 1;
                object->body_vitality = 0.0f;
                damage_response_data->response_metadata = 1;
                damage_response_data->flags |= 0xc0;
            }
        }

        body_damage = apply_body_damage_to_damage_section(info, damage_response_data->section_ref, object_index, &damage_data->owner_data,
            damage_data->reporting_info, body_damage, inverse_vitality, damage->custom_response_label, damage_response_data);
        float body_damage_scale = body_damage * inverse_vitality;
        object->body_vitality -= body_damage_scale;
        if (((info->flags) & (1 << 7)) || ((object->damage_flags_words[BIG_ENDIAN_VIEW_INDEX(0, 2)]) & (1 << 0)))
        {
            if (!(object->body_vitality >= 0.0001f))
                object->body_vitality = 0.0001f;
        }
        simulation_action_object_update(object_index, 64);
        damage_response_data->shield_damage_scale = body_damage_scale;
    }

    if (!((damage_data->flags) & (1 << 15)) &&
        !(damage_object_get_actual_body_vitality(object_index) > 0.0f))
    {
        if (!((damage_data->flags) & (1 << 8)) && vehicle_has_passengers(object_index, false, false, -1, NULL, NULL))
            object->damage_flags |= 0x4000;
        else
            object_kill(object_index, damage_data, &damage_data->owner_data, (s_damage_response_data *)damage_response_data, true,
                damage_data->reporting_info);
    }

    info = (const s_model_damage_info *)damage_response_data->definition;
    if (!(info->minimum_stun_damage >= body_damage))
    {
        float stun_time = info->stun_time;
        if (((1 << object->object_type) & 0x1003) != 0 && object->unit.actor_index != -1)
        {
            character_vitality_properties *properties = character_definition_vitality_properties_get(
                datum_get<actor_datum>(actor_data, object->unit.actor_index)->character_index);
            if (properties)
                stun_time = properties->stun_time;
        }
        object->body_stun_ticks = (short)damage_seconds_to_ticks_round(stun_time, 0.5f);
    }

    if (((damage_data->flags) & (1 << 0)) && object->object_type == 0 && !damage_response_data->silent)
    {
        const biped_definition *definition = (const biped_definition *)tag_get('bipd', object->definition_index);
        if (definition->death_effect.index != -1 &&
            (!(unsigned char)((object->damage_flags >> 2) & 1) && body_damage > 10.0f ||
             ((damage_response_data->flags) & (1 << 0)) || ((damage_response_data->flags) & (1 << 6))))
        {
            effect_new_from_object(definition->death_effect.index, &damage_data->owner_data, 0, object_index, 0.0f, 0.0f, 0, 0, 1);
        }
    }

    damage_response_data->body_damage -= body_damage;
}

// @address 0x33a4e0 ?damage_spawn_effect_from_marker_name@@YAXJJPBUs_damage_owner@@J_N1@Z
void damage_spawn_effect_from_marker_name(long effect_index, long object_index, const s_damage_owner *damage_owner, long marker_name, bool detached, bool flags)
{
    c_tag_index tag_storage;
    if (!detached) {
        c_tag_index *tag = new (&tag_storage) c_tag_index(effect_index);
        effect_new_attached_from_marker_name(*tag, damage_owner, 0, object_index, -1, marker_name, 1.0f, 1.0f, 0, 0, flags != 0, 1);
    } else {
        object_datum *object = (object_datum *)OBJECT_GET(object_index, s_damage_object_datum, ~0u);
        if (marker_name == 0)
            marker_name = _string_id_root;
        object_marker marker;
        internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false);
        real_vector3d velocity;
        object_get_velocities(object_index, &velocity, 0);
        s_effect_vector vectors[3];
        build_gravity_effect_vectors(&marker.matrix.position, vectors);
        vectors[2].marker_name = marker_name;
        vectors[2].point = marker.matrix.position;
        vectors[2].vector = marker.matrix.forward;
        c_tag_index *tag = new (&tag_storage) c_tag_index(effect_index);
        effect_create_damage_effect(*tag, damage_owner, marker_name, &velocity, 3, vectors, 0, &object->location, 1.0f, 1.0f, 0, 0, flags != 0, 1);
    }
}



static __forceinline void damage_transfer_iterator_new(long unit_index, unit_passenger_iterator *iterator)
{
    s_damage_object_datum *unit = OBJECT_GET(unit_index, s_damage_object_datum, 0x1003);
    iterator->unit_index = unit_index;
    iterator->passenger_index = -1;
    iterator->seat_index = -1;
    iterator->next_object_index = unit->first_child_object_index;
}

static __forceinline const s_model_damage_info *damage_transfer_get_damage_info(long object_index)
{
    const s_model_damage_info *result = 0;
    if (object_index != -1) {
        const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
        const unit_definition *definition = (const unit_definition *)tag_get('obje', object->definition_index);
        unsigned long model_index = definition->model.index;
        if (model_index != 0xffffffff) {
            const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', model_index);
            if (model->damage_info.count > 0)
                result = (const s_model_damage_info *)model->damage_info.address;
        }
    }
    return result;
}

static __forceinline const unit_definition *damage_transfer_get_definition(c_tag_index index)
{
    return (const unit_definition *)tag_get('vehi', index);
}

// @address 0x33c260 ?handle_vehicle_damage_transfer@@YAXPAUs_damage_data@@J@Z
void handle_vehicle_damage_transfer(s_damage_data *damage_data, long vehicle_index)
{
    const cache_file_tag_instance *instances = global_tag_instances;
    const s_damage_effect_definition *effect =
        (const s_damage_effect_definition *)instances[datum_index_to_absolute_index(damage_data->effect)].definition;
    s_damage_object_datum *vehicle = OBJECT_GET(vehicle_index, s_damage_object_datum, 2);
    const unit_definition *definition =
        (const unit_definition *)instances[datum_index_to_absolute_index(vehicle->definition_index)].definition;
    const s_model_damage_info *info = object_get_damage_info(vehicle_index);
    s_damage_data transferred = *damage_data;
    unit_passenger_iterator iterator;
    damage_transfer_iterator_new(vehicle_index, &iterator);
    transferred.flags |= 0x200;
    transferred.reporting_info = damage_data->reporting_info & ~3;
    while (unit_passenger_iterator_next(&iterator)) {
        long passenger_index = iterator.passenger_index;
        s_damage_object_datum *passenger = OBJECT_GET(passenger_index, s_damage_object_datum, 0x1003);
        float distance_squared = distance_squared3d(&damage_data->position, &passenger->bounding_center);
        if ((unsigned short)passenger->unit.parent_seat_index != 0xffff && passenger_index != damage_data->object_index
            && (!(effect->damage.flags & 1) || passenger_index != damage_data->owner_data.object_index)
            && (!(effect->damage.flags & 8) || game_team_is_enemy((e_game_team)passenger->unit.team, (e_game_team)damage_data->owner_data.team))) {
            const unit_seat *seat = &((unit_seat *)definition->seats.address)[passenger->unit.parent_seat_index];
            for (long seat_index = 0; seat_index < info->seats.count; seat_index++) {
                s_damage_seat_info *damage_seat = TAG_BLOCK_GET_ELEMENT(&info->seats, seat_index, s_damage_seat_info);
                if (damage_seat->flags == seat->label) {
                    s_damage_seat_region_state_setting settings;
                    damage_seat_get_transfer_info(vehicle_index, damage_seat, &settings);
                    if (damage_data->proximity) {
                        real_point3d nearest_point;
                        damage_proximity_find_nearest(damage_data->proximity, &passenger->bounding_center, &nearest_point, &transferred.cone_amount);
                    } else {
                        float multiplier;
                        if (!(effect->damage.flags & (1 << 11)) || (info->flags & (1 << 14))) {
                            float close_scale = effect->damage.rider_maximum_transfer_damage_scale;
                            if (info->flags & (1 << 14))
                                close_scale = 1.0f;
                            if (!(damage_seat->radius * damage_seat->radius <= distance_squared)) {
                                multiplier = settings.b * close_scale;
                            } else {
                                multiplier = settings.c * effect->damage.rider_minimum_transfer_damage_scale;
                            }
                        } else {
                            multiplier = settings.c * effect->damage.rider_minimum_transfer_damage_scale;
                        }
                        transferred.cone_amount *= multiplier;
                    }
                    object_cause_damage(&transferred, passenger_index, -1, -1, -1, 0);
                    break;
                }
            }
        }
    }
    long child_index = vehicle->first_child_object_index;
    while (child_index != -1) {
        s_damage_object_datum *child = OBJECT_GET(child_index, s_damage_object_datum, ~0u);
        if (((1 << child->object_type) & 2)
            && !((unsigned char)((damage_transfer_get_definition(child->definition_index)->flags >> 19) & 1))
            && !(damage_data->flags & (1 << 14)) && damage_transfer_get_damage_info(child_index))
            handle_vehicle_damage_transfer(&transferred, child_index);
        child_index = child->next_object_index;
    }
}


// match: the simulation caller has a different C-linkage signature.


static __forceinline unsigned long damage_response_simulation_get()
{
    return (unsigned char)game_globals->options.simulation_type;
}

static __forceinline bool damage_response_is_predicted()
{
    bool result = false;
    unsigned long simulation_type = damage_response_simulation_get();
    if (simulation_type == 4)
        result = true;
    return result;
}

static __forceinline void damage_response_set_flag(unsigned long *flags, long bit, bool enable)
{
    if (enable)
        *flags |= FLAG(bit);
    else
        *flags &= ~FLAG(bit);
}

static __forceinline long damage_response_get_weapon(long object_index)
{
    s_damage_object_datum *unit = OBJECT_GET(object_index, s_damage_object_datum, 0x1003);
    short weapon_slot = unit->unit.current_weapon_index;
    return weapon_slot == NONE ? NONE : unit->unit.weapon_object_indices[weapon_slot];
}

static __forceinline char *damage_response_header_block_get(long object_index, const s_object_block_reference *reference)
{
    object_datum *object = (object_datum *)OBJECT_GET(object_index, s_damage_object_datum, ~0UL);
    return (char *)object + reference->offset;
}

static __forceinline char *damage_response_header_block_get_with_count(
    long object_index, const s_object_block_reference *reference, unsigned long element_size, long *count)
{
    *count = reference->size / element_size;
    return damage_response_header_block_get(object_index, reference);
}

static __forceinline bool damage_response_get_active_instances(long object_index, unsigned long **instances)
{
    object_datum *object = (object_datum *)OBJECT_GET(object_index, s_damage_object_datum, ~0UL);
    const unsigned long has_instances = (unsigned char)((object->flags >> 11) & 1);
    if (has_instances) {
        long count;
        char *block = damage_response_header_block_get_with_count(object_index, &object->regions, 1, &count);
        *instances = (unsigned long *)(block + (count - 16));
        return true;
    }
    return false;
}

static __forceinline void damage_response_destroy_instance_group(long object_index, long group_index)
{
    unsigned long *active_instances;
    if (damage_response_get_active_instances(object_index, &active_instances)) {
        const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0UL);
        const unit_definition *definition = (const unit_definition *)tag_get('obje', object->definition_index);
        unsigned long instances[2];
        memset(instances, 0, sizeof(instances));
        model_instance_group_calculate_all_possible_instances(definition->model.index, group_index, instances);
        for (long i = 0; i < 2; ++i)
            instances[i] = ~instances[i];
        for (long i = 0; i < 2; ++i)
            active_instances[i] &= instances[i];
        object_compute_instance_bitfield_visible(object_index);
    }
}

// @address 0x33a980 damage_handle_section_response
extern "C" void damage_handle_section_response(const s_model_damage_info *model_damage_info, long object_index, const s_damage_owner *owner,
    unsigned char reporting_info, long section_index, long response_index, s_damage_response_data *response)
{
    s_damage_response_data child_response;
    object_datum *object = (object_datum *)OBJECT_GET(object_index, s_damage_object_datum, ~0UL);
    const s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&model_damage_info->sections, section_index, s_damage_section);
    const s_instantaneous_damage_response *instantaneous_response = TAG_BLOCK_GET_ELEMENT(&section->responses, response_index, s_instantaneous_damage_response);
    if (!damage_response_is_predicted() && object->gamestate_index != NONE) {
        simulation_action_damage_section_response(object_index, section_index, response_index, _damage_section_response_instantaneous);
        simulation_action_object_update(object_index, 256);
        simulation_action_object_update(object_index, 512);
    }
    for (long index = 0; index < 2; ++index) {
        const damage_region_state *state = index ? &instantaneous_response->region_states[1] : &instantaneous_response->region_states[0];
        if (state->region != NONE && state->state != NONE) {
            if (!response->silent)
                object_set_at_rest(object_index, false);
            object_set_model_state(object_index, state->region, state->state, false);
            if ((FLAG(object->object_type) & 0x1003) && state->state >= 3) {
                const unit_definition *definition = (const unit_definition *)tag_get('unit', object->definition_index);
                unit_passenger_iterator iterator;
                damage_transfer_iterator_new(object_index, &iterator);
                while (unit_passenger_iterator_next(&iterator)) {
                    if (iterator.seat_index != NONE) {
                        const unit_seat *seat = &((unit_seat *)definition->seats.address)[iterator.seat_index];
                        if ((bool)((seat->flags >> 19) & 1) && seat->region_index == state->region)
                            object_set_hidden(iterator.passenger_index, false);
                    }
                }
            }
        }
    }
    if (instantaneous_response->destroy_instance_group != NONE)
        damage_response_destroy_instance_group(object_index, instantaneous_response->destroy_instance_group);
    if (instantaneous_response->effect.index != NONE) {
        const bool effect_flag = (bool)(((unsigned long)instantaneous_response->flags >> 27) & 1);
        const bool detached = (bool)(((unsigned long)instantaneous_response->flags >> 20) & 1);
        damage_spawn_effect_from_marker_name(
            instantaneous_response->effect.index, object_index, owner, instantaneous_response->effect_marker, detached, effect_flag);
    }
    if (instantaneous_response->secondary_effect.index != NONE)
        damage_spawn_effect_from_marker_name(instantaneous_response->secondary_effect.index, object_index, owner,
            instantaneous_response->effect_marker, (bool)(((unsigned long)instantaneous_response->flags >> 20) & 1), false);
    if (!damage_response_is_predicted() && instantaneous_response->damage_effect.index != NONE)
        damage_spawn_damage_effect_from_marker_name(instantaneous_response->damage_effect.index, object_index, owner,
            instantaneous_response->damage_effect_marker, (unsigned char)(model_damage_info->reporting_type << 2));
    if (TEST_BIT(instantaneous_response->flags, 1)) object->inhibited_flags |= 1;
    if (TEST_BIT(instantaneous_response->flags, 2)) object->inhibited_flags |= 2;
    if (TEST_BIT(instantaneous_response->flags, 3)) object->inhibited_flags |= 4;
    if (TEST_BIT(instantaneous_response->flags, 4)) object->inhibited_flags |= 8;
    if (TEST_BIT(instantaneous_response->flags, 22)) response->flags |= FLAG(9);
    if (TEST_BIT(instantaneous_response->flags, 5) || TEST_BIT(instantaneous_response->flags, 6)) {
        long barrel = !TEST_BIT(instantaneous_response->flags, 5);
        const unsigned long object_type = (unsigned char)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type;
        if (FLAG(object_type) & 0x1003) {
            long weapon_index = damage_response_get_weapon(object_index);
            if (weapon_index != NONE)
                weapon_destroy_barrel(weapon_index, barrel);
        } else if (TEST_BIT(FLAG(object_type), 2)) {
            weapon_destroy_barrel(object_index, barrel);
        }
    }
    if (TEST_BIT(instantaneous_response->flags, 8) || TEST_BIT(instantaneous_response->flags, 9)) {
        long barrel = !TEST_BIT(instantaneous_response->flags, 8);
        if (FLAG(((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type) & 0x1003) {
            long weapon_index = damage_response_get_weapon(object_index);
            if (weapon_index != NONE)
                weapon_damage_barrel(weapon_index, barrel);
        } else if (TEST_BIT(FLAG(((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type), 2)) {
            weapon_damage_barrel(object_index, barrel);
        }
    }
    if (TEST_BIT(instantaneous_response->flags, 17) || (TEST_BIT(instantaneous_response->flags, 26) && game_globals->options.game_mode == 1)) {
        if (FLAG(((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type) & 12)
            item_detonate(object_index);
        else if (TEST_BIT(FLAG(((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type), 5))
            projectile_detonate(object_index, NONE, false, 0.0f);
    }
    if (TEST_BIT(instantaneous_response->flags, 0)) {
        if (!damage_response_is_predicted() && (!TEST_BIT(section->flags, 2) || !vehicle_has_passengers(object_index, false, false, NONE, 0, 0)))
            object_kill(object_index, 0, owner, (s_damage_response_data *)response, true, reporting_info);
    } else if (TEST_BIT(instantaneous_response->flags, 16)) {
        if (!game_uses_predicted_simulation() && (game_globals->options.game_mode != 1 || !vehicle_has_passengers(object_index, true, false, NONE, 0, 0)))
            object_kill(object_index, 0, owner, (s_damage_response_data *)response, true, reporting_info);
    }
    if (TEST_BIT(instantaneous_response->flags, 7)) {
        if (damage_response_is_predicted())
            object_set_hidden(object_index, true);
        else
            response->flags |= FLAG(2);
    }
    if (TEST_BIT(instantaneous_response->flags, 19)) {
        for (long child_index = object->first_child_object_index; child_index != NONE;) {
            object_datum *child = (object_datum *)OBJECT_GET(child_index, s_damage_object_datum, ~0UL);
            if ((bool)((child->flags >> 26) & 1)) {
                memset(&child_response, 0, sizeof(child_response));
                object_kill(child_index, 0, owner, (s_damage_response_data *)&child_response, true, reporting_info);
            }
            child_index = child->next_object_index;
        }
    }
    if (instantaneous_response->constraint_name != 0 && instantaneous_response->constraint_name != NONE) {
        const s_tag_block &constraints = model_damage_info->constraints;
        long constraint_index = NONE;
        for (long i = 0; i < constraints.count; ++i) {
            const s_damage_model_constraint *constraint = TAG_BLOCK_GET_ELEMENT(&constraints, i, s_damage_model_constraint);
            if (constraint->group_name == instantaneous_response->constraint_name) {
                constraint_index = i;
                break;
            }
        }
        if (constraint_index != NONE) {
            switch (instantaneous_response->constraint_damage_type) {
            case 0: break;
            case 1: case 2: response->first_constraint |= FLAG(constraint_index); break;
            case 3: case 4: response->second_constraint |= FLAG(constraint_index); break;
            // match: 0x33a980: unreachable hint omits dead branch check
            default: __assume(0);
            }
        } else {
            switch (instantaneous_response->constraint_damage_type) {
            case 0: break;
            case 2:
            damage_section_damage_all_constraints(model_damage_info, (const s_instantaneous_damage_response *)instantaneous_response,
                object->broken_constraints, &response->first_constraint);
            break;
            case 4:
            damage_section_damage_all_constraints(model_damage_info, (const s_instantaneous_damage_response *)instantaneous_response,
                object->loosened_constraints, &response->second_constraint);
            break;
            case 1:
            damage_section_damage_one_constraint(model_damage_info, (const s_instantaneous_damage_response *)instantaneous_response,
                object->broken_constraints, &response->first_constraint);
            break;
            case 3:
            damage_section_damage_one_constraint(model_damage_info, (const s_instantaneous_damage_response *)instantaneous_response,
                object->loosened_constraints, &response->second_constraint);
            break;
            // match: 0x33a980: unreachable hint omits dead branch check
            default: __assume(0);
            }
        }
    }
    if (instantaneous_response->eject_seat != 0 && instantaneous_response->eject_seat != NONE && (FLAG(object->object_type) & 0x1003)) {
        const unit_definition *definition = (const unit_definition *)tag_get('unit', object->definition_index);
        long seat_index;
        for (seat_index = 0; seat_index < definition->seats.s_tag_block::count; ++seat_index) {
            const unit_seat *seat = &((unit_seat *)definition->seats.address)[seat_index];
            if (seat->label == instantaneous_response->eject_seat)
                break;
        }
        if (seat_index != definition->seats.s_tag_block::count) {
            unit_passenger_iterator iterator;
            damage_transfer_iterator_new(object_index, &iterator);
            while (unit_passenger_iterator_next(&iterator)) {
                if (iterator.seat_index == seat_index) {
                    action_request request;
                    memset(&request, 0, sizeof(request));
                    memset(&request, 0, sizeof(request));
                    request.action = 20;
                    action_submit(iterator.passenger_index, &request);
                }
            }
        }
    }
    if (instantaneous_response->destroyed_child_object_marker_name != NONE && instantaneous_response->destroyed_child_object_marker_name != 0)
        object_placement_deactivate_on_zone_set_switch(object_index, instantaneous_response->destroyed_child_object_marker_name);
    if (TEST_BIT(instantaneous_response->flags, 25)
        && TEST_BIT(FLAG(((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type), 12))
        giant_update_variant_scale(object_index, true);
    if (TEST_BIT(instantaneous_response->flags, 28)
        && TEST_BIT(FLAG(((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type), 12)) {
        s_giant_datum *giant = (s_giant_datum *)OBJECT_GET(object_index, s_damage_object_datum, FLAG(12));
        damage_response_set_flag(&giant->giant_flags, 0, true);
        damage_response_set_flag(&giant->giant_flags, 2, false);
        damage_response_set_flag(&giant->giant_flags, 3, true);
    }
}

// @address 0x33b630 damage_update_section_damage
extern "C" void damage_update_section_damage(const s_model_damage_info *info, long object_index, const s_damage_owner *owner,
    unsigned char reporting_info, long section_index, float damage, long definition, s_damage_response_data *damage_response_data)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0UL);
    const s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&info->sections, section_index, s_damage_section);
    bool section_depleted = false;
    long section_count = (unsigned long)(short)object->damage_sections.size / sizeof(s_damage_object_section_state);
    s_damage_object_section_state *states = (s_damage_object_section_state *)((char *)object + object->damage_sections.offset);
    if (VALID_INDEX(section_index, section_count)) {
        s_damage_object_section_state *state = &states[section_index];
        const float minimum_interval = 1.0f / 255.0f;
        float previous_threshold = 1.0f;
        float next_threshold = 0.0f;
        long index;
        for (index = 0; index < section->responses.count; ++index) {
            const s_instantaneous_damage_response *instantaneous_response = TAG_BLOCK_GET_ELEMENT(
                    &section->responses, index, s_instantaneous_damage_response);
            if (!(state->flags & FLAG(index))) {
                next_threshold = instantaneous_response->threshold;
                break;
            } else {
                previous_threshold = instantaneous_response->threshold;
            }
        }
        section_depleted = next_threshold == 0.0f;
        float interval = MAX(previous_threshold - next_threshold, minimum_interval);
        float scaled_damage = damage / interval;
        if (scaled_damage > 0.0f) {
            long new_damage = (long)((float)state->damage + 255.0f * scaled_damage);
            state->damage = (unsigned char)PIN(new_damage, 0, 255);
            state->recharge_delay_ticks = (short)damage_seconds_to_ticks_round(section->damage_stun_time, 0.5f);
        }
        if (state->damage == 255 && TEST_BIT(section->flags, 2) && vehicle_has_passengers(object_index, false, false, NONE, 0, 0))
            state->damage = 254;
        if (state->damage == 255) {
            bool found_threshold = false;
            float response_threshold = 0.0f;
            unsigned long responses_to_fire = 0;
            state->damage = 0;
            for (index = 0; index < section->responses.count; ++index) {
                long response_bit = FLAG(index);
                if (state->flags & response_bit)
                    continue;
                const s_instantaneous_damage_response *instantaneous_response = TAG_BLOCK_GET_ELEMENT(
                    &section->responses, index, s_instantaneous_damage_response);
                bool fire_response = false;
                bool skip_response = false;
                if (!found_threshold) {
                    found_threshold = true;
                    response_threshold = instantaneous_response->threshold;
                    fire_response = true;
                } else if (instantaneous_response->threshold == response_threshold) {
                    fire_response = true;
                } else {
                    break;
                }
                if (section_depleted && TEST_BIT(section->flags, 2) && vehicle_has_passengers(object_index, false, false, NONE, 0, 0))
                    fire_response = false;
                if (!damage_section_response_is_valid(info, object_index, owner, section_index, index)) {
                    fire_response = false;
                    skip_response = true;
                }
                if (damage > instantaneous_response->vitality) {
                    if (TEST_BIT(instantaneous_response->flags, 21)) {
                        fire_response = false;
                        skip_response = true;
                    }
                } else if (!TEST_BIT(instantaneous_response->flags, 21)) {
                    fire_response = false;
                    skip_response = true;
                }
                if (TEST_BIT(damage_response_data->flags, 9)) {
                    if (TEST_BIT(instantaneous_response->flags, 24)) {
                        fire_response = false;
                        skip_response = true;
                    }
                } else if (TEST_BIT(instantaneous_response->flags, 23)) {
                    fire_response = false;
                    skip_response = true;
                }
                switch (instantaneous_response->condition) {
                case 1:
                    if (definition != instantaneous_response->condition_argument) {
                        fire_response = false;
                        skip_response = true;
                    }
                    break;
                case 2:
                    if (definition == instantaneous_response->condition_argument) {
                        fire_response = false;
                        skip_response = true;
                    }
                    break;
                case 3:
                    if (definition != instantaneous_response->condition_argument && definition != 0) {
                        fire_response = false;
                        skip_response = true;
                    }
                    break;
                }
                if (instantaneous_response->skip_fraction != 0.0f) {
                    float random_fraction = (float)(random_seed_next(TLS_RANDOM_SEED) >> 16) * 1.5259022e-5f;
                    if (random_fraction < instantaneous_response->skip_fraction) {
                        fire_response = false;
                        skip_response = true;
                    }
                }
                if (fire_response) {
                    if (instantaneous_response->delay > 0.0f) {
                        if (state->delayed_response & 0xfff) {
                            long delayed_index = state->delayed_response >> 12;
                            if (delayed_index != index && !(state->flags & FLAG(delayed_index)))
                                damage_handle_section_response(
                                    info, object_index, owner, state->response_index, section_index, delayed_index, damage_response_data);
                        }
                        state->response_index = reporting_info;
                        state->delayed_response = (state->delayed_response & 0xfff) | (index << 12);
                        object_wake(object_index);
                        if (!(state->delayed_response & 0xfff)) {
                            long ticks = damage_seconds_to_ticks_round(instantaneous_response->delay, 0.5f);
                            state->delayed_response = (ticks & 0xfff) | (state->delayed_response & 0xf000);
                            if (instantaneous_response->delay_effect.index != NONE) {
                                damage_spawn_effect_from_marker_name(instantaneous_response->delay_effect.index, object_index, owner,
                                    instantaneous_response->delay_effect_marker, TEST_BIT(instantaneous_response->flags, 20), false);
                                simulation_action_damage_section_response(object_index, section_index, index, _damage_section_response_delayed);
                            }
                        }
                    } else {
                        state->flags |= response_bit;
                        responses_to_fire |= response_bit;
                    }
                } else if (skip_response) {
                    state->flags |= response_bit;
                    if ((state->delayed_response & 0xfff) && (state->delayed_response >> 12) == index &&
                        !TEST_BIT(instantaneous_response->flags, 0) && !TEST_BIT(instantaneous_response->flags, 16))
                        state->delayed_response = 0;
                }
            }
            for (index = 0; index < section->responses.count; ++index) {
                if (TEST_BIT(responses_to_fire, index)) {
                    damage_handle_section_response(info, object_index, owner, reporting_info, section_index, index, damage_response_data);
                }
            }
        }
    }
}

// @address 0x33bc50 ?damage_section_deplete@@YAXPBUs_model_damage_info@@JPBUs_damage_owner@@EJPAUs_damage_response_data@@@Z
void damage_section_deplete(const s_model_damage_info *model_damage_info, long object_index, const s_damage_owner *owner,
    unsigned char response_index, long section_index, s_damage_response_data *response)
{
    const s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&model_damage_info->sections, section_index, s_damage_section);
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0UL);
    s_damage_object_section_state *state = &((s_damage_object_section_state *)((char *)object + object->damage_sections.offset))[section_index];
    for (long index = 0; index < section->responses.count; ++index) {
        long response_bit = 1 << index;
        if (state->flags & response_bit)
            continue;
        const s_instantaneous_damage_response *instantaneous_response = TAG_BLOCK_GET_ELEMENT(
                    &section->responses, index, s_instantaneous_damage_response);
        if (instantaneous_response->threshold != 0.0f)
            continue;
        bool matches_condition = true;
        switch (instantaneous_response->condition) {
        case 1: matches_condition = 0 == instantaneous_response->condition_argument; break;
        case 2: matches_condition = 0 != instantaneous_response->condition_argument; break;
        case 3: matches_condition = true; break;
        default: break;
        }
        if (!matches_condition)
            continue;
        if (instantaneous_response->skip_fraction != 0.0f) {
            float random_fraction = (float)(random_seed_next(TLS_RANDOM_SEED) >> 16) * 1.5259022e-5f;
            if (random_fraction < instantaneous_response->skip_fraction)
                continue;
        }
        float vitality = instantaneous_response->vitality;
        if (!(vitality >= 1.0f) && (instantaneous_response->flags & (1 << 21)))
            continue;
        if (!(vitality <= 1.0f) && !(instantaneous_response->flags & (1 << 21)))
            continue;
        if ((response->flags & (1 << 9)) && (instantaneous_response->flags & (1 << 24)))
            continue;
        if (!(response->flags & (1 << 9)) && (instantaneous_response->flags & (1 << 23))) {
            continue;
        }
        if (!(instantaneous_response->delay <= 0.0f) && !(response->flags & (1 << 10))) {
            if (state->delayed_response & 0xfff) {
                long delayed_index = state->delayed_response >> 12;
                if (delayed_index != index && !(state->flags & (1 << delayed_index))) {
                    state->flags |= response_bit;
                    damage_handle_section_response(model_damage_info, object_index, owner, state->response_index, section_index, delayed_index, response);
                }
            }
            state->response_index = (unsigned char)response_index;
            state->delayed_response = (state->delayed_response & 0xfff) | (index << 12);
            object_wake(object_index);
            if (!(state->delayed_response & 0xfff)) {
                long ticks = (long)((float)game_time_globals->ticks_per_second * instantaneous_response->delay + 0.5f);
                state->delayed_response = (ticks & 0xfff) | (state->delayed_response & 0xf000);
                if (instantaneous_response->delay_effect.index != -1) {
                    damage_spawn_effect_from_marker_name(instantaneous_response->delay_effect.index, object_index, owner,
                        instantaneous_response->delay_effect_marker, (((unsigned long)instantaneous_response->flags >> 20) & 1) != 0,
                        false);
                    simulation_action_damage_section_response(object_index, section_index, index, _damage_section_response_delayed);
                }
            }
        } else {
            state->flags |= response_bit;
            damage_handle_section_response(model_damage_info, object_index, owner, response_index, section_index, index, response);
        }
    }
    state->damage = 255;
}

static const float damage_shield_max_resistance = 1000.0f; // @rdata 0x820336f0
static const float damage_shield_quarter = 0.25f; // @rdata 0x82033bc0
// @address 0x3396b8 ?object_damage_shield@@YAXJPBUdamage_definition@@PBUs_damage_data@@PAUs_damage_response_data@@@Z
void object_damage_shield(long p0, const damage_definition *damage_definition_param, const s_damage_data *damage_data,
    s_damage_response_data *damage_response_data)
{
    const struct damage_definition *damage_definition = damage_definition_param;
    float body_damage = damage_response_data->body_damage;
    s_damage_object_datum *object = (s_damage_object_datum *)OBJECT_GET(p0, s_damage_object_datum, ~0u);
    float clamped;
    float shield;
    float clamped_shield;
    unsigned long obj_flags;
    float max_shield;
    s_global_material_definition *material_definition;
    float factor;
    float high;
    float scaled = 0.0f;
    float inv_high;
    float transferred;
    float post;
    if (body_damage > 0.0f)
        clamped = body_damage;
    else
        clamped = 0.0f;
    shield = object->shield_vitality;
    if (shield > 0.0f)
        clamped_shield = shield;
    else
        clamped_shield = 0.0f;
    object->shield_vitality = clamped_shield;
    if (clamped_shield == 0.0f)
        clamped = 0.0f;
    obj_flags = object->damage_flags;
    if ((unsigned char)((obj_flags >> 5) & 1)) {
        body_damage = 0.0f;
    } else if (!(unsigned char)((obj_flags >> 8) & 1)) {
        max_shield = object_get_maximum_shield_vitality(p0, damage_definition->category == 1);
        c_global_material_type material = unit_override_damage_info_material_type(
            p0, ((const s_model_damage_info *)damage_response_data->definition)->shield_material_type);
        material_definition = damage_global_get_material(material.m_index);
        factor = 1.0f;
        if (game_is_multiplayer() && object->object_type == 0) {
            long player_index = OBJECT_GET(p0, s_damage_object_datum, 0x2003)->unit.player_index;
            if (player_index != -1) {
                const player_datum *player = (const player_datum *)datum_try_and_get(player_data, player_index);
                float resist;
                if (player != 0)
                    resist = player->traits.m_shield_vitality_traits.get_damage_resistance_percentage();
                else
                    resist = 0.0f;
                if (!(damage_definition->flags & 0x100000) && !(resist <= k_damage_cone_epsilon)) {
                    if (!(resist < damage_shield_max_resistance))
                        factor = 0.0f;
                    else
                        factor = 1.0f / resist;
                }
            }
        }
        if (clamped_shield > max_shield)
            high = clamped_shield;
        else
            high = max_shield;
        if (high > 0.0f)
            inv_high = 1.0f / high;
        else
            inv_high = 0.0f;
        if (damage_data->in_range == 0)
            body_damage = body_damage - clamped;
        if (!(clamped <= 0.0f) && material_definition != 0) {
            long material_armor_b = material_definition->armor_type_b;
            factor = damage_armor_table_lookup(damage_definition->general_damage, damage_definition->specific_damage,
                material_definition->armor_type_a, material_armor_b) * factor;
        }
        scaled = factor * clamped;
        transferred = inv_high * scaled;
        damage_response_data->shield_response = transferred;
        post = object->shield_vitality;
        object->shield_vitality = post - transferred;
        if (game_is_multiplayer() != 0) {
        {
            long owner_index = damage_data->owner_data.player_index;
            const void *ally_player;
            const player_datum *ally;
            long ally_object;
            s_damage_object_datum *ally_datum;
            long ally_player_index;
            const void *damaged_player;
            float transfer;
            float ally_max;
            if (owner_index != -1) {
            ally_player = datum_try_and_get(player_data, owner_index);
            if (ally_player != 0) {
            ally = (const player_datum *)ally_player;
            ally_object = ally->unit_index;
            if (ally_object != -1 && ally_object != p0) {
            ally_datum = (s_damage_object_datum *)OBJECT_GET(ally_object, s_damage_object_datum, ~0u);
            if (object_header_get(p0)->object_type == 0 && object->unit.player_index != -1) {
                ally_player_index = object->unit.player_index;
                damaged_player = datum_try_and_get(player_data, ally_player_index);
            } else
                damaged_player = 0;
            switch ((long)ally->traits.m_shield_vitality_traits.m_vampirism - 1) {
            case 0:
                transfer = 0.0f;
                break;
            case 1:
                transfer = 0.1f;
                break;
            case 2:
                transfer = damage_shield_quarter;
                break;
            case 3:
                transfer = 0.5f;
                break;
            case 4:
                transfer = 1.0f;
                break;
            default:
                transfer = 0.0f;
                break;
            }
            ally_max = ally->traits.m_shield_vitality_traits.get_maximum_shield_vitality();
            if (damaged_player != 0) {
            if ((unsigned char)game_engine_has_teams() == 0 ||
                (unsigned char)game_engine_team_is_enemy((e_game_team)ally->configuration.host.team_index,
                    (e_game_team)((const player_datum *)damaged_player)->configuration.host.team_index) != 0) {
            {
                float scaled_transfer = transfer;
                float ally_shield = ally_datum->shield_vitality;
                if (ally_shield < ally_max && !(transfer <= k_damage_cone_epsilon)) {
                    float updated_shield = object->shield_vitality;
                    float selected = (updated_shield < 0.0f) ? post : transferred;
                    scaled_transfer *= selected;
                    float new_ally = (ally_shield + scaled_transfer > ally_max) ? ally_max : ally_shield + scaled_transfer;
                    ally_datum->shield_vitality = new_ally;
                    simulation_action_object_update(ally->unit_index, 128);
                }
            }
            }
            }
            }
            }
            }
        }
        }
        {
            float tail_shield = object->shield_vitality;
            if (!(tail_shield > 0.0f)) {
                if (!(tail_shield >= 0.0f)) {
                    float negative_shield = -tail_shield;
                    float back = negative_shield * max_shield;
                    if (!(back <= 0.0f) && !(factor <= k_damage_cone_epsilon)) {
                        float inverse_factor = 1.0f / factor;
                        body_damage = inverse_factor * back + body_damage;
                    }
                    object->shield_vitality = 0.0f;
                }
                if (!(unsigned char)((object->damage_flags >> 3) & 1))
                    object_deplete_shield_internal(p0, &damage_data->owner_data, damage_response_data);
            }
        }
        if (!(unsigned char)((object->damage_flags >> 1) & 1)) {
            const s_model_damage_info *def = (const s_model_damage_info *)damage_response_data->definition;
            if (!(def->shield_warning_threshold <= object->shield_vitality) && damage_response_data->silent == 0) {
                c_tag_index tag_storage;
                c_tag_index *tag = new (&tag_storage) c_tag_index(def->shield_warning_effect.index);
                effect_new_from_object(*tag, &damage_data->owner_data, 0, p0, 0.0f, 0.0f, (const real_rgb_color *)0, (const effect_vector_field *)0, 1);
                object->damage_flags = object->damage_flags | 2;
            }
        }
        {
            const s_model_damage_info *def = (const s_model_damage_info *)damage_response_data->definition;
            if (def->shield_damage_threshold < scaled
                || (object->shield_vitality == 0.0f && !(body_damage <= k_damage_cone_epsilon))
                || (damage_definition->side_effect == 3 && damage_data->in_range != 0)) {
            {
                float fv12 = def->shield_stun_time;
                if (((1 << (long)object->object_type) & 0x1003) != 0) {
                    long actor_index = ((const unit_datum *)object)->unit.actor_index;
                    if (actor_index != -1) {
                        character_vitality_properties *vit = character_definition_vitality_properties_get(
                            datum_get<actor_datum>(actor_data, actor_index)->character_index);
                        if (vit != 0)
                            fv12 = vit->surge_duration;
                    }
                    {
                        long player_index = object->unit.player_index;
                        if (player_index != -1) {
                            const player_datum *player = (const player_datum *)datum_try_and_get(player_data, player_index);
                            if (player != 0) {
                                float rate = player->traits.m_shield_vitality_traits.get_shield_recharge_rate_percentage();
                                if (!(rate <= 0.0f))
                                    fv12 = fv12 / rate;
                            }
                        }
                    }
                }
                if (object->damage_recharge_flags != 0x7fff)
                    object->damage_recharge_flags = (long)(game_time_globals->ticks_per_second * fv12 + 0.5f);
            }
            }
        }
    }
    simulation_action_object_update(p0, 128);
    damage_response_data->body_damage = body_damage;
}

// @address 0x339c80 ?object_compute_damage_aftermath@@YAXJPBUs_damage_data@@PBUs_damage_response_data@@PAUs_damage_aftermath_result_data@@@Z
void object_compute_damage_aftermath(long object_index, const s_damage_data *damage_data, const s_damage_response_data *response,
    // match: 0x339c80: pointer __restrict qualifier prevents alias reload
    s_damage_aftermath_result_data *__restrict result)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    result->flags = 0;
    result->damage_definition_index = damage_data->effect;
    result->body_damage = response->shield_damage_scale;
    result->shield_damage = response->body_damage_scale;
    result->epicenter = damage_data->position;
    result->owner = damage_data->owner_data;
    result->scale = damage_data->amount * damage_data->cone_amount;
    result->shake_scale = damage_data->shield_amount;
    result->body_part = response->node_index;
    result->node_index = (unsigned short)response->aftermath_node_raw;
    result->distance = damage_data->distance;
    result->ping_type = 0;
    result->response_metadata = response->response_metadata;
    float zero = 0.0f;
    if (result->body_damage == zero && result->shield_damage == zero && response->scaled_body_damage > zero)
        result->body_damage = response->scaled_body_damage;
    unsigned long damage_flags = damage_data->flags;
    unsigned long direct_damage = damage_flags & 1;
    if (direct_damage != 0 && !TEST_BIT(damage_flags, 8))
        result->direction = damage_data->initial_direction;
    else
        result->direction = damage_data->direction;
    result->reporting_info = damage_data->reporting_info;
    if (damage_data->in_range ||
        (damage_data->effect != -1 && ((s_damage_effect_definition *)tag_get(0x6a707421, damage_data->effect))->damage.side_effect == 3))
        result->flags |= 0x400;
    unsigned long response_flags = response->flags;
    if (TEST_BIT(response_flags, 0)) result->flags |= 1;
    if (TEST_BIT(response_flags, 2)) result->flags |= 0x200;
    if (TEST_BIT(response_flags, 3)) result->flags |= 2;
    if (TEST_BIT(damage_flags, 7)) result->flags |= 0x800;
    if (direct_damage != 0) result->flags |= 4;
    if (TEST_BIT(response_flags, 4)) result->flags |= 16;
    if (TEST_BIT(damage_flags, 4)) result->flags |= 32;
    if (TEST_BIT(response_flags, 6)) result->flags |= 64;
    if (TEST_BIT(damage_flags, 13)) result->flags |= 0x100;
    if (TEST_BIT(response_flags, 8)) result->flags |= 128;
    if (response->maximum_body_damage > zero) result->flags |= 8;
    if ((FLAG(object->object_type) & 0x1003) != 0)
        unit_damage_aftermath_compute(object_index, response_flags, damage_data, result);
    else if ((FLAG(object->object_type) & 0x800) != 0)
        creature_damage_aftermath_compute(object_index, response_flags, damage_data, result);
}


// @address 0x337798 ?area_effect_damage_test_obstruction@@YA_NJPBTreal_point3d@@PBUs_damage_data@@@Z
bool area_effect_damage_test_obstruction(long object_index, const real_point3d *point, const s_damage_data *damage)
{
    const s_damage_effect_definition *effect = (const s_damage_effect_definition *)tag_get('jpt!', damage->effect);
    const s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0u);
    const unit_definition *definition = (const unit_definition *)tag_get('obje', object->definition_index);
    bool obstructed = true;

    if ((FLAG(object->object_type) & 0x1003) != 0 && !(unsigned char)((object->damage_flags >> 2) & 1) &&
        effect->damage.aoe_core_radius > 0.0001f && damage->proximity == 0)
    {
        real_vector3d direction;
        real_vector3d perpendicular;
        real_vector3d cross;
        vector_from_points3d(&damage->origin, point, &direction);
        normalize3d(perpendicular3d(&direction, &perpendicular));
        normalize3d(cross_product3d(&direction, &perpendicular, &cross));

        for (long test_index = 0; test_index < 4; test_index++)
        {
            collision_result collision;
            real_vector3d vector;
            switch (test_index)
            {
            case 0:
                scale_vector3d(&perpendicular, effect->damage.aoe_core_radius, &vector);
                break;
            case 1:
                scale_vector3d(&perpendicular, -effect->damage.aoe_core_radius, &vector);
                break;
            case 2:
                scale_vector3d(&cross, effect->damage.aoe_core_radius, &vector);
                break;
            case 3:
                scale_vector3d(&cross, -effect->damage.aoe_core_radius, &vector);
                break;
            }

            collision_test_vector(k_collision_test_visibility, false, &damage->origin, &vector, object_get_ultimate_parent(object_index),
                damage->object_index, NONE, &collision);
            real_point3d test_point = collision.point;
            obstructed = collision_test_line(k_collision_test_visibility, &test_point, point, object_get_ultimate_parent(object_index),
                damage->object_index, &collision);
            if (!obstructed)
            {
                long child_index = OBJECT_GET(object_index, s_damage_object_datum, ~0u)->first_child_object_index;
                while (child_index != -1)
                {
                    const s_object_header *header = object_header_get(child_index);
                    const s_damage_object_datum *child = (const s_damage_object_datum *)header->datum;
                    if (TEST_BIT(FLAG(header->object_type), 3) &&
                        TEST_BIT(((const s_equipment_definition *)tag_get('eqip', child->definition_index))->flags, 3))
                    {
                        real_vector3d test_vector;
                        vector_from_points3d(&test_point, point, &test_vector);
                        const s_collision_test_flags *base_flags = &k_collision_test_visibility;
                        s_collision_test_flags flags = *base_flags;
                        flags.collision_flags &= ~FLAG(17);
                        if (object_test_vector(child_index, flags, &test_point, &test_vector, -1, -1, &collision))
                        {
                            obstructed = true;
                            break;
                        }
                    }
                    child_index = child->next_object_index;
                }
            }
            if (!obstructed)
                break;
        }
    }
    else
    {
        if ((unsigned char)((definition->s_object_definition::flags >> 15) & 1))
            return false;

        float radius = g_havok_constants.minimum_radius * 2.0f;
        collision_result collision;
        real_point3d center;
        object_get_center_of_mass(object_index, &center);
        real_vector3d vector;
        vector_from_points3d(point, &center, &vector);
        real_point3d test_point;
        if (normalize3d(&vector) > radius)
            point_from_line3d(point, &vector, radius, &test_point);
        else
            test_point = *point;
        return collision_test_line(k_collision_test_visibility_first_person, &damage->origin, &test_point,
            inl_object_get_ultimate_parent(object_index), damage->object_index, &collision);
    }
    return obstructed;
}

namespace {
static __forceinline bool damage_unit_seat_allows_assassination(long unit_index)
{
    bool result = false;
    if (TEST_BIT(FLAG(object_header_get(unit_index)->object_type), 0))
    {
        unit_seat *seat = unit_get_current_seat(unit_index);
        result = !seat || (bool)((seat->flags >> 20) & 1);
    }
    return result;
}
static __forceinline const s_model_damage_info *find_object_damage_info(long object_index)
{
    const s_model_damage_info *result = 0;
    if (object_index != -1) {
        const s_damage_object_datum *object =
            (const s_damage_object_datum *)object_header_get(object_index)->datum;
        const unit_definition *definition =
            (const unit_definition *)tag_get('obje', object->definition_index);
        unsigned long model_index = definition->model.index;
        if (model_index != 0xffffffff) {
            const s_model_definition *model =
                (const s_model_definition *)tag_get('hlmt', model_index);
            if (model->damage_info.count > 0)
                result = (const s_model_damage_info *)model->damage_info.address;
        }
    }
    return result;
}
}

// @address 0xf43c8
void object_cause_damage(s_damage_data *damage_data, long object_index, short node_index, short region_index, short material_index,
    const real_vector3d *direction)
{
    bool difficulty_scaled = false;
    bool parent_takes_body_damage = true;
    bool parent_takes_shield_damage = true;
    s_damage_effect_definition *effect = (s_damage_effect_definition *)tag_get('jpt!', damage_data->effect);
    damage_definition *damage = &effect->damage;
    long enemy_passenger_count = 0;
    bool driver_is_enemy = false;
    bool enemy_vehicle = vehicle_has_passengers(
        object_index, game_is_multiplayer(), false, damage_data->owner_data.player_index, &driver_is_enemy, &enemy_passenger_count);
    float total_damage = damage_compute_amount((s_damage_data *)damage_data, (const s_damage_effect_definition *)effect,
        (const damage_definition *)damage, object_index, &difficulty_scaled);
    float current_damage = total_damage;
    long damaged_object_count = 0;
    long damaged_object_indices[16];
    bool material_updated = false;

    if (TEST_BIT(damage_data->flags, 0) || TEST_BIT(damage_data->flags, 2) || TEST_BIT(damage_data->flags, 9) || TEST_BIT(damage_data->flags, 8))
    {
        damaged_object_indices[damaged_object_count++] = object_index;
    }
    else if (TEST_BIT(damage_data->flags, 12))
    {
        const s_model_damage_info *object_damage_info = object_get_damage_info(object_index);
        long parent_index = OBJECT_GET(object_index, s_damage_object_datum, ~0u)->parent;
        if (!object_damage_info && parent_index != NONE)
            damaged_object_indices[damaged_object_count++] = parent_index;
        else
            damaged_object_indices[damaged_object_count++] = object_index;
    }
    else
    {
        for (long index = object_index; index != NONE; index = OBJECT_GET(index, s_damage_object_datum, ~0u)->parent)
            damaged_object_indices[damaged_object_count++] = index;
    }

    const s_model_damage_info *object_damage_info = object_get_damage_info(object_index);
    if (object_damage_info)
    {
        parent_takes_body_damage = !TEST_BIT(object_damage_info->flags, 4);
        parent_takes_shield_damage = !TEST_BIT(object_damage_info->flags, 6);
    }

    for (long i = damaged_object_count - 1; i >= 0; i--)
    {
        long damaged_object_index = damaged_object_indices[i];
        s_damage_object_datum *object = OBJECT_GET(damaged_object_index, s_damage_object_datum, ~0u);
        const s_model_damage_info *damage_info = find_object_damage_info(damaged_object_index);
        unit_definition *object_definition = (unit_definition *)tag_get('obje', object->definition_index);
        s_damage_response_data response;
        memset(&response, 0, sizeof(response));
        response.maximum_body_damage = total_damage;
        response.body_damage = current_damage;
        response.node_index = NONE;
        if (i == 0)
            response.flags |= FLAG(8);

        if (damage_data->shot_id != NONE && damage_data->owner_data.player_index != NONE &&
            ((1 << object->object_type) & 0x1003) != 0 && object->unit.player_index != NONE)
        {
            damage_data->shot_id = NONE;
        }

        if (damage_info && object_definition->model.index != NONE)
        {
            s_model_definition *model = (s_model_definition *)tag_get('hlmt', object_definition->model.index);
            bool ignores_shields = false;
            response.definition = (const s_model_damage_info *)damage_info;
            if (VALID_INDEX(node_index, damage_info->nodes.count))
                response.node_index = TAG_BLOCK_GET_ELEMENT(&damage_info->nodes, node_index, s_damage_node)->runtime_damage_part;

            if (damaged_object_index == object_index && material_index != NONE)
            {
                const s_damage_model_material *material = TAG_BLOCK_GET_ELEMENT(&model->materials, material_index, s_damage_model_material);
                damage_data->material_type = material->material;
                response.material = material;
            }
            else if (damaged_object_index == object_index && !TEST_BIT(damage_data->flags, 0))
            {
                damage_data->material_type = damage_info->indirect_material_type;
            }
            else if (TEST_BIT(damage_data->flags, 0))
            {
                if (damage_data->model_region_index != NONE &&
                    VALID_INDEX(damage_data->model_region_index, model->materials.count) &&
                    damage_data->material_type == TAG_BLOCK_GET_ELEMENT(&model->materials, damage_data->model_region_index, s_damage_model_material)->material)
                {
                    response.material = TAG_BLOCK_GET_ELEMENT(&model->materials, damage_data->model_region_index, s_damage_model_material);
                    material_index = (short)damage_data->model_region_index;
                }
                else
                {
                    damage_data->material_type = damage_info->indirect_material_type;
                }
            }
            damage_data->material_type = unit_override_damage_info_material_type(damaged_object_index, damage_data->material_type);

            if (i == 0)
            {
                response.region_index = region_index;
                response.aftermath_node_raw = node_index;
                response.direction = direction;
            }
            else
            {
                response.region_index = NONE;
                response.aftermath_node_raw = NONE;
                response.direction = 0;
            }

            long section_index = response.material ? response.material->damage_section : damage_info->indirect_damage_section;
            if (VALID_INDEX(section_index, damage_info->sections.count) &&
                TEST_BIT(TAG_BLOCK_GET_ELEMENT(&damage_info->sections, section_index, s_damage_section)->flags, 8))
                ignores_shields = true;

            if (current_damage == 0.0f && i == 0)
            {
                if (object->shield_vitality > 0.0f && !ignores_shields)
                {
                    damage_data->material_type = damage_info->shield_material_type;
                    damage_data->shield_vitality = object->shield_vitality;
                }
                damage_data->material_type = unit_override_damage_info_material_type(damaged_object_index, damage_data->material_type);
            }

            if (current_damage > 0.0f || damage_data->in_range || damage_data->shield_amount > 0.0f)
            {
                bool killed_instantly = TEST_BIT(damage_data->flags, 2);
                object_set_damage_owner(damaged_object_index, &damage_data->owner_data, false);
                damage_handle_acceleration(damaged_object_index, damage_data->effect, response.aftermath_node_raw, &damage_data->owner_data,
                    damage_data->cone_amount * damage_data->amount, &damage_data->direction, &damage_data->position,
                    TEST_BIT(response.flags, 8), TEST_BIT(damage_data->flags, 13), TEST_BIT(damage_data->flags, 0));
                if (difficulty_scaled)
                    response.flags |= FLAG(5);
                if (damage_data->owner_data.team != NONE && ((1 << object->object_type) & 0x1003) != 0 &&
                    game_team_is_friendly((e_game_team)object->unit.team, (e_game_team)damage_data->owner_data.team))
                {
                    response.flags |= FLAG(4);
                }
                if (TEST_BIT(damage->flags, 17))
                    response.flags |= FLAG(9);

                if (i == 0 && damage_unit_seat_allows_assassination(damaged_object_index) &&
                    !TEST_BIT(damage_data->flags, 9) && damage->side_effect == 2 &&
                    unit_can_be_assassinated(damaged_object_index, &damage_data->position, &damage_data->direction) &&
                    !(unsigned char)((object->damage_flags >> 8) & 1))
                {
                    const s_object_header *header = object_header_get(damaged_object_index);
                    if (!(((1 << header->object_type) & 0x1003) != 0 &&
                          game_team_is_friendly((e_game_team)damage_data->owner_data.team,
                              (e_game_team)((const unit_datum *)header->datum)->unit.team)))
                    {
                        killed_instantly = true;
                        damage_data->reporting_info = (damage_data->reporting_info & ~3) | 2;
                    }
                }

                if (i + 1 < damaged_object_count)
                    response.body_damage *= object_determine_rider_damage_scale(damage_data, damaged_object_index, damaged_object_indices[i + 1]);

                if (TEST_BIT(damage_info->flags, 7) || object_damage_vitality_pinned(object))
                    killed_instantly = false;

                if (killed_instantly && !(unsigned char)((object->damage_flags >> 2) & 1))
                {
                    response.flags |= FLAG(6);
                    object_kill(damaged_object_index, damage_data, &damage_data->owner_data, (s_damage_response_data *)&response,
                        !TEST_BIT(damage_data->flags, 7), damage_data->reporting_info);
                }

                if (!TEST_BIT(damage_data->flags, 5) && !TEST_BIT(damage->flags, 9) &&
                    (!ignores_shields || damage_data->in_range || damage->side_effect == 3) &&
                    object->maximum_shield_vitality > 0.0f &&
                    (i == 0 || parent_takes_shield_damage && TEST_BIT(((const s_model_damage_info *)response.definition)->flags, 0)))
                {
                    object_damage_shield(damaged_object_index, damage, damage_data, &response);
                }

                if (!game_is_predicted() && !TEST_BIT(damage_data->flags, 8) &&
                    TEST_BIT(FLAG(object_header_get(damaged_object_index)->object_type), 1) &&
                    !TEST_BIT(damage_data->flags, 14) && i == 0 && response.definition &&
                    (!TEST_BIT(damage_data->flags, 12) || TEST_BIT(damage_info->flags, 8)))
                {
                    handle_vehicle_damage_transfer(damage_data, damaged_object_index);
                }

                if (!TEST_BIT(damage->flags, 6) &&
                    (i == 0 || parent_takes_body_damage && TEST_BIT(((const s_model_damage_info *)response.definition)->flags, 1)))
                {
                    if (TEST_BIT(damage_data->flags, 16))
                        area_of_effect_cause_damage_to_object_targets(damage_data, response.body_damage, damaged_object_index, &response);
                    object_damage_body(damaged_object_index, damage, damage_data, &response);
                }

                if (!material_updated)
                {
                    float shield_damage = response.body_damage_scale;
                    float body_damage = response.shield_damage_scale;
                    float other_damage = response.scaled_body_damage;
                    if (shield_damage > k_damage_cone_epsilon || other_damage > k_damage_cone_epsilon || body_damage > k_damage_cone_epsilon)
                    {
                        if (shield_damage > MAX(body_damage, other_damage))
                        {
                            damage_data->material_type = ((const s_model_damage_info *)response.definition)->shield_material_type;
                            damage_data->shield_vitality = object->shield_vitality;
                        }
                        else
                        {
                            damage_data->shield_vitality = PIN(object->body_vitality, 0.0f, 1.0f);
                        }
                        damage_data->material_type = unit_override_damage_info_material_type(damaged_object_index, damage_data->material_type);
                        object_wake(damaged_object_index);
                        material_updated = true;
                    }
                }
            }
        }
        else
        {
            damage_handle_acceleration(damaged_object_index, damage_data->effect, response.aftermath_node_raw, &damage_data->owner_data,
                damage_data->cone_amount * damage_data->amount, &damage_data->direction, &damage_data->position,
                TEST_BIT(response.flags, 8), TEST_BIT(damage_data->flags, 13), TEST_BIT(damage_data->flags, 0));
        }

        if (object->component_index != NONE && (response.first_constraint != 0 || response.second_constraint != 0))
            object_damage_constraints(object_index, response.first_constraint, response.second_constraint);

        s_damage_aftermath_result_data result;
        object_compute_damage_aftermath(damaged_object_index, damage_data, &response, &result);
        if (TEST_BIT(result.flags, 0) && enemy_vehicle)
        {
            long player_index = damage_data->owner_data.player_index;
            if (player_index != NONE)
            {
                if (damage_reporting_info_get_type(damage_data->reporting_info) == 59 && driver_is_enemy)
                    game_results_try_and_grant_achievement(player_index, _achievement_used_car_salesman);
                if (enemy_passenger_count >= 3)
                    game_results_try_and_grant_achievement(player_index, _achievement_we_re_in_for_some_chop);
            }
        }
        if (game_is_predicted())
            result.flags &= ~FLAG(0);
        object_apply_damage_aftermath(damaged_object_index, &result);
        if (TEST_BIT(response.flags, 2) && !game_is_predicted())
            object_delete(damaged_object_index);
        current_damage = response.body_damage;
    }
}


// @address 0xf5460
void object_damage_new(long object_index)
{
    if (!game_uses_predicted_simulation())
    {
        object_datum *object = (object_datum *)OBJECT_GET(object_index, s_damage_object_datum, ~0UL);
        const s_model_damage_info *damage_info = object_get_damage_info(object_index);
        s_damage_response_data damage_response;
        memset(&damage_response, 0, sizeof(damage_response));
        s_damage_owner damage_owner;
        damage_compute_damage_owner_from_object_index(object_index, &damage_owner);
        long section_count = (unsigned long)object->damage_section_bytes / sizeof(s_damage_object_section_state);
        s_damage_object_section_state *states = (s_damage_object_section_state *)((char *)object + object->damage_section_offset);
        for (long section_index = 0; section_index < section_count; ++section_index)
        {
            s_damage_object_section_state *state = &states[section_index];
            const s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&damage_info->sections, section_index, s_damage_section);
            for (long response_index = 0; response_index < section->responses.count; ++response_index)
            {
                const s_instantaneous_damage_response *response = TAG_BLOCK_GET_ELEMENT(&section->responses, response_index, s_instantaneous_damage_response);
                if (!TEST_BIT(state->flags, response_index) && TEST_BIT(response->flags, 18))
                {
                    if (!(response->delay <= 0.0f))
                    {
                        if (state->delayed_response & 0xfff)
                        {
                            long delayed_index = state->delayed_response >> 12;
                            if (delayed_index != response_index && !TEST_BIT(state->flags, delayed_index))
                            {
                                state->flags |= FLAG(response_index);
                                damage_handle_section_response(damage_info, object_index, &damage_owner, state->response_index,
                                    section_index, delayed_index, &damage_response);
                            }
                        }
                        state->delayed_response = (state->delayed_response & 0xfff) | (response_index << 12);
                        state->response_index = 0;
                        long ticks = damage_seconds_to_ticks_round(response->delay, 0.5f);
                        state->delayed_response = (ticks & 0xfff) | (state->delayed_response & 0xf000);
                        object_wake(object_index);
                        if (response->delay_effect.index != NONE)
                        {
                            damage_spawn_effect_from_marker_name(response->delay_effect.index, object_index, &damage_owner,
                                response->delay_effect_marker, TEST_BIT(response->flags, 20), false);
                            simulation_action_damage_section_response(object_index, section_index, response_index, _damage_section_response_delayed);
                        }
                    }
                    else
                    {
                        state->flags |= FLAG(response_index);
                        damage_handle_section_response(damage_info, object_index, &damage_owner, 0, section_index, response_index, &damage_response);
                    }
                }
            }
        }
    }
}

namespace {
static const float k_damage_update_one = 1.0f;

static __forceinline game_time_globals_definition *damage_update_get_game_time()
{
    return game_time_globals;
}

static __forceinline void damage_update_get_owner(long object_index, s_damage_owner *owner)
{
    object_datum *object = (object_datum *)OBJECT_GET(object_index, s_damage_object_datum, ~0UL);
    *owner = object->damage_owner;
}

static __forceinline void damage_update_get_inherited_owner(long object_index, const s_damage_object_datum *object, s_damage_owner *owner)
{
    damage_update_get_owner(object_index, owner);
    if (owner->object_index == NONE && object->m_object_flags.test(_object_data_flag_26))
    {
        if (object->parent != NONE)
            damage_update_get_owner(object->parent, owner);
    }
}

static __forceinline void damage_update_kill_instantly(long object_index, unsigned long flags, const s_damage_object_datum *object)
{
    s_damage_owner owner;
    damage_update_get_owner(object_index, &owner);
    if (game_globals->options.game_mode == 1 && owner.player_index == NONE && (FLAG(object->object_type) & 0x1003))
    {
        long player_index = NONE;
        unsigned long best_age = 1024;
        const unit_datum *unit = (const unit_datum *)object;
        const s_unit_damage_record *record = unit->unit.damage_records;
        for (long i = 0; i < 4; ++i, ++record)
        {
            if (record->player_index != NONE)
            {
                unsigned long age = game_time_globals->time - record->time;
                if (age < best_age)
                {
                    player_index = record->player_index;
                    best_age = age;
                }
            }
        }
        owner.player_index = player_index;
    }
    object_cause_default_damage(object_index, TEST_BIT(flags, 7), TEST_BIT(flags, 12), false, &owner, 0);
}

static __forceinline void damage_update_restore_shield(long object_index)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0UL);
    if (object->m_damage_flags.test(_damage_shield_depleted))
    {
        object->m_damage_flags.set(_damage_shield_depleted, false);
        object_set_model_state_property_per_region(object_index, NONE, 2, 0, true);
    }
}
}

// @address 0xdd9e0
bool object_damage_update(long object_index)
{
    s_damage_object_datum *object = OBJECT_GET(object_index, s_damage_object_datum, ~0UL);
    const s_model_damage_info *damage_info = object_get_damage_info(object_index);
    bool update = false;
    if ((FLAG(object->object_type) & 0x1003) && damage_game_skull_is_active_primary(1)
        && object->unit.player_index != NONE && object->shield_stun_ticks > 1)
        object->shield_stun_ticks = 10;
    if (damage_info)
    {
        const character_vitality_properties *vitality = 0;
        s_damage_owner deplete_owner;
        if (damage_info->sections.count > 0 && !game_uses_predicted_simulation())
        {
            long section_count = (unsigned long)(short)object->damage_sections.size / sizeof(s_damage_object_section_state);
            s_damage_object_section_state *states = (s_damage_object_section_state *)((char *)object + object->damage_sections.offset);
            for (long section_index = 0; section_index < section_count; ++section_index)
            {
                if ((states[section_index].delayed_response & 0xfff) != 0)
                {
                    update = true;
                    states[section_index].delayed_response = (states[section_index].delayed_response & 0xf000)
                        | ((states[section_index].delayed_response - 1) & 0xfff);
                    if ((states[section_index].delayed_response & 0xfff) == 0)
                    {
                        s_damage_owner owner;
                        (void)&owner.team;
                        damage_update_get_inherited_owner(object_index, object, &owner);
                        long response_index = states[section_index].delayed_response >> 12;
                        if (damage_section_response_is_valid(damage_info, object_index, &owner, section_index, response_index))
                        {
                            unsigned char reporting_info = states[section_index].response_index;
                            s_damage_response_data response;
                            memset(&response, 0, sizeof(response));
                            states[section_index].flags |= FLAG(response_index);
                            damage_handle_section_response(damage_info, object_index, &owner, reporting_info, section_index, response_index, &response);
                            if (response.first_constraint || response.second_constraint)
                                object_damage_constraints(object_index, response.first_constraint, response.second_constraint);
                            if (game_is_authoritative() && TEST_BIT(response.flags, 0) && (FLAG(object->object_type) & 0x1003))
                            {
                                const s_falling_damage_definition *options = (const s_falling_damage_definition *)global_game_globals->falling_damage.address;
                                s_damage_aftermath_result_data aftermath;
                                memset(&aftermath, 0, sizeof(aftermath));
                                aftermath.flags |= FLAG(0);
                                if (TEST_BIT(response.flags, 2))
                                    aftermath.flags |= FLAG(9);
                                else
                                    aftermath.flags &= ~FLAG(9);
                                aftermath.body_damage = k_damage_update_one;
                                aftermath.body_part = NONE;
                                aftermath.damage_definition_index = options->falling_damage.index;
                                aftermath.owner = owner;
                                aftermath.reporting_info = reporting_info;
                                aftermath.direction = *global_up3d;
                                object_apply_damage_aftermath(object_index, &aftermath);
                            }
                            if (TEST_BIT(response.flags, 2))
                                object_destroy_internal(object_index, owner.player_index, owner.object_index, reporting_info, false);
                        }
                    }
                }
            }
        }
        if ((FLAG(object->object_type) & 0x1003) && object->unit.actor_index != NONE)
            vitality = character_definition_vitality_properties_get(datum_get<actor_datum>(actor_data, object->unit.actor_index)->character_index);
        if (object->m_damage_flags.test(_object_die_from_no_riders_bit))
        {
            damage_compute_damage_owner_from_object_index(object_index, &deplete_owner);
            s_damage_response_data response;
            memset(&response, 0, sizeof(response));
            object_kill(object_index, 0, &deplete_owner, (s_damage_response_data *)&response, true, 0);
            object->m_damage_flags.set(_object_die_from_no_riders_bit, false);
        }
        if (object->m_damage_flags.test(_object_die_act_of_god_bit) || object->m_damage_flags.test(_object_die_act_of_god_silent_bit) ||
            object->m_damage_flags.test(_object_die_act_of_god_no_statistics_bit))
        {
            if (!game_uses_predicted_simulation())
                damage_update_kill_instantly(object_index, object->damage_flags, object);
            object->m_damage_flags.set(_object_die_act_of_god_bit, false);
            object->m_damage_flags.set(_object_die_act_of_god_silent_bit, false);
            object->m_damage_flags.set(_object_die_act_of_god_no_statistics_bit, false);
        }
        float maximum_shield = 1.0f;
        float overshield = 2.0f;
        float recharge_scale = 1.0f;
        if (game_is_multiplayer() && (FLAG(object->object_type) & 0x1003) && object->unit.player_index != NONE)
        {
            const player_datum *player = datum_get<const player_datum>(player_data, object->unit.player_index);
            maximum_shield = player->traits.m_shield_vitality_traits.get_maximum_shield_vitality();
            overshield = 2.0f * maximum_shield;
            recharge_scale = player->traits.m_shield_vitality_traits.get_shield_recharge_rate_percentage();
        }
        if (!object->m_damage_flags.test(_object_damage_body_depleted_bit) && !object->m_damage_flags.test(_damage_shield_destroyed) &&
            object->maximum_shield_vitality > 0.0f)
        {
            bool decay_shield = false;
            if (game_is_multiplayer() && (FLAG(object->object_type) & 0x1003) && object->unit.player_index != NONE)
            {
                const player_datum *player = datum_get<const player_datum>(player_data, object->unit.player_index);
                if (player->traits.m_shield_vitality_traits.get_shield_recharge_rate_percentage() < 0.0f)
                    decay_shield = true;
            }
            if (object->m_damage_flags.test(_object_shield_over_charging_bit))
            {
                object->shield_vitality += overshield / (damage_tick_rate() * 2.0f);
                if (object->shield_vitality >= overshield + 1.0f)
                {
                    object->shield_vitality = overshield + 1.0f;
                    object->m_damage_flags.set(_object_shield_over_charging_bit, false);
                    object->m_damage_flags.set(_object_shield_charging_bit, false);
                }
                else
                {
                    update = true;
                    object->m_damage_flags.set(_object_shield_charging_bit, true);
                }
            }
            else if (object->shield_vitality > maximum_shield && game_is_multiplayer())
            {
                float excess_shield = object->shield_vitality - 1.0f;
                float decay = overshield / (damage_tick_rate() * 90.0f);
                object->m_damage_flags.set(_object_shield_charging_bit, false);
                if (decay > excess_shield)
                    object->shield_vitality = maximum_shield;
                else
                    object->shield_vitality -= decay;
                simulation_action_object_update(object_index, 128);
                update = true;
            }
            else if (object->m_damage_flags.test(_object_shield_charging_bit) || object->shield_vitality < maximum_shield || decay_shield)
            {
                if (object->shield_stun_ticks == 0)
                {
                    float recharge_rate = vitality ? vitality->shield_recharge_rate : damage_info->shield_recharge_rate;
                    recharge_rate *= recharge_scale;
                    const long &object_type = object->object_type;
                    game_time_globals_definition *const &time_globals = damage_update_get_game_time();
                    float recharge = recharge_rate * time_globals->tick_length;
                    if (FLAG(object_type) & 0x1003)
                        recharge *= game_difficulty_get_team_value(3, (e_game_team)object->unit.team);
                    if (recharge > 0.0f)
                    {
                        damage_update_restore_shield(object_index);
                        if (!object->m_damage_flags.test(_object_shield_charging_bit)
                            && (object->shield_vitality == 0.0f || !TEST_BIT(damage_info->flags, 13)))
                        {
                            s_damage_owner owner;
                            damage_compute_damage_owner_from_object_index(object_index, &owner);
                            effect_new_from_object(damage_info->shield_recharge_effect.index, &owner, 0, object_index, 0.0f, 0.0f, 0, 0, 1);
                        }
                    }
                    object->shield_vitality += recharge;
                    if (object->shield_vitality >= maximum_shield)
                    {
                        bool continue_recharge = false;
                        if (game_uses_predicted_simulation())
                        {
                            update = true;
                            continue_recharge = object->m_damage_flags.test(_object_shield_charging_networked_bit);
                        }
                        object->shield_vitality = maximum_shield;
                        object->m_damage_flags.set(_object_shield_charging_bit, continue_recharge);
                        object->m_damage_flags.set(_object_passed_shield_damage_threshold_bit, false);
                    }
                    else if (recharge <= 0.0f)
                    {
                        if (object->shield_vitality < 0.0f)
                            object->shield_vitality = 0.0f;
                        else
                            update = true;
                        object->m_damage_flags.set(_object_shield_charging_bit, false);
                        object->m_damage_flags.set(_object_passed_shield_damage_threshold_bit, false);
                    }
                    else
                    {
                        update = true;
                        object->m_damage_flags.set(_object_shield_charging_bit, true);
                    }
                }
                else
                {
                    if (!game_uses_predicted_simulation() && object->shield_stun_ticks != 0x7fff)
                    {
                        update = true;
                        --object->shield_stun_ticks;
                    }
                    object->m_damage_flags.set(_object_shield_charging_bit, false);
                }
                simulation_action_object_update(object_index, 128);
            }
            else
                object->m_damage_flags.set(_object_shield_charging_bit, false);
        }
        if (object->maximum_body_vitality > 0.0f && (!object->m_damage_flags.test(_object_damage_body_depleted_bit) || TEST_BIT(damage_info->flags, 12)))
        {
            float recharge_rate;
            float maximum_body;
            if (vitality)
            {
                recharge_rate = vitality->body_recharge_rate;
                maximum_body = vitality->body_recharge_fraction;
            }
            else
            {
                recharge_rate = damage_info->body_recharge_rate;
                maximum_body = damage_info->body_recharge_fraction;
            }
            if (object->body_vitality < maximum_body && recharge_rate > 0.0f)
            {
                if (object->body_stun_ticks == 0)
                {
                    float recharge = game_time_globals->tick_length * recharge_rate;
                    if (FLAG(object->object_type) & 0x1003)
                    {
                        e_game_team team = (e_game_team)object->unit.team;
                        recharge *= game_difficulty_get_team_value(3, team);
                    }
                    object->m_damage_flags.set(_object_body_charging_bit, true);
                    const float &previous_body_vitality = +object->body_vitality;
                    object->body_vitality = recharge + previous_body_vitality;
                    if (object->body_vitality > maximum_body)
                    {
                        object->body_vitality = maximum_body;
                        object->m_damage_flags.set(_object_body_charging_bit, false);
                    }
                    else
                        update = true;
                }
                else if (!game_uses_predicted_simulation())
                {
                    --object->body_stun_ticks;
                    update = true;
                }
                simulation_action_object_update(object_index, 64);
            }
        }
        if (object->maximum_body_vitality > 0.0f && !object->m_damage_flags.test(_object_damage_body_depleted_bit))
        {
            long section_count = (unsigned long)(short)object->damage_sections.size / sizeof(s_damage_object_section_state);
            s_damage_object_section_state *states = (s_damage_object_section_state *)((char *)object + object->damage_sections.offset);
            for (long section_index = 0; section_index < section_count; ++section_index)
            {
                if (states[section_index].damage)
                {
                    if (states[section_index].recharge_delay_ticks == 0 && damage_info->sections.count > section_index)
                    {
                        const s_damage_section *section = TAG_BLOCK_GET_ELEMENT(&damage_info->sections, section_index, s_damage_section);
                        if (section->recharge_rate > 0.0f)
                        {
                            long recharge = (long)(game_time_globals->tick_length * section->recharge_rate * 255.0f + 0.5f);
                            if (states[section_index].damage > recharge)
                            {
                                states[section_index].damage -= recharge;
                                update = true;
                            }
                            else
                                states[section_index].damage = 0;
                        }
                    }
                    else
                    {
                        --states[section_index].recharge_delay_ticks;
                        update = true;
                    }
                }
            }
        }
        update |= object_damage_update_decay(vitality, object->m_damage_flags.test(_object_damage_body_depleted_bit), &object->current_body_damage,
            &object->body_damage, (char *)&object->body_damage_reset);
        if (object->m_damage_flags.test(_object_shield_damage_scripted_bit))
        {
            if (object->shield_decay_timer > 0)
            {
                object->current_shield_damage += object->shield_damage;
                --object->shield_decay_timer;
            }
            update = true;
        }
        else
            update |= object_damage_update_decay(vitality, object->m_damage_flags.test(_damage_shield_depleted),
                &object->current_shield_damage, &object->shield_damage, (char *)&object->shield_damage_reset);
    }
    return update;
}
