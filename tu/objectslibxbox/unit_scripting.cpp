// objectslibxbox/unit_scripting.obj: from the records 0x2dd450 0x35a958 0x35a9c0 0x35aae8 0x35ac18 0x35ad08 0x35ae78 0x35af70 0x35b010 0x35b190 0x35b280 0x35b478 0x35b5e8 0x35b778 0x35b870 0x35bae0 0x35bbb0 0x35bc40
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "tag_files/tag_groups.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_iterator.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"
#include "units/unit_types.h"
#include "units/unit_seat.h"
#include <ppcintrinsics.h>
#include "motor/action_system.h"
#include "objects/objects.h"
#include "units/units.h"
#include "ai/ai_vehicles.h"
#include "hs/hs_runtime.h"
#include "hs/object_lists.h"
#include "objects/damage.h"
#include "units/unit_scripting.h"
#include "units/action_request.h"
#include "units/unit_seat_source.h"
#include "objects/damage_response_types.h"
#include "simulation/game_interface/simulation_game_damage_operations.h"
#include "objects/object_list_header_types.h"
#include "objects/list_reference_types.h"
#include "hs/hs_function.h"
#include "hs/hs_runtime_data.h"
#include "hs/hs_unit_seats.h"
#include "units/vehicle_types.h"
#include "hs/hs_value_types.h"
#include "game/player_mapping_types.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "hs/hs_function_definition.h"
#include <string.h>
#include "animations/animation_manager.h"
#include "units/vehicle_definitions.h"
#include "units/unit_seat_helpers.h"
#include "memory/data.h"
#include "hs/object_list_inlines.h"

struct s_unit_scripting_request : action_request {
    __forceinline s_unit_scripting_request() { memset(this, 0, sizeof(*this)); }
};
// @address 0x2dd450 unit_scripting_set_component_flag_8_evaluate
// @address 0x35a958 ?unit_scripting_set_maximum_vitality@@YAXJMM@Z
// @address 0x35a9c0 ?unit_scripting_list_set_maximum_vitality@@YAXJMM@Z
// @address 0x35ad08 ?unit_scripting_list_set_object_flag_40000@@YAXJ_N@Z
// @address 0x35ae78 ?unit_scripting_list_set_object_flag_8000@@YAXJ@Z
// @address 0x35af70 ?unit_scripting_set_component_flag_8@@YAXJ_N@Z
// @address 0x35b010 ?unit_scripting_start_user_animation_list@@YA_NJJJ_N@Z
// @address 0x35b190 ?unit_scripting_vehicle_test_seat@@YA_NJJJ@Z
// @address 0x35b280 ?unit_scripting_vehicle_test_seat_list@@YA_NJJJ@Z
// @address 0x35b478 ?unit_scripting_enter_vehicle@@YAXJJJ@Z
// @address 0x35b5e8 ?unit_scripting_exit_vehicle2@@YAXJF@Z

// @address 0x35b870 ?vehicle_scripting_load_magic@@YAFJJJ@Z
// @address 0x35bae0 ?unit_scripting_magic_melee_attack@@YAXXZ
// @address 0x35bbb0 ?unit_start_user_animation@@YA_NJJJ_N@Z
// @address 0x35bc40 ?unit_start_user_animation_relative@@YA_NJJJ_NJ@Z



namespace {

static __forceinline long unit_scripting_list_iterator_new(long list_index, long *iterator)
{
    long result = -1;
    if (list_index != -1) {
        *iterator = datum_get<s_object_list_header>(tls_object_list_header_data(), list_index)->first_list_reference;
        result = object_list_get_first(list_index, iterator);
    }
    return result;
}

}


namespace {

static __forceinline e_vehicle_type vehicle_definition_get_default_type(long definition_index)
{
    const vehicle_definition *definition =
        (const vehicle_definition *)tag_get('vehi', definition_index);
    e_vehicle_type result = _vehicle_type_none;
    for (long type_index = 0; type_index < 10; ++type_index)
    {
        if (definition->types[type_index].count != 0)
        {
            result = (e_vehicle_type)type_index;
            break;
        }
    }
    return result;
}
}



