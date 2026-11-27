#ifndef INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_XENON_H
#define INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_XENON_H

// Functions defined in tu/interfacelibxbox/gui_custom_bitmap_storage_xenon.cpp.

#include <stdlib.h>

struct s_gui_custom_bitmap_storage_xenon;

extern "C" void gui_custom_bitmap_storage_set_buffer(
    s_gui_custom_bitmap_storage_xenon *storage, unsigned long width, unsigned long height, uintptr_t data_address, long trailing_size);
extern "C" bool gui_custom_bitmap_storage_encode_image(
    s_gui_custom_bitmap_storage_xenon *storage, long, long, void *output_destination, unsigned long output_size,
    void *base, long limit, unsigned long *out_size);

#endif // INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_XENON_H
