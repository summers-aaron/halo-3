// blamlibxbox/cache_file_tag_resource_runtime.obj: from the records 0x86410 0x86628 0x866c8 0x86740 0x86878 0x86918 0x86ac8 0x86b08 0x879a0 0x879d8 0x87a10 0x87a40 0x88570 0x88b10 0x88bd8 0x88d50 0x101958 0x1019e0 0x141860 0x141b80 0x15fe58 0x19a168 0x1acb38 0x1aefc8 0x1aefe0 0x1af008 0x1af018 0x1af0a0 0x1af0e0 0x1af1d0 0x1af210 0x1af2b8 0x1af2c8 0x1af3b0 0x1af400 0x1af5e0 0x1af798 0x1af880 0x1afa20 0x1afa30 0x1afa40 0x1afa48 0x1afa68 0x1afb50 0x1afda8 0x1afe48 0x1aff08 0x1b0230 0x1b03a0 0x1b0400 0x1b05d8 0x1b12e0 0x1b13d8 0x1b13f8 0x1b1658 0x1b1ac8 0x1b1b68 0x1b1bf8 0x1b1c80 0x1b1e38 0x1b1f80 0x1b21e0 0x1b2290 0x1b22f8 0x1b23d0 0x1b24b0 0x1b26e8 0x1b2790 0x1b27c8 0x1b2838 0x1b2ab0 0x1b2c60 0x1b2cc8 0x1b2ce8 0x1b2d00 0x1b2d40 0x1b2e68 0x1b2f18 0x1b2f40 0x1b2f50 0x1b2fc8 0x1b30c8 0x1b3270 0x1b3368 0x1b33c0 0x1b3428 0x1b3460 0x1b3498 0x1b3508 0x1b3828 0x1b3a50 0x1b3b40 0x1b3c18 0x1b3d08 0x1b3e10 0x1b3f00 0x1b3fd8 0x1b4130 0x1b4220 0x1b4298 0x1b4308 0x1b4360 0x1b43c8 0x1b44a0 0x1b4520 0x1b4750 0x1b48f0 0x1b4e00 0x1b5468 0x1b5670 0x1b5750 0x1b58c8 0x1b5a78 0x1b5c60 0x1b5d80 0x1b5ef8 0x1b6ef0 0x21a688 0x21abb8 0x21ae60 0x21ba88 0x21bb78 0x21bc58 0x21bcd0 0x21bfd8 0x21c168 0x21c278 0x21c388 0x21c490 0x21c4f8 0x21c6e8 0x21fe50 0x21ff58 0x21ffc8 0x220028 0x220088 0x220168 0x220218
// @flags /TP /O2 /Oc
#include "saved_games/game_state_xenon_externs.h"
#include "shell/utility_drive.h"
#include "memory/runtime_state_allocation.h"
#include "memory/physical_memory_map_types.h"
#include "cache/cache_file_tag_resource_sort.h"
#include "memory/tls_data_arrays.h"
#include <xtl.h>
#include "cseries/crt_xdk.h"
#include "cseries/pointer_utilities.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "memory/physical_memory_allocation.h"
#include "tag_files/tag_groups.h"
#include "multithreading/synchronization.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "cache/cache_file_tag_resource_runtime_io.h"
#include "cache/tag_resource_runtime_access.h"
#include "cache/tag_resource_external_access_state.h"

#include "cache/cache_file_runtime_globals.h"


#include "cache/cache_file_globals.h"
#include "tag_files/files.h"
#include "tag_files/files_windows.h"
#include "text/font_cache.h"
#include "main/levels.h"
#if !defined(_XBOX) || defined(__clang__)
#include "main/main.h"
#endif
#include "saved_games/game_state_procs.h"
#include "saved_games/game_state_procs_externs.h"
#include "saved_games/game_state_xenon.h"
#include "cache/cache_file_bulk_reader.h"
#include "cache/cache_file_tag_resource_bulk_reader.h"
#include "tag_files/tag_resource_cache_external_access.h"
#include "tag_files/tag_resource_cache_paging.h"
#include "scenario/scenario.h"
#include "cseries/async_helpers.h"
#include "cache/tag_resource_cache_thread_lock.h"
#include "memory/lruv_block_types.h"
#include "cache/cache_file_bulk_reader_slot_bits_types.h"
#include "memory/lruv_block_header_types.h"
#include "interface/gui_custom_bitmap_storage_hash_types.h"
#include "cache/cache_file_ancestor_list_types.h"
#include "cache/tag_resource_shortcut_cache_types.h"
#include "cache/async_io_file_types.h"
#include "tag_files/tag_resource_types.h"
#include "geometry/geometry_block_types.h"
#include "cache/synchronization_semaphore_types.h"
#include "cache/async_io_last_read_types.h"
#include "cache/cache_file_bulk_reader_set_bits_types.h"
#include "cache/critical_sections_types.h"
#include "multithreading/read_write_lock.h"
#include "memory/lruv_cache_types.h"
#include "cache/geometry_storage.h"
#include "physics/collision_bsp_cache_types.h"

// @address 0x74b898 g_geometry_globals
extern "C" {
s_geometry_globals g_geometry_globals = { 0, 0, 0, (c_cache_file_tag_resource_allocator *)&g_runtime_state_allocation };
}

#include "memory/physical_memory_map.h"
#include "cache/cache_file_resource_gestalt.h"
// @address 0x8a1ead g_utility_drive_mounted
// @address 0x1af0e0 ?write@s_cache_file_tag_resource_cursor@@UAA_NV?$c_basic_buffer@X@@PAV2@@Z
// @address 0x1b0230 ??0c_tag_resource_cache_new@@QAA@XZ
// @address 0x1b4e00 ??$_Unguarded_partition@PAUs_cache_file_tag_resource_relocation@@Us_cache_file_tag_resource_sort_key_predicate@@@std@@YA?AU?$pair@PAUs_cache_file_tag_resource_relocation@@PAU1@@0@PAUs_cache_file_tag_resource_relocation@@0Us_cache_file_tag_resource_sort_key_predicate@@@Z
// @address 0x1b5a78 ??$_Med3@PAUs_cache_file_tag_resource_relocation@@Us_cache_file_tag_resource_sort_key_predicate@@@std@@YAXPAUs_cache_file_tag_resource_relocation@@00Us_cache_file_tag_resource_sort_key_predicate@@@Z
// @address 0x1b48f0 ??$_Unguarded_partition@PAUs_cache_file_tag_resource_relocation@@Us_cache_file_tag_resource_sort_predicate@@@std@@YA?AU?$pair@PAUs_cache_file_tag_resource_relocation@@PAU1@@0@PAUs_cache_file_tag_resource_relocation@@0Us_cache_file_tag_resource_sort_predicate@@@Z
// @address 0x1b5750 ??$_Med3@PAUs_cache_file_tag_resource_relocation@@Us_cache_file_tag_resource_sort_predicate@@@std@@YAXPAUs_cache_file_tag_resource_relocation@@00Us_cache_file_tag_resource_sort_predicate@@@Z
// @address 0x21bb78 ?find_with_offset@s_cache_file_resource_fixed_array@@QAAEJKJPAEPAK@Z
// @address 0x1b2790 ?is_resource_required@c_cache_file_tag_resource_runtime_manager_storage@@UBA_NJJ@Z
// @address 0x86410 ?get_control_data@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAA?AV?$c_basic_buffer@X@@J_N@Z
// @address 0x15fe58 ?tag_resource_available@@YAPAXPBUs_tag_resource@@@Z
// @address 0x86ac8 ?get_interop_buffer@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAA?AV?$c_basic_buffer@X@@J_N@Z
// @address 0x1acb38 ?allocate@c_physical_memory_allocation@@UAAPAXIPBD@Z
// @address 0x1b4298 ?tag_resources_unlock_render_access@@YAPAUs_resource_access_cookie@@XZ
// @address 0x1b4308 ?scenario_active_zone_is_tag_available@@YA_NJ@Z
// @address 0x86740 ?get_control_fixups@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAA?AV?$c_wrapped_array@$$CBUs_tag_resource_fixup_location@@@@J@Z
// @address 0x86b08 ?get_interop_locations@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAA?AV?$c_wrapped_array@$$CBUs_tag_resource_interop_location@@@@J@Z
// @address 0x879a0 ?get_root_address_fixup@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAA?AVc_tag_resource_fixup@@J@Z
// @address 0x879d8 ?get_resource_definition@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAAJJ@Z
// @address 0x87a10 ?get_resource_owner@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAAJJ@Z
// @address 0x87a40 ?register_resource@c_cache_file_tag_resource_runtime_manager_storage@@UAA_NJJPAX@Z
// @address 0x1b26e8 ?unregister_resource@c_cache_file_tag_resource_runtime_manager_storage@@UAAXJJPAX@Z
// @address 0x21ba88 ?find_direct@s_cache_file_resource_fixed_array@@QAAEJKJPAEPAK@Z
// @address 0x21bc58 ?find_span@s_cache_file_resource_fixed_array@@QAA_NJKJPAK@Z
// @address 0x19a168 ?scenario_handle_tag_resource_lock@@YAXJ@Z
// @address 0x21c490 cache_file_resource_get_first_handle_datum
// @address 0x21c4f8 tag_resource_fixed_array_get_second_handle
// @address 0x1b3428 ?get_required_page_index@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAAJJ@Z
// @address 0x1b3460 ?get_optional_page_index@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAAJJ@Z
// @address 0x1b44a0 cache_file_tag_resource_cursor_new
// @address 0x1b4750 cache_file_tag_resource_sort_by_key
// @address 0x866c8 ?any_pageable_data@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAA_NJ@Z
// @address 0x86918 ?any_optional_data@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAA_NJ@Z
// @address 0x1aefe0 ?allocate@c_cache_file_tag_resource_runtime_listener_storage@@UAAPAXIPBD@Z
// @address 0x5bec98 ?deallocate@c_cache_file_tag_resource_runtime_listener_storage@@UAAXPAX@Z
// @address 0x1af2b8 ?create@s_cache_file_tag_resource_page_state@@UAAPAUs_cache_file_tag_resource_reader@@_KJV?$c_basic_buffer@X@@@Z
// @address 0x1af2c8 cache_file_tag_resource_open_reader
// @address 0x1b5468 cache_file_tag_resource_sort_insertion_by_key
// @address 0x1b4520 cache_file_tag_resource_sort_by_offset
// @address 0x1b5670 cache_file_tag_resource_sort_heap_by_key
// @address 0x21bfd8 cache_file_resource_get_datum_index
// @address 0x21c168 cache_file_resource_get_page_offset
// @address 0x21c278 cache_file_resource_get_element_by_index
// @address 0x21c388 cache_file_resource_get_element_by_datum
// @address 0x1b3c18 tag_resource_dispose_zone_data
#include "cseries/cseries.h"
#include <ppcintrinsics.h>
#include <string.h>
#include <stdlib.h>
#include <xutility>

struct s_data_array;

#include "cache/cache_file_tag_resource_vtable_list.h"

#include "memory/basic_buffer.h"
struct s_cache_file_resource_runtime_data_new
{
    c_basic_buffer<void> m_interop_buffer;
    c_basic_buffer<void> m_page_buffer;
    c_basic_buffer<void> m_optional_buffer;
};
struct s_resource_access_cookie;
#include "cache/cache_file_tag_resource_runtime_types.h"
#include "multithreading/synchronized_value.h"
struct s_tag_resource_definition;
class c_tag_resource_page_table;
class c_tag_resource_paging_page_filter;
 class c_tag_resource_controller_page_table_io_listener;
class c_tag_resource_address_cache_control_interface;
#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_lruv_cache_calls.h"
#include "cache/tag_resource_cache_new.h"
#include "cache/cache_file_tag_resource_runtime_manager_storage.h"

#include "multithreading/thread_tls.h"
#include "memory/bit_vectors.h"
#include "cache/cache_file_tag_resource_memory_stream.h"
struct s_cache_file_tag_resource_cursor_entry
{
    int start;
    int length;
    __forceinline int end() const { int result = length; result += start; return result; }
};
struct s_cache_file_tag_resource_cursor_source
{
    int unknown;
    int count;
    TAG_POINTER(s_cache_file_tag_resource_cursor_entry) entries;
};
struct s_cache_file_tag_resource_reader
{
    virtual bool set_buffer(c_basic_buffer<void> buffer) = 0;
};
struct s_cache_file_tag_resource_cursor : public s_cache_file_tag_resource_reader
{
    virtual bool set_buffer(c_basic_buffer<void> buffer) { m_buffer = buffer; return true; }
    virtual bool write(c_basic_buffer<void> data, c_basic_buffer<void> *out);
    virtual int is_at_end(c_basic_buffer<void> *out);
    const s_cache_file_tag_resource_cursor_source *source;
    c_basic_buffer<void> m_buffer;
    long m_position;
    long m_total_size;
    __forceinline s_cache_file_tag_resource_cursor(const s_cache_file_tag_resource_cursor_source *owner) : m_buffer(c_basic_buffer<void>::_uninitialized)
    {
        source = owner;
        memset(&m_buffer, 0, sizeof(m_buffer));
        m_position = 0;
        if (source->count == 0) {
            m_total_size = 0;
        } else {
            s_cache_file_tag_resource_cursor_entry *end = source->entries + source->count;
            m_total_size = end[-1].end();
        }
    }
};
#include "tag_files/tag_resource_internals_types.h"
#include "tag_files/tag_vertex_buffers.h"
struct s_cache_file_tag_interop_bulk_sizer
{
    __forceinline unsigned long get_next_offset_manual(unsigned long type_alignment_bits) const
    {
        return (size + (1 << type_alignment_bits) - 1) & (-1 << type_alignment_bits);
    }
    __forceinline void add_type_manually(unsigned long type_alignment_bits, unsigned long type_size)
    {
        if (alignment_bits <= type_alignment_bits)
            alignment_bits = type_alignment_bits;
        size = ((size + (1 << alignment_bits) - 1) & (-1 << alignment_bits)) + type_size;
    }
    __forceinline unsigned long get_size() const { return size; }
    __forceinline unsigned long get_unaligned_total_allocation_size() const
    {
        return size ? size + (1 << alignment_bits) - 1 : 0;
    }

    unsigned long alignment_bits;
    unsigned long size;
};
struct s_cache_file_tag_resource_page_reader
{
    virtual c_basic_buffer<void> get_buffer() const;
    virtual bool next_matching_page(long *out_handle, c_basic_buffer<void> *buffer, bool *out_pending);
    c_cache_file_tag_resource_runtime_manager_storage *m_manager;
    long m_index;
};
struct s_cache_file_tag_resource_file_header;
struct s_cache_file_runtime_definition
{
    short salt;
    unsigned short flags;
    void *file_handle;
    void *overlapped_handle;
    long io_file_index;
    s_cache_file_tag_resource_file_header *tag_data;
    long file_offset;
    long file_index;
};
struct tag_resource_external_access_entry8;
struct tag_resource_external_access_entry20;
#include "saved_games/game_state_xenon_types.h"
struct s_data_array;
struct s_cache_file_runtime_definition_iterator
{
    void *m_datum;
    s_data_iterator m_iterator;
    __forceinline bool next()
    {
        m_datum = data_iterator_next(&m_iterator);
        return m_datum != 0;
    }
};
struct s_cache_file_bit_vector_target
{
    long count0;
    unsigned long *bits0;
    long count1;
    unsigned long *bits1;
    unsigned char flag;
};
#include <new>
struct s_cache_file_tag_resource_cursor_source_slot
{
    s_cache_file_tag_resource_cursor_source source;
    long unknownc;
};
struct s_cache_file_tag_resource_sort_key_predicate
{
    char m_unused;
    bool operator()(const s_cache_file_tag_resource_relocation &a, const s_cache_file_tag_resource_relocation &b) const
    {
        return memcmp(&a.key, &b.key, sizeof(a.key)) < 0;
    }
};
#include "cache/cache_file_tag_resource_element_types.h"
#include "cache/cache_file_resource_elements.h"
#include "cache/cache_file_tag_resource_runtime_window_allocator_storage.h"
// 0x19a168 (scenario_handle_tag_resource_lock)
struct scenario;
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "structures/structure_runtime_resources.h"
#include "structures/structure_bsp_tag.h"
#include "cseries/xdk_memory.h"
// 0x1b0400
struct s_data_array;
class c_allocation_base;
#include "saved_games/game_state_procs_types.h"
class c_allocation_base;
struct s_cache_file_tag_resource_file_page
{
    unsigned long flags;                            // bit 0: keyed, bit 1: shared, size << 2
    unsigned long size;
    s_network_http_request_hash location;
};
struct s_cache_file_insertion_point_resource_usage
{
    signed char initial_zone_set_index;
    unsigned long shared_required_locations[32];
    unsigned long local_required_locations[10];
};
// The page table occupies 0x28..0x2328; availability reads the same file.
struct s_cache_file_tag_resource_file_header
{
    // game_state_xenon_try_and_get_data returns &s_cache_file_header::trailing_fields.
    long trailing_fields[4];
    unsigned short resource_count;
    unsigned short page_count;
    unsigned long offset;
    s_tag_persistent_identifier key;
    s_cache_file_tag_resource_file_page pages[320];
    unsigned char zone_count;
    s_cache_file_insertion_point_resource_usage insertion_point_usages[1];
};
// 0x1b1ac8: add a relocation for a datum index's element when its slot handle is present
struct s_cache_file_tag_resource_page_visitor_interface
{
    virtual void add_element_relocation(long datum_index, c_basic_buffer<void> buffer) = 0;
};
struct s_cache_file_tag_resource_page_visitor : public s_cache_file_tag_resource_page_visitor_interface
{
    virtual void add_element_relocation(long datum_index, c_basic_buffer<void> buffer);
    c_cache_file_tag_resource_runtime_manager_storage *m_manager;
};
// 0x1019e0: the definition and element words of a datum index (the owner is a view 20 bytes into the handle set)
// 0x1b1c80: load a play-list entry's map file: bind its page table, mark its element range and allocate its slot handle
struct s_cache_file_tag_resource_external_access_flush_view
{
    s_tag_resource_cache_external_access_state state;
    unsigned long last_flush_time;
};
// 0x1b2fc8: when the active handle is set and the flag is clear, ask the slot's source (or the
// default/fallback emptiness value) whether the resource still needs loading; if so, resolve the
// datum's element and definition and hand them to the 0x1af400 worker
class c_sound_cache_hash_source;
#include "multithreading/synchronized_value.h"
#include "cache/asynchronous_io_marker_types.h"
#include "cache/cache_file_bulk_reader_types.h"
#include "tag_files/file_reference.h"
#include "main/main_halt.h"
// 0x1b2f50: run one bulk read through the async io arena while the busy flag is held
class c_indirect_cache_file_location_atlas;
class c_indirect_cache_file_bulk_read_iterator;
class c_sound_cache_hash_source;
class c_sound_cache_bulk_reader;
#include "cache/indirect_cache_file_bulk_read_iterator.h"
#include "cache/cache_file_bulk_reader_state_types.h"
// 0x1b3270: mark a tag and everything it depends on in the visitor's bit vector
struct s_cache_file_tag_dependency_visitor
{
    long root_tag_index;
    const s_cache_file_resource_gestalt *graph;
    unsigned long *visited;
    long depth;
};
// 0x1b30c8: hand every resource of a tag (and of the tags it depends on) to a visitor. The override lives on a
// secondary base at +0x1c, so `this` arrives adjusted and get_gestalt() reads the manager through a copied this.
struct s_cache_file_tag_resource_graph_visitor
{
    virtual bool visit(void *context, long resource_handle) = 0;
};
#include "cache/tag_resource_page_datum.h"
// 0x1b05d8: c_cache_file_tag_resource_runtime_manager::initialize_for_new_map
class c_memcmp_equals
{
public:
    template <typename t_type>
    __forceinline bool operator()(const t_type &a, const t_type &b) const { return memcmp(&a, &b, sizeof(t_type)) == 0; }
};
template <typename t_element, typename t_value, typename t_compare>
class member_compare_val_t
{
public:
    __forceinline member_compare_val_t(t_value t_element::*member, const t_value &value) : m_value(value), m_member(member) {}
    __forceinline bool operator()(const t_element *element) const { return t_compare()(m_value, element->*m_member); }

    t_value m_value;
    t_value t_element::*m_member;
};

// the parts of <algorithm> std::sort uses here, as the XDK header spells them (<algorithm> includes <memory>, whose
// <exception> defines _bad_alloc_Message: its "bad allocation" literal stays in this object
// only from other objects at 0x82021290)
namespace std {
template<class _RanIt,
    class _Pr> inline
    void _Med3(_RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr _Pred)
    {	// sort median of three elements to middle
    if (_DEBUG_LT_PRED(_Pred, *_Mid, *_First))
        std::swap(*_Mid, *_First);
    if (_DEBUG_LT_PRED(_Pred, *_Last, *_Mid))
        std::swap(*_Last, *_Mid);
    if (_DEBUG_LT_PRED(_Pred, *_Mid, *_First))
        std::swap(*_Mid, *_First);
    }

template<class _RanIt,
    class _Pr> inline
    void _Median(_RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr _Pred)
    {	// sort median element to middle
    if (40 < _Last - _First)
        {	// median of nine
        size_t _Step = (_Last - _First + 1) / 8;
        _Med3(_First, _First + _Step, _First + 2 * _Step, _Pred);
        _Med3(_Mid - _Step, _Mid, _Mid + _Step, _Pred);
        _Med3(_Last - 2 * _Step, _Last - _Step, _Last, _Pred);
        _Med3(_First + _Step, _Mid, _Last - _Step, _Pred);
        }
    else
        _Med3(_First, _Mid, _Last, _Pred);
    }

template<class _RanIt,
    class _Pr> inline
    pair<_RanIt, _RanIt> _Unguarded_partition(_RanIt _First, _RanIt _Last,
        _Pr _Pred)
    {	// partition [_First, _Last), using _Pred
    _RanIt _Mid = _First + (_Last - _First) / 2;
    _Median(_First, _Mid, _Last - 1, _Pred);
    _RanIt _Pfirst = _Mid;
    _RanIt _Plast = _Pfirst + 1;

    while (_First < _Pfirst
        && !_DEBUG_LT_PRED(_Pred, *(_Pfirst - 1), *_Pfirst)
        && !_Pred(*_Pfirst, *(_Pfirst - 1)))
        --_Pfirst;
    while (_Plast < _Last
        && !_DEBUG_LT_PRED(_Pred, *_Plast, *_Pfirst)
        && !_Pred(*_Pfirst, *_Plast))
        ++_Plast;

    _RanIt _Gfirst = _Plast;
    _RanIt _Glast = _Pfirst;

    for (; ; )
        {	// partition
        for (; _Gfirst < _Last; ++_Gfirst)
            {
            if (_DEBUG_LT_PRED(_Pred, *_Pfirst, *_Gfirst))
                continue;
            if (_Pred(*_Gfirst, *_Pfirst))
                break;
            std::swap(*_Plast++, *_Gfirst);
            }
        for (; _First < _Glast; --_Glast)
            {
            if (_DEBUG_LT_PRED(_Pred, *(_Glast - 1), *_Pfirst))
                continue;
            if (_Pred(*_Pfirst, *(_Glast - 1)))
                break;
            std::swap(*--_Pfirst, *(_Glast - 1));
            }
        if (_Glast == _First && _Gfirst == _Last)
            return (pair<_RanIt, _RanIt>(_Pfirst, _Plast));

        if (_Glast == _First)
            {	// no room at bottom, rotate pivot upward
            if (_Plast != _Gfirst)
                std::swap(*_Pfirst, *_Plast);
            ++_Plast;
            std::swap(*_Pfirst++, *_Gfirst++);
            }
        else if (_Gfirst == _Last)
            {	// no room at top, rotate pivot downward
            if (--_Glast != --_Pfirst)
                std::swap(*_Glast, *_Pfirst);
            std::swap(*_Pfirst, *--_Plast);
            }
        else
            std::swap(*_Gfirst++, *--_Glast);
        }
    }

        // TEMPLATE FUNCTION find_if
template<class _InIt,
    class _Pr> inline
    _InIt _Find_if(_InIt _First, _InIt _Last, _Pr _Pred)
    {	// find first satisfying _Pred
    _DEBUG_RANGE(_First, _Last);
    _DEBUG_POINTER(_Pred);
    for (; _First != _Last; ++_First)
        if (_Pred(*_First))
            break;
    return (_First);
    }

template<class _InIt,
    class _Pr> inline
    _InIt find_if(_InIt _First, _InIt _Last, _Pr _Pred)
    {	// find first satisfying _Pred
    _ASSIGN_FROM_BASE(_First,
        _Find_if(_CHECKED_BASE(_First), _CHECKED_BASE(_Last), _Pred));
    return (_First);
    }

}


STATIC_ASSERT_ABI32(offsetof(c_tag_resource_cache_new, m_resource_address_cache) == 0xd0, runtime_cache_address_offset);
STATIC_ASSERT_ABI32(offsetof(c_tag_resource_cache_new, m_resize_flags) == 0x310, runtime_cache_flags_offset);
STATIC_ASSERT_ABI32(sizeof(c_tag_resource_cache_new) == 0x320, runtime_cache_size);
__forceinline const s_cache_file_tag_resource_data *
c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_entry(long resource_handle) const
{
    const s_cache_file_resource_gestalt *gestalt = (const s_cache_file_resource_gestalt *)m_header->instances[(short)m_resource_gestalt_index].definition;
    return &((const s_cache_file_tag_resource_data *)gestalt->resources.address)[DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle)];
}

__forceinline void c_cache_file_tag_resource_runtime_manager_storage::bind_file_slots()
{
    c_cache_file_tag_resource_window_allocator *allocator = this;
    m_file_slots.loader = allocator;
}

