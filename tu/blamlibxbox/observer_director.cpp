// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
// @address 0x11df8cc ?g_observer_globals@@3PAUs_player_globals@@A
// blamlibxbox/observer_director.obj: from the records 0x201558 0x201588 0x293350 0x293aa0 0x293b10 0x2940a8 0x2940f0 0x2941d0 0x2942f8 0x294460 0x294af0 0x294e40 0x295040
// @flags /TP /O2 /Oc
#include "game/game.h"
#include "camera/camera_globals.h"
#include "camera/observer_globals_types.h"
#include "units/biped_types.h"
#include "items/weapon_definitions.h"
#include "memory/tls_data_arrays.h"
#include "scenario/scenario_tag.h"
#include "game/materials.h"
#include "physics/collision_types.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "motor/motor_system.h"
#include "scenario/scenario_trigger_volumes.h"
#include "scenario/scenario_definitions.h"
#include "camera/director.h"
#include "camera/game_director.h"
#include "game/game_engine_sandbox.h"
#include "interface/user_interface_controller.h"
#include "saved_games/saved_film.h"
#include "scenario/scenario_soft_ceilings.h"
#include "camera/observer_director.h"
#include "game/players.h"
#include "game/players_globals.h"
#include "camera/camera.h"
#include "camera/orbiting_camera.h"
#include "tag_files/tag_groups.h"
#include "interface/c_window_manager.h"
#include "camera/director_vec3i_types.h"
#include "cache/cache_files.h"
#include "input/input_abstraction.h"
#include "input/input_abstraction_types.h"
#include "structures/cluster_bit_vectors.h"
#include "physics/collision_test_vector.h"
#include "interface/user_interface_utilities.h"
#include <math.h>
#include "game/game_globals_storage.h"
#include "math/matrix_math.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "memory/static_arrays.h"
#include "objects/object_reference_types.h"
#include "shell/shell.h"
#include "physics/collision_bsp_types.h"
#include "tag_files/tag_types.h"
#include "physics/collision_result_types.h"
#include "interface/c_gui_screen_widget.h"
#include <ppcintrinsics.h>
#include "camera/camera_mode_types.h"
#include "game/player_types.h"
#include "game/game_time_types.h"
#include "game/game_time_access.h"
#include <stdlib.h>
#include "physics/collisions.h"
#include "camera/saved_film_director_internals.h"
#include "memory/gameplay_tls_globals.h"
#include "game/players_pvs.h"
#include "math/real_constant_pool.h"
#include "physics/collision_sphere_queries.h"
#include "saved_games/saved_film_recording_types.h"
#include "interface/user_interface_window_manager.h"
struct s_director_vec3i;
// @address 0x2941d0 observer_director_can_use_first_person











// @address 0x201558 ?handle_deleted_player@c_director@@UAAXJ@Z
void c_director::handle_deleted_player(long target)
{
    if (watched_player_index == target)
        watched_player_index = -1;
    get_camera()->handle_deleted_player(target);
}

// @address 0x201588 observer_director_is_free_camera
bool observer_director_is_free_camera(const c_observer_director *director)
{
    c_camera *camera = director->get_camera();
    bool result = false;
    switch (camera->get_type())
    {
    case _camera_mode_flying:
    case _camera_mode_scripted:
        result = true;
        break;
    case _camera_mode_authored:
        result = camera->get_target() == -1;
        break;
    }
    return result;
}

// @address 0x2940a8 ?should_draw_hud@c_observer_director@@UBA_NXZ
bool c_observer_director::should_draw_hud() const
{
    return m_mode != 2 && !observer_director_is_free_camera(this);
}

// @address 0x2940f0 ?should_draw_hud_saved_film@c_observer_director@@UBA_NXZ
bool c_observer_director::should_draw_hud_saved_film() const
{
    bool result = false;
    if (!saved_film_try_and_get_timer_elapsed(0))
        result = m_mode == 0;
    return result;
}



