#ifndef TAG_FILES_TAG_RESOURCE_CONTROL_DATUM_H
#define TAG_FILES_TAG_RESOURCE_CONTROL_DATUM_H

#include "memory/basic_buffer.h"
#include "cache/tag_resource_cache_internal_handle.h"
#include "cache/tag_resource_cache_file_datum_handler.h"
#include "memory/data_types.h"
class c_tag_resource_fixup
{
public:
    unsigned long value;
};
typedef c_tag_resource_cache_file_datum_handler c_resource_buffer_handler;

class c_tag_resource_cache_controller;

// Per-controller datum info block; each control datum reaches its controller through this.
class c_tag_resource_cache_control_datum_info
{
public:
    c_tag_resource_cache_controller *m_controller;
    __forceinline void initialize_for_new_map(c_tag_resource_cache_controller *controller) { m_controller = controller; }
    __forceinline void dispose_from_old_map() { m_controller = 0; }
};

enum e_tag_resource_control_datum_state { _tag_resource_control_datum_state_active = 4 };
struct s_tag_resource_control_datum_data
{
    s_datum_header header;
    unsigned char flags;
    unsigned char state;
    long page_handle;
    __forceinline bool test_flag(long bit) const { return (flags & (1 << bit)) != 0; }
    __forceinline void set_flag(long bit, bool value)
    {
        unsigned char mask = 1 << bit;
        if (value)
            flags |= mask;
        else
            flags &= ~mask;
    }
};
class c_tag_resource_control_datum {
public:
    __forceinline c_tag_resource_control_datum(bool cache, long h, c_tag_resource_cache_control_datum_info *o,
        s_tag_resource_control_datum_data *d, c_resource_buffer_handler *f)
        : handle(h), external_handle(c_tag_resource_cache_internal_handle(h).external),
          enabled(c_tag_resource_cache_internal_handle(h).optional), cache_enabled(cache), owner_info(o), data(d), handler(f) {}
    int handle;
    long external_handle;
    bool enabled;
    unsigned char cache_enabled;
    c_tag_resource_cache_control_datum_info *owner_info;
    s_tag_resource_control_datum_data *data;
    void set_state(e_tag_resource_control_datum_state state);
    bool always_required() const;
    __forceinline long get_absolute_internal_index() const { return handle & 0xffff; }
    long get_page_reservation() const;
    __forceinline long get_external_resource_handle() const { return external_handle; }
    c_basic_buffer<void> get_cache_data();
    c_basic_buffer<void> try_to_get_optional_data();
    void *get_root_address();
    __forceinline bool active() const { return ((data->flags >> 1) & 1) != 0 || (data->flags & 4) != 0; }
    __forceinline bool not_required() const { return ((data->flags >> 1) & 1) == 0 && (data->flags & 8) != 0; }
    __forceinline bool unneeded() const { return !enabled && ((data->flags >> 1) & 1) == 0 && (data->flags & 8) != 0; }
    __forceinline bool test_flag_5() const { return (data->flags & 0x20) != 0; }
    __forceinline bool test_flag(long bit) const { return data->test_flag(bit); }
    bool deferred() const;
    __forceinline void set_flag(long bit, bool value)
    {
        data->set_flag(bit, value);
    }
    c_resource_buffer_handler *handler;
};

#endif