inline c_tag_resource_cache_new::c_tag_resource_cache_new()
{
}
static __forceinline void page_storage_first_allocation_reset(s_cache_file_tag_resource_cursor_storage *storage)
{
    if (storage->object != 0)
        storage->object = 0;
}
static __forceinline void page_storage_second_allocation_reset(s_cache_file_tag_resource_stream_storage *storage)
{
    if (storage->object != 0)
        storage->object = 0;
}
static __forceinline void page_storage_view_reset(s_cache_file_tag_resource_page_state *page_storage)
{
    page_storage_second_allocation_reset(&page_storage->stream);
    page_storage_first_allocation_reset(&page_storage->cursor);
}

namespace {
static __forceinline void scenario_handle_tag_resource_unlock(long thread_index)
{
    memset(&g_collision_bsp_cache.thread_users[thread_index], 0, sizeof(s_collision_bsp_cache_user));
}
static __forceinline void tag_resource_shortcuts_handle_unlock()
{
    long thread_index = get_current_thread_index();
    scenario_handle_tag_resource_unlock(thread_index);
}
}
__forceinline s_resource_access_cookie *c_cache_file_tag_resource_runtime_manager::unlock_for_render_access()
{
    s_resource_access_cookie *result = 0;
    if (m_cache_file_resource_gestalt_index != -1)
        result = m_resource_cache.m_render_access.unlock_access_for_current_thread_internal(true);
    return result;
}

c_basic_buffer<void> c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_control_data(long resource_handle, bool page)
{
    const s_cache_file_tag_resource_data *entry = get_entry(resource_handle);
    const c_basic_buffer<void> *buffer;
    // match: 0x86410: only the selected buffer path is initialized.
    c_basic_buffer<void> data(c_basic_buffer<void>::_uninitialized);
    if (page)
    {
        buffer = &m_data[DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle)].m_page_buffer;
    }
    else
    {
        const s_cache_file_resource_gestalt *gestalt = (const s_cache_file_resource_gestalt *)m_header->instances[(short)m_resource_gestalt_index].definition;
        data.m_address = (char *)gestalt->fixup_information.address + entry->data_offset;
        data.m_size = entry->data_size;
        buffer = &data;
    }
    return *buffer;
}

// @address 0x86628 ?get_pageable_offset@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAAKJ@Z
unsigned long c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_pageable_offset(long resource_handle)
{
    if (any_pageable_data(resource_handle))
    {
        const s_cache_file_resource_gestalt *gestalt = (const s_cache_file_resource_gestalt *)m_header->instances[(short)m_resource_gestalt_index].definition;
        const s_cache_file_tag_resource_data *entry =
            &((const s_cache_file_tag_resource_data *)gestalt->resources.address)[DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle)];
        return m_element_set->first->handles[entry->fixup_index].first_fixup_value;
    }
    return 0;
}

c_wrapped_array<const s_tag_resource_fixup_location>
c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_control_fixups(long resource_handle)
{
    const s_cache_file_tag_resource_data *entry = get_entry(resource_handle);
    return c_wrapped_array<const s_tag_resource_fixup_location>(
        static_cast<const s_tag_resource_fixup_location *>(entry->fixups.elements), entry->fixups.count);
}

// @address 0x86878 ?get_optional_offset@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAAKJ@Z
unsigned long c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_optional_offset(long resource_handle)
{
    if (any_optional_data(resource_handle))
    {
        const s_cache_file_resource_gestalt *gestalt = (const s_cache_file_resource_gestalt *)m_header->instances[(short)m_resource_gestalt_index].definition;
        const s_cache_file_tag_resource_data *entry =
            &((const s_cache_file_tag_resource_data *)gestalt->resources.address)[DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle)];
        return m_element_set->first->handles[entry->fixup_index].second_fixup_value;
    }
    return 0;
}

c_basic_buffer<void> c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_interop_buffer(long resource_handle, bool optional)
{
    c_basic_buffer<void> *buffer;
    if (optional)
        buffer = &m_data[DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle)].m_optional_buffer;
    else
        buffer = &m_data[DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle)].m_interop_buffer;
    return *buffer;
}

c_wrapped_array<const s_tag_resource_interop_location>
c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_interop_locations(long resource_handle)
{
    const s_cache_file_tag_resource_data *entry = get_entry(resource_handle);
    return c_wrapped_array<const s_tag_resource_interop_location>(entry->interop_locations.elements, entry->interop_locations.count);
}

c_tag_resource_fixup c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_root_address_fixup(long resource_handle)
{
    c_tag_resource_fixup fixup;
    fixup.value = get_entry(resource_handle)->location.value;
    return fixup;
}

long c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_resource_definition(long resource_handle)
{
    return get_entry(resource_handle)->page_index;
}

long c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_resource_owner(long resource_handle)
{
    return get_entry(resource_handle)->owner_tag.index;
}

bool c_cache_file_tag_resource_runtime_manager_storage::register_resource(long tag_index, long resource_type, void *resource)
{
    bool result = false;
    if (tag_try_and_get_unsafe(tag_get_group_tag(tag_index), tag_index)) {
        if (m_resource_vtables[resource_type] && m_resource_vtables[resource_type]->register_resource)
            result = m_resource_vtables[resource_type]->register_resource(tag_index, resource);
        else
            result = true;
    }
    return result;
}

void *c_physical_memory_allocation::allocate(unsigned int size, const char *name)
{
    return ::_physical_memory_malloc((memory_stage)m_stage, (char const *)(NULL), size, 1, (char const *)(NULL), -1);
}

void c_cache_file_tag_resource_runtime_manager_storage::unregister_resource(long tag_index, long resource_type, void *resource)
{
    if (m_resource_vtables[resource_type] && m_resource_vtables[resource_type]->unregister_resource &&
        tag_try_and_get_unsafe(tag_get_group_tag(tag_index), tag_index))
        m_resource_vtables[resource_type]->unregister_resource(tag_index, resource);
}

bool c_cache_file_tag_resource_runtime_manager_storage::is_resource_required(long tag_index, long resource_handle) const
{
    return bit_vector_test(m_required_resource_flags, DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle));
}

bool c_cache_file_tag_resource_runtime_manager_storage::is_resource_unneeded(long tag_index, long resource_handle) const
{
    return !bit_vector_test(m_required_resource_flags, DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle))
        && bit_vector_test(m_active_resource_flags, DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle));
}

// @address 0x1aefc8 ?allocate@c_tag_resource_page_range_allocator@@UAAXPAV?$c_basic_buffer@X@@K@Z
void c_tag_resource_page_range_allocator::allocate(c_basic_buffer<void> *buffer, unsigned long size)
{
    resize(buffer, size, size, 0);
}

// >>> 0x1aefe0
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_listener_storage, m_first) == 4, runtime_listener_first_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_listener_storage, m_second) == 0xc, runtime_listener_second_offset);
STATIC_ASSERT_ABI32(sizeof(c_cache_file_tag_resource_runtime_listener_storage) == 0x14, runtime_listener_size);
void *c_cache_file_tag_resource_runtime_listener_storage::allocate(unsigned int size, const char *name)
{
    unsigned long aligned_size = (size + 3) & ~3;
    void *const &top = m_second.m_address;
    m_second.m_address = (char *)m_second.m_address - aligned_size;
    m_second.m_size += aligned_size;
    return top;
}
// <<< 0x1aefe0

// @address 0x1af008 ?set_buffer@s_cache_file_tag_resource_memory_stream@@UAA_NV?$c_basic_buffer@X@@@Z
bool s_cache_file_tag_resource_memory_stream::set_buffer(c_basic_buffer<void> buffer)
{
    m_buffer = buffer;
    return true;
}

// @address 0x1af018 ?write@s_cache_file_tag_resource_memory_stream@@UAAHV?$c_basic_buffer@X@@PAV2@@Z
int s_cache_file_tag_resource_memory_stream::write(c_basic_buffer<void> data, c_basic_buffer<void> *written)
{
    // match: 0x1af018: reading the size through a reference schedules its lwz from the homed argument before mr r28,r5
    const c_basic_buffer<void> &source = data;
    unsigned long size = source.m_size;
    int result = false;
    // match: 0x1af018: a named stream pointer orders the final position add as add r11,size,position
    s_cache_file_tag_resource_memory_stream *stream = this;
    int &position = stream->m_position;
    if (position + size <= m_buffer.m_size) {
        c_basic_buffer<void> buffer = m_buffer;
        void *destination = buffer.get_range(position, size);
        memcpy(destination, source.m_address, size);
        if (written)
            *written = c_basic_buffer<void>(destination, size);
        result = true;
        position += size;
    }
    return result;
}

// @address 0x1af0a0 ?is_at_end@s_cache_file_tag_resource_memory_stream@@UAAHPAV?$c_basic_buffer@X@@@Z
int s_cache_file_tag_resource_memory_stream::is_at_end(c_basic_buffer<void> *out)
{
    if (out && (unsigned long)m_position == m_buffer.m_size)
    {
        out->m_address = NULL;
        out->m_size = 0;
    }
    return m_position == m_buffer.m_size;
}

STATIC_ASSERT_ABI32(sizeof(s_cache_file_tag_resource_cursor) == 0x18, runtime_cursor_size);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_cursor, m_buffer) == 8, runtime_cursor_buffer_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_cursor, m_position) == 0x10, runtime_cursor_position_offset);

// @address 0x1af1d0 ?is_at_end@s_cache_file_tag_resource_cursor@@UAAHPAV?$c_basic_buffer@X@@@Z
int s_cache_file_tag_resource_cursor::is_at_end(c_basic_buffer<void> *out)
{

    if (out != NULL && (unsigned long)m_position >= (unsigned long)m_total_size)
    {
        out->m_address = NULL;
        out->m_size = 0;
    }
    return (unsigned long)m_position >= (unsigned long)m_total_size;
}


// @address 0x1af210 ?initialize@s_cache_file_tag_resource_page_state@@QAAXPAVc_allocation_base@@V?$c_wrapped_array@$$CBUs_tag_persistent_identifier@@@@PAUs_cache_file_tag_resource_codec_resolver@@V?$c_wrapped_array@$$CBUs_cache_file_tag_resource_cursor_source_slot@@@@@Z
void s_cache_file_tag_resource_page_state::initialize(c_allocation_base *allocator,
    c_wrapped_array<const s_tag_persistent_identifier> block, s_cache_file_tag_resource_codec_resolver *resolver,
    c_wrapped_array<const s_cache_file_tag_resource_cursor_source_slot> source)
{
    cursor_sources = source;
    source_count = block.count();
    for (int index = 0; index < block.count(); ++index) {
        sources[index] = static_cast<s_cache_file_tag_resource_slot_source *>(resolver->resolve(&block[index]));
    }
    c_basic_buffer<void> buffer(c_basic_buffer<void>::_uninitialized);
    buffer.m_address = allocator->allocate(0x20000, "decompression buffer");
    buffer.m_size = 0x20000;
    this->buffer = buffer;
}

// @address 0x1af3b0 ?release@s_cache_file_tag_resource_page_state@@UAAX_KJPAVc_sound_cache_hash_source@@@Z
void s_cache_file_tag_resource_page_state::release(unsigned long long position, long index, c_sound_cache_hash_source *source)
{
    if (index == -1)
    {
        stream.object = 0;
        return;
    }
    if ((index & (long)0x80000000) == 0)
    {
        sources[index]->handle(source);
        return;
    }
    cursor.object = 0;
}

// @address 0x1afa20 ?set_mode@s_cache_file_tag_resource_slots@@UAAXJPAUs_game_state_buffer_description@@@Z
void s_cache_file_tag_resource_slots::set_mode(long mode, s_game_state_buffer_description *descriptions)
{
    m_reclaim_expired = true;
}

// @address 0x1afa30 ?reset@s_cache_file_tag_resource_slots@@UAAXXZ
void s_cache_file_tag_resource_slots::reset()
{
    m_reclaim_expired = false;
}

// @address 0x1afa40 ?game_state_proc_handler_allocate@s_cache_file_tag_resource_slots@@UAAPAXJJJPAUc_game_state_proc_listener@@@Z
void *s_cache_file_tag_resource_slots::game_state_proc_handler_allocate(long slot_index, long load_mode, long token, c_game_state_proc_listener *listener)
{
    return try_and_get(slot_index, load_mode, token, listener);
}

// @address 0x1afa48 ?game_state_proc_handler_release@s_cache_file_tag_resource_slots@@UAAXJPAX@Z
void s_cache_file_tag_resource_slots::game_state_proc_handler_release(long slot_index, void *resource)
{
    request_data.requests[slot_index].owner = NULL;
    request_data.requests[slot_index].external = false;
    tag_resource_cache_free_block(this, slot_index, resource);
}

static __forceinline void cache_file_tag_resource_runtime_prepare(s_cache_file_tag_resource_requests *data, int slot_index, int token)
{
    s_cache_file_tag_resource_request *request = &data->requests[slot_index];
    if (request->external == 0 || request->resource == NULL)
    {
        if (request->valid == 0)
            request->valid = 1;
        request->timestamp = data->current_timestamp;
        request->token = token;
    }
}
static __forceinline void cache_file_tag_resource_runtime_bit_vector_clear(unsigned int *vector, int bit)
{
    vector[bit >> 5] &= ~(1 << (bit & 31));
}
// @address 0x1afa68 ?try_and_get@s_cache_file_tag_resource_slots@@QAAPAXHHHPAUc_game_state_proc_listener@@@Z
void *s_cache_file_tag_resource_slots::try_and_get(int slot_index, int load_mode, int token, c_game_state_proc_listener *listener)
{
    void *result = NULL;
    cache_file_tag_resource_runtime_prepare(&request_data, slot_index, token);
    if (request_data.requests[(__int64)slot_index].resource == NULL)
    {
        if (load_mode == 2)
            try_and_load(slot_index);
        else
            cache_file_tag_resource_requests_try_and_load(this, slot_index);
    }
    if (request_data.requests[(__int64)slot_index].resource != NULL)
    {
        cache_file_tag_resource_runtime_bit_vector_clear(bitmap, slot_index);
        request_data.requests[slot_index].listener = listener;
        request_data.requests[slot_index].external = 1;
        request_data.requests[slot_index].valid = 0;
        result = request_data.requests[(__int64)slot_index].resource;
    }
    return result;
}

// @address 0x1afb50 ?try_and_load@s_cache_file_tag_resource_slots@@QAAXH@Z
void s_cache_file_tag_resource_slots::try_and_load(int slot_index)
{
    if (request_data.requests[slot_index].resource == NULL && request_data.requests[slot_index].valid != 0)
    {
        int token = request_data.requests[slot_index].token;
        bool all_empty = true;
        for (int next = slot_index + 1; next < 6; next++)
        {
            if (request_data.requests[next].resource != NULL || (unsigned char)request_data.requests[next].valid != 0)
            {
                all_empty = false;
                break;
            }
        }
        if (all_empty)
        {
            void *resource = loader->allocate(token);
            if (resource != NULL)
                request_data.requests[slot_index].resource = resource;
        }
    }
}

// @address 0x1afda8 ?next_tracked_resource_handle@c_cache_file_tag_resource_setup_iterator@@UAA_NPAJ@Z
bool c_cache_file_tag_resource_setup_iterator::next_tracked_resource_handle(long *out)
{
    bool found = false;
    do
    {
        if (m_index >= (unsigned long)m_owner->resources.count)
            return found;
        const s_cache_file_tag_resource_data *entry = &((const s_cache_file_tag_resource_data *)m_owner->resources.address)[m_index];
        if (entry->salt != -1 && entry->page_index != -1
            && (m_all != 0 || (((const s_cache_file_tag_resource_type_definition *)m_owner->resource_types.address)[entry->page_index].flags & 2) == 0))
        {
            found = true;
            *out = (entry->salt << 16) | m_index;
        }
        m_index = m_index + 1;
    } while (!found);
    return found;
}

// @address 0x333b0 g_d3d_texture_interop_vtable
// @rdata 0x820333b0..0x820333e0
static const s_tag_resource_interop_type g_d3d_texture_interop_vtable = {
    { 0x64017671, 0x437a46f0, 0x9a33c422, 0x517096e3 },
    { 0x9568e346, 0xb6c04fbc, 0xbffd7092, 0xfa348987 },
    0x34, 2, tag_d3d_texture_fixup, tag_d3d_texture_teardown
};
// @rdata-end

// @address 0x333e0 g_d3d_texture_interleaved_interop_vtable
// @rdata 0x820333e0..0x82033410
static const s_tag_resource_interop_type g_d3d_texture_interleaved_interop_vtable = {
    { 0x90486d0a, 0x944a4941, 0x9fe79af0, 0x39e2e611 },
    { 0x90486d0a, 0x944a4941, 0x9fe79af0, 0x39e2e611 },
    0x68, 2, tag_d3d_texture_interleaved_fixup, tag_d3d_texture_interleaved_teardown
};
// @rdata-end

// @address 0x33410 g_d3d_vertex_buffer_interop_vtable
// @rdata 0x82033410..0x82033440
static const s_tag_resource_interop_type g_d3d_vertex_buffer_interop_vtable = {
    { 0x12721193, 0x798f4648, 0xb08e254d, 0xfb5907df },
    { 0x12721193, 0x798f4648, 0xb08e254d, 0xfb5907df },
    0x28, 2, tag_d3d_vertex_buffer_fixup, tag_d3d_vertex_buffer_teardown
};
// @rdata-end

// @address 0x33440 g_d3d_index_buffer_interop_vtable
// @rdata 0x82033440..0x82033470
static const s_tag_resource_interop_type g_d3d_index_buffer_interop_vtable = {
    { 0x08380d7d, 0xf14d4112, 0x82ce3471, 0x1e43a770 },
    { 0x08380d7d, 0xf14d4112, 0x82ce3471, 0x1e43a770 },
    0x28, 2, tag_d3d_index_buffer_fixup, tag_d3d_index_buffer_teardown
};
// @rdata-end

// @address 0x746030 k_tag_resource_interop_types
extern "C" {
const s_tag_resource_interop_type *k_tag_resource_interop_types[4] = {
    &g_d3d_vertex_buffer_interop_vtable,
    &g_d3d_index_buffer_interop_vtable,
    &g_d3d_texture_interop_vtable,
    &g_d3d_texture_interleaved_interop_vtable
};
}

// @address 0x1b12e0 cache_file_tag_resource_accumulate_interop_types
extern "C" bool cache_file_tag_resource_accumulate_interop_types(const s_cache_file_tag_resource_data *resource_data,
    s_cache_file_tag_interop_bulk_sizer *sizer,
    unsigned long *offset, unsigned long *size)
{
    bool result = false;
    if (resource_data->interop_locations.count > 0)
    {
        unsigned long first_alignment_bits = k_tag_resource_interop_types[resource_data->interop_locations.elements[0].interop_type_index]->alignment_bits;
        unsigned long start = (sizer->size + (1 << first_alignment_bits) - 1) & (-1 << first_alignment_bits);
        for (long interop_index = 0; interop_index < resource_data->interop_locations.count; interop_index++)
        {
            const s_tag_resource_interop_type *type = k_tag_resource_interop_types[resource_data->interop_locations.elements[interop_index].interop_type_index];
            unsigned long type_alignment_bits = type->alignment_bits;
            unsigned long type_size = type->size;
            unsigned long alignment_bits = sizer->alignment_bits > type_alignment_bits ? sizer->alignment_bits : type_alignment_bits;
            sizer->alignment_bits = alignment_bits;
            sizer->size = ((sizer->size + (1 << alignment_bits) - 1) & (-1 << alignment_bits)) + type_size;
        }
        unsigned long total_size = sizer->size - start;
        if (offset)
            *offset = start;
        if (size)
            *size = total_size;
        result = true;
    }
    return result;
}

static __forceinline long cache_file_tag_resource_find_tag(const tag_header *definition, unsigned long tag)
{
    long result = -1;
    for (long i = 0; i < definition->count; i++) {
        if (definition->tags[i].group == tag)
            result = definition->tags[i].index;
    }
    return result;
}

// @address 0x1b1b68 cache_file_tag_resource_get_play_list_strings
extern "C" c_typed_tag_block<s_cache_file_resource_shared_file> *cache_file_tag_resource_get_play_list_strings(
    c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    s_cache_file_resource_gestalt *default_block =
        (s_cache_file_resource_gestalt *)manager->m_header->instances[(short)manager->m_cache_file_resource_gestalt_index].definition;
    long found = cache_file_tag_resource_find_tag(manager->m_header, 'play');
    if (found != -1) {
        s_cache_file_resource_layout_table_definition *block
            = (s_cache_file_resource_layout_table_definition *)manager->m_header->instances[(short)found].definition;
        if (block != 0)
            return &block->layout_table.shared_files;
    }
    return &default_block->layout_table.shared_files;
}

// @address 0x1b1bf8 cache_file_tag_resource_get_play_list_keys
extern "C" c_typed_tag_block<s_tag_persistent_identifier> *cache_file_tag_resource_get_play_list_keys(
    c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    s_cache_file_resource_gestalt *default_block =
        (s_cache_file_resource_gestalt *)manager->m_header->instances[(short)manager->m_cache_file_resource_gestalt_index].definition;
    long found = cache_file_tag_resource_find_tag(manager->m_header, 'play');
    if (found != -1) {
        s_cache_file_resource_layout_table_definition *block
            = (s_cache_file_resource_layout_table_definition *)manager->m_header->instances[(short)found].definition;
        if (block != 0)
            return &block->layout_table.codecs;
    }
    return &default_block->layout_table.codecs;
}

#pragma pack(push, 4)
struct s_cache_file_runtime_handle_set;
static __forceinline s_cache_file_runtime_handle_stack &cache_file_tag_resource_handles(
    c_cache_file_tag_resource_runtime_manager_storage *manager);
static __forceinline s_data_array *&cache_file_tag_resource_definitions(c_cache_file_tag_resource_runtime_manager_storage *manager);
static __forceinline long &cache_file_tag_resource_active_handle(c_cache_file_tag_resource_runtime_manager_storage *manager);
static __forceinline bool &cache_file_tag_resource_pending_map(c_cache_file_tag_resource_runtime_manager_storage *manager);
static __forceinline c_tag_resource_cache_new &cache_file_tag_resource_cache(c_cache_file_tag_resource_runtime_manager_storage *manager);

#pragma pack(pop)

static __forceinline int cache_file_tag_resource_runtime_slot(int index)
{
    int slot;
    if (index == -1)
        slot = 0;
    else if (index == -2)
        slot = 4;
    else
        slot = index + 1;
    return slot;
}
static __forceinline int cache_file_tag_resource_runtime_handle(c_cache_file_tag_resource_runtime_manager_storage *manager, int index)
{
    int slot = cache_file_tag_resource_runtime_slot(index);
    if (slot == 4)
        return manager->m_zone_names.m_zone_set_index;
    return cache_file_tag_resource_handles(manager)[slot];
}

static __forceinline void cache_file_tag_resource_definition_set_pending(long *file_index, unsigned short *flags, long value)
{
    *file_index = value;
    *flags |= 1;
}

// @address 0x1b1e38 cache_file_tag_resource_definition_initialize
void cache_file_tag_resource_definition_initialize(
    s_cache_file_runtime_handle_set *set, long index, const char *path, const s_cache_file_resource_shared_file *shared_file, bool pending)
{
    // The handle set is the manager's storage; the handle-set spelling stays because
    // include/cache/cache_file_tag_resource_runtime.h owns this declaration.
    c_cache_file_tag_resource_runtime_manager_storage *manager = (c_cache_file_tag_resource_runtime_manager_storage *)set;
    s_cache_file_runtime_definition *definition
        = datum_get<s_cache_file_runtime_definition>(cache_file_tag_resource_definitions(manager), index);
    if (pending)
    {
        long file_index = game_state_xenon_find_file_by_path(path);
        xenon_game_state_globals.pending_file_index = file_index;
        cache_file_tag_resource_definition_set_pending(&definition->file_index, &definition->flags, file_index);
    }
    {
        long file_index = game_state_xenon_find_file_by_path(path);
        if (game_state_xenon_is_file_valid(file_index))
            definition->io_file_index = xenon_game_state_globals.files[file_index].io_file_index;
    }
    {
        long file_index = game_state_xenon_find_file_by_path(path);
        if (game_state_xenon_is_file_valid(file_index))
            definition->file_handle = xenon_game_state_globals.files[file_index].file_handle;
    }
    {
        long file_index = game_state_xenon_find_file_by_path(path);
        if (game_state_xenon_is_file_valid(file_index))
            definition->overlapped_handle = xenon_game_state_globals.files[file_index].overlapped_handle;
    }
    if (shared_file != NULL && (shared_file->flags & 1) == 0)
        definition->file_offset = shared_file->extra;
    else
    {
        long file_index = game_state_xenon_find_file_by_path(path);
        if (game_state_xenon_is_file_valid(file_index))
            definition->file_offset = xenon_game_state_globals.files[file_index].header.map_file_offsets[1];
    }
    definition->tag_data = (s_cache_file_tag_resource_file_header *)game_state_xenon_try_and_get_data(path);
}

// @address 0x1b2290 cache_file_tag_resource_find_paged_entry
extern "C" int cache_file_tag_resource_find_paged_entry(c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    int result = -1;
    c_typed_tag_block<s_cache_file_resource_shared_file> *block = cache_file_tag_resource_get_play_list_strings(manager);
    int index = 0;
    if (block->count() > 0) {
        do {
            unsigned char flag = ((*block)[index].flags >> 2) & 1;
            if (flag)
                result = index;
            index++;
        } while (index < block->count());
    }
    return result;
}

