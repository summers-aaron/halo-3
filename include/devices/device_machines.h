#ifndef DEVICES_DEVICE_MACHINES_H
#define DEVICES_DEVICE_MACHINES_H

// Functions defined in tu/objectslibxbox/device_machines.cpp that other units call.

struct s_game_cluster_bit_vectors;

bool machine_door_test_object(long machine_index, long *objects, short count);
bool machine_compute_activation(long object_index, const s_game_cluster_bit_vectors *active, bool *out_active);

struct object_placement_data;
struct s_scenario_machine;
void machine_place(long object_index, s_scenario_machine *scenario_machine);
void machine_unplace(long object_index);
bool machine_new(long object_index, object_placement_data *placement, bool *out_of_memory);
void machine_delete(long object_index);

#endif // DEVICES_DEVICE_MACHINES_H
