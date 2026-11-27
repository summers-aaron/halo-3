// blamlibxbox/hs_library_external.obj: from the records 0x2ba1d8 0x2ba2c8 0x2ba420 0x2ba5b0 0x2ba710 0x2ba858 0x2ba9b8 0x2ba9d8 0x2ba9f8 0x2baa90 0x2bab68 0x2baca8 0x2bad20 0x2bae68 0x2bb050 0x2bb190 0x2bb210 0x2bb498 0x2bb518 0x2bb5c8 0x2bb750 0x2bb8f8 0x2bb998 0x2bbab0 0x2bbc98 0x2bbe88 0x2bbff8
// @flags /TP /O2 /Oc
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "cseries/cseries_types.h"
#include "tag_files/tag_index.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "memory/data_types.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"
#include "game/player_types.h"
#include "game/game_globals.h"
#include "game/game_globals_storage.h"
#include "game/game_simulation_predicates.h"
#include "objects/object_iterator.h"
#include "objects/object_placement.h"
#include "ai/ai_point3d.h"
#include "game/materials.h"
#include "physics/collision_types.h"
#include "physics/collision_test_vector.h"
#include "scenario/scenario_tag.h"
// @address 0x2bb190 ?hs_object_iterate_names_containing@@YAXPBDP6AXF@Z@Z
// @address 0x2bb210 hs_object_teleport_to_position
// @address 0x2bb518 ?hs_objects_predict@@YAXJ_N@Z
// @address 0x2ba1d8 ?hs_teleport_players_not_in_trigger_volume@@YAXFF@Z
// @address 0x2ba710 ?hs_objects_distance_to_object@@YAMJJ@Z
// @address 0x2bad20 ?hs_object_destroy_all_type_mask@@YAXJ@Z
// @address 0x2ba9b8 ?hs_object_create@@YAXF@Z
// @address 0x2ba9d8 ?hs_object_create_clone@@YAXF@Z
// @address 0x2ba9f8 ?hs_object_create_anew@@YAXF@Z
// @address 0x2bbc98 ?hs_damage_object_effect@@YAXJJ@Z
// @address 0x2bbe88 ?hs_damage_object@@YAXJJM@Z
// @address 0x2bbff8 ?hs_damage_players@@YAXJ@Z
// @address 0x2baca8 ?hs_object_destroy_by_name@@YAXF@Z
// @address 0x2bae68 hs_objects_delete_by_name_in_table
#include <ppcintrinsics.h>
#include <math.h>
#include "game/players.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "scenario/scenario_access.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "ai/ai_reference_frame.h"
#include "ai/cl_engine.h"
#include "hs/object_lists.h"
#include "scenario/scenario_trigger_volumes.h"
#include "motor/action_system.h"
#include "hs/hs_library_external.h"
#include "memory/data.h"
#include "memory/data_absolute_index.h"
#include "objects/object_datum.h"
#include "items/item_types.h"
#include "game/player_with_unit_iterator.h"
#include "cache/cache_file_globals.h"
#include "objects/damage.h"
#include "effects/effects.h"
#include "effects/effect_creation.h"
#include "scenario/scenario_cutscene_flag.h"
#include "hs/hs_runtime_data.h"
#include "objects/object_list_header_types.h"
#include "objects/list_reference_types.h"
#include "objects/object_name_list.h"
#include "math/real_math_globals.h"
#include "scenario/scenario_object_name_types.h"
#include <new>
#include "objects/damage_data.h"
#include "ai/cs_scenario_definitions.h"

#include "game/falling_damage_types.h"
#include "scenario/scenario_objects.h"
#include "models/model_types.h"

#include "units/unit_types.h"
#include "objects/object_definitions.h"
#include "objects/object_runtime_tls.h"
#include "hs/object_list_inlines.h"

 static __forceinline long hs_flag_list_first(long list, long *iterator) {
    long result = -1;
    if (list != -1) {
        *iterator = datum_get<s_object_list_header>(tls_object_list_header_data(), list)->first_list_reference;
        result = object_list_get_first(list, iterator);
    }
    return result;
}
static __forceinline long hs_flag_list_next(long list, long *iterator) {
    s_data_array *array = tls_list_object_reference_data();
    long result;
    if (*iterator != -1) {
        s_list_reference *datum = (s_list_reference *)datum_get(array, *iterator);
        long next = datum->next_index;
        if (next != -1)
            __dcbt(0, array->data + (unsigned short)next * array->size);
        result = datum->value;
        *iterator = next;
    } else result = -1;
    return result;
}

