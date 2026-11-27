#include "effects/decal_message_queue_tls.h"
#ifndef FIRST_PERSON_WEAPONS_DEPENDENCIES_H
#define FIRST_PERSON_WEAPONS_DEPENDENCIES_H
#include "game/player_control_globals.h"
#include "interface/interface_globals.h"
#include "camera/saved_film_director_storage.h"
#include "render/object_render_message_queue_tls.h"
#include "units/biped_globals.h"

struct fpw_first_person;
struct s_controllers;
#include "game/game_time_types.h"
struct s_player_control_globals;

struct real_matrix4x3;
extern "C" void biped_apply_inverse_kinematics(long unit_render_model, long unit_node_index, real_matrix4x3 *unit_marker_matrix,
                             real_matrix4x3 *weapon_marker_matrix, float blend, long node_count, real_matrix4x3 *unit_nodes);

#endif // FIRST_PERSON_WEAPONS_DEPENDENCIES_H
