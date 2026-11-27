#ifndef RENDER_RENDER_WATER_H
#define RENDER_RENDER_WATER_H

// Functions defined in tu/graphicslibxbox/render_water.cpp.

void __tls_set_g_water_interaction_event_allocator(void *address);
void render_water_set_enabled_flags(bool interaction, bool refraction);
extern "C" int water_interaction_ripples_find_best_offset(int current_offset, int offset_count, float reuse_bias);
extern "C" void water_renderer_draw_composite();
extern "C" void water_renderer_handle_interaction_events(unsigned long event_count);

// Data defined in tu/graphicslibxbox/render_water.cpp that other units use.
extern long g_water_interaction_event_count;
extern long g_water_render_structure_part_indices[];

void water_interaction_ripples_initialize(void);
void water_interaction_ripples_dispose(void);
void water_interaction_ripples_clear(void);
#endif // RENDER_RENDER_WATER_H
