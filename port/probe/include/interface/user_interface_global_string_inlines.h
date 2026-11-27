#ifndef INTERFACE_USER_INTERFACE_GLOBAL_STRING_INLINES_H
#define INTERFACE_USER_INTERFACE_GLOBAL_STRING_INLINES_H

#include "interface/user_interface.h"
#include "interface/user_interface_shared_globals.h"

// PC port: text_group.cpp defines extern "C" bool text_group_get_string; declaring int reads dirty upper %eax on i686.
extern "C" bool text_group_get_string(int controller, int id, wchar_t *buffer);

static __forceinline int user_interface_global_string_get_unchecked(int id, wchar_t *buffer)
{
    int result = 0;
    const s_user_interface_shared_globals *shared;
    buffer[0] = 0;
    shared = ::user_interface_shared_tag_globals_try_and_get();
    if (shared && shared->global_strings.index != -1)
        result = text_group_get_string(shared->global_strings.index, id, buffer);
    return result;
}

#endif // INTERFACE_USER_INTERFACE_GLOBAL_STRING_INLINES_H
