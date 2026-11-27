// blamlibxbox/tag_resource_cache_paging.obj: from the records 0x852b0 0x86370 0x13fad8 0x1414e0 0x141690 0x1418c0 0x141c80 0x2aab20 0x2aacf8 0x2aae18 0x2aaf90 0x2ab0c8 0x2ab1c0 0x2ab1e0 0x2ab250 0x2ab280 0x2ab290 0x2ab380 0x2ab6f8 0x2ab8e0 0x2ab988 0x2aba70 0x2abbf8 0x2abd18 0x2abe48 0x2abf40 0x2ac040 0x2ac118 0x2ac2f8 0x2ac478 0x2ac568 0x2ac628 0x2ac770 0x2ac808 0x2ac868 0x2ac8e0 0x2ac9d0 0x2acaa8 0x2acab0 0x2acb90 0x2acb98 0x2acc70 0x2acc78 0x2acd30 0x2acdf8 0x2aceb0 0x2ad008 0x2ad068 0x2ad0e0 0x2ad230 0x2ad420 0x2ad5f8 0x2ad798 0x2af250 0x2af530 0x2af548 0x2af5f0 0x2af618 0x2af668 0x2af678 0x2af688 0x2af6c8 0x2af730 0x2af740 0x2af7d8 0x2af918 0x2afa38 0x2afa48 0x2afab8 0x2ffb18 0x314928 0x314990 0x314a50 0x314ae8 0x314bb0 0x314c98 0x314db0 0x314ee8 0x315170 0x315408 0x315b70 0x3163b8 0x316440 0x316758 0x3168c0 0x3169f8 0x316a00 0x316dc0 0x317108 0x317388 0x317420
// @flags /TP /O2 /Oc
// @address 0x59fc10 GetCurrentFiber
// @address 0x59fc20 ConvertThreadToFiber
// @address 0x59fd00 CreateFiber
// @address 0x59fde0 DeleteFiber
// @address 0x59fe40 SwitchToFiber
#include <xtl.h>
#include "cache/tag_resource_cache_address_cache.h"
#include "memory/data_types.h"
#include "cache/asynchronous_io_marker_types.h"
#include "memory/contiguous_allocation.h"
#include "cache/tag_resource_cache_internal_handle.h"
#include "cache/tag_resource_page_reservation_types.h"
#include "cseries/crt_xdk.h"
#include "cseries/algorithm_internal.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "multithreading/synchronized_value.h"
#include "cache/security_functions.h"
#include "memory/lruv_cache.h"
#include "tag_files/tag_resource_cache_external_access.h"
#include "tag_files/tag_resource_cache_paging.h"
#include "tag_files/tag_groups.h"
#include "memory/data.h"
#include "main/loading.h"
#include "main/loading_progress.h"
#include "networking/session/network_session_parameters.h"
#include "memory/data_types.h"
#include "memory/data_types.h"
#include "game/cached_resource_state_types.h"
#include "memory/lruv_block_types.h"
#include "memory/lruv_block_header_types.h"
// @address 0x13fad8 tag_resource_paging_controller_reset
// @address 0x141c80 ?request_data@?$c_tag_resource_page_table_io_wrapper@$1?request_page_async@c_tag_resource_page_table@@AAA_NJPBUs_indirect_cache_file_read_request@@@Z@@UAA_NJPBUs_indirect_cache_file_read_request@@@Z
// @address 0x914 ??_7?$c_tag_resource_page_table_io_wrapper@$1?request_page_async@c_tag_resource_page_table@@AAA_NJPBUs_indirect_cache_file_read_request@@@Z@@6B@
// @address 0x2ab8e0 ?request_page_async@c_tag_resource_page_table@@AAA_NJPBUs_indirect_cache_file_read_request@@@Z
// @address 0x2ad420 tag_resource_partition_priorities
// @address 0x2ad008 tag_resource_page_datum_get_size
// @address 0x2ad5f8 ??$_Median@PA_K@std@@YAXPA_K00@Z
// @address 0x2ad798 ??$_Adjust_heap@PA_KH_K@std@@YAXPA_KHH_K@Z
// @address 0x2af250 ?dispose_resource_address@c_tag_resource_address_cache@@QAAXJPAX@Z
// @address 0x2ffb18 tag_resource_page_compute_hash
// @address 0x19f058 ?security_calculate_hash@@YA_NPBDK_NPAUs_network_http_request_hash@@@Z
// @address 0x101518 ?lruv_idle@@YAXPAUs_lruv_cache@@@Z
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
// @address 0x2aba70 tag_resource_page_table_compute_resize_sizes
// @address 0x2abd18 tag_resource_page_table_get_loading_progress
// @address 0x2ac118 tag_resource_page_table_set_page_ready
// @address 0x2ac2f8 tag_resource_page_table_update_required_pages
// @address 0x2acdf8 tag_resource_lruv_cache_allocate_buffer
#include "cache/tag_resource_cache_paging_constants.h"
#include "main/main_crash_support.h"

// @address 0x2a3d4 k_deflate_code_length_order
#if !defined(_XBOX) || defined(__clang__)
const unsigned char k_deflate_code_length_order[19] = {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
#endif

#include "cache/tag_resource_page_allocator.h"

#include "cache/tag_resource_page_datum.h"
#include "cache/cache_file_globals.h"
#include "cache/cache_file_byte_swap.h"

#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_lruv_cache_calls.h"
#include "cache/tag_resource_cache_new.h"
#include "cache/tag_resource_cache_thread_lock_externs.h"
#include "memory/allocation_description.h"
#include "memory/wrapped_arrays.h"
#include <new>
#include "cseries/pointer_utilities.h"
#include "cache/tag_resource_cache_published_interface.h"
typedef char paging_cache_controller_offset[(sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_controller) == 108))) ? 1 : -1];
typedef char paging_cache_inverse_offset[(sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_inverse_page_table) == 276))) ? 1 : -1];






struct s_tag_resource_paging_async_header
{
    long identifier;
    long page_handle;
    c_synchronized_long bytes_read;
};
struct s_tag_resource_async_io_marker : c_asynchronous_io_marker
{
    long get_io_status() const { long result = ((const c_interlocked_long *)&io_status)->peek(); __lwsync(); return result; }
    bool request_completed() const { return get_io_status() == 2 && (unsigned long)internal[0] != 259; }
};
struct s_tag_resource_paging_async_request : s_tag_resource_paging_async_header, s_tag_resource_async_io_marker
{
};
typedef char paging_async_request_size[(sizeof(void *) != 4 || ((sizeof(s_tag_resource_paging_async_request) == 40))) ? 1 : -1];

#include "memory/lruv_cache_types.h"

#pragma pack(push, 4)
// The paging task's receiver: XOR-decodes page buffers with the task's key.
struct s_tag_resource_paging_stream
{
#ifdef HALO3_64BIT
    virtual bool initialize(c_basic_buffer<void> buffer);
    virtual bool decode_buffer(c_basic_buffer<void> packed, s_tag_resource_page_buffer *output);
#else
    virtual bool initialize(unsigned __int64 buffer);
    virtual bool decode_buffer(unsigned __int64 packed, s_tag_resource_page_buffer *output);
#endif
    virtual bool finish(s_tag_resource_page_buffer *output);
    __forceinline s_tag_resource_paging_stream(signed char stream_key)
    {
        memset(&initial, 0, sizeof(initial) + sizeof(remaining));
        key = stream_key;
    }

    union {
        s_tag_resource_page_buffer initial;
        unsigned __int64 initial_packed;
    };
    union {
        s_tag_resource_page_buffer remaining;
        unsigned __int64 remaining_packed;
    };
    signed char key;
};
#pragma pack(pop)
typedef char paging_stream_size[(sizeof(void *) != 4 || ((sizeof(s_tag_resource_paging_stream) == 24))) ? 1 : -1];
typedef char paging_stream_remaining_size_offset[
    (sizeof(void *) != 4 || ((offsetof(s_tag_resource_paging_stream, remaining)
    + offsetof(s_tag_resource_page_buffer, size) == 16))) ? 1 : -1];
typedef char paging_stream_key_offset[(sizeof(void *) != 4 || ((offsetof(s_tag_resource_paging_stream, key) == 20))) ? 1 : -1];

// The initializer constructs the 24-byte stream in this placement storage.
#ifdef HALO3_64BIT
struct s_tag_resource_flag_target { char storage[0x30]; s_tag_resource_paging_stream *object; };
#else
struct s_tag_resource_flag_target { char storage[0x18]; s_tag_resource_paging_stream *object; };
#endif
typedef char paging_stream_target_object_offset[(sizeof(void *) != 4 || offsetof(s_tag_resource_flag_target, object) == 24) ? 1 : -1];

#include "cache/tag_resource_paging_task.h"

struct s_tag_resource_flag_source : c_tag_resource_paging_task
{
    __forceinline s_tag_resource_flag_source(unsigned char stream_key) : flag(stream_key) {}
    virtual void initialize_context(void *context);

    unsigned char flag;
};
typedef char paging_stream_factory_key_offset[(sizeof(void *) != 4 || offsetof(s_tag_resource_flag_source, flag) == 0x28) ? 1 : -1];

// 0x2aab20


// 0x2ac2f8
// 0x2af7d8
#include "cache/tag_resource_paging_decoder.h"

#include "saved_games/saved_film_stream_compressor.h"

// 0x2af918
typedef char paging_codec_fiber_offset[(sizeof(void *) != 4 || ((offsetof(s_tag_resource_paging_codec_stream, fiber) == 0x1818))) ? 1 : -1];
typedef char paging_codec_window_offset[(sizeof(void *) != 4 || ((offsetof(s_tag_resource_paging_codec_stream, window) == 0x1828))) ? 1 : -1];
typedef char paging_codec_status_offset[(sizeof(void *) != 4 || ((offsetof(s_tag_resource_paging_codec_stream, status) == 0x9830))) ? 1 : -1];
typedef char paging_codec_size[(sizeof(void *) != 4 || ((sizeof(s_tag_resource_paging_codec_stream) == 0x9834))) ? 1 : -1];
typedef char tag_resource_decoder_literal_lookup_offset[(offsetof(tag_resource_cache_paging_decoder_state, literal_lookup) == 356) ? 1 : -1];
typedef char tag_resource_decoder_bytes_left_offset[
    (sizeof(void *) != 4 || ((offsetof(tag_resource_cache_paging_decoder_state, bytes_left) == 6152))) ? 1 : -1];

struct s_tag_resource_sync_reader
{
    virtual c_basic_buffer<void> get_buffer() const = 0;
    virtual bool next_matching_page(long *out_handle, c_basic_buffer<void> *out_buffer, bool *out_verify) = 0;
};

// RFC 1951 code-length repeat codes and fixed Huffman table extents.
enum e_deflate_code_length_repeat
{
    _deflate_repeat_previous_length = 16,
    _deflate_repeat_zero_short = 17,
    _deflate_repeat_zero_long = 18
};
enum e_deflate_fixed_table_extents
{
    _deflate_fixed_8bit_literal_count = 144,
    _deflate_fixed_9bit_literal_count = 112,
    _deflate_fixed_7bit_literal_count = 24,
    _deflate_fixed_trailer_literal_count = 8
};

struct s_tag_resource_paging_async_read
{
    long identifier;
    long page_handle;
    long bytes_read;
    __forceinline long peek_bytes_read() const { long result = bytes_read; __lwsync(); return result; }
};


namespace {
template <typename T, long Count> class resource_io_array
{
public:
    T elements[Count];
    long element_count;
    __forceinline T *new_element()
    {
        long index = element_count;
        element_count++;
        return &elements[index];
    }
    __forceinline long count() const { return element_count; }
    __forceinline T &operator[](long index) { return elements[index]; }
};
}

static __forceinline void push_heap(unsigned __int64 *first, int hole, int top, unsigned __int64 value)
{
    for (int index = (hole - 1) / 2; top < hole && first[index] < value; index = (hole - 1) / 2) {
        first[hole] = first[index];
        hole = index;
    }
    first[hole] = value;
}

void c_tag_resource_page_table_control_interface::pin_page(long handle) { m_table->pin_page(handle); }

extern "C" void tag_resource_paging_controller_reset(
    c_tag_resource_cache_controller *controller, c_tag_resource_page_table_control_interface *control_interface)
{
    if (controller->m_can_predict_optional)
    {
        controller->m_busy = 1;
        controller->m_prediction_generation = (controller->m_prediction_generation + 1) & 0x7fffffff;
        controller->m_predicted.clear();
        control_interface->m_table->m_pinned_pages.clear();
        controller->m_prediction_pending = 1;
        controller->m_prediction_state = 0;
        lruv_idle(controller->m_page_table->m_table->m_lruv_cache.m_cache);
    }
}



// @address 0x2ab290 tag_resource_page_handle_pending_flags
extern "C" void tag_resource_page_handle_pending_flags(c_tag_resource_error_receiver *pending_flag_receiver, c_tag_resource_page_datum page_datum)
{
  s_tag_resource_page_datum *p = page_datum.m_page;
  bool b1 = (p->flags & 4) != 0 || (p->flags & 2) != 0;
  if (b1) {
   bool b2 = p->io_references >= 3;
   if (b2) {
    pending_flag_receiver->report_page_failure();
    if ((p->flags & 2) != 0) {
      pending_flag_receiver->report_read_failure();
    }
    if ((p->flags & 4) != 0) {
      pending_flag_receiver->report_hash_failure();
    }
   }
  }
}

bool c_tag_resource_page_table::request_page_async(long page_handle, const s_indirect_cache_file_read_request *request)
{
    bool result = false;
    long index = datum_new(m_async_requests);
    if (index != -1)
    {
        s_tag_resource_paging_async_request *entry = datum_get<s_tag_resource_paging_async_request>(m_async_requests, index);
        entry->page_handle = page_handle;
        c_asynchronous_io_marker &marker = *entry;
        result = m_reader->request_data_async(request, &entry->bytes_read, &marker);
        if (!result)
            ::datum_delete(m_async_requests, index);
    }
    return result;
}


namespace {
static __forceinline void swap_request_priority(unsigned __int64 &left, unsigned __int64 &right)
{
    unsigned __int64 temporary = left;
    left = right;
    right = temporary;
}
static __forceinline void median_of_three(unsigned __int64 *first, unsigned __int64 *middle, unsigned __int64 *last)
{
    if (*middle < *first)
        swap_request_priority(*middle, *first);
    if (*last < *middle)
        swap_request_priority(*last, *middle);
    if (*middle < *first)
        swap_request_priority(*middle, *first);
}
}

namespace {
static __forceinline bool page_is_unreferenced(const s_tag_resource_page_datum *page)
{
    return page->io_references == 0 && (page->flags & 8) != 0;
}
}

// @address 0x2abe48 tag_resource_cache_is_loaded
extern "C" unsigned char tag_resource_cache_is_loaded(c_tag_resource_page_table *page_table)
{
    c_tag_resource_page_table *table = page_table;
    unsigned char cache_loaded;
    cache_loaded = 0;
    if (table->m_async_requests_idle)
    {
        if (table->m_loaded_bytes >= table->m_required_bytes)
            cache_loaded = 1;
        else
        {
            unsigned char has_unreferenced;
            has_unreferenced = 0;
            c_data_iterator<s_tag_resource_paging_async_request> iterator;
            iterator.begin(table->m_async_requests);
            while (iterator.next())
            {
                s_tag_resource_paging_async_request *request = iterator.get_datum();
                s_tag_resource_page_datum *page = datum_get<s_tag_resource_page_datum>(table->m_pages, request->page_handle);
                unsigned char page_unreferenced = page_is_unreferenced(page);
                if (page_unreferenced != 0)
                    has_unreferenced = 1;
            }
            cache_loaded = has_unreferenced == 0;
        }
    }
    return cache_loaded;
}

// @address 0x2abf40 tag_resource_page_find_next_loadable
extern "C" void tag_resource_page_find_next_loadable(c_tag_resource_page_table *cursor)
{
    if (!cursor->m_async_requests_idle)
    {
        do
        {
            if (cursor->m_pages_iterator.next())
            {
                s_tag_resource_page_datum *page = datum_get<s_tag_resource_page_datum>(cursor->m_pages, cursor->m_pages_iterator.get_index());
                if (cursor->m_include_unreferenced || !page_is_unreferenced(page))
                {
                    if (page->state < 2)
                    {
                        if (page->io_references == 2 || page_is_unreferenced(page))
                            break;
                    }
                }
            }
            else
                cursor->m_async_requests_idle = true;
        } while (!cursor->m_async_requests_idle);
    }
}

