#ifndef AI_SQUADS_H
#define AI_SQUADS_H

#include "ai/pureform_types.h"

#include <string.h>

#include "units/action_request.h"
#include "ai/squad_types.h"

// Styles every squad allows; set in full when ai initializes.
extern "C" s_style_flags style_all;

// An action request zeroed on construction, for squad placement and vehicle entry.
struct squads_action_request : action_request {
    squads_action_request() { memset(this, 0, sizeof(*this)); }
};

// Functions defined in tu/ailibxbox/squads.cpp.

struct actor_datum;
struct actor_starting_location_definition;
struct s_tag_block;
struct s_task;
struct squad_actor_iterator;
struct squad_area_iterator;
struct squad_datum;

struct s_task * squad_get_task(struct squad_datum * squad);
short squad_area_iterator_next(squad_area_iterator *iterator);
void squad_actor_iterator_new(squad_actor_iterator *iterator, long squad_index, bool active_only);
struct actor_datum * squad_actor_iterator_next(struct squad_actor_iterator * iterator);
void squads_initialize_for_new_map(void);
void starting_location_new(struct actor_starting_location_definition * starting_location);
void ai_handle_squad_placement(long squad_index);
void squad_remove_vehicle(long _sym_p0, long datum_index);
bool squad_update_style(long squad_index, short style_global_index);
void squad_get_actor_character_definition_index(short zone_index, short squad_index, short location_index, long * out);
void squad_set_actor_starting_location(long actor_index, short squad_index, short member_index, short start_index);
bool squad_consider_major_upgrade(short squad_index, float chance);
bool squad_force_activate(long squad_index);
bool squad_activate(long squad_index);
void squad_deactivate(long squad_index);
void squad_attach_actor(long actor_index, short squad_index, bool preserve_team);
void squad_detach_actor(long actor_index, bool died);
void squadless_attach_actor(long actor_index);
float squad_adoption_distance_to_actor(long squad_index, long actor_index, short area_type);
void squads_update_dirty_status(void);
void actor_update_squad_status(long index);
void squads_test_activation(long ai_index);
void squad_load_in_squad(long squad_index, long destination_index);
void squad_update_status(long squad_index);
void calculate_dynamic_areas(long squad_index, s_tag_block *tag_block, float falloff);
void squads_update(void);
void squad_create(long squad_index, short fire_team_index, short location_index, short count, bool force, bool collect, bool created_by_script);
extern "C" void squadless_detach_actor(int actor_index);

struct area_definition;
struct squad_definition;
struct squad_group_squad_iterator;

extern "C" bool squad_test_area_type(short area_type, area_definition *area);
extern "C" void __tls_set_g_squad_data_allocator(void *object);
void squad_new(squad_definition *definition);
short squad_placement_find_seat(long p0, short p1, long * p2, short * p3);
long squad_placement_find_vehicle(short vehicle_type, short seat_type, long first_vehicle, short * vehicle_seat_index);
extern "C" squad_datum *squad_group_squad_iterator_next(squad_group_squad_iterator *iterator);
long squad_actor_adopt_squad(long actor_index, short area_type);
extern "C" void major_upgrade_chance(long character_index, short type, bool *force_major, bool *is_random,
    float *random_chance);
extern "C" void squad_vehicle_change_squad(unsigned int datum_index, int owner_index);
bool squad_area_iterator_new(squad_area_iterator *iterator, short squad_index, short flags, short area_type, bool enabled_only, e_pureform pureform);

void __tls_set_g_squad_group_data_allocator(void *address);

#endif // AI_SQUADS_H
