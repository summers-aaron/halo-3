#ifndef EFFECTS_EFFECT_CREATION_DEPENDENCIES_H
#define EFFECTS_EFFECT_CREATION_DEPENDENCIES_H

class c_tag_index;
struct effect_part_definition;
struct real_matrix4x3;

extern "C" int effect_new_unattached_from_point(c_tag_index, char *, char *, char *, int, int, char *, char *);
extern "C" void ai_handle_effect_creation(long, long, const effect_part_definition *, const real_matrix4x3 *, bool);

#endif // EFFECTS_EFFECT_CREATION_DEPENDENCIES_H
