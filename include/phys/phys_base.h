#ifndef INCLUDE_PHYS_BASE_H
#define INCLUDE_PHYS_BASE_H

#include "cseries/crt_xdk.h"
#include <math.h>
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>

#define PHYS_FORCE_INLINE __forceinline
#ifndef PHYS_CALL
#define PHYS_CALL
#endif

// HK_DECLARE_REFLECTION declares Havok's reflection data (hkClass members). The game uses no reflection and it adds
// no data members, so it declares nothing here. HK_ALIGN16 aligns one member to 16 bytes.
#define PHYS_DECLARE_REFLECTION()
#define PHYS_ALIGN16(ALIGNED_DECL) __declspec(align(16)) ALIGNED_DECL

#include "phys/phys_scalar_types.h"
typedef __vector4 phys_simd_vec;

#include "phys/phys_fixup_tag.h"

#include "phys/phys_base_types.h"

#include "phys/phys_thread_memory.h"

// Pointer arithmetic helpers (March 2007: hkPointerToInt32, hkGetByteOffset, hkGetByteOffsetInt, hkAddByteOffset,
// hkAddByteOffsetConst, hkSelectOther).
__forceinline int phys_ptr_to_i32(const void *pointer) { return static_cast<int>(reinterpret_cast<phys_ulong>(pointer)); }

__forceinline phys_ulong phys_get_byte_offset(const void *base, const void *pointer) {
    return reinterpret_cast<phys_ulong>(pointer) - reinterpret_cast<phys_ulong>(base);
}

__forceinline int phys_get_byte_offset_int(const void *base, const void *pointer) {
    return static_cast<int>(phys_get_byte_offset(base, pointer));
}

template <typename T>
__forceinline T *phys_add_byte_offset(T *base, phys_ulong offset) {
    return reinterpret_cast<T *>(reinterpret_cast<char *>(base) + offset);
}

template <typename T>
__forceinline const T *phys_add_byte_offset_const(const T *base, phys_ulong offset) {
    return reinterpret_cast<const T *>(reinterpret_cast<const char *>(base) + offset);
}

// The member of the pair (pairA, pairB) that is not a.
template <typename T>
__forceinline T *phys_select_other(T *a, T *pair_a, T *pair_b) {
    return reinterpret_cast<T *>(reinterpret_cast<phys_ulong>(a) ^ reinterpret_cast<phys_ulong>(pair_a) ^
                                 reinterpret_cast<phys_ulong>(pair_b));
}

class phys_class_desc;

// An object and its reflection class.
struct phys_variant {
    void *m_object;
    const phys_class_desc *m_class_desc;
};

template <typename T>
class phys_object_array;

#include "phys/phys_string.h"

// A float stored as its upper 16 bits (March 2007: hkHalf(), operator=(const float &), operator float).
#include "phys/phys_half.h"

// A non-negative float stored as an index into a 256-entry table.
extern "C" {
extern const float phys_ufloat8_to_real[256];
}
class phys_ufloat8 {
public:
    enum { VALUE_LIMIT = 256 };

    inline phys_ufloat8() {}
    // The nearest table entry at or above fv, found by halving the table range seven times.
    inline phys_ufloat8 &operator=(const float &fv) {
        int bits = *(int *)&fv;
        int *low = (int *)&phys_ufloat8_to_real[0];
        int *high = (int *)&phys_ufloat8_to_real[VALUE_LIMIT];
        int *middle = (int *)&phys_ufloat8_to_real[VALUE_LIMIT / 2];
        for (int i = 6; i >= 0; i--) {
            if (bits > middle[0]) {
                low = middle;
            } else {
                high = middle;
            }
            middle = (int *)((((phys_ulong)low) >> 1) + (((phys_ulong)high) >> 1));
        }
        if (bits > middle[0]) {
            middle++;
        }
        phys_ulong index = middle - (int *)(&phys_ufloat8_to_real[0]);
        m_value = (unsigned char)(index);
        return *this;
    }
    inline phys_ufloat8(const float f) { *this = f; }
    inline operator float() const { return phys_ufloat8_to_real[m_value]; }

    unsigned char m_value;
};

#include "phys/phys_object.h"
enum {
    PHYS_ARRAY_CAPACITY_MASK = 0x3fffffff,
    PHYS_ARRAY_LOCKED_FLAG = 0x40000000,
    PHYS_ARRAY_DONT_DEALLOCATE_FLAG = 0x80000000
};

