// ailibxbox/actor_type_flood.obj: from the records 0x569b40 0x569bb8 0x569c60 0x569e00 0x569e60 0x569eb0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "ai/behavior_info.h"
#include "memory/data.h"
#include "memory/tls_globals.h"
#include "ai/actor_type_flood_types.h"
#include "units/biped_definitions.h"
#include "math/matrix_math.h"
#include "math/random_math_range.h"
#include "objects/object_placement.h"
#include "ai/actor_globals.h"
#include "ai/actor_datum.h"
#include "ai/prop_state.h"
#include "ai/behavior.h"
#include "ai/actors.h"
#include "objects/objects.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "ai/ai.h"
#include "ai/props.h"
#include "ai/actor_type_flood.h"
#include "ai/character_definitions.h"
#include "ai/styles.h"
#include "ai/prop_ref_datum.h"
#include "ai/clump_actor_cursor_types.h"
#include "ai/swarms.h"

// behavior_flood_eject_parasite's data after the layer header
class c_flood_eject_parasite_data : public c_behavior_state {
public:
    short ticks;
};
struct s_behavior_input;
struct s_behavior_arguments;

// @address 0x569b40 ?behavior_flood_eject_parasite_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_flood_eject_parasite_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments)
{
    short result = 0;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (actor->unit_index != -1)
    {
        // match: 0x569b40: volatile forces the second lwz of unit_index@0x14 for the header lookup; plain read reuses the compared register.
        volatile actor_datum &volatile_actor = *actor;
        s_object_header *header = object_header_get(volatile_actor.unit_index);
        object_datum *datum = header->datum;
        if (header->object_type == 0)
        {
            if (datum->inhibited_flags & 1)
            {
                if (datum->inhibited_flags & 2)
                    result = 3;
            }
        }
    }
    return result;
}

// @address 0x569bb8 ?behavior_flood_eject_parasite_activate@@YA_NJPAVc_behavior_state@@@Z
bool behavior_flood_eject_parasite_activate(long actor_index, c_behavior_state *behavior_state)
{
    c_flood_eject_parasite_data *eject_data = (c_flood_eject_parasite_data *)behavior_state;
    unsigned long *seed = tls_random_seed();
    random_seed_next(seed);
    float fraction = (float)(*seed >> 16) * 1.5259022e-05f;
    float scale = fraction * 2.0f + 2.0f;
    eject_data->ticks = (short)(long)(scale * (float)tls_game_time_globals()->ticks_per_second + 0.5f);
    return true;
}





static __forceinline void actor_type_flood_swarm_iterator_new(clump_actor_cursor *cursor)
{
    if (tls_actor_globals()->enabled)
    {
        cursor->iterator.absolute_index = -1;
        cursor->iterator.index = -1;
        cursor->index = -1;
        cursor->iterator.data = tls_swarm_data();
    }
}

// @address 0x569c60 actor_type_flood_handle_eject_parasite
extern "C" void actor_type_flood_handle_eject_parasite(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    long best_index = -1;
    object_datum *object = object_header_get(actor->unit_index)->datum;
    long effect = TAG_GET('obje', biped_definition, object->definition_index)->death_spawn_character.index;
    if (effect != -1)
    {
        character_general_properties *properties = character_definition_general_properties_get(effect);
        real_point3d origin;
        object_marker marker;
        if (internal_object_get_markers_by_string_id(actor->unit_index, 0x250, &marker, 1, false) > 0)
            origin = marker.matrix.position;
        else
            origin = actor->position.head;
        if (properties && (properties->flags & 1))
        {
            float best_distance_squared = 25.0f;
            clump_actor_cursor cursor;
            actor_type_flood_swarm_iterator_new(&cursor);
            swarm_datum *swarm = (swarm_datum *)clump_actor_cursor_next(&cursor);
            if (swarm)
            {
                real_point3d search_origin;
                search_origin.z = origin.z;
                search_origin.y = origin.y;
                search_origin.x = origin.x;
                do
                {
                real_vector3d v; vector_from_points3d(&swarm->position, &search_origin, &v); float distance_squared = v.j * v.j + v.k * v.k + v.i * v.i;
                if (distance_squared < best_distance_squared)
                {
                    best_index = cursor.index;
                    best_distance_squared = distance_squared;
                }
                } while ((swarm = (swarm_datum *)clump_actor_cursor_next(&cursor)) != 0);
            }
        }
        actor_new_child_from_object(actor->unit_index, best_index, effect, 1, 2, &origin);
    }
}


