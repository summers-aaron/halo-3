#ifndef PHYSICS_HAVOK_LISTENERS_H
#define PHYSICS_HAVOK_LISTENERS_H

#include "phys/phys_error.h"
#include "memory/static_arrays.h"
#include <stddef.h>

// March 2007 havok_listeners.cpp: hkError base, disabled ids at 0x08, count at 0x48.
class c_havok_error : public phys_error {
public:
    __forceinline c_havok_error() : count(0) {}
    virtual int message(phys_error::error_message severity, int id, const char *description, const char *file, int line);
    virtual void error_set_enabled(int id, phys_bool enabled);
    virtual phys_bool is_error_enabled(int id);
    virtual void enable_all_reports();

    c_static_array<long, 16> disabled;
    long count;
};

typedef char havok_size_c_havok_error[(sizeof(void *) != 4 || ((sizeof(c_havok_error) == 0x4c) )) ? 1 : -1];
typedef char havok_offset_c_havok_error_disabled[(sizeof(void *) != 4 || (offsetof(c_havok_error, disabled) == 8)) ? 1 : -1];
typedef char havok_offset_c_havok_error_count[(sizeof(void *) != 4 || (offsetof(c_havok_error, count) == 0x48)) ? 1 : -1];

// Globals defined in tu/blamlibxbox/havok_listeners.cpp.

extern "C" c_havok_error g_hk_error;

#endif // PHYSICS_HAVOK_LISTENERS_H