extern "C" unsigned char observer_director_can_use_first_person(int p0)
{
    bool tr3;
    tr3 = 0;
    if (p0 != -1) {
        s_object_header *header = datum_get<s_object_header>(object_header_data, p0);
        if ((1 << header->object_type & 4099) != 0) {
            unit_datum *unit = (unit_datum *)header->object;
            int tr11 = -1;
            signed char tr10;
            tr10 = unit->unit.current_weapon_index;
            if (tr10 != -1) {
                tr11 = unit->unit.weapon_object_indices[tr10];
            }
            bool tr31;
            tr31 = 1;
            if (tr11 != -1) {
                tr31 =
                    _rotl(~TAG_GET('weap', const weapon_definition, datum_get<s_object_header>(object_header_data, tr11)->object->definition_index)->flags,
                        19) &
                    1;
            }
            int l50[4];
            if ((unsigned int)tr31 != 0 && ((long)::director_game_desired_perspective(p0, (e_seat_state *)((long *)(char *)&l50))) == 1) {
                tr31 = 0;
            }
            if ((unsigned int)tr31 != 0 && (_rotl(unit->object.damage_flags, 30) & 1) != 0) {
                tr31 = 0;
            }
            if ((unsigned int)tr31 != 0 && (unsigned char)game_is_cooperative() != 0) {
                return players_is_object_player_local(p0);
            } else {
                tr3 = tr31;
            }
        }
    }
    return tr3;
}

// @address 0x293aa0 ??0c_observer_director@@QAA@J@Z
c_observer_director::c_observer_director(long user_index) : c_director(user_index)
{
    set_camera_mode(_camera_mode_first_person, 0.0f, 0);
    m_initial_camera = 0;
    m_mode = 0;
    m_camera_mode = 3;
    m_follow_any_player = false;
}



static __forceinline bool observer3_unit_desires_third_person(long unit_index)
{
    bool result = false;
    if (unit_index != -1) {
        s_object_header *header = datum_get<s_object_header>(object_header_data, unit_index);
        if ((1 << header->object_type) & 0x1003)
            result = (bool)(_rotl(~header->object->damage_flags, 30) & 1);
    }
    return result;
}

static __forceinline long observer3_player_unit(player_datum *player)
{
    long unit_index;
    if (player->unit_index == -1)
        unit_index = player->dead_unit_index;
    else
        unit_index = player->unit_index;
    return unit_index;
}

// @address 0x2942f8 observer_director_update_camera_mode
extern "C" void observer_director_update_camera_mode(c_observer_director *director)
{
    long camera_mode = director->get_camera()->get_type();
    unit_datum *object = (unit_datum *)object_try_and_get_and_verify_type(
        director->get_camera()->get_target(), 0x1003);
    if (object && object->unit.player_index != -1 && camera_mode != 7 && camera_mode != 6) {
        player_datum *player = datum_get<player_datum>(player_data, object->unit.player_index);
        long unit_index = observer3_player_unit(player);
        long seat_state[4];
        long perspective = ((long)::director_game_desired_perspective(unit_index, (e_seat_state *)seat_state));
        if (director->m_camera_mode == 3 && perspective == 1) {
            if (observer3_unit_desires_third_person(unit_index))
                director->set_camera_mode(_camera_mode_following, g_camera_globals.following_camera_transition_time, 0);
        } else if (director->m_camera_mode == 3 && perspective == 0 && observer_director_can_use_first_person(unit_index)) {
            director->set_camera_mode(_camera_mode_first_person, g_camera_globals.following_camera_transition_time, 0);
        }
    }
}


static __forceinline s_object_header *observer_director_object_header(long object_index)
{
    return datum_get<s_object_header>(object_header_data, object_index);
}

