#ifndef UNITS_VEHICLES_H
#define UNITS_VEHICLES_H

#include "units/vehicle_types.h"

// Functions defined in tu/objectslibxbox/vehicles.cpp.

class c_sector_ref;
union real_point3d;

bool vehicle_is_flipped(long object_index);
bool vehicle_is_running_trick(long object_index);
bool vehicle_physics_disabled(long object_index);
long vehicle_get_ultimate_parent(long object_index);
short vehicle_get_driver_seat(long object_index, long *driver_object_index);
bool vehicle_about_to_detonate_near_any_player(long * vehicle_index);
void vehicle_set_auto_turret(long object_index, long trigger, float minimum_range, float maximum_range, float angle, long owner_index);
void vehicle_hover(long vehicle_index, bool hover);
void vehicle_find_pathfinding_location(long vehicle_index, short *bsp_index, long *surface_index, c_sector_ref *sector_ref,
    real_point3d *position, long *object_index, unsigned long *bsp_reference);
bool vehicle_moving_near_any_player(long *vehicle_index);

class c_vehicle_type_human_plane;
struct render_model_definition;
class c_animation_channel;
typedef c_animation_channel vehicles_animation_channel;

void vehicle_process_animation_channels(long object_index,
    void (*callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *), void *data);

extern "C" void vehicle_impulse_process_animation(
    c_vehicle_type_human_plane *record, long object_index,
    void (*callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *),
    void *user, c_animation_channel *channel, bool refresh);
extern "C" void vehicle_impulse_channel_node_preprocess_callback(long object_index, const render_model_definition *model, c_animation_channel *channel,
    float ratio, float pitch, float weight, void *user_data);
void vehicle_reset(long object_index);
e_vehicle_type vehicle_get_type(long object_index);
extern "C" void vehicle_handle_deleted_object(unsigned int vehicle_object_index, int deleted_object_index);

struct s_scenario_vehicle;
void vehicle_place(long vehicle_index, s_scenario_vehicle *scenario_vehicle);
bool vehicle_compute_function_value(long vehicle_index, long function_name, long function_index, float *value, bool *active, bool *deterministic);

#endif // UNITS_VEHICLES_H
