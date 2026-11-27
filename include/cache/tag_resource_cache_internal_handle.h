#ifndef CACHE_TAG_RESOURCE_CACHE_INTERNAL_HANDLE_H
#define CACHE_TAG_RESOURCE_CACHE_INTERNAL_HANDLE_H

class c_tag_resource_cache_internal_handle
{
public:
    bool optional;
    long external;

    __forceinline c_tag_resource_cache_internal_handle(long handle)
    {
        unsigned short salt = (unsigned short)(handle >> 16);
        optional = (salt & 0x8000) != 0;
        unsigned short external_salt = (unsigned short)~salt;
        external_salt |= 0x8000;
        unsigned long absolute_index = handle & 0xffff;
        absolute_index /= 2;
        external = (external_salt << 16) | absolute_index;
    }
};

// Inverse of c_tag_resource_cache_internal_handle's external-handle decode.
static __forceinline long tag_resource_cache_make_internal_handle(long external_handle, bool optional)
{
    unsigned short external_salt = (unsigned short)(external_handle >> 16);
    unsigned short internal_salt = (unsigned short)~external_salt;
    optional ? (internal_salt |= 0x8000) : (internal_salt &= ~0x8000);
    unsigned long absolute_index = (external_handle & 0xffff) * 2;
    if (!optional)
        absolute_index++;
    return (internal_salt << 16) | absolute_index;
}

#endif