// @address 0x293350 observer_director_get_dead_camera_mode
extern "C" long observer_director_get_dead_camera_mode(long unit_index, long user_index)
{
    long result = 4;
    if (user_index != -1)
    {
        c_director *user = tls_director_globals()->directors[user_index].get_director();
        long last_active_time = -1;
        bool is_biped = false;
        bool flagged = false;
        bool active = false;
        if (unit_index != -1)
        {
            s_object_header *header = observer_director_object_header(unit_index);
            long type_mask = 1 << header->object_type;
            if (type_mask & 0x1003)
                last_active_time = ((unit_datum *)header->object)->unit.last_active_time;
            if (type_mask & 1)
            {
                is_biped = true;
                flagged = (((biped_datum *)header->object)->biped.flags >> 4) & 1;
            }
        }
        if (user->get_camera()->get_type() == 4)
        {
            if (is_biped && !flagged)
            {
                real_vector3d velocity;
                object_get_velocities(unit_index, &velocity, 0);
                if (!tls_game_globals()->game_lost && !motor_system_in_airborne_state(unit_index))
                {
                    if (velocity.i * velocity.i + (velocity.k * velocity.k + velocity.j * velocity.j) <
                        g_camera_globals.director_idle_speed * g_camera_globals.director_idle_speed)
                        active = (float)(game_time_globals->time - last_active_time) >=
                            (float)game_time_globals->ticks_per_second * g_camera_globals.director_idle_time;
                }
                if (!active && tls_game_globals()->options.game_mode != 1)
                {
                    long controller_index = user_interface_controller_get_rumble_enabled(user_index);
                    if (controller_index != -1)
                    {
                        s_game_input_state *input = &input_abstraction_globals.input_states[controller_index];
                        active = input->look.j != 0.0f || input->look.i != 0.0f || input->throttle.i != 0.0f || input->throttle.j != 0.0f ||
                            input->buttons[14].down_amount() != 0.0f || input->buttons[15].down_amount() != 0.0f;
                    }
                }
            }
            if (!active)
            {
                long target = user->get_camera()->get_target();
                if (target != -1)
                {
                    s_object_header *header = observer_director_object_header(target);
                    if ((1 << header->object_type) & 0x1003)
                        active = ((unit_datum *)header->object)->unit.last_active_time == -1;
                }
            }
            if (active)
                result = 1;
        }
        else if (user->get_camera()->get_type() == 1)
        {
            result = 1;
            if (flagged)
                result = 4;
        }
    }
    return result;
}



static __forceinline const s_observer_result_fields *observer_director_get_camera(long user_index)
{
    return &g_observer_globals->blocks[user_index].result;
}