struct phys_array_base {
    void *data;
    int size;
    int capacity_and_flags;
};

#include "phys/phys_array_util.h"

namespace phys_small_array_util {
    void reserve_array_storage(void *array, int array_elem_count, int elem_size);
    void grow_array_storage(void *array, int elem_size);
}

template <typename T>
class phys_array {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_ARRAY, phys_array<T>);

    __forceinline phys_array()
        : m_data(0), m_size(0), m_capacity_and_flags(PHYS_ARRAY_DONT_DEALLOCATE_FLAG) {}

    // Allocates exactly n elements.
    __forceinline explicit phys_array(int n)
        : m_data(static_cast<T *>(phys_thread_memory::get_singleton_instance().allocate_chunk(n * static_cast<int>(sizeof(T)), PHYS_MEM_ARRAY))),
          m_size(n), m_capacity_and_flags(n) {}

    __forceinline phys_array(int n, const T &fill)
        : m_data(static_cast<T *>(phys_thread_memory::get_singleton_instance().allocate_chunk(n * static_cast<int>(sizeof(T)), PHYS_MEM_ARRAY))),
          m_size(n), m_capacity_and_flags(n) {
        for (int i = 0; i < n; ++i) {
            m_data[i] = fill;
        }
    }

    __forceinline ~phys_array() {
        release_memory();
    }

    __forceinline int getSize() const { return m_size; }
    __forceinline int get_sector_capacity() const { return m_capacity_and_flags & PHYS_ARRAY_CAPACITY_MASK; }
    __forceinline int get_capacity_and_flags() const { return m_capacity_and_flags; }
    __forceinline phys_bool is_array_empty() const { return phys_bool(m_size == 0); }
    __forceinline void clear() { m_size = 0; }

    __forceinline void release_memory() {
        if ((m_capacity_and_flags & PHYS_ARRAY_DONT_DEALLOCATE_FLAG) == 0) {
            // match: 0xfbbd0, 0x6520d8: retain this capacity local; the direct getter changes instructions in both callers.
            int capacity = m_capacity_and_flags & PHYS_ARRAY_CAPACITY_MASK;
            phys_thread_memory::get_singleton_instance().free_chunk(
                m_data, capacity * static_cast<int>(sizeof(T)), PHYS_MEM_ARRAY);
        }
    }

    // March 2007: releaseMemory, then the locked flag survives.
    __forceinline void clear_and_deallocate() {
        release_memory();
        m_data = 0;
        m_size = 0;
        m_capacity_and_flags = (m_capacity_and_flags & PHYS_ARRAY_LOCKED_FLAG) | PHYS_ARRAY_DONT_DEALLOCATE_FLAG;
    }

    __forceinline T &operator[](int index) { return m_data[index]; }
    __forceinline const T &operator[](int index) const { return m_data[index]; }

    __forceinline T *begin() { return m_data; }
    __forceinline const T *begin() const { return m_data; }

    void swap(phys_array<T> &other) {
        T *data = m_data;
        m_data = other.m_data;
        other.m_data = data;
        int size = m_size;
        m_size = other.m_size;
        other.m_size = size;
        int capacity = m_capacity_and_flags;
        m_capacity_and_flags = other.m_capacity_and_flags;
        other.m_capacity_and_flags = capacity;
    }

    // March 2007 (0x82a6ace0): grows to at least twice the old capacity.
    __forceinline void reserve(int n) {
        const int capacity = get_sector_capacity();
        if (capacity < n) {
            int array_doubled_capacity = 2 * capacity;
            int array_new_capacity = (n < array_doubled_capacity) ? array_doubled_capacity : n;
            phys_array_util::reserve_array_storage(this, array_new_capacity, static_cast<int>(sizeof(T)));
        }
    }

    // Controls whether capacity changes are locked.
    __forceinline void set_locked_flag(bool locked) {
        if (locked)
            m_capacity_and_flags |= PHYS_ARRAY_LOCKED_FLAG;
        else
            m_capacity_and_flags &= ~PHYS_ARRAY_LOCKED_FLAG;
    }

    // March 2007 (0x82ba4490); the locked-array warning is debug only.
    __forceinline void reserve_exactly(int n) {
        if (get_sector_capacity() < n) {
            phys_array_util::reserve_array_storage(this, n, static_cast<int>(sizeof(T)));
        }
    }

    // March 2007 (0x8356fde8); the locked-array warning is debug only.
    __forceinline void optimize_capacity(int free_elems_left) {
        int total_capacity = m_size + free_elems_left;
        if (total_capacity * 2 <= get_sector_capacity()) {
            phys_array_util::shrink_array_storage(this, static_cast<int>(sizeof(T)), 0, 0);
        }
    }

    __forceinline phys_bool is_locked() { return phys_bool((m_capacity_and_flags & PHYS_ARRAY_LOCKED_FLAG) != 0); }

    __forceinline T *end() { return m_data + m_size; }
    __forceinline const T *end() const { return m_data + m_size; }

    static __forceinline void copy(T *dst, const T *src, int n) {
        for (int i = 0; i < n; ++i) {
            dst[i] = src[i];
        }
    }

    static __forceinline void copy_backwards(T *dst, const T *src, int n) {
        for (int i = n - 1; i >= 0; --i) {
            dst[i] = src[i];
        }
    }

    // March 2007 (0x83477258); the locked-array warning is debug only.
    __forceinline phys_array &operator=(const phys_array &other) {
        if (get_sector_capacity() < other.m_size) {
            release_memory();
            m_data = static_cast<T *>(phys_thread_memory::get_singleton_instance().allocate_chunk(
                other.m_size * static_cast<int>(sizeof(T)), PHYS_MEM_ARRAY));
            m_capacity_and_flags = other.m_size | (m_capacity_and_flags & PHYS_ARRAY_LOCKED_FLAG);
        }
        m_size = other.m_size;
        copy(m_data, other.m_data, m_size);
        return *this;
    }

    __forceinline T &back() { return m_data[m_size - 1]; }
    __forceinline const T &back() const { return m_data[m_size - 1]; }
    __forceinline void drop_back(int count = 1) { m_size -= count; }

    // March 2007 (0x83795440): takes the buffer as is, flags included.
    __forceinline void set_owned_data(T *data, int size, int capacity) {
        m_data = data;
        m_size = size;
        m_capacity_and_flags = capacity;
    }

    __forceinline void set_size_unchecked(int n) { m_size = n; }

    __forceinline T *expand_by(int n) {
        int size = m_size;
        set_array_size(size + n);
        return m_data + size;
    }

    __forceinline T *expand_by_unchecked(int n) {
        int size = m_size;
        set_size_unchecked(size + n);
        return m_data + size;
    }

    // Inserts elements before the specified index.
    void insert_at(int index, const T *p, int count);
    void insert_at(int index, const T &t);

    __forceinline void set_array_size(int n) {
        reserve(n);
        m_size = n;
    }

    // March 0x8345a568: reserve, fill the newly added elements, then publish the size.
    __forceinline void set_array_size(int n, const T &fill) {
        if (n > m_size) {
            const int array_old_size = m_size;
            reserve(n);
            for (int i = array_old_size; i < n; ++i) m_data[i] = fill;
        }
        m_size = n;
    }

    // March 0x83480fb0: leave an uninitialized gap before the shifted tail.
    __forceinline T *expand_at(int index, int insert_count) {
        const int array_new_size = insert_count + m_size;
        const int array_move_count = m_size - index;
        if (array_new_size > get_sector_capacity()) reserve(array_new_size);
        copy_backwards(m_data + index + insert_count, m_data + index, array_move_count);
        m_size = array_new_size;
        return m_data + index;
    }

    // March 2007 (0x82a6a9f0): the capacity through getCapacity(); the element stored at the old size.
    __forceinline void array_push_back(const T &value) {
        if (m_size == get_sector_capacity()) {
            phys_array_util::grow_array_storage(this, static_cast<int>(sizeof(T)));
        }
        m_data[m_size++] = value;
    }

    __forceinline void push_back_unchecked(const T &value) {
        m_data[m_size++] = value;
    }

    // March 0x83539698: keep the element reference, then increment the size and return it.
    // match: 0x67e120: the named reference preserves the inlined overlap-test scheduling.
    __forceinline T &expand_one() {
        if (m_size == (m_capacity_and_flags & PHYS_ARRAY_CAPACITY_MASK)) {
            phys_array_util::grow_array_storage(this, static_cast<int>(sizeof(T)));
        }
        T &element = m_data[m_size];
        m_size++;
        return element;
    }

    __forceinline void remove_at(int index) {
        m_size--;
        m_data[index] = m_data[m_size];
    }

    __forceinline void remove_at_and_copy(int index) {
        --m_size;
        for (int i = index; i < m_size; ++i) {
            T *next = &m_data[i + 1];
            m_data[i] = *next;
        }
    }

    __forceinline int array_index_of(const T &t) const {
        for (int i = 0; i < m_size; ++i) {
            if (m_data[i] == t) {
                return i;
            }
        }
        return -1;
    }

    __forceinline int last_index_of(const T &t) const {
        for (int i = m_size - 1; i >= 0; --i) {
            if (m_data[i] == t) {
                return i;
            }
        }
        return -1;
    }

    T *m_data;
    int m_size;
    int m_capacity_and_flags;

    // March 2007 (0x82c33848): a loaded array keeps its fields as they are.
    __forceinline phys_array(phys_fixup_tag) {}

    // Uses a caller-owned buffer without taking responsibility for freeing it.
    __forceinline phys_array(T *buffer, int size, int capacity)
        : m_data(buffer), m_size(size), m_capacity_and_flags(capacity | PHYS_ARRAY_DONT_DEALLOCATE_FLAG) {}
};

