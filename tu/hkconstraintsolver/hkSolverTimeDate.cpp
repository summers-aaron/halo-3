// hkconstraintsolver/hkSolverTimeDate.obj: from the records 0x6777f8 0x677800
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x677800 phys_solver_system_clock

#include "cseries/crt_xdk.h"
#include "phys/phys_solver_time_date.h"

extern "C" __int64 time(__int64 *timer)
{
    return _time64(timer);
}

extern "C" __int64 phys_solver_system_clock(void)
{
    return time(0);
}

