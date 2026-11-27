#ifndef BINK_RADCB_ABI_H
#define BINK_RADCB_ABI_H

extern "C" {
// match: 0x712a10: Keep the long-pointer callback parameter; this TU cannot include bink_sdk.h.
void RADCB_suspend_callback(long, long *);
// match: 0x7129b8: Retain the accepted boolean result ABI; radcb.h declares an integer result.
bool RADCB_try_to_suspend_callback(unsigned int, long *);
// match: 0x712a40: Keep the long-pointer callback parameter; this TU cannot include bink_sdk.h.
void RADCB_resume_callback(long, long *);
void RADCB_idle_on_callbacks(void);
}

#endif