// match: 0x6ae030: xdk5632 Havok 4.5 releaseMemory calls getCapacity() inside deallocateChunk without the local kept above for xdk6130.
template <>
__forceinline void phys_array<phys_bool>::release_memory() {
    if ((m_capacity_and_flags & PHYS_ARRAY_DONT_DEALLOCATE_FLAG) == 0) {
        phys_thread_memory::get_singleton_instance().free_chunk(
            m_data, get_sector_capacity() * static_cast<int>(sizeof(phys_bool)), PHYS_MEM_ARRAY);
    }
}

template <typename T>
void phys_array<T>::insert_at(int index, const T *p, int count) {
    const int array_new_size = count + m_size;
    const int array_move_count = m_size - index;
    if (array_new_size > get_sector_capacity()) {
        reserve(array_new_size);
    }
    copy_backwards(m_data + index + count, m_data + index, array_move_count);
    copy_backwards(m_data + index, p, count);
    m_size = array_new_size;
}

template <typename T>
void phys_array<T>::insert_at(int index, const T &t) {
    insert_at(index, &t, 1);
}

template <typename T, unsigned N>
class phys_inline_array : public phys_array<T> {
public:
    typedef phys_inline_array<T, N> self_type;
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_ARRAY, self_type);

    __forceinline phys_inline_array(int size = 0)
        : phys_array<T>(m_storage, size, static_cast<int>(N)) {}

    __forceinline ~phys_inline_array() {}

    // March 2007 (0x834d6d20): retain in-place storage when reducing capacity.
    __forceinline void optimize_capacity(int free_elems_left) {
        if ((this->m_capacity_and_flags & PHYS_ARRAY_DONT_DEALLOCATE_FLAG) == 0) {
            int total_capacity = this->m_size + free_elems_left;
            if (total_capacity < static_cast<int>(N) || total_capacity * 2 < this->get_sector_capacity()) {
                phys_array_util::shrink_array_storage(this, sizeof(T), reinterpret_cast<char *>(&m_storage[0]), static_cast<int>(N));
            }
        }
    }

    inline phys_array<T> &operator=(const phys_inline_array<T, N> &other) {
        return phys_array<T>::operator=(other);
    }

    T m_storage[N];
};