// @address 0x1b22f8 cache_file_tag_resource_find_handle_by_path
extern "C" int cache_file_tag_resource_find_handle_by_path(c_cache_file_tag_resource_runtime_manager_storage *manager, const char *path)
{
    int result = -1;
    char name[264];
    name[0] = 0;
    cache_files_windows_get_canonical_map_path(path, name);
    c_typed_tag_block<s_cache_file_resource_shared_file> *block = cache_file_tag_resource_get_play_list_strings(manager);
    int index = 0;
    if (block->count() > 0) {
        int offset = 0;
        do {
            char entry_name[256];
            entry_name[0] = 0;
            cache_files_windows_get_canonical_map_path((*block)[index].name, entry_name);
            if (::csstricmp((char const *)(name), (char const *)(entry_name)) == 0) {
                result = cache_file_tag_resource_runtime_handle(manager, index);
                break;
            }
            ++index;
            offset += 264;
        } while (index < block->count());
    }
    return result;
}

// @address 0x1b27c8 ?is_resource_unneeded@c_cache_file_tag_resource_runtime_manager_storage@@UBA_NJJ@Z

static __forceinline c_basic_buffer<void> cache_file_tag_resource_buffer_get_range(c_basic_buffer<void> buffer, unsigned long offset, unsigned long size)
{
    return c_basic_buffer<void>(buffer, offset, size);
}

static __forceinline void cache_file_tag_resource_slots_release(s_cache_file_tag_resource_slots *slots, long slot_index)
{
    s_cache_file_tag_resource_request *slot = &slots->request_data.requests[slot_index];
    if (slot->external)
        slot->owner->terminate(slot->resource);
    else
        tag_resource_cache_free_block(slots, slot_index, slot->resource);
}

static __forceinline void cache_file_tag_resource_slots_release_last(s_cache_file_tag_resource_slots *slots)
{
    for (long slot_index = 5; slot_index >= 0; slot_index--)
    {
        if (slots->request_data.requests[slot_index].resource)
        {
            cache_file_tag_resource_slots_release(slots, slot_index);
            break;
        }
    }
}

// @address 0x1b2e68 ?allocate@c_cache_file_tag_resource_runtime_manager_storage@@UAAPAXK@Z
void * c_cache_file_tag_resource_runtime_manager_storage::allocate(unsigned long size)
{
    void *result = allocate_lruv(size);
    if (!result) {
        unsigned long available = m_result_window.available(m_window);
        if (available < size) {
            unsigned long needed = size - available;
            tag_resource_cache_new_handle_space_demand(&m_resource_cache.m_cache, needed > size ? size : needed);
        }
        result = allocate_lruv(size);
    }
    return result;
}

// @address 0x1b2f18 ?advance_window@c_cache_file_tag_resource_runtime_manager_storage@@UAAXHJ@Z
void c_cache_file_tag_resource_runtime_manager_storage::advance_window(int unused, long size)
{
    c_basic_buffer<void> &window = m_window;
    window.m_address = static_cast<char *>(window.m_address) + size;
    window.m_size -= size;
}

// @address 0x1b3368 cache_file_tag_resource_get_slot_definition
extern "C" s_cache_file_runtime_definition *cache_file_tag_resource_get_slot_definition(
    c_cache_file_tag_resource_runtime_manager_storage *manager, long slot)
{
    unsigned long handle;
    if (slot == -1) {
        slot = 0;
    } else if (slot == -2 || (slot = slot + 1) == 4) {
        slot = 4;
    }
    if (slot == 4)
        handle = manager->m_zone_names.m_zone_set_index;
    else
        handle = cache_file_tag_resource_handles(manager)[slot];
    return datum_get<s_cache_file_runtime_definition>(cache_file_tag_resource_definitions(manager), handle);
}

// @address 0x1b23d0 cache_file_tag_resource_reset_handle_set
extern "C" void cache_file_tag_resource_reset_handle_set(c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    s_data_array **definitions = &cache_file_tag_resource_definitions(manager);
    s_cache_file_runtime_definition_iterator iterator;
    iterator.m_iterator.absolute_index = -1;
    iterator.m_iterator.index = -1;
    iterator.m_iterator.data = (s_data_array *)*definitions;
    while (iterator.next()) {
        if (datum_get<s_cache_file_runtime_definition>(*definitions, iterator.m_iterator.index)->flags & 1)
            xenon_game_state_globals.pending_file_index = -1;
        ::datum_delete(*definitions, iterator.m_iterator.index);
    }
    s_data_array *array = *definitions;
    cache_file_tag_resource_handles(manager).m_count = 0;
    array->valid = false;
    cache_file_tag_resource_active_handle(manager) = -1;
    cache_file_tag_resource_pending_map(manager) = false;
    manager->m_zone_names.reset();
}

// @address 0x1b33c0 ?is_streamed@c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@UAA_NJ@Z
bool c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::is_streamed(long resource_handle)
{
    const s_cache_file_resource_gestalt *gestalt = (const s_cache_file_resource_gestalt *)m_header->instances[(short)m_resource_gestalt_index].definition;
    const s_cache_file_tag_resource_data *entry
        = &((const s_cache_file_tag_resource_data *)gestalt->resources.address)[DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle)];
    return (bool)(_rotl(((const s_cache_file_tag_resource_type_definition *)gestalt->resource_types.address)[entry->page_index].flags, 31) & 1)
        && !m_streaming;
}

// @address 0x1b4220 tag_resources_lock_render_access
extern "C" void tag_resources_lock_render_access(void)
{
    c_cache_file_tag_resource_runtime_manager *storage = g_geometry_globals.manager;
    if (storage->m_cache_file_resource_gestalt_index != -1) {
        c_tag_resource_thread_access *sub = &storage->m_resource_cache.m_render_access;
        sub->m_access_lock.read_lock();
        c_tag_resource_cache_file_access_cache **values = sub->m_per_thread_access_cache;
        int slot = get_current_thread_index();
        c_tag_resource_cache_file_access_cache *access_cache = tag_resource_cache_thread_lock_lock_freeish_acquire(sub->m_resource_thread_access_lock);
        values[slot] = access_cache;
        sub->m_per_thread_acquired_access_cache[slot] = 1;
    }
    scenario_handle_tag_resource_lock(get_current_thread_index());
}

s_resource_access_cookie *tag_resources_unlock_render_access()
{
    tag_resource_shortcuts_handle_unlock();
    return g_geometry_globals.manager->unlock_for_render_access();
}

bool scenario_active_zone_is_tag_available(long index)
{
    return index != NONE
        && bit_vector_test(g_geometry_globals.manager->m_active_tag_flags, DATUM_INDEX_TO_ABSOLUTE_INDEX(index));
}

// @address 0x1b4360 ?dispose@s_cache_file_tag_resource_buffer@@QAAXXZ
void s_cache_file_tag_resource_buffer::dispose()
{
    if (m_allocated) {
        m_allocated = 0;
        m_allocator->deallocate(m_address);
        m_address = 0;
        m_size = 0;
    }
}

// @address 0x1b43c8 cache_file_tag_resource_runtime_dispose
// shell_dispose passes &g_geometry_globals; the body works on the global itself.
extern "C" int cache_file_tag_resource_runtime_dispose(s_geometry_globals *globals)
{
    c_cache_file_tag_resource_runtime_manager *storage = g_geometry_globals.manager;
    page_storage_view_reset(&storage->m_page_storage);
    s_geometry_storage_member *member = &storage->m_resource_cache.m_geometry_storage;
    s_geometry_second_base *second = member;
    second->m_access_lock.~c_read_write_lock();
    member->m_resource_thread_access_lock.m_access_lock.~c_read_write_lock();
    storage->m_listener_buffer.dispose();
    g_geometry_globals.storage = NULL;
    int result = g_geometry_globals.allocator->deallocate(g_geometry_globals.allocation.address);
    g_geometry_globals.allocation.address = NULL;
    g_geometry_globals.allocation.size = 0;
    return result;
}

// @address 0x3bbc8 ??_7s_cache_file_tag_resource_cursor@@6B@
// @address 0x1af008 ?set_buffer@s_cache_file_tag_resource_cursor@@UAA_NV?$c_basic_buffer@X@@@Z

extern "C" inline s_cache_file_tag_resource_cursor *cache_file_tag_resource_cursor_new(
    s_cache_file_tag_resource_cursor_storage *storage, const s_cache_file_tag_resource_cursor_source *source)
{
    s_cache_file_tag_resource_cursor *object = new (align_pointer(storage->memory, 2)) s_cache_file_tag_resource_cursor(source);
    storage->object = object;
    return object;
}

static __forceinline c_basic_buffer<void> cache_file_tag_resource_buffer_range(const c_basic_buffer<void> &buffer, unsigned long offset, unsigned long size)
{
    c_basic_buffer<void> result(c_basic_buffer<void>::_uninitialized);
    result.m_address = buffer.get_range(offset, size);
    result.m_size = size;
    // match: 0x1af0e0: the explicit return copy keeps the pre-RA moves and register priorities.
    return c_basic_buffer<void>(result);
}

// >>> 0x1af0e0
// 0x1af0e0: the cursor's write: copies the part of a buffer that each entry covers to the entry's place in the
// destination, and advances the cursor's position by the buffer's size
bool s_cache_file_tag_resource_cursor::write(c_basic_buffer<void> data, c_basic_buffer<void> *out)
{
    unsigned long size = data.m_size;
    unsigned long offset = 0;
    for (long index = 0; index < source->count; index++)
    {
        s_cache_file_tag_resource_cursor_entry *entry = &source->entries[index];
        unsigned long position = m_position;
        unsigned long start = MAX(entry->start, position);
        unsigned long entry_end = entry->length + entry->start;
        unsigned long limit = position + size;
        unsigned long end = entry_end > limit ? limit : entry_end;
        if (start < end)
        {
            c_basic_buffer<void> destination = m_buffer;
            c_basic_buffer<void> entry_buffer = cache_file_tag_resource_buffer_range(destination, offset, entry->length);
            c_basic_buffer<void> source = cache_file_tag_resource_buffer_range(data, start - position, end - start);
            c_basic_buffer<void> target = cache_file_tag_resource_buffer_range(entry_buffer, start - entry->start, end - start);
            memcpy(target.m_address, source.m_address, source.m_size);
        }
        offset += entry->length;
    }
    if (out)
        *out = data;
    m_position += size;
    return true;
}
// <<< 0x1af0e0

// >>> 0x1af2c8
// 0x1af2c8: opens a reader over a buffer: slot -1 is a plain buffer stream, a set sign bit one of the cursors, any
// other slot asks that slot's source
// (the cursor is built by its out-of-line constructor)

// @address 0x3bbbc ??_7s_cache_file_tag_resource_memory_stream@@6B@

extern "C" s_cache_file_tag_resource_reader *cache_file_tag_resource_open_reader(
    s_cache_file_tag_resource_page_state *state, long slot, c_basic_buffer<void> buffer)
{
    s_cache_file_tag_resource_reader *reader;
    if (slot == -1)
    {
        s_cache_file_tag_resource_stream_storage *storage = &state->stream;
        s_cache_file_tag_resource_memory_stream *stream = new (align_pointer(storage->memory, 2)) s_cache_file_tag_resource_memory_stream();
        storage->object = stream;
        reader = (s_cache_file_tag_resource_reader *)stream;
        reader->set_buffer(buffer);
    }
    else if ((slot & (long)0x80000000) == 0)
    {
        reader = state->sources[slot]->open(buffer);
    }
    else
    {
        const s_cache_file_tag_resource_cursor_source_slot *cursor_sources = state->cursor_sources.begin();
        reader = (s_cache_file_tag_resource_reader *)cache_file_tag_resource_cursor_new(&state->cursor, &cursor_sources[slot & 0x7fffffff].source);
        reader->set_buffer(buffer);
    }
    return reader;
}
// <<< 0x1af2c8

// >>> 0x1af2b8
s_cache_file_tag_resource_reader *s_cache_file_tag_resource_page_state::create(unsigned long long position, long index,
    c_basic_buffer<void> buffer)
{
    return cache_file_tag_resource_open_reader(this, index, buffer);
}
// <<< 0x1af2b8

static __forceinline void cache_file_tag_resource_sort_swap(s_cache_file_tag_resource_relocation *a, s_cache_file_tag_resource_relocation *b)
{
    s_cache_file_tag_resource_relocation temp = *b;
    *b = *a;
    *a = temp;
}

template std::pair<s_cache_file_tag_resource_relocation *, s_cache_file_tag_resource_relocation *>
std::_Unguarded_partition<s_cache_file_tag_resource_relocation *, s_cache_file_tag_resource_sort_predicate>(
    s_cache_file_tag_resource_relocation *, s_cache_file_tag_resource_relocation *, s_cache_file_tag_resource_sort_predicate);

template std::pair<s_cache_file_tag_resource_relocation *, s_cache_file_tag_resource_relocation *>
std::_Unguarded_partition<s_cache_file_tag_resource_relocation *, s_cache_file_tag_resource_sort_key_predicate>(
    s_cache_file_tag_resource_relocation *, s_cache_file_tag_resource_relocation *, s_cache_file_tag_resource_sort_key_predicate);

static __forceinline void cache_file_tag_resource_push_heap(s_cache_file_tag_resource_relocation *first, long hole, long top,
    const s_cache_file_tag_resource_relocation &value, s_cache_file_tag_resource_sort_predicate predicate)
{
    long parent = (hole - 1) / 2;
    while (top < hole && predicate(first[parent], value)) {
        first[hole] = first[parent];
        hole = parent;
        parent = (hole - 1) / 2;
    }
    first[hole] = value;
}
// @address 0x1b58c8 cache_file_tag_resource_sort_adjust_heap
// match: 0x1b4520: inline lets cl expand this into the make_heap loop of cache_file_tag_resource_sort_by_offset; plain, its frame drops to __savegprlr_29
#if defined(_XBOX) && !defined(__clang__)
extern "C" inline
#else
extern "C"
#endif
void cache_file_tag_resource_sort_adjust_heap(s_cache_file_tag_resource_relocation *first, long hole, long bottom,
    s_cache_file_tag_resource_relocation value, s_cache_file_tag_resource_sort_predicate predicate)
{
    long top = hole;
    long child = (hole + 1) * 2;
    while (child < bottom) {
        if (predicate(first[child], first[child - 1]))
            child--;
        first[hole] = first[child];
        hole = child;
        child = (hole + 1) * 2;
    }
    if (child == bottom) {
        first[hole] = first[bottom - 1];
        hole = bottom - 1;
    }
    cache_file_tag_resource_push_heap(first, hole, top, value, predicate);
}

// @address 0x1b5c60 cache_file_tag_resource_sort_adjust_heap_by_key
// match: 0x1b5670: inline lets cl expand this into cache_file_tag_resource_sort_heap_by_key; plain, the call stays and its frame and registers move
#if defined(_XBOX) && !defined(__clang__)
extern "C" inline
#else
extern "C"
#endif
void cache_file_tag_resource_sort_adjust_heap_by_key(s_cache_file_tag_resource_relocation *first, long hole, long bottom,
    s_cache_file_tag_resource_relocation value, s_cache_file_tag_resource_sort_key_predicate predicate)
{
    long top = hole;
    long child = (hole + 1) * 2;
    for (; child < bottom; child = (child + 1) * 2) {
        if (memcmp(&first[child].key, &first[child - 1].key, sizeof(first[child].key)) < 0)
            --child;
        first[hole] = first[child];
        hole = child;
    }
    if (child == bottom) {
        first[hole] = first[bottom - 1];
        hole = bottom - 1;
    }
    cache_file_tag_resource_sort_push_heap_by_key(first, hole, top, value, predicate);
}

// @address 0x1b5d80 cache_file_tag_resource_sort_rotate
extern "C" inline void cache_file_tag_resource_sort_rotate(s_cache_file_tag_resource_relocation *first, s_cache_file_tag_resource_relocation *mid,
    s_cache_file_tag_resource_relocation *last, long *distance_type, s_cache_file_tag_resource_relocation *value_type)
{
    long shift = mid - first;
    long factor = last - first;
    for (long temp = shift; temp != 0; )
    {
        long remainder = factor % temp;
        factor = temp;
        temp = remainder;
    }
    if (factor < last - first)
    {
        for (; 0 < factor; --factor)
        {
            s_cache_file_tag_resource_relocation *start = first + factor;
            s_cache_file_tag_resource_relocation *hole = start;
            s_cache_file_tag_resource_relocation value = *hole;
            s_cache_file_tag_resource_relocation *next = hole + shift;
            if (next == last)
                next = first;
            while (next != start)
            {
                *hole = *next;
                hole = next;
                if (shift < last - next)
                    next += shift;
                else
                    next = first + (shift - (last - next));
            }
            *hole = value;
        }
    }
}

// >>> 0x1b5468
// 0x1b5468: std::_Insertion_sort over the key predicate, as <algorithm> spells it (std::rotate inlined; its
// _Rotate is cache_file_tag_resource_sort_rotate, called with the distance and value type pointers it ignores)

static __forceinline void cache_file_tag_resource_rotate(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *mid, s_cache_file_tag_resource_relocation *last)
{
    if (first != mid && mid != last)
        cache_file_tag_resource_sort_rotate(first, mid, last, (long *)0, (s_cache_file_tag_resource_relocation *)0);
}
extern "C" void cache_file_tag_resource_sort_insertion_by_key(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, s_cache_file_tag_resource_sort_key_predicate predicate)
{
    if (first != last)
        for (s_cache_file_tag_resource_relocation *next = first; ++next != last; )
            if (predicate(*next, *first))
            {
                s_cache_file_tag_resource_relocation *next1 = next;
                cache_file_tag_resource_rotate(first, next, ++next1);
            }
            else
            {
                s_cache_file_tag_resource_relocation *dest = next;
                for (s_cache_file_tag_resource_relocation *dest0 = dest; predicate(*next, *--dest0); )
                    dest = dest0;
                if (dest != next)
                {
                    s_cache_file_tag_resource_relocation *next1 = next;
                    cache_file_tag_resource_rotate(dest, next, ++next1);
                }
            }
}
// <<< 0x1b5468

// >>> 0x1b4520
// 0x1b4520: std::_Sort over the end-offset predicate, as <algorithm> spells it (make_heap and _Insertion_sort inlined;
// _Adjust_heap is cache_file_tag_resource_sort_adjust_heap; sort_heap is the identical copy
// main_game links as main_game_sort_heap)

static __forceinline void cache_file_tag_resource_make_heap(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, s_cache_file_tag_resource_sort_predicate predicate)
{
    if (1 < last - first)
    {
        long bottom = last - first;
        for (long hole = bottom / 2; 0 < hole; )
        {
            --hole;
            cache_file_tag_resource_sort_adjust_heap(first, hole, bottom, s_cache_file_tag_resource_relocation(*(first + hole)), predicate);
        }
    }
}
static __forceinline void cache_file_tag_resource_insertion_sort(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, s_cache_file_tag_resource_sort_predicate predicate)
{
    if (first != last)
        for (s_cache_file_tag_resource_relocation *next = first; ++next != last; )
            if (predicate(*next, *first))
            {
                s_cache_file_tag_resource_relocation *next1 = next;
                cache_file_tag_resource_rotate(first, next, ++next1);
            }
            else
            {
                s_cache_file_tag_resource_relocation *dest = next;
                for (s_cache_file_tag_resource_relocation *dest0 = dest; predicate(*next, *--dest0); )
                    dest = dest0;
                if (dest != next)
                {
                    s_cache_file_tag_resource_relocation *next1 = next;
                    cache_file_tag_resource_rotate(dest, next, ++next1);
                }
            }
}
extern "C" void cache_file_tag_resource_sort_by_offset(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, long ideal, s_cache_file_tag_resource_sort_predicate predicate)
{
    long count;
    for (; 32 < (count = last - first) && 0 < ideal; )
    {
        std::pair<s_cache_file_tag_resource_relocation *, s_cache_file_tag_resource_relocation *> mid = std::_Unguarded_partition(first, last, predicate);
        ideal /= 2, ideal += ideal / 2;
        if (mid.first - first < last - mid.second)
            cache_file_tag_resource_sort_by_offset(first, mid.first, ideal, predicate), first = mid.second;
        else
            cache_file_tag_resource_sort_by_offset(mid.second, last, ideal, predicate), last = mid.first;
    }
    if (32 < count)
    {
        cache_file_tag_resource_make_heap(first, last, predicate);
        main_game_sort_heap(first, last, predicate);
    }
    else if (1 < count)
        cache_file_tag_resource_insertion_sort(first, last, predicate);
}
// <<< 0x1b4520

// >>> 0x1b4750

extern "C" void cache_file_tag_resource_sort_by_key(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, long ideal, s_cache_file_tag_resource_sort_key_predicate predicate)
{
    long count;
    for (; 32 < (count = last - first) && 0 < ideal; )
    {
        std::pair<s_cache_file_tag_resource_relocation *, s_cache_file_tag_resource_relocation *> mid = std::_Unguarded_partition(first, last, predicate);
        ideal /= 2, ideal += ideal / 2;
        if (mid.first - first < last - mid.second)
            cache_file_tag_resource_sort_by_key(first, mid.first, ideal, predicate), first = mid.second;
        else
            cache_file_tag_resource_sort_by_key(mid.second, last, ideal, predicate), last = mid.first;
    }
    if (32 < count)
    {
        long bottom = last - first;
        if (1 < bottom)
        {
            for (long hole = bottom / 2; 0 < hole; )
            {
                --hole;
                cache_file_tag_resource_sort_adjust_heap_by_key(first, hole, bottom, s_cache_file_tag_resource_relocation(*(first + hole)),
                    predicate);
            }
        }
        cache_file_tag_resource_sort_heap_by_key(first, last, predicate);
    }
    else if (1 < count)
        cache_file_tag_resource_sort_insertion_by_key(first, last, predicate);
}
// <<< 0x1b4750

// >>> 0x1b5670
// 0x1b5670: std::_Sort_heap over the memcmp predicate, as <algorithm> spells it (pop_heap inlined; _Adjust_heap is
// cache_file_tag_resource_sort_adjust_heap_by_key)

static __forceinline void cache_file_tag_resource_pop_heap(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, s_cache_file_tag_resource_relocation *dest,
    s_cache_file_tag_resource_relocation value, s_cache_file_tag_resource_sort_key_predicate predicate)
{
    *dest = *first;
    cache_file_tag_resource_sort_adjust_heap_by_key(first, 0, last - first, value, predicate);
}
static __forceinline void cache_file_tag_resource_pop_heap_0(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, s_cache_file_tag_resource_sort_key_predicate predicate)
{
    cache_file_tag_resource_pop_heap(first, last - 1, last - 1, s_cache_file_tag_resource_relocation(*(last - 1)), predicate);
}
static __forceinline void cache_file_tag_resource_pop_heap(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, s_cache_file_tag_resource_sort_key_predicate predicate)
{
    if (1 < last - first)
        cache_file_tag_resource_pop_heap_0(first, last, predicate);
}
extern "C" void cache_file_tag_resource_sort_heap_by_key(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, s_cache_file_tag_resource_sort_key_predicate predicate)
{
    for (; 1 < last - first; --last)
        cache_file_tag_resource_pop_heap(first, last, predicate);
}
// <<< 0x1b5670

// @address 0x1b5ef8 cache_file_tag_resource_sort_push_heap_by_key
extern "C" void cache_file_tag_resource_sort_push_heap_by_key(s_cache_file_tag_resource_relocation *first, long hole, long top,
    s_cache_file_tag_resource_relocation value, s_cache_file_tag_resource_sort_key_predicate predicate)
{
    long parent = (hole - 1) / 2;
    while (top < hole && memcmp(&first[parent].key, &value.key, sizeof(value.key)) < 0) {
        first[hole] = first[parent];
        hole = parent;
        parent = (hole - 1) / 2;
    }
    first[hole] = value;
}

// @address 0x21ae60 cache_file_tag_resource_find_ready_slot
extern "C" long cache_file_tag_resource_find_ready_slot(const s_cache_file_bulk_reader_slot_set *set)
{
    long result = -1;
    unsigned long i = 0;
    const long ready_mask = set->mask_2;
    for (; i < NUMBEROF(set->slots); ++i) {
        if (TEST_BIT(ready_mask, i) && set->slots[i].bits.index == set->bits.offset) {
            result = i;
            break;
        }
    }
    return result;
}

void *tag_resource_available(const s_tag_resource *resource)
{
    void *result = 0;
    c_tag_resource_cache_file_access_cache *cache
        = g_geometry_globals.manager->m_resource_cache.m_render_access.m_per_thread_access_cache[get_current_thread_index()];
    long resource_handle = resource->resource_handle;
    if (resource_handle != -1 && cache->m_resource_handles[DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle)] != -1)
        result = cache->m_resource_values[DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_handle)];
    return result;
}

unsigned char s_cache_file_resource_fixed_array::find_direct(
    long index, unsigned long offset, long unused, unsigned char *alternate, unsigned long *result)
{
    bool success = false;
    s_cache_file_resource_handle_pair *entry = &handles[index];
    short first = entry->first_shared_page;
    if (first == -1)
    {
        s_cache_file_tag_resource_element *node_base = elements;
        short node = entry->first;
        *alternate = 0;
        short next = node_base[node].shared_page_index;
        if (next != -1)
        {
            success = find_span(next, offset, unused, result);
        }
        else
        {
            *result = offset;
            success = true;
        }
    }
    else
    {
        short next = entry->second_shared_page;
        if (next != -1 && find_span(next, offset, unused, result))
        {
            *alternate = 1;
            success = true;
        }
        else if (find_span(first, offset, unused, result))
        {
            *alternate = 0;
            success = true;
        }
    }
    return success;
}

