#ifndef OBJECTS_WIDGETS_WIDGET_DEBUG_H
#define OBJECTS_WIDGETS_WIDGET_DEBUG_H

void antenna_render_debug(long object_index, long antenna_index);
void cloth_render_debug(long object_index, long cloth_index);
void leaf_system_delete(long leaf_system_index);
void leaf_system_render_debug(long object_index, long leaf_system_index);
bool leaf_system_get_is_transparent(long leaf_system_index);

#endif // OBJECTS_WIDGETS_WIDGET_DEBUG_H