template <typename T>
class phys_small_array {
public:
    enum {
        ARRAY_CAPACITY_BITS_MASK = 0x3fff,
        LOCK_BIT_FLAG = 0x4000,
        ARRAY_NO_FREE_FLAG = 0x8000
    };

    __forceinline phys_small_array() : m_data(0), m_size(0), m_capacity_and_flags(ARRAY_NO_FREE_FLAG) {}
    __forceinline ~phys_small_array() { release_memory(); }
    __forceinline int getSize() const { return m_size; }
    __forceinline int get_sector_capacity() const { return m_capacity_and_flags & ARRAY_CAPACITY_BITS_MASK; }
    __forceinline phys_bool is_array_empty() const { return m_size == 0; }
    __forceinline T &operator[](int i) { return m_data[i]; }
    __forceinline const T &operator[](int i) const { return m_data[i]; }
    __forceinline T *begin() { return m_data; }
    __forceinline const T *begin() const { return m_data; }
    __forceinline T *end() { return m_data + m_size; }
    __forceinline const T *end() const { return m_data + m_size; }

    __forceinline T &expand_one() {
        if (m_size == get_sector_capacity()) {
            phys_small_array_util::grow_array_storage(this, sizeof(T));
        }
        return m_data[m_size++];
    }

    // March 0x834b3fd8, 0x834c4858.
    __forceinline void array_push_back(const T &element) {
        if (m_size == get_sector_capacity()) phys_small_array_util::grow_array_storage(this, sizeof(T));
        m_data[m_size] = element;
        ++m_size;
    }
    __forceinline void remove_at(int index) {
        --m_size;
        m_data[index] = m_data[m_size];
    }

