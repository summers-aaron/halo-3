// objectslibxbox/device_machines.obj: from the records 0xec348 0x348868 0x348a60 0x348a88 0x348b28 0x348ca0 0x348ed0
#include "sound/game_sound_globals.h"
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "visibility/visibility_portal_activation_types.h"
#include "memory/tls_globals.h"
#include "devices/device_animation_control.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/real_math_constants.h"
#include "cseries/cseries_wrapped.h"
#include "memory/data.h"
#include "tag_files/tag_types.h"
#include "animations/animation_id.h"
#include "objects/object_datum.h"
#include <math.h>
#include "devices/device_datum.h"
#include "devices/device_definitions.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "scenario/scenario_definitions.h"
#include "structures/structure_bsp_tag.h"
#include "structures/structure_cluster_portal.h"
#include "units/unit_types.h"
#include "units/unit_definitions.h"
#include "units/vehicle_definitions.h"
#include "objects/objects.h"
#include "objects/object_animation_manager_inline.h"
#include "cseries/bit_test.h"
#include "ai/ai_reference_frame.h"
#include "devices/device_machines.h"
#include "devices/devices.h"
#include "devices/device_group.h"
#include "physics/havok_utilities.h"
#include "sound/game_sound.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "structures/cluster_bit_vectors.h"
#include "animations/animation_manager.h"
#include "objects/object_placement.h"
#include "scenario/scenario_tag.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "devices/machine_datum.h"
#include "scenario/scenario_objects.h"
#include "objects/object_type_access.h"
#include "structures/structure_portal_reference_types.h"
// @address 0x348868 ?machine_place@@YAXJPAUs_scenario_machine@@@Z
// @address 0x348ca0 ?machine_door_test_object@@YA_NJPAJF@Z
struct s_animation_event_data;

typedef char machine_datum_flags_offset[(offsetof(s_machine_datum, machine_flags) == 0x228) ? 1 : -1];
typedef char machine_datum_pathfinding_offset[(offsetof(s_machine_datum, pathfinding_object_index) == 0x232) ? 1 : -1];
typedef char machine_definition_policy_offset[(offsetof(s_machine_definition, pathfinding_policy) == 0x1a4) ? 1 : -1];
namespace
{
    static __forceinline bool machine_definition_flag_test(const s_machine_definition *definition, long bit)
    {
        bool result = (bool)((definition->flags >> bit) & 1);
        return result;
    }
    static __forceinline const structure_bsp *structure_bsp_get(long bsp_index)
    {
        return (const structure_bsp *)tag_get('sbsp', TAG_BLOCK_GET_ELEMENT(
            &global_scenario->structure_bsps, bsp_index, scenario_structure_bsp_reference)->structure_bsp.index);
    }
    static __forceinline s_portal_reference device_machines_door_portal_reference_get(long zone_set_index, long bsp_index, long portal_index)
    {
        s_portal_reference result;
        short bsp_part = (short)(bsp_index << 10);
        result.value = (short)((*TAG_BLOCK_GET_ELEMENT(
            &TAG_BLOCK_GET_ELEMENT(
                &TAG_BLOCK_GET_ELEMENT(&global_scenario->zone_set_pvs,
                    TAG_BLOCK_GET_ELEMENT(&global_scenario->zone_sets, zone_set_index, s_scenario_zone_set)->pvs_index,
                    s_scenario_zone_set_visibility)->machine_door_mappings,
                bsp_index, s_scenario_bsp_machine_door_mapping)->machine_door_portal_references,
            portal_index, short) &
                                   1023) |
            bsp_part);
        return result;
    }
    static __forceinline long device_machines_portal_index(s_portal_reference reference)
    {
        return ((long)reference.value << 22) >> 22;
    }
    static __forceinline bool device_machines_cluster_test(const s_game_cluster_bit_vectors *vectors, long bsp_index, long cluster)
    {
        return (vectors->bit_vectors[bsp_index].m_storage[cluster >> 5] & (1 << (cluster & 31))) != 0;
    }
    static __forceinline s_device_group_datum *device_machines_group_get(long group_index)
    {
        return datum_get<s_device_group_datum>(g_device_groups_data, group_index);
    }
    static __forceinline long device_machines_portal_bsp_index(s_portal_reference reference)
    {
        return ((long)reference.value << 16) >> 26;
    }
    static __forceinline void device_machines_portal_activation_set_active(s_portal_reference reference, bool active)
    {
        long portal_index = device_machines_portal_index(reference);
        unsigned long *words = &g_active_portal_bitvectors[device_machines_portal_bsp_index(reference) * 16];
        if (active)
            words[portal_index >> 5] |= 1UL << (portal_index & 31);
        else
            words[portal_index >> 5] &= ~(1UL << (portal_index & 31));
    }
    static __forceinline void device_machines_set_door_occlusion(long bsp_index, long door_index, float occlusion)
    {
        if (bsp_index != -1)
        {
            long quantized_occlusion = quantize_real(occlusion, 0.0f, 1.0f, 16, false, false);
            long occlusion_index = game_sound_compute_group_item_offset(bsp_index, door_index);
            g_game_sound_globals->occlusion[occlusion_index] = (unsigned short)quantized_occlusion;
        }
    }
}


