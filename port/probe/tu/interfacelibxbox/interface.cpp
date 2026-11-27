// @address 0x11df8d8 g_first_person_weapon_data
// interfacelibxbox/interface.obj: from the records 0x47c298 0x47c348 0x47c3b0 0x47c3f8
// @flags /TP /O2
// @address 0x47c298 ?interface_initialize@@YAXXZ
// @address 0x7c1b44 ?g_first_person_weapons_allocator@@3V?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_first_person_weapons_allocator@@YAXPAX@Z@@A
// @address 0x7c1b50 ?g_first_person_weapon_orientations_allocator@@3V?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_first_person_weapon_orientations_allocator@@YAXPAX@Z@@A

#include "interface/first_person_weapons.h"
#include "interface/interface.h"
#include "rasterizer/rasterizer_main.h"
#include "cache/restricted_memory_regions.h"
#include "interface/user_interface_objectives.h"
#include "game/players.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "../../splitscreen_view.h"
#include "math/real_math.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "rasterizer/rasterizer_dx9_dynamic_geometry.h"
#include "interface/interface_globals.h"

namespace {

static __forceinline void interface_objectives_clear()
{
    for (long index = 0; index < 10; ++index)
        g_objectives_state[index] = 0;
    for (long index = 0; index < 10; ++index)
        g_objectives_state[index + 10] = 0;
}

}

void interface_initialize(void)
{
    g_first_person_weapon_data = (fpw_first_person *)g_first_person_weapons_allocator
        .reserve_memory("fp weapons", 0, 0x90c0, 0);
    g_first_person_weapon_orientation_data = g_first_person_weapon_orientations_allocator
        .reserve_memory("fp orientations", 0, 0x8000, 4);
    g_objectives_state = (signed char *)g_objectives_allocator
        .reserve_memory("campaign objectives", 0, 0x14, 0);
}

// @address 0x5bec98 ?interface_dispose@@YAXXZ
void interface_dispose(void)
{
}

// @address 0x47c348 ?interface_initialize_for_new_map@@YAXXZ
void interface_initialize_for_new_map()
{
    first_person_weapons_initialize_for_new_map();
    interface_objectives_clear();
}

static __forceinline void interface_first_person_weapons_dispose_from_old_map(void)
{
    for (long user_index = 0; user_index < 4; user_index++)
        first_person_weapon_attach(user_index, -1, true);
}

static __forceinline void interface_user_interface_objectives_dispose_from_old_map(void)
{
}

// @address 0x47c3b0
void interface_dispose_from_old_map(void)
{
    interface_first_person_weapons_dispose_from_old_map();
    interface_user_interface_objectives_dispose_from_old_map();
}

static __forceinline void interface_splitscreen_draw_rectangle(short top, short left, short bottom, short right)
{
    point2d points[4];
    points[0].x = left;
    points[0].y = top;
    points[1].x = right;
    points[1].y = top;
    points[2].x = right;
    points[2].y = bottom;
    points[3].x = left;
    points[3].y = bottom;
    rasterizer_quad_screenspace(points, 0xFF000000, NULL, 0, false);
}

// @address 0x47c3f8 ?interface_splitscreen_render@@YAXXZ
void interface_splitscreen_render(void)
{
    long player_count = players_get_active_user_count();
    if (player_count > 1)
    {
        short width;
        short height;
        if (rasterizer_main_get_video_mode()->fIsWideScreen)
        {
            width = (short)c_rasterizer::render_globals.width;
            height = (short)c_rasterizer::render_globals.height;
        }
        else
        {
            width = 640;
            height = 480;
        }
        long screen_width = width;
        long screen_height = height;

        switch (player_count)
        {
        case 4:
        {
            long half_width = screen_width >> 1;
            interface_splitscreen_draw_rectangle(0, (short)(half_width - 1), height, (short)(screen_width - half_width + 1));
            long half_height = screen_height >> 1;
            interface_splitscreen_draw_rectangle((short)(half_height - 1), 0, (short)(screen_height - half_height + 1), width);
            break;
        }
        case 3:
        {
            long half_height = screen_height >> 1;
            interface_splitscreen_draw_rectangle((short)(half_height - 1), 0, (short)(screen_height - half_height + 1), width);
            interface_splitscreen_draw_rectangle(
                (short)half_height, (short)((screen_width >> 1) - 1), height, (short)(screen_width - (screen_width >> 1) + 1));
            break;
        }
        case 2:
        {
            long half_height = screen_height >> 1;
            interface_splitscreen_draw_rectangle((short)(half_height - 1), 0, (short)(screen_height - half_height + 1), width);
            break;
        }
        }

        if (rasterizer_main_get_video_mode()->fIsWideScreen && (player_count == 2 || player_count == 3))
        {
            const view_config *bounds = &splitscreen_view_config(true, player_count, 0);
            if (bounds->x0 == 0.0f)
                return;
            if (player_count == 2)
            {
                interface_splitscreen_draw_rectangle(-1, -1, height, (short)(width * bounds->x0));
                interface_splitscreen_draw_rectangle(-1, (short)(width * bounds->x1 - 1.0f), height, width);
            }
            else if (player_count == 3)
            {
                interface_splitscreen_draw_rectangle(-1, -1, (short)(height * bounds->y1), (short)(width * bounds->x0));
                interface_splitscreen_draw_rectangle(-1, (short)(width * bounds->x1 - 1.0f), (short)(height * bounds->y1), width);
            }
        }
    }
}
