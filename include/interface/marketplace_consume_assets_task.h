#ifndef INTERFACE_MARKETPLACE_CONSUME_ASSETS_TASK_H
#define INTERFACE_MARKETPLACE_CONSUME_ASSETS_TASK_H

#include "cseries/async_xoverlapped_types.h"
#include "shell/shell.h"
#ifndef __XONLINE__
// XDK asset declaration for task users that do not include xonline.h.
typedef struct _XMARKETPLACE_ASSET
{
    unsigned long dwAssetID;
    unsigned long dwQuantity;
} XMARKETPLACE_ASSET;
#endif

// The catalog-request task at 0x827c33dc holds the owned asset at +0x0c.
// The asset-consumption task at 0x827c33f0 uses +0x10 instead.
// The two tasks use different packing, so each keeps its own layout.
#pragma pack(push, 4)
class c_marketplace_request_consume_assets_task : public c_overlapped_task
{
public:
    c_marketplace_request_consume_assets_task()
        : c_overlapped_task(0, 0),
          controller_index(k_no_controller),
          asset()
    {
    }
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    e_controller_index controller_index;
    XMARKETPLACE_ASSET asset;
};
#pragma pack(pop)

#pragma pack(push, 8)
class c_marketplace_consume_assets_task : public c_overlapped_task
{
public:
    // A new task has no controller or offer and reports E_FAIL until started.
    c_marketplace_consume_assets_task()
        : c_overlapped_task(0, 0),
          controller_index(k_no_controller),
          offer_id(0),
          result((long)0x80004005L)
    {
    }
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    __forceinline void initialize(e_controller_index controller_index, unsigned __int64 offer_id)
    {
        this->controller_index = controller_index;
        this->offer_id = offer_id;
        result = (long)0x80004005L;
    }

    e_controller_index controller_index;
    unsigned __int64 offer_id;
    long result;
};
#pragma pack(pop)

extern "C" c_marketplace_consume_assets_task g_marketplace_asset_consume_task;

#endif // INTERFACE_MARKETPLACE_CONSUME_ASSETS_TASK_H
