#ifndef NETWORKING_ONLINE_ONLINE_SERVICE_RECORD_H
#define NETWORKING_ONLINE_ONLINE_SERVICE_RECORD_H

#include "interface/experience_rank_types.h"

#include "interface/experience_grade_types.h"

// Functions defined in tu/networkinglibxbox/online_service_record.cpp, and the service record identity
// (March: c_online_service_record::get_identity() returns it; the start menu panes copy it).

#include "game/player_types.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"


// Field names from gui_start_menu_hq_service_record.cpp's use.
struct s_service_record_identity
{
    wchar_t name[16];
    s_player_appearance appearance;
    short pad3e;
    long color_index;
    long highest_skill;
    long experience;
    long hopper_weight;
    e_experience_rank rank;
    e_experience_grade grade;
    long games_played;
};

typedef char service_record_identity_size[(sizeof(s_service_record_identity) == 0x5c) ? 1 : -1];

extern "C" void online_service_record_dispose_downloader(void);

struct s_service_record_buffer
{
    const char *data;
    long size;
};

// One service record fetch: state, stored-buffer downloader, owner xuid,
// completion timestamp and the downloaded record view.
class c_online_service_record_manager
{
public:
    long state;
    c_http_stored_buffer_downloader<2768> downloader;
    unsigned __int64 xuid;
    __int64 timestamp;
    s_service_record_buffer buffer;
};

typedef char online_service_record_downloader_offset[(sizeof(void *) != 4 || ((offsetof(c_online_service_record_manager, downloader) == 4))) ? 1 : -1];
typedef char online_service_record_xuid_offset[(sizeof(void *) != 4 || ((offsetof(c_online_service_record_manager, xuid) == 4456))) ? 1 : -1];
typedef char online_service_record_buffer_offset[(sizeof(void *) != 4 || ((offsetof(c_online_service_record_manager, buffer) == 4472))) ? 1 : -1];

extern c_online_service_record_manager g_online_service_record_manager;

#endif // NETWORKING_ONLINE_ONLINE_SERVICE_RECORD_H
