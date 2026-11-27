#ifndef OBJECTS_MODEL_CUSTOMIZATION_H
#define OBJECTS_MODEL_CUSTOMIZATION_H

// Functions defined in tu/objectslibxbox/model_customization.cpp that other units call.

struct object_placement_data;
struct s_player_appearance;
struct s_multiplayer_customized_model_character;
enum e_player_model_choice;

s_multiplayer_customized_model_character *get_multiplayer_customized_model_character(e_player_model_choice player_model_choice);

void model_customization_get_overrides_for_placement(struct s_player_appearance const * _sym_p0, struct object_placement_data * _sym_p1);

#endif // OBJECTS_MODEL_CUSTOMIZATION_H