// Inline expansion of data_next_index; the out-of-line copy lives in data.obj.
static __forceinline long data_next_index_inline(s_data_array *array, long index)
{
    long absolute_index = ::data_next_absolute_index((const ::s_data_array *)(array), (unsigned short)(index + 1));
    return datum_absolute_index_to_index(array, absolute_index);
}
static __forceinline bool hs_library_external_trigger_volume_test_object(long volume, long object_index)
{
    bool result = false;
    if (object_index != -1) {
        const real_point3d *position = &object_get(object_index)->bounding_center;
        if (scenario_trigger_volume_test_point(volume, position)) result = true;
    }
    return result;
}

// 0x2ba1d8
void hs_teleport_players_not_in_trigger_volume(short volume, short flag)
{
    for (long player_index = ::data_next_index(tls_player_data(), -1); player_index != -1;
         player_index = data_next_index_inline(tls_player_data(), player_index)) {
        player_datum *player = datum_get<player_datum>(tls_player_data(), player_index);
        if (player->unit_index != -1 && !hs_library_external_trigger_volume_test_object(volume, player->unit_index))
            hs_object_teleport(player->unit_index, flag);
    }
}

// @address 0x2ba2c8 hs_trigger_volume_test_objects
extern "C" signed char hs_trigger_volume_test_objects(short trigger_volume_index, int test_object_list, unsigned char require_all)
{
    object_datum *object_data;
    unsigned char require_all_flag;
    s_data_array *reference_array;
    char *reference_data;
    int reference_size;
    s_list_reference *current_reference;
    int next_reference_index;
    int first_trigger_object;
    int object_index;
    unsigned char object_in_volume;
    int trigger_reference_cursor;
    int next_object_index;
    long reference_iterator_storage;
    first_trigger_object = -1;
    if (test_object_list != -1) {
        reference_iterator_storage = datum_get<s_object_list_header>(tls_object_list_header_data(), test_object_list)->first_list_reference;
        first_trigger_object = object_list_get_first(test_object_list, &reference_iterator_storage);
    }
    object_index = first_trigger_object;
    if (first_trigger_object != -1) {
        trigger_reference_cursor = reference_iterator_storage;
        do {
            object_in_volume = 0;
            if (object_index != -1) {
                object_data = object_get(object_index);
                if (((unsigned char)::scenario_trigger_volume_test_point(
                        ((short)trigger_volume_index), &object_data->bounding_center)) != 0)
                    object_in_volume = 1;
            }
            require_all_flag = ((unsigned char)require_all);
            if (((unsigned char)object_in_volume) != 0) {
                if (require_all_flag == 0)
                    return 1;
            }
            else if (require_all_flag != 0) {
                return 0;
            }
            reference_array = tls_list_object_reference_data();
            if (trigger_reference_cursor != -1) {
                reference_data = reference_array->data;
                reference_size = reference_array->size;
                current_reference = (s_list_reference *)(reference_data + (unsigned short)trigger_reference_cursor * reference_size);
                next_reference_index = current_reference->next_index;
                if (next_reference_index != -1) {
                    __dcbt(0, (void *)((((unsigned short)next_reference_index) * reference_size) + reference_data));
                }
                next_object_index = current_reference->value;
                trigger_reference_cursor = next_reference_index;
            } else {
                next_object_index = -1;
            }
            object_index = next_object_index;
        } while (next_object_index != -1);
    }
    return require_all;
}

 static __forceinline long hs_library_external_distance_list_first(long list, long *iterator) {
    long result = -1;
    if (list != -1) {
        *iterator = datum_get<s_object_list_header>(tls_object_list_header_data(), list)->first_list_reference;
        result = object_list_get_first(list, iterator);
    }
    return result;
}
static __forceinline long hs_library_external_distance_list_next(long list, long *iterator) {
    s_data_array *array = tls_list_object_reference_data();
    long result;
    if (*iterator != -1) {
        s_list_reference *datum = (s_list_reference *)datum_get(array, *iterator);
        long next = datum->next_index;
        if (next != -1)
            __dcbt(0, array->data + (unsigned short)next * array->size);
        result = datum->value;
        *iterator = next;
    } else result = -1;
    return result;
}