// @address 0x2ac040 tag_resource_page_table_is_idle
extern "C" int tag_resource_page_table_is_idle(c_tag_resource_page_table *table)
{
    int is_idle = 0;
    if (!table->m_include_unreferenced)
    {
        int still_idle = 1;
        c_data_iterator<s_tag_resource_paging_async_request> iterator;
        iterator.begin(table->m_async_requests);
        while (iterator.next())
        {
            s_tag_resource_paging_async_request *request = iterator.get_datum();
            s_tag_resource_page_datum *page = datum_get<s_tag_resource_page_datum>(table->m_pages, request->page_handle);
            unsigned char page_unreferenced = page_is_unreferenced(page);
            if (page_unreferenced != 0)
                still_idle = 0;
        }
        is_idle = still_idle;
    }
    return is_idle;
}

namespace {
static __forceinline bool page_is_reading(const s_tag_resource_page_datum *page)
{
    return page->io_references == 1 || page->io_references == 3;
}
static __forceinline bool page_is_ready(const s_tag_resource_page_datum *page)
{
    return page->io_references >= 3;
}
static __forceinline bool page_is_unavailable(const s_tag_resource_page_datum *page)
{
    return (page_is_ready(page) || page->io_references == 2 || page_is_unreferenced(page)) && page->io_references != 1;
}
}


static __forceinline void tag_resource_cache_paging_block_until_async_requests_done(
    c_tag_resource_page_table *table, c_tag_resource_controller_page_table_io_listener *listener)
{
    while (table->m_async_requests->first_unallocated > 0)
        table->update_async_requests(listener);
}

// @address 0x2ac808 ?cancel_async_and_block_until_async_done@c_tag_resource_page_table@@QAAXPAVc_tag_resource_controller_page_table_io_listener@@@Z
void c_tag_resource_page_table::cancel_async_and_block_until_async_done(c_tag_resource_controller_page_table_io_listener *listener)
{
    tag_resource_cache_paging_block_until_async_requests_done(this, listener);
}

// @address 0x2ac478 tag_resource_page_table_dispose_idle_pages
extern "C" void tag_resource_page_table_dispose_idle_pages(c_tag_resource_page_table *table)
{
    c_data_iterator<s_tag_resource_page_datum> iterator;
    iterator.begin(table->m_pages);
    while (iterator.next())
    {
        unsigned int handle = iterator.get_index();
        s_tag_resource_page_datum *page = &((s_tag_resource_page_datum *)table->m_pages->data)[(unsigned short)handle];
        if (!page_is_reading(page) && page->io_references != 2 && !page_is_unreferenced(page))
            tag_resource_page_table_dispose_page(table, handle);
    }
}

__forceinline unsigned long c_tag_resource_page_datum::get_page_size() const
{
    s_tag_resource_page_location location;
    unsigned long size = 0;
    if (m_datum_handler->get_page_location(m_handle, &location))
        size = location.size;
    return size;
}

// @address 0x2ac628 tag_resource_page_table_dispose_minimum_size
extern "C" void tag_resource_page_table_dispose_minimum_size(c_tag_resource_page_table *table, unsigned int minimum_size)
{
    unsigned int total_size = 0;
    c_data_iterator<s_tag_resource_page_datum> iterator;
    iterator.begin(table->m_pages);
    if (minimum_size != 0)
    {
        do
        {
            if (!iterator.next())
                break;
            unsigned int handle = iterator.get_index();
            c_tag_resource_page_allocator *allocator = table->m_page_allocator;
            s_tag_resource_page_datum *page = &((s_tag_resource_page_datum *)table->m_pages->data)[(unsigned short)handle];
            if (!page_is_unavailable(page) && (page->flags & 1) != 0)
            {
                tag_resource_page_table_dispose_page(table, handle);
                long size = allocator->get_page_size(handle);
                unsigned long alignment_bits = table->m_lruv_cache.m_alignment_bits;
                total_size += ((1 << alignment_bits) + size - 1) & (-1 << alignment_bits);
            }
        } while (total_size < minimum_size);
    }
}

// @address 0x2ac770 tag_resource_page_table_compact_pages
extern "C" void tag_resource_page_table_compact_pages(c_tag_resource_page_table *table)
{
    tag_resource_page_table_dispose_idle_pages(table);
    s_lruv_cache *cache = table->m_lruv_cache.m_cache;
    unsigned long page_index = 0;
    for (long handle = cache->first_block_index; handle != -1;
         handle = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, handle)->next_block_index)
    {
        s_lruv_block *block = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, handle);
        if ((unsigned long)block->page_index > page_index)
        {
            if (cache->move_proc)
                cache->move_proc(cache->procs.client_data, handle, block->page_index, page_index);
            block->page_index = page_index;
        }
        page_index += block->page_count;
    }
}


// @address 0x2ac8e0 tag_resource_page_table_reset_page
extern "C" void tag_resource_page_table_reset_page(c_tag_resource_page_table *table, long handle)
{
    long index = handle & 0xffff;
    s_tag_resource_page_datum *page = &((s_tag_resource_page_datum *)table->m_pages->data)[index];
    if (((s_tag_resource_page_datum *)table->m_pages->data)[index].flags & 8)
    {
        page->flags &= ~8;
    }
    if (page->flags & 1)
    {
        lruv_block_delete(table->m_lruv_cache.m_cache, handle, false);
        page->flags &= ~1;
    }
    long old_state = ((s_tag_resource_page_datum *)table->m_pages->data)[index].state;
    ((s_tag_resource_page_datum *)table->m_pages->data)[index].state = 0;
    if (old_state > 0)
    {
        if (old_state >= 4)
            table->m_pinned_pages.set(index, false);
        if (old_state >= 3)
            table->m_ready_pages.set(index, false);
    }
}


// @address 0x2acaa8 tag_resource_page_handle_lruv_delete
extern "C" void tag_resource_page_handle_lruv_delete(void *context, long handle, bool release)
{
    c_tag_resource_page_table::lruv_delete_tag_resource_page_thunk(context, handle, release);
}

void c_tag_resource_page_table_control_interface::unpin_page(long handle) { m_table->unpin_page(handle); }

namespace {
static __forceinline unsigned long resource_align_up(unsigned long value, long alignment_bits)
{
    return (value + (1 << alignment_bits) - 1) & (-1 << alignment_bits);
}
static __forceinline unsigned long resource_lruv_get_used_size(s_lruv_cache *cache)
{
    long used_pages = 0;
    if (cache->first_block_index != -1)
    {
        s_data_array *data = (s_data_array *)cache->header.block_data;
        s_lruv_block *block = datum_get<s_lruv_block>(data, cache->last_block_index);
        used_pages = block->page_index + block->page_count;
    }
    return used_pages << cache->page_size_bits;
}
static __forceinline void resource_lruv_resize_non_destructive(s_lruv_cache *cache, long page_count)
{
    cache->maximum_page_count = page_count;
    long pages_per_block_ratio = -1;
    // match: 0x2acdf8 (+0x2aceb0, +0x2aacf8 inlined): the long-typed null check emits cmpwi; comparing the pointer emits cmplwi
    if (cache->maximum_page_count != 0 && (intptr_t)cache->header.block_data != 0)
        pages_per_block_ratio = (cache->maximum_page_count / ((s_data_array *)cache->header.block_data)->maximum_count) << 3;
    cache->pages_per_block_ratio = pages_per_block_ratio;
}
}

// @address 0x2aceb0 ?resize@c_tag_resource_lruv_cache@c_tag_resource_page_table@@QAA_NKKJ_NPA_N@Z
bool c_tag_resource_page_table::c_tag_resource_lruv_cache::resize(
    unsigned long minimum_size, unsigned long extra_size, long flags, bool allow_partial, bool *out_full_size)
{
    bool result = false;
    unsigned long size = minimum_size + extra_size;
    unsigned long aligned_size = resource_align_up(size, m_alignment_bits);
    if (aligned_size >= resource_lruv_get_used_size(m_cache) &&
        m_allocator->resize(&m_buffer, minimum_size, aligned_size, flags) &&
        (allow_partial || m_buffer.m_size >= aligned_size))
    {
        resource_lruv_resize_non_destructive(m_cache, m_buffer.m_size >> m_alignment_bits);
        result = true;
    }
    if (out_full_size)
        *out_full_size = result && m_buffer.m_size >= minimum_size + extra_size;
    return result;
}

extern "C" unsigned long tag_resource_page_datum_get_size(const c_tag_resource_page_datum *page)
{
    return page->get_page_size();
}

// @address 0x2ad068 tag_resource_page_datum_is_unavailable
extern "C" unsigned char tag_resource_page_datum_is_unavailable(const c_tag_resource_page_datum *page)
{
    return page_is_unavailable(page->m_page);
}

namespace {
static __forceinline bool tag_resource_cache_paging_verify_hash(
    const c_basic_buffer<void> &buffer, unsigned long size, const s_tag_resource_page_hash_pair &expected)
{
    s_tag_resource_page_hash_pair actual;
    bool result = false;
    if ((unsigned char)tag_resource_page_compute_hash(static_cast<const char *>(buffer.m_address), size, true, &actual))
    {
        if (memcmp(&expected.head, &actual.head, sizeof(expected.head)) == 0)
        {
            if (memcmp(&expected.tail, &actual.tail, sizeof(expected.tail)) == 0)
                result = true;
        }
    }
    return result;
}
}
// @address 0x2ad0e0 tag_resource_page_is_hash_valid
extern "C" bool tag_resource_page_is_hash_valid(c_tag_resource_page_datum *query)
{
    s_tag_resource_page_hash_pair expected;
    bool has_hash = query->m_datum_handler->get_page_hash(query->m_handle, &expected);
    c_basic_buffer<void> buffer = query->m_page->buffer;
    unsigned long size = query->m_datum_handler->get_page_size(query->m_handle);
    bool result = false;
    if (has_hash)
        result = tag_resource_cache_paging_verify_hash(buffer, size, expected);
    if (!result && !has_hash)
    {
        if (query->m_page->io_references == 0)
        {
            bool locked = query->m_page->io_lock_count > 0;
            if (!locked)
                result = true;
        }
    }
    return result;
}

namespace {
static __forceinline void tag_resource_cache_paging_make_heap(unsigned __int64 *first, unsigned __int64 *last)
{
    if (1 < last - first)
    {
        int bottom = last - first;
        for (int hole = bottom / 2; 0 < hole; )
        {
            --hole;
            std::_Adjust_heap(first, hole, bottom, *(first + hole));
        }
    }
}
static __forceinline void tag_resource_cache_paging_sort_heap(unsigned __int64 *first, unsigned __int64 *last)
{
    for (; 1 < last - first; --last)
    {
        if (1 < last - first)
        {
            unsigned __int64 value = *(last - 1);
            *(last - 1) = *first;
            std::_Adjust_heap(first, (ptrdiff_t)0, (last - 1) - first, value);
        }
    }
}
static __forceinline void tag_resource_cache_paging_insertion_sort(unsigned __int64 *first, unsigned __int64 *last)
{
    if (first != last)
    {
        for (unsigned __int64 *next = first; ++next != last; )
        {
            unsigned __int64 *hole = next;
            unsigned __int64 value = *next;
            if (value < *first)
            {
                ++hole;
                int count = next - first;
                if (0 < count)
                {
                    unsigned int size = count * sizeof(*first);
                    memmove_s(hole - count, size, first, size);
                }
                *first = value;
            }
            else
            {
                for (unsigned __int64 *previous = hole; value < *--previous; hole = previous)
                    *hole = *previous;
                *hole = value;
            }
        }
    }
}
}
// @address 0x2ad230 ??$_Sort@PA_KH@std@@YAXPA_K0H@Z
namespace std {
template <> void _Sort<unsigned __int64 *, int>(unsigned __int64 *first, unsigned __int64 *last, int ideal)
{
    int count;
    for (; 32 < (count = last - first) && 0 < ideal; )
    {
        s_tag_resource_paging_partition middle = tag_resource_partition_priorities(first, last);
        ideal /= 2;
        ideal += ideal / 2;
        if (middle.first - first < last - middle.second)
        {
            _Sort(first, middle.first, ideal);
            first = middle.second;
        }
        else
        {
            _Sort(middle.second, last, ideal);
            last = middle.first;
        }
    }
    if (32 < count)
    {
        tag_resource_cache_paging_make_heap(first, last);
        tag_resource_cache_paging_sort_heap(first, last);
    }
    else if (1 < count)
        tag_resource_cache_paging_insertion_sort(first, last);
}
#ifdef HALO3_64BIT
template <> void _Sort<unsigned __int64 *, __int64>(unsigned __int64 *first, unsigned __int64 *last, __int64 ideal)
{
    _Sort<unsigned __int64 *, int>(first, last, (int)ideal);
}
#endif
}


namespace std {
template <> inline void _Median<unsigned __int64 *>(unsigned __int64 *first, unsigned __int64 *middle, unsigned __int64 *last)
{
    if (last - first > 40)
    {
        unsigned int step = (last - first + 1) / 8;
        median_of_three(first, first + step, first + 2 * step);
        median_of_three(middle - step, middle, middle + step);
        median_of_three(last - 2 * step, last - step, last);
        median_of_three(first + step, middle, last - step);
    }
    else
        median_of_three(first, middle, last);
}
}

static __forceinline void tag_resource_cache_paging_partition_swap(unsigned __int64 *a, unsigned __int64 *b)
{
    unsigned __int64 tmp = *a;
    *a = *b;
    *b = tmp;
}
extern "C" s_tag_resource_paging_partition tag_resource_partition_priorities(unsigned __int64 *first, unsigned __int64 *last)
{
    unsigned __int64 *mid = first + (last - first) / 2;
    std::_Median(first, mid, last - 1);
    unsigned __int64 *pfirst = mid;
    unsigned __int64 *plast = pfirst + 1;
    while (first < pfirst && !(*(pfirst - 1) < *pfirst) && !(*pfirst < *(pfirst - 1)))
        --pfirst;
    while (plast < last && !(*plast < *pfirst) && !(*pfirst < *plast))
        ++plast;
    unsigned __int64 *gfirst = plast;
    unsigned __int64 *glast = pfirst;
    for (;;)
    {
        for (; gfirst < last; ++gfirst)
        {
            if (*pfirst < *gfirst)
                continue;
            else if (*gfirst < *pfirst)
                break;
            else
            {
                unsigned __int64 *p = plast++;
                tag_resource_cache_paging_partition_swap(p, gfirst);
            }
        }
        for (; first < glast; --glast)
        {
            if (*(glast - 1) < *pfirst)
                continue;
            else if (*pfirst < *(glast - 1))
                break;
            else
                tag_resource_cache_paging_partition_swap(--pfirst, glast - 1);
        }
        if (glast == first && gfirst == last)
            return s_tag_resource_paging_partition(pfirst, plast);
        if (glast == first)
        {
            if (plast != gfirst)
                tag_resource_cache_paging_partition_swap(pfirst, plast);
            ++plast;
            unsigned __int64 *p = pfirst++;
            unsigned __int64 *g = gfirst++;
            tag_resource_cache_paging_partition_swap(p, g);
        }
        else if (gfirst == last)
        {
            if (--glast != --pfirst)
                tag_resource_cache_paging_partition_swap(glast, pfirst);
            tag_resource_cache_paging_partition_swap(pfirst, --plast);
        }
        else
            tag_resource_cache_paging_partition_swap(gfirst++, --glast);
    }
}

namespace std {
template<typename Iterator, typename Difference, typename Value>
void _Adjust_heap(Iterator first, Difference hole, Difference bottom, Value value)
{
    int top = hole;
    int index = 2 * hole + 2;
    for (; index < bottom; index = 2 * index + 2) {
        if (first[index] < first[index - 1])
            --index;
        first[hole] = first[index];
        hole = index;
    }
    if (index == bottom) {
        first[hole] = first[bottom - 1];
        hole = bottom - 1;
    }
    push_heap(first, hole, top, value);
}

}


static __forceinline bool tag_resource_cache_paging_handle_is_optional(long handle)
{
    return c_tag_resource_cache_internal_handle(handle).optional;
}

static __forceinline long tag_resource_cache_paging_handle_get_external_handle(long handle)
{
    return c_tag_resource_cache_internal_handle(handle).external;
}


