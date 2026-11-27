#ifndef AI_AI_SCRIPT_H
#define AI_AI_SCRIPT_H

// Functions defined in tu/ailibxbox/ai_script.cpp.

struct actor_datum;
struct ai_index_actor_iterator;
struct ai_index_squad_iterator;
struct squad_datum;
struct ai_script_context
{
    long ai_index;
    long object_index;
};

extern ai_script_context global_script_context;

long ai_index_get_actor(long ai_index);
void ai_index_actor_iterator_new(long ai_index, ai_index_actor_iterator *iterator);
actor_datum *ai_index_actor_iterator_next(ai_index_actor_iterator *iterator);
long ai_index_get_vehicle(long ai_index);
void ai_index_squad_iterator_new(long ai_index, struct ai_index_squad_iterator * iterator);
squad_datum *ai_index_squad_iterator_next(ai_index_squad_iterator *iterator);
bool ai_handle_script_verification(bool enable);
long object_list_from_ai_reference(long reference);
void ai_scripting_attach_unit(long unit_index, long ai_index);
void ai_scripting_cannot_die(long ai_index, bool cannot_die);
bool ai_scripting_vitality_pinned(long ai_index);
long ai_scripting_get_object(long ai_index);
long ai_scripting_get_turret_ai(long ai_index, short turret_index);
long ai_scripting_nearest_point(long object_index, long point_reference);
long ai_scripting_random_point(long point_reference, float minimum_distance, float maximum_distance, float angle);
void ai_scripting_erase(long ai_index);
void ai_scripting_set_deaf(long ai_index, bool deaf);
void ai_scripting_set_blind(long ai_index, bool blind);
void ai_magically_see(long ai_index, long other_ai_index);
void ai_magically_see_object(long ai_index, long object_index);
long ai_scripting_count_internal(long ai_index, short count_type, long *out_alive_count, float *out_health);
short ai_fighting_count(long ai_index);
void ai_scripting_migrate_internal(long ai_index, long squad_index, bool, bool);
void ai_scripting_allegiance(short team1, short team2);
short ai_enter_squad_vehicles(long ai_index);
void ai_vehicle_enter_internal(long ai_index, long vehicle_index, long seat_set, bool force);
void ai_scripting_exit_vehicle(long ai_index, long seat);
void ai_scripting_renew(long ai_index);
short ai_play_line(long ai_index, long dialogue_line);
short ai_play_line_at_player(long p0, long p1);
bool cs_command_script_running(long ai_index, long script_index);
void ai_scripting_place_internal(long ai_index, bool force);
void ai_scripting_place2_internal(long ai_index, short count, bool force);
void ai_scripting_set_team(long ai_index, short team);
void ai_scripting_braindead_by_unit(long object_list_index, bool braindead);
void ai_scripting_kill_internal(long ai_index, bool silent, bool multiplayer_cinematic);
extern "C" bool ai_index_test_squad(long a, long b);
extern "C" bool ai_vehicle_has_player(long vehicle_index);
extern "C" short ai_vehicle_count(long ai_index);
extern "C" int ai_task_get_count(long reference);
extern "C" void ai_scripting_migrate_infantry(long ai_index, long squad_index);
extern "C" void ai_scripting_prefer_target(long object_list_index, bool enable);
extern "C" void ai_scripting_set_actor_value(long ai_index, short value, unsigned char flag);
extern "C" void ai_scripting_teleport(long ai_index, long point_ref);
extern "C" void ai_scripting_force_active(long ai_index, bool force_active);
extern "C" bool ai_vehicle_reserve(long unit_index, bool reserve);
extern "C" short ai_play_line_on_object(long object_index, long dialogue_id);
struct s_duration_sort_entry { short index; short priority; };
extern "C" bool ai_script_duration_compare(s_duration_sort_entry p0, s_duration_sort_entry p1, const void *context);
extern "C" void ai_update_activation();

struct unit_seat_source;

short ai_assign_actors_to_seats(const long *actor_indices, short num_actors, const unit_seat_source *seats, short num_seats, bool force);

bool ai_vehicle_reserve_seat(long ai_index, long seats, bool reserve);

enum e_hs_type;
long object_index_from_ai(long ai_index, e_hs_type type);

#endif // AI_AI_SCRIPT_H
