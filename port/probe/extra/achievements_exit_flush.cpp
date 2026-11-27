// probe only (port side of tu/networkinglibxbox/online_achievements.cpp): achievements the game has granted but not
// written yet are written before the process ends on request.
//
// gamer_grant_achievement only sets a bit in g_achievement_globals.controllers[controller]; achievements_update writes
// the bits later with c_achievement_write_overlapped_task (XUserWriteAchievements), and holds them while a cinematic
// plays and for 2 s after it (retail 0x823a5160), so no Achievement Unlocked toast covers one. A mission's completion
// achievements are granted as its closing scene starts (Sierra 117: 010_jungle_mission_won runs
// game_award_level_complete_achievements, then the outro, game_won, the next mission's load and its opening
// cinematic), so they stay in memory for minutes. On the console the title is not ended there; on a PC the player
// closes the window, and the port ends the process from the present (vulkan_device.cpp, port_exit): those
// achievements were lost, while the profile's progress (written as soon as it changes) was kept.
// port_exit_flush (port/sdk/runtime/port_runtime.h) calls this writer on every orderly way out (the window closed,
// XLaunchNewImage, main returning); never on a fault. It writes what the write task would have written, the same way
// (k_online_achievement_ids, the controller's index as its user index, signed-in controllers only), without the
// overlapped task: the process is ending, the toast does not matter, and XUserWriteAchievements keeps each one once
// (an achievement the game's own write task is writing at the same moment is not recorded twice).
#include "cseries/cseries.h"
#include "interface/achievement_globals_types.h"
#include "interface/controller_types.h"
#include "networking/online/online_achievements.h"
#include "xbox/online_services.h"
#include "runtime/port_runtime.h"

namespace {

void achievements_exit_flush()
{
    for (unsigned long controller = 0; controller < 4; controller++) {
        const s_controller_achievements &pending = g_achievement_globals.controllers[controller];
        XUSER_ACHIEVEMENT achievements[49];
        unsigned long count = 0;
        for (unsigned long achievement = 0; achievement < 49; achievement++) {
            if ((pending.flags[achievement >> 5] & (1ul << (achievement & 31))) == 0)
                continue;
            achievements[count].dwUserIndex = controller;
            achievements[count].dwAchievementId = k_online_achievement_ids[achievement];
            count++;
        }
        if (count != 0 && g_controller_globals.controllers[controller].is_signed_in_to_machine())
            XUserWriteAchievements(count, achievements, 0);
    }
}

struct s_achievements_exit_flush_registration {
    s_achievements_exit_flush_registration() { port_set_exit_flush(achievements_exit_flush); }
} g_achievements_exit_flush_registration;

} // namespace
