// objectslibxbox/vehicles.obj: from the records 0xa1ad0 0xa9548 0xd2188 0xd80b8 0xdb830 0xdb930 0xdc2a8 0xed628 0xedce8 0xee8c8 0x353b30 0x353c28 0x353db8 0x353e50 0x354260 0x3542a0 0x354338 0x354410 0x354668 0x354898 0x3548e8 0x3671a0
// @flags /TP /O2 /Oc
#include "models/model_damage_info.h"
#include "math/real_math_constants.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "memory/datum_index.h"
#include "physics/collision_test_flags.h"
#include "units/vehicle_type_component.h"
#include "units/vehicle_definitions.h"
#include "physics/collision_test_vector.h"
#include "tag_files/tag_types.h"
#include <string.h>
#include <math.h>
#include "math/real_math.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"
#include "ai/sector_ref.h"
#include "animations/animation_id.h"
#include "objects/object_iterator.h"
#include "units/vehicle_types.h"
#include "units/unit_seat_flags.h"
#include "units/motor_enums.h"
#include "units/motor_program_state.h"
// @address 0xa1ad0 ?vehicle_is_flipped@@YA_NJ@Z
// @address 0xa9548 ?vehicle_get_type@@YA?AW4e_vehicle_type@@J@Z
// @address 0xd80b8 ?vehicle_is_running_trick@@YA_NJ@Z
// @address 0xdb930 vehicle_process_type_animation_channels
// @address 0xed628 vehicle_impulse_process_animation
// @address 0x354898 ?vehicle_get_ultimate_parent@@YAJJ@Z
// @address 0xedce8 vehicle_handle_deleted_object
#include "math/matrix_math.h"
#include <new>
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "units/units.h"
#include "objects/damage_owner.h"
#include "physics/havok_component.h"
#include "units/vehicles.h"
#include "animations/animation_manager.h"
#include "memory/data.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "motor/action_system.h"
#include "memory/data_types.h"
#include "game/player_types.h"
#include "units/scenario_unit_datum_types.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "ai/sector_geometry.h"
#include "units/unit_seat_source.h"
#include "objects/object_placement.h"
#include "models/render_model_definitions.h"
#include "models/model_types.h"
#include "math/real_math_types.h"
#include "units/vehicle_type_alien_fighter.h"
#include "memory/tls_data_arrays.h"
#include "units/vehicle_type_turret.h"
#include "units/vehicle_type_human_jeep.h"
#include "units/vehicle_type_alien_scout.h"
#include "units/vehicle_type_chopper.h"
#include "physics/havok_component_class.h"
#include "physics/collision_types.h"

#include "game/game_globals.h"
#include "game/player_control_constants.h"


#include "objects/object_definitions.h"


#include "objects/object_header.h"
#include "units/vehicle_datum.h"
#include "animations/animation_search_path_iterator.h"




  /* object_header_data */

#include "units/vehicle_type_vtol.h"

#include "units/vehicle_type_human_plane.h"


struct s_vehicle_animation_context
{
    long count;
    real_orientation *orientations;
    const c_static_flags<255> *mask;
};












// 0x353b30
struct s_scenario_vehicle {
    char object_data[0x6c];                     // object datum + permutation
    s_scenario_unit_datum unit;
};

enum { _vehicle_scenario_placed_bit = 6 };

#include "units/vehicle_type_component_internals.h"

#include "game/game_time_types.h"




#include "animations/suspension_animation_iterator.h"


struct s_s_vehicle_animation_context
{
    vehicle_datum *object;
    const cache_file_tag_instance *tag_instances;
    const s_object_definition *definition;
};



static __forceinline vehicle_datum *vehicles_get(long object_index)
{
    return (vehicle_datum *)object_get(object_index);
}

// Returns whether the vehicle is tilted beyond the flip threshold.
bool vehicle_is_flipped(long object_index)
{
    float threshold = (float)cos((double)(1.5707964f - ((s_player_control_constants *)global_game_globals->player_control.address)->vehicle_flip_angle));
    real_vector3d up;
    ::object_get_orientation(object_index, 0, &up);
    return up.k < threshold;
}

e_vehicle_type vehicle_get_type(long object_index)
{
    vehicle_datum *object = vehicles_get(object_index);
    vehicle_definition *definition = (vehicle_definition *)tag_get(0, object->object.definition_index);
    int result = _vehicle_type_none;
    for (int i = 0; i < k_vehicle_type_count; i++) {
        if (definition->types[i].count != 0) {
            result = i;
            break;
        }
    }
    return (e_vehicle_type)result;
}

