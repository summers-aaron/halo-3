#ifndef INTERFACE_CHUD_CHUD_NAVPOINTS_H
#define INTERFACE_CHUD_CHUD_NAVPOINTS_H

// Functions defined in tu/interfacelibxbox/chud_navpoints.cpp that other units call.

#include "interface/chud/chud_draw_types.h"

// Navpoint text uses the shared CHUD geometry layout.
struct chud_navpoints_widget_geometry : s_widget_geometry {};
union real_rectangle2d;

extern "C" void chud_navpoints_delete_navpoint(short user_index, short navpoint_id);
extern "C" bool chud_compute_text_geometry(chud_navpoints_widget_geometry *geometry, const wchar_t *string,
    long font_index, unsigned char centered, float scale, const real_rectangle2d *clip);

extern "C" long g_chud_navpoint_companion_search_timer;

class c_chud_navpoint_manager;

extern "C" void chud_draw_navpoints(c_chud_navpoint_manager *manager, long user_index);

#endif // INTERFACE_CHUD_CHUD_NAVPOINTS_H