namespace {
static __forceinline bool object_is_damage_locked(const unit_datum *object)
{
    return (object->object.damage_flags >> 2) & 1;
}
}



// 0x2dd450
extern "C" void unit_scripting_set_component_flag_8_evaluate(short function_index, long thread_index, bool initialize)
{
    const hs_function_definition *definition = hs_function_table[function_index];
    hs_value *arguments = (hs_value *)hs_arguments_evaluate(thread_index, definition->formal_parameter_count, definition->formal_parameters, initialize);
    if (arguments) {
        unit_scripting_set_component_flag_8(arguments[0].long_value, arguments[1].boolean_value);
        hs_return(thread_index, 0);
    }
}

void unit_scripting_set_maximum_vitality(long object_index, float maximum_body, float maximum_shield)
{
    if (object_index != -1) {
        unit_datum *object = (unit_datum *)object_get_and_verify_type(object_index, ~0UL);
        if (!object_is_damage_locked(object))
            object_initialize_vitality(object_index, &maximum_body, &maximum_shield);
    }
}




void unit_scripting_list_set_maximum_vitality(long list_index, float maximum_body, float maximum_shield)
{
    long iterator;
    long reference;
    long object_index = unit_scripting_list_iterator_new(list_index, &iterator);
    if (object_index != -1) {
        float shield = maximum_shield;
        float body = maximum_body;
        reference = iterator;
        do {
            if (object_index != -1) {
                unit_datum *object = (unit_datum *)object_get_and_verify_type(object_index, ~0UL);
                if (!object_is_damage_locked(object))
                    object_initialize_vitality(object_index, &body, &shield);
            }
            object_index = object_list_get_next(list_index, &reference);
        } while (object_index != -1);
    }
}


// @address 0x35ac18 unit_scripting_list_set_current_vitality
extern "C" void unit_scripting_list_set_current_vitality(long list_index, float current_body, float current_shield)
{
    long iterator;
    long reference;
    long object_index = unit_scripting_list_iterator_new(list_index, &iterator);
    if (object_index != -1) {
        reference = iterator;
        do {
            unit_scripting_set_current_vitality(object_index, current_body, current_shield);
            object_index = object_list_get_next(list_index, &reference);
        } while (object_index != -1);
    }
}

void unit_scripting_list_set_object_flag_8000(long list_index)
{
    long iterator;
    long reference;
    long object_index = unit_scripting_list_iterator_new(list_index, &iterator);
    if (object_index != -1) {
        reference = iterator;
        do {
            unit_datum *object = (unit_datum *)object_try_and_get_and_verify_type(object_index, 0x1003);
            if (object)
                object->unit.unit_flags |= 0x8000;
            object_index = object_list_get_next(list_index, &reference);
        } while (object_index != -1);
    }
}

void unit_scripting_list_set_object_flag_40000(long list_index, bool set)
{
    long iterator;
    long reference;
    long object_index = unit_scripting_list_iterator_new(list_index, &iterator);
    if (object_index != -1) {
        reference = iterator;
        do {
            s_object_header *header = (s_object_header *)inl_datum_try_and_get(tls_object_header_data(), object_index);
            if (header && ((1U << header->object_type) & 0x1003) != 0) {
                unit_datum *object = (unit_datum *)header->datum;
                if (object) {
                    if (set)
                        object->unit.unit_flags |= 0x40000;
                    else
                        object->unit.unit_flags &= ~0x40000;
                }
            }
            object_index = object_list_get_next(list_index, &reference);
        } while (object_index != -1);
    }
}

void unit_scripting_set_component_flag_8(long object_index, bool set)
{
    if (object_index == -1)
        return;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, NONE);
    c_animation_manager *block = (c_animation_manager *)((char *)object + object->animation.offset);
    c_mixing_board_definition *layout = 0;
    if (block->m_mixing_board_type <= 2)
        layout = &g_mixing_board_definition_list[block->m_mixing_board_type];
    if (layout) {
        if (layout->component_types[4].count > 0) {
            unsigned char *data = (unsigned char *)block + layout->component_types[4].offset + 0xb0;
            if (data) {
                if (set)
                    data[0x5b] |= 8;
                else
                    data[0x5b] &= ~8;
            }
        }
    }
}