void c_tag_resource_address_cache::dispose_resource_address(long handle, void *address)
{
    long external_handle = tag_resource_cache_paging_handle_get_external_handle(handle);
    long index = (unsigned short)external_handle;
    if (tag_resource_cache_paging_handle_is_optional(handle))
        { s_cached_resource_state *s = m_states.begin() + index; s->optional_address = 0; }
    else
        { s_cached_resource_state *s = m_states.begin() + index; s->required_address = 0; }
    long bit = handle & 0xffff;
    m_required_resources.m_flags[bit >> 5] &= ~(1 << (bit & 31));
}

// @address 0x2af530 ?initialize@s_tag_resource_paging_stream@@UAA_N_K@Z
#ifdef HALO3_64BIT
bool s_tag_resource_paging_stream::initialize(c_basic_buffer<void> buffer)
{
    initial.data = (const char *)buffer.m_address;
    initial.size = buffer.m_size;
    remaining = initial;
    return true;
}

bool s_tag_resource_paging_stream::decode_buffer(c_basic_buffer<void> packed, s_tag_resource_page_buffer *output)
{
    s_tag_resource_page_buffer input;
    input.data = (const char *)packed.m_address;
    input.size = packed.m_size;
#else
bool s_tag_resource_paging_stream::initialize(unsigned __int64 buffer)
{
    initial_packed = buffer;
    remaining_packed = buffer;
    return true;
}

// @address 0x2af548 ?decode_buffer@s_tag_resource_paging_stream@@UAA_N_KPAUs_tag_resource_page_buffer@@@Z
bool s_tag_resource_paging_stream::decode_buffer(unsigned __int64 packed, s_tag_resource_page_buffer *output)
{
    s_tag_resource_page_buffer input;
    memcpy(&input, &packed, sizeof(input));
#endif
    unsigned int size = input.size;
    s_tag_resource_page_buffer region = remaining;
    char *destination = const_cast<char *>(region.data);
    memcpy(destination, input.data, size);
    int stream_key = key;
    unsigned int count = size;
    unsigned char *cursor = (unsigned char *)destination;
    if (count != 0) {
        do {
            --count;
            *cursor ^= stream_key;
            ++cursor;
        } while (count != 0);
    }
    remaining.data += size;
    remaining.size -= size;
    if (output) {
        s_tag_resource_page_buffer result;
        result.data = destination;
        result.size = size;
        *output = result;
    }
    return true;
}

// @address 0x2af5f0 ?finish@s_tag_resource_paging_stream@@UAA_NPAUs_tag_resource_page_buffer@@@Z
bool s_tag_resource_paging_stream::finish(s_tag_resource_page_buffer *output)
{
    if (remaining.size == 0) {
        output->data = 0;
        output->size = 0;
    }
    return remaining.size == 0;
}

// @address 0x41678 ??_7s_tag_resource_paging_stream@@6B@
// @address 0x41684 ??_7s_tag_resource_flag_source@@6B@
// @address 0x2af618 ?initialize_context@s_tag_resource_flag_source@@UAAXPAX@Z
void s_tag_resource_flag_source::initialize_context(void *context)
{
    s_tag_resource_flag_target *target = static_cast<s_tag_resource_flag_target *>(context);
    // match: 0x2af618: reading the key before the placement-new null test keeps the lbz ahead of the clrrwi
    unsigned char stream_key = flag;
    target->object = new (align_pointer(target->storage, 2)) s_tag_resource_paging_stream(stream_key);
}

// @address 0x793a58 ?g_tag_resource_paging_task_context@@3Us_tag_resource_flag_source@@A
s_tag_resource_flag_source g_tag_resource_paging_task_context = s_tag_resource_flag_source(0x55);

extern "C" {
// @address 0x2af668 tag_resource_paging_get_task_context
void *tag_resource_paging_get_task_context(void)
{
    return &g_tag_resource_paging_task_context;
}
}

// @address 0x2af678 ?is_receiver_null@c_tag_resource_paging_task@@UAAHXZ
int c_tag_resource_paging_task::is_receiver_null()
{
    return receiver == 0;
}

// @address 0x2af688 ?process@c_tag_resource_paging_task@@UAAPAVc_tag_resource_paging_receiver@@XZ
c_tag_resource_paging_receiver *c_tag_resource_paging_task::process()
{
    initialize_context(&context);
    return receiver;
}

// @address 0x2af6c8 ?process_and_handle@c_tag_resource_paging_task@@UAAPAVc_tag_resource_paging_receiver@@_K@Z
c_tag_resource_paging_receiver *c_tag_resource_paging_task::process_and_handle(t_tag_resource_paging_buffer_param buffer)
{
    initialize_context(&context);
    receiver->handle(buffer);
    return receiver;
}

// @address 0x2af730 ?reset_receiver@c_tag_resource_paging_task@@UAAXHHH@Z
void c_tag_resource_paging_task::reset_receiver(int, int, int)
{
    receiver = 0;
}

// @address 0x793a88 ?g_tag_resource_paging_secondary_context@@3Vc_tag_resource_paging_secondary_task@@A
c_tag_resource_paging_secondary_task g_tag_resource_paging_secondary_context;

extern "C" {
// @address 0x2afa38 tag_resource_paging_get_secondary_context
void *tag_resource_paging_get_secondary_context(void)
{
    return &g_tag_resource_paging_secondary_context;
}

}
// @address 0x2afa48 ?process_and_handle@c_tag_resource_paging_secondary_task@@UAAPAUs_saved_film_stream_compressor@@_K@Z
s_saved_film_stream_compressor *c_tag_resource_paging_secondary_task::process_and_handle(t_tag_resource_paging_buffer_param buffer)
{
    initialize_context(&context);
    context.decoder->initialize(buffer);
    return context.decoder;
}

extern "C" {
int tag_resource_page_compute_hash(const char *data, unsigned long size, bool salted, s_tag_resource_page_hash_pair *pair)
{
    pair->status = -1;
    int head_ok = 1;
    memset(&pair->complete, 0, sizeof(pair->complete));
    unsigned long chunk = size > 0x400 ? 0x400 : size;
    const char *tail = data - chunk + size;
    if (!security_calculate_hash(data, chunk, true, &pair->head))
        head_ok = 0;
    if (!security_calculate_hash(tail, chunk, true, &pair->tail))
        return 0;
    return head_ok;
}
}

namespace std { template void _Adjust_heap<unsigned __int64 *, int, unsigned __int64>(unsigned __int64 *, int, int, unsigned __int64); }

extern "C" {
// @address 0x412f4 ??_7c_data_array_allocation_description@@6B@
// @address 0x415b8 k_wrapped_flags_allocation_description_vtable
// @address 0x415b8 ??_7c_wrapped_flags_allocation_description@@6B@
}
static __forceinline void tag_resource_cache_paging_lruv_set_function_pointers(s_lruv_cache *cache, void (*delete_proc)(void *, long, bool),
    bool (*locked_proc)(void *, long), void (*move_proc)(void *, long, unsigned long, unsigned long))
{
    cache->procs.delete_proc = delete_proc;
    cache->procs.locked_proc = locked_proc;
    cache->move_proc = move_proc;
}


__forceinline void c_tag_resource_page_table::c_tag_resource_lruv_cache::initialize_for_new_map(long maximum_block_count,
    unsigned long page_size_bits, c_tag_resource_page_range_allocator *range_allocator, void *client_data,
    void (*delete_proc)(void *, long, bool), bool (*locked_proc)(void *, long),
    void (*move_proc)(void *, long, unsigned long, unsigned long), c_allocation_base *allocator)
{
    m_allocator = range_allocator;
    m_alignment_bits = page_size_bits;
    m_cache = lruv_new("tag resource cache", 0, page_size_bits, maximum_block_count, client_data, delete_proc, locked_proc, 0, allocator, -1);
    tag_resource_cache_paging_lruv_set_function_pointers(m_cache, delete_proc, locked_proc, move_proc);
}

// @address 0x2aab20 ?initialize_for_new_map@c_tag_resource_page_table@@QAAXKPAVc_tag_resource_cache_file_reader@@PAVc_tag_resource_cache_file_datum_handler@@PAVc_tag_resource_page_allocator@@PAVc_tag_resource_cache_published_interface@@PAVc_allocation_base@@PAVc_tag_resource_page_range_allocator@@@Z
void c_tag_resource_page_table::initialize_for_new_map(unsigned long maximum_page_count, c_tag_resource_cache_file_reader *reader,
    c_tag_resource_cache_file_datum_handler *datum_handler, c_tag_resource_page_allocator *page_allocator,
    c_tag_resource_cache_published_interface *published_interface, c_allocation_base *allocator,
    c_tag_resource_page_range_allocator *range_allocator)
{
    m_allocator = allocator;
    m_lruv_cache.initialize_for_new_map(maximum_page_count, 12, range_allocator, this, tag_resource_page_handle_lruv_delete,
        c_tag_resource_page_table::lruv_is_tag_resource_page_locked_thunk, tag_resource_page_handle_lruv_move, allocator);
    m_reader = reader;
    m_page_allocator = page_allocator;
    m_published_interface = published_interface;
    s_allocation_description_holder descriptions[] = {
        { c_data_array_allocation_description(m_pages, "tag resource cache page state", maximum_page_count, sizeof(s_tag_resource_page_datum), 2) },
        { c_data_array_allocation_description(m_async_requests, "tag resource async requests", 1, sizeof(s_tag_resource_paging_async_request), 2) },
        { c_wrapped_flags_allocation_description(m_ready_pages, maximum_page_count) },
        { c_wrapped_flags_allocation_description(m_pinned_pages, maximum_page_count) }
    };
    c_wrapped_array_no_init<s_allocation_description_holder> list_storage;
    c_wrapped_array<s_allocation_description_holder> *list =
        new (&list_storage) c_wrapped_array<s_allocation_description_holder>(descriptions);
    allocate_list_contiguous(allocator, *list, "tag resource cache page table", &m_storage, 0);
    m_ready_pages.clear();
    m_pinned_pages.clear();
    data_make_valid(m_pages);
    data_make_valid(m_async_requests);
    m_async_requests_idle = true;
    m_control_interface.m_table = this;
}

// 0x2aba70
static __forceinline unsigned long tag_resource_cache_paging_align_to_page_size(c_tag_resource_page_table *table, unsigned long size)
{
    return ((1 << table->m_lruv_cache.m_alignment_bits) + size - 1) & (-1 << table->m_lruv_cache.m_alignment_bits);
}
extern "C" void tag_resource_page_table_compute_resize_sizes(
    c_tag_resource_page_table *table, unsigned long *out_necessary_size, unsigned long *out_reading_size)
{
    unsigned long necessary_size = 0;
    unsigned long reading_size = 0;
    c_data_iterator<s_tag_resource_page_datum> iterator;
    iterator.begin(table->m_pages);
    while (iterator.next())
    {
        c_tag_resource_page_datum page = table->get_resource_page_from_handle(iterator.get_index());
        if (page.is_unreferenced())
            reading_size += tag_resource_cache_paging_align_to_page_size(table, page.m_datum_handler->get_page_size(page.m_handle));
        else if (tag_resource_page_datum_is_unavailable(&page))
            necessary_size += tag_resource_cache_paging_align_to_page_size(table, page.m_datum_handler->get_page_size(page.m_handle));
        else if (page_is_reading(page.m_page) && page.get_reservation() == 1)
            reading_size += tag_resource_cache_paging_align_to_page_size(table, page.m_datum_handler->get_page_size(page.m_handle));
    }
    *out_necessary_size = necessary_size;
    *out_reading_size = reading_size;
}

// 0x2abd18
extern "C" float tag_resource_page_table_get_loading_progress(c_tag_resource_page_table *table)
{
    float progress = 1.0f;
    if (!table->m_async_requests_idle && table->m_required_bytes != 0)
    {
        unsigned long loaded_bytes = table->m_loaded_bytes;
        c_data_iterator<s_tag_resource_paging_async_read> iterator;
        iterator.begin(table->m_async_requests);
        while (iterator.next())
        {
            s_tag_resource_paging_async_read *request = iterator.get_datum();
            if (page_is_unreferenced(datum_get<s_tag_resource_page_datum>(table->m_pages, request->page_handle)))
                loaded_bytes += request->peek_bytes_read();
        }
        progress = (float)loaded_bytes / (float)table->m_required_bytes;
    }
    return MIN(MAX(progress, 0.0f), 1.0f);
}

extern "C" void tag_resource_page_table_set_page_ready(
    c_tag_resource_page_table *table, long handle, bool verify, c_tag_resource_controller_page_table_io_listener *listener)
{
    c_tag_resource_page_datum page = table->get_resource_page_from_handle(handle);
    if (verify && !tag_resource_page_is_hash_valid(&page))
    {
        table->set_page_state(handle, 1);
        page.set_flag(2, true);
    }
    else
    {
        cache_file_resource_page_to_host(&g_cache_file_globals.header, g_cache_file_globals.tag_cache_base_address, &page);
        table->set_page_state(handle, 3);
        c_tag_resource_cache_new *owner = listener->m_owner;
        long index = handle & 0xffff;
        long resource_count = ((s_tag_resource_cache_external_access_slot *)owner->m_inverse_page_table.table->data)[index].count;
        for (long resource_index = 0; resource_index < resource_count; resource_index++)
        {
            s_tag_resource_cache_external_access_slot_owner *map = &owner->m_inverse_page_table;
            long resource_handle =
                map->list.begin()[((s_tag_resource_cache_external_access_slot *)map->table->data)[index].first +
                    resource_index];
            c_wrapped_flags *ready = (c_wrapped_flags *)&owner->m_controller.m_pending_release.m_storage;
            ready->set(resource_handle & 0xffff, true);
        }
    }
}

extern "C" void tag_resource_page_table_update_required_pages(c_tag_resource_page_table *table, c_tag_resource_paging_page_filter *filter)
{
    table->m_required_bytes = 0;
    table->m_loaded_bytes = 0;
    c_data_iterator<s_tag_resource_page_datum> iterator;
    iterator.begin(table->m_pages);
    while (iterator.next())
    {
        c_tag_resource_page_datum page = table->get_resource_page_from_handle(iterator.get_index());
        if (filter->page_is_necessary(page.m_handle))
        {
            page.set_flag(3, true);
            if (page.is_unreferenced())
            {
                table->m_required_bytes += page.get_page_size();
                if (page.is_ready())
                    table->m_loaded_bytes += tag_resource_page_datum_get_size(&page);
            }
        }
        else
            page.set_flag(3, false);
    }
}

// 0x2acab0
class c_tag_resource_cache_new;

// @address 0x2ac568 tag_resource_page_table_dispose_reserved_pages
extern "C" void tag_resource_page_table_dispose_reserved_pages(c_tag_resource_page_table *table)
{
    c_data_iterator<s_tag_resource_page_datum> iterator;
    iterator.begin(table->m_pages);
    while (iterator.next())
    {
        long handle = iterator.get_index();
        c_tag_resource_page_datum page = table->get_resource_page_from_handle(handle);
        if (page.get_reservation() == 1)
        {
            if (page.get_state() >= 3)
                table->m_published_interface->release_address(handle);
            tag_resource_page_table_reset_page(table, handle);
        }
    }
}

// @address 0x2ac868 tag_resource_page_table_dispose_page
extern "C" void tag_resource_page_table_dispose_page(c_tag_resource_page_table *table, unsigned int handle)
{
    c_tag_resource_page_datum page = table->get_resource_page_from_handle(handle);
    if (page.get_state() >= 3)
        table->m_published_interface->release_address(handle);
    tag_resource_page_table_reset_page(table, handle);
}

// @address 0x2ac9d0 ?lruv_delete_tag_resource_page_thunk@c_tag_resource_page_table@@CAXPAXJ_N@Z
void c_tag_resource_page_table::lruv_delete_tag_resource_page_thunk(void *context, long handle, bool release)
{
    c_tag_resource_page_table *table = (c_tag_resource_page_table *)context;
    c_tag_resource_page_datum page = table->get_resource_page_from_handle(handle);
    if (release)
        table->m_published_interface->release_address(handle);
    page.set_flag(0, false);
    table->set_page_state(handle, 0);
    page.m_page->buffer.m_address = 0;
    page.m_page->buffer.m_size = 0;
}
__forceinline bool c_tag_resource_page_datum::address_is_locked() const
{
    return address_is_pinned() && m_published_interface->address_is_locked(m_handle);
}
// @address 0x2acab0 ?lruv_is_tag_resource_page_locked@c_tag_resource_page_table@@AAA_NJ@Z
bool c_tag_resource_page_table::lruv_is_tag_resource_page_locked(long handle)
{
    bool locked = false;
    c_tag_resource_page_datum page = get_resource_page_from_handle(handle);
    if (page.address_is_locked())
        locked = true;
    else if (tag_resource_page_datum_is_unavailable(&page))
        locked = true;
    else if (page.get_state() == 2)
        locked = true;
    return locked;
}

// @address 0x2acb90 ?lruv_is_tag_resource_page_locked_thunk@c_tag_resource_page_table@@CA_NPAXJ@Z
bool c_tag_resource_page_table::lruv_is_tag_resource_page_locked_thunk(void *context, long handle)
{
    return ((c_tag_resource_page_table *)context)->lruv_is_tag_resource_page_locked(handle);
}


// @address 0x2acc70 tag_resource_page_handle_lruv_move
extern "C" void tag_resource_page_handle_lruv_move(void *context, long handle, unsigned long old_page_index, unsigned long new_page_index)
{
    ((c_tag_resource_page_table *)context)->lruv_move_tag_resource_page(handle, old_page_index, new_page_index);
}

// 0x2acc78
static __forceinline void tag_resource_cache_paging_lruv_set_block_locked(s_lruv_cache *cache, long block_index, bool locked)
{
    s_lruv_block *block = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, block_index);
    if (locked)
        block->flags |= 2;
    else
        block->flags &= ~2;
}
__forceinline void c_tag_resource_page_table::set_page_reservation(long handle, e_tag_resource_page_reservation reservation)
{
    c_tag_resource_page_datum page = get_resource_page_from_handle(handle);
    long old_reservation = page.get_reservation();
    page.set_reservation(reservation);
    if (page.test_flag(0))
    {
        if (old_reservation < 2 && reservation >= 2)
            tag_resource_cache_paging_lruv_set_block_locked(m_lruv_cache.m_cache, handle, true);
        else if (reservation < 2 && old_reservation >= 2)
            tag_resource_cache_paging_lruv_set_block_locked(m_lruv_cache.m_cache, handle, false);
    }
}
// @address 0x2acc78 ?set_page_reservation@c_tag_resource_page_table_control_interface@@QAAXJW4e_tag_resource_page_reservation@@@Z
void c_tag_resource_page_table_control_interface::set_page_reservation(long handle, e_tag_resource_page_reservation reservation)
{
    m_table->set_page_reservation(handle, reservation);
}