// @address 0x294af0 observer_director_update_coop_camera_position
extern "C" void observer_director_update_coop_camera_position(c_observer_director *director)
{
    if (!game_is_cooperative())
        return;
    if (!g_observer_globals->blocks[director->m_user_index].result_valid)
        return;
    bool zone_switched = false;
    const s_observer_result *camera_result = &g_observer_globals->blocks[director->m_user_index].camera_result;
    real_point3d position = camera_result->position;
    s_location location;
    scenario_location_from_point(&location, &position);
    bool outside = location.cluster_reference.bsp_index == -1;
    bool volume_clear = scenario_trigger_volumes_test_point(&position) == 0;
    if (tls_game_globals()->options.game_mode == 1 && !outside)
    {
        s_game_cluster_bit_vectors pvs;
        players_compute_combined_pvs(&pvs, false, 0);
        long zone_set_index = global_zone_set_index;
        if (zone_set_index != -1)
        {
            struct scenario *scenario = global_scenario;
            for (long index = 0; index < scenario->zone_set_switch_trigger_volumes.count; index++)
            {
                bool check = false;
                if ((unsigned long)index <= 349)
                    check = ((((s_players_globals *)tls_players_globals())->disabled[index >> 5] & (1 << (index & 31))) == 0);
                if (!check)
                    continue;
                s_scenario_zone_set_switch_trigger_volume *volume = &((s_scenario_zone_set_switch_trigger_volume *)
                    scenario->zone_set_switch_trigger_volumes.address)[index];
                if (volume->commit_zone_set == -1 || volume->commit_zone_set == zone_set_index)
                    continue;
                if (!scenario_trigger_volume_test_point(volume->trigger_volume, &position))
                    continue;
                zone_switched = true;
                break;
            }
        }
    }
    if (!outside && !zone_switched && !volume_clear)
        return;
    long best_unit = -1;
    float best_distance = 3.4028234663852886e+38f;
    c_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    if (!iterator.next())
        return;
    float original_z = position.z;
    float original_y = position.y;
    float original_x = position.x;
    do
    {
        long unit_index;
        if (iterator.get_datum()->unit_index != -1)
            unit_index = iterator.get_datum()->unit_index;
        else
            unit_index = iterator.get_datum()->dead_unit_index;
        if (unit_index != -1)
        {
            s_object_header *header = observer_director_object_header(unit_index);
            bool valid = true;
            if (((1 << header->object_type) & 1) != 0)
                valid = ((~(((biped_datum *)header->object)->biped.flags >> 4)) & 1) != 0;
            if (valid)
            {
                position = header->object->bounding_center;
                float dy = original_y - position.y;
                float dx = original_x - position.x;
                float dz = original_z - position.z;
                float distance = dx * dx + dz * dz + dy * dy;
                if (best_unit == -1 || distance < best_distance)
                {
                    best_unit = unit_index;
                    best_distance = distance;
                }
            }
        }
    } while (iterator.next());
    if (best_unit != -1)
    {
        real_point3d new_position;
        real_vector3d new_direction;
        observer_compute_object_frame(best_unit, &new_position, &new_direction);
        c_camera *camera = (c_camera *)director->m_camera_storage;
        camera->set_position(&new_position);
        camera->set_forward(&new_direction);
        camera->set_roll(0.0f);
        camera->m_camera_state = 5;
        camera->m_camera_flags.set(0, true);
    }
}

// @address 0x294e40 observer_compute_object_frame
extern "C" void observer_compute_object_frame(int object_index, real_point3d *position, real_vector3d *direction)
{
    long next_object_index = object_index;
    long root_object_index = -1;
    if (next_object_index != -1) {
        do {
            root_object_index = next_object_index;
            next_object_index = observer_director_object_header(next_object_index)->object->parent_object_index;
        } while (next_object_index != -1);
    }
    s_object_header *header = observer_director_object_header(root_object_index);
    object_datum *object = header->object;
    float scale = object->bounding_radius;
    const real_matrix4x3 *root_node = (const real_matrix4x3 *)((char *)object + object->node_matrices_offset);
    real_vector3d forward = root_node->forward;
    if (header->object_type == 0)
        forward = ((unit_datum *)object)->unit.looking_vector;
    forward.k *= 0.3f;
    normalize3d(&forward);
    real_vector3d offset;
    offset.i = -forward.i;
    offset.j = -forward.j;
    offset.k = 1.0f - forward.k;
    float magnitude = sqrtf(offset.i * offset.i + offset.j * offset.j + offset.k * offset.k);
    if (magnitude > 0.0001f) {
        scale /= magnitude;
        offset.i *= scale;
        offset.j *= scale;
        offset.k *= scale;
    } else {
        offset = k_real_math_identity_matrix_storage.up;
    }
    real_point3d point;
    // match: 0x294e40: origin/result pointers store point.x before the y load; member adds hoist it
    const real_point3d *origin = &root_node->position;
    real_point3d *result = &point;
    result->x = origin->x + offset.i;
    result->y = origin->y + offset.j;
    result->z = origin->z + offset.k;
    scenario_soft_ceilings_push_point_out(&point, g_camera_globals.collision_radius, 10.0f);
    s_location location;
    scenario_location_from_point(&location, &point);
    if (location.cluster_reference.bsp_index == -1)
        point = root_node->position;
    *position = point;
    *direction = forward;
}

