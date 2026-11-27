#ifndef AI_ACTOR_TYPE_PUREFORM_H
#define AI_ACTOR_TYPE_PUREFORM_H

#include "ai/pureform_types.h"

// Functions defined in tu/ailibxbox/actor_type_pureform.cpp that other units call.

struct s_pureform_distribution;
struct s_tag_block;

struct actor_type_pureform_preference
{
    float maximum_distance;
    e_pureform forms[3];
};

extern "C" {
extern actor_type_pureform_preference k_pureform_distance_preferences[3];
}

e_pureform actor_get_current_form(long actor_index);
e_pureform character_get_form(long tag_index);
bool actor_allowed_to_morph(long actor_index, bool force, e_pureform form);
bool actor_morph_impl(long actor_index, e_pureform form, bool force, bool immediate);
bool actor_morph_inhibited(long actor_index, e_pureform form);
bool pureform_pathfind_to_firing_position(
    long actor_index, e_pureform pureform, bool *reached, bool *failed, float *distance, bool special, bool use_former_position);
bool actor_set_pending_form_impl(long actor_index, e_pureform form, bool force, e_morph_priority priority, bool immediate);

bool ai_scripting_morph(long actor_index, short form);
void squad_handle_morph_redistribution(long squad_index, s_pureform_distribution *distribution, s_tag_block *area_set);
bool actor_handle_morph(long actor_index, long object_index, long character_index, bool exact);
extern "C" int actor_pureform_get_firing_point_selection_form(int actor_index);

#endif // AI_ACTOR_TYPE_PUREFORM_H