// 0x2acdf8
extern "C" void tag_resource_lruv_cache_allocate_buffer(c_tag_resource_page_table::c_tag_resource_lruv_cache *cache, unsigned long size)
{
    cache->m_allocator->allocate(&cache->m_buffer, resource_align_up(size, cache->m_alignment_bits));
    resource_lruv_resize_non_destructive(cache->m_cache, cache->m_buffer.m_size >> cache->m_alignment_bits);
}

// @address 0x2af7d8 ?compress@s_saved_film_stream_compressor@@UAA_N_JPAX@Z
#ifdef HALO3_64BIT
bool s_saved_film_stream_compressor::compress(c_basic_buffer<void> data, void *result)
{
    char *initial = m_cursor;
    s_tag_resource_page_buffer remaining;
    remaining.data = (const char *)data.m_address;
    remaining.size = data.m_size;
    s_tag_resource_page_buffer *output = (s_tag_resource_page_buffer *)result;
    long &status = m_stream.status;
    if (status == 0 && remaining.size != 0)
#else
bool s_saved_film_stream_compressor::compress(__int64 data, void *result)
{
    char *initial = m_cursor;
    union { unsigned __int64 packed_value; s_tag_resource_page_buffer buffer; } input;
    input.packed_value = data;
    s_tag_resource_page_buffer *output = (s_tag_resource_page_buffer *)result;
    s_tag_resource_page_buffer remaining = input.buffer;
    long &status = m_stream.status;
    if (status == 0 && input.buffer.size != 0)
#endif
    {
        s_tag_resource_paging_codec_stream *stream = &m_stream;
        do
        {
            unsigned long processed = remaining.size;
            unsigned long requested = m_available;
            unsigned char flags = stream->flags & 63;
            char *begin = m_begin;
            char *cursor = m_cursor;
            stream->processed = &processed;
            stream->input = remaining.data;
            stream->input_size = remaining.size;
            stream->flags = flags;
            stream->available = &requested;
            if (!begin)
                begin = cursor;
            stream->begin = begin;
            stream->cursor = cursor;
            unsigned long &capacity = stream->capacity;
            capacity = requested;
            if (stream->fiber.fiber)
                SwitchToFiber(stream->fiber.fiber);
            remaining.data += processed;
            remaining.size -= processed;
            status = stream->fiber.result;
            char *next = m_cursor + requested;
            unsigned long available = m_available - requested;
            m_cursor = next;
            m_available = available;
        } while (status == 0 && remaining.size != 0);
    }
    if (output && status >= 0)
    {
        char *end = m_cursor;
        output->data = initial;
        output->size = end - initial;
    }
    return status >= 0;
}
// @address 0x317388 tag_resource_paging_fiber_initialize
extern "C" int tag_resource_paging_fiber_initialize(s_tag_resource_paging_fiber *context, LPFIBER_START_ROUTINE start_routine, void *fiber_parameter)
{
    LPFIBER_START_ROUTINE fiber_proc = tag_resource_cache_paging_decoder_handle_fiber;
    LPFIBER_START_ROUTINE *param_pointer = &start_routine;
    tag_resource_paging_fiber_dispose(context);
    if (context->stack == 0)
    {
        context->stack = GetCurrentFiber();
        if ((uintptr_t)context->stack < 0x10000)
            context->stack = ConvertThreadToFiber(0);
        if (context->stack == 0)
            return 0;
    }
    context->fiber = CreateFiber(0x8000, fiber_proc, fiber_parameter);
    return context->fiber != 0;
}
// @address 0x317420 tag_resource_paging_fiber_dispose
extern "C" void tag_resource_paging_fiber_dispose(s_tag_resource_paging_fiber *context)
{
    if (context->fiber)
    {
        DeleteFiber(context->fiber);
        context->fiber = 0;
    }
    if (context->stack)
        context->stack = 0;
}

// @address 0x2af740 ?initialize@s_saved_film_stream_compressor@@UAA_N_J@Z
#ifdef HALO3_64BIT
bool s_saved_film_stream_compressor::initialize(c_basic_buffer<void> buffer)
{
    m_begin = (char *)buffer.m_address;
    m_input_size = buffer.m_size;
    m_cursor = (char *)buffer.m_address;
    m_available = buffer.m_size;
#else
bool s_saved_film_stream_compressor::initialize(__int64 buffer)
{
    input_packed[0] = buffer;
    input_packed[1] = buffer;
#endif
    s_tag_resource_paging_codec_stream *stream = &m_stream;
    tag_resource_paging_fiber_dispose(&stream->fiber);
    stream->window_count = 0;
    stream->error_count = 0;
    stream->flags |= 32;
    ((int (*)(s_tag_resource_paging_fiber *, LPFIBER_START_ROUTINE, void *, int, int, int, int))tag_resource_paging_fiber_initialize)(
        &stream->fiber, tag_resource_cache_paging_decoder_handle_fiber, stream, 0, 0x8000, 0x20000, 0);
    return true;
}

// @address 0x2af918 ?finish@s_saved_film_stream_compressor@@UAA_NPAX@Z
bool s_saved_film_stream_compressor::finish(void *result)
{
    s_saved_film_stream_compressor *state = this;
    s_tag_resource_page_buffer *output = static_cast<s_tag_resource_page_buffer *>(result);
    long &status = state->m_stream.status;
    char *initial = state->m_cursor;
    if (status == 0)
    {
        s_tag_resource_paging_codec_stream *stream = &state->m_stream;
        do
        {
            unsigned long requested = state->m_available;
            unsigned char flags = (stream->flags & 63) | 128;
            char *begin = state->m_begin;
            char *cursor = state->m_cursor;
            unsigned long processed = 0;
            stream->processed = &processed;
            stream->input = 0;
            stream->input_size = 0;
            stream->flags = flags;
            stream->available = &requested;
            if (!begin)
                begin = cursor;
            stream->begin = begin;
            stream->cursor = cursor;
            unsigned long &capacity = stream->capacity;
            capacity = requested;
            if (stream->fiber.fiber)
                SwitchToFiber(stream->fiber.fiber);
            status = stream->fiber.result;
            if (status < 0)
                break;
            char *next = state->m_cursor + requested;
            unsigned long remaining = state->m_available - requested;
            state->m_cursor = next;
            state->m_available = remaining;
        } while (status != 1);
    }
    if (output && status == 1)
    {
        char *end = state->m_cursor;
        output->data = initial;
        output->size = end - initial;
    }
    tag_resource_paging_fiber_dispose(&state->m_stream.fiber);
    return status == 1;
}

// @address 0x2afab8 tag_resource_paging_owner_dispose
extern "C" void tag_resource_paging_owner_dispose(s_tag_resource_paging_owner *paging_owner)
{
    s_saved_film_stream_compressor *decoder = paging_owner->decoder;
    if (decoder != 0)
    {
        s_tag_resource_paging_codec_stream *stream = &decoder->m_stream;
        tag_resource_paging_fiber_dispose(&stream->fiber);
        tag_resource_paging_fiber_dispose(&stream->fiber);
        paging_owner->decoder = 0;
    }
}

// @address 0x314990 tag_resource_cache_paging_decoder_get_byte
extern "C" int tag_resource_cache_paging_decoder_get_byte(tag_resource_cache_paging_decoder_state *state)
{
    while (--state->bytes_left < 0)
    {
        state->bytes_left++;
        if ((state->end_of_stream & ~0x7fU) != 0)
        {
            state->error_count++;
            return 0xff;
        }
        *state->position_a -= state->bytes_left;
        *state->position_b -= state->consumed_b;
        state->error = 0;
        void *jump_buffer = state->jump_buffer;
        if (jump_buffer != 0)
        {
            SwitchToFiber(jump_buffer);
        }
    }
    return *state->read_pointer++;
}

// @address 0x314a50 tag_resource_cache_paging_decoder_get_dword
extern "C" int tag_resource_cache_paging_decoder_get_dword(tag_resource_cache_paging_decoder_state *state)
{
    int &bytes_left = state->bytes_left;
    if ((bytes_left -= 4) < 0) {
        bytes_left += 4;
        unsigned int result = tag_resource_cache_paging_decoder_get_byte(state);
        result |= tag_resource_cache_paging_decoder_get_byte(state) << 8;
        result |= tag_resource_cache_paging_decoder_get_byte(state) << 16;
        result |= tag_resource_cache_paging_decoder_get_byte(state) << 24;
        return result;
    } else {
        unsigned int *read_pointer = (unsigned int *)state->read_pointer;
        unsigned int result = __loadwordbytereverse(0, read_pointer);
        state->read_words = read_pointer + 1;
        return result;
    }
}

// @rdata 0x8202a3e8..0x8202a66c
static const int tag_resource_cache_paging_length_extra_bits[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0, 0 };
static const int tag_resource_cache_paging_length_base[32] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0, 0 };
static const int tag_resource_cache_paging_distance_extra_bits[32] = {
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
    7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 0, 0 };
static const int tag_resource_cache_paging_distance_base[32] = {
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
    257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0, 0 };
static const unsigned long tag_resource_cache_paging_bit_masks[33] = {
    0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
    0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
    0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
    0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
    0xffffffff };
// @rdata-end

// @address 0x314bb0 tag_resource_cache_paging_decoder_get_bits
extern "C" unsigned long tag_resource_cache_paging_decoder_get_bits(tag_resource_cache_paging_decoder_state *state, int count)
{
    unsigned long result = (unsigned long)(tag_resource_cache_paging_bit_masks[count] & state->bit_buffer);
    state->bit_count -= count;
    state->bit_buffer >>= count;
    if (state->bit_count < 0) {
        unsigned __int64 word;
        if ((state->bytes_left -= 4) < 0) {
            state->bytes_left += 4;
            word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
            word |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
            word |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
            word |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
        } else {
            unsigned int * read_pointer = state->read_words;
            unsigned int next_word = __loadwordbytereverse(0, read_pointer);
            state->read_words = read_pointer + 1;
            word = next_word;
        }
        state->bit_count += 32;
        state->bit_buffer |= word << state->bit_count;
    }
    return result;
}

// @address 0x314ae8 tag_resource_cache_paging_decoder_refill_bits
extern "C" void tag_resource_cache_paging_decoder_refill_bits(tag_resource_cache_paging_decoder_state *state)
{
    unsigned __int64 word;
    if ((state->bytes_left -= 4) < 0) {
        state->bytes_left += 4;
        word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
        word |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
        word |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
        word |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
    } else {
        unsigned int * read_pointer = state->read_words;
        unsigned int next_word = __loadwordbytereverse(0, read_pointer);
        state->read_words = read_pointer + 1;
        word = next_word;
    }
    state->bit_count += 32;
    state->bit_buffer |= word << state->bit_count;
}

// @address 0x314928 tag_resource_cache_paging_decoder_set_error_and_abort
extern "C" void tag_resource_cache_paging_decoder_set_error_and_abort(tag_resource_cache_paging_decoder_state *state, int code)
{
    if (code == 0)
        code = -9;
    *state->position_a -= state->bytes_left;
    *state->position_b -= state->consumed_b;
    state->error = code;
    void *jump_buffer = state->jump_buffer;
    if (jump_buffer != 0)
        SwitchToFiber(jump_buffer);
    abort();
}

// @address 0x314c98 tag_resource_cache_paging_decoder_get_symbol
extern "C" long tag_resource_cache_paging_decoder_get_symbol(tag_resource_cache_paging_decoder_state *state, const int *short_lookup, const int *long_lookup)
{
    unsigned __int64 bits = state->bit_buffer;
    int value = short_lookup[bits & 0xff];
    if (value < 0) {
        unsigned int bit_source = (unsigned int)(bits >> 8);
        do {
            value = long_lookup[(int)(bit_source & 1) - value - 1];
            bit_source >>= 1;
        } while (value < 0);
    }
    long result = value & 0xffff;
    unsigned int bit_count = value >> 16;
    state->bit_count -= bit_count;
    state->bit_buffer = bits >> bit_count;
    if (state->bit_count < 0) {
        unsigned __int64 word;
        if ((state->bytes_left -= 4) < 0) {
            state->bytes_left += 4;
            word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
            word |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
            word |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
            word |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
        } else {
            unsigned int * read_pointer = state->read_words;
            unsigned int next_word = __loadwordbytereverse(0, read_pointer);
            state->read_words = read_pointer + 1;
            word = next_word;
        }
        state->bit_count += 32;
        state->bit_buffer |= word << state->bit_count;
    }
    return result;
}

// @address 0x1418c0 ?allocate_page_storage@c_tag_resource_page_table@@QAAPAXJPAK@Z
static __forceinline s_lruv_block *tag_resource_cache_paging_lruv_block(s_lruv_cache *cache, long index)
{
    return datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, index);
}
static __forceinline void *tag_resource_cache_paging_lruv_block_address(c_tag_resource_page_table::c_tag_resource_lruv_cache *lruv, long index)
{
    s_lruv_cache *cache = lruv->m_cache;
    return (char *)lruv->m_buffer.m_address + (tag_resource_cache_paging_lruv_block(cache, index)->page_index << cache->page_size_bits);
}
static __forceinline unsigned long tag_resource_cache_paging_lruv_block_size(c_tag_resource_page_table::c_tag_resource_lruv_cache *lruv, long index)
{
    s_lruv_cache *cache = lruv->m_cache;
    return tag_resource_cache_paging_lruv_block(cache, index)->page_count << cache->page_size_bits;
}
void *c_tag_resource_page_table::allocate_page_storage(long handle, unsigned long *out_size)
{
    c_tag_resource_page_datum page = get_resource_page_from_handle(handle);
    unsigned long size = m_page_allocator->get_page_size(handle);
    c_tag_resource_lruv_cache *lruv = &m_lruv_cache;
    void *address = 0;
    long block_index = lruv_block_new(lruv->m_cache, handle, size, 0);
    if (block_index != -1)
    {
        address = tag_resource_cache_paging_lruv_block_address(lruv, block_index);
        *out_size = tag_resource_cache_paging_lruv_block_size(lruv, block_index);
        if (address != 0)
        {
            page.set_flag(0, true);
            if (tag_resource_page_datum_is_unavailable(&page))
                tag_resource_cache_paging_lruv_block(m_lruv_cache.m_cache, handle)->flags |= 2;
        }
    }
    return address;
}

