#ifndef SAVED_GAMES_MARKETPLACE_ASSET_TASK_TYPES_H
#define SAVED_GAMES_MARKETPLACE_ASSET_TASK_TYPES_H
#include "cseries/async_xoverlapped_types.h"
#include "cseries/crt_xdk.h"
#include "saved_games/marketplace_asset_types.h"
#include "tag_files/file_last_modification_date.h"
// XEnumerate reply layout from the XDK's xonline.h (XMARKETPLACE_ASSET_PACKAGE / XMARKETPLACE_ASSET_ENUMERATE_REPLY).
// Skipped when the real <xonline.h> is already included (via xtl.h): same layout, XDK member names.
#ifndef __XONLINE__
struct XMARKETPLACE_ASSET_PACKAGE {
    s_file_last_modification_date enumerate_time;
    unsigned long asset_count;
    unsigned long total_asset_count;
    s_marketplace_asset assets[1];
};
struct XMARKETPLACE_ASSET_ENUMERATE_REPLY {
    unsigned char signature[256];
    XMARKETPLACE_ASSET_PACKAGE asset_package;
};
#endif
class c_marketplace_asset_enumeration_overlapped_task : public c_overlapped_task {
public:
    c_marketplace_asset_enumeration_overlapped_task()
        : c_overlapped_task(0, 0),
          m_controller_index(k_no_controller),
          m_enumeration_handle((void *)-1),
          m_enumeration_buffer(0),
          m_asset_count(0)
    {
    }
    virtual ~c_marketplace_asset_enumeration_overlapped_task();
    static __forceinline void operator delete(void *block)
    {
        if (block != 0)
            RtlFreeHeap(GetProcessHeap(), 0, block);
    }
    virtual void reset();
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void success(unsigned long return_result);
    virtual void complete();
    virtual bool is_result_successful(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error);
    e_controller_index m_controller_index;
    void *m_enumeration_handle;
    XMARKETPLACE_ASSET_ENUMERATE_REPLY *m_enumeration_buffer;
    s_marketplace_asset m_assets[32];
    long m_asset_count;
};

extern c_marketplace_asset_enumeration_overlapped_task g_marketplace_asset_enumeration_overlapped_task;


#endif
