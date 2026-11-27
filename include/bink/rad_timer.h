#ifndef BINK_RAD_TIMER_H
#define BINK_RAD_TIMER_H

struct s_rad_timer_state
{
    unsigned int base;
    unsigned int last;
};
extern "C" {
extern s_rad_timer_state g_rad_timer_state;
unsigned int RADTimerRead(void);
}

#endif // BINK_RAD_TIMER_H
