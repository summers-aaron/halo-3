#ifndef SCENARIO_SCENARIO_RUNTIME_SYMBOLS_H
#define SCENARIO_SCENARIO_RUNTIME_SYMBOLS_H

extern "C" void portal_activation_initialize_for_new_structure_bsp(unsigned long structure_bsp_index);
extern "C" void cache_file_tag_resource_reset_active_state();
extern "C" void cache_file_tag_resource_set_active_state(void *manager, long scenario_index, long zone_set_data, void *state);

extern "C" unsigned long g_active_designer_zone_mask;
extern "C" unsigned int g_active_cinematic_zone_mask;

#endif // SCENARIO_SCENARIO_RUNTIME_SYMBOLS_H
