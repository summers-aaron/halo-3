#ifndef INTERFACE_QUEUED_WORK_ITEM_STATUS_STATE_TYPES_H
#define INTERFACE_QUEUED_WORK_ITEM_STATUS_STATE_TYPES_H

enum e_queued_work_item_status_state
{
    _queued_work_item_status_state_default = 0,
    _queued_work_item_status_state_upload_queued = 1,
    _queued_work_item_status_state_compressing = 2,
    _queued_work_item_status_state_compressed = 3,
    _queued_work_item_status_state_uploading = 4,
    _queued_work_item_status_state_download_queued = 5,
    _queued_work_item_status_state_downloading = 6,
    _queued_work_item_status_state_downloaded = 7,
    _queued_work_item_status_state_decompressing = 8
};

#endif // INTERFACE_QUEUED_WORK_ITEM_STATUS_STATE_TYPES_H
