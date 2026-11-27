// ailibxbox/pathfinding_utilities.obj: from the records 0xae198 0xb19d8 0xb3180 0xb3248 0xb33d8 0xb35e0 0xc1188 0x569818
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "memory/data_types.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "cseries/cseries_wrapped.h"
#include "math/real_math_types.h"
#include "math/real_math_globals.h"
#include "math/matrix_math_types.h"
#include "game/game_time_types.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"
#include "objects/object_reference_types.h"
#include "ai/ai_point3d.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_datum.h"
#include "ai/sector_ref.h"
#include "ai/actors.h"
#include "objects/objects.h"
#include "ai/ai.h"
#include "units/vehicles.h"
#include "ai/ai_reference_frame.h"
#include "ai/pathfinding_utilities.h"
#include "physics/character_physics_component.h"
#include "units/bipeds.h"
#include "units/giants.h"
#include "units/biped_types.h"
#include "ai/actor_datum.h"
#include "creatures/creature_datum.h"
#include "ai/swarms.h"
#include "ai/sector_geometry.h"
#include "physics/collision_types.h"
// @address 0xae198 ?actor_pathfinding_available@@YA_NJ@Z
// @address 0xb33d8 ?actor_find_pathfinding_location@@YAXJ@Z
// @address 0xb35e0 ?object_pathfinding_available@@YA_NJ@Z
// @address 0xc1188 ?prop_find_pathfinding_location@@YAXJJ@Z

#define actor_data tls_actor_data()

static __forceinline void *pathfinding_datum_get(s_data_array *array, long index, long size)
{
    return array->data + (index & 0xffff) * size;
}
#define actor_get(actor_index) ((actor_datum *)pathfinding_datum_get(actor_data, (actor_index), sizeof(actor_datum)))
static __forceinline object_datum *pathfinding_object_get(long object_index, unsigned long type_mask)
{
    return (object_datum *)object_get_and_verify_type(object_index, type_mask);
}
static __forceinline e_object_type pathfinding_object_get_type(long object_index)
{
    return (e_object_type)object_header_get(object_index)->object_type;
}
static __forceinline c_character_physics_component::e_mode biped_get_movement_type(long biped_index)
{
    biped_datum *biped = (biped_datum *)pathfinding_object_get(biped_index, FLAG(_object_type_biped));
    return biped->biped.physics.get_mode();
}
static __forceinline bool pathfinding_motor_system_in_contact_with_ground(long object_index)
{
    object_datum *object = pathfinding_object_get(object_index, 0xffffffff);
    bool result = false;
    if (object->object_identifier.get_type() == _object_type_biped)
    {
        biped_datum *biped = (biped_datum *)pathfinding_object_get(object_index, FLAG(_object_type_biped));
        result = !TEST_BIT(biped->biped.physics.m_flags, 1);
    }
    return result;
}
static __forceinline bool biped_pathfinding_available(long biped_index)
{
    bool result = false;
    if (biped_get_movement_type(biped_index) == c_character_physics_component::_mode_ground)
        result = pathfinding_motor_system_in_contact_with_ground(biped_index);
    else
        result = true;
    return result;
}
static __forceinline long pathfinding_ultimate_parent_inline(long object_index)
{
    long result = NONE;
    while (object_index != NONE)
    {
        result = object_index;
        object_index = pathfinding_object_get(object_index, NONE)->parent_object_index;
    }
    return result;
}
static __forceinline bool creature_pathfinding_available(long creature_index)
{
    creature_datum *creature = (creature_datum *)pathfinding_object_get(creature_index, _object_mask_creature);
    bool result = false;
    if (creature->creature.physics.get_mode() == c_character_physics_component::_mode_ground)
        result = !creature->creature.m_flags.test(_creature_flag_7);
    else
        result = true;
    return result;
}

bool actor_pathfinding_available(long actor_index)
{
    actor_datum *actor = actor_get(actor_index);
    bool result = false;
    if (actor->use_object)
    {
        result = true;
    }
    else if (actor->root_vehicle_index != NONE)
    {
        result = true;
    }
    else if (actor->unit_index != NONE && pathfinding_object_get_type(actor->unit_index) == _object_type_biped)
    {
        result = biped_pathfinding_available(actor->unit_index);
    }
    else
    {
        result = true;
    }
    return result;
}

static __forceinline long pathfinding_game_time_get()
{
    return tls_game_time_globals()->time;
}

bool object_pathfinding_immediate(long object_index)
{
    bool result = false;
    object_index = pathfinding_ultimate_parent_inline(object_index);
    object_datum *object = pathfinding_object_get(object_index, NONE);
    switch (object->object_identifier.get_type())
    {
    case _object_type_biped:
        {
            biped_datum *biped = (biped_datum *)object;
            if (biped->biped.pathfinding_surface_index != NONE || biped->biped.pathfinding_time == pathfinding_game_time_get())
                result = true;
        }
        break;
    case _object_type_creature:
        result = creature_pathfinding_available(object_index);
        break;
    default:
        result = false;
        break;
    }
    return result;
}