unsigned char s_cache_file_resource_fixed_array::find_with_offset(
    // match: 0x21bb78: without __restrict the found branches reorder li r3,1, the lwz of *result and the *alternate stb
    long index, unsigned long offset, long unused, unsigned char *__restrict alternate, unsigned long *__restrict result)
{
    bool success = false;
    s_cache_file_resource_handle_pair *entry = &handles[index];
    short first = entry->first_shared_page;
    if (first == -1)
    {
        *alternate = 0;
        *result = offset;
        success = true;
    }
    else
    {
        short next = entry->second_shared_page;
        if (next != -1 && find_span(next, offset, unused, result))
        {
            *result += entry->second_fixup_value;
            *alternate = 1;
            success = true;
        }
        else if (find_span(first, offset, unused, result))
        {
            *result += entry->first_fixup_value;
            *alternate = 0;
            success = true;
        }
    }
    return success;
}

bool s_cache_file_resource_fixed_array::find_span(long list_index, unsigned long offset, long unused, unsigned long *result)
{
    bool found = false;
    const s_cache_file_resource_shared_page *page = &shared_pages.begin()[list_index];
    unsigned long packed = 0;
    for (long i = 0; i < page->subpages.count(); i++)
    {
        const s_cache_file_span *span = &page->subpages.begin()[i];
        if (offset < span->start)
            break;
        if (offset - span->start < span->size)
        {
            *result = packed - span->start + offset;
            found = true;
            break;
        }
        packed += span->size;
    }
    return found;
}

static __forceinline s_structure_bsp_resources *cache_file_tag_resource_bsp_get_resources(
    const c_structure_bsp_resource_interface *resource_interface)
{
    unsigned int index = resource_interface->get_datum_index();
    s_geometry_storage_member *member = &g_geometry_globals.manager->m_resource_cache.m_geometry_storage;
    s_geometry_second_base *secondary = member;
    return static_cast<s_structure_bsp_resources *>(
        secondary->m_per_thread_access_cache[get_current_thread_index()]->m_resource_values[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)]);
}
__forceinline const s_structure_bsp_resources *s_structure_bsp_resource::get_resources() const
{
    if (use_resource_items)
        return cache_file_tag_resource_bsp_get_resources(&collision_resource_interface);
    return (const s_structure_bsp_resources *)raw_resources.address;
}
static __forceinline bool scenario_tag_resource_structure_bsp_is_active(long structure_bsp_index)
{
    return structure_bsp_index >= 0 && structure_bsp_index < 16 && (g_active_structure_bsp_mask & (1 << structure_bsp_index)) != 0;
}
static __forceinline const structure_bsp *scenario_tag_resource_structure_bsp_get(long structure_bsp_index)
{
    const scenario *scenario_definition = global_scenario;
    return (const structure_bsp *)
        global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(
            ((scenario_structure_bsp_reference *)scenario_definition->structure_bsps.address)[structure_bsp_index].structure_bsp.index)]
            .definition;
}
void scenario_handle_tag_resource_lock(long thread_index)
{
    s_tag_resource_lock_cache_block *blocks = g_collision_bsp_cache.lock_blocks;
    memset(&blocks[thread_index], 0, sizeof(s_tag_resource_lock_cache_block));
    for (long structure_bsp_index = 0; structure_bsp_index < 16; structure_bsp_index++) {
        if (scenario_tag_resource_structure_bsp_is_active(structure_bsp_index)) {
            const structure_bsp *bsp = scenario_tag_resource_structure_bsp_get(structure_bsp_index);
const s_structure_bsp_resources *resources = bsp->resource.get_resources();
            blocks[thread_index].collision_bsps[structure_bsp_index] = resources->collision_bsp;
resources = bsp->resource.get_resources();
            blocks[thread_index].bounds[structure_bsp_index] = resources->bounds;
        }
    }
}

static __forceinline char *cache_file_tag_resource_buffer_end(const c_basic_buffer<void> &buffer)
{
    return (char *)buffer.m_address + buffer.m_size;
}
static __forceinline bool cache_file_tag_resource_buffer_contains(const c_basic_buffer<void> &outer, const c_basic_buffer<void> &inner)
{
    char *outer_address = (char *)outer.m_address;
    unsigned long outer_size = outer.m_size;
    char *address = (char *)inner.m_address;
    unsigned long size = inner.m_size;
    return address >= outer_address && address <= outer_address + outer_size && address + size >= outer_address && address + size <= outer_address + outer_size;
}

static __forceinline bool cache_file_tag_resource_page_key_less(const s_cache_file_tag_resource_page_key &a, const s_cache_file_tag_resource_page_key &b)
{
    return memcmp(&a, &b, sizeof(s_cache_file_tag_resource_page_key)) < 0;
}

static __forceinline s_cache_file_tag_resource_relocation *cache_file_tag_resource_relocation_lower_bound(
    s_cache_file_tag_resource_relocation *first, s_cache_file_tag_resource_relocation *last, const s_cache_file_tag_resource_page_key &key)
{
    long count = last - first;
    while (count > 0)
    {
        long half = count / 2;
        s_cache_file_tag_resource_relocation *middle = first + half;
        if (cache_file_tag_resource_page_key_less(middle->key, key))
        {
            first = middle + 1;
            count -= half + 1;
        }
        else
        {
            count = half;
        }
    }
    return first;
}

// @address 0x1af5e0 cache_file_tag_resource_try_and_get_relocation
extern "C" bool cache_file_tag_resource_try_and_get_relocation(s_cache_file_tag_resource_relocation_list *list,
    const s_cache_file_runtime_definition *file, const s_cache_file_tag_resource_element *page, c_basic_buffer<void> *buffer)
{
    bool result = false;
    s_cache_file_tag_resource_page_key key;
    key.file = file->io_file_index;
    key.offset = file->file_offset + page->offset;
    key.compressed_size = page->compressed_size;
    key.uncompressed_size = page->size;
    s_cache_file_tag_resource_relocation *relocation = cache_file_tag_resource_relocation_lower_bound(list->entries, list->entries + list->count, key);
    if (relocation != list->entries + list->count && !cache_file_tag_resource_page_key_less(key, relocation->key) &&
        !cache_file_tag_resource_page_key_less(relocation->key, key))
    {
        if (cache_file_tag_resource_buffer_contains(list->window, relocation->buffer))
        {
            result = true;
            *buffer = relocation->buffer;
        }
    }
    return result;
}

// @address 0x1af798 cache_file_tag_resource_add_relocation
extern "C" void cache_file_tag_resource_add_relocation(s_cache_file_tag_resource_relocation_list *list,
    const s_cache_file_runtime_definition *file, const s_cache_file_tag_resource_element *page, c_basic_buffer<void> buffer)
{
    if (list->count == 16384)
        return;
    long index = list->count++;
    s_cache_file_tag_resource_relocation *relocation = &list->entries[index];
    relocation->key.offset = file->file_offset + page->offset;
    relocation->key.compressed_size = page->compressed_size;
    relocation->key.uncompressed_size = page->size;
    relocation->key.file = file->io_file_index;
    relocation->buffer = buffer;
    if (list->window.m_size == 0)
        list->window = buffer;
    if (list->window.m_address > buffer.m_address)
    {
        c_basic_buffer<void> window(c_basic_buffer<void>::_uninitialized);
        window.m_address = buffer.m_address;
        window.m_size = cache_file_tag_resource_buffer_end(list->window) - (char *)buffer.m_address;
        list->window = window;
    }
    if (cache_file_tag_resource_buffer_end(list->window) < cache_file_tag_resource_buffer_end(buffer))
    {
        c_basic_buffer<void> window(c_basic_buffer<void>::_uninitialized);
        window.m_address = list->window.m_address;
        window.m_size = cache_file_tag_resource_buffer_end(buffer) - (char *)list->window.m_address;
        list->window = window;
    }
}

static __forceinline c_basic_buffer<void> cache_file_tag_resource_buffer_intersection(c_basic_buffer<void> a, c_basic_buffer<void> b)
{
    char *start = (char *)MAX(a.m_address, b.m_address);
    char *end = cache_file_tag_resource_buffer_end(a) <= cache_file_tag_resource_buffer_end(b) ? cache_file_tag_resource_buffer_end(a)
                                                                                               : cache_file_tag_resource_buffer_end(b);
    c_basic_buffer<void> result(c_basic_buffer<void>::_uninitialized);
    result.m_address = start;
    result.m_size = end - start;
    return result;
}

static __forceinline bool cache_file_tag_resource_index_valid(long index, long count)
{
    const unsigned long &position = index;
    const unsigned long &limit = count;
    return position < limit;
}

static __forceinline c_basic_buffer<void> cache_file_tag_resource_buffer_make(void *address, unsigned long size)
{
    c_basic_buffer<void> buffer(c_basic_buffer<void>::_uninitialized);
    buffer.m_address = address;
    buffer.m_size = size;
    return buffer;
}

static __forceinline void cache_file_tag_resource_copy(void *destination, const c_basic_buffer<void> &source)
{
    memcpy(destination, source.m_address, source.m_size);
}

// @address 0x1af880 cache_file_tag_resource_update_relocation_window
extern "C" void cache_file_tag_resource_update_relocation_window(
    s_cache_file_tag_resource_relocation_list *list, c_basic_buffer<void> old_buffer, c_basic_buffer<void> buffer)
{
    if (list->window.m_size == 0)
        return;
    if (cache_file_tag_resource_buffer_contains(buffer, list->window))
        return;
    if (list->window.m_address < buffer.m_address)
    {
        const long &count = list->count;
        while (cache_file_tag_resource_index_valid(list->cursor, count))
        {
            s_cache_file_tag_resource_relocation *relocation = &list->entries[list->cursor];
            if (relocation->buffer.m_address >= buffer.m_address)
                break;
            if ((unsigned long)(cache_file_tag_resource_buffer_end(buffer) - cache_file_tag_resource_buffer_end(list->window)) < relocation->buffer.m_size)
                break;
            const long &size = list->window.m_size;
            char *start = (char *)list->window.m_address;
            char *destination = start + size;
            list->window = cache_file_tag_resource_buffer_make(
                list->window.m_address, cache_file_tag_resource_buffer_end(list->window) + relocation->buffer.m_size - start);
            c_basic_buffer<void> relocated(c_basic_buffer<void>::_uninitialized);
            relocated.m_address = destination;
            relocated.m_size = relocation->buffer.m_size;
            cache_file_tag_resource_copy(destination, relocation->buffer);
            relocation->buffer = relocated;
            list->cursor++;
        }
    }
    list->window = cache_file_tag_resource_buffer_intersection(list->window, buffer);
}
// >>> 0x1b0230

// <<< 0x1b0230

// @address 0xcc8 ??_7c_cache_file_tag_resource_runtime_manager_storage@@6Bc_tag_resource_runtime_listener@@@
// @address 0x3bb80 ??_7c_tag_resource_runtime_active_set@@6B@
// @address 0x3bc00 ??_7c_cache_file_tag_resource_runtime_manager_storage@@6Bc_tag_resource_runtime_active_set@@@
// @address 0x42d380 ?any_resources_active@c_cache_file_tag_resource_runtime_manager_storage@@UBA_NXZ
// @address 0x42d298 ?is_resource_deferred@c_cache_file_tag_resource_runtime_manager_storage@@UBA_NJJ@Z
// @address 0x3bb90 ??_7c_tag_resource_paging_page_filter@@6B@
// @address 0x3bc10 ??_7c_cache_file_tag_resource_runtime_manager_storage@@6Bc_tag_resource_paging_page_filter@@@
// @address 0x1b2838 ?page_is_necessary@c_cache_file_tag_resource_runtime_manager_storage@@UAA_NJ@Z
// @address 0x3bb94 ??_7c_tag_resource_page_range_allocator@@6B@
// @address 0x42d298 ?set_window@c_tag_resource_page_range_allocator@@UAA_NV?$c_basic_buffer@X@@PAV2@@Z
// @address 0x5bec98 ?get_window@c_tag_resource_page_range_allocator@@UAAXPAUs_cache_file_tag_resource_window@@@Z
// @address 0x5bec98 ?set_locked_window@c_tag_resource_page_range_allocator@@UAAXV?$c_basic_buffer@X@@@Z
// @address 0x5bec98 ?lock_window@c_tag_resource_page_range_allocator@@UAAXXZ
// @address 0x3bc14 ??_7c_cache_file_tag_resource_runtime_manager_storage@@6Bc_tag_resource_page_range_allocator@@@
// @address 0x1b2ab0 ?resize@c_cache_file_tag_resource_runtime_manager_storage@@UAA_NPAV?$c_basic_buffer@X@@KKJ@Z
// @address 0x1b2c60 ?get_window@c_cache_file_tag_resource_runtime_manager_storage@@UAAXPAUs_cache_file_tag_resource_window@@@Z
// @address 0x1b2d00 ?reset_window@c_cache_file_tag_resource_runtime_manager_storage@@UAAXPAUs_cache_file_tag_resource_window@@@Z
// @address 0x1b2cc8 ?set_locked_window@c_cache_file_tag_resource_runtime_manager_storage@@UAAXV?$c_basic_buffer@X@@@Z
// @address 0x1b2ce8 ?lock_window@c_cache_file_tag_resource_runtime_manager_storage@@UAAXXZ
// @address 0x3bb80 ??_7c_tag_resource_cache_file_reader@@6B@
// @address 0xdec ??_7c_cache_file_tag_resource_runtime_manager_storage@@6Bc_tag_resource_cache_file_reader@@@
// @address 0x1b2f40 ?update_io_state@c_cache_file_tag_resource_runtime_manager_storage@@UAAXXZ
// @address 0x1b2f48 ?wait_for_io@c_cache_file_tag_resource_runtime_manager_storage@@UAAXXZ
// @address 0x1b2f50 ?request_data@c_cache_file_tag_resource_runtime_manager_storage@@UAA_NPAVc_indirect_cache_file_bulk_read_iterator@@@Z
// @address 0x1b2fc8 ?request_data_async@c_cache_file_tag_resource_runtime_manager_storage@@UAA_NPBUs_indirect_cache_file_read_request@@PAVc_synchronized_long@@PAVc_asynchronous_io_marker@@@Z
// @address 0x3bb90 ??_7c_indirect_cache_file_location_atlas@@6B@
// @address 0x998 ??_7c_cache_file_tag_resource_runtime_manager_storage@@6Bc_indirect_cache_file_location_atlas@@@
// @address 0x1019e0 ?get_location@c_cache_file_tag_resource_runtime_manager_storage@@UAA_N_KPAUs_indirect_cache_file_location@@@Z
// @address 0x3bbdc ??_7c_physical_memory_contiguous_region_listener@@6B@
// @address 0x3bc30 ??_7c_cache_file_tag_resource_runtime_manager_storage@@6Bc_physical_memory_contiguous_region_listener@@@
// @address 0x5bec98 ?initialize_resize_buffer@c_cache_file_tag_resource_runtime_manager_storage@@UAAXV?$c_basic_buffer@X@@@Z
// @address 0x1b03a0 ?resize_no_fail@c_cache_file_tag_resource_runtime_manager_storage@@UAAXV?$c_basic_buffer@X@@0@Z
// @address 0x5bec98 ?dispose_resize_buffer@c_cache_file_tag_resource_runtime_manager_storage@@UAAXV?$c_basic_buffer@X@@@Z
// @address 0x3bb90 ??_7s_cache_file_tag_resource_graph_interface@@6B@
// @address 0x3bc3c ??_7c_cache_file_tag_resource_runtime_manager_storage@@6Bs_cache_file_tag_resource_graph_interface@@@
// @address 0x3bbdc ??_7c_cache_file_tag_resource_window_allocator@@6B@
// @address 0x3bc40 ??_7c_cache_file_tag_resource_runtime_manager_storage@@6Bc_cache_file_tag_resource_window_allocator@@@
// @address 0x3bb80 ??_7c_game_state_proc_handler@@6B@


// @address 0x1b88 ??_7c_cache_file_tag_resource_datum_handler@c_cache_file_tag_resource_runtime_manager@@6B@

// @address 0x111c ??_7c_cache_file_tag_resource_runtime_window_allocator_storage@@6B@

// @address 0x3bbb4 ??_7c_cache_file_tag_resource_runtime_listener_storage@@6B@

// @address 0x3bbd4 ??_7s_cache_file_tag_resource_page_state@@6B@

// @address 0x3bbe8 ??_7s_cache_file_tag_resource_slots@@6B@

STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_thread_access_storage, m_access_cache_storage) == 0x24, runtime_thread_access_storage_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_thread_access_storage, m_views) == 0x48, runtime_thread_access_view_offset);
STATIC_ASSERT_ABI32(sizeof(c_cache_file_tag_resource_runtime_thread_access_storage) == 0x80, runtime_thread_access_storage_size);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_slots, request_data) + offsetof(s_cache_file_tag_resource_requests, requests) == 0xc,
    runtime_file_slot_array_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_slots, request_data) == 8, runtime_file_slot_timestamp_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_slots, bitmap) == 0x84, runtime_file_slot_bitmap_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_slots, loader) == 0x88, runtime_file_slot_listener_offset);
STATIC_ASSERT_ABI32(sizeof(s_cache_file_tag_resource_runtime_resource_arrays) == 0x20, runtime_resource_arrays_size);

static __forceinline s_cache_file_runtime_handle_stack &cache_file_tag_resource_handles(
    c_cache_file_tag_resource_runtime_manager_storage *manager)
{ return manager->m_handles; }
static __forceinline s_data_array *&cache_file_tag_resource_definitions(c_cache_file_tag_resource_runtime_manager_storage *manager)
{ return manager->m_shared_file_handles; }
static __forceinline long &cache_file_tag_resource_active_handle(c_cache_file_tag_resource_runtime_manager_storage *manager)
{ return manager->m_zone_state.m_active_zone_handle; }
static __forceinline bool &cache_file_tag_resource_pending_map(c_cache_file_tag_resource_runtime_manager_storage *manager)
{ return manager->m_active_zone_changed; }
static __forceinline c_tag_resource_cache_new &cache_file_tag_resource_cache(c_cache_file_tag_resource_runtime_manager_storage *manager)
{ return manager->m_resource_cache.m_cache; }
static __forceinline s_cache_file_resource_element_set &cache_file_tag_resource_elements(
    c_cache_file_tag_resource_runtime_manager_storage *manager)
{ return manager->m_element_set; }
static __forceinline s_cache_file_tag_resource_relocation_list &cache_file_tag_resource_relocations(
    c_cache_file_tag_resource_runtime_manager_storage *manager)
{ return manager->m_relocations; }
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_handles) == 0x2b0bc, runtime_handles_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_shared_file_handles) == 0x2b0d0, runtime_definitions_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_element_set) == 0x2b0f4, runtime_elements_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_zone_names) == 0x50, runtime_slot_state_offset);

// @address 0x1b3e10 cache_file_tag_resource_compute_required_flags
extern "C" void cache_file_tag_resource_compute_required_flags(c_cache_file_tag_resource_runtime_manager *runtime, void *buffer, void *buffer2)
{
    s_cache_file_bit_vector_target target;
    const s_cache_file_resource_gestalt *block =
        (const s_cache_file_resource_gestalt *)runtime->m_header->instances[(short)runtime->m_cache_file_resource_gestalt_index].definition;
    cache_file_tag_resource_bit_vector_target_initialize(&target, block->resources.count, buffer,
        g_cache_file_globals.tags_header->instance_count, buffer2, runtime->m_streaming_enabled);
    cache_file_tag_resource_update_all_bits(&target, &block->always);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask0, runtime->m_zone_state.m_active_state.mask4);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask0_alternate, runtime->m_zone_state.m_active_state.mask4);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask1, runtime->m_zone_state.m_active_state.mask8);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask2, runtime->m_zone_state.m_active_state.maskc);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask3, runtime->m_zone_state.m_active_state.mask10);
    if (!runtime->m_zone_state.m_active_state.flags) {
        cache_file_tag_resource_update_all_bits(&target, &block->optional);
    }
}

// @address 0x1b3f00 cache_file_tag_resource_compute_zone_flags
extern "C" void cache_file_tag_resource_compute_zone_flags(c_cache_file_tag_resource_runtime_manager *runtime, void *buffer)
{
    s_cache_file_bit_vector_target target;
    const s_cache_file_resource_gestalt *block =
        (const s_cache_file_resource_gestalt *)runtime->m_header->instances[(short)runtime->m_cache_file_resource_gestalt_index].definition;
    cache_file_tag_resource_bit_vector_target_initialize(&target, block->resources.count, buffer,
        g_cache_file_globals.tags_header->instance_count, 0, runtime->m_streaming_enabled);
    cache_file_tag_resource_update_all_bits(&target, &block->always);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask0, runtime->m_zone_state.m_active_state.mask14);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask0_alternate, runtime->m_zone_state.m_active_state.mask14);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask1, runtime->m_zone_state.m_active_state.mask14);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask2, runtime->m_zone_state.m_active_state.mask18);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask3, runtime->m_zone_state.m_active_state.mask1c);
}

static __forceinline void cache_file_tag_resource_runtime_scratch_initialize(
    s_cache_file_bit_vector_target *scratch, long resource_count, void *resource_bits,
    long tag_count, void *tag_bits, unsigned char include_streamed_resources)
{
    scratch->count0 = resource_count;
    scratch->bits0 = (unsigned long *)resource_bits;
    scratch->count1 = tag_count;
    scratch->bits1 = (unsigned long *)tag_bits;
    scratch->flag = include_streamed_resources;
    // Full resource/tag flag words, matching the manager's [1024]/[625] flag arrays.
    if (scratch->bits0)
        memset(scratch->bits0, 0, sizeof(unsigned long) * 1024);
    if (scratch->bits1)
        memset(scratch->bits1, 0, sizeof(unsigned long) * 625);
}

// @address 0x1b4130 cache_file_tag_resource_compute_scenario_flags
extern "C" void cache_file_tag_resource_compute_scenario_flags(c_cache_file_tag_resource_runtime_manager_storage *runtime, unsigned long mask0,
    unsigned long mask1, unsigned long mask2, unsigned long mask3, void *buffer2)
{
    s_cache_file_bit_vector_target target;
    const s_cache_file_resource_gestalt *block =
        (const s_cache_file_resource_gestalt *)runtime->m_header->instances[(short)runtime->m_cache_file_resource_gestalt_index].definition;
    cache_file_tag_resource_runtime_scratch_initialize(&target, block->resources.count, 0,
        g_cache_file_globals.tags_header->instance_count, buffer2, runtime->m_streaming_enabled);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask0, mask0);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask0_alternate, mask0);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask1, mask1);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask2, mask2);
    cache_file_tag_resource_update_masked_bits(&target, &block->mask3, mask3);
    cache_file_tag_resource_update_all_bits(&target, &block->always);
}

// @address 0x1b13d8 ?get_buffer@s_cache_file_tag_resource_page_reader@@UBA?AV?$c_basic_buffer@X@@XZ
c_basic_buffer<void> s_cache_file_tag_resource_page_reader::get_buffer() const
{
    return m_manager->m_relocations.window;
}

STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_required_resource_flags) == 0x28294, runtime_required_flags_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_active_resource_flags) == 0x29294, runtime_active_flags_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_active_tag_flags) == 0x2a294, runtime_active_tag_flags_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_locked) == 0x8b61d, runtime_locked_offset);
// @address 0x1aff08 ??0c_cache_file_tag_resource_runtime_manager_storage@@QAA@XZ
c_cache_file_tag_resource_runtime_manager_storage::c_cache_file_tag_resource_runtime_manager_storage() :
    m_listener_link(&m_listener_storage),
    m_requested_buffer(), m_result_buffer(),
    m_handles(0), m_locked(false)
{
}

void c_cache_file_tag_resource_runtime_manager_storage::resize_no_fail(c_basic_buffer<void> old_buffer, c_basic_buffer<void> buffer)
{
    if (m_locked)
    {
        XPhysicalProtect(buffer.m_address, buffer.m_size, 4);
        cache_file_tag_resource_update_relocation_window(&cache_file_tag_resource_relocations(this),
            old_buffer, buffer);
    }
}

static __forceinline void cache_file_tag_resource_file_slots_initialize(s_cache_file_tag_resource_slots *file_slots)
{
    file_slots->m_reclaim_expired = false;
    s_cache_file_tag_resource_request empty_slot = {};
    file_slots->request_data.requests[0] = empty_slot;
    file_slots->request_data.requests[1] = empty_slot;
    file_slots->request_data.requests[2] = empty_slot;
    file_slots->request_data.requests[3] = empty_slot;
    file_slots->request_data.requests[4] = empty_slot;
    file_slots->request_data.requests[5] = empty_slot;
}

static __forceinline void cache_file_tag_resource_cursor_reset(s_cache_file_tag_resource_named_value *cursor)
{
    cursor->m_value = -1;
    cursor->m_name.clear();
}

// @address 0x1b0400 cache_file_tag_resource_runtime_initialize
extern "C" void cache_file_tag_resource_runtime_initialize(c_cache_file_tag_resource_runtime_manager_storage *storage, c_allocation_base *allocator)
{
    storage->m_streaming_enabled = CACHE_FILES_ENABLED && !g_utility_drive_mounted;
    storage->m_previous_game_mode = 0;
    storage->m_game_mode = 0;
    storage->m_cache_file_resource_gestalt_index = -1;
    cache_file_tag_resource_cache_initialize(&storage->m_resource_cache);
    g_physical_memory_globals.listener = storage;
    cache_file_tag_resource_file_slots_initialize(&storage->m_file_slots);
    g_game_state_procs.handlers[1] = &storage->m_file_slots;
    storage->m_shared_file_handles = data_new("shared file handles", 5, sizeof(s_cache_file_runtime_definition), 0, (c_allocation_base *)&g_runtime_state_allocation);
    cache_file_tag_resource_cursor_reset(&storage->m_second_cursor);
    cache_file_tag_resource_cursor_reset(&storage->m_first_cursor);
}

