#ifndef AI_OBJECTIVES_H
#define AI_OBJECTIVES_H

extern "C" short g_objectives_next_task_index;

// Functions defined in tu/ailibxbox/objectives.cpp.

union real_vector3d;
struct squad_datum;

void objectives_update(void);
bool squad_valid_task(struct squad_datum * _sym_p0);
void objective_update_leadership(short objective_index, short task_index, short mode, long actor_index);
void objective_task_break(short objective_index, short task_index);
void objective_task_request_engage(short objective_index, short task_index, bool force);
void objective_task_request_group_grenade(short objective_index, short task_index);
bool objective_task_get_direction(short objective_index, short task_index, real_vector3d *direction);
bool ai_scripting_set_task_condition(long ai_index, bool condition);
bool ai_scripting_leadership(long ai_index);
bool ai_scripting_leadership_all(long ai_index);
short ai_task_count(long ai_index);
bool objective_task_is_vehicle_task(long objective_index, long task_index);
bool squad_set_objective(long squad_index, short objective_index, short task_index, bool temporary);
void objectives_initialize_for_new_map();

struct s_task;
struct s_task_group;

extern "C" long task_get_movement(short team, s_task *task);
extern "C" short task_group_compute_next_state(s_task_group *group, float *a, float *b);
extern "C" void objective_handle_death(short objective_index,short task_index,long actor_index);
extern "C" short ai_scripting_task_status(long ai_index);
extern "C" bool ai_scripting_set_task(long ai_index,long objective_name,long task_name);

union real_point3d;
struct s_squad_info;
struct s_task_info;
struct s_task_record;

void __tls_set_g_tasks_data_allocator(void *value);
bool objective_decide(long objective_index);
bool decide_task_group(short objective_index, short parent_index, s_task_info *info, float *scores, long *valid);
bool decide_tasks_priority_group(short objective_index, short parent_index, short current_priority_index,
	short current_count, short index, s_task_info *info, float *scores, long *valid);
void disallowed_tasks_unassign_squads(short objective_index, short root_task_index, s_task_info *task_info,
	unsigned long *disallowed_task_flags, s_squad_info *squad_list, short *squad_count);
float task_score(short objective_index, short task_index, short squad_index, short group, s_task_info *task_info, float *scores, long *valid);
bool find_assignment(short objective_index, short first_task_index, short task_count, short group,
	s_squad_info *squad_list, short squad_count, s_task_info *task_info, unsigned long *disallowed_tasks, float *scores, long *valid);
void objective_task_break_nearby(short objective_index, short task_index, short team, short highest_rank);
const real_point3d *objective_task_get_position(short objective_index, short task_index);
void finalize_task_change(long squad_index);
void disallow_task(short objective_index, short task_index, unsigned long *disallowed_tasks);
bool test_filter(long squad_index, s_task *task, s_task_info *task_info, s_task_record *task_record, short threshold, bool allow, bool *valid);
void task_update(short objective_index, short task_index);
bool objective_task_test_activation(short objective_index, short task_index, struct s_task_info const * task_info, bool update, bool * exhausted);
void objective_update(long objective_index);

struct actor_datum;
struct s_task_squad_iterator;
struct task_actor_iterator;

void task_actor_iterator_new(short objective_index, short task_index, task_actor_iterator *iterator);
squad_datum *task_squad_iterator_next(s_task_squad_iterator *iterator);
actor_datum *task_actor_iterator_next(task_actor_iterator *iterator);
void task_squad_iterator_new(s_task_squad_iterator *iterator, short objective_index, short task_index, bool include_child_tasks, bool include_something);
extern "C" squad_datum *task_actor_iterator_get_next_squad(task_actor_iterator *iterator);

enum e_squad_source;
bool squad_list_add(short, e_squad_source, s_task_info *, s_squad_info *, short *);

struct s_task_iterator;
s_task *task_iterator_next(s_task_iterator *iterator);

#endif // AI_OBJECTIVES_H