// 0x2ba710
float hs_objects_distance_to_object(long object_list_index, long object_index)
{
    float result = 3.4028234663852886e38f;
    if (object_index != -1) {
        real_point3d origin;
        object_get_origin(object_index, &origin);
        long reference_index;
        for (long current_object_index = hs_library_external_distance_list_first(object_list_index, &reference_index);
             current_object_index != -1;
             current_object_index = hs_library_external_distance_list_next(object_list_index, &reference_index)) {
            real_point3d current_origin;
            object_get_origin(current_object_index, &current_origin);
            float distance = distance3d(&origin, &current_origin);
            result = result > distance ? distance : result;
        }
    }
    if (result == 3.4028234663852886e38f) result = -1.0f;
    return result;
}

// @address 0x2ba858 hs_objects_distance_to_flag
extern "C" float hs_objects_distance_to_flag(long object_list, short flag_index) {
    float result = 3.4028234663852886e38f;
    if (flag_index >= 0 && flag_index < global_scenario->cutscene_flags.count()) {
        scenario_cutscene_flag *flag = &global_scenario->cutscene_flags[flag_index];
        long iterator;
        long object = hs_flag_list_first(object_list, &iterator);
        if (object == -1)
            return -1.0f;
        float flag_x = flag->position.x;
        float flag_y = flag->position.y;
        float flag_z = flag->position.z;
        do {
            real_point3d origin;
            object_get_origin(object, &origin);
            float z = origin.z - flag_z;
            float x = origin.x - flag_x;
            float y = origin.y - flag_y;
            float distance = sqrtf(y*y + (x*x + z*z));
            if (result > distance)
                result = distance;
            object = hs_flag_list_next(object_list, &iterator);
        } while (object != -1);
        if (result != 3.4028234663852886e38f)
            return result;
    }
    return -1.0f;
}

// 0x2baca8
void hs_object_destroy_by_name(short name_index)
{
    if (name_index != -1 && name_index >= 0 && name_index < 0x800) {
        long object_index = tls_object_name_list()[name_index];
        if (object_index != -1 && !object_is_or_contains_player(object_index))
            object_delete(object_index);
    }
}

static __forceinline long hs_library_external_object_index_from_name_index(short name_index)
{
    long object_index = -1;
    if (name_index >= 0 && name_index < 0x800)
        object_index = tls_object_name_list()[name_index];
    return object_index;
}

static __forceinline void hs_library_external_object_destroy(long object_index)
{
    if (object_index != -1 && !object_is_or_contains_player(object_index))
        object_delete(object_index);
}

// 0x2ba9f8
void hs_object_create_anew(short name_index)
{
    if (name_index != -1) {
        hs_library_external_object_destroy(hs_library_external_object_index_from_name_index(name_index));
        object_new_by_name(name_index, false, false);
    }
}

void hs_object_create(short name_index)
{
    if (name_index != -1)
        object_new_by_name(name_index, false, false);
}

void hs_object_create_clone(short name_index)
{
    if (name_index != -1)
        object_new_by_name(name_index, true, false);
}

// 0x2bad20
void hs_object_destroy_all_type_mask(long type_mask)
{
    if (type_mask & 2) {
        c_data_iterator<player_datum> players;
        players.begin(tls_player_data());
        while (players.next()) {
            player_datum *player = players.get_datum();
            if (player->unit_index != -1 && inl_object_get_ultimate_parent(player->unit_index) != player->unit_index)
                action_submit_event(player->unit_index, 20);
        }
    }
    c_object_iterator<object_datum> objects;
    objects.begin(type_mask, 0);
    while (objects.next()) {
        object_datum *object = objects.get_datum();
        if (object->parent_object_index == -1 && !object_is_or_contains_player(objects.get_index()))
            object_delete(objects.get_index());
    }
}

void hs_object_iterate_names_containing(const char *substring,void (*callback)(short))
{
  scenario *scenario_data=global_scenario_get();
  for(short name_index=0;name_index<scenario_data->object_names.count;name_index++)
  {
    s_scenario_object_name *name = (s_scenario_object_name *)tag_block_get_element_with_size(
        &scenario_data->object_names, name_index, sizeof(s_scenario_object_name));
    if(strstr(name->name,substring)) callback(name_index);
  }
}