// @address 0x569e00 ?behavior_flood_eject_parasite_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_flood_eject_parasite_next(long actor_index, c_behavior_state *behavior_state, c_behavior_state *)
{
    c_flood_eject_parasite_data *eject_data = (c_flood_eject_parasite_data *)behavior_state;
    short result = -2;
    eject_data->ticks--;
    if (eject_data->ticks <= 0)
    {
        actor_type_flood_handle_eject_parasite(actor_index);
        actor_kill(actor_index, true, true);
        result = -1;
    }
    return result;
}

// @address 0x569e60 ?behavior_flood_eject_parasite_control@@YAXJPAVc_behavior_state@@@Z
void behavior_flood_eject_parasite_control(long actor_index, c_behavior_state *behavior_state)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    actor->blind_panic_control_active = true;
    actor->control_type = 137;
    actor->control_vector = actor->facing_vector;
}


// @address 0x569eb0 ?combatform_berserk_control_get_impulse@@YAFJPAVc_behavior_state@@@Z
short combatform_berserk_control_get_impulse(long actor_index, c_behavior_state *behavior_state)
{
    actor_datum *actor;
    long prop_index;
    character_combatform_properties *combatform;
    prop_ref_datum *prop;
    float chance;
    float roll;
    unsigned long seed;
    short attitude;
    style_definition *style;

    actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if ((short)actor_executing_layer(actor_index, 5) != -1)
        return -1;

    prop_index = actor->target_prop_index;
    if (prop_index != -1)
    {
        combatform = character_definition_combatform_multi_properties_get(actor_index);
        if (combatform != 0)
        {
            prop = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_index);
            if ((int)(((unsigned int)prop->flags_1e << 24) & 0xf0000000u) >= 0x20000000
                && prop->distance < combatform->berserk_distance
                && prop_state_get(prop)->object_index == -1)
            {
                actor_set_berserk(actor_index, true, false);
                return -1;
            }
            if (actor->uncovering != 0)
            {
                chance = ai_chance_this_tick(combatform->berserk_chance);
                seed = *tls_random_seed() * 0x19660du + 0x3c6ef35fu;
                *tls_random_seed() = seed;
                roll = (float)(double)(__int64)(unsigned int)(seed >> 16) * 1.5259022e-05f;
                if (!(roll >= chance))
                {
                    if (actor->clump_index == -1 || (unsigned char)clump_can_berserk(actor->clump_index, actor_index, actor->target_prop_index) != 0)
                    {
                        actor_set_berserk(actor_index, true, false);
                        return -1;
                    }
                }
            }
        }
    }

    attitude = 0;
    style = actor_get_task_style(actor_index);
    if (style != 0)
        attitude = style->attitude;
    if (attitude < 2)
        actor->throttle_close = true;
    return -1;
}

// @address 0x7c54e4 ?combatform_berserk_control_impulse@@3VBehavior_impulse@@A
Behavior_impulse combatform_berserk_control_impulse = Behavior_impulse(148, 0x37f007ff,
    combatform_berserk_control_get_impulse);

// @address 0x7c5498 ?flood_eject_parasite_behavior@@3VPrimitive_behavior@@A
Primitive_behavior flood_eject_parasite_behavior = Primitive_behavior(149, -1,
    behavior_flood_eject_parasite_get_desire, behavior_flood_eject_parasite_next,
    behavior_flood_eject_parasite_activate, 0, -1, 0, behavior_flood_eject_parasite_control);
