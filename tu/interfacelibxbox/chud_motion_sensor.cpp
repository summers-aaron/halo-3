// interfacelibxbox/chud_motion_sensor.obj: from the records 0x1189a8 0x133470 0x133660 0x133868 0x133a60 0x133ae8 0x133e80 0x1342c8 0x1370d8 0x4a3b18 0x4a3c20 0x4a3e58
// @compiler xdk6130
#include "scenario/scenario.h"
#include "game/player_control_types.h"
#include "items/equipment.h"
#include "items/equipment_globals.h"
#include "interface/chud/chud_scripting_globals_types.h"
#include "interface/chud/chud_navpoint_manager.h"
#include "game/game_engine_list.h"
// @flags /TP /O2 /Oc
#include "game/player_control_globals.h"
#include "units/unit_seat.h"
#include "units/unit_definitions.h"
#include "objects/object_header.h"
#include "cseries/bit_operations.h"
#include "tag_files/tag_groups.h"
#include "game/player_traits.h"
#include "game/game_globals_storage.h"
#include "objects/object_iterator.h"
#include <math.h>
#include "math/real_math.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"
#include "units/unit_types.h"
#include "items/item_types.h"
#include "items/equipment_definitions.h"
#include "objects/objects.h"
#include "units/units.h"
#include "interface/chud/chud_motion_sensor.h"
#include "interface/chud/chud.h"
#include "game/game_allegiance.h"
#include "math/real_math_types.h"
#include "game/game_time_inline.h"
#include "game/player_types.h"
#include "objects/object_marker_globals_types.h"
#include "game/game_engine_default.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_types.h"
#include "game/player_mapping_types.h"
#include "memory/data_types.h"
// @address 0x133a60 ?update_tracked_entity@c_chud_motion_sensor@@AAAXJPBTreal_point3d@@@Z
#include "game/game_engine_team_relationships.h"
#include "game/player_access.h"
#include "game/player_mapping_inlines.h"
#include "game/game_globals.h"
#include "game/player_information_definition.h"
#include "creatures/creature_datum.h"
#include "creatures/creature_definitions.h"
#include "objects/object_marker_access.h"
#include "motor/player_equipment_action_state.h"
#include "game/game_globals_inline.h"

namespace {
static __forceinline int invalid_game_engine_index(long index) {
    if (index < 1) return 1;
    if (index > 10) return 1;
    return 0;
}
static __forceinline bool game_engine_has_teams() {
    long index = ((s_game_engine_globals *)tls_game_engine_globals())->game_engine_index;
    return !invalid_game_engine_index(index) && game_engines[index] != 0 &&
        (((s_game_engine_globals *)tls_game_engine_globals())->flags & 8) != 0;
}
static __forceinline void chud_motion_sensor_flags_set(unsigned long &flags, long bit, bool enable)
{
    if (enable)
        flags |= 1 << bit;
    else
        flags &= ~(1 << bit);
}
static __forceinline bool chud_motion_sensor_equipment_is_active_noise_maker(long equipment_index) {
    object_datum *equipment = (object_datum *)object_get_and_verify_type(equipment_index, 8);
    s_equipment_definition *definition = TAG_GET(0, s_equipment_definition, equipment->definition_index);
    bool result = false;
    if (definition->equipment_types[4].count && ((equipment_datum *)equipment)->activation_time != -1 &&
        !bit_test(equipment->damage_flags, 2)) {
        float seconds = game_ticks_to_seconds((float)(game_time_get() - ((equipment_datum *)equipment)->activation_time));
        if (seconds > *(const float *)definition->equipment_types[4].address)
            result = true;
    }
    return result;
}


static __forceinline s_object_marker_globals *chud_motion_sensor_get_object_marker_globals()
{
    return &OBJECT_MARKER_GLOBALS[g_registered_thread_index - 1];
}

static __forceinline void chud_motion_sensor_object_marker_begin()
{
    s_object_marker_globals *markers = chud_motion_sensor_get_object_marker_globals();
    ++markers->mark;
    markers->active = true;
}

static __forceinline void chud_motion_sensor_object_marker_end()
{
    chud_motion_sensor_get_object_marker_globals()->active = false;
}

static __forceinline bool chud_motion_sensor_object_mark(long object_index)
{
    s_object_marker_globals *markers = chud_motion_sensor_get_object_marker_globals();
    markers->marks[(unsigned short)object_index] = markers->mark;
    return true;
}

static __forceinline bool chud_motion_sensor_object_unmarked(long object_index)
{
    s_object_marker_globals *markers = chud_motion_sensor_get_object_marker_globals();
    return markers->marks[(unsigned short)object_index] != markers->mark;
}
}
static __forceinline float chud_motion_sensor_boardable_magnitude3d(const real_vector3d *v)
{
    return sqrtf(v->i * v->i + (v->k * v->k + v->j * v->j));
}
static __forceinline real_vector3d *boardable_vector_from_points3d(const real_point3d *a, const real_point3d *b, real_vector3d *v)
{
    v->i = b->x - a->x;
    v->j = b->y - a->y;
    v->k = b->z - a->z;
    return v;
}
static __forceinline float boardable_dot_product3d(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + (a->k * b->k + b->j * a->j);
}
static __forceinline bool boardable_flags_test(unsigned long flags, long bit)
{
    return (flags & (1UL << bit)) != 0;
}
static __forceinline unit_definition *boardable_tag_get(long index)
{
    return TAG_GET(0, unit_definition, index);
}

