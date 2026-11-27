// objectslibxbox/object_types.obj: from the records 0xd64c8 0xe4380 0xfc2e0 0x108530 0x112830 0x2fbf28 0x349058 0x349360 0x349448 0x349560 0x349608 0x3496a8 0x349760
// @flags /TP /O2 /Oc
#include "math/real_math.h"
#include "scenario/scenario_objects.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
// @address 0x349058 ?object_types_initialize@@YAXXZ
// @address 0x112830 ?object_type_delete@@YAXJ@Z
// @address 0x349448 ?object_type_new@@YA_NJPAUobject_placement_data@@PA_N@Z
// @address 0x349560 ?object_type_place@@YAXJPBUs_scenario_object@@@Z
// @address 0x3496a8 ?object_type_preprocess_node_orientations@@YAXJPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@@Z
#include <string.h>
#include <math.h>
#include "animations/animation_interpolation.h"
#include "cache/cache_file_globals.h"
#include "objects/object_type_definition.h"
#include "math/matrix_math_types.h"
#include "tag_files/string_ids.h"


// @address 0x7fc360 first_object_type_definition
object_type_definition *first_object_type_definition;
extern "C" {
// @address 0x73cc34 g_object_types_deleted_object_mask
unsigned long g_object_types_deleted_object_mask = 0xFFFFFFFF;
// @address 0x73d028 g_object_types_deleted_player_mask
unsigned long g_object_types_deleted_player_mask = 0xFFFFFFFF;
// @address 0x73d504 g_unit_entering_state_names
// 660-663 are entering-state string ids with no string_ids.h names yet.
long g_unit_entering_state_names[5] = { _string_id_enter, 660, 661, 662, 663 };
}
object_type_definition *object_type_definitions[14] = {
 (object_type_definition *)0x8273d318, (object_type_definition *)0x8273d410,
 (object_type_definition *)0x8273d620, (object_type_definition *)0x8273d728,
 (object_type_definition *)0x8273de08, (object_type_definition *)0x8273d820,
 (object_type_definition *)0x8273d920, (object_type_definition *)0x8273dd00,
 (object_type_definition *)0x8273df00, (object_type_definition *)0x8273da18,
 (object_type_definition *)0x8273db10, (object_type_definition *)0x8273dff8,
 (object_type_definition *)0x8273e0f0, (object_type_definition *)0x8273e1e8
};