// @address 0x2bb210 hs_object_teleport_to_position

static __forceinline bool hs_library_external_object_test_flag(const unit_datum *object, long bit)
{
    return (object->object.flags & (1 << bit)) != 0;
}

static __forceinline real_vector3d *hs_library_external_transform_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    float x = vector->i;
    float y = vector->j;
    float z = vector->k;
    result->i = x * matrix->forward.i + y * matrix->left.i + z * matrix->up.i;
    result->j = x * matrix->forward.j + y * matrix->left.j + z * matrix->up.j;
    result->k = x * matrix->forward.k + y * matrix->left.k + z * matrix->up.k;
    return result;
}
extern "C" void hs_object_teleport_to_position(
    long object_index, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up, bool update_player, bool)
{
    if (object_index != -1)
    {
        unit_datum *object = (unit_datum *)object_get(object_index);
        if (update_player)
        {
            if (object->object.parent_object_index != -1)
                object_detach(object_index);
            else if (hs_library_external_object_test_flag(object, 7))
                object_set_in_limbo(object_index, false);
        }
        object_reset_after_teleport(object_index);
        object = (unit_datum *)object_get(object_index);
        if (hs_library_external_object_test_flag(object, 7) && hs_library_external_object_test_flag(object, 27))
        {
            object_set_in_limbo(object_index, false);
            object->object.flags &= ~0x8000000;
        }
        object = (unit_datum *)object_try_and_get_and_verify_type(object_index, 0x1003);
        if (object)
        {
            unit_datum *unit = (unit_datum *)object_try_and_get_and_verify_type(object_index, 0x1003);
            long player_index = -1;
            if (unit)
                player_index = unit->unit.player_index;
            real_vector3d facing;
            if (object->object.parent_object_index != -1)
            {
                unit_datum *parent =
                    (unit_datum *)object_get(object->object.parent_object_index);
                real_matrix4x3 *node_matrices = (real_matrix4x3 *)((char *)parent + parent->object.node_matrices_offset);
                real_matrix4x3 inverse;
                matrix4x3_inverse(node_matrices + object->object.parent_node_index, &inverse);
                hs_library_external_transform_vector(&inverse, forward, &facing);
            }
            else
            {
                facing = *forward;
            }
            object->unit.desired_facing_vector = *forward;
            object->unit.desired_aiming_vector = *forward;
            object->unit.desired_looking_vector = *forward;
            simulation_action_object_update(object_index, 0x10000);
            if (player_index != -1)
            {
                if (update_player)
                {
                    player_move_unit_to_position(player_index, -1, false, position);
                    update_player = false;
                }
                player_set_facing(player_index, &facing);
            }
        }
        object_set_position_direct(object_index, update_player ? position : 0, forward, up, 0, false);
    }
}

// @address 0x2bb498 hs_object_teleport
extern "C" void hs_object_teleport(long object_index, short flag_index)
{
    scenario_cutscene_flag *flag = &global_scenario->cutscene_flags[flag_index];
    real_vector3d forward;
    real_vector3d up;
    vector3d_from_euler_angles2d(&forward, &flag->facing);
    generate_up_vector3d(&forward, &up);
    hs_object_teleport_to_position(object_index, &flag->position, &forward, &up, true, true);
}

void hs_objects_predict(long object_list_index,bool low_priority)
{
  long reference_index;
  long object_index = -1;
  if (object_list_index != -1) {
      reference_index = datum_get<s_object_list_header>(tls_object_list_header_data(), object_list_index)->first_list_reference;
      object_index = object_list_get_first(object_list_index, &reference_index);
  }
  for (; object_index != -1;
      object_index = object_list_get_next(object_list_index, &reference_index))
  {

  }
}

// @address 0x2bb8f8 hs_effect_new_from_object_marker
extern "C" void hs_effect_new_from_object_marker(long effect_index, long object_index, long marker_name)
{
    object_marker markers;
    if (effect_index != -1 && object_index != -1 &&
        internal_object_get_markers_by_string_id(object_index, marker_name, &markers, 1, false) != 0) {
        long tag_storage;
        c_tag_index *tag = new (&tag_storage) c_tag_index(effect_index);
        ::effect_new_attached_from_marker_name(*tag, 0, 0, object_index, -1, marker_name, 1.0f, 1.0f,
            0, 0, 0, 1);
    }
}

