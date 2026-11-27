// blamlibxbox/recorded_animation_initialize.obj: from the records 0x3010c0
// @flags /TP /O2 /Oc
// @address 0x3010c0 ?recorded_animation_initialize_unit_control@@YAXPAUunit_control_data@@PAPBDE@Z
#include <string.h>
#include "cutscene/recorded_animation_initialize.h"
// The per-version unit-control field lists: each entry is a {type, size, offset}
// run terminated by {0, -1, -1}; versions 0..4 consume the entry prefix.
#if !defined(_XBOX) || defined(__clang__)
// The four lists are the shipped .data at 0x8273e418 / 0x8273e4a8 / 0x8273e4c0 / 0x8273e4d8 (GAME-CHANGES F119);
// the Xbox table holds those addresses. `type` is a byte-swap code pointer in the image (element size noted per
// row); recorded_animation_initialize_unit_control reads only size and offset, so these lists leave it 0.
static const field_definition k_recorded_animation_unit_control_fields_v0[] = {
    { 0, 4, 0 }, { 0, 2, 4 },                    // element size 1
    { 0, 4, 16 }, { 0, 2, 6 },                   // element size 2
    { 0, 1, 8 }, { 0, 1, 9 },                    // element size 1
    { 0, 2, -1 },                                // element size 2, skipped
    { 0, 8, 20 },                                // real_vector2d
    { 0, 12, 40 }, { 0, 12, 52 }, { 0, 12, 64 }, // real_vector3d
    { 0, -1, -1 },
};
static const field_definition k_recorded_animation_unit_control_fields_v1[] = { { 0, 4, 32 }, { 0, -1, -1 } };
static const field_definition k_recorded_animation_unit_control_fields_v2[] = { { 0, 2, 10 }, { 0, -1, -1 } };
static const field_definition k_recorded_animation_unit_control_fields_v3[] = { { 0, 2, 12 }, { 0, -1, -1 } };
#endif
extern "C" {
// @address 0x73e4f0 k_recorded_animation_unit_control_fields
const field_definition *k_recorded_animation_unit_control_fields[4] = {
#if defined(_XBOX) && !defined(__clang__)
 (const field_definition *)0x8273e418,
 (const field_definition *)0x8273e4a8,
 (const field_definition *)0x8273e4c0,
 (const field_definition *)0x8273e4d8,
#else
 k_recorded_animation_unit_control_fields_v0,
 k_recorded_animation_unit_control_fields_v1,
 k_recorded_animation_unit_control_fields_v2,
 k_recorded_animation_unit_control_fields_v3,
#endif
};
}

#include "math/real_math.h"
#include "units/unit_control.h"


void recorded_animation_initialize_unit_control(unit_control_data *control,const char **stream,unsigned char version) {
 memset(control,0,sizeof(*control));
 control->zoom_level=-1;
 for(short i=0;i<MAX(version,1);i++) {
  const field_definition *field=k_recorded_animation_unit_control_fields[i];
  while(field->size!=-1) {
   if(field->offset!=-1) memcpy((char *)control+field->offset,*stream,field->size);
   *stream+=field->size;
   field++;
  }
 }
}