static __forceinline long object_type_from_group_tag(unsigned long group_tag) {
 long result = -1;
 for (long type = 0; type < 14; type++) {
  object_type_definition *definition = object_type_definition_get((e_object_type)type);
  if (definition->group_tag == group_tag) { result = type; break; }
 }
 return result;
}
static __forceinline long object_type_from_definition_index(long definition_index) {
 unsigned long group_tag = tag_get_group_tag(definition_index);
 return object_type_from_group_tag(group_tag);
}
static __forceinline unsigned long object_types_flag(long bit) { return 1UL << bit; }
void object_types_initialize() {
 object_type_definition **link = &first_object_type_definition;
 for (long type = 0; type < 14; type++) {
  object_type_definition *definition = object_type_definition_get((e_object_type)type);
  *link = definition;
  link = &definition->next;
  for (short part_index = 0; (unsigned short)part_index < 16; part_index++) {
   object_type_definition *part = definition->parts[part_index];
   if (!part) break;
   definition->inherited_mask |= part->inherited_mask;
   if (!part->next) {
    *link = part;
    link = &part->next;
   }
  }
 }
 *link = 0;
 for (object_type_definition *definition = first_object_type_definition; definition; definition = definition->next) {
  if (definition->initialize) definition->initialize();
 }
 g_object_types_deleted_object_mask = 0;
 g_object_types_deleted_player_mask = 0;
 for (long type = 0; type < 14; type++) {
  object_type_definition *definition = object_type_definition_get((e_object_type)type);
  if (definition->handle_deleted_object)
      g_object_types_deleted_object_mask |= object_types_flag(type);
  else
      g_object_types_deleted_object_mask &= ~object_types_flag(type);
  if (definition->handle_deleted_player)
      g_object_types_deleted_player_mask |= object_types_flag(type);
  else
      g_object_types_deleted_player_mask &= ~object_types_flag(type);
  for (long part_index = 0; part_index < 16; part_index++) {
   object_type_definition *part = definition->parts[part_index];
   if (part) {
    definition->inherited_mask |= part->inherited_mask;
    if (part->handle_deleted_object) g_object_types_deleted_object_mask |= object_types_flag(type);
    if (part->handle_deleted_player) g_object_types_deleted_player_mask |= object_types_flag(type);
   }
  }
 }
 for (long type = 0; type < 14; type++) {
  object_type_definition *definition = object_type_definition_get((e_object_type)type);
  unsigned long bit = object_types_flag(type);
  definition->mask = 0;
  for (long other = 0; other < 14; other++) {
   if (object_type_definition_get((e_object_type)other)->inherited_mask & bit) definition->mask |= object_types_flag(other);
  }
 }
}
bool object_type_update(long object_index) {
 object_datum *object_data = OBJECT_GET(object_index, object_datum, _object_mask_all);
 object_type_definition *definition = object_type_definition_get(object_data->object_identifier.get_type());
 bool result = false;
 long count = 0;
 object_type_definition *types[16];
 for (short type_index = 0; definition->parts[type_index] && count < 16; type_index++) {
  types[count++] = definition->parts[type_index];
  object_type_definition *type = definition->parts[type_index];
  if (type->update) result |= type->update(object_index);
 }
 for (short type_index = (short)(count - 1); type_index >= 0; type_index--) {
  if (types[type_index]->post_update) result |= types[type_index]->post_update(object_index);
 }
 return result;
}
bool object_type_compute_function_value(long object_index, long p1, long p2, float *p3, bool *p4, bool *p5) {
 object_datum *object_data = OBJECT_GET(object_index, object_datum, _object_mask_all);
 object_type_definition *definition = object_type_definition_get(object_data->object_identifier.get_type());
 bool result = false;
 *p3 = 0.0f;
 *p4 = false;
 for (long type_index = 0; !result && definition->parts[type_index]; type_index++) {
  object_type_definition *type = definition->parts[type_index];
  if (type->compute_function_value) result = type->compute_function_value(object_index, p1, p2, p3, p4, p5);
 }
 if (!result) {
  *p3 = 0.0f;
  *p4 = false;
 }
 return result;
}
void object_type_fix_transform(long object_index, real_point3d *position, real_vector3d *forward, real_vector3d *up) {
 object_datum *object_data = OBJECT_GET(object_index, object_datum, _object_mask_all);
 object_type_definition *definition = object_type_definition_get(object_data->object_identifier.get_type());
 for (long type_index = 0; definition->parts[type_index]; type_index++) {
  object_type_definition *type = definition->parts[type_index];
  if (type->fix_transform) type->fix_transform(object_index, position, forward, up);
 }
}
void object_type_fix_transform_from_physics(long object_index, real_matrix4x3 *transform) {
 object_datum *object_data = OBJECT_GET(object_index, object_datum, _object_mask_all);
 object_type_definition *definition = object_type_definition_get(object_data->object_identifier.get_type());
 for (long type_index = 0; definition->parts[type_index]; type_index++) {
  object_type_definition *type = definition->parts[type_index];
  if (type->fix_transform_from_physics) type->fix_transform_from_physics(object_index, transform);
 }
}
void object_type_delete(long object_index) {
 object_datum *object_data = OBJECT_GET(object_index, object_datum, _object_mask_all);
 object_type_definition *definition=object_type_definition_get(object_data->object_identifier.get_type());
 for(short type_index=0;definition->parts[type_index];type_index++) {
  object_type_definition *type_definition=definition->parts[type_index];
  if(type_definition->delete_proc && object_data->type_part_flags.test(type_index)) {
   type_definition->delete_proc(object_index);
   object_data->type_part_flags.set(type_index,false);
  }
 }
}
void object_type_adjust_placement(object_placement_data *placement) {
 long type = object_type_from_definition_index(placement->definition_index);
 if (type != -1) {
  object_type_definition *definition = object_type_definition_get((e_object_type)type);
  for (short type_index = 0; definition->parts[type_index]; type_index++) {
   object_type_definition *type_definition = definition->parts[type_index];
   if (type_definition->adjust_placement) type_definition->adjust_placement(placement);
  }
 }
}
bool object_type_new(long object_index, object_placement_data *placement, bool *out_of_objects) {
 object_datum *object_data = OBJECT_GET(object_index, object_datum, _object_mask_all);
 object_type_definition *definition = object_type_definition_get(object_data->object_identifier.get_type());
 bool success = true;
 object_data->type_part_flags.clear();
 for (short type_index = 0; definition->parts[type_index]; type_index++) {
  object_type_definition *type_definition = definition->parts[type_index];
  if (type_definition->new_proc) {
   object_data->type_part_flags.set(type_index, success);
   success = success && type_definition->new_proc(object_index, placement, out_of_objects);
  }
 }
 return success;
}
void object_type_place(long object_index,const s_scenario_object *scenario_object) {
 object_datum *object_data = OBJECT_GET(object_index, object_datum, _object_mask_all);
 object_type_definition *definition=object_type_definition_get(object_data->object_identifier.get_type());
 for (short type_index=0; definition->parts[type_index]; type_index++) {
  object_type_definition *type=definition->parts[type_index];
  if (type->place) type->place(object_index,scenario_object);
 }
}
void object_type_unplace(long object_index) {
 object_datum *object_data = OBJECT_GET(object_index, object_datum, _object_mask_all);
 object_type_definition *definition = object_type_definition_get(object_data->object_identifier.get_type());
 for (short type_index = 0; definition->parts[type_index]; type_index++) {
  object_type_definition *type = definition->parts[type_index];
  if (type->unplace) type->unplace(object_index);
 }
}
void object_type_preprocess_node_orientations(long object_index, const c_static_flags<255> *node_mask, long node_count,
    real_orientation *node_orientations) {
 object_datum *object_data = OBJECT_GET(object_index, object_datum, _object_mask_all);
 object_type_definition *definition = object_type_definition_get(object_data->object_identifier.get_type());
 for (short type_index = 0; definition->parts[type_index]; type_index++) {
  object_type_definition *type = definition->parts[type_index];
  if (type->preprocess_node_orientations) type->preprocess_node_orientations(object_index, node_mask, node_count, node_orientations);
 }
}
// @rdata 0x8202490c
static const float k_object_types_ratio_epsilon = 0.0001f;
// @address 0x2fbf28 object_type_preprocess_node_orientations
extern "C" void object_type_preprocess_node_orientations(const c_interpolator_control *control, long node_count,
    const c_static_flags<255> *flags, const real_orientation *orientations, real_orientation *result_orientations)
{
 const real_orientation *orientation = orientations;
 if (control->enabled())
 {
  float ratio = control->get_ratio();
  if (fabsf(ratio - 1.0f) >= k_object_types_ratio_epsilon)
  {
   for (short node_index = 0; node_index < node_count; node_index++, orientation++, result_orientations++)
   {
    if (!flags || flags->test(node_index))
     orientation_interpolate(orientation, result_orientations, ratio, result_orientations);
   }
  }
 }
}

void object_type_fix_transform_to_physics(long object_index, real_matrix4x3 *transform) {
 object_datum *object_data = OBJECT_GET(object_index, object_datum, _object_mask_all);
 object_type_definition *definition = object_type_definition_get(object_data->object_identifier.get_type());
 for (long type_index = 0; definition->parts[type_index]; type_index++) {
  object_type_definition *type = definition->parts[type_index];
  if (type->fix_transform_to_physics) type->fix_transform_to_physics(object_index, transform);
 }
}