// 0x2bbc98
void hs_damage_object_effect(long damage_effect, long object_index)
{
    if (!game_simulation::game_is_predicted() && object_index != -1 && damage_effect != -1 &&
        tag_get_group_tag(damage_effect) == 'jpt!') {
        s_damage_data damage;
        damage_data_new(&damage, damage_effect);
        object_get_origin(object_index, &damage.origin);
        damage.position = damage.origin;
        scenario_location_from_point(&damage.location, &damage.origin);
        object_cause_damage(&damage, object_index, -1, -1, -1, 0);
    }
}

// 0x2bbe88
void hs_damage_object(long object_index, long region_name, float damage_amount)
{
    if (!game_simulation::game_is_predicted()) {
        s_falling_damage_definition *globals =
            (s_falling_damage_definition *)tag_block_get_element_with_size(
                &global_game_globals->falling_damage, 0, sizeof(s_falling_damage_definition));
        if (globals->script_damage.index != -1 && object_index != -1) {
            object_datum *object = object_get(object_index);
            s_object_definition *definition = (s_object_definition *)tag_get(0, object->definition_index);
            if (definition->model.index != -1) {
                s_model_definition *model = (s_model_definition *)tag_get(0, definition->model.index);
                long region_count = model->materials.count;
                long region_index;
                for (region_index = 0; region_index < region_count; region_index++) {
                    s_model_region *region =
                        (s_model_region *)tag_block_get_element_with_size(
                            &model->materials, region_index, sizeof(s_model_region));
                    if (region->name == region_name) break;
                    else continue;
                }
                if (region_index < region_count) {
                    s_damage_data damage;
                    damage_data_new(&damage, globals->script_damage.index);
                    object_get_origin(object_index, &damage.origin);
                    damage.position = damage.origin;
                    damage.scale[2] = damage_amount;
                    scenario_location_from_point(&damage.location, &damage.origin);
                    object_cause_damage(&damage, object_index, -1, -1, (short)region_index, 0);
                }
            }
        }
    }
}

// @address 0x2bb998 ?hs_effect_new_on_ground@@YAXJJ@Z
void hs_effect_new_on_ground(long effect_index, long object_index)
{
    if (effect_index != -1 && object_index != -1) {
        real_point3d origin;
        object_get_origin(object_index, &origin);
        collision_result collision;
        s_collision_test_flags flags;
        flags.collision_flags = k_collision_test_structure_and_water.collision_flags | 0x20000 | 0x4000;
        flags.object_flags = k_collision_test_structure_and_water.object_flags;
        if (collision_test_vector(flags, false, &origin, global_down3d, -1, -1, -1, &collision)) {
            real_point3d effect_point;
            point_from_line3d(&collision.point, &collision.plane.normal, 0.01f, &effect_point);
            effect_new_unattached_from_point(effect_index, &effect_point, global_down3d, &collision.plane.normal, 0, 1, 0, 0);
        }
    }
}

// 0x2bbff8
void hs_damage_players(long damage_effect)
{
    if (!game_simulation::game_is_predicted()) {
        c_player_with_unit_iterator players;
        players.begin();
        while (players.next()) {
            player_datum *player = players.get_datum();
            if (player->unit_index != -1) {
                s_damage_data damage;
                damage_data_new(&damage, damage_effect);
                object_get_origin(player->unit_index, &damage.origin);
                damage.position = damage.origin;
                scenario_location_from_point(&damage.location, &damage.origin);
                object_cause_damage(&damage, player->unit_index, -1, -1, -1, 0);
            }
        }
    }
}

static __forceinline long hs_library_external_object_get_root_parent(long object_index)
{
    long current = object_index;
    long root;
    do {
        root = current;
        current = object_get(current)->parent_object_index;
    } while (current != -1);
    return root;
}

// @address 0x2baa90 hs_objects_create_by_name_in_table
extern "C" void hs_objects_create_by_name_in_table(int id, s_tag_block *table, s_tag_block *palette, long stride)
{
    for (short i = 0; i < table->count; i++) {
        s_scenario_object *entry = (s_scenario_object *)((char *)table->address + i * stride);
        if (entry->editor_folder != id)
            continue;
        s_object_globals *name_data = tls_object_runtime_globals();
        long first = name_data->first_name_index[entry->object_identifier.m_type];
        if (first == -1 || name_data->object_indices[i + first] == -1) {
            objects_garbage_collection();
            object_new_from_scenario_internal(entry->object_identifier.m_type, i, entry, palette, -1, true, false, false);
        }
    }
}

