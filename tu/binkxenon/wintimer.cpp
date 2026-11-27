// binkxenon/wintimer.obj: from the records 0x712db0
// @flags /TP /O2 /Oc

#include "cseries/crt_xdk.h"
#include "bink/rad_timer.h"
// @address 0x11df2e0 g_rad_timer_state
// @address 0x712db0 RADTimerRead
unsigned int RADTimerRead(void)
{
    unsigned int now = GetTickCount();
    if (g_rad_timer_state.base == 0)
        g_rad_timer_state.base = now;
    unsigned int elapsed = now - g_rad_timer_state.base;
    unsigned int result = g_rad_timer_state.last;
    if (elapsed - result <= 0xC0000000) {
        g_rad_timer_state.last = elapsed;
        result = elapsed;
    }
    return result;
}
