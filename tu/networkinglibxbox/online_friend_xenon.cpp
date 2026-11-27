// OBJ: from the records 0x3d2510 0x3d25b0 0x3d25c0 0x3d2688 0x3d2690 0x3d26d8
// @compiler xdk6130
// @flags /TP /O2 /Oc

#include <xtl.h>
#include "cseries/async_xoverlapped.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "cseries/async_xoverlapped_types.h"
#include "networking/online/online_friend_xenon.h"


// @address 0x3d2510 ??1c_online_find_friend_task@@UAA@XZ
c_online_find_friend_task::~c_online_find_friend_task()
{
    cleanup();
}

const char *c_online_find_friend_task::get_context_string() const
{
    return "XFriendsCreateEnumerator";
}
// @address 0x3d25c0 ?start@c_online_find_friend_task@@UAAKPAX@Z
unsigned long c_online_find_friend_task::start(void *overlapped)
{
    DWORD buffer_size = 0;
    DWORD result = controller_get(m_controller_index)->is_signed_in_to_live()
        ? XFriendsCreateEnumerator(m_controller_index, 0, 100, &buffer_size, &m_enumeration_handle)
        : E_FAIL;
    if (result == ERROR_SUCCESS)
        result = XEnumerate(m_enumeration_handle, m_friends, sizeof(m_friends), NULL, (PXOVERLAPPED)overlapped);
    if (result != ERROR_IO_PENDING)
        cleanup();
    return result;
}
void c_online_find_friend_task::success(unsigned long friend_count)
{
    m_friend_count = friend_count;
}
void c_online_find_friend_task::complete()
{
    cleanup();
}
// @address 0x3d26d8 ?reset@c_online_find_friend_task@@UAAXXZ
void c_online_find_friend_task::reset()
{
    c_overlapped_task::reset();
    cleanup();
}