    __forceinline void remove_at_and_copy(int index) {
        m_size--;
        for (int i = index; i < m_size; ++i) {
            m_data[i] = m_data[i + 1];
        }
    }

    __forceinline int array_index_of(const T &t) const {
        for (int i = 0; i < m_size; ++i) {
            if (m_data[i] == t) {
                return i;
            }
        }
        return -1;
    }

    // March 2007: releaseMemory, then the locked flag survives.
    __forceinline void clear_and_deallocate() {
        release_memory();
        m_data = 0;
        m_size = 0;
        m_capacity_and_flags = static_cast<unsigned short>((m_capacity_and_flags & LOCK_BIT_FLAG) | ARRAY_NO_FREE_FLAG);
    }

    __forceinline phys_bool is_locked() { return phys_bool((m_capacity_and_flags & LOCK_BIT_FLAG) != 0); }

    // March 2007 (0x835053c0, 0x83505318).
    __forceinline T &back() { return m_data[m_size - 1]; }
    __forceinline void drop_back(int remove_count = 1) { m_size = static_cast<unsigned short>(m_size - remove_count); }

    // March 2007 (0x83505940): grows to at least twice the old capacity; the locked-array warning is debug only.
    __forceinline void reserve(int n) {
        const int capacity = get_sector_capacity();
        if (capacity < n) {
            int array_doubled_capacity = 2 * capacity;
            int array_new_capacity = (n < array_doubled_capacity) ? array_doubled_capacity : n;
            phys_small_array_util::reserve_array_storage(this, array_new_capacity, static_cast<int>(sizeof(T)));
        }
    }

    static __forceinline void copy_backwards(T *dst, const T *src, int n) {
        for (int i = n - 1; i >= 0; --i) {
            dst[i] = src[i];
        }
    }

    // Inserts elements before the specified index.
    void insert_at(int index, const T *p, int count);
    void insert_at(int index, const T &t);

protected:
    __forceinline void release_memory() {
        if ((m_capacity_and_flags & ARRAY_NO_FREE_FLAG) == 0) {
            phys_thread_memory::get_instance_ptr()->free_chunk(
                m_data, get_sector_capacity() * static_cast<int>(sizeof(T)), PHYS_MEM_ARRAY);
        }
    }

public:
    T *m_data;
    unsigned short m_size;
    unsigned short m_capacity_and_flags;
};

template <typename T>
void phys_small_array<T>::insert_at(int index, const T *p, int count) {
    const int array_new_size = count + m_size;
    const int array_move_count = m_size - index;
    if (array_new_size > get_sector_capacity()) {
        reserve(array_new_size);
    }
    copy_backwards(m_data + index + count, m_data + index, array_move_count);
    copy_backwards(m_data + index, p, count);
    m_size = static_cast<unsigned short>(array_new_size);
}

template <typename T>
void phys_small_array<T>::insert_at(int index, const T &t) {
    insert_at(index, &t, 1);
}

#include "phys/phys_fixed_array.h"

#include "phys/phys_math.h"

typedef char havok_size_hkBaseObject[(sizeof(void *) != 4 || ((sizeof(phys_object) == 4) )) ? 1 : -1];
typedef char havok_size_hkReferencedObject[(sizeof(void *) != 4 || ((sizeof(phys_ref_object) == 8) )) ? 1 : -1];
typedef char havok_size_hkArrayBase[(sizeof(void *) != 4 || ((sizeof(phys_array_base) == 12) )) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_BASE_H
