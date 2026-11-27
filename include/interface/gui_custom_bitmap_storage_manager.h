#ifndef INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_MANAGER_H
#define INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_MANAGER_H

struct s_data_array;

class c_gui_custom_bitmap_storage_manager
{
public:
    long allocate_bitmap(long width, long height, bool use_compressed_format);

private:
    s_data_array *m_bitmap_data;
};

typedef char gui_custom_bitmap_storage_manager_size[(sizeof(void *) != 4 || sizeof(c_gui_custom_bitmap_storage_manager) == 4) ? 1 : -1];

struct s_screenshot_capture_storage;
extern "C" void gui_custom_bitmap_storage_capture_screenshot(s_screenshot_capture_storage *storage);

#endif // INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_MANAGER_H