void actor_find_pathfinding_location(long actor_index)
{
    actor_datum *actor = actor_get(actor_index);
    if (!actor->sector_valid)
    {
        actor->sector_valid = true;
        actor->pathfinding_point.set(&actor->position.body);
        actor->pathfinding_sector.clear();
        if (actor->use_object)
        {
            if (actor->root_vehicle_index != NONE)
                ::ai_get_center_of_mass(actor->root_vehicle_index, &actor->pathfinding_point.point);
            else
                ::ai_get_center_of_mass(actor->unit_index, &actor->pathfinding_point.point);
        }
        else if (actor->root_vehicle_index != NONE)
            object_get_pathfinding_location(actor->root_vehicle_index, &actor->pathfinding_sector, &actor->pathfinding_point);
        else if (actor->unit_index != NONE)
        {
            if (actor_index == g_updating_actor_index && pathfinding_object_get_type(actor->unit_index) == _object_type_giant)
            {
                ::object_get_giant_pathfinding_location(actor->unit_index, &actor->pathfinding_sector, &actor->pathfinding_point);
                if (!actor->pathfinding_sector.valid() && actor->command_script_index != NONE && actor->dialogue_sector.valid())
                {
                    actor->pathfinding_sector = actor->dialogue_sector;
                    actor->pathfinding_point = actor->dialogue_point;
                }
            }
            else if (object_pathfinding_available(actor->unit_index))
                object_get_pathfinding_location(actor->unit_index, &actor->pathfinding_sector, &actor->pathfinding_point);
        }
        if (actor->pathfinding_sector.valid())
        {
            actor->dialogue_sector = actor->pathfinding_sector;
            actor->dialogue_point = actor->pathfinding_point;
            actor->pathfinding_location_invalid = false;
        }
        else
            actor->pathfinding_location_invalid = true;
    }
}

bool object_pathfinding_available(long object_index)
{
    bool result = true;
    object_index = pathfinding_ultimate_parent_inline(object_index);
    object_datum *object = pathfinding_object_get(object_index, NONE);
    switch (object->object_identifier.get_type())
    {
    case _object_type_biped:
        result = biped_pathfinding_available(object_index);
        break;
    case _object_type_creature:
        result = creature_pathfinding_available(object_index);
        break;
    }
    return result;
}

static __forceinline prop_state *pathfinding_prop_state_get(const prop_ref_datum *ref)
{
    if (ref->view_index != NONE)
        return &((tracking_datum *)pathfinding_datum_get(tls_prop_data(), ref->view_index, sizeof(tracking_datum)))->state;
    return &((prop_datum *)pathfinding_datum_get(tls_ai_prop_data(), ref->prop_index, sizeof(prop_datum)))->state;
}

static __forceinline swarm_creature_state *pathfinding_creature_get_swarm_state(long creature_index)
{
    creature_datum *creature = (creature_datum *)pathfinding_object_get(creature_index, _object_mask_creature);
    swarm_creature_state *state = 0;
    if (creature->creature.link_type == 0)
        state = (swarm_creature_state *)object_header_block_get(creature, &creature->creature.creature_reference);
    return state;
}

static __forceinline long pathfinding_actor_index_from_object(long object_index)
{
    long actor_index = NONE;
    object_datum *object = pathfinding_object_get(object_index, NONE);
    if (TEST_BIT(0x1003, object->object_identifier.get_type()))
    {
        actor_index = ((unit_datum *)object)->unit.actor_index;
    }
    else if (object->object_identifier.get_type() == _object_type_creature)
    {
        swarm_creature_state *state = pathfinding_creature_get_swarm_state(object_index);
        if (state != 0)
            actor_index = state->swarm_index;
    }
    return actor_index;
}

// @address 0xb3248 ?prop_find_pathfinding_location_internal@@YAXJJPAUprop_state@@@Z
void prop_find_pathfinding_location_internal(long actor_index, long object_index, prop_state *state)
{
    if (actor_index != NONE && actor_is_giant(actor_get(actor_index)))
    {
        object_get_giant_pathfinding_location(object_index, &state->pathfinding_sector, &state->pathfinding_point);
        state->valid = true;
    }
    else
    {
        long object_actor_index = pathfinding_actor_index_from_object(object_index);
        actor_datum *actor = 0;
        if (object_actor_index != NONE)
            actor = actor_get(object_actor_index);
        if (actor != 0 && !actor_is_giant(actor) && actor->swarm_index == NONE)
        {
            actor_find_pathfinding_location(object_actor_index);
            state->pathfinding_sector = actor->pathfinding_sector;
            state->pathfinding_point = actor->pathfinding_point;
        }
        else if (state->object_index != NONE)
        {
            object_get_pathfinding_location(state->object_index, &state->pathfinding_sector, &state->pathfinding_point);
        }
        else
        {
            object_get_pathfinding_location(object_index, &state->pathfinding_sector, &state->pathfinding_point);
        }
    }
}
void prop_find_pathfinding_location(long actor_index, long ref_index)
{
    prop_ref_datum *ref = (prop_ref_datum *)pathfinding_datum_get(tls_prop_ref_data(), ref_index, sizeof(prop_ref_datum));
    prop_state *state = pathfinding_prop_state_get(ref);
    if (ref->status >= 1 && ref->status <= 2 && (object_pathfinding_available(ref->object_index) || !state->valid))
    {
        prop_find_pathfinding_location_internal(actor_index, ref->object_index, state);
        state->valid = true;
    }
    else if (!state->valid)
    {
        if (actor_is_giant(actor_get(actor_index)))
        {
            state->pathfinding_sector = giant_sector_from_point(&state->position);
            state->pathfinding_point.set(&state->position);
        }
        else
        {
            collision_result collision;
            state->pathfinding_sector = pathfinding_find_ground_sector(&state->position, global_up3d, &state->pathfinding_point, &collision, 0.5f);
        }
        state->valid = true;
    }
}