bool vehicle_is_running_trick(long object_index)
{
    if (vehicle_get_type(object_index) != _vehicle_type_alien_fighter)
        return false;
    return vehicles_get(object_index)->vehicle.get_alien_fighter()->is_running_trick(object_index);
}


bool vehicle_physics_disabled(long object_index)
{
    e_vehicle_type type = vehicle_get_type(object_index);
    switch (type)
    {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
        return (vehicles_get(object_index)->object.damage_flags >> 2) & 1;
    case 0:
    case 1:
    case 8:
        return false;
    default:
        // match: 0xdc2a8: all vehicle types are cased, so the hint omits the switch range check
        __assume(0);
    }
}

// refreshes the cached marker handle on the object (when asked), then starts the effect
// named through the object's definition chain if the handle is valid and the animation manager accepts it.
typedef void (*vehicles_animation_callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *);

// walks the vehicle type's animation channels; the callback argument is replaced by the
// impulse preprocess callback before the type's walker runs.
extern "C" void vehicle_process_type_animation_channels(
    void *state, long object_index, vehicles_animation_callback callback, void *context, c_animation_channel *channel, bool refresh)
{
    callback = (vehicles_animation_callback)vehicle_impulse_channel_node_preprocess_callback;
    switch (vehicle_get_type(object_index)) {
    case 1: ((c_vehicle_type_human_jeep *)state)->process_animation_channels(object_index, callback, context, channel, refresh); break;
    case 2:
    vehicle_impulse_process_animation(
        (c_vehicle_type_human_plane *)state, object_index, callback, context, channel, refresh);
    break;
    case 3: ((c_vehicle_type_alien_scout *)state)->process_animation_channels(object_index, callback, context, channel, refresh); break;
    case 4: ((c_vehicle_type_alien_fighter *)state)->process_animation_channels(object_index, callback, context, channel, refresh); break;
    case 7: ((c_vehicle_type_vtol *)state)->process_animation_channels(object_index, callback, context, channel, refresh); break;
    case 8: ((c_vehicle_type_chopper *)state)->process_animation_channels(object_index, callback, context, channel, refresh); break;
    case 0: case 5: case 6: case 9: break;
    // match: 0xdb930: all vehicle types are cased, so the hint omits the switch range check
    default: __assume(0);
    }
}


void vehicle_impulse_process_animation(
    c_vehicle_type_human_plane *record, long object_index, vehicles_animation_callback callback, void *user, c_animation_channel *channel, bool refresh)
{
    vehicle_datum *object = vehicles_get(object_index);
    char *attachments = (char *)object + object->object.animation.offset;
    c_animation_manager *manager = (c_animation_manager *)attachments;
    s_object_definition *definition = (s_object_definition *)tag_get(0, object->object.definition_index);
    s_model_definition *model = (s_model_definition *)tag_get(0, definition->model.index);
    const render_model_definition *effect = (const render_model_definition *)tag_get(0, model->render_model_index);

    if (refresh)
        record->m_animation = manager->find_parallel_impulse_animation_id(226, 226, 124);
    if (record->m_animation.valid() && manager->setup_animation_channel_by_index(channel, record->m_animation, 0))
        vehicle_impulse_channel_node_preprocess_callback(object_index, effect, channel, record->m_smoothed_control, 0.0f, 1.0f, user);
}


void vehicle_handle_deleted_object(unsigned int vehicle_object_index,int deleted_object_index)

{
  int vehicle_kind;
  vehicle_datum *vehicle_object;
  
  vehicle_object = vehicles_get(vehicle_object_index);
  vehicle_kind = vehicle_get_type((long)vehicle_object_index);
  if (((vehicle_kind == 5) && ((bool)(vehicle_object->vehicle.flags >> 7 & 1))) &&
     (vehicle_object->vehicle.type_component.get_type_turret()->m_auto_control.target_object_index == deleted_object_index)) {
    vehicle_object->vehicle.type_component.get_type_turret()->m_auto_control.target_object_index = 0xffffffff;
  }
  return;
}

// Walks the chain of vehicle parents.
long vehicle_get_ultimate_parent(long object_index)
{
    long parent_index = NONE;
    while (object_index != NONE)
    {
        struct vehicle_datum *object = vehicles_get(object_index);
        if (object->object.object_identifier.get_type() != _object_type_vehicle)
            break;
        parent_index = object_index;
        object_index = object->object.parent_object_index;
    }
    return parent_index;
}


