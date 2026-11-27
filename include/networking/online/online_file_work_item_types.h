#ifndef NETWORKING_ONLINE_ONLINE_FILE_WORK_ITEM_TYPES_H
#define NETWORKING_ONLINE_ONLINE_FILE_WORK_ITEM_TYPES_H

#include "saved_games/saved_game_item_metadata.h"

enum e_queued_work_item_type
{
    _queued_work_item_type_none = 0,
    _queued_work_item_type_upload = 1,
    _queued_work_item_type_download = 2,
};

struct s_online_file_description
{
    unsigned __int64 owner_xuid;
    bool named;
    s_saved_game_item_metadata metadata;
};

struct s_queued_work_item_status
{
    e_queued_work_item_type type;
    s_online_file_description description;
    long priority;
    long state;
    unsigned __int64 owner_xuid;
    long slot;
    float progress;
};

struct s_queued_work_item_status_tracker
{
    char filename[24];
    s_queued_work_item_status status;
};

typedef char online_file_description_size[(sizeof(s_online_file_description) == 0x108) ? 1 : -1];
typedef char queued_work_item_status_size[(sizeof(s_queued_work_item_status) == 0x128) ? 1 : -1];
typedef char queued_work_item_status_tracker_size[(sizeof(s_queued_work_item_status_tracker) == 0x140) ? 1 : -1];

#endif // NETWORKING_ONLINE_ONLINE_FILE_WORK_ITEM_TYPES_H