// @address 0x141690 tag_resource_page_table_allocate_page
extern "C" bool tag_resource_page_table_allocate_page(c_tag_resource_page_table *table, long handle, c_tag_resource_controller_page_table_io_listener *listener)
{
    bool result = false;
    c_tag_resource_page_datum page = table->get_resource_page_from_handle(handle);
    if (page.get_state() == 0)
    {
        c_tag_resource_page_allocator *allocator = page.m_datum_handler;
        if ((unsigned long)allocator->get_page_size(handle) != 0)
        {
            unsigned long size;
            void *address = table->allocate_page_storage(handle, &size);
            if (address != 0)
            {
                page.m_page->buffer.m_address = address;
                page.m_page->buffer.m_size = size;
                table->set_page_state(handle, 1);
                result = true;
            }
            else if ((unsigned long)allocator->get_page_size(handle) == 0)
            {
                page.m_page->buffer.m_address = 0;
                page.m_page->buffer.m_size = 0;
                table->set_page_state(handle, 1);
                result = true;
            }
        }
        else
        {
            tag_resource_page_table_set_page_ready(table, handle, true, listener);
            result = true;
        }
    }
    return result;
}

// @address 0x1414e0 tag_resource_page_table_read_page
static __forceinline bool tag_resource_cache_paging_build_read_request(
    c_tag_resource_page_table *table, long handle, s_indirect_cache_file_read_request *request)
{
    bool valid = false;
    c_tag_resource_page_allocator *allocator = table->m_page_allocator;
    request->buffer.m_address = 0;
    request->buffer.m_size = 0;
    c_tag_resource_page_datum page = table->get_resource_page_from_handle(handle);
    if (page.get_state() == 1)
    {
        s_tag_resource_page_location location;
        allocator->get_page_location(handle, &location);
        request->buffer = page.m_page->buffer;
        request->position = allocator->get_page_position(handle);
        memcpy(&request->offset, &location, sizeof(location));
        valid = true;
    }
    return valid;
}
extern "C" void tag_resource_page_table_read_page(c_tag_resource_page_table *table, long handle, c_tag_resource_page_table::c_page_io_interface *reader,
    c_tag_resource_controller_page_table_io_listener *listener)
{
    if (handle != -1)
    {
        c_tag_resource_page_datum page = table->get_resource_page_from_handle(handle);
        tag_resource_page_table_allocate_page(table, handle, listener);
        if (page.get_state() == 1)
        {
            s_indirect_cache_file_read_request request;
            if (tag_resource_cache_paging_build_read_request(table, handle, &request) && reader->request_data(handle, &request))
            {
                table->set_page_state(handle, 2);
                page.set_flag(2, false);
                unsigned char *flags = &page.m_page->flags;
                *flags &= ~2;
            }
        }
    }
}

static __forceinline bool tag_resource_cache_paging_async_requests_available(c_tag_resource_page_table *table)
{
    return table->m_async_requests->actual_count < table->m_async_requests->maximum_count;
}
// @address 0x2ab988 ?demand_page@c_tag_resource_page_table@@QAAXJPAVc_tag_resource_controller_page_table_io_listener@@@Z
void c_tag_resource_page_table::demand_page(long page_handle, c_tag_resource_controller_page_table_io_listener *listener)
{
    if (page_handle != -1)
    {
        s_tag_resource_page_datum *page = datum_get<s_tag_resource_page_datum>(m_pages, page_handle);
        if ((page_is_reading(page) || page->io_references == 2 || page_is_unreferenced(page)) &&
            tag_resource_cache_paging_async_requests_available(this))
        {
            c_tag_resource_page_table_io_wrapper<&c_tag_resource_page_table::request_page_async> io_wrapper(this);
            tag_resource_page_table_read_page(this, page_handle, &io_wrapper, listener);
        }
    }
}

// @address 0x2abbf8 tag_resource_page_table_update_next_page
extern "C" void tag_resource_page_table_update_next_page(c_tag_resource_page_table *table, c_tag_resource_controller_page_table_io_listener *listener)
{
    if (!table->m_async_requests_idle && table->m_pages_iterator.get_index() == -1)
        tag_resource_page_find_next_loadable((c_tag_resource_page_table *)table);
    if (!table->m_async_requests_idle)
    {
        bool started = false;
        long handle = table->m_pages_iterator.get_index();
        c_tag_resource_page_datum page = table->get_resource_page_from_handle(handle);
        if (page.get_state() < 2 && tag_resource_cache_paging_async_requests_available(table))
        {
            started = true;
            table->demand_page(handle, listener);
        }
        bool advance = false;
        if (started)
        {
            if (page.get_state() < 2)
                table->m_loaded_bytes += tag_resource_page_datum_get_size(&page);
            advance = true;
        }
        else if (page.get_state() >= 3)
            advance = true;
        if (advance)
            tag_resource_page_find_next_loadable((c_tag_resource_page_table *)table);
    }
}

// @address 0x852b0 ?update_async_requests@c_tag_resource_page_table@@QAAXPAVc_tag_resource_controller_page_table_io_listener@@@Z


void c_tag_resource_page_table::update_async_requests(c_tag_resource_controller_page_table_io_listener *listener)
{
    bool any_completed = false;
    {
        c_data_iterator<s_tag_resource_paging_async_request> iterator;
        iterator.begin(m_async_requests);
        do
        {
            if (!iterator.next())
                break;
            s_tag_resource_paging_async_request *request = iterator.get_datum();
            any_completed = request->request_completed();
        } while (!any_completed);
    }
    if (any_completed)
        m_reader->update_io_state();
    c_data_iterator<s_tag_resource_paging_async_request> iterator;
    iterator.begin(m_async_requests);
    while (iterator.next())
    {
        s_tag_resource_paging_async_request *request = iterator.get_datum();
        long status = request->get_io_status();
        long handle = request->page_handle;
        c_tag_resource_page_datum page(handle, &((s_tag_resource_page_datum *)m_pages->data)[(unsigned short)handle], m_page_allocator, m_published_interface);
        if (status == 1)
        {
            tag_resource_page_table_set_page_ready(this, handle, true, listener);
            if (page.is_unreferenced())
                { long size = (long)page.get_page_size(); m_loaded_bytes += size; }
            ::datum_delete(m_async_requests, iterator.get_index());
        }
        else if (status == 0)
        {
            set_page_state(handle, 1);
            page.set_flag(1, true);
            if (page.is_unreferenced())
                m_loaded_bytes += tag_resource_page_datum_get_size(&page);
            ::datum_delete(m_async_requests, iterator.get_index());
        }
    }
}

static __forceinline bool tag_resource_cache_paging_has_pending_flags(const c_tag_resource_page_datum &page)
{
    return page.test_flag(2) || page.test_flag(1);
}
// @address 0x2ab6f8 tag_resource_page_table_read_required_pages
extern "C" bool tag_resource_page_table_read_required_pages(c_tag_resource_page_table *table,
    c_tag_resource_controller_page_table_io_listener *listener, bool include_reading, bool synchronous,
    c_tag_resource_error_receiver *flag_receiver)
{
    bool result = true;
    c_data_iterator<s_tag_resource_page_datum> iterator;
    iterator.begin(table->m_pages);
    c_tag_resource_page_table_io_wrapper<&c_tag_resource_page_table::request_page_async> io_wrapper(table);
    while (iterator.next())
    {
        long handle = iterator.get_index();
        c_tag_resource_page_datum page = table->get_resource_page_from_handle(handle);
        if (page.get_state() < 2)
        {
            if (page_is_ready(page.m_page) || (include_reading && page_is_reading(page.m_page)))
                tag_resource_page_table_read_page(table, handle, &io_wrapper, listener);
            else if (page.m_page->io_references == 2)
            {
                if (synchronous)
                    tag_resource_page_table_read_page(table, handle, &io_wrapper, listener);
                else
                    tag_resource_page_table_allocate_page(table, handle, listener);
            }
        }
        while (page.get_state() == 2)
        {
            table->m_reader->wait_for_io();
            table->update_async_requests(listener);
        }
        if (page_is_ready(page.m_page) && tag_resource_cache_paging_has_pending_flags(page))
            result = false;
        tag_resource_page_handle_pending_flags(flag_receiver, page);
    }
    return result;
}

// @address 0x415a4 ??_7c_synchronous_resource_io@?1??load_required_pages@c_tag_resource_page_table@@QAA_NPAVc_tag_resource_controller_page_table_io_listener@@_N1PAVc_tag_resource_error_receiver@@@Z@6Bc_page_io_interface@2@@
// @address 0x415a8 ??_7c_synchronous_resource_io@?1??load_required_pages@c_tag_resource_page_table@@QAA_NPAVc_tag_resource_controller_page_table_io_listener@@_N1PAVc_tag_resource_error_receiver@@@Z@6Bc_indirect_cache_file_bulk_read_iterator@@@
// @address 0x2aaf90 ?request_data@c_synchronous_resource_io@?1??load_required_pages@c_tag_resource_page_table@@QAA_NPAVc_tag_resource_controller_page_table_io_listener@@_N1PAVc_tag_resource_error_receiver@@@Z@UAA_NJPBUs_indirect_cache_file_read_request@@@Z
// @address 0x2ab0c8 ?block_until_done@c_synchronous_resource_io@?1??load_required_pages@c_tag_resource_page_table@@QAA_NPAVc_tag_resource_controller_page_table_io_listener@@_N1PAVc_tag_resource_error_receiver@@@Z@QAA_NXZ
// @address 0x2ab1c0 ?reset@c_synchronous_resource_io@?1??load_required_pages@c_tag_resource_page_table@@QAA_NPAVc_tag_resource_controller_page_table_io_listener@@_N1PAVc_tag_resource_error_receiver@@@Z@EAAXXZ
// @address 0x2ab1e0 ?next@c_synchronous_resource_io@?1??load_required_pages@c_tag_resource_page_table@@QAA_NPAVc_tag_resource_controller_page_table_io_listener@@_N1PAVc_tag_resource_error_receiver@@@Z@EAA_NPAUs_indirect_cache_file_read_request@@PAH@Z
// @address 0x2ab250 ?read_progress@c_synchronous_resource_io@?1??load_required_pages@c_tag_resource_page_table@@QAA_NPAVc_tag_resource_controller_page_table_io_listener@@_N1PAVc_tag_resource_error_receiver@@@Z@EAAXK1@Z
// @address 0x2ab280 ?finished@c_synchronous_resource_io@?1??load_required_pages@c_tag_resource_page_table@@QAA_NPAVc_tag_resource_controller_page_table_io_listener@@_N1PAVc_tag_resource_error_receiver@@@Z@EAAXKJ@Z
// @address 0x2ab380 ?load_required_pages@c_tag_resource_page_table@@QAA_NPAVc_tag_resource_controller_page_table_io_listener@@_N1PAVc_tag_resource_error_receiver@@@Z
// The request iterator is local to this member, as in the development source.
bool c_tag_resource_page_table::load_required_pages(c_tag_resource_controller_page_table_io_listener *listener,
    bool include_reading, bool urgent, c_tag_resource_error_receiver *flag_receiver)
{
    class c_synchronous_resource_io : public c_page_io_interface, public c_indirect_cache_file_bulk_read_iterator
    {
    public:
        __forceinline c_synchronous_resource_io(c_tag_resource_page_table *page_table, c_tag_resource_cache_file_reader *reader,
            c_tag_resource_page_allocator *datum_handler)
        {
            m_request_priorities.element_count = 0;
            s_indirect_cache_file_read_request *element = m_requests.elements;
            long remaining = 1499;
            do
            {
                remaining--;
                element->buffer.m_address = 0;
                element->buffer.m_size = 0;
                element++;
            } while (remaining >= 0);
            m_requests.element_count = 0;
            m_page_handles.element_count = 0;
            m_request_results.element_count = 0;
            m_total_bytes = 0;
            m_next_request = 0;
            m_overflow_count = 0;
            m_page_table = page_table;
            m_reader = reader;
            m_datum_handler = datum_handler;
        }

        virtual bool request_data(long page_handle, const s_indirect_cache_file_read_request *request)
        {
            bool result = false;
            if (m_requests.count() != 1500)
            {
                long request_index = m_page_handles.count();
                *m_requests.new_element() = *request;
                *m_page_handles.new_element() = page_handle;
                unsigned long priority = m_datum_handler->get_request_priority(page_handle);
                *m_request_priorities.new_element() = ((unsigned __int64)priority << 32) | (unsigned long)request_index;
                *m_request_results.new_element() = 5;
                m_total_bytes += request->size;
                result = true;
            }
            else
                m_overflow_count++;
            return result;
        }

        bool block_until_done()
        {
            bool has_more_requests;
            if (m_requests.count() > 0)
            {
                int progress_bytes = m_total_bytes;
                if (g_loading_progress.visible)
                {
                    g_loading_progress.stage = 2;
                    loading_update_progress(progress_bytes);
                }
                progress_new("loading resources (blocking)");
                // match: 0x2ab0c8: reading the count through the array pointer keeps the m_completed_bytes store ahead of the count load
                resource_io_array<unsigned __int64, 1500> *key_array = &m_request_priorities;
                unsigned __int64 *first = key_array->elements;
                m_completed_bytes = 0;
                unsigned __int64 *last = first + key_array->count();
                std::_Sort(first, last, last - first);
                has_more_requests = m_reader->request_data(this);
                progress_done();
                if (g_loading_progress.visible)
                {
                    loading_update_progress(0);
                    g_loading_progress.stage = -1;
                }
            }
            else
                has_more_requests = true;
            return has_more_requests;
        }

        __forceinline unsigned long get_request_count() { return m_requests.count(); }
        __forceinline long get_page_handle(unsigned long index) { return m_page_handles[index]; }
        __forceinline long get_request_result(unsigned long index) { return m_request_results[index]; }
        __forceinline bool any_outstanding_requests() const { return (unsigned long)m_overflow_count != 0; }

    private:
        __forceinline unsigned long get_actual_request_index(unsigned long index)
        {
            unsigned __int64 priority = m_request_priorities[index];
            unsigned long actual_index = (unsigned long)priority;
            return actual_index;
        }

        virtual void reset()
        {
            m_next_request = 0;
            m_completed_bytes = 0;
        }
        virtual bool next(s_indirect_cache_file_read_request *out_read_request, int *out_request_identifier)
        {
            bool result = false;
            if (m_next_request < (unsigned long)m_request_priorities.count())
            {
                unsigned long actual_index = get_actual_request_index(m_next_request);
                *out_read_request = m_requests[actual_index];
                *out_request_identifier = actual_index;
                ++m_next_request;
                result = true;
            }
            return result;
        }
        virtual void finished(unsigned long request_identifier, long result)
        {
            m_request_results[request_identifier] = result;
        }
        virtual void read_progress(unsigned long bytes_read, bool complete)
        {
            m_completed_bytes += bytes_read;
            reinterpret_cast<long (*)(long, long, const char *)>(progress_update_with_description)(m_completed_bytes, m_total_bytes, 0);
        }

        resource_io_array<unsigned __int64, 1500> m_request_priorities;
        resource_io_array<s_indirect_cache_file_read_request, 1500> m_requests;
        resource_io_array<long, 1500> m_page_handles;
        resource_io_array<long, 1500> m_request_results;
        long m_total_bytes;
        long m_completed_bytes;
        unsigned long m_next_request;
        long m_overflow_count;
        c_tag_resource_page_table *m_page_table;
        c_tag_resource_cache_file_reader *m_reader;
        c_tag_resource_page_allocator *m_datum_handler;
    };

    bool complete = true;
    bool more_requests;
    bool done;
    do
    {
        c_synchronous_resource_io io(this, m_reader, m_page_allocator);
        c_data_iterator<s_tag_resource_page_datum> iterator;
        iterator.begin(m_pages);
        while (iterator.next())
        {
            long handle = iterator.get_index();
            c_tag_resource_page_datum page = get_resource_page_from_handle(handle);
            if (page.get_state() < 2)
            {
                if (page_is_ready(page.m_page) || (include_reading && page_is_reading(page.m_page)))
                    tag_resource_page_table_read_page(this, handle, &io, listener);
                else if (page.m_page->io_references == 2)
                {
                    if (urgent)
                        tag_resource_page_table_read_page(this, handle, &io, listener);
                    else
                        tag_resource_page_table_allocate_page(this, handle, listener);
                }
            }
        }
        more_requests = io.block_until_done();
        unsigned long request_count = io.get_request_count();
        for (unsigned long index = 0; index < request_count; index++)
        {
            long handle = io.get_page_handle(index);
            long result = io.get_request_result(index);
            c_tag_resource_page_datum page = get_resource_page_from_handle(handle);
            if (result == 0)
                tag_resource_page_table_set_page_ready(this, handle, true, listener);
            else
            {
                set_page_state(handle, 1);
                page.set_flag(1, true);
                complete = false;
            }
            if (tag_resource_cache_paging_has_pending_flags(page))
                complete = false;
            tag_resource_page_handle_pending_flags(flag_receiver, page);
        }
        done = !more_requests || !io.any_outstanding_requests();
    } while (complete && !done);
    return complete;
}