short vehicle_get_driver_seat(long object_index, long *driver_object_index)
{
    short driver_seat_index = -1;
    long driver_index = -1;
    short seat_count = 0;
    object_index = vehicle_get_ultimate_parent(object_index);
    unit_seat_source sources[64];
    unit_get_all_seats_internal(object_index, sources, &seat_count, 64, false);
    for (short seat_index = 0; seat_index < seat_count; ++seat_index)
    {
        const unit_seat *seat = sources[seat_index].definition;
        if (seat->test_flag(_unit_seat_driver_bit))
        {
            driver_seat_index = seat_index;
            driver_index = sources[seat_index].unit_index;
            break;
        }
    }
    if (driver_object_index)
        *driver_object_index = driver_index;
    return driver_seat_index;
}

static __forceinline const c_model_animation *vehicles_channel_get_animation(const c_animation_channel *channel)
{
    const c_model_animation *animation = 0;
    if (channel->m_animation_id.valid())
        animation = ((const c_model_animation_graph *)tag_get(0, channel->m_graph_index))->get_animation(channel->m_animation_id);
    return animation;
}
static __forceinline bool vehicles_channel_aiming_screen_attached(const c_animation_channel *channel)
{
    const c_model_animation *animation = vehicles_channel_get_animation(channel);
    return animation->blend_screen != -1;
}
// @address 0x353db8 vehicle_impulse_channel_node_preprocess_callback
extern "C" void vehicle_impulse_channel_node_preprocess_callback(
    long object_index, const render_model_definition *model, c_animation_channel *channel,
    float ratio, float pitch, float weight, void *user_data)
{
    s_vehicle_animation_context *data = (s_vehicle_animation_context *)user_data;
    if (vehicles_channel_aiming_screen_attached(channel))
        (channel)->apply_weighted_blend_screen_node_orientations(
            ratio, pitch, weight, data->mask, data->count,
            data->orientations, 0, object_index);
    else
        (channel)->apply_weighted_node_orientations_at_playback_ratio(
            (const render_model_definition *)model, ratio, weight, data->mask,
            data->count, data->orientations, 0, object_index);
}

// @address 0x354260 vehicle_preprocess_node_orientations
extern "C" void vehicle_preprocess_node_orientations(
    long context, const c_static_flags<255> *node_mask, long node_count, real_orientation *orientations)
{
    s_vehicle_animation_context data;
    data.orientations = orientations;
    data.mask = node_mask;
    data.count = node_count;
    vehicle_process_animation_channels(context, (vehicles_animation_callback)vehicle_impulse_channel_node_preprocess_callback, &data);
}

static __forceinline float vehicles_distance_squared(const real_point3d *a, const real_point3d *b)
{
    float y = b->y - a->y;
    float z = b->z - a->z;
    float x = b->x - a->x;
    return x * x + (z * z + y * y);
}