extern "C" {
// match: this caller's two-word flags differ from the object unit's sphere bridge.
}

// @address 0x295040 observer_director_can_place_flying_camera
extern "C" bool observer_director_can_place_flying_camera(real_point3d *point)
{
    bool result = true;
    s_collision_test_flags flags;
    flags.collision_flags = k_collision_test_structure_and_water.collision_flags | 0x100004;
    flags.object_flags = k_collision_test_structure_and_water.object_flags | 1;
    if (scenario_soft_ceilings_push_point_out(point, g_camera_globals.collision_radius, 10.0f))
    {
        if (scenario_soft_ceilings_push_point_out(point, g_camera_globals.collision_radius, 10.0f))
            result = false;
        else if (collision_test_point(flags, point, -1, -1, 0))
            result = false;
        else if (collision_test_sphere(flags, point, g_camera_globals.collision_radius, -1, -1))
            result = false;
    }
    return result;
}


// @address 0x744610 k_observer_director_camera_modes
static e_camera_mode k_observer_director_camera_modes[2] = { _camera_mode_following, _camera_mode_flying };
static __forceinline bool observer_director_valid_slot_count(long count)
{
    bool valid = false;
    // match: 0x294460: without the rotate, the range check fuses into addi/cmplwi/bgtlr; the two-compare shape is required.
    if (count > 0 && (long)_rotl(count, 0) <= 6)
        valid = true;
    return valid;
}
static __forceinline bool observer_director_saved_film_ready()
{
    bool result = false;
    if (saved_film_is_recording()) {
        result = observer_director_valid_slot_count(g_saved_film_recording_globals.state);
    }
    return result;
}
// @address 0x294460 observer_director_handle_camera_input
extern "C" void observer_director_handle_camera_input(c_observer_director *director, s_game_input_state *input)
{
    long camera_type = director->get_camera()->get_type();
    long target = director->get_camera()->get_target();
    c_camera *camera = director->get_camera();
    bool mode_changed = false;
    bool skip_toggle = camera_type == _camera_mode_authored && observer_director_saved_film_ready();
    if (!skip_toggle && camera_type != _camera_mode_scripted && input->buttons[13].down_frames() == 1) {
        long previous = director->m_initial_camera;
        bool failed = false;
        director->m_initial_camera = (director->m_initial_camera - 1) & 1;
        real_point3d focus = observer_director_get_camera(director->m_user_index)->position;
        if (k_observer_director_camera_modes[director->m_initial_camera] == camera_type)
            director->m_initial_camera = (director->m_initial_camera - 1) & 1;
        if (k_observer_director_camera_modes[director->m_initial_camera] == _camera_mode_flying && !observer_director_can_place_flying_camera(&focus)) {
            ::user_interface_play_sound(_ui_sound_error, -1);
            failed = true;
            director->m_initial_camera = previous;
        }
        if (!failed) {
            mode_changed = director->set_camera_mode(k_observer_director_camera_modes[director->m_initial_camera], 0.0f, 0);
            if (k_observer_director_camera_modes[director->m_initial_camera] != _camera_mode_following) {
                camera->set_target(-1);
                camera->set_position(&focus);
                camera->set_forward(&g_observer_globals->blocks[director->m_user_index].result.forward);
                camera->m_camera_state = 5;
                camera->m_camera_flags.set(0, true);
            }
            else {
                bool found = false;
                collision_result collision;
                collision.global_material.m_index = -1;
                s_collision_test_flags flags;
                flags.collision_flags = k_collision_test_camera.collision_flags;
                flags.object_flags = k_collision_test_camera.object_flags | 2;
                real_vector3d offset;
                offset.i = g_observer_globals->blocks[director->m_user_index].result.forward.i * 100.0f;
                offset.j = g_observer_globals->blocks[director->m_user_index].result.forward.j * 100.0f;
                offset.k = g_observer_globals->blocks[director->m_user_index].result.forward.k * 100.0f;
                if (collision_test_vector(flags, false, &g_observer_globals->blocks[director->m_user_index].result.position, &offset, -1, -1, -1, &collision)) {
                    if (collision.object_index != -1) {
                        s_object_header *header = observer_director_object_header(collision.object_index);
                        if (header->object_type == 0) {
                            long player_index = ((unit_datum *)header->object)->unit.player_index;
                            if (player_index != -1) {
                                director_set_watched_player(director, player_index);
                                camera->set_target(collision.object_index);
                                found = true;
                            }
                        }
                    }
                }
                if (!found) {
                    long unit_index = -1;
                    long player_index = c_director::player_get_next_player_with_a_unit(
                        -1, director->watched_player_index, -1, 0, true, true, true, &unit_index);
                    if (player_index != -1) {
                        director_set_watched_player(director, player_index);
                        camera->set_target(unit_index);
                        found = true;
                    }
                }
                if (!found)
                    camera->set_target(-1);
                director->m_camera_mode = 0;
            }
        }
    }
    if (camera_type != _camera_mode_authored && camera_type != _camera_mode_scripted) {
        // match: 0x294460: const reference binding preserves temporary lifetime
        const unsigned long &changed = (unsigned char)mode_changed;
        if (changed)
            return;
        if (input->buttons[8].down_frames() == 1) {
            if (camera->get_type() == _camera_mode_orbiting) {
                if (observer3_unit_desires_third_person(target)) {
                    director->set_camera_mode(_camera_mode_following, 0.5f, 0);
                    camera->set_target(target);
                    director->m_camera_mode = 0;
                    return;
                }
            }
            else if (camera->get_type() == _camera_mode_following) {
                director->set_camera_mode(_camera_mode_orbiting, 0.5f, 0);
                camera->set_target(target);
                director->m_camera_mode = 1;
                return;
            }
        }
        else if (input->buttons[9].down_frames() == 1) {
            if (camera->get_type() == _camera_mode_first_person) {
                director->set_camera_mode(_camera_mode_following, 0.0f, 0);
                camera->set_target(target);
                director->m_camera_mode = 0;
                return;
            }
            if ((camera->get_type() == _camera_mode_following || camera->get_type() == _camera_mode_orbiting) &&
                observer_director_can_use_first_person(target))
            {
                director->set_camera_mode(_camera_mode_first_person, 0.0f, 0);
                camera->set_target(target);
                director->m_camera_mode = 3;
                return;
            }
        }
        else if (camera->get_type() == _camera_mode_following
            && (input->look.j != 0.0f || input->look.i != 0.0f || input->throttle.i != 0.0f || input->throttle.j != 0.0f
                || input->buttons[14].down_amount() != 0.0f || input->buttons[15].down_amount() != 0.0f)) {
            director->set_camera_mode(_camera_mode_orbiting, 0.5f, 0);
            camera->set_target(target);
            static_cast<c_orbiting_camera *>(camera)->set_distance(director->stored_command.focus_distance);
            director->m_camera_mode = 1;
        }
    }
}