static __forceinline bool cache_file_tag_resource_active_state_first_differs(
    const s_cache_file_tag_resource_active_state *a, const s_cache_file_tag_resource_active_state *b)
{
    return a->flags != b->flags || a->mask4 != b->mask4 || a->mask8 != b->mask8 || a->maskc != b->maskc || a->mask10 != b->mask10;
}
static __forceinline bool cache_file_tag_resource_active_state_second_differs(
    const s_cache_file_tag_resource_active_state *a, const s_cache_file_tag_resource_active_state *b)
{
    return a->flags != b->flags || a->mask14 != b->mask14 || a->mask18 != b->mask18 || a->mask1c != b->mask1c;
}

// @address 0x1b3d08 cache_file_tag_resource_set_active_state
extern "C" void cache_file_tag_resource_set_active_state(c_cache_file_tag_resource_runtime_manager_storage *manager, long, long,
    // match: 0x1b3d08: without __restrict the lbz pair of the inlined active-state byte compare swaps r8/r9
    const s_cache_file_tag_resource_active_state *__restrict state)
{
    if (manager->m_locked)
        return;
    manager->m_required_resource_state_changed = !manager->m_state_valid
        || cache_file_tag_resource_active_state_first_differs(&manager->m_zone_state.m_active_state, state);
    manager->m_active_resource_state_changed = cache_file_tag_resource_active_state_second_differs(&manager->m_zone_state.m_active_state, state);
    manager->m_zone_state.m_active_state = *state;
}

// @address 0x1b6ef0 cache_file_tag_resource_reset_active_state
extern "C" void cache_file_tag_resource_reset_active_state(void)
{
    s_cache_file_tag_resource_active_state state;
    memset(&state, 0, sizeof(state));
    state.flags = 1;
    cache_file_tag_resource_set_active_state(g_geometry_globals.storage, -1, 0, &state);
}

static __forceinline void cache_file_tag_resource_flags_set(unsigned char *flags, unsigned char mask)
{
    *flags |= mask;
}

static __forceinline const s_tag_persistent_identifier *cache_file_tag_resource_file_key_find(
    const s_tag_persistent_identifier *first, const s_tag_persistent_identifier *last,
    const s_tag_persistent_identifier &key)
{
    for (; first != last; first++)
    {
        if (memcmp(&key, first, sizeof(s_tag_persistent_identifier)) == 0)
            break;
    }
    return first;
}

static __forceinline bool cache_file_tag_resource_file_key_list_find(const c_typed_tag_block<s_tag_persistent_identifier> *list,
    const s_tag_persistent_identifier &key, const s_tag_persistent_identifier **result)
{
    bool found = false;
    const s_tag_persistent_identifier *it = cache_file_tag_resource_file_key_find(list->begin(), list->begin() + list->count(), key);
    if (it != list->begin() + list->count())
    {
        *result = it;
        found = true;
    }
    return found;
}

// @address 0x21bcd0 cache_file_tag_resource_page_table_initialize
extern "C" void cache_file_tag_resource_page_table_initialize(s_data_array **table,
    const s_cache_file_tag_resource_file_header *header, const c_typed_tag_block<s_tag_persistent_identifier> *keys, long owner, long base_index)
{
    s_data_array *pages = *table;
    pages->valid = true;
    data_delete_all(pages);
    if (!header)
        return;
    unsigned long offset = header->offset;
    s_tag_persistent_identifier key = header->key;
    for (long page_index = 0; page_index < header->page_count;
        offset += ((header->pages[page_index].flags >> 2) + 0xfff) & ~0xfff, page_index++)
    {
        const s_cache_file_tag_resource_file_page *page = &header->pages[page_index];
        long key_index = -1;
        if (page->flags & 1)
        {
            bool found = false;
            const s_tag_persistent_identifier *found_key;
            if (cache_file_tag_resource_file_key_list_find(keys, key, &found_key))
            {
                key_index = found_key - keys->begin();
                found = true;
            }
            if (!found)
                continue;
        }
        {
            long datum_index = datum_new_at_absolute_index(*table, page_index);
            s_cache_file_tag_resource_element *datum = datum_get<s_cache_file_tag_resource_element>(*table, datum_index);
            cache_file_tag_resource_flags_set(&datum->flags, 0x20);
            cache_file_tag_resource_flags_set(&datum->flags, 0x02);
            if (page->flags & 2)
                cache_file_tag_resource_flags_set(&datum->flags, 0x04);
            datum->file_index = (unsigned char)key_index;
            datum->slot = owner;
            datum->global_page_index = base_index != -1 ? page_index + base_index : -1;
            datum->offset = offset;
            datum->compressed_size = page->flags >> 2;
            datum->size = page->size;
            datum->hash = page->location;
            datum->first_handle = 0;
            datum->shared_page_index = -1;
        }
    }
}

// The runtime resources live in three arrays: a fixed array of 88-byte elements followed by two data arrays. An
// absolute index runs over all three; the datum of an element of the fixed array is made from its distance to the end.
// the datum index of an element of the set's fixed array (the same form as 0x21c490 / 0x21c4f8); the caller has
// checked the index against the array
static __forceinline long cache_file_resource_fixed_array_datum_index(const s_cache_file_resource_element_set *set, long index)
{
    const s_cache_file_resource_fixed_array *array = set->first;
    // match: 0x21bfd8: without it an index == -1 early-out (li r3,-1) appears and the array->elements subtraction drops
    __assume(index >= 0 && index < array->count);
    if (index == -1)
        return -1;
    const s_cache_file_resource_fixed_array *current = set->first;
    const char *element = reinterpret_cast<const char *>(current->elements) + (current->count - index) * long(sizeof(s_cache_file_tag_resource_element));
    return (((element - reinterpret_cast<const char *>(array->elements)) / long(sizeof(s_cache_file_tag_resource_element))) << 16) | index;
}
static __forceinline long cache_file_resource_data_array_datum_index(const s_data_array *array, long index)
{
    long result = -1;
    if (index != -1)
        result = (*(unsigned short *)(array->data + array->element_size * index) << 16) | index;
    return result;
}
static __forceinline bool cache_file_resource_index_in_range(long index, long base, long count)
{
    return index - base >= 0 && index - base < count;
}

static __forceinline char *cache_file_resource_data_array_get(const s_data_array *array, long datum_index)
{
    return array->data + DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index) * 88;
}

// 0x21bfd8: the datum index of an absolute index
// >>> 0x21bfd8
extern "C" long cache_file_resource_get_datum_index(const s_cache_file_resource_element_set *set, long index)
{
    long result = -1;
    if (cache_file_resource_index_in_range(index, 0, set->first->count))
    {
        result = cache_file_resource_fixed_array_datum_index(set, index);
    }
    else
    {
        long first_count = set->first->count;
        if (cache_file_resource_index_in_range(index, set->first->count, (*set->second)->actual_count))
        {
            long datum_index = cache_file_resource_data_array_datum_index(*set->second, index - first_count);
            result = (DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index) + first_count) | (datum_index & 0xffff0000);
        }
        else
        {
            long second_count = (*set->second)->maximum_count;
            long third_base = second_count + first_count;
            if (cache_file_resource_index_in_range(index, third_base, (*set->third)->actual_count))
            {
                long datum_index = cache_file_resource_data_array_datum_index(*set->third, index - second_count - first_count);
                result = (DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index) + third_base) | (datum_index & 0xffff0000);
            }
        }
    }
    return result;
}
// <<< 0x21bfd8

// 0x21c388: the element of a datum index
// >>> 0x21c388
extern "C" s_cache_file_tag_resource_element *cache_file_resource_get_element_by_datum(
    const s_cache_file_resource_element_set *set, long datum_index)
{
    char *result = NULL;
    char *data;
    long offset;
    long index = DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index);
    if (cache_file_resource_index_in_range(index, 0, set->first->count))
    {
        data = reinterpret_cast<char *>(set->first->elements);
        offset = index * 88;
    }
    else
    {
        const long &first_count = set->first->count;
        if (cache_file_resource_index_in_range(index, first_count, (*set->second)->actual_count))
        {
            data = (*set->second)->data;
            offset = DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index - first_count) * 88;
        }
        else
        {
            if (cache_file_resource_index_in_range(index, (*set->second)->maximum_count + first_count, (*set->third)->actual_count))
                result = cache_file_resource_data_array_get(*set->third, datum_index - (*set->second)->maximum_count - first_count);
            return (s_cache_file_tag_resource_element *)result;
        }
    }
    result = data + offset;
    return (s_cache_file_tag_resource_element *)result;
}
// <<< 0x21c388

// >>> 0x21c168
// 0x21c168: the page and offset of a datum index's element
extern "C" unsigned long cache_file_resource_get_page_offset(const s_cache_file_resource_element_set *set, long datum_index)
{
    unsigned long result = 0;
    char *data;
    long offset;
    const s_cache_file_tag_resource_element *element;
    long index = DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index);
    if (cache_file_resource_index_in_range(index, 0, set->first->count))
    {
        data = reinterpret_cast<char *>(set->first->elements);
        offset = index * 88;
    }
    else
    {
        const long &first_count = set->first->count;
        if (cache_file_resource_index_in_range(index, first_count, (*set->second)->actual_count))
        {
            data = (*set->second)->data;
            offset = DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index - first_count) * 88;
        }
        else
        {
            if (cache_file_resource_index_in_range(index, (*set->second)->maximum_count + first_count, (*set->third)->actual_count))
            {
                element = (const s_cache_file_tag_resource_element *)cache_file_resource_data_array_get(
                    *set->third, datum_index - (*set->second)->maximum_count - first_count);
                result = ((unsigned long)(unsigned short)element->slot << 20) | ((unsigned long)element->offset >> 12);
            }
            return result;
        }
    }
    element = (const s_cache_file_tag_resource_element *)(data + offset);
    result = ((unsigned long)(unsigned short)element->slot << 20) | ((unsigned long)element->offset >> 12);
    return result;
}
// <<< 0x21c168

// >>> 0x21c278
// 0x21c278: the element of an absolute index
extern "C" s_cache_file_tag_resource_element *cache_file_resource_get_element_by_index(
    const s_cache_file_resource_element_set *set, long index)
{
    char *result = NULL;
    if (cache_file_resource_index_in_range(index, 0, set->first->count))
    {
        result = reinterpret_cast<char *>(set->get_first_element(index));
    }
    else
    {
        long first_count = set->first->count;
        if (cache_file_resource_index_in_range(index, set->first->count, (*set->second)->actual_count))
        {
            result = cache_file_resource_data_array_get(*set->second, cache_file_resource_data_array_datum_index(*set->second, index - first_count));
        }
        else
        {
            long second_count = (*set->second)->maximum_count;
            if (cache_file_resource_index_in_range(index, second_count + first_count, (*set->third)->actual_count))
                result = cache_file_resource_data_array_get(
                    *set->third, cache_file_resource_data_array_datum_index(*set->third, index - second_count - first_count));
        }
    }
    return (s_cache_file_tag_resource_element *)result;
}
// <<< 0x21c278

extern "C" long cache_file_resource_get_first_handle_datum(const s_cache_file_resource_element_set *set, long index)
{
    s_cache_file_resource_fixed_array *array = set->first;
    long first = array->handles[index].first;
    if (first == -1)
        return -1;
    s_cache_file_resource_fixed_array *current = set->first;
    const char *element = reinterpret_cast<const char *>(current->elements) + (current->count - first) * long(sizeof(s_cache_file_tag_resource_element));
    return (((element - reinterpret_cast<const char *>(array->elements)) / long(sizeof(s_cache_file_tag_resource_element))) << 16) | first;
}

// @address 0x21c6e8 cache_file_tag_resource_cache_initialize
void cache_file_tag_resource_cache_initialize(s_cache_file_tag_resource_runtime_cache_storage *storage)
{
    c_tag_resource_cache_new *cache = &storage->m_cache;
    long &runtime_count=cache->m_resize_flags; runtime_count=0;
    cache->m_resize_policy_enabled = 1;
    cache->m_controller.m_can_predict_optional = 1;
    cache->m_controller.m_require_async_page_capacity = 0;
    c_tag_resource_cache_thread_lock_lock_freeish *thread_lock = &cache->m_resource_thread_access_lock;
    thread_lock->m_locked_for_blocking_update = false;
    thread_lock->m_access_lock.setup(k_crit_section_simple_resource_cache_access_rw_lock, k_semaphore_simple_resource_cache_publish_rw_lock);
    thread_lock->m_available_caches.initialize(3);
    thread_lock->m_on_release_events.initialize(3);
    c_cache_file_tag_resource_runtime_thread_access_storage *access = &storage->m_render_access;
    cache->m_allow_optional = true;
    cache->m_incremental_publishing = 1;
    cache->m_resource_spam = false;
    cache->m_use_demand_policy = 1;
    cache->m_per_frame_publishing = false;
    cache->m_unknown30c = 0;
    access->m_access_lock.setup(k_crit_section_resource_threading_access_rw_lock, k_semaphore_resource_threading_access_rw_lock);
    c_cache_file_tag_resource_runtime_thread_access_storage::s_thread_views views;
    views.acquired.data = access->m_acquired_access_storage;
    views.caches.count = NUMBEROF(access->m_access_cache_storage);
    views.caches.data = access->m_access_cache_storage;
    views.acquired.count = NUMBEROF(access->m_acquired_access_storage);
    access->m_views = views;
    for (long *slot = access->m_views.acquired.data, *end = slot + access->m_views.acquired.count; slot != end; slot++)
        *slot = 0;
    for (c_tag_resource_cache_file_access_cache **slot = access->m_views.caches.data, **end = slot + access->m_views.caches.count; slot != end; slot++)
        *slot = NULL;
    access->m_resource_thread_access_lock = NULL;
}

// @address 0x88570 cache_file_tag_resource_requests_try_and_load
extern "C" void cache_file_tag_resource_requests_try_and_load(s_cache_file_tag_resource_slots *slots, int request_index)
{
    if (slots->request_data.requests[request_index].resource == NULL && slots->request_data.requests[request_index].valid != 0)
    {
        int token = slots->request_data.requests[request_index].token;
        bool all_empty = true;
        for (int next = request_index + 1; next < 6; next++)
        {
            if (slots->request_data.requests[next].resource != NULL || (unsigned char)slots->request_data.requests[next].valid != 0)
            {
                all_empty = false;
                break;
            }
        }
        if (all_empty)
        {
            void *resource = slots->loader->allocate_lruv(token);
            if (resource != NULL)
                slots->request_data.requests[request_index].resource = resource;
        }
    }
}

// @address 0x88b10 ?tag_resources_lock_game@@YAJXZ
long tag_resources_lock_game(void)
{
    int result = 0;
    c_cache_file_tag_resource_runtime_manager *storage = g_geometry_globals.manager;
    bool busy = false;
    if (storage->m_cache_file_resource_gestalt_index != -1) {
        busy = storage->m_resource_cache.m_render_access.m_per_thread_acquired_access_cache[get_current_thread_index()] != 0;
    }
    if (!busy) {
        if (storage->m_cache_file_resource_gestalt_index != -1) {
            c_tag_resource_thread_access *sub = &storage->m_resource_cache.m_render_access;
            sub->m_access_lock.read_lock();
            c_tag_resource_cache_file_access_cache **values = sub->m_per_thread_access_cache;
            int slot = get_current_thread_index();
            c_tag_resource_cache_file_access_cache *access_cache = tag_resource_cache_thread_lock_lock_freeish_acquire(sub->m_resource_thread_access_lock);
            values[slot] = access_cache;
            sub->m_per_thread_acquired_access_cache[slot] = 1;
        }
        result = 1;
        scenario_handle_tag_resource_lock(get_current_thread_index());
    }
    return result;
}

// @address 0x88bd8 tag_resource_cache_thread_lock_lock_freeish_acquire
extern "C" c_tag_resource_cache_file_access_cache *tag_resource_cache_thread_lock_lock_freeish_acquire(c_tag_resource_cache_thread_lock_lock_freeish *lock)
{
    c_tag_resource_cache_file_access_cache *result = 0;
    lock->m_access_lock.read_lock();
    internal_critical_section_enter(47);
    c_tag_resource_cache_thread_lock_lock_freeish::c_synchronized_access_cache *cache = lock->m_published_cache;
    __lwsync();
    if (cache) {
        _InterlockedIncrement(&cache->m_reference_count);
        result = (c_tag_resource_cache_file_access_cache *)cache;
    }
    internal_critical_section_leave(47);
    return result;
}

// @address 0x88d50 ?tag_resources_unlock_game@@YAXAAJ@Z
void tag_resources_unlock_game(long & _sym_p0)
{
    if (*((int *)&_sym_p0) == 1) {
        tag_resource_shortcuts_handle_unlock();
        c_cache_file_tag_resource_runtime_manager *storage = g_geometry_globals.manager;
        if (storage->m_cache_file_resource_gestalt_index != -1) {
            c_tag_resource_thread_access *sub = &storage->m_resource_cache.m_render_access;
            s_resource_access_cookie *cookie = sub->unlock_access_for_current_thread_internal(false);
            sub->m_resource_thread_access_lock->release_access_cache((c_tag_resource_cache_file_access_cache *)cookie);
        }
        *((int *)&_sym_p0) = 0;
    }
}

// @address 0x101958 ?get_page_position@c_cache_file_tag_resource_runtime_window_allocator_storage@@UAA_JJ@Z
__int64 c_cache_file_tag_resource_runtime_window_allocator_storage::get_page_position(long handle)
{
    return handle;
}

static __forceinline void cache_file_tag_resource_bit_vector_or(long count, unsigned long *dst, const unsigned long *src)
{
    for (long i = 0; i < (count + 31) >> 5; i++)
        dst[i] |= src[i];
}

// @address 0x220088 cache_file_tag_resource_update_bit_vector
extern "C" void cache_file_tag_resource_update_bit_vector(s_cache_file_bit_vector_target *target, const s_cache_file_zone_manifest *sources)
{
    if (target->bits0)
    {
        cache_file_tag_resource_bit_vector_or(target->count0, target->bits0, (const unsigned long *)sources->required_resources.address);
        if (target->flag)
            cache_file_tag_resource_bit_vector_or(target->count0, target->bits0, (const unsigned long *)sources->streamed_resources.address);
    }
    if (target->bits1)
        cache_file_tag_resource_bit_vector_or(target->count1, target->bits1, (const unsigned long *)sources->active_resource_owners.address);
}

// @address 0x21ff58 cache_file_tag_resource_bit_vector_target_initialize
extern "C" s_cache_file_bit_vector_target *cache_file_tag_resource_bit_vector_target_initialize(
    s_cache_file_bit_vector_target *scratch, long resource_count, void *resource_bits,
    long tag_count, void *tag_bits, unsigned char include_streamed_resources)
{
    scratch->count0 = resource_count;
    scratch->bits0 = (unsigned long *)resource_bits;
    scratch->count1 = tag_count;
    scratch->bits1 = (unsigned long *)tag_bits;
    scratch->flag = include_streamed_resources;
    // Full resource/tag flag words, matching the manager's [1024]/[625] flag arrays.
    if (scratch->bits0)
        memset(scratch->bits0, 0, sizeof(unsigned long) * 1024);
    if (scratch->bits1)
        memset(scratch->bits1, 0, sizeof(unsigned long) * 625);
    return scratch;
}

// @address 0x21ffc8 cache_file_tag_resource_update_masked_bits
extern "C" void cache_file_tag_resource_update_masked_bits(s_cache_file_bit_vector_target *target, const s_tag_block *list, unsigned long mask)
{
    for (long i = 0; i < list->count; i++) {
        if ((1 << i) & mask)
            cache_file_tag_resource_update_bit_vector(target, TAG_BLOCK_GET_ELEMENT(list, i, s_cache_file_zone_manifest));
    }
}

// @address 0x220028 cache_file_tag_resource_update_all_bits
extern "C" void cache_file_tag_resource_update_all_bits(s_cache_file_bit_vector_target *target, const s_tag_block *list)
{
    for (long i = 0; i < list->count; i++) {
        const s_cache_file_zone_manifest *source = TAG_BLOCK_GET_ELEMENT(list, i, s_cache_file_zone_manifest);
        cache_file_tag_resource_update_bit_vector(target, source);
    }
}

// @address 0x220168 ?get_permanent_resource_root_address@c_tag_resource_cache_file_datum_handler@@UAAPAXJ_N@Z
void *c_tag_resource_cache_file_datum_handler::get_permanent_resource_root_address(long resource_handle, bool optional)
{
    c_tag_resource_fixup root_address_fixup = get_root_address_fixup(resource_handle);
    c_basic_buffer<void> control_data = get_control_data(resource_handle, optional);
    // match: 0x220168: naming the control address before the fixup value puts the value first in the add (add r10,r10,r11)
    unsigned int control_address = address_from_pointer(control_data.m_address);
    unsigned int value = root_address_fixup.value & 0x1fffffff;
    void *result;
    switch (root_address_fixup.value >> 29)
    {
    case _tag_resource_fixup_type_value:
        result = pointer_from_address(value);
        break;
    case _tag_resource_fixup_type_data:
        result = pointer_from_address(value + control_address);
        break;
    case _tag_resource_fixup_type_cache_data:
        result = NULL;
        break;
    case _tag_resource_fixup_type_optional_data:
        result = NULL;
        break;
    default:
        // match: 0x220168: drops the cmplwi 3 / bgt bounds check in front of the fixup-type jump table
        __assume(0);
    }
    return result;
}

// @address 0x35944 k_tag_resource_handler_table
#if !defined(_XBOX) || defined(__clang__)
// Codec identifiers of the two tag-resource paging handlers.
static const s_tag_persistent_identifier k_tag_resource_paging_codec_identifier = { { 0x047adbec, 0x4bca4ea4, 0x9fc876f4, 0x520c8518 } };
static const s_tag_persistent_identifier k_tag_resource_paging_secondary_codec_identifier = { { 0x7eda2ab4, 0x80cf4905, 0x9343a8bd, 0xa0c7b36c } };
const s_tag_resource_handler_entry k_tag_resource_handler_table[2] = {
    { &k_tag_resource_paging_codec_identifier, tag_resource_paging_get_task_context },
    { &k_tag_resource_paging_secondary_codec_identifier, tag_resource_paging_get_secondary_context }
};
#endif

// @address 0x220218 ?resolve@s_cache_file_tag_resource_handler_table_resolver@@UAAPAXPBUs_tag_persistent_identifier@@@Z
void *s_cache_file_tag_resource_handler_table_resolver::resolve(const s_tag_persistent_identifier *codec)
{
    void *context = NULL;
    for (unsigned long index = 0; index < 2; index++)
    {
        if (std::equal(codec->parts, codec->parts + 4, k_tag_resource_handler_table[index].key->parts))
            context = k_tag_resource_handler_table[index].get_context();
    }
    return context;
}

extern "C" long tag_resource_fixed_array_get_second_handle(const s_cache_file_resource_element_set *set, long index)
{
    s_cache_file_resource_fixed_array *array = set->first;
    long second = array->handles[index].second;
    if (second == -1)
        return -1;
    s_cache_file_resource_fixed_array *current = set->first;
    const char *element = reinterpret_cast<const char *>(current->elements) + (current->count - second) * long(sizeof(s_cache_file_tag_resource_element));
    return (((element - reinterpret_cast<const char *>(array->elements)) / long(sizeof(s_cache_file_tag_resource_element))) << 16) | second;
}

long c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_required_page_index(long resource_index)
{
    const s_cache_file_resource_gestalt *gestalt = (const s_cache_file_resource_gestalt *)m_header->instances[(short)m_resource_gestalt_index].definition;
    const s_cache_file_tag_resource_data *entry =
        &((const s_cache_file_tag_resource_data *)gestalt->resources.address)[DATUM_INDEX_TO_ABSOLUTE_INDEX(resource_index)];
    return cache_file_resource_get_first_handle_datum(m_element_set, entry->fixup_index);
}

// looks up the entry of a resource and hands its fixup index to the fixed array
long c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::get_optional_page_index(long resource_handle)
{
    const s_cache_file_tag_resource_data *entry = get_entry(resource_handle);
    return tag_resource_fixed_array_get_second_handle(m_element_set, entry->fixup_index);
}

// >>> 0x866c8
// 0x866c8: the entry's first fixup is in the fixed array
bool c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::any_pageable_data(long resource_handle)
{
    const s_cache_file_tag_resource_data *entry = get_entry(resource_handle);
    bool result = (entry->flags & 1) != 0;
    if (result && cache_file_resource_get_first_handle_datum(m_element_set, entry->fixup_index) == -1)
        result = false;
    return result;
}
// <<< 0x866c8

// >>> 0x86918
// 0x86918: the entry's second fixup is in the fixed array
bool c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler::any_optional_data(long resource_handle)
{
    const s_cache_file_tag_resource_data *entry = get_entry(resource_handle);
    bool result = ((entry->flags >> 1) & 1) != 0;
    if (result && tag_resource_fixed_array_get_second_handle(m_element_set, entry->fixup_index) == -1)
        result = false;
    return result;
}
// <<< 0x86918
// 0x1b3508: a page's request priority is its element's page offset
// @address 0x1b3508 ?get_request_priority@c_cache_file_tag_resource_runtime_window_allocator_storage@@UAAKJ@Z
unsigned long c_cache_file_tag_resource_runtime_window_allocator_storage::get_request_priority(long handle)
{
    return cache_file_resource_get_page_offset(m_element_set, handle);
}

