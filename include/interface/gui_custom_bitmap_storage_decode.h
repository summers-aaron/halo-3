#ifndef INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_DECODE_H
#define INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_DECODE_H

struct s_nsc_bitmap_item;

extern "C" bool gui_custom_bitmap_storage_decode_image(
    s_nsc_bitmap_item *storage, long unused, const void *image_data, unsigned long image_size, void *base, long limit, long aspect_index);

#endif // INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_DECODE_H