// @address 0x314db0 tag_resource_cache_paging_decoder_flush_window
extern "C" void tag_resource_cache_paging_decoder_flush_window(tag_resource_cache_paging_decoder_state *state)
{
    int offset = 0;
    int pending = state->window_count;
    while (pending != 0)
    {
        int count = state->consumed_b;
        if (pending < count)
            count = pending;
        if ((state->end_of_stream & 0x20) != 0)
            XMemCpy(state->output, state->window + offset, count);
        else
            memcpy(state->output, state->window + offset, count);
        pending -= count;
        offset += count;
        state->consumed_b -= count;
        state->output += count;
        if (state->consumed_b == 0)
        {
            *state->position_a -= state->bytes_left;
            *state->position_b -= state->consumed_b;
            state->error = 0;
            void *jump_buffer = state->jump_buffer;
            if (jump_buffer != 0)
                SwitchToFiber(jump_buffer);
            if (pending > 0 && state->consumed_b == 0)
            {
                *state->position_a -= state->bytes_left;
                *state->position_b -= state->consumed_b;
                state->error = -7;
                jump_buffer = state->jump_buffer;
                if (jump_buffer != 0)
                    SwitchToFiber(jump_buffer);
            }
        }
    }
    state->window_count = 0;
}

static __forceinline void tag_resource_cache_paging_decoder_yield(tag_resource_cache_paging_decoder_state *state, int code)
{
    int bytes_left = state->bytes_left;
    *state->position_a -= bytes_left;
    *state->position_b -= state->consumed_b;
    state->error = code;
    void *jump_buffer = state->jump_buffer;
    if (jump_buffer != 0)
        SwitchToFiber(jump_buffer);
}

static __forceinline void tag_resource_cache_paging_decoder_fail(tag_resource_cache_paging_decoder_state *state, int code)
{
    tag_resource_cache_paging_decoder_yield(state, code);
    abort();
    // match: 0x315170: drops the epilogue after abort(); without it the caller saves r25 not r26
    __assume(0);
}

// @address 0x315170 tag_resource_cache_paging_decoder_decode_stored_output
extern "C" void tag_resource_cache_paging_decoder_decode_stored_output(tag_resource_cache_paging_decoder_state *state)
{
    long discard = state->bit_count & 7;
    state->bit_count -= discard;
    state->bit_buffer >>= discard;
    if (state->bit_count < 0)
        tag_resource_cache_paging_decoder_refill_bits(state);
    unsigned long length = tag_resource_cache_paging_decoder_get_bits(state, 16);
    unsigned long complement = tag_resource_cache_paging_decoder_get_bits(state, 16);
    if ((unsigned short)~length != complement)
        tag_resource_cache_paging_decoder_fail(state, -4);
    if (length > (unsigned long)state->consumed_b)
        tag_resource_cache_paging_decoder_fail(state, -7);
    long buffered_bits = state->bit_count + 32;
    while (length != 0)
    {
        if (buffered_bits != 0)
        {
            *state->output++ = (unsigned char)state->bit_buffer;
            state->consumed_b--;
            buffered_bits -= 8;
            length--;
            state->bit_buffer >>= 8;
        }
        else
        {
            while (state->bytes_left == 0)
            {
                if ((state->end_of_stream & 0x80) != 0)
                    tag_resource_cache_paging_decoder_fail(state, -4);
                tag_resource_cache_paging_decoder_yield(state, 0);
            }
            unsigned long count = state->bytes_left;
            if (length < count)
                count = length;
            XMemCpy(state->output, state->read_pointer, count);
            length -= count;
            state->consumed_b -= count;
            state->output += count;
            state->read_pointer += count;
            state->bytes_left -= count;
        }
    }
    while (buffered_bits < 32)
    {
        state->bit_buffer |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << buffered_bits);
        buffered_bits += 8;
    }
    state->bit_count = buffered_bits - 32;
}