static __forceinline void machine_align_rigid_bodies_to_node_positions(long object_index)
{
    s_machine_datum *machine = (s_machine_datum *)object_get_and_verify_type(object_index, 128);
    havok_object_keyframed_rigid_bodies_to_node_transforms(object_index, true);
    machine->machine_flags.set(_machine_flag_6, false);
}

// @address 0xec348 ?machine_update@@YA_NJ@Z
bool machine_update(long object_index)
{
    bool result = false;
    s_machine_datum *machine = (s_machine_datum *)object_get_and_verify_type(object_index, 128);
    const s_machine_definition *definition = (const s_machine_definition *)tag_get('mach', machine->definition_index);
    c_animation_manager *manager = 0;
    if (object_has_animation_manager(object_index))
        manager = (c_animation_manager *)object_header_block_get(machine, &machine->animation);

    machine->device_flags &= ~0x100UL;
    float zero = 0.0f;
    float one = 1.0f;
    if ((machine->device_flags & 8) != 0)
    {
        if ((machine->device_flags & 0x20) == 0)
        {
            float previous_position = machine->device_position;
            float playback_ratio;
            bool finished = device_update_animation_control(object_index, &machine->position_control, manager,
                &machine->position_channel, previous_position, &playback_ratio);
            machine->device_position = playback_ratio;
            machine->position_velocity = (playback_ratio - previous_position) / tls_game_time_globals()->tick_length;
            if (finished)
                machine->device_flags |= 0x20;
            else
                machine->device_flags &= ~0x20UL;
            device_set_animation_changed(object_index);
            result = true;
        }
    }
    else if (machine->position_group_index != -1)
    {
        s_device_group_datum *group = device_machines_group_get(machine->position_group_index);
        if (group->actual_value != machine->device_position || machine->position_velocity != zero
            || (machine->device_flags & 0x80) != 0)
        {
            if (manager != 0 && machine->position_channel.valid())
            {
                float playback_ratio = machine->device_position;
                if ((machine->device_flags & 1) != 0)
                    playback_ratio = one - playback_ratio;
                machine->position_channel.set_frame_position_from_playback_ratio(playback_ratio);
            }

            float power = machine->power;
            float inverse_power = one - power;
            float acceleration = definition->depowered_position_acceleration_time * inverse_power
                + definition->position_acceleration_time * power;
            float maximum_velocity = definition->depowered_position_transition_time * inverse_power
                + definition->position_transition_time * power;
            bool opening = machine->position_velocity > zero;
            if (acceleration > zero && maximum_velocity > zero)
                machine->device_flags |= 0x100;
            else
                machine->device_flags &= ~0x100UL;

            if ((float)machine->delay_ticks * tls_game_time_globals()->tick_length >= definition->delay_time
                || machine->device_position != zero || group->actual_value < machine->device_position)
            {
                float old_velocity = machine->position_velocity;
                float previous_position = machine->device_position;
                if (fabsf(old_velocity) > maximum_velocity)
                    machine->position_velocity = opening ? maximum_velocity : -maximum_velocity;
                if (real_update_smooth_value(&machine->device_position, &machine->position_velocity, group->actual_value,
                        tls_game_time_globals()->tick_length, acceleration, maximum_velocity, zero, one,
                        (definition->device_flags & 1) != 0))
                {
                    device_effect_new(object_index, opening ? definition->opened.index : definition->closed.index);
                }
                else
                {
                    if (machine->position_velocity != zero && old_velocity * machine->position_velocity <= zero)
                        device_effect_new(object_index, machine->position_velocity > old_velocity
                            ? definition->open_up.index : definition->close_down.index);
                }
                if (previous_position != machine->device_position || (machine->device_flags & 0x80) != 0)
                {
                    device_set_animation_changed(object_index);
                    if (manager != 0 && machine->position_channel.valid())
                    {
                        float playback_ratio;
                        if ((machine->device_flags & 1) != 0)
                            playback_ratio = one - machine->device_position;
                        else
                            playback_ratio = machine->device_position;
                        machine->position_channel.update_to_playback_ratio(playback_ratio, manager,
                            object_animation_callback, object_index);
                    }
                }
            }
            else
            {
                machine->delay_ticks = (short)(machine->delay_ticks + 1);
                if (machine->delay_ticks == 1)
                    device_effect_new(object_index, definition->delay_effect.index);
            }
            result = true;
        }
        else
        {
            machine->delay_ticks = 0;
        }
    }

    if ((machine->device_flags & 0x10) != 0)
    {
        if ((machine->device_flags & 0x40) == 0)
        {
            bool finished = device_update_animation_control(object_index, &machine->power_and_overlay_control, manager,
                &machine->power_and_overlay_track_channel, zero, 0);
            if (finished)
                machine->device_flags |= 0x40;
            else
                machine->device_flags &= ~0x40UL;
            device_set_animation_changed(object_index);
            result = true;
        }
    }
    else if ((machine->device_flags & 8) == 0 && machine->power_group_index != -1)
    {
        s_device_group_datum *group = device_machines_group_get(machine->power_group_index);
        if (group->actual_value != machine->power || machine->power_velocity != zero)
        {
            if (manager != 0 && machine->power_and_overlay_track_channel.valid())
                machine->power_and_overlay_track_channel.set_frame_position_from_playback_ratio(machine->power);

            float previous_power = machine->power;
            real_update_smooth_value(&machine->power, &machine->power_velocity, group->actual_value,
                tls_game_time_globals()->tick_length, definition->power_acceleration_time, definition->power_transition_time,
                zero, one, false);
            if (previous_power != machine->power)
            {
                device_set_animation_changed(object_index);
                if (manager != 0 && machine->power_and_overlay_track_channel.valid())
                    machine->power_and_overlay_track_channel.update_to_playback_ratio(machine->power, manager,
                        object_animation_callback, object_index);
            }
            result = true;
        }
    }

    if ((machine->device_flags & 8) != 0 || machine->position_group_index != -1)
    {
        if ((machine->device_flags & 0x80) != 0)
        {
            if (((1 << object_get_type(object_index)) & 0x80) != 0)
            {
                object_compute_node_matrices(object_index);
                machine_align_rigid_bodies_to_node_positions(object_index);
            }
            result = true;
            machine->device_flags &= ~0x80UL;
        }
    }
    return result;
}