bool unit_scripting_start_user_animation_list(long list_index, long animation, long flags, bool relative)
{
    bool result = true;
    long iterator;
    long reference;
    long object_index = unit_scripting_list_iterator_new(list_index, &iterator);
    if (object_index != -1) {
        reference = iterator;
        do {
            s_object_header *header = (s_object_header *)inl_datum_try_and_get(tls_object_header_data(), object_index);
            void *object = 0;
            if (header && ((1U << header->object_type) & 0x1003) != 0)
                object = header->datum;
            if (object)
                result = result && unit_start_user_animation(object_index, animation, flags, relative);
            object_index = object_list_get_next(list_index, &reference);
        } while (object_index != -1);
    }
    return result;
}

bool unit_scripting_vehicle_test_seat(long vehicle_index, long seat_label, long unit_index)
{
 bool result = false;
 if (vehicle_index != -1 && unit_index != -1) {
  void *vehicle = object_try_and_get_and_verify_type(vehicle_index,0x1003);
  unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index,0x1003);
  if (vehicle) {
   unit_seat_source seats[64];
   short seat_count = 0;
   seat_count = unit_get_all_seats(vehicle_index,seats,64);
   for (long seat_index = 0; seat_index < seat_count; ++seat_index) {
    const unit_seat *seat = seats[seat_index].definition;
    if ((seat_label == 0 || seat_label == -1 || seat_label == seat->label) &&
     unit->object.parent_object_index == seats[seat_index].unit_index &&
     unit->unit.parent_seat_index == seats[seat_index].seat_index) {
     result = true;
     break;
    }
   }
  }
 }
 return result;
}


namespace {


static __forceinline long unit_scripting_player_unit_index_from_user_index(long user_index)
{
    long result = -1;
    if (user_index != -1)
        result = tls_player_mapping_globals()->player_controller_mapping[user_index];
    return result;
}
}

bool unit_scripting_vehicle_test_seat_list(long unit_index, long seat_label, long list_index)
{
    bool result = false;
    if (unit_index != -1) {
        unit_seat_source seats[64];
        short seat_count = unit_get_all_seats(unit_index, seats, 64);
        for (long seat_index = 0; seat_index < seat_count; ++seat_index) {
            long local_index = seats[seat_index].seat_index;
            long object_index = seats[seat_index].unit_index;
            const unit_seat *seat = seats[seat_index].definition;
            if (seat_label == 0 || seat_label == -1 || seat_label == seat->label) {
                c_object_iterator<unit_datum> iterator;
                bool found = false;
                iterator.begin(0x1003, 0);
                while (iterator.next()) {
                    if (iterator.get_datum()->object.parent_object_index == object_index && iterator.get_datum()->unit.parent_seat_index == local_index) {
                        found = true;
                        break;
                    }
                }
                if (found) {
                    long list_cursor;
                    long list_object = unit_scripting_list_iterator_new(list_index, &list_cursor);
                    while (list_object != -1 && iterator.get_index() != list_object)
                        list_object = object_list_get_next(list_index, &list_cursor);
                    if (iterator.get_index() == list_object)
                        result = true;
                }
            }
        }
    }
    return result;
}

void unit_scripting_enter_vehicle(long unit_index, long vehicle_index, long seat_label)
{
    if (unit_index == -1 || vehicle_index == -1 || seat_label == 0)
        return;
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, ~0UL);
    if (object_is_damage_locked(unit))
        return;
    if (unit->object.parent_object_index != -1 && (unsigned short)unit->unit.parent_seat_index != 0xffff)
        action_submit_event(unit_index, 20);
    if (unit->object.parent_object_index != -1)
        return;
    unit_seat_source seats[64];
    short seat_count = unit_get_all_seats(vehicle_index, seats, 64);
    for (long seat_index = 0; seat_index < seat_count; ++seat_index) {
        const unit_seat *seat = seats[seat_index].definition;
        unsigned short seat_local = seats[seat_index].seat_index;
        long seat_object = seats[seat_index].unit_index;
        if (seat_label == seat->label &&
            ::unit_seat_get_occupant(seat_object, seat_local) == -1 &&
            ::unit_has_seat_animation(unit_index, seat_object, seat_local)) {
            action_request request;
            memset(&request, 0, sizeof(request));
            request.type = 17;
            request.enter_vehicle.vehicle = seat_object;
            request.enter_vehicle.seat = seat_local;
            request.enter_vehicle.immediate = false;
            request.enter_vehicle.force = true;
            action_submit(unit_index, &request);
            return;
        }
    }
}

