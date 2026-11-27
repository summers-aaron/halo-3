// tmp/probe/extra (GAME-CHANGES F51): the deleting destructors of five screen classes that the window manager's hand
// vtables (user_interface_window_manager.cpp c_screen_139 / 140 / 174 / 191 / 193, tmp/probe/fill/fntables.spec
// k_gui_screen_*_vftable slot 1) point at. Retail names them after the classes whose code they are (folded); the
// source has no out-of-line destructor for those classes. Each one destroys the members with destructors, then the
// c_gui_widget base, then frees the screen when asked (decompiled from retail 0x4b2af8, 0x4b42b0, 0x4cad10, 0x4cb920,
// 0x4d9e70). The probe binds them with `t4=` slot overrides in fntables.spec.
#include "cseries/async_xoverlapped_types.h"
#include "interface/gui_widget.h"
#include "interface/user_interface_memory.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/online/online_friend_xenon.h"

template<typename t_member>
static __forceinline t_member *probe_member(void *screen, unsigned long offset)
{
    return (t_member *)((char *)screen + offset);
}

static __forceinline void *probe_gui_screen_destroy(void *screen, unsigned int flags)
{
    ((c_gui_widget *)screen)->c_gui_widget::~c_gui_widget();
    if (flags & 1)
        user_interface_free(screen);
    return screen;
}

struct probe_gui_screen_destroyer
{
    void *destroy_139(unsigned int flags); // file share choose item: the string verify task at 0x1cb8
    void *destroy_140(unsigned int flags); // file share item selected: the find-friend task at 0x2074
    void *destroy_174(unsigned int flags); // postgame lobby: the hopper statistics downloader at 0x1a88
    void *destroy_191(unsigned int flags); // matchmaking searching: downloaders at 0x2bc0 and 0x1b80
    void *destroy_193(unsigned int flags); // matchmaking advanced options: downloaders at 0x1b118 and 0x1a88
};

void *probe_gui_screen_destroyer::destroy_139(unsigned int flags)
{
    probe_member<c_overlapped_task>(this, 0x1cb8)->c_overlapped_task::~c_overlapped_task();
    return probe_gui_screen_destroy(this, flags);
}

void *probe_gui_screen_destroyer::destroy_140(unsigned int flags)
{
    probe_member<c_online_find_friend_task>(this, 0x2074)->c_online_find_friend_task::~c_online_find_friend_task();
    return probe_gui_screen_destroy(this, flags);
}

void *probe_gui_screen_destroyer::destroy_174(unsigned int flags)
{
    probe_member<c_http_buffer_downloader>(this, 0x1a88)->c_http_buffer_downloader::~c_http_buffer_downloader();
    return probe_gui_screen_destroy(this, flags);
}

void *probe_gui_screen_destroyer::destroy_191(unsigned int flags)
{
    probe_member<c_http_buffer_downloader>(this, 0x2bc0)->c_http_buffer_downloader::~c_http_buffer_downloader();
    probe_member<c_http_buffer_downloader>(this, 0x1b80)->c_http_buffer_downloader::~c_http_buffer_downloader();
    return probe_gui_screen_destroy(this, flags);
}

void *probe_gui_screen_destroyer::destroy_193(unsigned int flags)
{
    probe_member<c_http_buffer_downloader>(this, 0x1b118)->c_http_buffer_downloader::~c_http_buffer_downloader();
    probe_member<c_http_buffer_downloader>(this, 0x1a88)->c_http_buffer_downloader::~c_http_buffer_downloader();
    return probe_gui_screen_destroy(this, flags);
}