void machine_place(long object_index, s_scenario_machine *scenario_machine)
{
    s_machine_datum *machine =
        (s_machine_datum *)object_get_and_verify_type(object_index, 128);
    if ((scenario_machine->flags & 1) != 0)
        machine->machine_flags.set(_machine_flag_0, true);
    if ((scenario_machine->flags & 32) != 0)
        machine->machine_flags.set(_machine_flag_1, true);
    if ((scenario_machine->flags & 2) != 0)
        machine->machine_flags.set(_machine_flag_2, true);
    if ((scenario_machine->flags & 16) != 0)
        machine->machine_flags.set(_machine_flag_3, true);
    if ((scenario_machine->flags & 4) != 0)
        machine->machine_flags.set(_machine_flag_4, true);
    if ((scenario_machine->flags & 8) != 0)
        machine->machine_flags.set(_machine_flag_5, true);
    if ((scenario_machine->flags & 64) != 0)
        machine->machine_flags.set(_machine_flag_7, true);
    machine->object_flag_bits().set(_object_data_flag_22, false);
    machine->object_flag_bits().set(_object_data_flag_23, false);
    long definition_index = machine->definition_index;
    short policy = 3;
    switch (scenario_machine->pathfinding_policy)
    {
    case 0:
        if (definition_index != -1)
        {
            const s_machine_definition *definition =
                (const s_machine_definition *)tag_get('mach', definition_index);
            switch (definition->pathfinding_policy)
            {
            case 0:
                policy = 2;
                break;
            case 1:
                policy = 1;
                break;
            case 2:
                policy = 0;
                break;
            }
        }
        break;
    case 1:
        policy = 2;
        break;
    case 2:
        policy = 1;
        break;
    case 3:
        policy = 0;
        break;
    }
    switch (policy)
    {
    case 0:
    case 3:
        machine->object_flag_bits().set(_object_data_flag_23, true);
        break;
    case 1:
        machine->object_flag_bits().set(_object_data_flag_22, true);
        object_refresh_reference_frames(object_index);
        machine->object_flag_bits().set(_object_data_flag_23, true);
        break;
    }
    object_update_collision_culling(object_index);
}

// @address 0x348a60 ?machine_unplace@@YAXJ@Z
void machine_unplace(long object_index)
{
    s_machine_datum *machine = (s_machine_datum *)object_get_and_verify_type(object_index, 128);
    machine->machine_flags.m_storage = 0;
}

