// blamlibxbox/synchronized_value.obj: from the records 0x1ac700
// @flags /TP /O2
// @address 0x1ac700 ?set_bit@c_synchronized_long@@QAAJJ_N@Z
#include "multithreading/synchronized_value.h"

long c_synchronized_long::set_bit(long bit, bool enabled)
{
    return enabled ? bitwise_or(1 << bit) : bitwise_and(~(1 << bit));
}
