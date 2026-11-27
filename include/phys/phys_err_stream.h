#ifndef INCLUDE_PHYS_ERR_STREAM_H
#define INCLUDE_PHYS_ERR_STREAM_H

#include "phys/phys_out_stream.h"

class phys_err_stream : public phys_out_stream {
public:
    phys_err_stream(void *buffer, int size);
};

// Functions defined in tu/hkbase/hkError.cpp.

extern "C" void phys_error_message(const char *message);

class phys_ref_object;

void phys_remove_reference_error(const phys_ref_object *object, const char *name);

#endif // HAVOK_HK_ERROR_H
