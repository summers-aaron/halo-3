#ifndef INTERFACE_CHUD_CHUD_WIDGET_DATA_H
#define INTERFACE_CHUD_CHUD_WIDGET_DATA_H

struct s_data_array;

// Per-user widget data allocated by the four CHUD restricted allocators.
extern "C" __declspec(thread) s_data_array *g_chud_widget_data[4];

#endif // INTERFACE_CHUD_CHUD_WIDGET_DATA_H
