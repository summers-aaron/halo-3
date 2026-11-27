#ifndef OBJECTS_WIDGETS_WIDGETS_H
#define OBJECTS_WIDGETS_WIDGETS_H

// Functions defined in tu/objectslibxbox/widgets.cpp that other units call.

struct s_data_array;

// The widget data array (thread-local slot).
extern "C" {
extern __declspec(thread) s_data_array *g_widget_data;
}
static __forceinline s_data_array *&tls_widget_data() { return g_widget_data; }

void widgets_dispose_from_old_structure_bsp(unsigned long old_structure_bsp_mask);
void widget_type_new_on_object(long object_index, short type, long definition_index);
void widget_delete_type_datum(long object_index, long datum_index, bool delete_datum);
void widget_submit_transparent(long widget_index);
void widgets_render_opaque(long object_index, long entry_point);
void __tls_set_g_widget_data_allocator(void *allocator);
void widgets_new(long object_index);

#endif // OBJECTS_WIDGETS_WIDGETS_H
