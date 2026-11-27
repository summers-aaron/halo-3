#ifndef XBOX_CONTENT_API_H
#define XBOX_CONTENT_API_H

extern "C" unsigned long XMarketplaceCreateAssetEnumerator(
    unsigned long user_index, unsigned long maximum_assets_per_enumerate, unsigned long *buffer_size, void **enumeration_handle);
extern "C" unsigned long XContentCreateEx(unsigned long user_index, const char *root_name, void *content_data, unsigned long flags,
    unsigned long *disposition, unsigned long *license_mask, unsigned long file_cache_size, unsigned __int64 content_size,
    void *overlapped);
extern "C" unsigned long XContentDelete(int user_index, void *content_data, void *overlapped);
extern "C" unsigned long XContentCreateEnumerator(unsigned long user_index, unsigned long device_id,
    unsigned long content_type, unsigned long flags, unsigned long maximum_items,
    unsigned long *buffer_size, void **enumeration_handle);

extern "C" long XContentFlush(const char *root_name, void *overlapped);
extern "C" unsigned long XContentClose(const char *root_name, void *overlapped);

#endif // XBOX_CONTENT_API_H
