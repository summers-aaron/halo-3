#ifndef AI_AI_ATOMS_H
#define AI_AI_ATOMS_H

// Functions defined in tu/ailibxbox/ai_atoms.cpp, plus the two atom callbacks compiled into other units.

void cs_approach_reset(long script_index);

struct cs_command;
struct s_atom_control;
struct s_atom_definition;

bool cs_movement_perform(long actor_index, cs_command *command, long command_index, bool *failed);

// Grenade atom start, defined in tu/ailibxbox/behavior.cpp.
bool grenade_start(long actor_index, long unit_index, cs_command *command, s_atom_control *control, long script_index);
// Equipment atom start, defined in tu/objectslibxbox/units.cpp; it ignores the command arguments.
extern "C" unsigned char unit_actor_try_use_equipment(long actor_index, long unit_index);

// Atom definitions indexed by e_ai_atom, in g_atom_definitions order. March 2007 names all but the approach-object
// (6) and equipment (8) atoms, which it lacks.
extern s_atom_definition g_atom_pause;
extern s_atom_definition g_atom_go_to;
extern s_atom_definition g_atom_go_by;
extern s_atom_definition g_atom_go_to_and_face;
extern s_atom_definition g_atom_go_to_and_posture;
extern s_atom_definition g_atom_move_direction;
extern s_atom_definition g_atom_approach_object;
extern s_atom_definition g_atom_grenade;
extern s_atom_definition g_atom_equipment;
extern s_atom_definition g_atom_vehicle;
extern s_atom_definition g_atom_jump;
extern s_atom_definition g_atom_targeted_jump;
extern s_atom_definition g_atom_recording;
extern s_atom_definition g_atom_action;
extern s_atom_definition g_atom_vocalize;
extern s_atom_definition g_atom_die;
extern s_atom_definition g_atom_behavior;
extern s_atom_definition g_atom_fly_to;
extern s_atom_definition g_atom_fly_by;
extern s_atom_definition g_atom_fly_to_and_face;
extern s_atom_definition g_atom_formation;
extern s_atom_definition g_atom_approach;
extern s_atom_definition g_atom_deploy_turret;
extern s_atom_definition g_atom_custom_animation;
extern s_atom_definition g_atom_play_sound;
extern s_atom_definition g_atom_melee;
extern s_atom_definition g_atom_smash;
extern s_atom_definition g_atom_swarm_to;
extern s_atom_definition g_atom_swarm_from;

#endif // AI_AI_ATOMS_H
