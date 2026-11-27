#ifndef ITEMS_ITEMS_H
#define ITEMS_ITEMS_H

// Functions defined in tu/objectslibxbox/items.cpp.

bool dangerous_items_near_player(long *result_index);
void item_maintain_z_up(long item_index);
void item_unstow_inventory(long item_index, long parent_object_index, long parent_marker_name, long child_marker_name);
void item_hide_inventory(long item_index);
void item_show_inventory_unattached(long item_index, long unit_index);
void item_detonate(long item_index);
void item_in_unit_inventory(long item_index, long unit_index);
void item_attach_to_node(long parent_object_index, long item_index, short node_index);
void item_detach_from_parent(long item_index);

union real_vector3d;
struct s_variant_object_datum;

extern "C" void variant_object_datum_initialize(s_variant_object_datum *datum);
extern "C" void item_adjust_gravity(long item_index, real_vector3d *gravity);
extern "C" void item_adjust_scale_to_unit(long unit_index, long unused_parameter, long item_index);

#endif // ITEMS_ITEMS_H
