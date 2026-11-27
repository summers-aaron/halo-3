#ifndef CUTSCENE_RECORDED_ANIMATION_INITIALIZE_H
#define CUTSCENE_RECORDED_ANIMATION_INITIALIZE_H

// Functions defined in tu/blamlibxbox/recorded_animation_initialize.cpp that other units call.

struct unit_control_data;

struct field_definition { long type, size, offset; };
extern "C" {
extern const field_definition *k_recorded_animation_unit_control_fields[];
}

void recorded_animation_initialize_unit_control(unit_control_data *control,const char **stream,unsigned char version);

#endif // CUTSCENE_RECORDED_ANIMATION_INITIALIZE_H