static __forceinline uintptr_t tag_resource_cache_paging_prefetch(uintptr_t address, uintptr_t limit)
{
    uintptr_t base = address & -128;
    uintptr_t end = base + 0x180;
    if (end > limit)
    {
        if (base > limit)
        {
            limit = base + 0x180;
            end = limit + 0x180;
        }
        uintptr_t span = end - limit;
        uintptr_t offset = 0;
        if (span != 0)
        {
            do
            {
                __dcbt(0, (const void *)(limit + offset));
                offset += 128;
            } while (offset < span);
        }
        limit = end;
    }
    return limit;
}
// @address 0x315408 tag_resource_cache_paging_decoder_decode_huffman_window
extern "C" void tag_resource_cache_paging_decoder_decode_huffman_window(tag_resource_cache_paging_decoder_state *state)
{
    unsigned int extra = 7;
    unsigned char *read_pointer = state->read_pointer;
    unsigned __int64 bits = state->bit_buffer;
    int bit_count = state->bit_count;
    int bytes_left = state->bytes_left;
    int window_count = state->window_count;
    uintptr_t read_aligned = (uintptr_t)read_pointer & -128;
    __dcbt(0x180, (void *)read_aligned);
    uintptr_t read_end = read_aligned + 0x200;
    uintptr_t window_aligned = (uintptr_t)&state->window[window_count] & -128;
    __dcbt(0x180, (void *)window_aligned);
    uintptr_t window_end = window_aligned + 0x200;
    unsigned char *window = state->window;
    for (;;)
    {
        read_end = tag_resource_cache_paging_prefetch((uintptr_t)read_pointer, read_end);
        window_end = tag_resource_cache_paging_prefetch((uintptr_t)(window + window_count), window_end);
        int sym;
        unsigned int len;
        for (;;)
        {
            unsigned long next = (unsigned long)(bits >> extra);
            int node = state->literal_lookup[bits & 0xff];
            if (node < 0)
            {
                unsigned long rest = (unsigned long)(bits >> 8);
                do
                {
                    node = state->literal_long_lookup[(int)(rest & 1) - node - 1];
                    rest >>= 1;
                } while (node < 0);
            }
            sym = (unsigned short)node;
            len = node >> 16;
            if (sym >= 0x100)
                break;
            if (len != extra)
                next = (unsigned long)(bits >> len);
            int node2 = state->literal_lookup[next & 0xff];
            if (node2 < 0)
            {
                unsigned long rest2 = next >> 8;
                do
                {
                    node2 = state->literal_long_lookup[(int)(rest2 & 1) - node2 - 1];
                    rest2 >>= 1;
                } while (node2 < 0);
            }
            int sym2 = (unsigned short)node2;
            extra = (unsigned int)(node2 >> 16);
            len += extra;
            if (sym2 >= 0x100)
            {
                window[window_count] = (unsigned char)sym;
                window_count++;
                if (window_count == 0x8000)
                {
                    state->window_count = 0x8000;
                    state->read_pointer = read_pointer;
                    state->bytes_left = bytes_left;
                    tag_resource_cache_paging_decoder_flush_window(state);
                    window_count = state->window_count;
                    read_pointer = state->read_pointer;
                    bytes_left = state->bytes_left;
                }
                sym = sym2;
                break;
            }
            window[window_count] = (unsigned char)sym;
            bit_count -= len;
            bits >>= len;
            int next_count;
            if (window_count >= 0x7ffe)
            {
                window_count++;
                if (window_count == 0x8000)
                {
                    state->window_count = 0x8000;
                    state->read_pointer = read_pointer;
                    state->bytes_left = bytes_left;
                    tag_resource_cache_paging_decoder_flush_window(state);
                    window_count = state->window_count;
                    read_pointer = state->read_pointer;
                    bytes_left = state->bytes_left;
                }
                window[window_count++] = (unsigned char)sym2;
                if (window_count == 0x8000)
                {
                    state->window_count = 0x8000;
                    state->read_pointer = read_pointer;
                    state->bytes_left = bytes_left;
                    tag_resource_cache_paging_decoder_flush_window(state);
                    window_count = state->window_count;
                    read_pointer = state->read_pointer;
                    bytes_left = state->bytes_left;
                }
                next_count = window_count;
            }
            else
            {
                state->window[window_count + 1] = (unsigned char)sym2;
                next_count = window_count + 2;
            }
            window_count = next_count;
            if (bit_count < 0)
            {
                unsigned __int64 word;
                if ((bytes_left -= 4) < 0)
                {
                    bytes_left += 4;
                    state->window_count = window_count;
                    state->read_pointer = read_pointer;
                    state->bytes_left = bytes_left;
                    word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
                    window_count = state->window_count;
                    read_pointer = state->read_pointer;
                    bytes_left = state->bytes_left;
                }
                else
                {
                    word = __loadwordbytereverse(0, (unsigned int *)read_pointer);
                    read_pointer += 4;
                }
                bit_count += 32;
                bits |= word << bit_count;
            }
        }
        bit_count -= len;
        bits >>= len;
        if (bit_count < 0)
        {
            unsigned __int64 word;
            if ((bytes_left -= 4) < 0)
            {
                bytes_left += 4;
                state->window_count = window_count;
                state->read_pointer = read_pointer;
                state->bytes_left = bytes_left;
                word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
                window_count = state->window_count;
                read_pointer = state->read_pointer;
                bytes_left = state->bytes_left;
            }
            else
            {
                word = __loadwordbytereverse(0, (unsigned int *)read_pointer);
                read_pointer += 4;
            }
            bit_count += 32;
            bits |= (unsigned __int64)word << bit_count;
        }
        if (sym == 0x100)
        {
            state->window_count = window_count;
            state->read_pointer = read_pointer;
            state->bytes_left = bytes_left;
            state->bit_buffer = bits;
            state->bit_count = bit_count;
            return;
        }
        int length = tag_resource_cache_paging_length_base[sym - 0x101];
        if (sym >= 0x109)
        {
            int extra_bits = tag_resource_cache_paging_length_extra_bits[sym - 0x101];
            unsigned long low = (unsigned long)bits;
            bit_count -= extra_bits;
            unsigned long mask = tag_resource_cache_paging_bit_masks[extra_bits];
            int extra_length = (int)(mask & low);
            bits >>= extra_bits;
            if (bit_count < 0)
            {
                unsigned __int64 word;
                if ((bytes_left -= 4) < 0)
                {
                    bytes_left += 4;
                    state->window_count = window_count;
                    state->read_pointer = read_pointer;
                    state->bytes_left = bytes_left;
                    word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
                    window_count = state->window_count;
                    read_pointer = state->read_pointer;
                    bytes_left = state->bytes_left;
                }
                else
                {
                    word = __loadwordbytereverse(0, (unsigned int *)read_pointer);
                    read_pointer += 4;
                }
                bit_count += 32;
                bits |= (unsigned __int64)word << bit_count;
            }
            length += extra_length;
        }
        int distance_node = state->distance_lookup[bits & 0xff];
        if (distance_node < 0)
        {
            unsigned long distance_rest = (unsigned long)(bits >> 8);
            do
            {
                distance_node = state->distance_long_lookup[(int)(distance_rest & 1) - distance_node - 1];
                distance_rest >>= 1;
            } while (distance_node < 0);
        }
        int distance_sym = distance_node & 0xffff;
        unsigned int distance_count = (unsigned int)(distance_node >> 16);
        bit_count -= distance_count;
        bits >>= distance_count;
        if (bit_count < 0)
        {
            unsigned __int64 word;
            if ((bytes_left -= 4) < 0)
            {
                bytes_left += 4;
                state->window_count = window_count;
                state->read_pointer = read_pointer;
                state->bytes_left = bytes_left;
                word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
                window_count = state->window_count;
                read_pointer = state->read_pointer;
                bytes_left = state->bytes_left;
            }
            else
            {
                word = __loadwordbytereverse(0, (unsigned int *)read_pointer);
                read_pointer += 4;
            }
            bit_count += 32;
            bits |= (unsigned __int64)word << bit_count;
        }
        int distance_extra = tag_resource_cache_paging_distance_extra_bits[distance_sym];
        bit_count -= distance_extra;
        unsigned long distance_mask = tag_resource_cache_paging_bit_masks[distance_extra];
        int distance_low = (int)(distance_mask & (unsigned long)bits);
        bits >>= distance_extra;
        if (bit_count < 0)
        {
            unsigned __int64 word;
            if ((bytes_left -= 4) < 0)
            {
                bytes_left += 4;
                state->window_count = window_count;
                state->read_pointer = read_pointer;
                state->bytes_left = bytes_left;
                word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
                window_count = state->window_count;
                read_pointer = state->read_pointer;
                bytes_left = state->bytes_left;
            }
            else
            {
                word = __loadwordbytereverse(0, (unsigned int *)read_pointer);
                read_pointer += 4;
            }
            bit_count += 32;
            bits |= (unsigned __int64)word << bit_count;
        }
        int dist = tag_resource_cache_paging_distance_base[distance_sym] + distance_low;
        if (dist > window_count || length + window_count >= 0x8000)
        {
            for (; length > 0; length--)
            {
                window[window_count] = state->window[(window_count - dist) & 0x7fff];
                window_count++;
                if (window_count == 0x8000)
                {
                    state->window_count = 0x8000;
                    state->read_pointer = read_pointer;
                    state->bytes_left = bytes_left;
                    tag_resource_cache_paging_decoder_flush_window(state);
                    window_count = state->window_count;
                    read_pointer = state->read_pointer;
                    bytes_left = state->bytes_left;
                }
            }
            continue;
        }
        unsigned char *dest = window + window_count;
        window_count += length;
        unsigned char *src = dest - dist;
        if (length > dist)
        {
            if (dist == 1)
            {
                XMemSet(dest, *src, length);
                continue;
            }
            for (int i = length; i > 0; i--)
                *dest++ = *src++;
            continue;
        }
        XMemCpy(dest, src, length);
    }
}
// @address 0x315b70 tag_resource_cache_paging_decoder_decode_huffman_output
extern "C" void tag_resource_cache_paging_decoder_decode_huffman_output(tag_resource_cache_paging_decoder_state *state)
{
    unsigned char *read_pointer = state->read_pointer;
    unsigned __int64 bits = state->bit_buffer;
    int bit_count = state->bit_count;
    unsigned char *output = state->output;
    int output_left = state->consumed_b;
    int bytes_left = state->bytes_left;
    uintptr_t read_aligned = (uintptr_t)read_pointer & -128;
    __dcbt(0x180, (void *)read_aligned);
    uintptr_t read_end = read_aligned + 0x200;
    uintptr_t output_aligned = (uintptr_t)output & -128;
    __dcbt(0x180, (void *)output_aligned);
    uintptr_t output_end = output_aligned + 0x200;
    unsigned int extra = 7;
    for (;;)
    {
        read_end = tag_resource_cache_paging_prefetch((uintptr_t)read_pointer, read_end);
        output_end = tag_resource_cache_paging_prefetch((uintptr_t)output, output_end);
        int sym;
        unsigned int total_len;
        for (;;)
        {
            unsigned long next = (unsigned long)(bits >> extra);
            int node = state->literal_lookup[bits & 0xff];
            if (node < 0)
            {
                unsigned long rest = (unsigned long)(bits >> 8);
                do
                {
                    node = state->literal_long_lookup[(int)(rest & 1) - node - 1];
                    rest >>= 1;
                } while (node < 0);
            }
            sym = (unsigned short)node;
            unsigned int len = node >> 16;
            total_len = len;
            if (sym >= 0x100)
                break;
            if (len != extra)
                next = (unsigned long)(bits >> len);
            int node2 = state->literal_lookup[next & 0xff];
            if (node2 < 0)
            {
                unsigned long rest2 = next >> 8;
                do
                {
                    node2 = state->literal_long_lookup[(int)(rest2 & 1) - node2 - 1];
                    rest2 >>= 1;
                } while (node2 < 0);
            }
            int sym2 = (unsigned short)node2;
            extra = (unsigned int)(node2 >> 16);
            total_len = len + extra;
            if (sym2 >= 0x100)
            {
                if (--output_left < 0)
                {
                    output_left++;
                    state->output = output;
                    state->consumed_b = output_left;
                    state->read_pointer = read_pointer;
                    state->bytes_left = bytes_left;
                    tag_resource_cache_paging_decoder_fail(state, -7);
                }
                *output++ = (unsigned char)sym;
                sym = sym2;
                break;
            }
            bit_count -= total_len;
            bits >>= total_len;
            if ((output_left -= 2) < 0)
            {
                output_left += 2;
                state->output = output;
                state->consumed_b = output_left;
                state->read_pointer = read_pointer;
                state->bytes_left = bytes_left;
                tag_resource_cache_paging_decoder_fail(state, -7);
            }
            output[0] = (unsigned char)sym;
            output[1] = (unsigned char)sym2;
            output += 2;
            if (bit_count < 0)
            {
                unsigned __int64 word;
                if ((bytes_left -= 4) < 0)
                {
                    bytes_left += 4;
                    state->output = output;
                    state->consumed_b = output_left;
                    state->read_pointer = read_pointer;
                    state->bytes_left = bytes_left;
                    word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
                    output = state->output;
                    output_left = state->consumed_b;
                    read_pointer = state->read_pointer;
                    bytes_left = state->bytes_left;
                }
                else
                {
                    word = __loadwordbytereverse(0, (unsigned int *)read_pointer);
                    read_pointer += 4;
                }
                bit_count += 32;
                bits |= word << bit_count;
            }
        }
        bit_count -= total_len;
        bits >>= total_len;
        if (bit_count < 0)
        {
            unsigned __int64 word;
            if ((bytes_left -= 4) < 0)
            {
                bytes_left += 4;
                state->output = output;
                state->consumed_b = output_left;
                state->read_pointer = read_pointer;
                state->bytes_left = bytes_left;
                word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
                output = state->output;
                output_left = state->consumed_b;
                read_pointer = state->read_pointer;
                bytes_left = state->bytes_left;
            }
            else
            {
                word = __loadwordbytereverse(0, (unsigned int *)read_pointer);
                read_pointer += 4;
            }
            bit_count += 32;
            bits |= (unsigned __int64)word << bit_count;
        }
        if (sym == 0x100)
        {
            state->output = output;
            state->consumed_b = output_left;
            state->read_pointer = read_pointer;
            state->bytes_left = bytes_left;
            state->bit_buffer = bits;
            state->bit_count = bit_count;
            return;
        }
        int length = tag_resource_cache_paging_length_base[sym - 0x101];
        if (sym >= 0x109)
        {
            int extra_bits = tag_resource_cache_paging_length_extra_bits[sym - 0x101];
            bit_count -= extra_bits;
            int extra_length = (int)(tag_resource_cache_paging_bit_masks[extra_bits] & (unsigned long)bits);
            bits >>= extra_bits;
            if (bit_count < 0)
            {
                unsigned __int64 word;
                if ((bytes_left -= 4) < 0)
                {
                    bytes_left += 4;
                    state->output = output;
                    state->consumed_b = output_left;
                    state->read_pointer = read_pointer;
                    state->bytes_left = bytes_left;
                    word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
                    word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
                    output = state->output;
                    output_left = state->consumed_b;
                    read_pointer = state->read_pointer;
                    bytes_left = state->bytes_left;
                }
                else
                {
                    word = __loadwordbytereverse(0, (unsigned int *)read_pointer);
                    read_pointer += 4;
                }
                bit_count += 32;
                bits |= (unsigned __int64)word << bit_count;
            }
            length += extra_length;
        }
        if (length > output_left)
        {
            state->output = output;
            state->consumed_b = output_left;
            state->read_pointer = read_pointer;
            state->bytes_left = bytes_left;
            tag_resource_cache_paging_decoder_fail(state, -7);
        }
        int distance_node = state->distance_lookup[bits & 0xff];
        if (distance_node < 0)
        {
            unsigned long distance_rest = (unsigned long)(bits >> 8);
            do
            {
                distance_node = state->distance_long_lookup[(int)(distance_rest & 1) - distance_node - 1];
                distance_rest >>= 1;
            } while (distance_node < 0);
        }
        int distance_sym = distance_node & 0xffff;
        unsigned int distance_count = (unsigned int)(distance_node >> 16);
        bit_count -= distance_count;
        bits >>= distance_count;
        if (bit_count < 0)
        {
            unsigned __int64 word;
            if ((bytes_left -= 4) < 0)
            {
                bytes_left += 4;
                state->output = output;
                state->consumed_b = output_left;
                state->read_pointer = read_pointer;
                state->bytes_left = bytes_left;
                word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
                output = state->output;
                output_left = state->consumed_b;
                read_pointer = state->read_pointer;
                bytes_left = state->bytes_left;
            }
            else
            {
                word = __loadwordbytereverse(0, (unsigned int *)read_pointer);
                read_pointer += 4;
            }
            bit_count += 32;
            bits |= (unsigned __int64)word << bit_count;
        }
        int distance_extra = tag_resource_cache_paging_distance_extra_bits[distance_sym];
        bit_count -= distance_extra;
        int distance_low = (int)(tag_resource_cache_paging_bit_masks[distance_extra] & (unsigned long)bits);
        bits >>= distance_extra;
        if (bit_count < 0)
        {
            unsigned __int64 word;
            if ((bytes_left -= 4) < 0)
            {
                bytes_left += 4;
                state->output = output;
                state->consumed_b = output_left;
                state->read_pointer = read_pointer;
                state->bytes_left = bytes_left;
                word = (unsigned int)tag_resource_cache_paging_decoder_get_byte(state);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 8);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 16);
                word |= (unsigned __int64)(unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << 24);
                output = state->output;
                output_left = state->consumed_b;
                read_pointer = state->read_pointer;
                bytes_left = state->bytes_left;
            }
            else
            {
                word = __loadwordbytereverse(0, (unsigned int *)read_pointer);
                read_pointer += 4;
            }
            bit_count += 32;
            bits |= (unsigned __int64)word << bit_count;
        }
        int dist = tag_resource_cache_paging_distance_base[distance_sym] + distance_low;
        unsigned char *dest = output;
        unsigned char *src = output - dist;
        if (src < state->output_base)
        {
            state->output = output;
            state->consumed_b = output_left;
            state->read_pointer = read_pointer;
            state->bytes_left = bytes_left;
            tag_resource_cache_paging_decoder_fail(state, -6);
        }
        if (length > dist)
        {
            if (dist == 1)
            {
                XMemSet(dest, *src, length);
            }
            else
            {
                for (int i = length; i > 0; i--)
                    *dest++ = *src++;
            }
        }
        else if (length > 8)
        {
            XMemCpy(dest, src, length);
        }
        else
        {
            for (int i = length; i > 0; i--)
                *dest++ = *src++;
        }
        output += length;
        output_left -= length;
    }
}
// @address 0x316dc0 tag_resource_cache_paging_decoder_initialize_distance_table
static __forceinline __declspec(noreturn) void tag_resource_cache_paging_code_length_fail(tag_resource_cache_paging_decoder_state *);
static __forceinline void tag_resource_cache_paging_fill_table_32(const signed char *lengths, const unsigned long *counts, long *table, long *nodes)
{
    long next_node = -1;
    for (unsigned long symbol = 0; symbol < 32; symbol++)
    {
        long length = lengths[symbol];
        if (length != 0)
        {
            unsigned long codeword = counts[symbol];
            unsigned long step = 0;
            unsigned long reversed = 0;
            unsigned long count = (unsigned long)length;
            do { reversed = (reversed << 1) | (codeword & 1); codeword >>= 1; } while (--count != 0);
            if (length <= 8)
            {
                step = 1 << length;
                if (reversed < 256)
                {
                    long *entry = &table[reversed];
                    long stride = (long)(step << 2);
                    do
                    {
                        reversed += step;
                        *entry = (lengths[symbol] << 16) | symbol;
                        entry = (long *)((char *)entry + stride);
                    } while (reversed < 256);
                }
            }
            else
            {
                long *root = &table[reversed & 0xff];
                long node = *root;
                if (node == 0)
                {
                    node = next_node;
                    *root = next_node;
                    next_node -= 2;
                }
                unsigned long tree_length = (unsigned long)lengths[symbol];
                unsigned long bits = reversed >> 7;
                for (unsigned long bit = 9; bit < tree_length; bit++)
                {
                    bits >>= 1;
                    // Child slots live below the node array: wrap the negative node bias
                    // into an unsigned 31-bit offset so c2 keeps the flat subf/slwi/subf form.
#ifdef HALO3_64BIT
                    long *child = &nodes[(long)(bits & 1) - node - 1];
#else
                    unsigned long child_offset = (node - (long)(bits & 1) + 1) & 0x7fffffff;
                    long *child = nodes - child_offset;
#endif
                    node = *child;
                    if (node == 0)
                    {
                        node = next_node;
                        *child = next_node;
                        next_node -= 2;
                    }
                }
                nodes[(long)((bits >> 1) & 1) - node - 1] = (lengths[symbol] << 16) | symbol;
            }
        }
    }
}
extern "C" void tag_resource_cache_paging_decoder_initialize_distance_table(tag_resource_cache_paging_decoder_state *state, int *a, int *b, int *c)
{
    int counts[17];
    int next_code[17];
    int codes[32];
    signed char *lengths = (signed char *)a;
    int *root_table = b;
    int *sub_table = c;
    int i;
    for (i = 0; i < 17; i++)
        counts[i] = 0;
    for (i = 0; i < 32; i++)
        counts[lengths[i]]++;
    next_code[1] = 0;
    int code = 0;
    next_code[0] = 0;
    for (unsigned k = 1; k <= 15; k++)
    {
        code = (counts[k] + code) << 1;
        next_code[k + 1] = code;
    }
    unsigned valid = 1;
    if ((unsigned)code != 0x10000)
    {
        unsigned length = 16;
        unsigned total = 0;
        for (; length != 0; length--)
        {
            total = (unsigned)counts[length] + total;
            if (total > 1)
            {
                valid = 0;
                break;
            }
        }
    }
    if (valid)
    {
        for (i = 0; i < 32; i++)
            codes[i] = next_code[lengths[i]]++;
        for (i = 0; i < 256; i++)
            root_table[i] = 0;
        for (i = 0; i < 64; i++)
            sub_table[i] = 0;
        tag_resource_cache_paging_fill_table_32(lengths, (const unsigned long *)codes, (long *)root_table, (long *)sub_table);
        return;
    }
    tag_resource_cache_paging_code_length_fail(state);
}
// @address 0x3163b8 tag_resource_cache_paging_decoder_decode_huffman_block
extern "C" void tag_resource_cache_paging_decoder_decode_huffman_block(tag_resource_cache_paging_decoder_state *state)
{
    tag_resource_cache_paging_decoder_initialize_literal_table(state, (int *)state->literal_lengths, state->literal_lookup, state->literal_long_lookup);
    tag_resource_cache_paging_decoder_initialize_distance_table(state, (int *)state->distance_lengths, state->distance_lookup, state->distance_long_lookup);
    if ((state->end_of_stream & 0x40) != 0)
        tag_resource_cache_paging_decoder_decode_huffman_window(state);
    else
        tag_resource_cache_paging_decoder_decode_huffman_output(state);
}

// @address 0x3169f8 tag_resource_cache_paging_decoder_handle_fiber
extern "C" void WINAPI tag_resource_cache_paging_decoder_handle_fiber(void *decoder_state)
{
    tag_resource_cache_paging_decoder_decode_stream(static_cast<tag_resource_cache_paging_decoder_state *>(decoder_state));
}

static __forceinline void tag_resource_cache_paging_fill_table_288(const signed char *lengths, const unsigned long *counts, long *table, long *nodes)
{
    long next_node = -1;
    for (unsigned long symbol = 0; symbol < 288; symbol++)
    {
        long length = lengths[symbol];
        if (length != 0)
        {
            unsigned long codeword = counts[symbol];
            unsigned long step = 0;
            unsigned long reversed = 0;
            unsigned long count = (unsigned long)length;
            do { reversed = (reversed << 1) | (codeword & 1); codeword >>= 1; } while (--count != 0);
            if (length <= 8)
            {
                step = 1 << length;
                if (reversed < 256)
                {
                    long *entry = &table[reversed];
                    long stride = (long)(step << 2);
                    do
                    {
                        reversed += step;
                        *entry = (lengths[symbol] << 16) | symbol;
                        entry = (long *)((char *)entry + stride);
                    } while (256 > reversed);
                }
            }
            else
            {
                long *root = &table[reversed & 0xff];
                long node = *root;
                if (node == 0)
                {
                    node = next_node;
                    *root = next_node;
                    next_node -= 2;
                }
                unsigned long tree_length = (unsigned long)lengths[symbol];
                unsigned long bits = reversed >> 7;
                for (unsigned long bit = 9; bit < tree_length; bit++)
                {
                    bits >>= 1;
#ifdef HALO3_64BIT
                    long *child = &nodes[(long)(bits & 1) - node - 1];
#else
                    unsigned long child_offset = (node - (long)(bits & 1) + 1) & 0x7fffffff;
                    long *child = nodes - child_offset;
#endif
                    node = *child;
                    if (node == 0)
                    {
                        node = next_node;
                        *child = next_node;
                        next_node -= 2;
                    }
                }
                nodes[(long)((bits >> 1) & 1) - node - 1] = (lengths[symbol] << 16) | symbol;
            }
        }
    }
}
// @address 0x316a00 tag_resource_cache_paging_decoder_initialize_literal_table
extern "C" void tag_resource_cache_paging_decoder_initialize_literal_table(tag_resource_cache_paging_decoder_state *state, int *a, int *b, int *c)
{
    int counts[17];
    int next_code[17];
    int codes[288];
    signed char *lengths = (signed char *)a;
    int *root_table = b;
    int *sub_table = c;
    int i;
    for (i = 0; i < 17; i++)
        counts[i] = 0;
    for (i = 0; i < 288; i++)
        counts[lengths[i]]++;
    next_code[1] = 0;
    int code = 0;
    next_code[0] = 0;
    for (unsigned k = 1; k <= 15; k++)
    {
        code = (counts[k] + code) << 1;
        next_code[k + 1] = code;
    }
    unsigned valid = 1;
    if ((unsigned)code != 0x10000)
    {
        unsigned length = 16;
        unsigned total = 0;
        for (; length != 0; length--)
        {
            total = (unsigned)counts[length] + total;
            if (total > 1)
            {
                valid = 0;
                break;
            }
        }
    }
    if (valid)
    {
        for (i = 0; i < 288; i++)
            codes[i] = next_code[lengths[i]]++;
        for (i = 0; i < 256; i++)
            root_table[i] = 0;
        for (i = 0; i < 576; i++)
            sub_table[i] = 0;
        tag_resource_cache_paging_fill_table_288(lengths, (const unsigned long *)codes, (long *)root_table, (long *)sub_table);
        return;
    }
    tag_resource_cache_paging_code_length_fail(state);
}

static __forceinline void tag_resource_cache_paging_fill_code_length_table(const signed char *lengths, const unsigned long *counts, long *table, long *nodes)
{
    long next_node = -1;
    for (unsigned long symbol = 0; symbol < 19; symbol++)
    {
        long length = lengths[symbol];
        if (length != 0)
        {
            unsigned long codeword = counts[symbol];
            unsigned long step = 0;
            unsigned long reversed = 0;
            unsigned long count = length;
            do { reversed = (reversed << 1) | (codeword & 1); codeword >>= 1; } while (--count != 0);
            if (length <= 8)
            {
                step = 1 << length;
                for (; reversed < 256; reversed += step)
                    table[reversed] = (lengths[symbol] << 16) | symbol;
            }
            else
            {
                long *root = &table[reversed & 0xff];
                long node = *root;
                if (node == 0)
                {
                    node = next_node;
                    *root = next_node;
                    next_node -= 2;
                }
                unsigned long tree_length = lengths[symbol];
                unsigned long bits = reversed >> 7;
                for (unsigned long bit = 9; bit < tree_length; bit++)
                {
                    bits >>= 1;
                    // Child slots live below the node array: fold the negative node bias
                    // into an unsigned 31-bit offset before scaling it to a slot pointer.
#ifdef HALO3_64BIT
                    long *child = &nodes[(long)(bits & 1) - node - 1];
#else
                    unsigned long child_offset = (node - (long)(bits & 1) + 1) & 0x7fffffff;
                    long *child = nodes - child_offset;
#endif
                    node = *child;
                    if (node == 0)
                    {
                        node = next_node;
                        *child = next_node;
                        next_node -= 2;
                    }
                }
                nodes[(long)((bits >> 1) & 1) - node - 1] = (lengths[symbol] << 16) | symbol;
            }
        }
    }
}

