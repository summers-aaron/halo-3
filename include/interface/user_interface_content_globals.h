#ifndef INTERFACE_USER_INTERFACE_CONTENT_GLOBALS_H
#define INTERFACE_USER_INTERFACE_CONTENT_GLOBALS_H

#include <stddef.h>
#include "memory/static_arrays.h"
#include "interface/c_player_profile.h"

struct s_user_interface_content_globals
{
    struct s_task_slot
    {
        s_task_slot();

        long state;
        c_string_verify_task task;
        c_static_wchar_string<16> name;
        c_static_wchar_string<128> description;
    };
    s_user_interface_content_globals();
    ~s_user_interface_content_globals();

    s_task_slot tasks[3];
    bool was_busy;
    unsigned long started;
};

typedef char user_interface_content_globals_task_slot_size[
    (sizeof(void *) != 4 || ((sizeof(s_user_interface_content_globals::s_task_slot) == 0x968))) ? 1 : -1];
typedef char user_interface_content_globals_name_offset[
    (sizeof(void *) != 4 || ((offsetof(s_user_interface_content_globals::s_task_slot, name) == 0x848))) ? 1 : -1];
typedef char user_interface_content_globals_description_offset[
    (sizeof(void *) != 4 || ((offsetof(s_user_interface_content_globals::s_task_slot, description) == 0x868))) ? 1 : -1];
typedef char user_interface_content_globals_size[(sizeof(void *) != 4 || ((sizeof(s_user_interface_content_globals) == 0x1c40))) ? 1 : -1];

extern "C" s_user_interface_content_globals g_user_interface_content_globals;

#endif
