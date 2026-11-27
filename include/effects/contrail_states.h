#ifndef EFFECTS_CONTRAIL_STATES_H
#define EFFECTS_CONTRAIL_STATES_H

// Functions defined in tu/blamlibxbox/contrail_states.cpp that other units call.

struct s_definition_14;
struct s_object_14;

extern "C" void contrail_states_set_definition(s_object_14 *object, s_definition_14 *definition);
extern "C" void contrail_states_update_values(s_object_14 *object, unsigned long mask);

#endif // EFFECTS_CONTRAIL_STATES_H