// @address 0x2bab68 hs_objects_create_by_name
extern "C" void hs_objects_create_by_name(int id)
{
    if (id != -1) {
        scenario *scenario_data = global_scenario;
        hs_objects_create_by_name_in_table(id, &scenario_data->crates, &scenario_data->crate_palette, 0xb0);
        hs_objects_create_by_name_in_table(id, &scenario_data->scenery, &scenario_data->scenery_palette, 0xb4);
        hs_objects_create_by_name_in_table(id, &scenario_data->bipeds, &scenario_data->biped_palette, 0x74);
        hs_objects_create_by_name_in_table(id, &scenario_data->vehicles, &scenario_data->vehicle_palette, 0xa8);
        hs_objects_create_by_name_in_table(id, &scenario_data->equipment, &scenario_data->equipment_palette, 0x8c);
        hs_objects_create_by_name_in_table(id, &scenario_data->weapons, &scenario_data->weapon_palette, 0xa8);
        hs_objects_create_by_name_in_table(id, &scenario_data->machines, &scenario_data->machine_palette, 0x70);
        hs_objects_create_by_name_in_table(id, &scenario_data->terminals, &scenario_data->terminal_palette, 0x60);
        hs_objects_create_by_name_in_table(id, &scenario_data->controls, &scenario_data->control_palette, 100);
        hs_objects_create_by_name_in_table(id, &scenario_data->sound_scenery, &scenario_data->sound_scenery_palette, 0x70);
        hs_objects_create_by_name_in_table(id, &scenario_data->giants, &scenario_data->giant_palette, 0x84);
        hs_objects_create_by_name_in_table(id, &scenario_data->effect_scenery, &scenario_data->effect_scenery_palette, 0x54);
        hs_objects_create_by_name_in_table(id, &scenario_data->light_volumes, &scenario_data->light_volume_palette, 0x8c);
    }
}

// 0x2bae68
extern "C" void hs_objects_delete_by_name_in_table(int id, s_tag_block *table, void *unused, long stride)
{
    if (table->count <= 0)
        return;
    for (short i = 0; i < table->count; i++) {
        s_scenario_object *entry = (s_scenario_object *)((char *)table->address + i * stride);
        if (entry->editor_folder != id)
            continue;
        signed char type = entry->object_identifier.m_type;
        s_object_globals *name_data = tls_object_runtime_globals();
        long first = name_data->first_name_index[type];
        if (first == -1)
            continue;
        long object_index = name_data->object_indices[i + first];
        if (object_index == -1)
            continue;
        long bits = 1 << type;
        bool keep = true;
        long masked = bits & 0x1003;
        if (masked != 0) {
            long root = hs_library_external_object_get_root_parent(object_index);
            c_data_iterator<player_datum> players;
            players.begin(tls_player_data());
            if (players.next()) {
                do {
                    player_datum *player = players.get_datum();
                    long unit_index = player->unit_index;
                    if (unit_index != -1) {
                        long unit_root = hs_library_external_object_get_root_parent(unit_index);
                        if (root == unit_root)
                            keep = false;
                    }
                } while (players.next());
            }
        } else if ((bits & 4) != 0) {
            item_datum *bit2_datum = (item_datum *)object_get(object_index);
            if (bit2_datum->owner_relation != 0 && bit2_datum->unit_object_index != -1)
                keep = false;
        } else if ((bits & 8) != 0) {
            if (((item_datum *)object_get(object_index))->owner_relation != 0)
                keep = false;
        }
        if (keep != 0)
            object_delete(object_index);
    }
}

const real k_hs_library_external_radians_per_degree = 0.017453292f;

static __forceinline float hs_library_external_angle_to_radians(float angle)
{
    return angle * k_hs_library_external_radians_per_degree;
}