// @address 0x293b10 ?update@c_observer_director@@UAAXM@Z
void c_observer_director::update(float dt)
{
    long user_index = m_user_index;
    long controller_index = user_interface_controller_get_rumble_enabled(user_index);
    bool advance_mode = false;
    observer_director_update_camera_mode(this);
    if (controller_index != -1 && camera_responding_gate((e_controller_index)controller_index) == 0)
    {
        s_game_input_state *input = &input_abstraction_globals.input_states[controller_index];
        if (input->buttons[0].access_context() != -1)
            input->buttons[0].set_access_context(-1);
        bool use_secondary = false;
        if (!observer_director_is_free_camera(this))
        {
            if (director_update_watched_player(this))
            {
                m_camera_mode = 0;
                set_camera_mode(_camera_mode_following, 0.0f, 0);
            }
            use_secondary = m_mode == 0;
        }
        if (use_secondary)
            director_update_scoreboard_screen(this);
        else
            director_dispose_controller_screen(this);
        observer_director_handle_camera_input(this, input);
        if (g_window_manager.get_screen_by_name(_console_window, 0x10081) == 0 && input->buttons[11].down_frames() == 1)
            advance_mode = true;
    }
    if (advance_mode)
    {
        // match: 0x293b10: the shared accessor reschedules the dispatch load after the camera-base adjustment.
        long mode = ((c_camera *)m_camera_storage)->get_type();
        if (!observer_director_is_free_camera(this) && mode != _camera_mode_dead)
            m_mode = (m_mode + 1) % 3;
        else
            m_mode = m_mode == 0 ? 2 : 0;
    }
    long camera_type = ((c_camera *)m_camera_storage)->get_type();
    c_camera *camera = (c_camera *)m_camera_storage;
    if (camera_type == _camera_mode_dead)
    {
        long target = camera->get_target();
        if (target != -1 &&
            observer_director_get_dead_camera_mode(target, m_user_index) == 1 &&
            set_camera_mode(_camera_mode_orbiting, 0.0f, 0))
        {
            camera->set_target(target);
            camera->set_forward(&stored_command.forward);
            static_cast<c_orbiting_camera *>(camera)->set_distance(g_camera_globals.orbiting_camera_maximum_distance);
        }
    }
    if (camera->get_type() != _camera_mode_scripted && camera->get_type() != _camera_mode_flying && camera->get_type() != _camera_mode_authored)
    {
        long player_index = watched_player_index;
        if (player_index != -1)
        {
            player_datum *player = datum_get<player_datum>(player_data, player_index);
            if (player->unit_index != -1)
            {
                if (camera->get_target() != player->unit_index)
                {
                    set_camera_mode((e_camera_mode)m_camera_mode, 0.0f, 0);
                    camera->set_target(player->unit_index);
                }
            }
            else if (player->dead_unit_index != -1)
            {
                long mode = camera->get_type();
                if (mode == _camera_mode_first_person || mode == _camera_mode_following)
                {
                    set_camera_mode(_camera_mode_dead, 0.0f, 0);
                    camera->set_target(player->dead_unit_index);
                }
                else if (mode == _camera_mode_orbiting)
                {
                    long dead_unit = player->dead_unit_index;
                    bool valid = object_try_and_get_and_verify_type(dead_unit, 0x1003) != 0;
                    bool is_biped = valid && ((1 << observer_director_object_header(dead_unit)->object_type) & 1) != 0;
                    if (valid && is_biped &&
                        ((unsigned char)(((biped_datum *)observer_director_object_header(dead_unit)->object)->biped.flags >> 4) &
                            1) != 0)
                    {
                        set_camera_mode(_camera_mode_dead, 0.0f, 0);
                        camera->set_target(player->dead_unit_index);
                    }
                }
            }
        }
    }
    if (camera->get_target() != -1)
    {
        m_follow_any_player = 1;
        s_location location;
        object_get_location(camera->get_target(), &location);
        if (location.cluster_reference.bsp_index == -1)
            find_any_player();
    }
    else if (!observer_director_is_free_camera(this))
    {
        bool check_coop = true;
        long coop_index = watched_player_index;
        if (coop_index != -1)
        {
            player_datum *coop_player = datum_get<player_datum>(player_data, coop_index);
            if (coop_player->unit_index == -1 && coop_player->dead_unit_index == -1)
            {
                bool survival = false;
                if (tls_game_globals()->options.simulation_type == 4)
                    survival = true;
                if (survival && game_is_cooperative() && (coop_player->flags & 8) == 0 && game_engine_sandbox_is_active())
                    check_coop = false;
            }
        }
        if (check_coop)
            find_any_player();
    }
    if (camera->get_type() == _camera_mode_flying)
    {
        m_camera.m_flags |= 4;
        observer_director_update_coop_camera_position(this);
    }
    c_director::update(dt);
}