// 0x1b3498: copy the element's 64-byte hash pair when its tag is unclaimed and it is marked present
// @address 0x1b3498 ?get_page_hash@c_cache_file_tag_resource_runtime_window_allocator_storage@@UAA_NJPAUs_tag_resource_page_hash_pair@@@Z
bool c_cache_file_tag_resource_runtime_window_allocator_storage::get_page_hash(long handle, s_tag_resource_page_hash_pair *hash)
{
    bool result = false;
    const s_cache_file_tag_resource_element *element = cache_file_resource_get_element_by_datum(m_element_set, handle);
    if ((unsigned short)element->shared_page_index == 0xffff && (element->flags & 1))
    {
        memcpy(hash, element->payload, sizeof(element->payload));
        result = true;
    }
    return result;
}

// 0x1b3a50: remove the third resource range and reset the active resource set.
static __forceinline long cache_file_tag_resource_runtime_23_range_begin(const s_cache_file_resource_element_set *const &elements, long which)
{
    switch (which)
    {
    case 0:
        return 0;
    case 1:
        return elements->first->count;
    default:
        return (*elements->second)->maximum_count + elements->first->count;
    }
}
static __forceinline long cache_file_tag_resource_runtime_23_range_end(const s_cache_file_resource_element_set *const &elements, long which)
{
    switch (which)
    {
    case 0:
        return elements->first->count;
    case 1:
        return (*elements->second)->actual_count + cache_file_tag_resource_runtime_23_range_begin(elements, 1);
    default:
        return (*elements->third)->actual_count + cache_file_tag_resource_runtime_23_range_begin(elements, 2);
    }
}
// @address 0x1b3a50 tag_resource_reset_active_zone
extern "C" void tag_resource_reset_active_zone(c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    const s_cache_file_resource_element_set *set;
    long index;
    c_tag_resource_cache_new *cache;
    c_tag_resource_page_table *table;
    long handle;
    s_data_array **definitions;
    if (manager->m_zone_state.m_active_zone_handle == -1)
        return;
    set = &manager->m_element_set;
    const s_cache_file_resource_element_set *elements = set;
    index = cache_file_tag_resource_runtime_23_range_begin(elements, 2);
    long limit = cache_file_tag_resource_runtime_23_range_end(elements, 2);
    if (index != limit)
    {
        cache = &manager->m_resource_cache.m_cache;
        table = &cache->m_page_table;
        do
        {
            handle = cache_file_resource_get_datum_index(set, index);
            tag_resource_page_table_reset_page(table, handle);
            ::datum_delete(table->m_pages, handle);
            ::datum_delete(cache->m_inverse_page_table.table, handle);
            index = index + 1;
        } while (index != limit);
    }
    definitions = &manager->m_pending_locations;
    data_delete_all(*definitions);
    (*definitions)->valid = 0;
    manager->m_zone_state.m_active_zone_handle = -1;
    manager->m_active_zone_changed = 1;
}

extern "C" void tag_resource_dispose_zone_data(c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    long paged_entry_index = cache_file_tag_resource_find_paged_entry(manager);
    if (paged_entry_index == -1)
        return;
    long zone_handle = cache_file_tag_resource_runtime_handle(manager, paged_entry_index);
    if (zone_handle == -1)
        return;
    const s_cache_file_resource_element_set *set = &manager->m_element_set;
    // match: 0x1b3c18: reading the range start through manager (not set) keeps the second load of set->first for the range end
    long index = manager->m_element_set.first->count;
    long limit = (*set->second)->actual_count + set->first->count;
    if (index != limit)
    {
        c_tag_resource_cache_new *cache = &manager->m_resource_cache.m_cache;
        c_tag_resource_page_table *table = &cache->m_page_table;
        do
        {
            long handle = cache_file_resource_get_datum_index(set, index);
            tag_resource_page_table_reset_page(table, handle);
            ::datum_delete(table->m_pages, handle);
            ::datum_delete(cache->m_inverse_page_table.table, handle);
            index = index + 1;
        } while (index != limit);
    }
    s_data_array **definitions = &manager->m_runtime_locations;
    data_delete_all(*definitions);
    (*definitions)->valid = 0;
}


// 0x21fe50: true when the tag's group is at least as deep under the model ancestors as under proj
// @address 0x21fe50 tag_resource_is_model_family_tag
extern "C" unsigned char tag_resource_is_model_family_tag(long tag_index)
{
    bool result = true;
    unsigned long model_tags[8];
    unsigned long proj_tag;
    int group_tag = (int)tag_get_group_tag(tag_index);
    int model_depth;
    {
        s_cache_file_ancestor_list ancestors;
        s_cache_file_ancestor_list *plist = &ancestors;
        plist->tags = model_tags;
        plist->count = 8;
        model_tags[0] = 'mode';
        model_tags[1] = 'obje';
        model_tags[2] = 'foot';
        model_tags[3] = 'sbsp';
        model_tags[4] = 'scnr';
        model_tags[5] = 'rmop';
        model_tags[6] = 'chdt';
        model_tags[7] = 'char';
        model_depth = tag_group_get_min_ancestor_depth(group_tag, *plist);
    }
    {
        s_cache_file_ancestor_list ancestors;
        s_cache_file_ancestor_list *plist = &ancestors;
        plist->count = 1;
        plist->tags = &proj_tag;
        proj_tag = 'proj';
        if (model_depth < tag_group_get_min_ancestor_depth(group_tag, *plist))
            result = false;
    }
    return result;
}
// 0x1afe48: find the next valid resource element in the owner's set
// @address 0x1afe48 ?next_tracked_page_handle@s_cache_file_tag_resource_set_iterator@@UAA_NPAJ0@Z
bool s_cache_file_tag_resource_set_iterator::next_tracked_page_handle(long *out_handle, long *out_first_index)
{
    bool found = false;
    do
    {
        if (m_index >= m_set->first->count + (*m_set->second)->maximum_count + (*m_set->third)->actual_count)
            return found;
        if (levels_is_index_in_range(m_set, m_index) != 0)
        {
            const s_cache_file_tag_resource_element *element = cache_file_resource_get_element_by_index(m_set, m_index);
            if ((unsigned long)element->size > 0)
            {
                *out_handle = cache_file_resource_get_datum_index(m_set, m_index);
                if (out_first_index != 0)
                    *out_first_index = element->first_handle;
                found = true;
            }
        }
        m_index = m_index + 1;
    } while (!found);
    return found;
}
// @address 0x1b1ac8 ?add_element_relocation@s_cache_file_tag_resource_page_visitor@@UAAXJV?$c_basic_buffer@X@@@Z
void s_cache_file_tag_resource_page_visitor::add_element_relocation(long datum_index, c_basic_buffer<void> buffer)
{
    const s_cache_file_tag_resource_element *element =
        cache_file_resource_get_element_by_datum(&cache_file_tag_resource_elements(m_manager), datum_index);
    short slot = element->slot;
    long handle = cache_file_tag_resource_runtime_handle(m_manager, slot);
    if (handle != -1)
    {
        s_cache_file_runtime_definition *definition = cache_file_tag_resource_get_slot_definition(m_manager, slot);
        cache_file_tag_resource_add_relocation(&cache_file_tag_resource_relocations(m_manager), definition, element, buffer);
    }
}

// 0x1b3b40: when the given path has a new index, bind its page table and mark its element range
// @address 0x1b3b40 tag_resource_set_active_map_file
extern "C" void tag_resource_set_active_map_file(c_cache_file_tag_resource_runtime_manager_storage *manager, long index, const char *path)
{
    if (index != -1)
    {
        cache_file_tag_resource_active_handle(manager) = index;
        char *header = game_state_xenon_try_and_get_data(path);
        const c_typed_tag_block<s_tag_persistent_identifier> *keys = cache_file_tag_resource_get_play_list_keys(manager);
        cache_file_tag_resource_page_table_initialize(&manager->m_pending_locations,
            (const s_cache_file_tag_resource_file_header *)header, keys, -2, 0);
        const s_cache_file_resource_element_set *elements = &cache_file_tag_resource_elements(manager);
        long datum = cache_file_tag_resource_runtime_23_range_begin(elements, 2);
        long limit = cache_file_tag_resource_runtime_23_range_end(elements, 2);
        if (datum != limit)
        {
            do
            {
                long datum_index = cache_file_resource_get_datum_index(elements, datum);
                tag_resource_cache_track_element((tag_resource_external_access_set_object *)&cache_file_tag_resource_cache(manager), datum_index, 0);
                datum = datum + 1;
            } while (datum != limit);
        }
        cache_file_tag_resource_pending_map(manager) = 1;
    }
}

// 0x1b3828: switches the active map file to the pending one once the page table is idle

static __forceinline bool cache_file_tag_resource_map_file_index_valid(long index)
{
    return index >= 0 && index < 15;
}
static __forceinline bool cache_file_tag_resource_map_file_valid(const char *path)
{
    bool result = false;
    long index = game_state_xenon_find_file_by_path(path);
    if (index != -1 && cache_file_tag_resource_map_file_index_valid(index))
        result = true;
    return result;
}
// @address 0x1b3828 tag_resource_update_active_map_file
extern "C" void tag_resource_update_active_map_file(c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    if (!manager->m_zone_names.m_second.m_name.is_empty())
    {
        if (strncmp(manager->m_zone_names.m_first.m_name.get_string(),
            manager->m_zone_names.m_second.m_name.get_string(), manager->m_zone_names.m_second.m_name.max_length()) != 0)
        {
            manager->m_resource_cache.m_cache.m_page_table.m_include_unreferenced = false;
            if (tag_resource_page_table_is_idle(&manager->m_resource_cache.m_cache.m_page_table))
                tag_resource_reset_active_zone(manager);
            if (cache_file_tag_resource_active_handle(manager) != -1 || cache_file_tag_resource_pending_map(manager))
                return;
            long handle = manager->m_zone_names.m_zone_set_index;
            if (handle != -1)
            {
                if (datum_get<s_cache_file_runtime_definition>(cache_file_tag_resource_definitions(manager), handle)->flags & 1)
                    xenon_game_state_globals.pending_file_index = -1;
                datum_delete((s_data_array *)cache_file_tag_resource_definitions(manager), handle);
                manager->m_zone_names.m_zone_set_index = -1;
                manager->m_zone_names.m_first.m_name.clear();
            }
            if (manager->m_zone_names.m_zone_set_index != -1
                || !cached_map_file_exists(manager->m_zone_names.m_second.m_name.get_string()))
                return;
            long index = cache_file_tag_resource_find_handle_by_path(manager, manager->m_zone_names.m_second.m_name.get_string());
            if (index == -1)
            {
                long new_index = -1;
                if (cache_file_tag_resource_map_file_valid(manager->m_zone_names.m_second.m_name.get_string()))
                {
                    new_index = datum_new_at_absolute_index((s_data_array *)cache_file_tag_resource_definitions(manager), 4);
                    cache_file_tag_resource_definition_initialize((s_cache_file_runtime_handle_set *)manager, new_index,
                        manager->m_zone_names.m_second.m_name.get_string(), NULL, true);
                }
                index = new_index;
                if (new_index != -1)
                    manager->m_zone_names.m_zone_set_index = new_index;
            }
            tag_resource_set_active_map_file(manager, index,
                manager->m_zone_names.m_second.m_name.get_string());
            manager->m_zone_names.m_first = manager->m_zone_names.m_second;
            manager->m_zone_names.m_second.m_name.clear();
        }
        else if (manager->m_zone_names.m_first.m_value != manager->m_zone_names.m_second.m_value)
        {
            manager->m_resource_cache.m_cache.m_page_table.m_include_unreferenced = false;
            if (tag_resource_page_table_is_idle(&manager->m_resource_cache.m_cache.m_page_table))
            {
                manager->m_zone_names.m_first = manager->m_zone_names.m_second;
                manager->m_zone_names.m_second.m_name.clear();
                cache_file_tag_resource_pending_map(manager) = true;
            }
        }
    }
}

static __forceinline s_cache_file_runtime_definition *cache_file_tag_resource_slot_definition(
    c_cache_file_tag_resource_runtime_manager_storage *manager, short slot)
{
    int handle = cache_file_tag_resource_runtime_handle(manager, slot);
    return datum_get<s_cache_file_runtime_definition>(cache_file_tag_resource_definitions(manager), handle);
}

bool c_cache_file_tag_resource_runtime_manager_storage::get_location(unsigned __int64 datum_index, s_indirect_cache_file_location *location)
{
    const s_cache_file_tag_resource_element *element = cache_file_resource_get_element_by_datum(
        m_window_allocator.m_element_set, (long)datum_index);
    location->file_index = cache_file_tag_resource_slot_definition(this, element->slot)->io_file_index;
    location->offset = cache_file_tag_resource_slot_definition(this, element->slot)->file_offset + element->offset;
    location->size = element->compressed_size;
    return true;
}

bool c_cache_file_tag_resource_runtime_manager_storage::page_is_necessary(long resource_index)
{
    bool result = false;
    if (m_zone_state.m_active_zone_handle != -1) {
        s_cache_file_tag_resource_element *element = cache_file_resource_get_element_by_datum(
            &m_element_set, resource_index);
        if (element->slot != -1 && (element->flags & 2)) {
            if (!(element->flags & 4) || m_streaming_enabled) {
                s_cache_file_tag_resource_file_header *file
                    = (s_cache_file_tag_resource_file_header *)game_state_xenon_try_and_get_data(
                    m_zone_names.m_first.m_name.get_string());
                bool available = false;
                if (file) {
                    long zone_index = m_zone_names.m_first.m_value;
                    if (zone_index == -1) zone_index = 0;
                    if (zone_index >= 0 && zone_index < file->zone_count) {
                        const s_cache_file_insertion_point_resource_usage *usage = &file->insertion_point_usages[zone_index];
                        const c_typed_tag_block<s_cache_file_resource_shared_file> *play_list = cache_file_tag_resource_get_play_list_strings(
                            this);
                        if ((unsigned long)element->slot < (unsigned long)play_list->count() &&
                            (element->global_page_index != -1 && element->global_page_index >= 0 && element->global_page_index < file->resource_count &&
                                bit_vector_test(usage->shared_required_locations, element->global_page_index)))
                            available = true;
                        else if (element->slot == -2 &&
                            (element->global_page_index != -1 && element->global_page_index >= 0 && element->global_page_index < file->page_count &&
                                bit_vector_test(usage->local_required_locations, element->global_page_index)))
                            available = true;
                    }
                } else {
                    if (cache_file_tag_resource_runtime_handle(this, element->slot)
                        == m_zone_state.m_active_zone_handle)
                        available = true;
                }
                if (available)
                    result = (!(element->flags & 4) && !(element->flags & 8)) || m_streaming_enabled;
            }
        }
    }
    return result;
}

// 0x1b13f8: advance to the next element whose page location still matches its owning file
// @address 0x1b13f8 ?next_matching_page@s_cache_file_tag_resource_page_reader@@UAA_NPAJPAV?$c_basic_buffer@X@@PA_N@Z
bool s_cache_file_tag_resource_page_reader::next_matching_page(
    long *out_handle, c_basic_buffer<void> *buffer, bool *out_pending)
{
    bool found = false;
    do
    {
        c_cache_file_tag_resource_runtime_manager_storage *manager = m_manager;
        const s_cache_file_resource_element_set *elements = &cache_file_tag_resource_elements(manager);
        if (m_index >= elements->first->count + (*elements->second)->maximum_count + (*elements->third)->actual_count)
            break;
        if (levels_is_index_in_range(elements, m_index))
        {
            const s_cache_file_tag_resource_element *element = cache_file_resource_get_element_by_index(elements, m_index);
            if (cache_file_tag_resource_runtime_handle(manager, element->slot) != -1)
            {
                s_cache_file_runtime_definition *definition = cache_file_tag_resource_slot_definition(manager, element->slot);
                if (cache_file_tag_resource_try_and_get_relocation(&cache_file_tag_resource_relocations(manager),
                        definition, element, buffer))
                {
                    long handle = cache_file_resource_get_datum_index(
                        &cache_file_tag_resource_elements(m_manager), m_index);
                    const s_cache_file_tag_resource_file_header *header = definition->tag_data;
                    bool valid = true;
                    if (header)
                    {
                        const c_typed_tag_block<s_cache_file_resource_shared_file> *names =
                            cache_file_tag_resource_get_play_list_strings(m_manager);
                        long base;
                        if ((unsigned long)element->slot < (unsigned long)names->count())
                            base = -(*names)[element->slot].global_page_offset;
                        else
                            base = 0;
                        long page_index = element->global_page_index + base;
                        if (page_index == -1 || page_index < 0 || page_index >= header->page_count ||
                            memcmp(&element->hash, &header->pages[page_index].location, sizeof(element->hash)) != 0)
                            valid = false;
                    }
                    if (valid)
                    {
                        found = true;
                        *out_handle = handle;
                    }
                }
            }
        }
        m_index++;
    } while (!found);
    if (found)
        *out_pending = false;
    return found;
}

// @address 0x1b1c80 tag_resource_load_playlist_entry
extern "C" bool tag_resource_load_playlist_entry(c_cache_file_tag_resource_runtime_manager_storage *manager, long index)
{
    bool result = false;
    c_typed_tag_block<s_cache_file_resource_shared_file> *block = cache_file_tag_resource_get_play_list_strings(manager);
    const s_cache_file_resource_shared_file *entry = &(*block)[index];
    if (cache_file_tag_resource_map_file_valid(entry->name))
    {
        unsigned char has_pages = (entry->flags >> 2) & 1;
        if (has_pages)
        {
            char *header = game_state_xenon_try_and_get_data(entry->name);
            const c_typed_tag_block<s_tag_persistent_identifier> *keys = cache_file_tag_resource_get_play_list_keys(manager);
            cache_file_tag_resource_page_table_initialize(&manager->m_runtime_locations,
                (const s_cache_file_tag_resource_file_header *)header, keys, index, entry->global_page_offset);
            long datum = cache_file_tag_resource_runtime_23_range_begin(&cache_file_tag_resource_elements(manager), 1);
            long limit = cache_file_tag_resource_runtime_23_range_end(&cache_file_tag_resource_elements(manager), 1);
            if (datum != limit)
            {
                do
                {
                    long datum_index = cache_file_resource_get_datum_index(&cache_file_tag_resource_elements(manager), datum);
                    tag_resource_cache_track_element((tag_resource_external_access_set_object *)&cache_file_tag_resource_cache(manager), datum_index, 0);
                    datum = datum + 1;
                } while (datum != limit);
            }
        }
        int slot = cache_file_tag_resource_runtime_slot(index);
        do
        {
            if (cache_file_tag_resource_runtime_handle(manager, index) != -1)
                break;
            long handle = datum_new_at_absolute_index((s_data_array *)cache_file_tag_resource_definitions(manager), slot);
            if (handle != -1)
            {
                cache_file_tag_resource_definition_initialize((s_cache_file_runtime_handle_set *)manager, handle, entry->name, entry, false);
                result = true;
                cache_file_tag_resource_handles(manager)[slot] = handle;
            }
        } while (false);
    }
    return result;
}

// 0x1b1f80: reset the map-file definitions, bind the active game-state file as the default definition and load the
// play-list entries that are not deferred
static __forceinline bool cache_file_tag_resource_active_game_state_file_valid()
{
    long file_index = xenon_game_state_globals.active_file_index;
    return file_index >= 0 && (file_index >= 0 ? file_index < 15 : false);
}
// @address 0x1b1f80 tag_resource_initialize_map_definitions
extern "C" void tag_resource_initialize_map_definitions(c_cache_file_tag_resource_runtime_manager_storage *manager, long game_mode)
{
    s_data_array *definitions = cache_file_tag_resource_definitions(manager);
    definitions->valid = true;
    data_delete_all((s_data_array *)definitions);
    c_typed_tag_block<s_cache_file_resource_shared_file> *block = cache_file_tag_resource_get_play_list_strings(manager);
    cache_file_tag_resource_handles(manager).m_count = block->count() + 1;
    s_cache_file_runtime_handle_stack *handles = &cache_file_tag_resource_handles(manager);
    std::fill(handles->begin(), handles->end(), (unsigned long)-1);

    long index = datum_new_at_absolute_index((s_data_array *)cache_file_tag_resource_definitions(manager), 0);
    s_cache_file_runtime_definition *definition
        = datum_get<s_cache_file_runtime_definition>(cache_file_tag_resource_definitions(manager), index);
    bool bound = false;
    if (cache_file_tag_resource_active_game_state_file_valid())
    {
        definition->io_file_index = xenon_game_state_globals.files[xenon_game_state_globals.active_file_index].io_file_index;
        bound = true;
    }
    if (bound && cache_file_tag_resource_active_game_state_file_valid())
        definition->file_offset = xenon_game_state_globals.files[xenon_game_state_globals.active_file_index].header.map_file_offsets[1];
    if (cache_file_tag_resource_active_game_state_file_valid())
        definition->file_handle = xenon_game_state_globals.files[xenon_game_state_globals.active_file_index].file_handle;
    if (cache_file_tag_resource_active_game_state_file_valid())
        definition->overlapped_handle = xenon_game_state_globals.files[xenon_game_state_globals.active_file_index].overlapped_handle;
    char *tag_data = NULL;
    if (cache_file_tag_resource_active_game_state_file_valid())
    {
        s_xenon_game_state_file *file = &xenon_game_state_globals.files[xenon_game_state_globals.active_file_index];
        if (file->header.saved_game_data)
            tag_data = (char *)file->header.trailing_fields;
    }
    definition->tag_data = (s_cache_file_tag_resource_file_header *)tag_data;
    cache_file_tag_resource_handles(manager)[0] = index;

    for (long i = 0; i < block->count(); i++)
    {
        unsigned char deferred = ((*block)[i].flags >> 1) & 1;
        if (!deferred)
            tag_resource_load_playlist_entry(manager, i);
    }
}

// 0x1b21e0: load the play-list entries that are deferred (skipping slots that already have handles)
// @address 0x1b21e0 tag_resource_load_deferred_entries
extern "C" void tag_resource_load_deferred_entries(c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    c_typed_tag_block<s_cache_file_resource_shared_file> *block = cache_file_tag_resource_get_play_list_strings(manager);
    long i = 0;
    if (block->count() > 0) {
        do {
            unsigned char deferred = ((*block)[i].flags >> 1) & 1;
            if (deferred) {
                if (cache_file_tag_resource_runtime_handle(manager, i) == -1)
                    tag_resource_load_playlist_entry(manager, i);
            }
            i++;
        } while (i < block->count());
    }
}

static __forceinline bool cache_file_tag_resource_external_access_dirty(const s_cache_file_tag_resource_external_access_flush_view *state)
{
    return state->state.key_list_overflow || state->state.group_overflow
        || state->state.value_overflow || state->state.slot_overflow;
}
static __forceinline s_data_array *cache_file_tag_resource_page_blocks(const s_lruv_cache *cache)
{ return (s_data_array *)cache->header.block_data; }
static __forceinline bool cache_file_tag_resource_request_expired(const s_cache_file_tag_resource_requests *data, long index)
{
    if (data->requests[index].resource && !data->requests[index].external)
    {
        if (!data->requests[index].valid || (unsigned long)(data->current_timestamp - data->requests[index].timestamp) > 2)
            return true;
    }
    return false;
}
// @address 0x1b24b0 tag_resource_update
extern "C" void tag_resource_update(c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    if (manager->m_cache_file_resource_gestalt_index != -1)
    {
        manager->update_bulk_reader();
        tag_resource_cache_new_update(&manager->m_resource_cache.m_cache);
        s_cache_file_tag_resource_external_access_flush_view *external_access
            = manager->m_listener_link.m_flush_view;
        if (external_access && cache_file_tag_resource_external_access_dirty(external_access))
        {
            unsigned long current_time = GetTickCount();
            if (current_time - external_access->last_flush_time >= 5000)
            {
                external_access->last_flush_time = GetTickCount();
                tag_resource_cache_reset_state(&external_access->state);
            }
        }
        tag_resource_load_deferred_entries(manager);
        tag_resource_update_active_map_file(manager);
    }
    s_cache_file_tag_resource_slots *slots = &manager->m_file_slots;
    if (slots->m_reclaim_expired)
    {
        for (long slot_index = 0; slot_index < 6; slot_index++)
        {
            if (cache_file_tag_resource_request_expired(&slots->request_data, slot_index))
                tag_resource_cache_free_block(slots, slot_index, slots->request_data.requests[slot_index].resource);
        }
    }
    slots->request_data.current_timestamp++;
    if (cache_file_tag_resource_buffer_end(manager->m_requested_buffer) > cache_file_tag_resource_buffer_end(manager->m_result_buffer)
        && (char *)manager->m_window.m_address > cache_file_tag_resource_buffer_end(manager->m_result_buffer))
    {
        c_tag_resource_page_table::c_tag_resource_lruv_cache &lruv = manager->m_resource_cache.m_cache.m_page_table.m_lruv_cache;
        lruv.m_allocator->get_window(&lruv.m_window);
        s_lruv_cache *target = lruv.m_cache;
        long used_page_bytes = -1;
        target->maximum_page_count = (unsigned long)lruv.m_window.m_size >> lruv.m_alignment_bits;
        if (target->maximum_page_count && cache_file_tag_resource_page_blocks(target))
            used_page_bytes = ((unsigned long)target->maximum_page_count / cache_file_tag_resource_page_blocks(target)->maximum_count) << 3;
        target->pages_per_block_ratio = used_page_bytes;
    }
}

