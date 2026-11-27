#ifndef BITMAPS_BITMAP_ACCESS_H
#define BITMAPS_BITMAP_ACCESS_H

struct bitmap_data;
class c_rasterizer_texture_ref;

bitmap_data *get_bitmap_and_hardware_format(long bitmap_index, long sequence_index, long frame_index, c_rasterizer_texture_ref *texture);

#endif // BITMAPS_BITMAP_ACCESS_H