// @address 0x2ba420 ?hs_objects_can_see_object@@YA_NJJM@Z
// 0x2ba420
bool hs_objects_can_see_object(long object_list_index, long target_object_index, float angle)
{
    bool result = false;
    long reference_index;
    long first_index = -1;
    if (object_list_index != -1) {
        long list_slot = object_list_index & 0xffff;
        reference_index = ((s_object_list_header *)(tls_object_list_header_data()->data + 12 * list_slot))->first_list_reference;
        first_index = object_list_get_first(object_list_index, &reference_index);
    }
    long object_index = first_index;
    if (first_index != -1) {
        long iterator = reference_index;
        while (object_index != -1) {
            if (object_try_and_get_and_verify_type(object_index, 0x1003)) {
                bool can_see = false;
                if (target_object_index != -1) {
                    real_point3d target_position;
                    if (object_try_and_get_and_verify_type(target_object_index, 0x1003))
                        unit_get_head_position(target_object_index, &target_position);
                    else
                        target_position = object_get(target_object_index)->bounding_center;
                    can_see = unit_can_see_point(object_index, &target_position, hs_library_external_angle_to_radians(angle));
                }
                if (can_see != 0) {
                    result = true;
                    break;
                }
            }
            long next_object_index;
            s_data_array *list_array = tls_list_object_reference_data();
            if (iterator != -1) {
                char *list_data = list_array->data;
                long list_stride = list_array->size;
                s_list_reference *datum = (s_list_reference *)(list_data + (unsigned short)iterator * list_stride);
                long next_index = datum->next_index;
                if (next_index != -1)
                    __dcbt(0, list_data + (unsigned short)next_index * list_stride);
                next_object_index = datum->value;
                iterator = next_index;
            }
            else {
                next_object_index = -1;
            }
            object_index = next_object_index;
        }
    }
    return result;
}

// @address 0x2ba5b0 ?hs_objects_can_see_flag@@YA_NJFM@Z
// 0x2ba5b0
bool hs_objects_can_see_flag(long object_list_index, short flag_index, float angle)
{
    bool result = false;
    long reference_index;
    long first_index = -1;
    if (object_list_index != -1) {
        long list_slot = object_list_index & 0xffff;
        reference_index = ((s_object_list_header *)(tls_object_list_header_data()->data + 12 * list_slot))->first_list_reference;
        first_index = object_list_get_first(object_list_index, &reference_index);
    }
    long object_index = first_index;
    if (first_index != -1) {
        long iterator = reference_index;
        scenario *scenario_data = global_scenario;
        while (object_index != -1) {
            if (object_try_and_get_and_verify_type(object_index, 0x1003)) {
                short flag = flag_index;
                bool can_see = false;
                if (flag >= 0 && flag < scenario_data->cutscene_flags.count()) {
                    can_see = unit_can_see_point(object_index, &scenario_data->cutscene_flags[flag].position, hs_library_external_angle_to_radians(angle));
                    scenario_data = global_scenario;
                }
                if (can_see != 0) {
                    result = true;
                    break;
                }
            }
            long next_object_index;
            s_data_array *list_array = tls_list_object_reference_data();
            if (iterator != -1) {
                char *list_data = list_array->data;
                long list_stride = list_array->size;
                s_list_reference *datum = (s_list_reference *)(list_data + (unsigned short)iterator * list_stride);
                long next_index = datum->next_index;
                if (next_index != -1)
                    __dcbt(0, list_data + (unsigned short)next_index * list_stride);
                next_object_index = datum->value;
                iterator = next_index;
            }
            else {
                next_object_index = -1;
            }
            object_index = next_object_index;
        }
    }
    return result;
}

