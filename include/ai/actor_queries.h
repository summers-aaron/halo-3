#ifndef AI_ACTOR_QUERIES_H
#define AI_ACTOR_QUERIES_H

union real_point3d;
union real_vector3d;
struct actor_iterator;
struct actor_datum;

extern "C" bool giant_is_animation_state_current(long giant_index);
extern "C" bool actor_can_fire_while_moving(long actor_index);
extern "C" bool actor_is_blind(long unit_index);
extern "C" unsigned char actor_is_deaf(long actor_index);
extern "C" short actor_get_area_flags(long actor_index);
extern "C" short actor_get_area_type(long actor_index);
extern "C" int actor_pureform_get_firing_point_selection_form(long actor_index);
extern "C" void actor_change_squad(long actor_index, short squad_index);

#endif // AI_ACTOR_QUERIES_H