// @address 0x348a88 ?machine_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool machine_new(long object_index, object_placement_data *placement, bool *out_of_memory)
{
    s_machine_datum *machine =
        (s_machine_datum *)object_get_and_verify_type(object_index, 128);
    const s_machine_definition *definition =
        (const s_machine_definition *)tag_get('mach', machine->definition_index);
    machine->object_flag_bits().set(_object_data_flag_12, machine_definition_flag_test(definition, 2));
    machine->object_flag_bits().set(_object_data_flag_13, machine_definition_flag_test(definition, 2));
    machine->pathfinding_object_index = -1;
    machine->pathfinding_count = 0;
    machine->pathfinding_bsp_index = -1;
    return true;
}

// @address 0x348b28 ?machine_delete@@YAXJ@Z
void machine_delete(long object_index)
{
    s_machine_datum *machine = (s_machine_datum *)object_get_and_verify_type(object_index, 128);
    if (machine->pathfinding_count > 0)
    {
        for (long door_index = machine->pathfinding_object_index;
             door_index < machine->pathfinding_object_index + machine->pathfinding_count; door_index++)
        {
            s_portal_reference portal = device_machines_door_portal_reference_get(
                global_zone_set_index, machine->pathfinding_bsp_index, door_index);
            device_machines_portal_activation_set_active(portal, true);
            device_machines_set_door_occlusion(machine->pathfinding_bsp_index, door_index, 0.0f);
        }
    }
}

bool machine_door_test_object(long machine_index, long *objects, short count)
{
    s_machine_datum *machine = (s_machine_datum *)object_get_and_verify_type(machine_index, 128);
    bool result = false;
    if ((machine->machine_flags.m_storage & 1) == 0)
    {
        for (long i = 0; i < count; i++)
        {
            long index = objects[i];
            unit_datum *obj = (unit_datum *)object_get_and_verify_type(index, -1);
            if (((1 << obj->object.identifier_type) & 4099) != 0)
            {
                unit_datum *unit = obj;
                unit_definition *definition = (unit_definition *)tag_get('unit', unit->object.definition_index);
                bool opens = true;
                if (((1 << object_get_type(index)) & 2) != 0)
                {
                    vehicle_definition *vehicle = (vehicle_definition *)tag_get('vehi', unit->object.definition_index);
                    if (!test_bit(vehicle->flags, 1))
                        opens = false;
                }
                if ((opens && test_bit(unit->object.damage_flags, 2)) || test_bit(definition->flags, 14))
                    opens = false;
                if (opens && ((machine->machine_flags.m_storage & 4)
                    || ((machine->machine_flags.m_storage & 8) && unit->unit.player_index != -1))
                    && machine->device_position == 0.0f)
                {
                    object_marker marker;
                    if (internal_object_get_markers_by_string_id(machine_index, 674, &marker, 1, false) <= 0)
                    {
                        marker.matrix.position = machine->bounding_center;
                        object_get_orientation(machine_index, &marker.matrix.forward, 0);
                    }
                    real_vector3d direction;
                    vector_from_points3d(&marker.matrix.position, &unit->object.bounding_center, &direction);
                    if (dot_product3d(&marker.matrix.forward, &direction) > 0.0f)
                        opens = false;
                }
                if ((machine->machine_flags.m_storage & 128) && unit->unit.player_index != -1)
                    opens = false;
                if (opens)
                    result = true;
            }
        }
    }
    return result;
}

// @address 0x348ed0 ?machine_compute_activation@@YA_NJPBUs_game_cluster_bit_vectors@@PA_N@Z
bool machine_compute_activation(long object_index, const s_game_cluster_bit_vectors *active, bool *out_active)
{
    bool result = false;
    s_machine_datum *machine = (s_machine_datum *)object_get_and_verify_type(object_index, 128);
    if (machine->pathfinding_count > 0)
    {
        long bsp_index = machine->pathfinding_bsp_index;
        const structure_bsp *definition = structure_bsp_get(bsp_index);
        long end_index = machine->pathfinding_object_index + machine->pathfinding_count;
        for (long i = machine->pathfinding_object_index; i < end_index; i++)
        {
            s_portal_reference reference = device_machines_door_portal_reference_get(global_zone_set_index, bsp_index, i);
            const cluster_portal *portal = TAG_BLOCK_GET_ELEMENT(&definition->cluster_portals, device_machines_portal_index(reference), cluster_portal);
            if (device_machines_cluster_test(active, bsp_index, portal->cluster_indices[0]) ||
                device_machines_cluster_test(active, bsp_index, portal->cluster_indices[1]))
            {
                *out_active = true;
                break;
            }
        }
        result = true;
    }
    return result;
}