// @address 0x4a3c20 ?slow_moving_boardable_enemy_vehicles_nearby@c_chud_motion_sensor@@QAA_NXZ
bool c_chud_motion_sensor::slow_moving_boardable_enemy_vehicles_nearby()
{
    long player_index = player_index_from_user_index_render(m_user_index);
    bool result = false;
    if (player_index != -1)
    {
        player_datum *player = player_get(player_index);
        if (player->unit_index != -1)
        {
            object_datum *player_unit = (object_datum *)object_get_and_verify_type(player->unit_index, 8195);
            for (long entity_index = 0; entity_index < 20; ++entity_index)
            {
                s_tracked_entity *entity = &m_tracked_entities[entity_index];
                long object_index = entity->object_index;
                if (object_index != -1)
                {
                    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, ~0ul);
                    if (object && ((1 << object->object_identifier.get_type()) & 2))
                    {
                        object_datum *vehicle = object;
                        long type = object_get_blip_type(object_index);
                        float distance_threshold = 16.0f;
                        if (type == 3 && entity->age < distance_threshold)
                        {
                            float velocity_threshold = 1.5f;
                            real_vector3d velocity;
                            object_get_velocities(entity->object_index, &velocity, 0);
                            float speed = chud_motion_sensor_boardable_magnitude3d(&velocity);
                            if (speed < velocity_threshold)
                            {
                                float facing = (float)cos(0.5235987715423107);
                                real_vector3d direction;
                                boardable_vector_from_points3d(&player_unit->bounding_center,
                                    &vehicle->bounding_center, &direction);
                                normalize3d(&direction);
                                if (boardable_dot_product3d(&direction, &player_unit->forward) < facing)
                                {
                                    unit_definition *definition = boardable_tag_get(vehicle->definition_index);
                                    for (long seat_index = 0; seat_index < definition->seats.s_tag_block::count; ++seat_index)
                                    {
                                        unit_seat *seat = &definition->seats.begin()[seat_index];
                                        if (boardable_flags_test(seat->flags, 11))
                                        {
                                            result = true;
                                            break;
                                        }
                                    }
                                    if (result)
                                        break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x1189a8 ?object_get_blip_type@c_chud_motion_sensor@@AAA?AW4e_blip_type@1@J@Z
c_chud_motion_sensor::e_blip_type c_chud_motion_sensor::object_get_blip_type(long object_index)
{
    e_blip_type result = _blip_none;
    long player_index = player_index_from_user_index_render(m_user_index);
    if (player_index != -1 && object_index != -1) {
        player_datum *player = player_get(player_index);
        long team = player->configuration.host.team_index;
        if (object_index == player->unit_index) {
            result = _blip_self;
        } else {
            object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
            if ((1 << object->object_identifier.get_type()) & 0x1003) {
                object_datum *unit = object;
                if ((1 << unit->object_identifier.get_type()) & 2) {
                    if (((unit_datum *)unit)->unit.actor_index != -1 && (unsigned char)::game_team_is_enemy((e_game_team)((unit_datum
                        *)unit)->unit.team, (e_game_team)team)) {
                        result = _blip_enemy;
                    } else if (((unit_datum *)unit)->unit.gunner_object_index != -1) {
                        object_datum *gunner = (object_datum *)object_get_and_verify_type(((unit_datum *)unit)->unit.gunner_object_index, 0x1003);
                        result = (unsigned char)::game_team_is_enemy((e_game_team)((unit_datum *)gunner)->unit.team, (e_game_team)team) ?
                            _blip_enemy : _blip_friendly;
                    } else if (((unit_datum *)unit)->unit.driver_object_index != -1) {
                        object_datum *driver = (object_datum *)object_get_and_verify_type(((unit_datum *)unit)->unit.driver_object_index, 0x1003);
                        result = (unsigned char)::game_team_is_enemy((e_game_team)((unit_datum *)driver)->unit.team, (e_game_team)team) ?
                            _blip_enemy : _blip_friendly;
                    } else {
                        result = _blip_empty;
                    }
                } else {
                    result = (unsigned char)::game_team_is_enemy((e_game_team)((unit_datum *)unit)->unit.team, (e_game_team)team) ? _blip_enemy
                        : _blip_friendly;
                }
            } else if ((1 << object->object_identifier.get_type()) & 0x800) {
                object_datum *device = object;
                if (!game_is_multiplayer()) {
                    result = (unsigned char)::game_team_is_enemy((e_game_team)((creature_datum *)device)->creature.team, (e_game_team)team) ?
                        _blip_enemy : _blip_friendly;
                } else {
                    result = _blip_none;
                }
            } else if ((1 << object->object_identifier.get_type()) & 8) {
                result = _blip_enemy;
            }
        }
    }
    return result;
}

__forceinline bool c_chud_motion_sensor::get_sensor_origin(real_point3d *origin)
{
    long unit_index = -1;
    if (m_user_index != -1)
        unit_index = ((s_user_globals *)tls_player_mapping_globals())->unit_indices[m_user_index];
    bool result = false;
    if (unit_index != -1)
    {
        unit_get_head_position(unit_index, origin);
        result = true;
    }
    return result;
}

// @address 0x133470 ?update_user_properties@c_chud_motion_sensor@@AAAXXZ
void c_chud_motion_sensor::update_user_properties()
{
    if (game_is_multiplayer() && m_user_index >= 0 && m_user_index < 4)
    {
        long player_index = player_index_from_user_index_render(m_user_index);
        if (player_index != -1)
        {
            player_datum *player = player_get(player_index);
            float meters = player->traits.m_sensor_traits.get_motion_tracker_range();
            float radar_distance = meters * 0.328084f;
            set_radar_distance(radar_distance);
            set_tracking_distance((float)(long)((float)ceilf(radar_distance) + 0.5f));
            long setting = player->traits.m_sensor_traits.m_motion_tracker;
            bool a = false, b = false, c = false, d = false, e = false;
            if (setting == 0)
                setting = 3;
            m_tracking_flags = 0;
            switch (setting) {
            case 2: a = true; e = true; break;
            case 3: a = true; b = true; e = true; break;
            case 4: a = true; b = true; c = true; d = true; e = true; break;
            }
            chud_motion_sensor_flags_set(m_tracking_flags, 0, a);
            chud_motion_sensor_flags_set(m_tracking_flags, 1, b);
            chud_motion_sensor_flags_set(m_tracking_flags, 2, c);
            chud_motion_sensor_flags_set(m_tracking_flags, 3, d);
            chud_motion_sensor_flags_set(m_tracking_flags, 4, e);
        }
    }
}

// @address 0x133660 ?update_tracked_entities@c_chud_motion_sensor@@AAAX_N@Z
void c_chud_motion_sensor::update_tracked_entities(bool)
{
    real_point3d origin;
    if (get_sensor_origin(&origin))
    {
        chud_motion_sensor_object_marker_begin();
        for (long index = 0; index < 20; ++index)
        {
            s_tracked_entity *entity = &m_tracked_entities[index];
            if (entity->object_index != -1)
            {
                void *object = object_try_and_get_and_verify_type(entity->object_index, 0xffffffff);
                if (object && object_should_be_tracked(entity->object_index, &origin))
                {
                    chud_motion_sensor_object_mark(entity->object_index);
                    update_tracked_entity(index, &origin);
                }
                else
                {
                    invalidate_tracked_entity(index);
                }
            }
        }
        c_object_iterator<object_datum> object_iterator;
        object_iterator.begin(6155, 1);
        while (object_iterator.next())
        {
            long object_index = object_iterator.get_index();
            if (chud_motion_sensor_object_unmarked(object_index) && object_should_be_tracked(object_index, &origin))
            {
                long tracked_entity_index = new_tracked_entity(object_index);
                if (tracked_entity_index != -1)
                {
                    update_tracked_entity(tracked_entity_index, &origin);
                }
            }
        }
        chud_motion_sensor_object_marker_end();
    }
}

// @address 0x133868 ?object_should_be_tracked@c_chud_motion_sensor@@AAA_NJPBTreal_point3d@@@Z
bool c_chud_motion_sensor::object_should_be_tracked(long object_index, const real_point3d *position)
{
    bool result = false;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
    real_vector3d delta;
    vector_from_points3d(position, &object->bounding_center, &delta);
    delta.k = 0.0f;
    if (magnitude_squared3d(&delta) < m_tracking_distance * m_tracking_distance) {
        if ((1 << object->object_identifier.get_type()) & 8) {
            if (chud_motion_sensor_equipment_is_active_noise_maker(object_index))
                result = true;
        } else {
            long blip_type = object_get_blip_type(object_index);
            if (blip_type != _blip_none) {
                if (game_is_multiplayer()) {
                    switch (blip_type) {
                    case 1: result = true; break;
                    case 2: result = tracking_flag_test(0) || tracking_flag_test(2); break;
                    case 3: result = tracking_flag_test(1) || tracking_flag_test(3); break;
                    case 4: result = tracking_flag_test(4); break;
                    case 5: result = true; break;
                    case 6: result = true; break;
                    case 7: result = true; break;
                    }
                } else {
                    result = true;
                }
            }
        }
    }
    if (object->flags & 1) result = false;
    return result;
}

void c_chud_motion_sensor::update_tracked_entity(long entity_index, const real_point3d *position)
{
    c_chud_motion_sensor * sensor = this;
    // match: 0x133a60: __restrict keeps the entity->delay lwz below the origin.x lfs (removal hoists it above)
    const real_point3d *__restrict observer_position = position;
    s_tracked_entity *entity = &sensor->m_tracked_entities[entity_index];
    real_point3d origin;
    object_get_origin(entity->object_index, &origin);
    entity->age = distance_squared3d(&origin, observer_position);
    if (entity->delay > 0) --entity->delay;
}

// @address 0x1370d8 ?new_tracked_entity@c_chud_motion_sensor@@AAAJJ@Z
long c_chud_motion_sensor::new_tracked_entity(long object_index) {
    float greatest_age = 0.0f;
    long slot = -1;
    for (long index = 0; index < 20; ++index) {
        s_tracked_entity *entity = &m_tracked_entities[index];
        if (entity->object_index != -1) {
            if (entity->age > greatest_age) {
                greatest_age = entity->age;
                slot = index;
            }
        } else {
            slot = index;
            break;
        }
    }
    if (slot == -1) slot = 0;
    if (slot != -1) {
        s_tracked_entity *entity = &m_tracked_entities[slot];
        entity->object_index = object_index;
        entity->delay = 0;
        entity->age = 0.0f;
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
        if (game_is_multiplayer() && game_engine_has_teams() && ((1 << object->object_identifier.get_type()) & 0x1003)) {
            object_datum *unit = object;
            long viewer_index = player_index_from_user_index_render(m_user_index);
            if (((unit_datum *)unit)->unit.player_index != -1 && viewer_index != -1) {
                player_datum *target = player_get(((unit_datum *)unit)->unit.player_index);
                player_datum *viewer = player_get(viewer_index);
                if (target->grace_ticks > 0 &&
                    !(unsigned char)::game_engine_team_is_enemy((e_game_team)viewer->configuration.host.team_index,
                        (e_game_team)target->configuration.host.team_index))
                    entity->delay = game_tick_rate();
            }
        }
    }
    return slot;
}

bool c_chud_motion_sensor::enemies_nearby()
{
    long player_index = player_index_from_user_index_render(m_user_index);
    bool enemies = false;
    if (player_index != -1)
    {
        player_datum *player = player_get(player_index);
        if (player->unit_index != -1)
        {
            for (long entity_index = 0; entity_index < 20; ++entity_index)
            {
                s_tracked_entity *entity = &m_tracked_entities[entity_index];
                if (entity->object_index != -1)
                {
                    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(entity->object_index, ~0ul);
                    if (object && ((1 << object->object_identifier.get_type()) & 0x1003))
                    {
                        e_blip_type blip_type = object_get_blip_type(entity->object_index);
                        if (blip_type == _blip_enemy && entity->age < m_sensor_range * m_sensor_range)
                        {
                            enemies = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    return enemies;
}

// @address 0x4a3e58 ?create_custom_blip@c_chud_motion_sensor@@AAA_NPBTreal_point3d@@PAUs_motion_sensor_blip@1@PAT2@MM@Z
bool c_chud_motion_sensor::create_custom_blip(const real_point3d *origin, s_motion_sensor_blip *blip, real_point3d *point, float forward_x, float forward_y)
{
    bool result = true;
    real_vector3d offset;
    vector_from_points3d(point, origin, &offset);
    offset.k = 0.0f;
    bool in_range = magnitude_squared3d(&offset) < m_sensor_range * m_sensor_range;
    if (!in_range) {
        ::normalize3d(&offset);
        scale_vector3d(&offset, m_sensor_range, &offset);
    }
    blip->x = (-offset.i * forward_y + offset.j * forward_x) * m_sensor_scale;
    blip->y = (offset.i * forward_x + offset.j * forward_y) * m_sensor_scale;
    blip->intensity = in_range ? 5 : 6;
    blip->size = 0;
    blip->shape = in_range ? 1 : 2;
    return result;
}

// @address 0x1342c8 ?object_velocity_exceeds_threshold@@YA_NJ@Z
bool object_velocity_exceeds_threshold(long object_index)
{
    bool result = false;
    float speed = 0.0f;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
    if (!bit_test(object->physics_flags, 9)) {
        real_vector3d velocity;
        ::object_get_velocities(object_index, &velocity, 0);
        float vk = velocity.k * 0.33f;
        float vi;
        float vj;
        if (((1 << object->object_identifier.get_type()) & 0x1003) && ((unit_datum *)object)->unit.player_index != -1 &&
            bit_test(((unit_datum *)object)->unit.unit_flags, 22)) {
            vi = velocity.i * 0.5f;
            vj = velocity.j * 0.5f;
        } else {
            vj = velocity.j;
            vi = velocity.i;
        }
        speed = sqrtf(vi * vi + vj * vj + vk * vk);
    }
    if (((1 << object->object_identifier.get_type()) & 0x1003) && ((unit_datum *)object)->unit.player_index != -1) {
        float factor = 1.05f;
        if (speed >= global_game_globals->player_information[0].sneak_forward * factor)
            result = true;
    } else {
        float threshold = 0.03f;
        if (speed >= threshold)
            result = true;
    }
    return result;
}

namespace {

static __forceinline object_datum *sensor_blip_object_get(long index)
{
    return (object_datum *)object_get_and_verify_type(index, 0xffffffff);
}
static __forceinline float sensor_blip_magnitude_squared3d(const real_vector3d *v)
{
    return v->k * v->k + (v->j * v->j + v->i * v->i);
}
}
__forceinline c_chud_motion_sensor::e_blip_size c_chud_motion_sensor::object_get_blip_size(long object_index)
{
    e_blip_size size = _blip_size_none;
    if (object_index != -1)
    {
        object_datum *object = sensor_blip_object_get(object_index);
        long type_mask = 1 << object->type;
        if (type_mask & 0x1003)
            size = (e_blip_size)TAG_GET(0, unit_definition, object->definition_index)->motion_sensor_blip_size;
        else if (type_mask & 0x800)
            size = (e_blip_size)TAG_GET(0, creature_definition, object->definition_index)->motion_sensor_blip_size;
    }
    return size;
}

__forceinline bool c_chud_motion_sensor::object_is_moving(long object_index) {
        bool multiplayer = game_is_multiplayer();
        e_blip_type type = multiplayer ? object_get_blip_type(object_index) : _blip_none;
        bool always = multiplayer && ((type == _blip_friendly && tracking_flag_test(2)) || (type == _blip_enemy && tracking_flag_test(3)));
        return always ? true : object_velocity_exceeds_threshold(object_index);

}


// @address 0x11df75c g_chud_scripting_globals
// @address 0x133ae8 ?build_new_history@c_chud_motion_sensor@@AAAXPAVc_chud_navpoint_manager@@@Z
void c_chud_motion_sensor::build_new_history(c_chud_navpoint_manager *chud_navpoint_manager)
{
    real_point3d head_position;
    real_point3d blip_position;
    long unit_index = -1;
    if (m_user_index != -1)
        unit_index = ((s_user_globals *)tls_player_mapping_globals())->unit_indices[m_user_index];
    bool have_position = false;
    if (unit_index != -1)
    {
        unit_get_head_position(unit_index, &head_position);
        have_position = true;
    }
    if (!have_position)
        return;
    long contact_index = (m_valid_index + 1) % 10;
    float yaw = player_control_globals->states[m_user_index].action.look.yaw + 1.5707964f;
    float sin_forward = (float)sin((double)-yaw);
    float cos_forward = (float)cos((double)-yaw);
    s_motion_sensor_contact *contact = &m_contacts[contact_index];
    contact->active = 0;
    m_custom_blip_count = 0;
    for (long navpoint_index = 0; navpoint_index < NUMBEROF(chud_navpoint_manager->navpoints); navpoint_index++)
    {
        c_chud_navpoint_manager::s_managed_navpoint *nav_entry = &chud_navpoint_manager->navpoints[navpoint_index];
        if (nav_entry->valid && (((long)nav_entry->flags >> 6) & 1) != 0)
        {
            if (m_custom_blip_count >= 0 && m_custom_blip_count < 20 &&
                create_custom_blip(&nav_entry->position, &m_custom_blips[m_custom_blip_count], &head_position, sin_forward, cos_forward))
            {
                m_navpoint_data[m_custom_blip_count] = nav_entry->alpha;
                m_custom_blip_count = m_custom_blip_count + 1;
            }
        }
    }
    for (long entity_index = 0; entity_index < 20; entity_index++)
    {
        s_tracked_entity *tracked = &m_tracked_entities[entity_index];
        long object_index = tracked->object_index;
        bool skip = false;
        if (object_index != -1 && (((1 << sensor_blip_object_get(object_index)->type) & 8) != 0))
            skip = true;
        if (skip)
            continue;
        long contact_active = contact->active;
        if (contact_active >= 16)
            continue;
        if (!entity_create_blip(entity_index, &contact->blips[contact_active], &head_position, 0, sin_forward, cos_forward))
            continue;
        long new_active = contact->active + 1;
        contact->active = new_active;
        if (new_active >= 16)
            continue;
        long delay = tracked->delay;
        bool duplicate = delay > 0 && delay % 6 == 0;
        if (duplicate)
        {
            s_motion_sensor_blip *new_blip = &contact->blips[new_active];
            s_motion_sensor_blip *old_blip = &contact->blips[new_active - 1];
            new_blip->x_bits = old_blip->x_bits;
            new_blip->y_bits = old_blip->y_bits;
            new_blip->intensity = 7;
            new_blip->size = old_blip->size;
            new_blip->shape = 3;
            contact->active = contact->active + 1;
        }
    }
    s_equipment_slot *slots = g_chud_scripting_globals->equipment_slots;
    long sphere_index = chud_find_slot_containing_point(slots, &head_position, false);
    if (sphere_index != -1)
    {
        int point_count = slots[sphere_index].point_count;
        for (long point_index = 0; point_index < point_count; point_index++)
        {
            equipment_compute_orbit_point((s_equipment_slot *)slots, sphere_index, point_index, &blip_position);
            long active = contact->active;
            if (active < 16 &&
                entity_create_blip(-1, &contact->blips[active], &head_position, &blip_position.xy, sin_forward, cos_forward))
                contact->active = contact->active + 1;
        }
    }
    m_valid_index = contact_index;
}

// @address 0x133e80 ?entity_create_blip@c_chud_motion_sensor@@AAA_NJPAUs_motion_sensor_blip@1@PATreal_point3d@@PATreal_point2d@@MM@Z
bool c_chud_motion_sensor::entity_create_blip(
    long entity_index, s_motion_sensor_blip *blip, real_point3d *origin, real_point2d *override_position, float forward_x, float forward_y)
{
    s_tracked_entity *entity = entity_index == -1 ? 0 : &m_tracked_entities[entity_index];
    bool result = false;
    if (entity_index == -1 || entity->object_index != -1)
    {
    real_vector3d offset = (*global_zero_vector3d);
    object_datum *object = 0;
    if (entity && entity->object_index != -1)
    {
        object = sensor_blip_object_get(entity->object_index);
        offset.i = object->bounding_center.x - origin->x;
        offset.j = object->bounding_center.y - origin->y;
        offset.k = 0.0f;
    }
    if (override_position)
    {
        offset.i = override_position->x - origin->x;
        offset.j = override_position->y - origin->y;
        offset.k = 0.0f;
    }
    bool in_range = sensor_blip_magnitude_squared3d(&offset) < m_sensor_range * m_sensor_range;
    bool drawable = object ? !bit_test(object->damage_flags, 2) : true;
    if (in_range && drawable)
    {
    bool moving = object ? object_is_moving(entity->object_index) : true;
    bool delayed = object ? entity->delay > 0 : false;
    bool activity = false;
    bool occupied = false;
    if (object && ((1 << object->type) & 0x1003))
    {
        if (((unit_datum *)object)->unit.dual_error_ticks[0] || ((unit_datum *)object)->unit.dual_error_ticks[1])
            activity = true;
        object_datum *unit = sensor_blip_object_get(entity->object_index);
        const s_player_equipment_action_state *action = (const s_player_equipment_action_state *)object_header_block_get(unit, &((unit_datum
            *)unit)->unit.action);
        signed char seat = action->active;
        if (seat != 0 && seat != 3)
            occupied = true;
    }
    if (delayed || activity || moving || occupied)
    {
    blip->x = (offset.j * forward_x - offset.i * forward_y) * m_sensor_scale;
    blip->y = (offset.i * forward_x + offset.j * forward_y) * m_sensor_scale;
    blip->intensity = entity_index != -1 ? (unsigned char)object_get_blip_type(entity->object_index) : (unsigned char)_blip_enemy;
    unsigned char size = entity_index != -1 ? (unsigned char)object_get_blip_size(entity->object_index) : (unsigned char)_blip_size_none;
    blip->shape = 0;
    blip->size = size;
    result = true;
    }
    }
    }
    return result;
}
