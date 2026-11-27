#ifndef TAG_FILES_TAG_TYPES_H
#define TAG_FILES_TAG_TYPES_H

#include "memory/wrapped_arrays.h"

#if defined(_WIN64) || defined(__LP64__)
// 32-bit tag/cache pointer slot when type-checking on a 64-bit host: keeps on-disk tag layouts at their true 32-bit size.
template<typename t_target>
struct s_tag_pointer {
    unsigned long value;
    __forceinline t_target *get() const { return reinterpret_cast<t_target *>(static_cast<unsigned __int64>(value)); }
    __forceinline t_target *get() const volatile { return reinterpret_cast<t_target *>(static_cast<unsigned __int64>(value)); }
    __forceinline operator t_target *() const { return get(); }
    __forceinline operator t_target *() const volatile { return get(); }
    template<typename t_other>
    __forceinline explicit operator t_other() const { return (t_other)(unsigned __int64)value; }
    __forceinline t_target *operator->() const { return get(); }
    __forceinline bool operator!() const { return value == 0; }
    __forceinline t_target *operator=(t_target *ptr) {
        unsigned __int64 raw = reinterpret_cast<unsigned __int64>(ptr);
        if (raw >> 32) {
#if defined(__clang__) || defined(__GNUC__)
            __builtin_trap();
#else
            __debugbreak();
#endif
        }
        value = static_cast<unsigned long>(raw);
        return ptr;
    }
};
#define TAG_POINTER(t_target) s_tag_pointer<t_target >
#else
#define TAG_POINTER(t_target) t_target *
#endif

struct s_tag_block {
    long count;
    TAG_POINTER(void) address;
    TAG_POINTER(void) definition;
};

// March 2007: tag_groups.cpp:7299, out of line there with a range assert; the release build has no tag_groups.obj,
// so it is inline here. It sits beside s_tag_block because c_typed_tag_block::begin() needs it.
static __forceinline void *tag_block_get_range_with_size(const s_tag_block *block, long index, long count, long element_size) {
    (void)count;
    return (char *)block->address + index * element_size;
}

// Bungie's typed tag block: the 12 bytes of s_tag_block with the element type known. The class itself has count()
// and begin(); everything else (operator[], valid_index, get_element, ...) comes from the interface.
// March 2007 emits only the const count() and begin(): its eight instantiations are all used through const blocks,
// blocks are also written through in place, so the non-const begin() is
// the const one without the const.
template<typename t_element>
class c_typed_tag_block : public c_array_operator_interface<c_typed_tag_block<t_element>, t_element>, public s_tag_block {
public:
    __forceinline long count() const { return s_tag_block::count; }
    __forceinline t_element *begin() {
        return (t_element *)tag_block_get_range_with_size(this, 0, s_tag_block::count, sizeof(t_element));
    }
    __forceinline const t_element *begin() const {
        return (const t_element *)tag_block_get_range_with_size(this, 0, s_tag_block::count, sizeof(t_element));
    }
};

struct s_tag_reference {
    unsigned long group_tag;
    TAG_POINTER(const char) name;
    long name_length;
    long index;
};

struct s_tag_data {
    long size;
    long flags;
    long stream_position;
    TAG_POINTER(void) address;
    TAG_POINTER(void) definition;
};

struct cache_file_tag_instance {
    short group_index;
    unsigned short salt;
    TAG_POINTER(void) definition;
};

struct cache_file_tag_group {
    unsigned long group_tags[3];
    long name;
};

struct global_tag_index {
    unsigned long group;
    long index;
};

struct tag_iterator {
    // The cache iterator's state starts at byte 8.
    unsigned long : 32;
    unsigned long : 32;
    long index;
    long next_index;
    unsigned long group_tag;
};

typedef char tag_types_size_tag_block[(sizeof(s_tag_block) == 12) ? 1 : -1];
typedef char tag_types_size_typed_tag_block[(sizeof(c_typed_tag_block<long>) == 12) ? 1 : -1];
typedef char tag_types_size_tag_reference[(sizeof(s_tag_reference) == 16) ? 1 : -1];
typedef char tag_types_size_tag_data[(sizeof(s_tag_data) == 20) ? 1 : -1];
typedef char tag_types_size_tag_instance[(sizeof(cache_file_tag_instance) == 8) ? 1 : -1];
typedef char tag_types_size_tag_group[(sizeof(cache_file_tag_group) == 16) ? 1 : -1];
typedef char tag_types_size_tag_iterator[(sizeof(tag_iterator) == 20) ? 1 : -1];

#endif // TAG_FILES_TAG_TYPES_H
