#ifndef INTERFACE_GUI_START_MENU_HQ_SERVICE_RECORD_EXTERNS_H
#define INTERFACE_GUI_START_MENU_HQ_SERVICE_RECORD_EXTERNS_H

#include "networking/online/online_url_version.h"

// External dependencies used by gui_start_menu_hq_service_record.
class c_gui_widget;
enum e_gui_widget_type;
class c_gui_screen_widget;
struct s_service_record_identity;
struct s_player_configuration;

extern "C" c_gui_widget *gui_widget_get_next_sibling_of_type(c_gui_widget *widget, e_gui_widget_type type);
extern "C" void gui_screen_widget_set_text_widget_text(c_gui_screen_widget *screen, long name, const wchar_t *text);
extern "C" void service_record_identity_initialize_from_player(s_service_record_identity *identity, const s_player_configuration *player_configuration);
extern "C" const wchar_t k_next_rank_experience_tag[];
extern "C" const wchar_t k_next_rank_skill_tag[];
extern "C" c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, e_gui_widget_type type);
extern "C" void gui_widget_set_alternate_state(c_gui_widget *widget, unsigned char value);


#endif // INTERFACE_GUI_START_MENU_HQ_SERVICE_RECORD_EXTERNS_H
