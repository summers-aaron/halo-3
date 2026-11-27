#ifndef MAIN_MAIN_RENDER_H
#define MAIN_MAIN_RENDER_H

#include "objects/object_reference_types.h"

// Functions defined in tu/blamlibxbox/main_render.cpp.

void main_render_purge_pending_messages();
void main_render_process_messages();
void main_render();
void main_render_purge_effect_messages(bool flush_pending);
void main_render_update_last_milliseconds();
extern "C" void main_render_update_loading_progress(float value);
extern "C" unsigned char main_render_test_loading_buttons(void);
extern "C" void main_render_draw_message_screen(const wchar_t *text);
extern "C" void main_render_draw_loading_screen();

class c_tag_resource_controller_page_table_io_listener;
struct s_main_render_address_interface;
class c_tag_resource_cache_controller;
typedef c_tag_resource_cache_controller s_main_render_resource_controller;
struct c_visitor;

extern "C" void main_render_submit_model_section(c_visitor *target, unsigned long tag_handle, long section_index);
extern "C" void main_render_process_demanded_resource(s_main_render_resource_controller *controller, long external_handle, long optional,
	s_main_render_address_interface *address, c_tag_resource_controller_page_table_io_listener *listener, bool *load);
void main_render_game();


extern "C" void main_render_predict_tag_resources(c_visitor *visitor, long tag_index);

extern "C" void main_render_predict_scenario_tags(c_visitor *collector, long scenario_index, s_cluster_reference cluster_reference);

struct s_main_render_view_layout;

extern "C" s_main_render_view_layout *main_render_get_view_layout(s_main_render_view_layout *layout);

#endif // MAIN_MAIN_RENDER_H