// @address 0x1af400 tag_resource_load_element_data
extern "C" bool tag_resource_load_element_data(s_cache_file_tag_resource_page_request *request,
    s_cache_file_tag_resource_page_state *state, s_cache_file_runtime_definition *definition, s_cache_file_tag_resource_element *element,
    const s_indirect_cache_file_read_request *query, c_synchronized_long *bytes_read, c_asynchronous_io_marker *marker)
{
    request->marker = marker;
    long slot = query->hash_source_index;
    request->slot = slot;
    c_sound_cache_hash_source *source;
    if (slot == -1)
    {
        s_cache_file_tag_resource_stream_storage *stream_storage = &state->stream;
        char *memory = stream_storage->memory;
        s_cache_file_tag_resource_memory_stream *stream = (s_cache_file_tag_resource_memory_stream *)new (
            align_pointer(memory, 2)) s_cache_file_tag_resource_memory_stream();
        stream_storage->object = stream;
        source = (c_sound_cache_hash_source *)stream;
    }
    else if ((slot & (long)0x80000000) == 0)
    {
        source = state->sources[slot]->get_hash_source();
    }
    else
    {
        const s_cache_file_tag_resource_cursor_source_slot *cursor_sources = state->cursor_sources.begin();
        source = (c_sound_cache_hash_source *)cache_file_tag_resource_cursor_new(&state->cursor, &cursor_sources[slot & 0x7fffffff].source);
    }
    request->source = source;
    state->busy = true;
    request->buffer = state->buffer;
    request->success = false;
    request->ready = 0;
    request->progress.set(0);
    const s_gui_custom_bitmap_storage_hash *expected_hash = query->validate_hash ? &query->expected_hash : NULL;
    c_basic_buffer<void> destination(query->buffer, 0, query->uncompressed_size);
    request->id = gui_custom_bitmap_storage_queue_async_read(
        (e_async_priority)4,
        (void *)definition->overlapped_handle,
        (definition->file_offset + element->offset + query->offset),
        query->size,
        expected_hash,
        request->buffer,
        request->source,
        destination,
        &request->success,
        &request->progress,
        bytes_read,
        &request->done);
    if (request->id != -1)
    {
        c_asynchronous_io_marker *marker_arg = request->marker;
        __lwsync();
        _InterlockedExchange(&marker_arg->io_status, 2);
    }
    else
        cache_file_bulk_reader_complete_request(request, state);
    return request->id != -1;
}
bool c_cache_file_tag_resource_runtime_manager_storage::request_data_async(const s_indirect_cache_file_read_request *query,
    c_synchronized_long *bytes_read_counter, c_asynchronous_io_marker *bulk_reader_marker)
{
#if defined(_XBOX) && !defined(__clang__)
    bool result;
#else
    bool result = false;
#endif
    if (m_page_cursor.id != -1 || m_page_storage.busy)
        return result;
    long slot = query->hash_source_index;
    s_cache_file_tag_resource_page_state *state = &m_page_storage;
    bool ready;
    if (slot == -1)
        ready = state->stream.object == 0;
    else if ((slot & (long)0x80000000) == 0)
        ready = state->sources[slot]->is_empty();
    else
        ready = state->cursor.object == 0;
    if (!ready)
        return result;
    s_cache_file_tag_resource_element *element = cache_file_resource_get_element_by_datum(
        m_window_allocator.m_element_set, (long)query->position);
    result = tag_resource_load_element_data(&m_page_cursor, state,
        cache_file_tag_resource_get_slot_definition(this, element->slot),
        element, query,
        bytes_read_counter, bulk_reader_marker);
    return result;
}

// @address 0x75deb0 g_asynchronous_io_arena

static __forceinline void cache_file_tag_resource_runtime_file_reference_create(s_file_reference *reference, const char *name)
{
    memset(reference, 0, sizeof(s_file_reference));
    reference->signature = 'filo';
    reference->location = -1;
    reference->handle = (void *)-1;
    file_reference_set_name(reference, name);
}

// @address 0x21a688 asynchronous_io_arena_open_file
// the arena is always g_asynchronous_io_arena
extern "C" bool asynchronous_io_arena_open_file(c_asynchronous_io_arena *arena, long file_index, long flags, const char *file_name)
{
    arena = &g_asynchronous_io_arena;
    s_file_reference file;
    cache_file_tag_resource_runtime_file_reference_create(&file, file_name);
    c_static_string<256> full_path;
    file_location_get_full_path(file.location, file.path, full_path.get_buffer(), 256);
    void *handle = CreateFileA(full_path.get_string(), 0x80000000, 3, 0, 3, 0x70000000, 0);
    if (handle != (void *)-1) {
        CreateIoCompletionPort(handle, arena->completion_port, file_index, 1);
        arena->files[file_index].handle = handle;
        arena->files[file_index].outstanding_io_count = 0;
        arena->valid_files.set(file_index, true);
        return true;
    }
    c_static_string<256> error;
    error.print("failed to open '%s', GetLastError()==%d", full_path.get_string(), GetLastError());
    main_halt_and_display_errors();
}

// @address 0x21abb8 asynchronous_io_arena_handle_bulk_reads

// @address 0x3e46c ??_7c_cache_file_tag_resource_bulk_reader@@6B@

extern "C" void asynchronous_io_arena_handle_bulk_reads(c_asynchronous_io_arena *p0, c_indirect_cache_file_location_atlas *p1,
    s_cache_file_tag_resource_page_state *p2, c_indirect_cache_file_bulk_read_iterator *p3, c_basic_buffer<void> p4)
{
    unsigned long total_bytes;
    int out_100;
    c_basic_buffer<void> read_buffer(c_basic_buffer<void>::_uninitialized);
    s_indirect_cache_file_read_request req;
    char pad_260[12];
    s_sound_cache_hash_state hash_state;
    char pad_417[111];

    p3->reset();
    total_bytes = 0;
    req.buffer.m_address = 0;
    req.buffer.m_size = 0;
    bool cont = p3->next(&req, &out_100);
    if (!cont)
        return;
    unsigned long magic = 0x1fffffff;
    do {
        long result = 5;
        if (req.hash_source_index == -1) {
            unsigned long count = (req.size + magic) >> 29;
            unsigned long i = 0;
            if (count != 0) {
                do {
                    result = indirect_cache_file_read_blocking(p0, p1, p3, &req, (i & 7) << 29, 0x20000000, 0, 0, &total_bytes);
                    i++;
                } while (i < count);
            }
        } else {
            bool ready = false;
            result = 3;
            read_buffer.m_address = req.buffer.m_address;
            read_buffer.m_size = req.uncompressed_size;
            c_sound_cache_hash_source *source = (c_sound_cache_hash_source *)p2->create(req.position, req.hash_source_index, read_buffer);
            c_cache_file_tag_resource_bulk_reader reader;
            int z = 1;
            c_asynchronous_io_marker *marker = reader.m_markers;
            do {
                z--;
                marker->io_status = 0;
                ++marker;
            } while (z >= 0);
            reader.m_io_arena = p0;
            reader.m_atlas = p1;
            reader.m_iterator = p3;
            reader.m_request = &req;
            unsigned long *total_ptr = &total_bytes;
            reader.m_pending_requests = 0;
            reader.m_total_bytes = total_ptr;
            cache_file_bulk_reader_initialize_hash_state(&hash_state, req.size, p4, source, req.validate_hash);
            while (true) {
                bool done = (hash_state.read_failed != 0) || (hash_state.read_completed != 0);
                if (done)
                    break;
                if (hash_state.read_failed == 0)
                    cache_file_bulk_reader_update_reads(&hash_state, &reader);
                if (hash_state.read_failed == 0 && hash_state.read_completed == 0)
                    cache_file_bulk_reader_update_hash(&hash_state);
                if (hash_state.read_failed != 0)
                    cache_file_bulk_reader_finish_pending_reads(&hash_state, &reader);
            }
            if (hash_state.read_completed != 0) {
                if (req.validate_hash == 0 || cache_file_bulk_reader_test_expected_hash(&hash_state, &req.expected_hash))
                    ready = true;
                else
                    result = 4;
            }
            if (ready)
                result = 0;
            p2->release(req.position, req.hash_source_index, source);
        }
        p3->finished(out_100, result);
        cont = p3->next(&req, &out_100);
    } while (cont);
}
bool c_cache_file_tag_resource_runtime_manager_storage::request_data(c_indirect_cache_file_bulk_read_iterator *iterator)
{
    s_cache_file_tag_resource_page_state *state = &m_page_storage;
    state->busy = true;
    c_indirect_cache_file_location_atlas *atlas = this;
    asynchronous_io_arena_handle_bulk_reads(&g_asynchronous_io_arena, atlas, state, iterator, state->buffer);
    m_page_storage.busy = false;
    return true;
}

bool c_cache_file_tag_resource_runtime_manager_storage::resize(c_basic_buffer<void> *result, unsigned long minimum_size, unsigned long desired_size, long flags)
{
    bool success = false;
    if (minimum_size <= m_buffer.m_size)
    {
        unsigned long available = static_cast<char *>(m_window.m_address) - static_cast<char *>(m_buffer.m_address);
        while (available < minimum_size)
        {
            unsigned long previous_available = available;
            cache_file_tag_resource_slots_release_last(&m_file_slots);
            available = static_cast<char *>(m_window.m_address) - static_cast<char *>(m_buffer.m_address);
            if (previous_available == available)
                break;
        }
        if (minimum_size <= available)
        {
            m_requested_buffer = cache_file_tag_resource_buffer_get_range(
                m_buffer, 0, desired_size > m_buffer.m_size ? m_buffer.m_size : desired_size);
            m_result_buffer = cache_file_tag_resource_buffer_get_range(m_buffer, 0, desired_size > available ? available : desired_size);
            *result = m_result_buffer;
            success = true;
        }
    }
    return success;
}

void c_cache_file_tag_resource_runtime_manager_storage::get_window(s_cache_file_tag_resource_window *out)
{
    char *end = static_cast<char *>(m_window.m_address);
    if (m_requested_window.get_end() <= end)
        end = m_requested_window.get_end();
    s_cache_file_tag_resource_window window;
    window.m_start = static_cast<char *>(m_buffer.m_address);
    window.m_size = end - window.m_start;
    m_result_window = window;
    *out = window;
}

void c_cache_file_tag_resource_runtime_manager_storage::set_locked_window(c_basic_buffer<void> window)
{
    m_locked_window = window;
    m_map_initialized = false;
}

void c_cache_file_tag_resource_runtime_manager_storage::lock_window()
{
    m_map_initialized = true;
}

void c_cache_file_tag_resource_runtime_manager_storage::reset_window(s_cache_file_tag_resource_window *out)
{
    s_cache_file_tag_resource_window window;
    char *begin = static_cast<char *>(m_buffer.m_address);
    char *end = static_cast<char *>(m_buffer.m_address);
    window.m_start = begin;
    window.m_size = begin - end;
    m_result_window = window;
    m_requested_window = window;
    *out = window;
}



void c_cache_file_tag_resource_runtime_manager_storage::update_io_state()
{
    update_bulk_reader();
}

void c_cache_file_tag_resource_runtime_manager_storage::wait_for_io()
{
    main_update_idle_tasks();
}

// @address 0x1b3fd8 ?commit_active_zones@c_cache_file_tag_resource_runtime_manager@@QAAXXZ
void c_cache_file_tag_resource_runtime_manager::commit_active_zones()
{
    unsigned char changed[16];
    long buf_29f0[628];
    long buf_2020[1024];
    long buf_1020[1024];
    bool any_init = m_required_resource_state_changed || m_active_resource_state_changed || m_active_zone_changed;
    bool any = any_init;
    if (m_required_resource_state_changed)
    {
        cache_file_tag_resource_compute_required_flags(this, buf_2020, buf_29f0);
        memcpy(m_required_resource_flags, buf_2020, sizeof(buf_2020));
        memcpy(m_active_tag_flags, buf_29f0, sizeof(m_active_tag_flags));
        m_required_resource_state_changed = false;
    }
    if (m_active_resource_state_changed)
    {
        cache_file_tag_resource_compute_zone_flags(this, buf_1020);
        memcpy(m_active_resource_flags, buf_1020, sizeof(m_active_resource_flags));
        m_active_resource_state_changed = false;
    }
    if (any)
    {
        c_tag_resource_cache_new *page = &m_resource_cache.m_cache;
        changed[0] = 0;
        tag_resource_cache_update_resources_from_active_set(&page->m_controller, (void *)page->m_loaded_query, &page->m_resource_address_cache, &changed[0]);
        if (page->m_page_filter)
            tag_resource_page_table_update_required_pages(&page->m_page_table, page->m_page_filter);
        bool valid = page->m_incremental_publishing == 0 || changed[0] != 0;
        page->m_resources_valid = valid;
        page->m_resources_changed = 1;
        c_tag_resource_page_table *table = &page->m_page_table;
        table->m_pages_iterator.begin(table->m_pages);
        table->m_async_requests_idle = false;
    }
}
// @address 0x1b3270 tag_resource_iterate_dependencies
extern "C" void tag_resource_iterate_dependencies(s_cache_file_tag_dependency_visitor *visitor, const s_cache_file_tag_dependency *dependency)
{
    long tag_index = dependency->tag.index;
    if (tag_index != -1 && tag_try_and_get_unsafe(dependency->tag.group_tag, tag_index) &&
        (visitor->root_tag_index == tag_index || tag_resource_is_model_family_tag(tag_index)))
    {
        long absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(dependency->tag.index);
        if (!bit_vector_test(visitor->visited, absolute_index))
        {
            bit_vector_set(visitor->visited, absolute_index);
            visitor->depth++;
            for (long i = 0; i < dependency->child_indices.count(); i++)
            {
                long child_index = dependency->child_indices[i];
                tag_resource_iterate_dependencies(visitor, &visitor->graph->dependencies[child_index]);
            }
            visitor->depth--;
        }
    }
}

static __forceinline bool cache_file_tag_dependency_less(const s_cache_file_tag_dependency &dependency, long tag_index)
{
    return dependency.tag.index < tag_index;
}
static __forceinline const s_cache_file_tag_dependency *cache_file_tag_dependency_lower_bound(
    const s_cache_file_tag_dependency *first, const s_cache_file_tag_dependency *last, long tag_index)
{
    long count = last - first;
    while (count > 0)
    {
        long half = count / 2;
        const s_cache_file_tag_dependency *middle = first + half;
        if (cache_file_tag_dependency_less(*middle, tag_index))
        {
            first = middle + 1;
            count -= half + 1;
        }
        else
        {
            count = half;
        }
    }
    return first;
}
// @address 0x1b30c8 ?visit_tag_resources@c_cache_file_tag_resource_runtime_manager_storage@@UAA_NJPAXPAUs_cache_file_tag_resource_graph_visitor@@@Z
bool c_cache_file_tag_resource_runtime_manager_storage::visit_tag_resources(long tag_index, void *context, s_cache_file_tag_resource_graph_visitor *visitor)
{
    const s_cache_file_resource_gestalt *graph = get_gestalt();
    bool result = true;
    unsigned long visited[625];
    memset(visited, 0, sizeof(visited));
    s_cache_file_tag_dependency_visitor dependency_visitor;
    dependency_visitor.root_tag_index = tag_index;
    dependency_visitor.graph = graph;
    dependency_visitor.visited = visited;
    dependency_visitor.depth = 0;
    if (tag_index != NONE)
    {
        const s_cache_file_tag_dependency *dependency = cache_file_tag_dependency_lower_bound(
            graph->dependencies.begin(), graph->dependencies.end(), tag_index);
        if (dependency->tag.index == tag_index)
            tag_resource_iterate_dependencies(&dependency_visitor, dependency);
    }
    for (long index = 0; result && index < graph->resources.count; index++)
    {
        const s_cache_file_tag_resource_data *resource = &((const s_cache_file_tag_resource_data *)graph->resources.address)[index];
        if (resource->salt != NONE && resource->owner_tag.index != NONE &&
            bit_vector_test(visited, DATUM_INDEX_TO_ABSOLUTE_INDEX(resource->owner_tag.index)))
        {
            result = visitor->visit(context, (resource->salt << 16) | index);
        }
    }
    return result;
}

// 0x141b80: the location of a datum index's resource element. A local element's location is its file index with
// the high bit clear (NONE stays NONE); a shared element's location is its shared page index with the high bit set
// and its size comes from the set's shared page table.
// @address 0x141b80 ?get_page_location@c_cache_file_tag_resource_runtime_window_allocator_storage@@UAA_NJPAUs_tag_resource_page_location@@@Z
bool c_cache_file_tag_resource_runtime_window_allocator_storage::get_page_location(long handle, s_tag_resource_page_location *location)
{
    const s_cache_file_tag_resource_element *element = cache_file_resource_get_element_by_datum(m_element_set, handle);

    location->offset = 0;
    location->size = element->compressed_size;
    unsigned char flags = element->flags;
    if (TEST_BIT(flags, 5))
    {
        location->expected_hash = element->hash;
        location->validate_hash = true;
    }
    else if (TEST_BIT(flags, 0))
    {
        location->expected_hash = element->hash;
        location->validate_hash = true;
    }
    else
    {
        location->validate_hash = false;
    }

    if (element->shared_page_index == NONE)
    {
        location->uncompressed_size = element->size;
        long file_index = element->file_index;
        location->hash_source_index = file_index != NONE ? file_index & 0x7fffffff : NONE;
    }
    else
    {
        location->uncompressed_size = m_element_set->first->shared_pages.begin()[element->shared_page_index].size;
        location->hash_source_index = element->shared_page_index | 0x80000000;
    }
    return true;
}

// 0x141860: a local element's size, or its shared page's total size
// @address 0x141860 ?get_page_size@c_cache_file_tag_resource_runtime_window_allocator_storage@@UAAJI@Z
long c_cache_file_tag_resource_runtime_window_allocator_storage::get_page_size(unsigned int handle)
{
    const s_cache_file_tag_resource_element *element = cache_file_resource_get_element_by_datum(m_element_set, handle);
    if (element->shared_page_index == NONE)
        return element->size;
    return m_element_set->first->shared_pages.begin()[element->shared_page_index].size;
}
static __forceinline void cache_file_tag_resource_window_extend_start(c_basic_buffer<void> *window, unsigned long size)
{
    window->m_address = static_cast<char *>(window->m_address) - size;
    window->m_size += size;
}
// @address 0x1b2d40 ?allocate_lruv@c_cache_file_tag_resource_runtime_manager_storage@@UAAPAXK@Z
void * c_cache_file_tag_resource_runtime_manager_storage::allocate_lruv(unsigned long size)
{
    void *result = NULL;
    unsigned long available = m_result_window.available(m_window);
    if (available < size)
    {
        unsigned long needed = size - available;
        unsigned long shrink_size = size > needed ? needed : size;
        c_tag_resource_page_table::c_tag_resource_lruv_cache *cache = &m_resource_cache.m_cache.m_page_table.m_lruv_cache;
        s_lruv_cache *lruv = (s_lruv_cache *)cache->m_cache;
        unsigned long shrink_pages = ((shrink_size + (1 << cache->m_alignment_bits) - 1) & (-1 << cache->m_alignment_bits)) >> cache->m_alignment_bits;
        unsigned long used_pages = 0;
        if (lruv->first_block_index != -1)
        {
            s_data_array *data = (s_data_array *)lruv->header.block_data;
            s_lruv_block *block = datum_get<s_lruv_block>(data, lruv->last_block_index);
            used_pages = block->page_index + block->page_count;
        }
        unsigned long page_count = MAX(lruv->maximum_page_count - shrink_pages, used_pages);
        tag_resource_lruv_cache_allocate_buffer(cache, page_count << cache->m_alignment_bits);
    }
    if (m_result_window.available(m_window) >= size)
    {
        cache_file_tag_resource_window_extend_start(&m_window, size);
        result = m_window.m_address;
    }
    return result;
}

// 0x1b1658: c_cache_file_tag_resource_runtime_manager::dispose_from_old_map
// @address 0x3bc54 ??_7s_cache_file_tag_resource_page_visitor@@6B@

// @address 0x3bbf8 ??_7c_cache_file_tag_resource_setup_iterator@@6B@

// @address 0x3bbfc ??_7s_cache_file_tag_resource_set_iterator@@6B@

static __forceinline bool cache_file_tag_resource_page_is_ready(const s_tag_resource_page_datum *page) { return page->state >= 3; }
__forceinline void c_tag_resource_cache_thread_lock_lock_freeish::lock_for_blocking_update()
{
    m_access_lock.write_lock();
    m_locked_for_blocking_update = true;
}
__forceinline void c_tag_resource_cache_thread_lock_lock_freeish::unlock_for_blocking_update()
{
    m_access_lock.write_unlock();
    m_locked_for_blocking_update = false;
}
__forceinline void c_tag_resource_cache_new::prepare_for_disposal_from_old_map()
{
    m_resource_thread_access_lock.lock_for_blocking_update();
    m_thread_access_locked = true;
    revoke_active_access_cache();
    release_unlocked_resources(true);
    m_resource_thread_access_lock.unlock_for_blocking_update();
    m_thread_access_locked = false;
    m_controller.prepare_for_disposal_from_old_map((c_tag_resource_address_cache_control_interface *)&m_resource_address_cache);
    m_page_table.cancel_async_and_block_until_async_done((c_tag_resource_controller_page_table_io_listener *)&m_page_io_listener_owner);
}
__forceinline void c_tag_resource_cache_new::unsetup_bulk_tracked_resources(c_cache_file_tag_resource_setup_iterator *iterator)
{
    long resource_handle;
    while (iterator->next_tracked_resource_handle(&resource_handle))
        end_tracking_resource(resource_handle);
}
__forceinline void c_tag_resource_cache_new::unsetup_bulk_tracked_pages(s_cache_file_tag_resource_set_iterator *iterator)
{
    long resource_handle;
    while (iterator->next_tracked_page_handle(&resource_handle, NULL))
    {
        long handle = resource_handle;
        c_tag_resource_page_table *table = &m_page_table;
        tag_resource_page_table_reset_page(table, handle);
        ::datum_delete((::s_data_array *)table->m_pages, handle);
        ::datum_delete((::s_data_array *)m_inverse_page_table.table, handle);
    }
}
static __forceinline void cache_file_tag_resource_buffer_clear(c_basic_buffer<void> *buffer)
{
    buffer->m_address = NULL;
    buffer->m_size = 0;
}
static __forceinline void cache_file_tag_resource_flags_clear(c_wrapped_flags *flags)
{
    flags->m_flags = NULL;
    flags->m_count = 0;
}
static __forceinline void cache_file_tag_resource_shared_files_clear(s_cache_file_tag_resource_runtime_shared_file_arrays *files)
{
    files->m_pages.set_elements(NULL, 0);
    files->m_requests.set_elements(NULL, 0);
}
_STD_BEGIN
template<class _RanIt,
    class _Pr> inline
    void sort(_RanIt _First, _RanIt _Last, _Pr _Pred)
    {	// order [_First, _Last), using _Pred
    _DEBUG_RANGE(_First, _Last);
    _DEBUG_POINTER(_Pred);
    cache_file_tag_resource_sort_by_offset(_CHECKED_BASE(_First), _CHECKED_BASE(_Last), _Last - _First, _Pred);
    }
_STD_END
__forceinline void s_cache_file_tag_resource_page_state::dispose_from_old_map(c_cache_file_tag_resource_runtime_listener_storage *allocator)
{
    source_count = 0;
    allocator->deallocate(buffer.m_address);
    cache_file_tag_resource_buffer_clear(&buffer);
}
__forceinline void s_cache_file_tag_resource_relocation_list::sort()
{
    std::sort(begin(), end(), s_cache_file_tag_resource_sort_predicate());
    reset_cursor();
}
__forceinline void c_cache_file_tag_resource_runtime_listener_storage::reset()
{
    cache_file_tag_resource_buffer_clear(&m_second);
    cache_file_tag_resource_buffer_clear(&m_first);
}
static __forceinline void cache_file_tag_resource_visit_ready_pages(
    c_cache_file_tag_resource_runtime_manager_storage *manager,
    s_cache_file_tag_resource_page_visitor_interface *visitor,
    s_cache_file_runtime_definition_iterator *page_iterator)
{
    page_iterator->m_iterator.data = manager->m_resource_cache.m_cache.m_page_table.m_pages;
    page_iterator->m_iterator.absolute_index = -1;
    page_iterator->m_iterator.index = -1;
    while (page_iterator->next())
    {
        long handle = page_iterator->m_iterator.index;
        const s_tag_resource_page_datum *page = datum_get<s_tag_resource_page_datum>(
            manager->m_resource_cache.m_cache.m_page_table.m_pages, handle);
        if (cache_file_tag_resource_page_is_ready(page))
            visitor->add_element_relocation(handle, page->buffer);
    }
}
// @address 0x1b1658 cache_file_tag_resource_runtime_manager_dispose_from_old_map
extern "C" void cache_file_tag_resource_runtime_manager_dispose_from_old_map(c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    s_cache_file_runtime_definition_iterator page_iterator;
    s_cache_file_tag_resource_runtime_cache_storage *resource_cache = &manager->m_resource_cache;
    c_cache_file_tag_resource_runtime_thread_access_storage *thread_access = resource_cache ? &resource_cache->m_render_access : NULL;
    thread_access->m_access_lock.write_lock();
    thread_access->m_resource_thread_access_lock = NULL;
    thread_access->m_access_lock.write_unlock();
    c_tag_resource_cache_new *cache = &resource_cache->m_cache;
    cache->prepare_for_disposal_from_old_map();

    s_cache_file_tag_resource_page_visitor visitor;
    visitor.m_manager = manager;
    cache_file_tag_resource_visit_ready_pages(manager, &visitor, &page_iterator);
    tag_resource_reset_active_zone(manager);
    tag_resource_dispose_zone_data(manager);

    s_cache_file_resource_gestalt *gestalt = (s_cache_file_resource_gestalt *)manager->m_header->instances[
        (short)manager->m_cache_file_resource_gestalt_index].definition;
    c_cache_file_tag_resource_setup_iterator setup_iterator(gestalt, manager->m_streaming_enabled);
    manager->m_resource_cache.m_cache.unsetup_bulk_tracked_resources(&setup_iterator);

    s_cache_file_resource_element_set *element_set = &manager->m_element_set;
    s_cache_file_tag_resource_set_iterator set_iterator(element_set, manager->m_streaming_enabled);
    manager->m_resource_cache.m_cache.unsetup_bulk_tracked_pages(&set_iterator);
    if (manager->m_file_slots.m_reclaim_expired)
        game_state_procs_dispose_active_entries();
    for (long index = 0; index < 6; index++)
        manager->m_file_slots.request_data.requests[index].valid = false;
    cache_file_tag_resource_buffer_clear(&manager->m_buffer);
    cache->dispose_from_old_map();
    element_set->first = NULL;
    element_set->second = NULL;
    element_set->third = NULL;
    data_dispose(manager->m_pending_locations);
    data_dispose(manager->m_runtime_locations);
    cache_file_tag_resource_shared_files_clear(&manager->m_shared_files);
    c_cache_file_tag_resource_runtime_listener_storage *allocator = &manager->m_listener_storage;
    manager->m_page_storage.dispose_from_old_map(allocator);
    manager->m_window_allocator.m_element_set = NULL;
    manager->m_datum_handler.dispose_from_old_map();
    allocator->deallocate(manager->m_interop_buffer.m_address);
    cache_file_tag_resource_buffer_clear(&manager->m_interop_buffer);
    cache_file_tag_resource_buffer_clear(&manager->m_interop_buffer);
    allocator->deallocate(manager->m_resource_runtime_data.get_elements());
    manager->m_resource_runtime_data.set_elements(NULL, 0);
    cache_file_tag_resource_reset_handle_set(manager);
    manager->m_cache_file_resource_gestalt_index = -1;
    manager->m_listener_storage.reset();
    manager->m_relocations.sort();
    g_physical_memory_globals.current_stage--;
    cache_file_tag_resource_buffer_clear(&manager->m_region);
}