// @address 0x2bb050 hs_objects_delete_by_name
// 0x2bb050
extern "C" void hs_objects_delete_by_name(int id)
{
    if (id != -1) {
        scenario *scenario_data = global_scenario;
        hs_objects_delete_by_name_in_table(
            id, &scenario_data->crates, &scenario_data->crate_palette, 0xb0);
        hs_objects_delete_by_name_in_table(id, &scenario_data->scenery, &scenario_data->scenery_palette, 0xb4);
        hs_objects_delete_by_name_in_table(id, &scenario_data->bipeds, &scenario_data->biped_palette, 0x74);
        hs_objects_delete_by_name_in_table(id, &scenario_data->vehicles, &scenario_data->vehicle_palette, 0xa8);
        hs_objects_delete_by_name_in_table(
            id, &scenario_data->equipment, &scenario_data->equipment_palette, 0x8c);
        hs_objects_delete_by_name_in_table(
            id, &scenario_data->weapons, &scenario_data->weapon_palette, 0xa8);
        hs_objects_delete_by_name_in_table(
            id, &scenario_data->machines, &scenario_data->machine_palette, 0x70);
        hs_objects_delete_by_name_in_table(
            id, &scenario_data->terminals, &scenario_data->terminal_palette, 0x60);
        hs_objects_delete_by_name_in_table(id, &scenario_data->controls, &scenario_data->control_palette, 100);
        hs_objects_delete_by_name_in_table(
            id, &scenario_data->sound_scenery, &scenario_data->sound_scenery_palette, 0x70);
        hs_objects_delete_by_name_in_table(
            id, &scenario_data->giants, &scenario_data->giant_palette, 0x84);
        hs_objects_delete_by_name_in_table(
            id, &scenario_data->effect_scenery, &scenario_data->effect_scenery_palette, 0x54);
        hs_objects_delete_by_name_in_table(
            id, &scenario_data->light_volumes, &scenario_data->light_volume_palette, 0x8c);
    }
}

// @address 0x2bbab0 ?hs_damage_new@@YAXJF@Z
void hs_damage_new(long p0, short p1)
{
    bool is_predicted = false;

    if (tls_game_globals()->options.game_simulation == 4) is_predicted = true;
    if (is_predicted == 0 && p0 != -1 && tag_get_group_tag(p0) == 'jpt!')
    {
        scenario_cutscene_flag *flag = &global_scenario->cutscene_flags[(short)p1];
        s_damage_data damage;
        damage_data_new(&damage, p0);
        damage.position = flag->position;
        damage.origin = flag->position;
        scenario_location_from_point(&damage.location, &flag->position);
        area_of_effect_cause_damage(&damage, -1, -1);
    }
}

// @address 0x2bb5c8 ?hs_effect_new@@YAXJF@Z
void hs_effect_new(long effect_index, short flag_index)
{
    if (effect_index != -1 && flag_index != -1) {
        unsigned long group_tag = tag_get_group_tag(effect_index);
        scenario_cutscene_flag *flag = &global_scenario->cutscene_flags[flag_index];
        real_vector3d forward;
        vector3d_from_euler_angles2d(&forward, &flag->facing);
        if (group_tag == 0x65666665) {
            effect_new_unattached_from_point(effect_index, &flag->position, &forward, global_up3d, 1, 1, 0, 0);
            return;
        }
        if (group_tag == 0x6a707421) {
            s_damage_data damage;
            damage_data_new(&damage, effect_index);
            damage.owner = *damage_owner_unknown();
            damage.scale[0] = 1.0f;
            scenario_location_from_point(&damage.location, &flag->position);
            damage.position = flag->position;
            damage.origin = damage.position;
            damage.direction = forward;
            damage.cone_direction = forward;
            area_of_effect_cause_damage(&damage, -1, -1);
        }
    }
}

// @address 0x2bb750 ?hs_effect_new_at_ai_point@@YAXJJ@Z
void hs_effect_new_at_ai_point(long effect_index, long point_reference)
{
    if (valid_point_ref(point_reference)) {
        long point_index = point_reference & 0xffff;
        cs_point_set *point_set = cs_get_point_set((point_reference >> 16) & 0xffff);
        cs_point *point = (cs_point *)tag_block_get_element_with_size(&point_set->points, point_index, sizeof(cs_point));
        unsigned long group_tag = tag_get_group_tag(effect_index);
        real_vector3d forward;
        real_point3d position;
        vector3d_from_euler_angles2d(&forward, &point->facing);
        ai_point_get_position(&point->position, &position);
        if (group_tag == 0x65666665) {
            effect_new_unattached_from_point(effect_index, &position, &forward, global_up3d, 1, 1, 0, 0);
            return;
        }
        if (group_tag == 0x6a707421) {
            s_damage_data damage;
            damage_data_new(&damage, effect_index);
            damage.owner = *damage_owner_unknown();
            damage.scale[0] = 1.0f;
            scenario_location_from_point(&damage.location, &position);
            damage.position = position;
            damage.origin = damage.position;
            damage.direction = forward;
            damage.cone_direction = forward;
            area_of_effect_cause_damage(&damage, -1, -1);
        }
    }
}

