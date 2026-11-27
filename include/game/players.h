#ifndef GAME_PLAYERS_H
#define GAME_PLAYERS_H

// Functions defined in tu/blamlibxbox/players.cpp.
extern "C" bool players_is_player_local(long player_index);

struct game_options;
struct game_player_options;
struct player_action;
union real_euler_angles2d;
union real_point3d;
union real_vector3d;
struct s_campaign_armaments;
struct s_machine_identifier;
struct s_player_action_context;
struct s_player_configuration;
struct s_player_interaction;
struct s_unit_camera_info;
struct simulation_update;

long players_get_active_and_in_game_count(bool include_respawning);
void players_remap_local_players_from_flags(long flags);
void player_build_aiming_vector_from_facing(long player_index, const real_euler_angles2d *facing, real_vector3d *aiming_vector);
long player_index_from_user_index(long user_index);
void player_action_clear(player_action *action);
void player_control_set_facing(long player_index, const real_vector3d *facing);
bool players_joined_in_progress_and_waiting_to_respawn(void);
void player_force_spawn(long player_index);
void players_update_for_checkpoint(void);
long players_first_active_user(void);

#include "game/player_mapping_globals.h"
#include "game/player_mapping_types.h"

// Local inline body for inlined players_first_active_user call sites, named apart from
// the out-of-line function: this build emits bl for cross-unit calls, so inlined sites
// need this copy.
static __forceinline long inl_players_first_active_user(void)
{
    s_user_globals *players = player_mapping_globals;
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

// Local inline body for inlined next_active_user call sites, named apart from the
// private static in players.cpp: this build emits bl for cross-unit calls, so inlined
// sites need this copy.
static __forceinline long inl_next_active_user(long user_index)
{
    long result = -1;
    if (user_index == -1)
        user_index = 0;
    else
        user_index++;
    for (long index = user_index; index < 4; index++)
    {
        if (player_mapping_globals->player_indices[index] != -1)
        {
            result = index;
            break;
        }
        else
        {
            continue;
        }
    }
    return result;
}
#include "game/player_active_users.h"
void player_copy_object_appearance(long player_index, long object_index);
void player_swap(long absolute_player_index1, long absolute_player_index2);
long player_try_and_get_player_index_from_absolute_player_index(long absolute_player_index);
void player_find_player_character_unit_and_variant_info(long player_index, long * unit_definition_index, long * variant_index);
bool players_any_are_near_death(long *unit_index);
bool player_interaction_exists(long unused, unsigned long type_mask, struct s_player_interaction const * reference);
void players_build_persistent_player_options(game_options *options);
void player_set_facing(long player_index, const real_vector3d *facing);
extern "C" bool player_move_unit_to_position(long player_index, long unit_index, bool teleported, const real_point3d *position);
bool game_engine_is_player_in_edit_mode(long player_index, long *object_index, long *editor_index);
void player_set_unit_index(long player_index, long unit_index);
void players_store_persistent_campaign_data_on_game_won(s_campaign_armaments *armaments);
void player_set_configuration(long player_index, const s_player_configuration *player_configuration);
void unit_get_camera_info(long index,s_unit_camera_info *out);
void players_set_local_machine(const s_machine_identifier *machine_identifier);
void players_set_machines(unsigned long new_machine_valid_mask, const s_machine_identifier *new_machine_identifiers);
long player_new(long player_array_index, const game_player_options *options, bool initial_spawn);
void player_rejoined_game(long player_index, const game_player_options *options, bool initial_spawn);
void players_detach_from_map();
void player_leave_game_internal(long player_index);
void players_update_before_game(const simulation_update *update);
void players_update_after_game(const struct simulation_update *);
void player_find_action_context(long player_index, s_player_action_context *out_action_context);
bool player_spawn(long player_index, const real_point3d *position, const float *rotation);
extern "C" void players_set_local_player(long input_user_index, long player_index);
extern "C" void player_handle_death(long player_index);
extern "C" void player_delete(long player_index);

union real_rgb_color;

extern "C" int players_get_active_user_count(void);
extern "C" long player_get_user_from_controller(long controller_index);
// match: 0x131c80: pointer __restrict qualifier prevents alias reload
extern "C" real_rgb_color *player_profile_get_rgb_color(real_rgb_color * __restrict color, long index);
extern "C" bool players_any_are_in_the_air(long *out_unit_index);

struct player_iterator;
struct s_persistent_campaign_player;
struct s_player_appearance;
extern "C" void player_throw_equipment(long unit_index, long unused, const real_point3d *point, const real_vector3d *direction);
struct s_aim_assist_targeting_result;
struct cs_point;
struct s_scenario_game_state;

extern "C" bool players_test_targeting_bounds(s_aim_assist_targeting_result *record);
extern "C" bool player_function_get_function_value(long player_index, long field, float *value, bool *valid);
extern "C" long player_get_first_output_user(long index);
void __tls_set_g_players_globals_allocator(void *value);
void __tls_set_g_player_data_allocator(void *value);
extern "C" void player_increment_teleport_counter(unsigned long player_index);
extern "C" bool players_is_zone_transition_active(void);
extern "C" void player_update_input_target(long player_index, player_action *input);
extern "C" void players_detach_units_from_object(long object_index);
void player_prepare_action(long player_index, struct player_action * action);
extern "C" bool players_is_object_player_local(long object_index);
extern "C" bool players_is_object_campaign_companion(long object_index);
void __tls_set_g_player_mapping_globals_allocator(void *address);
extern "C" void players_reset_mapping_globals(void);
extern "C" long players_get_next_user_with_player(long user_index);
extern "C" void players_set_unit_for_player_views(int index, int value);
extern "C" void players_remove_local_player(long player_index);
extern "C" void players_set_controller_mapping(int active_index, int value);
extern "C" bool player_iterate_output_users(player_iterator *iterator);
extern "C" void players_set_user_unit(int user_index, int unit_index);
extern "C" bool players_is_zone_loading_required(const s_scenario_game_state *zone_state, long zone_set_index);
bool players_arm_unit_from_persistent_campaign_data(struct s_persistent_campaign_player const * _sym_p0, long _sym_p1);
extern "C" bool player_appearance_valid(const s_player_appearance *appearance);
extern "C" short players_get_nearest_player_distance(const cs_point *scenario_point);
extern "C" bool player_is_action_valid(player_action *action);
extern "C" bool players_is_zone_transition_valid(long transition_index, long source_index, long unused, long previous_index);
void players_update_activation(void);
extern "C" void players_remap_cooperative_users(int value);
extern "C" unsigned char player_handle_flip_interaction(long player_index, const s_player_interaction *interaction);
extern "C" void players_set_input_controller(long player_index, long controller_index);
extern "C" float player_compute_aim_score(long p0, long p1);
extern "C" void player_update_unit_after_teleport(long player_index, long source_index, bool teleported);
extern "C" void players_set_input_user(long player_index, long input_user_index);
extern "C" void players_remap_local_players(s_machine_identifier *local_machine_identifier, game_player_options *local_players);
extern "C" bool player_fix_unit_position(long player_index, long unit_index, const real_point3d *position, bool single_pass, bool teleported);
extern "C" void player_update_coop_respawn(long player_index);

struct unit_weapon_pickup_result;

bool player_find_best_spawn_location(long player_index, real_point3d *location, float *facing, float *pitch, bool single_player, bool initial_spawn);
extern "C" void players_death_status_calculate(unsigned char * a, unsigned char * b);
void player_approve_pickup_weapon(long player_index, long weapon_index, unit_weapon_pickup_result *pickup_result);
bool player_should_auto_pickup_weapon(long player_index, long weapon_index);

extern "C" int player_get_team_index(unsigned int index);

void players_initialize(void);
void players_dispose(void);
void players_initialize_for_new_map(void);
void players_dispose_from_old_map(void);
void players_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
void players_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);
void player_positions_initialize(void);
void player_positions_dispose(void);
void player_positions_initialize_for_new_map(void);
void player_positions_dispose_from_old_map(void);
void player_positions_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
void player_positions_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);

#endif // GAME_PLAYERS_H