template <typename t_element, typename t_value, typename t_compare>
static __forceinline member_compare_val_t<t_element, t_value, t_compare> member_compare_val(t_value t_element::*member, const t_value &value, t_compare)
{
    return member_compare_val_t<t_element, t_value, t_compare>(member, value);
}
template <typename t_container, typename t_predicate, typename t_iterator>
static __forceinline bool find_if_container_value(const t_container &container, t_predicate predicate, t_iterator *out_value_iterator)
{
    bool result = false;
    t_iterator iterator = std::find_if(container.begin(), container.end(), predicate);
    if (iterator != container.end())
    {
        *out_value_iterator = iterator;
        result = true;
    }
    return result;
}
template <typename t_type>
static __forceinline c_wrapped_array<t_type> cache_file_tag_block_get_elements(const s_tag_block &block)
{
    c_wrapped_array<t_type> elements;
    elements.set_elements((t_type *)block.address, block.count);
    return elements;
}
static __forceinline void *cache_file_tag_get(const tag_header *header, long tag_index)
{
    return header->instances[(short)tag_index].definition;
}
static __forceinline void cache_file_tag_resource_physical_memory_push_stage()
{
    long next_stage = g_physical_memory_globals.current_stage + 1;
    g_physical_memory_globals.stages[next_stage] = g_physical_memory_globals.stages[g_physical_memory_globals.current_stage];
    g_physical_memory_globals.current_stage = next_stage;
}
static __forceinline c_basic_buffer<void> cache_file_tag_resource_physical_memory_get_stage_buffer()
{
    const physical_stage *stage = &g_physical_memory_globals.stages[g_physical_memory_globals.current_stage];
    char *begin = static_cast<char *>(physical_memory_map_physical_address(stage->begin));
    char *end = static_cast<char *>(physical_memory_map_physical_address(stage->end));
    c_basic_buffer<void> buffer(c_basic_buffer<void>::_uninitialized);
    buffer.m_address = begin;
    buffer.m_size = end - begin;
    return buffer;
}
static __forceinline void cache_file_tag_resource_buffer_copy(c_basic_buffer<void> & destination, const c_basic_buffer<void> &source)
{
    destination = source;
}
static __forceinline void cache_file_tag_resource_reset_allocation_buffer(c_cache_file_tag_resource_runtime_listener_storage *allocator)
{
    cache_file_tag_resource_buffer_copy(allocator->m_first, allocator->m_second);
}
static __forceinline c_basic_buffer<void> cache_file_tag_resource_buffer_get_remaining(const c_basic_buffer<void> &buffer, void *position)
{
    c_basic_buffer<void> remaining(c_basic_buffer<void>::_uninitialized);
    remaining.m_address = position;
    remaining.m_size = buffer.m_size - ((char *)position - (char *)buffer.m_address);
    return remaining;
}
static __forceinline void *cache_file_tag_resource_buffer_end_for_map(const c_basic_buffer<void> &buffer)
{
    return offset_pointer(buffer.m_address, buffer.m_size);
}
static __forceinline bool cache_file_tag_resource_flag_test(unsigned short flags, int bit)
{
    return (flags >> bit) & 1;
}
// @address 0x3bc4c ??_7s_cache_file_tag_resource_page_reader@@6B@

static __forceinline void cache_file_tag_resource_read_page_table(c_tag_resource_cache_new *cache, s_cache_file_tag_resource_page_reader *reader)
{
    tag_resource_page_table_read_pages(&cache->m_page_table, reader, (c_tag_resource_controller_page_table_io_listener *)&cache->m_page_io_listener_owner);
}
static __forceinline void *cache_file_tag_resource_allocate(c_allocation_base *allocator, unsigned int size, const char *name)
{
    return allocator->allocate(size, name);
}
#if defined(_XBOX) && !defined(__clang__)
__forceinline void s_cache_file_tag_resource_runtime_cache_storage::initialize_for_new_map(
    long resource_count, long maximum_page_count,
    c_tag_resource_runtime_listener *runtime_listener, c_tag_resource_cache_file_reader *file_reader,
    c_tag_resource_cache_file_datum_handler *datum_handler, c_cache_file_tag_resource_runtime_window_allocator_storage *page_allocator,
    c_tag_resource_runtime_active_set *first_interface, c_tag_resource_paging_page_filter *second_interface,
    c_allocation_base *allocator, c_tag_resource_page_range_allocator *range_allocator)
{
    tag_resource_cache_new_initialize(get_resource_cache(), resource_count, maximum_page_count, 0, runtime_listener, file_reader,
        datum_handler, page_allocator, first_interface, second_interface, allocator, range_allocator);
    c_cache_file_tag_resource_runtime_thread_access_storage *thread_access = this ? &m_render_access : NULL;
    thread_access->m_access_lock.write_lock();
    thread_access->m_resource_thread_access_lock = &get_resource_cache()->m_resource_thread_access_lock;
    thread_access->m_access_lock.write_unlock();
}
#else
void s_cache_file_tag_resource_runtime_cache_storage::initialize_for_new_map(long resource_count, long maximum_page_count,
    c_cache_file_tag_resource_runtime_manager_storage *manager, c_allocation_base *allocator)
{
    tag_resource_cache_new_initialize(get_resource_cache(), resource_count, maximum_page_count, 0, manager, manager,
        &manager->m_datum_handler, &manager->m_window_allocator, manager, manager, allocator, manager);
    c_cache_file_tag_resource_runtime_thread_access_storage *thread_access = this ? &m_render_access : NULL;
    thread_access->m_access_lock.write_lock();
    thread_access->m_resource_thread_access_lock = &get_resource_cache()->m_resource_thread_access_lock;
    thread_access->m_access_lock.write_unlock();
}
#endif
// data_new with data_initialize, inlined into the map's data arrays
static __forceinline s_data_array *cache_file_tag_resource_data_new(const char *name, long maximum_count, long size, c_allocation_base *allocator)
{
    s_data_array *data = static_cast<s_data_array *>(
        allocator->allocate(sizeof(s_data_array) + ((maximum_count + 31) >> 5) * 4 + maximum_count * size, name));
    if (data)
    {
        char *storage = reinterpret_cast<char *>(data + 1);
        data_initialize_header(data, name, maximum_count, size, 0, allocator, storage + maximum_count * size);
        data->data = storage;
        data->data_offset = storage - (char *)data;
        data->flags &= ~2;
        data->flags &= ~1;
        memset(data->in_use_bit_vector, 0, ((maximum_count + 31) >> 5) * 4);
        data->flags |= 4;
    }
    return data;
}
__forceinline void c_cache_file_tag_resource_runtime_listener_storage::initialize(c_basic_buffer<void> buffer)
{
    m_first = buffer;
    m_second = buffer;
    m_second.m_address = (char *)m_second.m_address + buffer.m_size;
    m_second.m_size -= buffer.m_size;
}
__forceinline void c_tag_resource_cache_new::setup_bulk_tracked_pages(s_cache_file_tag_resource_set_iterator *iterator)
{
    long resource_handle;
    long page_handle;
    while (iterator->next_tracked_page_handle(&resource_handle, &page_handle))
        tag_resource_cache_track_element((tag_resource_external_access_set_object *)this, resource_handle, page_handle);
}
__forceinline void c_tag_resource_cache_new::setup_bulk_tracked_resources(c_cache_file_tag_resource_setup_iterator *iterator)
{
    long resource_handle;
    while (iterator->next_tracked_resource_handle(&resource_handle))
        tag_resource_cache_external_access_track_resource(this, resource_handle, 0);
}
__forceinline void s_cache_file_tag_resource_relocation_list::reset()
{
    count = 0;
    cache_file_tag_resource_buffer_clear(&window);
}
static __forceinline void cache_file_tag_resource_named_value_reset(s_cache_file_tag_resource_named_value *value)
{
    value->m_value = NONE;
    value->m_name.clear();
}
__forceinline void s_cache_file_tag_resource_relocation_list::sort_by_key()
{
    s_cache_file_tag_resource_relocation *first = begin();
    s_cache_file_tag_resource_relocation *last = end();
    cache_file_tag_resource_sort_by_key(first, last, last - first, s_cache_file_tag_resource_sort_key_predicate());
}
static __forceinline bool cache_file_tag_resource_has_runtime_locations(c_cache_file_tag_resource_runtime_manager_storage *manager)
{
    return cache_file_tag_resource_find_paged_entry(manager) != NONE;
}
template <typename t_layout_table_owner>
__forceinline void c_cache_file_tag_resource_runtime_manager_storage::initialize_layout_table(
    const t_layout_table_owner *owner, s_cache_file_tag_resource_codec_resolver *codec_resolver)
{
    m_page_storage.initialize(
        &m_listener_storage, owner->layout_table.codecs, codec_resolver, owner->layout_table.sizes);
    m_shared_files.initialize(owner->layout_table.sizes, owner->layout_table.pages, owner->layout_table.segments);
}
__forceinline void c_cache_file_tag_resource_runtime_manager_storage::initialize_external_access()
{
    const s_cache_file_resource_gestalt *gestalt = (const s_cache_file_resource_gestalt *)cache_file_tag_get(m_header, m_cache_file_resource_gestalt_index);
    tag_resource_cache_initialize_hash_table(&m_external_access, gestalt->external_access_list,
        gestalt->external_access_blocks[0], gestalt->external_access_blocks[1],
        gestalt->external_access_blocks[2], gestalt->external_access_blocks[3]);
}
// @address 0x1b05d8 ?initialize_for_new_map@c_cache_file_tag_resource_runtime_manager@@QAAXPAUtag_header@@JJV?$c_wrapped_array@PBUs_cache_file_tag_resource_vtable@@@@PAUs_cache_file_tag_resource_codec_resolver@@@Z
void c_cache_file_tag_resource_runtime_manager::initialize_for_new_map(tag_header *header, long game_mode, long cache_file_resource_gestalt_index,
    c_wrapped_array<const s_cache_file_tag_resource_vtable *> resource_vtables, s_cache_file_tag_resource_codec_resolver *codec_resolver)
{
    c_cache_file_tag_resource_runtime_manager_storage *manager = (c_cache_file_tag_resource_runtime_manager_storage *)this;
    long previous_game_mode = manager->m_previous_game_mode;
    cache_file_tag_resource_physical_memory_push_stage();
    c_basic_buffer<void> region = cache_file_tag_resource_physical_memory_get_stage_buffer();
    _physical_memory_malloc((memory_stage)6, NULL, region.m_size, 10, NULL, NONE);
    manager->m_region = region;
    c_cache_file_tag_resource_runtime_listener_storage *allocator = &manager->m_listener_storage;
    allocator->initialize(region);
    manager->m_header = header;
    manager->m_previous_game_mode = manager->m_game_mode;
    manager->m_game_mode = game_mode;
    manager->m_cache_file_resource_gestalt_index = cache_file_resource_gestalt_index;
    tag_resource_initialize_map_definitions(manager, game_mode);

    const s_cache_file_resource_gestalt *gestalt = (const s_cache_file_resource_gestalt *)cache_file_tag_get(
        manager->m_header, manager->m_cache_file_resource_gestalt_index);
    manager->m_zone_state.reset();
    manager->m_zone_names.reset();
    manager->m_active_zone_changed = false;
    manager->m_required_resource_state_changed = true;
    manager->m_state_valid = false;

    long resource_type_count = gestalt->resource_types.count;
    for (long resource_type_index = 0; resource_type_index < resource_type_count; resource_type_index++)
    {
        const s_cache_file_tag_resource_type_definition *resource_type = &(
            (const s_cache_file_tag_resource_type_definition *)gestalt->resource_types.address)[resource_type_index];
        const s_cache_file_tag_resource_vtable *const *vtable;
        if (find_if_container_value(resource_vtables,
                member_compare_val(&s_cache_file_tag_resource_vtable::identifier, resource_type->identifier, c_memcmp_equals()), &vtable))
            manager->m_resource_vtables[resource_type_index] = *vtable;
        else
            manager->m_resource_vtables[resource_type_index] = NULL;
    }

    long layout_table_index = NONE;
    for (long tag_index = 0; tag_index < header->count; tag_index++)
    {
        if (header->tags[tag_index].group == 'play')
            layout_table_index = header->tags[tag_index].index;
    }
    if (layout_table_index != NONE)
    {
        const s_cache_file_resource_layout_table_definition *layout_table =
            (const s_cache_file_resource_layout_table_definition *)cache_file_tag_get(header, layout_table_index);
        if (layout_table)
            manager->initialize_layout_table(layout_table, codec_resolver);
        else
            manager->initialize_layout_table(gestalt, codec_resolver);
    }
    else
    {
        manager->initialize_layout_table(gestalt, codec_resolver);
    }

    long location_count = cache_file_tag_resource_has_runtime_locations(manager) ? 320 : 0;
    manager->m_runtime_locations = (s_data_array *)cache_file_tag_resource_data_new(
        "runtime resource locations", location_count, 88, (c_allocation_base *)allocator);
    manager->m_pending_locations = (s_data_array *)cache_file_tag_resource_data_new("runtime resource locations", 320, 88, (c_allocation_base *)allocator);
    manager->m_element_set.initialize(
        (s_cache_file_resource_fixed_array *)&manager->m_shared_files, &manager->m_runtime_locations, &manager->m_pending_locations);
    main_update_idle_tasks();

    // match: 0x1b05d8: without the zero-bit rotates four later adds swap their operands (add r29,r11,r28)
    unsigned long runtime_data_size = _rotl(_rotl((unsigned long)gestalt->resources.count, 0), 0) * sizeof(s_cache_file_resource_runtime_data_new);
    s_cache_file_resource_runtime_data_new *runtime_data = (s_cache_file_resource_runtime_data_new *)cache_file_tag_resource_allocate(
        &manager->m_listener_storage, runtime_data_size, "resource runtime data");
    memset(runtime_data, 0, runtime_data_size);
    manager->m_resource_runtime_data.set_elements(runtime_data, gestalt->resources.count);
    manager->m_datum_handler.initialize_for_new_map(manager->m_header, manager->m_cache_file_resource_gestalt_index, manager->m_resource_runtime_data,
            manager->m_streaming_enabled, &manager->m_element_set);
    manager->m_window_allocator.m_element_set = &manager->m_element_set;
    main_update_idle_tasks();

    s_cache_file_tag_interop_bulk_sizer sizer;
    sizer.alignment_bits = 0;
    sizer.size = 0;
    long resource_count = gestalt->resources.count;
    for (long resource_index = 0; resource_index < resource_count; resource_index++)
    {
        const s_cache_file_tag_resource_data *resource = &((const s_cache_file_tag_resource_data *)gestalt->resources.address)[resource_index];
        cache_file_tag_resource_accumulate_interop_types(resource, &sizer, NULL, NULL);
        if (cache_file_tag_resource_flag_test(resource->flags, 1))
        {
            cache_file_tag_resource_accumulate_interop_types(resource, &sizer, NULL, NULL);
            sizer.add_type_manually(resource->control_alignment_bits, resource->data_size);
        }
    }
    unsigned long interop_size = sizer.get_unaligned_total_allocation_size();
    c_basic_buffer<void> interop_buffer(cache_file_tag_resource_allocate(&manager->m_listener_storage, interop_size,
        "resource interop data"), interop_size);
    manager->m_interop_buffer = interop_buffer;

    s_cache_file_tag_interop_bulk_sizer runtime_sizer;
    runtime_sizer.alignment_bits = 0;
    runtime_sizer.size = 0;
    resource_count = gestalt->resources.count;
    for (long resource_index = 0; resource_index < resource_count; resource_index++)
    {
        const s_cache_file_tag_resource_data *resource = &((const s_cache_file_tag_resource_data *)gestalt->resources.address)[resource_index];
        unsigned long interop_offset;
        unsigned long interop_buffer_size;
        if (cache_file_tag_resource_accumulate_interop_types(resource, &runtime_sizer, &interop_offset, &interop_buffer_size))
            manager->m_resource_runtime_data[resource_index].m_interop_buffer = cache_file_tag_resource_buffer_get_range(
                manager->m_interop_buffer, interop_offset, interop_buffer_size);
        unsigned long optional_offset;
        unsigned long optional_size;
        if (cache_file_tag_resource_flag_test(resource->flags, 1)
            && cache_file_tag_resource_accumulate_interop_types(resource, &runtime_sizer, &optional_offset, &optional_size))
            manager->m_resource_runtime_data[resource_index].m_optional_buffer = cache_file_tag_resource_buffer_get_range(
                manager->m_interop_buffer, optional_offset, optional_size);
        if (cache_file_tag_resource_flag_test(resource->flags, 1))
        {
            unsigned long control_offset = runtime_sizer.get_next_offset_manual(resource->control_alignment_bits);
            runtime_sizer.add_type_manually(resource->control_alignment_bits, resource->data_size);
            // match: 0x1b05d8: the copy helper takes the page-buffer address as its first operand.
            cache_file_tag_resource_buffer_copy(manager->m_resource_runtime_data[resource_index].m_page_buffer,
                cache_file_tag_resource_buffer_get_range(manager->m_interop_buffer, control_offset, runtime_sizer.get_size() - control_offset));
            memcpy(manager->m_resource_runtime_data[resource_index].m_page_buffer.m_address,
                (char *)gestalt->fixup_information.address + resource->data_offset, resource->data_size);
        }
    }

    s_cache_file_tag_resource_runtime_cache_storage *resource_cache = &manager->m_resource_cache;
    c_tag_resource_cache_new *cache = &resource_cache->m_cache;
#if defined(_XBOX) && !defined(__clang__)
    manager->m_resource_cache.initialize_for_new_map(gestalt->resources.count, manager->m_element_set.get_maximum_count(),
        manager, manager, &manager->m_datum_handler, &manager->m_window_allocator,
        manager,
        manager, (c_allocation_base *)allocator, manager);
#else
    manager->m_resource_cache.initialize_for_new_map(
        gestalt->resources.count, manager->m_element_set.get_maximum_count(), manager, allocator);
#endif

    s_cache_file_tag_resource_set_iterator set_iterator(&manager->m_element_set, manager->m_streaming_enabled);
    cache->setup_bulk_tracked_pages(&set_iterator);
    c_cache_file_tag_resource_setup_iterator setup_iterator((s_cache_file_resource_gestalt *)gestalt, manager->m_streaming_enabled);
    cache->setup_bulk_tracked_resources(&setup_iterator);

    manager->initialize_external_access();
    cache_file_tag_resource_reset_allocation_buffer(&manager->m_listener_storage);
    manager->bind_file_slots();
    manager->m_map_initialized = true;
    c_basic_buffer<void> remaining = manager->m_listener_storage.m_second;
    manager->m_buffer = c_cache_file_tag_resource_runtime_buffer(manager->m_region, 0, manager->m_region.m_size - remaining.m_size);
    void *window_start = cache_file_tag_resource_buffer_end_for_map(manager->m_buffer);
    manager->m_window = c_cache_file_tag_resource_runtime_buffer(manager->m_buffer, window_start);
    if (manager->m_buffer.m_size >= gestalt->optional_resource_size)
        manager->m_resource_cache.m_cache.enable_optional_resources(true);

    if (manager->m_locked)
    {
        if (game_mode == 3)
        {
            if (previous_game_mode == 3)
                cache_files_windows_update_load_status(manager, (short)manager->m_first_cursor.m_value, manager->m_first_cursor.m_name.get_string());
            else
                cache_files_windows_update_load_status(manager, (short)manager->m_second_cursor.m_value, manager->m_second_cursor.m_name.get_string());
            cache_file_tag_resource_named_value_reset(&manager->m_first_cursor);
            cache_file_tag_resource_named_value_reset(&manager->m_second_cursor);
            tag_resource_load_deferred_entries(manager);
            tag_resource_update_active_map_file(manager);
        }
        cache_file_tag_resource_update_relocation_window(
            &manager->m_relocations, manager->m_region, manager->m_buffer);
        manager->m_relocations.sort_by_key();
        c_basic_buffer<void> window = manager->m_relocations.window;
        if (window.m_size != 0)
        {
            s_cache_file_tag_resource_page_reader reader;
            reader.m_manager = manager;
            reader.m_index = 0;
            cache_file_tag_resource_read_page_table(&manager->m_resource_cache.m_cache, &reader);
        }
        manager->m_relocations.reset();
        manager->m_locked = false;
    }
}


STATIC_ASSERT(offsetof(s_cache_file_resource_shared_page, subpages) == 4, resource_shared_page_subpages_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_page_request, done) == 8, resource_request_done_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_page_request, slot) == 0x18, resource_request_slot_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_locked_window) == 0x2acf0, resource_locked_window_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_region) == 0x2b0b4, resource_region_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_streaming_enabled) == 0x8b61c, resource_streaming_enabled_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_locked_window) == 0x2acf0, resource_manager_locked_window_offset);
STATIC_ASSERT_ABI32(
    offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_zone_state) + offsetof(s_cache_file_tag_resource_active_state, mask4) == 0x30,
    resource_state_mask_offset);
STATIC_ASSERT(sizeof(s_cache_file_tag_resource_file_page) == 0x1c, resource_file_page_size);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_file_header, resource_count) == 0x10, resource_file_count_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_file_header, page_count) == 0x12, resource_file_page_count_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_file_header, offset) == 0x14, resource_file_offset_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_file_header, pages) == 0x28, resource_file_pages_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_file_header, zone_count) == 0x2328, resource_file_zone_count_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_file_header, insertion_point_usages) == 0x232c, resource_insertion_point_usages_offset);
STATIC_ASSERT(sizeof(s_cache_file_insertion_point_resource_usage) == 0xac, cache_file_insertion_point_resource_usage_size);
STATIC_ASSERT(offsetof(s_cache_file_bulk_reader_slot_set, slots) == 8, resource_bulk_slots_offset);
STATIC_ASSERT(offsetof(s_tag_resource_page_datum, buffer) == 0xc, resource_page_entry_buffer_offset);
STATIC_ASSERT_ABI32(sizeof(c_tag_resource_page_table) == 0x64, resource_page_table_size);
STATIC_ASSERT_ABI32(sizeof(c_tag_resource_cache_controller) == 0x64, resource_controller_size);

STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_page_state, busy) == 0x244, runtime_page_busy_offset);
STATIC_ASSERT_ABI32(sizeof(s_cache_file_tag_resource_page_state) == 0x248, runtime_page_state_size);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_resource_fixed_array, m_handle_count) == 0x10, runtime_fixed_array_handle_count_offset);

STATIC_ASSERT(sizeof(s_cache_file_resource_handle_pair) == 0x10, runtime_handle_pair_size);
STATIC_ASSERT(offsetof(s_cache_file_resource_handle_pair, first_fixup_value) == 4, runtime_handle_first_fixup_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_handle_pair, second_fixup_value) == 8, runtime_handle_second_fixup_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_resource_fixed_array, handles) == 0x14, runtime_handle_array_offset);
STATIC_ASSERT_ABI32(sizeof(c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler) == 0x1c, runtime_datum_handler_size);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_state_valid) == 0x25c,
    runtime_storage_state_valid_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_required_resource_state_changed) == 0x25d,
    runtime_storage_required_resource_state_changed_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_active_resource_state_changed) == 0x25e,
    runtime_storage_active_resource_state_changed_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_active_zone_changed) == 0x25f,
    runtime_storage_active_zone_changed_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_runtime_cache_storage, m_render_access) == 0x320, runtime_cache_render_access_offset);
STATIC_ASSERT_ABI32(sizeof(s_cache_file_tag_resource_runtime_cache_storage) == 0x3a0, runtime_cache_storage_size);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_resource_cache) == 0x2ad00, runtime_manager_cache_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager_storage, m_listener_storage) == 0x2b0a0, runtime_manager_listener_offset);
