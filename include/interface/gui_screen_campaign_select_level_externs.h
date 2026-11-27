#ifndef INTERFACE_GUI_SCREEN_CAMPAIGN_SELECT_LEVEL_EXTERNS_H
#define INTERFACE_GUI_SCREEN_CAMPAIGN_SELECT_LEVEL_EXTERNS_H

template<long k_maximum_count> class c_static_wchar_string;
class c_gui_widget;
class c_gui_screen_campaign_select_level;

extern "C" bool text_group_get_string(long tag_index, long string_id, c_static_wchar_string<1024> *string);
extern "C" c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, long type);

#endif // INTERFACE_GUI_SCREEN_CAMPAIGN_SELECT_LEVEL_EXTERNS_H
