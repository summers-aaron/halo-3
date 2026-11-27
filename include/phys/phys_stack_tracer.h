#ifndef INCLUDE_PHYS_STACK_TRACER_H
#define INCLUDE_PHYS_STACK_TRACER_H

// hkStackTracer, defined in tu/hkbase/hkStackTracer.cpp.

#include "phys/phys_base.h"

class phys_stack_tracer : public phys_ref_object {
public:
    phys_stack_tracer();
    virtual ~phys_stack_tracer();
    void dump_stack_trace(const unsigned long *trace, int count,
                        void (*print)(const char *, void *), void *context);
    int get_stack_trace(unsigned long *trace, int capacity);
};

#endif // HAVOK_HK_STACK_TRACER_H
