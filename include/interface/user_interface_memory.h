#ifndef INTERFACE_USER_INTERFACE_MEMORY_H
#define INTERFACE_USER_INTERFACE_MEMORY_H

// Functions defined in tu/interfacelibxbox/user_interface_memory.cpp that other units call.

void *user_interface_malloc_tracked(unsigned int size, const char *file, long line);
void user_interface_free(const void *pointer);

enum e_map_memory_configuration;
extern "C" void user_interface_memory_initialize(e_map_memory_configuration configuration);
void user_interface_memory_dispose();

// Destroys an object made by the tracked new and frees its storage. March instantiates it per type
// (ui_track_delete<c_gui_data>, <c_gui_screen_widget>, ...; ASSERT("object!=NULL") at interface\user_interface_memory.h:78);
template<class t_type>
inline void ui_track_delete(t_type *object)
{
    object->~t_type();
    user_interface_free(object);
}

#endif // INTERFACE_USER_INTERFACE_MEMORY_H