void unit_scripting_exit_vehicle2(long unit_index, short exit_type)
{
    if (unit_index == -1)
        return;
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, ~0UL);
    if (unit->object.parent_object_index == -1 || (unsigned short)unit->unit.parent_seat_index == 0xffff)
        return;
    if (exit_type == 0) {
        s_unit_scripting_request request;
        request.type = 13;
        request.exit_vehicle.exit = true;
        action_submit(unit_index, (const action_request *)&request);
        return;
    }
    if (exit_type == 1) {
        s_unit_scripting_request request;
        request.type = 15;
        request.exit_vehicle.immediate = false;
        action_submit(unit_index, (const action_request *)&request);
        return;
    }
    if (exit_type == 2) {
        s_unit_scripting_request request;
        request.type = 15;
        request.exit_vehicle.immediate = true;
        action_submit(unit_index, (const action_request *)&request);
        return;
    }
    if (exit_type == 3) {
        s_unit_scripting_request request;
        request.type = 13;
        request.exit_vehicle.exit = true;
        request.exit_vehicle.immediate = true;
        action_submit(unit_index, (const action_request *)&request);
    }
}

long unit_scripting_object_get_turret(long object_index, short turret_index)
{
    long result = -1;
    if (object_index != -1)
    {
        object_datum *object =
            (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
        short current_turret_index = 0;
        object_index = object->first_child_object_index;
        while (object_index != -1)
        {
            object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
            if (((1 << object->object_identifier.get_type()) & (1 << _object_type_vehicle)) != 0 &&
                vehicle_definition_get_default_type(object->definition_index) == _vehicle_type_turret)
            {
                if (turret_index == current_turret_index)
                {
                    result = object_index;
                    break;
                }
                else
                    ++current_turret_index;
            }
            object_index = object->next_object_index;
        }
    }
    return result;
}

static __forceinline long unit_scripting_magic_list_get_first(long list_index, long *iterator)
{
    long result = -1;
    if (list_index != -1) {
        *iterator = datum_get<s_object_list_header>(tls_object_list_header_data(), list_index)->first_list_reference;
        result = object_list_get_first(list_index, iterator);
    }
    return result;
}

static __forceinline long unit_scripting_magic_list_get_next(long *iterator)
{
    long result;
    s_data_array *array = tls_list_object_reference_data();
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

short vehicle_scripting_load_magic(long vehicle_index, long seats, long list_index)
{
    short count = 0;
    if (vehicle_index != -1) {
        unit_datum *vehicle = (unit_datum *)object_get_and_verify_type(vehicle_index, ~0UL);
        if (!object_is_damage_locked(vehicle)) {
            unit_seat_source sources[64];
            long source_count = ::hs_get_unit_seats(
                vehicle_index, seats, _vehicle_seat_desire_any, _unit_seat_occupancy_empty, sources, 64);
            if (source_count > 0) {
                long iterator;
                for (long unit_index = unit_scripting_magic_list_get_first(list_index, &iterator);
                     unit_index != -1;
                     unit_index = unit_scripting_magic_list_get_next(&iterator)) {
                    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, ~0UL);
                    if (((1 << unit->object.object_type) & 0x1003) != 0) {
                        for (long i = 0; i < source_count; i++) {
                            unit_seat_source *source = &sources[i];
                            long source_vehicle = source->unit_index;
                            if (source_vehicle != -1) {
                                short source_seat = source->seat_index;
                                if (::unit_has_seat_animation(unit_index, source_vehicle, source_seat)) {
                                    if (unit->object.parent_object_index != -1)
                                        action_submit_event(unit_index, 20);
                                    action_request request;
                                    memset(&request, 0, sizeof(request));
                                    request.type = 17;
                                    request.enter_vehicle.vehicle = source_vehicle;
                                    request.enter_vehicle.seat = source_seat;
                                    request.enter_vehicle.immediate = false;
                                    request.enter_vehicle.force = true;
                                    if (action_submit(unit_index, &request)) {
                                        long actor = unit->unit.actor;
                                        if (actor != -1) {
                                            long target_vehicle = source->unit_index;
                                            long root = ai_get_root_vehicle(target_vehicle);
                                            ::actor_attach_to_vehicle(actor, root, true);
                                            actor_attach_to_seat(actor, target_vehicle, source->seat_index, 0.0f, true);
                                        }
                                        source->unit_index = -1;
                                        count++;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return count;
}

void unit_scripting_magic_melee_attack()
{
    long user_index = -1;
    for (long index = 0; index < 4; ++index) {
        if (tls_player_mapping_globals()->player_user_mapping[index] != -1) {
            user_index = index;
            break;
        }
    }
    long unit_index = unit_scripting_player_unit_index_from_user_index(user_index);
    if (unit_index != -1) {
        s_unit_scripting_request request;
        memset(&request, 0, sizeof(request));
        request.type = 43;
        request.melee_ai.melee_type = 0;
        action_submit(unit_index, (const action_request *)&request);
    }
}

bool unit_start_user_animation(long unit_index, long animation, long flags, bool relative)
{
    if (unit_index != -1) {
        s_unit_scripting_request request;
        request.type = 1;
        request.play.graph = animation;
        request.play.animation = flags;
        request.play.custom = relative;
        request.play.object_index = -1;
        request.play.interpolate = false;
        request.play.use_position = false;
        request.play.loop = false;
        request.play.extra = false;
        return action_submit(unit_index, (const action_request *)&request);
    } else {
        return false;
    }
}

bool unit_start_user_animation_relative(long unit_index, long animation, long flags, bool relative, long relative_object_index)
{
    if (unit_index != -1) {
        s_unit_scripting_request request;
        request.type = 1;
        request.play.graph = animation;
        request.play.animation = flags;
        request.play.custom = relative;
        request.play.object_index = relative_object_index;
        request.play.interpolate = false;
        request.play.use_position = false;
        request.play.loop = false;
        request.play.extra = false;
        return action_submit(unit_index, (const action_request *)&request);
    } else {
        return false;
    }
}
// @address 0x35aae8 ?unit_scripting_set_current_vitality@@YAXJMM@Z
void unit_scripting_set_current_vitality(long unit_index, float body_vitality, float shield_vitality)
{
    if (unit_index != -1)
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, ~0UL);
        if (!object_is_damage_locked(unit))
        {
            const s_damage_owner *owner = damage_owner_unknown();
            float zero = 0.0f;
            float one = 1.0f;
            float shield_fraction;
            float body_fraction;
            if (!(unit->object.maximum_shield_vitality > zero))
                shield_fraction = zero;
            else if (!(shield_vitality < unit->object.maximum_shield_vitality))
                shield_fraction = one;
            else
                shield_fraction = shield_vitality / unit->object.maximum_shield_vitality;

            if (!(unit->object.maximum_body_vitality > zero))
                body_fraction = zero;
            else if (!(body_vitality < unit->object.maximum_body_vitality))
                body_fraction = one;
            else
                body_fraction = body_vitality / unit->object.maximum_body_vitality;

            object_set_shield_depleted(unit_index, shield_fraction <= zero, owner);
            float old_body_vitality = unit->object.body_vitality;
            unit->object.shield_vitality = shield_fraction;
            if (old_body_vitality > zero && !(body_fraction > zero))
            {
                s_destroy_routine_response response;
                memset(&response, 0, sizeof(response));
                object_kill(unit_index, 0, owner, &response, true, 0);
            }
            unit->object.body_vitality = body_fraction;
        }
    }
}