static __forceinline bool pathfinding_unit_is_attached(long object_index)
{
    return TEST_BIT(((unit_datum *)pathfinding_object_get(object_index, 0x1003))->unit.flags400, 0);
}
static __forceinline c_sector_ref pathfinding_unit_get_attached_sector(long object_index)
{
    c_sector_ref result;
    result.clear();
    unit_datum *unit = (unit_datum *)pathfinding_object_get(object_index, 0x1003);
    if (TEST_BIT(unit->unit.flags400, 0))
        result = unit->unit.pathfinding_attached_sector;
    return result;
}

static __forceinline c_sector_ref pathfinding_sector_from_point(const real_point3d *position, c_ai_point3d *point)
{
    collision_result collision;
    return pathfinding_find_ground_sector(position, global_up3d, point, &collision, 0.5f);
}

// @address 0xb19d8 ?object_get_pathfinding_location@@YAXJPAVc_sector_ref@@PAVc_ai_point3d@@@Z
void object_get_pathfinding_location(long object_index, c_sector_ref *sector, c_ai_point3d *point)
{
    long parent = pathfinding_ultimate_parent_inline(object_index);
    object_datum *object = pathfinding_object_get(parent, NONE);
    short type = (short)object->object_identifier.get_type();
    c_sector_ref location;
    location.clear();
    if (TEST_BIT(0x1003, type) && pathfinding_unit_is_attached(parent)) {
        if (point) {
            real_point3d origin;
            object_get_origin(parent, &origin);
            point->set(&origin);
        }
        location = pathfinding_unit_get_attached_sector(parent);
    } else {
        long reference_object = NONE;
        unsigned long flags = 0xffffffff;
        real_point3d position;
        if (type == _object_type_giant) {
            parent = object_index;
            type = (short)pathfinding_object_get_type(object_index);
            object = pathfinding_object_get(object_index, NONE);
        }
        switch (type) {
        case _object_type_biped:
            biped_find_pathfinding_location(parent, 0, 0, &location, &position, &reference_object, &flags);
            if (point) {
                if (location != NONE)
                    ai_point_set_from_world_position(&position, reference_object, flags, point);
                else
                    point->set(&position);
            }
            break;
        case _object_type_vehicle:
            vehicle_find_pathfinding_location(parent, 0, 0, &location, &position, &reference_object, &flags);
            if (point) {
                if (location != NONE)
                    ai_point_set_from_world_position(&position, reference_object, flags, point);
                else
                    point->set(&position);
            }
            break;
        case _object_type_giant:
            if (sector) {
                giant_find_pathfinding_location(parent, 0, 0, &location, &position, &reference_object, &flags);
            } else if (point) {
                object_get_origin(parent, &position);
                point->set(&position);
            }
            break;
        default:
            position = object->bounding_center;
            location = pathfinding_sector_from_point(&position, point);
            break;
        }
    }
    if (sector)
        *sector = location;
}

// @address 0x569818 ?object_get_giant_pathfinding_location@@YAXJPAVc_sector_ref@@PAVc_ai_point3d@@@Z
void object_get_giant_pathfinding_location(long object_index, c_sector_ref *sector, c_ai_point3d *point)
{
    long parent = pathfinding_ultimate_parent_inline(object_index);
    object_datum *object = pathfinding_object_get(parent, NONE);
    e_object_type type = object->object_identifier.get_type();
    c_sector_ref location;
    location.clear();
    if (((1 << type) & 0x1003) != 0 && pathfinding_unit_is_attached(parent))
    {
        if (point)
        {
            real_point3d origin;
            object_get_origin(parent, &origin);
            point->set(&origin);
        }
        location = pathfinding_unit_get_attached_sector(parent);
    }
    else if (point)
    {
        object_datum *original = pathfinding_object_get(object_index, NONE);
        point->set(&original->bounding_center);
        if (sector)
            location = giant_sector_from_point(&point->point);
    }
    if (sector)
        *sector = location;
}