// @address 0x317108 tag_resource_cache_paging_decoder_initialize_code_table
extern "C" void tag_resource_cache_paging_decoder_initialize_code_table(
    tag_resource_cache_paging_decoder_state *state, const signed char *lengths, long *table, long *nodes)
{
    unsigned long counts[19];
    unsigned long next_code[18];
    for (unsigned long length = 0; length < 17; length++)
        counts[length] = 0;
    for (unsigned long symbol = 0; symbol < 19; symbol++)
        counts[lengths[symbol]]++;
    next_code[0] = next_code[1] = 0;
    unsigned long code = 0;
    for (unsigned long length = 1; length <= 15; length++)
    {
        code = (counts[length] + code) << 1;
        next_code[length + 1] = code;
    }
    unsigned long valid = 1;
    if (code != 0x10000)
    {
        unsigned long length = 16;
        unsigned long total = 0;
        for (; length != 0; length--)
        {
            total = counts[length] + total;
            if (total > 1) { valid = 0; break; }
        }
    }
    if (valid)
    {
    for (unsigned long symbol = 0; symbol < 19; symbol++)
        counts[symbol] = next_code[lengths[symbol]]++;
    for (unsigned long index = 0; index < 256; index++)
        table[index] = 0;
    for (unsigned long index = 0; index < 38; index++)
        nodes[index] = 0;
    tag_resource_cache_paging_fill_code_length_table(lengths, counts, table, nodes);
    return;
    }
    tag_resource_cache_paging_code_length_fail(state);
}

static __forceinline __declspec(noreturn) void tag_resource_cache_paging_code_length_fail(tag_resource_cache_paging_decoder_state *state)
{
    tag_resource_cache_paging_decoder_set_error_and_abort(state, -1);
    // match: 0x316dc0: drops the epilogue after abort(); without it the add r9 operands swap
    __assume(0);
}

// @address 0x314ee8 tag_resource_cache_paging_decoder_decode_stored_window
extern "C" void tag_resource_cache_paging_decoder_decode_stored_window(tag_resource_cache_paging_decoder_state *state)
{
    long discard = state->bit_count & 7;
    state->bit_count -= discard;
    state->bit_buffer >>= discard;
    if (state->bit_count < 0)
        tag_resource_cache_paging_decoder_refill_bits(state);
    unsigned long length = tag_resource_cache_paging_decoder_get_bits(state, 16);
    unsigned long complement = tag_resource_cache_paging_decoder_get_bits(state, 16);
    if ((unsigned short)~length != complement)
        tag_resource_cache_paging_decoder_fail(state, -4);
    long buffered_bits = state->bit_count + 32;
    while (length != 0)
    {
        if (buffered_bits != 0)
        {
            state->window[state->window_count++] = (unsigned char)state->bit_buffer;
            if (state->window_count == 0x8000)
                tag_resource_cache_paging_decoder_flush_window(state);
            buffered_bits -= 8;
            length--;
            state->bit_buffer >>= 8;
        }
        else
        {
            while (state->bytes_left == 0)
            {
                if ((state->end_of_stream & 0x80) != 0)
                    tag_resource_cache_paging_decoder_fail(state, -4);
                tag_resource_cache_paging_decoder_yield(state, 0);
            }
            unsigned long space = 0x8000 - state->window_count;
            unsigned long count = state->bytes_left;
            if (length < count)
                count = length;
            if (count < space)
                space = count;
            XMemCpy(state->window + state->window_count, state->read_pointer, space);
            state->window_count += space;
            state->read_pointer += space;
            state->bytes_left -= space;
            if (state->window_count == 0x8000)
                tag_resource_cache_paging_decoder_flush_window(state);
            length -= space;
        }
    }
    while (buffered_bits < 32)
    {
        state->bit_buffer |= (unsigned int)(tag_resource_cache_paging_decoder_get_byte(state) << buffered_bits);
        buffered_bits += 8;
    }
    state->bit_count = buffered_bits - 32;
}


static __forceinline long tag_resource_cache_paging_lruv_new_block(
    c_tag_resource_page_table::c_tag_resource_lruv_cache *lruv, long handle, c_basic_buffer<void> buffer)
{
    long page_index = (unsigned long)((char *)buffer.m_address - (char *)lruv->m_buffer.m_address) >> lruv->m_alignment_bits;
    return lruv_block_new_at_page(lruv->m_cache, handle, page_index, buffer.m_size);
}

#if !defined(_XBOX) || defined(__clang__)
// A page tag_resource_page_table_new_page takes over from the relocation window was not read just now. It is either
// one an earlier map's table made ready (in host order already: the lobby precaches the next map's pages, and maps
// share pages) or one the loader read straight into the window (as on disc). This puts it in disc order, the order
// tag_resource_page_table_set_page_ready converts from. A page the port converted is known by its bytes; any other is
// told by its hash, which covers the bytes as on disc. The hash alone is not enough: the 8 MB first page of 005_intro
// matches it in both orders. Returns whether the page must still be verified: only a page matching its hash in
// neither order, which then fails verification and is read again.
static bool tag_resource_page_relocated_to_disc_order(c_tag_resource_page_datum *page)
{
    const s_cache_file_header *header = &g_cache_file_globals.header;
    const void *tag_buffer = g_cache_file_globals.tag_cache_base_address;
    unsigned long size = page->m_datum_handler->get_page_size(page->m_handle);
    if (port_resource_page_is_host_order(page->m_page->buffer.m_address, size))
    {
        cache_file_resource_page_to_big_endian(header, tag_buffer, page);
        return false;
    }
    s_tag_resource_page_hash_pair expected;
    if (!page->m_datum_handler->get_page_hash(page->m_handle, &expected))
        return false; // no hash to tell by: as on disc, the order every read has
    if (tag_resource_cache_paging_verify_hash(page->m_page->buffer, size, expected))
        return false;
    cache_file_resource_page_to_big_endian(header, tag_buffer, page);
    if (tag_resource_cache_paging_verify_hash(page->m_page->buffer, size, expected))
        return false;
    cache_file_resource_page_to_host(header, tag_buffer, page); // neither order: back as it was
    return true;
}
#endif

// @address 0x2aacf8 tag_resource_page_table_read_pages
extern "C" bool tag_resource_page_table_read_pages(
    c_tag_resource_page_table *table, s_tag_resource_sync_reader *reader, c_tag_resource_controller_page_table_io_listener *listener)
{
    c_basic_buffer<void> window = reader->get_buffer();
    long handle;
    c_basic_buffer<void> buffer(c_basic_buffer<void>::_uninitialized);
    bool verify;
    c_tag_resource_page_table::c_tag_resource_lruv_cache *lruv_cache = &table->m_lruv_cache;
    bool result = lruv_cache->m_allocator->set_window(window, &lruv_cache->m_buffer);
    if (result)
    {
        resource_lruv_resize_non_destructive(lruv_cache->m_cache, lruv_cache->m_buffer.m_size >> lruv_cache->m_alignment_bits);
    }
    if (result)
    {
        buffer.m_address = 0;
        buffer.m_size = 0;
        result = reader->next_matching_page(&handle, &buffer, &verify);
        while (result)
        {
            tag_resource_page_table_new_page(table, handle, buffer, verify, listener);
            result = reader->next_matching_page(&handle, &buffer, &verify);
        }
    }
    return result;
}

// @address 0x2aae18 tag_resource_page_table_new_page
extern "C" void tag_resource_page_table_new_page(c_tag_resource_page_table *table, long handle, c_basic_buffer<void> buffer, bool verify,
    c_tag_resource_controller_page_table_io_listener *listener)
{
    c_basic_buffer<void> page_buffer = buffer;
    c_tag_resource_page_datum page = table->get_resource_page_from_handle(handle);
    if (tag_resource_cache_paging_lruv_new_block(&table->m_lruv_cache, handle, page_buffer) == handle)
    {
        unsigned long size = tag_resource_cache_paging_lruv_block_size(&table->m_lruv_cache, handle);
        page.m_page->buffer.m_address = page_buffer.m_address;
        page.m_page->buffer.m_size = size;
        page.set_flag(0, true);
        if (tag_resource_page_datum_is_unavailable(&page))
            tag_resource_cache_paging_lruv_block(table->m_lruv_cache.m_cache, handle)->flags |= 2;
        table->set_page_state(handle, 2);
#if !defined(_XBOX) || defined(__clang__)
        verify = tag_resource_page_relocated_to_disc_order(&page);
#endif
        tag_resource_page_table_set_page_ready(table, handle, verify, listener);
    }
}

// @address 0x316440 tag_resource_cache_paging_decoder_decode_dynamic_tables
extern "C" void tag_resource_cache_paging_decoder_decode_dynamic_tables(tag_resource_cache_paging_decoder_state *state)
{
    unsigned char length_buffer[320];
    XMemSet(state->literal_lengths, 0, sizeof(state->literal_lengths));
    XMemSet(state->distance_lengths, 0, sizeof(state->distance_lengths));
    XMemSet(state->code_lengths, 0, sizeof(state->code_lengths));
    long literal_count = tag_resource_cache_paging_decoder_get_bits(state, 5) + 257;
    if (literal_count > 286)
        tag_resource_cache_paging_decoder_fail(state, -5);
    long distance_count = tag_resource_cache_paging_decoder_get_bits(state, 5) + 1;
    if (distance_count > 30)
        tag_resource_cache_paging_decoder_fail(state, -5);
    long total = literal_count + distance_count;
    long index = 0;
    long count = tag_resource_cache_paging_decoder_get_bits(state, 4) + 4;
    if (count > 0)
    {
        do
        {
            state->code_lengths[k_deflate_code_length_order[index]] = (signed char)tag_resource_cache_paging_decoder_get_bits(state, 3);
            index++;
        } while (index < count);
    }
    tag_resource_cache_paging_decoder_initialize_code_table(state, state->code_lengths, (long *)state->code_table, (long *)state->code_nodes);
    index = 0;
    if (total > 0)
    {
        do
        {
            long symbol = tag_resource_cache_paging_decoder_get_symbol(state, state->code_table, state->code_nodes);
            long extra = 0;
            unsigned char value;
            value = 0;
            if (symbol <= 15)
            {
                long next = index + 1;
                if (next > total)
                    tag_resource_cache_paging_decoder_fail(state, -5);
                length_buffer[index] = (unsigned char)symbol;
                index = next;
                continue;
            }
            switch (symbol)
            {
            case _deflate_repeat_previous_length:
                extra = tag_resource_cache_paging_decoder_get_bits(state, 2) + 3;
                if (index != 0)
                    value = length_buffer[index - 1];
                else
                    value = 0;
                break;
            case _deflate_repeat_zero_short:
                extra = tag_resource_cache_paging_decoder_get_bits(state, 3) + 3;
                value = 0;
                break;
            case _deflate_repeat_zero_long:
                extra = tag_resource_cache_paging_decoder_get_bits(state, 7) + 11;
                value = 0;
                break;
            }
            long next = extra + index;
            if (next > total)
                tag_resource_cache_paging_decoder_fail(state, -5);
            if (extra != 0)
            {
                unsigned char *fill = &length_buffer[index];
                for (unsigned long i = 0; i < (unsigned long)extra; i++)
                    fill[i] = value;
                index = next;
            }
        } while (index < total);
    }
    XMemCpy(state->literal_lengths, length_buffer, literal_count);
    XMemCpy(state->distance_lengths, length_buffer + literal_count, distance_count);
    tag_resource_cache_paging_decoder_decode_huffman_block(state);
}

// @address 0x316758 tag_resource_cache_paging_decoder_decode_block_header
extern "C" void tag_resource_cache_paging_decoder_decode_block_header(tag_resource_cache_paging_decoder_state *state)
{
    switch (tag_resource_cache_paging_decoder_get_bits(state, 2))
    {
    case 0:
        if ((state->end_of_stream & 0x40) != 0)
            tag_resource_cache_paging_decoder_decode_stored_window(state);
        else
            tag_resource_cache_paging_decoder_decode_stored_output(state);
        return;
    case 1:
        XMemSet(&state->literal_lengths[0], 8, _deflate_fixed_8bit_literal_count);
        XMemSet(&state->literal_lengths[144], 9, _deflate_fixed_9bit_literal_count);
        XMemSet(&state->literal_lengths[256], 7, _deflate_fixed_7bit_literal_count);
        XMemSet(&state->literal_lengths[280], 8, _deflate_fixed_trailer_literal_count);
        XMemSet(state->distance_lengths, 5, sizeof(state->distance_lengths));
        tag_resource_cache_paging_decoder_decode_huffman_block(state);
        return;
    case 2:
        tag_resource_cache_paging_decoder_decode_dynamic_tables(state);
        return;
    case 3:
        tag_resource_cache_paging_decoder_fail(state, -3);
        return;
    }
}

static __forceinline c_basic_buffer<void> tag_resource_cache_paging_lruv_page_buffer(
    c_tag_resource_page_table::c_tag_resource_lruv_cache *lruv, unsigned long page_index, unsigned long size)
{
    c_basic_buffer<void> buffer = lruv->m_buffer;
    return c_basic_buffer<void>(static_cast<char *>(buffer.m_address) + (page_index << lruv->m_cache->page_size_bits), size);
}
// @address 0x2acb98 ?lruv_move_tag_resource_page@c_tag_resource_page_table@@AAAXJKK@Z
void c_tag_resource_page_table::lruv_move_tag_resource_page(long handle, unsigned long old_page_index, unsigned long new_page_index)
{
    c_tag_resource_page_datum page = get_resource_page_from_handle(handle);
    unsigned long size = tag_resource_cache_paging_lruv_block_size(&m_lruv_cache, handle);
    c_basic_buffer<void> old_buffer = tag_resource_cache_paging_lruv_page_buffer(&m_lruv_cache, old_page_index, size);
    c_basic_buffer<void> new_buffer = tag_resource_cache_paging_lruv_page_buffer(&m_lruv_cache, new_page_index, size);
    if (page.get_state() >= 3)
        m_published_interface->prepare_for_move(handle);
    if (page.get_state() == 3)
        memmove(new_buffer.m_address, old_buffer.m_address, size);
    page.m_page->buffer = new_buffer;
}

// @address 0x3168c0 tag_resource_cache_paging_decoder_decode_stream
extern "C" void tag_resource_cache_paging_decoder_decode_stream(tag_resource_cache_paging_decoder_state *state)
{
    state->bit_buffer = (unsigned __int64)(unsigned int)tag_resource_cache_paging_decoder_get_dword(state);
    state->bit_buffer |= (unsigned __int64)tag_resource_cache_paging_decoder_get_dword(state) << 32;
    state->bit_count = 32;
    long bit;
    do
    {
        bit = tag_resource_cache_paging_decoder_get_bits(state, 1);
        tag_resource_cache_paging_decoder_decode_block_header(state);
    } while (bit == 0);
    if ((state->end_of_stream & 0x40) != 0)
        tag_resource_cache_paging_decoder_flush_window(state);
    long discard = state->bit_count & 7;
    state->bit_count -= discard;
    state->bit_buffer >>= discard;
    if (state->bit_count < 0)
        tag_resource_cache_paging_decoder_refill_bits(state);
    int *&position_a = state->position_a;
    int bit_count = state->bit_count;
    int adjust = ((bit_count + 32) >> 3) - state->error_count;
    *state->position_a -= state->bytes_left;
    *position_a -= adjust;
    *state->position_b -= state->consumed_b;
    state->error = 1;
    void *jump_buffer = state->jump_buffer;
    if (jump_buffer != 0)
        SwitchToFiber(jump_buffer);
    abort();
}
