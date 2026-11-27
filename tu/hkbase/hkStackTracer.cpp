// hkbase/hkStackTracer.obj: from the records 0x610238 0x610250 0x610258 0x610270
// @compiler xdk6130
// @flags /TP /O1
// @address 0x610238 ?dump_stack_trace@phys_stack_tracer@@QAAXPBKHP6AXPBDPAX@Z2@Z
// @address 0x610250 ?get_stack_trace@phys_stack_tracer@@QAAHPAKH@Z
// @address 0x610258 ??0phys_stack_tracer@@QAA@XZ
// @address 0x610270 ??1phys_stack_tracer@@UAA@XZ
#include "phys/phys_stack_tracer.h"
void phys_stack_tracer::dump_stack_trace(const unsigned long *trace, int count,
                                 void (*print)(const char *, void *), void *context)
{
    print("No stack trace available", context);
}
int phys_stack_tracer::get_stack_trace(unsigned long *trace, int capacity)
{
    return 0;
}
phys_stack_tracer::phys_stack_tracer()
{
}
phys_stack_tracer::~phys_stack_tracer()
{
}