// @address 0x354668 ?vehicle_about_to_detonate_near_any_player@@YA_NPAJ@Z
bool vehicle_about_to_detonate_near_any_player(long * vehicle_index)
{
    long player_count = 0;
    real_point3d player_positions[16];
    long player_units[16];
    c_data_iterator<player_datum> player_iterator;
    player_iterator.begin(player_data);
    while (player_iterator.next()) {
        player_datum *player = player_iterator.get_datum();
        if (player->unit_index != -1) {
            player_units[player_count] = player->unit_index;
            player_positions[player_count] = object_get(player->unit_index)->bounding_center;
            player_count++;
        }
    }
    if (player_count > 0) {
        c_object_iterator<object_datum> vehicles;
        vehicles.begin(2, 0);
        while (vehicles.next()) {
            object_datum *vehicle = vehicles.get_datum();
            for (short i = 0; i < player_count; i++) {
                object_datum *unit = object_get(player_units[i]);
                if (unit->parent_object_index != vehicles.get_index() &&
                    vehicles_distance_squared(&player_positions[i], &vehicle->bounding_center) < 100.0f &&
                    (unsigned char)((vehicle->damage_flags >> 2) & 1)) {
                    long section_count = (unsigned long)vehicle->damage_sections.size / sizeof(s_damage_object_section_state);
                    s_damage_object_section_state *sections = (s_damage_object_section_state *)((char *)vehicle + vehicle->damage_sections.offset);
                    for (long j = 0; j < section_count; j++) {
                        if ((sections[j].delayed_response & 0xfff) > 0) {
                            *vehicle_index = vehicles.get_index();
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
// @address 0x3671a0 ?init_auto_turret@c_vehicle_type_turret@@QAA_NJJMMMJ@Z
bool c_vehicle_type_turret::init_auto_turret(long object_index, long trigger, float minimum_range, float maximum_range, float angle, long owner_index)
{
    vehicle_datum *vehicle = (vehicle_datum *)vehicles_get(object_index);
    const float zero = 0.0f;
    this->m_auto_control.active = false;
    this->m_auto_control.damage_owner = *damage_owner_unknown();
    this->m_min_range = zero;
    this->m_max_range = zero;
    this->m_auto_control.target_object_index = -1;
    this->m_lost_seconds = zero;
    this->m_auto_control.target_time = -1;
    this->m_auto_control.minimum_range_start_time = -1;
    this->m_auto_control.target_lost_time = -1;
    this->m_volume_index = trigger;
    if (trigger != -1) {
        this->m_min_range = minimum_range <= zero ? zero : minimum_range;
        this->m_max_range = maximum_range;
        this->m_lost_seconds = angle;
    }
    if (owner_index != -1) {
        damage_compute_damage_owner_from_object_index(owner_index, &this->m_auto_control.damage_owner);
        this->m_auto_control.active = true;
    }
    vehicle->vehicle.forward = vehicle->object.forward;
    return true;
}
// @address 0x3542a0 ?vehicle_set_auto_turret@@YAXJJMMMJ@Z
void vehicle_set_auto_turret(long object_index, long trigger, float minimum_range, float maximum_range, float angle, long owner_index)
{
    if (object_index != NONE) {
        bool result = false;
        vehicle_datum *object = vehicles_get(object_index);
        c_vehicle_type_turret *storage = (c_vehicle_type_turret *)vehicle_get_auto_control(&object->vehicle.type_component, object_index);
        if (storage != 0)
            result = storage->init_auto_turret(object_index, trigger, minimum_range, maximum_range, angle, owner_index);
        if (result) {
            object->vehicle.flags |= 0x80;
            unit_set_actively_controlled(object_index, true);
            object_wake(object_index);
        }
    }
}

static __forceinline c_havok_component *vehicles_hover_havok_component_get(long havok_component_index)
{
    return ((c_havok_component *)((s_data_array *)g_havok_component_data)->data) + (havok_component_index & 0xffff);
}
// @address 0x354338 ?vehicle_hover@@YAXJ_N@Z
void vehicle_hover(long vehicle_index, bool hover)
{
    if (vehicle_index != NONE) {
        vehicle_datum *vehicle = (vehicle_datum *)vehicles_get(vehicle_index);
        bool was_hovering = vehicle->vehicle.m_flags.test(_vehicle_hover_bit);
        vehicle->vehicle.m_flags.set(_vehicle_hover_bit, hover);
        if (was_hovering != hover) {
            if (vehicle->object.havok_component_index != NONE) {
                c_havok_component *havok_component = vehicles_hover_havok_component_get(vehicle->object.havok_component_index);
                bool connected = havok_component->is_connected_to_world();
                if (connected)
                    havok_component->disconnect_from_havok_world();
                havok_component->clear_rigid_bodies();
                havok_component_construct(vehicle->object.havok_component_index);
                if (connected)
                    havok_component->connect_to_havok_world();
            }
        }
    }
}


// @address 0xdb830 ?vehicle_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool vehicle_new(long object_index, object_placement_data *placement, bool *out_of_memory)
{
    vehicle_datum *vehicle = vehicles_get(object_index);
    vehicle_definition *definition = (vehicle_definition *)tag_get(0, vehicle->object.definition_index);
    s_model_definition *referenced = (s_model_definition *)tag_get(0, definition->model.index);
    bool result = referenced->physics_model_index != -1;
    if (result)
    {
        vehicle->vehicle.squad_index = -1;
        vehicle->vehicle.next_vehicle_index = -1;
        vehicle->vehicle.friction_update_time = -1;
        vehicle->vehicle.starting_location_name = 0;
        vehicle->vehicle.locked_seats[0] = 0;
        vehicle->vehicle.locked_seats[1] = 0;
        memset(&vehicle->vehicle.reservations, -1, sizeof(vehicle->vehicle.reservations));
        vehicle_reset(object_index);
        c_motor_program_state *state = (c_motor_program_state *)((char *)vehicle + vehicle->unit.motor_offset);
        state->set_program(object_index, _motor_program_vehicle);
        result = result && ::object_header_block_allocate(object_index, 0x780, (short)(definition->havok_vehicle_physics.friction_points.count * 44), 4) != 0;
    }
    return result;
}

// @address 0x353b30 ?vehicle_place@@YAXJPAUs_scenario_vehicle@@@Z
void vehicle_place(long vehicle_index, s_scenario_vehicle *scenario_vehicle)
{
    vehicle_datum *vehicle = (vehicle_datum *)vehicles_get(vehicle_index);
    unit_place(vehicle_index, &scenario_vehicle->unit);
    vehicle->vehicle.m_flags.set((e_vehicle_flags)_vehicle_scenario_placed_bit, true);
    if (scenario_vehicle->unit.flags & 2)
    {
        if (motor_animation_set_mode_weapons_and_state(vehicle_index, 1, 226, 226, 97, 0.0f, 0, 29, false))
        {
            c_animation_manager *animation = (c_animation_manager *)((char *)vehicle + vehicle->object.animation.offset);
            animation->set_state_position_to_last_frame();
        }
        else
        {
            motor_animation_set_mode_weapons_and_state(vehicle_index, 1, 226, 226, 12, 0.0f, 0, 31, false);
        }
        object_compute_node_matrices(vehicle_index);
    }
}
static __forceinline void vehicles_reset_clear_longs(long *values, unsigned long count)
{
    for (unsigned long i = 0; i < count; i++)
        values[i] = 0;
}
// @address 0x353c28 ?vehicle_reset@@YAXJ@Z
void vehicle_reset(long object_index)
{
    vehicle_datum *vehicle = vehicles_get(object_index);
    vehicle->vehicle.m_flags.clear();
    vehicle->vehicle.upside_down_ticks = 0;
    vehicle->vehicle.airborne_ticks = 0;
    vehicle->vehicle.flip_direction = 0;
    vehicle->vehicle.flip_time = 0;
    vehicle->vehicle.braking_ticks = 0;
    vehicle->unit.vehicle_steering = 0.0f;
    vehicle->unit.vehicle_throttle = 0.0f;
    vehicle->vehicle.grounded = 0;
    vehicles_reset_clear_longs(reinterpret_cast<long *>(vehicle->vehicle.suspension_weights), sizeof(vehicle->vehicle.suspension_weights) / sizeof(long));
    vehicles_reset_clear_longs(reinterpret_cast<long *>(vehicle->vehicle.suspension_samples), sizeof(vehicle->vehicle.suspension_samples) / sizeof(long));
    vehicle->vehicle.counter = 0;
    vehicle->vehicle.m_flags.set((e_vehicle_flags)_vehicle_scenario_placed_bit, true);
    object_get_center_of_mass(object_index, &vehicle->vehicle.pathfinding_point);
    vehicle->vehicle.pathfinding_time = NONE;
    vehicle->vehicle.pathfinding_structure_index = NONE;
    vehicle->vehicle.pathfinding_surface_index = NONE;
    vehicle->vehicle.pathfinding_object_index = NONE;
    vehicle->vehicle.pathfinding_sector.clear();
    vehicle->vehicle.pathfinding_bsp_reference = NONE;
    vehicle->vehicle.animation_graph_index = NONE;
    vehicle->vehicle.impulse_ids[0].clear();
    vehicle->vehicle.impulse_ids[1].clear();
    vehicle->vehicle.impulse_ids[2].clear();
    vehicle->vehicle.impulse_ids[3].clear();
    for (long i = 0; i < NUMBEROF(vehicle->vehicle.screen_ids); ++i)
        vehicle->vehicle.screen_ids[i].clear();
    vehicle->vehicle.type_component.reset(object_index);
    vehicle->vehicle.forward = *global_forward3d;
    vehicle->vehicle.velocity = 0.0f;
    vehicle->vehicle.angular_velocity = 0.0f;
    real_point3d *position = &vehicle->vehicle.last_position;
    position->x = 65536.0f;
    position->y = 65536.0f;
    position->z = 65536.0f;
}

extern "C" {

}

// @address 0xd2188 ?vehicle_find_pathfinding_location@@YAXJPAFPAJPAVc_sector_ref@@PATreal_point3d@@1PAK@Z
void vehicle_find_pathfinding_location(long vehicle_index, short *bsp_index, long *surface_index, c_sector_ref *sector_ref,
    real_point3d *position, long *object_index, unsigned long *bsp_reference)
{
    bool found = false;
    vehicle_datum *vehicle = (vehicle_datum *)vehicles_get(vehicle_index);
    if (((1 << vehicle_get_type(vehicle_index)) & 148) == 0)
    {
        long time = game_time_globals->time;
        if (time > vehicle->vehicle.pathfinding_time)
        {
            collision_result collision;
            vehicle->vehicle.pathfinding_time = time;
            real_point3d point;
            object_get_origin(vehicle_index, &point);
            point_from_line3d(&point, global_up3d, 0.4f, &point);
            real_vector3d vector;
            scale_vector3d(global_down3d, 2.0f, &vector);
            if (collision_test_vector(k_biped_ground_collision_flags, false, &point, &vector, -1, -1, -1, &collision))
            {
                vehicle->vehicle.pathfinding_point = collision.point;
                vehicle->vehicle.pathfinding_structure_index = (short)collision.structure_bsp_index;
                vehicle->vehicle.pathfinding_surface_index = collision.surface_index;
                vehicle->vehicle.pathfinding_object_index = collision.object_index;
                vehicle->vehicle.pathfinding_bsp_reference = collision.bsp_reference;
                if (collision.surface_index != -1)
                    vehicle->vehicle.pathfinding_sector = collision_surface_get_sector((short)collision.structure_bsp_index, collision.surface_index,
                        collision.instance_index, collision.object_index, collision.bsp_reference, &collision.point);
                else
                    vehicle->vehicle.pathfinding_sector.clear();
                found = true;
            }
        }
        else
        {
            found = true;
        }
    }
    if (found)
    {
        if (position)
            *position = vehicle->vehicle.pathfinding_point;
        if (surface_index)
            *surface_index = vehicle->vehicle.pathfinding_surface_index;
        if (object_index)
            *object_index = vehicle->vehicle.pathfinding_object_index;
        if (bsp_reference)
            *bsp_reference = vehicle->vehicle.pathfinding_bsp_reference;
        if (sector_ref)
            *sector_ref = vehicle->vehicle.pathfinding_sector;
    }
    else
    {
        if (position)
            object_get_origin(vehicle_index, position);
        if (surface_index)
            *surface_index = -1;
        if (object_index)
            *object_index = -1;
        if (bsp_reference)
            *bsp_reference = -1;
        if (sector_ref)
            sector_ref->clear();
    }
}

// @address 0x354410 ?vehicle_moving_near_any_player@@YA_NPAJ@Z
bool vehicle_moving_near_any_player(long *vehicle_index)
{
    long player_count = 0;
    real_point3d player_positions[16];
    long player_units[16];
    c_data_iterator<player_datum> player_iterator;
    player_iterator.begin(player_data);
    while (player_iterator.next()) {
        if (player_count >= 16)
            break;
        player_datum *player = player_iterator.get_datum();
        if (player->unit_index != -1 && vehicles_get(player->unit_index)->object.parent_object_index == -1) {
            player_units[player_count] = player->unit_index;
            player_positions[player_count] = vehicles_get(player->unit_index)->object.bounding_center;
            player_count++;
        }
    }
    if (player_count > 0) {
        c_object_iterator<object_datum> vehicles;
        vehicles.begin(2, 0);
        if (vehicles.next()) {
            const float max_distance_squared = 100.0f;
            const float min_speed_squared = 1.0f;
            do {
                vehicle_datum *vehicle = (vehicle_datum *)vehicles.get_datum();
                long definition_index = ((vehicle_datum *)vehicle)->object.definition_index;
                const vehicle_definition *definition =
                    (const vehicle_definition *)global_tag_instances[datum_index_to_absolute_index(definition_index)].definition;
                bool no_detonate = ((definition->flags >> 9) & 1) != 0;
                if (!no_detonate) {
                    for (short i = 0; i < player_count; i++) {
                        real_vector3d velocity;
                        object_get_velocities(vehicles.get_index(), &velocity, 0);
                        float velocity_k = velocity.k;
                        float velocity_j = velocity.j;
                        float velocity_i = velocity.i;
                        vehicle_datum *unit = vehicles_get(player_units[i]);
                        if (unit->object.parent_object_index != vehicles.get_index() &&
                            vehicles_distance_squared(&player_positions[i], &vehicle->object.bounding_center) < max_distance_squared &&
                            velocity_i * velocity_i + (velocity_j * velocity_j + velocity_k * velocity_k) >= min_speed_squared) {
                            *vehicle_index = vehicles.get_index();
                            return true;
                        }
                    }
                }
            } while (vehicles.next());
        }
    }
    return false;
}
struct s_animation_lookup_request;
static __forceinline long vehicles_animation_graph_index(const c_animation_manager *manager)
{
    return manager->m_graph_tag_index;
}
static __forceinline bool vehicles_setup_suspension_settings(c_animation_channel *channel, c_animation_channel_settings *settings, long graph_index)
{
    bool result = false;
    if (graph_index != -1)
    {
        settings->m_graph_index = graph_index;
        if (channel->setup_channel(settings))
            result = true;
    }
    return result;
}
static __forceinline bool vehicles_setup_suspension(c_animation_manager *manager, c_animation_channel *channel,
    const suspension_animation_iterator *iterator, c_animation_channel_settings *storage)
{
    bool result = false;
    if (vehicles_animation_graph_index(manager) != -1)
    {
        new (storage) c_animation_channel_settings;
        c_animation_channel_settings &settings = *(c_animation_channel_settings *)storage;
        long graph_index = vehicles_animation_graph_index(manager);
        settings.m_playback_flags.set_unsafe(64);
        settings.m_graph_index = graph_index;
        settings.m_animation_id = iterator->animation;
        result = vehicles_setup_suspension_settings(channel, &settings, graph_index);
    }
    return result;
}
static __forceinline bool vehicles_setup_acceleration(
    c_animation_manager *manager, c_animation_channel *channel, c_animation_id id, c_animation_channel_settings *storage)
{
    bool result = false;
    if (vehicles_animation_graph_index(manager) != -1)
    {
        new (storage) c_animation_channel_settings;
        c_animation_channel_settings &settings = *(c_animation_channel_settings *)storage;
        long graph_index = vehicles_animation_graph_index(manager);
        settings.m_animation_id = id;
        settings.m_playback_flags.set_unsafe(0x8040);
        settings.m_graph_index = graph_index;
        result = manager->internal_setup_animation_channel(channel, &settings);
    }
    return result;
}
static __forceinline float vehicles_animation_axis_ratio(float value, float half)
{
    float scaled = value * half;
    return scaled + half;
}

static __forceinline s_s_vehicle_animation_context vehicles_get_animation_context(long object_index)
{
    s_s_vehicle_animation_context context;
    context.object = vehicles_get(object_index);
    context.tag_instances = global_tag_instances;
    context.definition = (const s_object_definition *)context.tag_instances[datum_index_to_absolute_index(context.object->object.definition_index)].definition;
    return context;
}

static __forceinline void vehicles_process_suspension_channels(c_animation_manager *manager, vehicle_datum *vehicle,
    long object_index, const render_model_definition *render_model, void *data, c_animation_channel_settings *settings_storage)
{
    suspension_animation_iterator iterator;
    c_animation_channel suspension;
    while (manager->find_next_vehicle_suspension(&iterator))
    {
        if (vehicles_setup_suspension(manager, &suspension, &iterator, settings_storage))
        {
            if ((unsigned long)iterator.suspension_index <= 31)
            {
                unsigned char weight = vehicle->vehicle.suspension_weights[iterator.suspension_index];
                float ratio = weight == 255 ? 1.0f : (float)(long)weight * 0.003921569f;
                vehicle_impulse_channel_node_preprocess_callback(
                    object_index, render_model, &suspension, ratio, 0.0f, 1.0f, data);
            }
        }
    }
    animation_channel_initialize_and_disable(&suspension);
}

// @address 0x353e50 ?vehicle_process_animation_channels@@YAXJP6AXJPBUrender_model_definition@@PAVc_animation_channel@@MMMPAX@Z2@Z
void vehicle_process_animation_channels(long object_index, vehicles_animation_callback callback, void *data)
{
    s_s_vehicle_animation_context context = vehicles_get_animation_context(object_index);
    callback = (vehicles_animation_callback)vehicle_impulse_channel_node_preprocess_callback;
    vehicle_datum *object = context.object;
    vehicle_datum *vehicle = (vehicle_datum *)object;
    real_vector3d velocity;
    bool refresh = false;
    object_get_velocities(object_index, &velocity, 0);
    if (context.definition->model.index == -1 || (unsigned short)vehicles_get(object_index)->object.animation.offset == 0xffff)
        return;
    const s_model_definition *model =
        (const s_model_definition *)context.tag_instances[datum_index_to_absolute_index(context.definition->model.index)].definition;
    if (model->render_model_index == -1)
        return;
    c_animation_manager *manager = (c_animation_manager *)((char *)object + object->object.animation.offset);
    const render_model_definition *render_model =
        (const render_model_definition *)context.tag_instances[datum_index_to_absolute_index(model->render_model_index)].definition;
    c_animation_channel channel;
    long graph_index = vehicles_animation_graph_index(manager);
    if (graph_index != vehicle->vehicle.animation_graph_index)
    {
        vehicle->vehicle.animation_graph_index = graph_index;
        refresh = true;
        vehicle->vehicle.impulse_ids[0] = manager->find_parallel_impulse_animation_id(226, 226, 124);
        vehicle->vehicle.impulse_ids[1] = manager->find_parallel_impulse_animation_id(226, 226, 125);
        vehicle->vehicle.impulse_ids[2] = manager->find_parallel_impulse_animation_id(226, 226, 126);
        vehicle->vehicle.impulse_ids[3] = manager->find_parallel_impulse_animation_id(226, 226, 127);
        for (long index = 0; index < 3; ++index)
        {
            short id[2];
            const c_animation_id *found = (const c_animation_id *)animation_manager_find_acceleration_overlay_id(
                id, manager, index);
            vehicle->vehicle.screen_ids[index] = *found;
        }
    }
    vehicle_process_type_animation_channels(&vehicle->vehicle.type_component, object_index, callback, data, &channel, refresh);
    __declspec(align(4)) unsigned char settings_storage[sizeof(c_animation_channel_settings)];
    vehicles_process_suspension_channels(manager, vehicle, object_index, render_model, data, (c_animation_channel_settings *)settings_storage);
    float half = 0.5f;
    for (long index = 0; index < 3; ++index)
    {
        if (vehicle->vehicle.screen_ids[index].valid())
        {
            float ratio = vehicles_animation_axis_ratio(vehicle->unit.seat_acceleration.n[index], half);
            ratio = PIN(ratio, 0.0f, 1.0f);
            if (vehicles_setup_acceleration(manager, &channel, vehicle->vehicle.screen_ids[index], (c_animation_channel_settings *)settings_storage))
                vehicle_impulse_channel_node_preprocess_callback(object_index, render_model, &channel, ratio, 0.0f, 1.0f, data);
        }
    }
    channel.initialize();
}

static __forceinline float vehicle_function_safe_divide(float numerator, float denominator)
{
    float result = 0.0f;
    if (!(fabs(denominator) <= 0.001f))
        result = numerator / denominator;
    return result;
}
// @address 0xee8c8 ?vehicle_compute_function_value@@YA_NJJJPAMPA_N1@Z
bool vehicle_compute_function_value(long vehicle_index, long function_name, long function_index, float *value, bool *active, bool *deterministic)
{
    vehicle_datum *vehicle = (vehicle_datum *)vehicles_get(vehicle_index);
    float result = 0.0f;
    bool valid = true;
    bool result_active = false;
    if (!vehicle->vehicle.type_component.compute_function_value(vehicle_index, function_name, &result, &result_active))
    {
        switch (function_name)
        {
        case _string_id_jump:
            result = TEST_BIT(vehicle->unit.control_flags, 1) ? 1.0f : 0.0f;
            break;
        case _string_id_boost:
            result = TEST_BIT(vehicle->unit.control_flags, 12) ? 1.0f : unit_compute_boost_fraction(vehicle_index);
            break;
        case _string_id_wingtip_contrail:
        {
            const float scale = 0.3f;
            real_vector3d parallel, perpendicular;
            component_vectors_from_normal3d(&vehicle->object.translational_velocity, &vehicle->object.forward, &parallel, &perpendicular);
            result = vehicle_function_safe_divide(magnitude3d(&perpendicular), scale);
            result *= result;
            break;
        }
        case _string_id_mean_antigrav:
            result = vehicle->vehicle.function_value;
            break;
        case _string_id_emitting:
            result = action_executing(vehicle_index, _action_emit_ai) ? 1.0f : 0.0f;
            break;
        case _string_id_surge:
            result = action_executing(vehicle_index, _action_surge) ? 1.0f : 0.0f;
            break;
        case _string_id_phase:
            result = action_executing(vehicle_index, _action_phase) ? 1.0f : 0.0f;
            break;
        default:
            valid = false;
            break;
        }
    }
    if (valid)
    {
        result = PIN(result, 0.0f, 1.0f);
        *value = result;
        *active = result > 0.0f || result_active;
    }
    return valid;
}
