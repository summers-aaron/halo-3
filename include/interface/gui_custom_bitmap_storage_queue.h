#ifndef INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_QUEUE_H
#define INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_QUEUE_H

#include "saved_games/autosave_queue_types.h"

typedef char gui_bitmap_queue_size[(sizeof(void *) != 4 || sizeof(s_autosave_queue) == 0x8e08) ? 1 : -1];
#endif
