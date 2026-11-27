#ifndef MEMORY_STATIC_ARRAYS_H
#define MEMORY_STATIC_ARRAYS_H

// source\memory\static_arrays.h (March 2007 debug asserts name this file).
// Fixed-capacity arrays, narrow/wide strings and bit vectors. Classes are in the file's order:
// c_static_array (asserts at lines 25-28), c_static_stack (61-73),
// c_static_string (713-885), c_static_wchar_string (1079-1193), [c_flags, 1263-1385: in cseries/cseries_wrapped.h
// for now], c_static_flags_no_init (1486-1785: in memory/static_arrays_flags.h for now), c_static_flags.
// Debug asserts are omitted.
// The C runtime string functions come from <string.h>: including it moves no function (lane P1 measurement).

#include <string.h>
#include "memory/static_arrays_flags.h"

// No user-declared constructor: an array of class elements gets the compiler's
// ??0?$c_static_array@...@@QAA@XZ (a vector constructor iterator), as in March.
template<typename t_type, long k_count>
class c_static_array {
public:
    __forceinline bool valid(long index) const { return index >= 0 && index < k_count; }
    __forceinline const t_type &element(long index) const { return m_storage[index]; }
    __forceinline t_type &element(long index) { return m_storage[index]; }
    __forceinline const t_type &operator[](long index) const { return m_storage[index]; }
    __forceinline t_type &operator[](long index) { return m_storage[index]; }

    __forceinline long get_count() const { return k_count; }
    __forceinline long get_element_size() const { return sizeof(t_type); }
    __forceinline long get_total_element_size() const { return sizeof(m_storage); }
    __forceinline t_type *get_elements() { return m_storage; }
    __forceinline const t_type *get_elements() const { return m_storage; }
    __forceinline t_type *begin() { return m_storage; }
    __forceinline const t_type *begin() const { return m_storage; }
    __forceinline t_type *end() { return m_storage + k_count; }

    __forceinline void set_all(const t_type &value) {
        for (long index = 0; index < k_count; ++index)
            m_storage[index] = value;
    }
    // March: qsort(m_storage, count, sizeof(t_type), compare).
    void sort(long count, int (__cdecl *compare)(const void *, const void *));

    t_type m_storage[k_count];
};

// March: m_count at 0, then a raw array (the destructor runs the vector destructor iterator on this+4).
// valid(), valid(index), empty, full, resize, push, pop, top, operator[] and get assert at lines 61-73.
template<typename t_type, long k_count>
class c_static_stack {
public:
    __forceinline c_static_stack() : m_count(0) {}

    __forceinline bool valid() const { return m_count >= 0 && m_count <= k_count; }
    __forceinline bool valid(long index) const { return index >= 0 && index < m_count; }
    __forceinline bool empty() const { return m_count == 0; }
    __forceinline bool full() const { return m_count == k_count; }
    __forceinline void resize(long count) { m_count = count; }
    __forceinline long push() { return m_count++; }
    __forceinline void pop() { m_count--; }
    __forceinline long top() const { return m_count - 1; }
    __forceinline const t_type &operator[](long index) const { return m_storage[index]; }
    __forceinline t_type &operator[](long index) { return m_storage[index]; }
    __forceinline const t_type *get(long index) const { return &m_storage[index]; }
    __forceinline t_type *get(long index) { return &m_storage[index]; }
    __forceinline t_type *get_top() { return get(top()); }
    // March's /Od body spills get()'s result to a stack slot (t_type *element = get(push()); *element = value);
    // Push and store in one expression.
    __forceinline void push_back(const t_type &value) { *get(push()) = value; }
    __forceinline long count() const { return m_count; }
    __forceinline void clear() { m_count = 0; }
    __forceinline t_type *get_elements() { return m_storage; }
    __forceinline const t_type *begin() const { return m_storage; }
    __forceinline const t_type *end() const { return m_storage + m_count; }

    long m_count;
    t_type m_storage[k_count];
};

// cseries.h function (March); defined here because cseries.h includes this file and units include this file alone.
static __forceinline char *csstrnzcpy(char *destination, const char *source, unsigned int size) {
    strncpy(destination, source, size);
    destination[size - 1] = 0;
    return destination;
}

template<long k_maximum_count>
class c_static_string {
public:
    static const long element_count = k_maximum_count;

    __forceinline c_static_string() { m_string[0] = 0; }
    __forceinline c_static_string(const char *source) { set(source); }
    __forceinline c_static_string(const wchar_t *source) { set_wchar(source); }

    __forceinline void clear() { m_string[0] = 0; }
    __forceinline void set(const char *source) { csstrnzcpy(m_string, source, k_maximum_count); }
    // March: csstrnzcpy(m_string, source, MIN(length + 1, NUMBEROF(m_string))), an unsigned compare.
    __forceinline void set_bounded(const char *source, long length) {
        long size = static_cast<unsigned long>(length + 1) < k_maximum_count ? length + 1 : k_maximum_count;
        csstrnzcpy(m_string, source, size);
    }
    __forceinline void null_terminate_buffer() { m_string[k_maximum_count - 1] = 0; }
    __forceinline void set_length(long length) {
        if (length >= 0 && length < k_maximum_count - 1)
            m_string[length] = 0;
    }

    __forceinline const char *get_string() const { return m_string; }
    __forceinline operator const char *() const { return m_string; }
    __forceinline char (&get_buffer())[k_maximum_count] { return m_string; }
    __forceinline unsigned int max_length() const { return k_maximum_count; }
    __forceinline bool is_empty() const { return m_string[0] == 0; }
    // March: csstrncmp (strncmp_debug in the debug build).
    __forceinline bool is_equal(const char *source) const {
        return strncmp(m_string, source, k_maximum_count) == 0;
    }

    __forceinline void replace(char original, char replacement) {
        for (long index = 0; index < k_maximum_count; index++) {
            if (m_string[index] == original)
                m_string[index] = replacement;
        }
    }

    // Declared here; defined in the owning source file.
    // March bodies (static_arrays.h line of the assert in brackets):
    //   print [713]: ASSERT(format); va_start; cvsnzprintf(m_string, k_maximum_count, format, list); va_end; return m_string
    //   append_print [740, 741]: long current_length = length(); ASSERT(format);
    //     ASSERT(current_length>=0 && current_length<NUMBEROF(m_string));
    //     cvsnzprintf(m_string + current_length, k_maximum_count - current_length, format, list); return m_string
    //   append: csstrnzcat(m_string, source, k_maximum_count); return m_string
    //   length: return csstrnlen(m_string, k_maximum_count)
    //   starts_with [876]: ASSERT(string); long length = csstrnlen(string, k_maximum_count);
    //     return csmemcmp(string, get_string(), length) == 0
    //   set_wchar: wchar_string_to_ascii_string(source, m_string, k_maximum_count, NULL)
    const char *print(const char *format, ...);
    char *append_print(const char *format, ...); // Returns the formatted string; treat it as read-only.
    const char *append(const char *source);
    long length() const;
    bool starts_with(const char *prefix) const;
    __forceinline bool substring(long index, long count, c_static_string<k_maximum_count> *result_string) const {
        bool result = false;
        if (index >= 0 && count > 0 && index + count <= length()) {
            long size = count + 1 > k_maximum_count ? k_maximum_count : count + 1;
            csstrnzcpy(result_string->m_string, get_string() + index, size);
            result = true;
        }
        return result;
    }
    void set_wchar(const wchar_t *source);
    void trim_end();
    // Not used by any unit yet; March bodies:
    //   print_va: cvsnzprintf(m_string, k_maximum_count, format, list)
    //   ends_with [885]: ASSERT(string); long length = csstrnlen(string, k_maximum_count);
    //     return csmemcmp(string, get_string() + this->length() - length, length) == 0
    //   (index_of, next_index_of [857]: defined below)
    //   to_lower: for each of k_maximum_count chars: m_string[i] = ascii_tolower(m_string[i])
    //   copy_wchar_string_to_buffer: c_static_string<N> s; s.set_wchar(source); s.copy_to(buffer, N)
    void print_va(const char *format, char *list);
    __forceinline char *copy_to(char *buffer, unsigned int size) const {
        size = size > k_maximum_count ? k_maximum_count : size;
        return csstrnzcpy(buffer, m_string, size);
    }
    bool ends_with(const char *string) const;
    __forceinline long index_of(const char *string) const { return next_index_of(string, 0); }
    __forceinline long next_index_of(const char *string, long index) const {
        long result = -1;
        if (index < length()) {
            const char *found = strstr(m_string + index, string);
            if (found)
                result = found - get_string();
        }
        return result;
    }
    void to_lower();
    static __forceinline void copy_wchar_string_to_buffer(const wchar_t *source, char (&buffer)[k_maximum_count]) {
        c_static_string<k_maximum_count> string;
        string.set_wchar(source);
        string.copy_to(buffer, k_maximum_count);
    }

    char m_string[k_maximum_count];
};

template<long k_maximum_count>
class c_static_wchar_string {
public:
    static const int k_buffer_size = k_maximum_count;

    __forceinline c_static_wchar_string() { m_string[0] = 0; }
    __forceinline c_static_wchar_string(const wchar_t *source) { set(source); }

    __forceinline void clear() { m_string[0] = 0; }
    __forceinline const wchar_t *get_string() const { return m_string; }
    __forceinline operator const wchar_t *() const { return m_string; }
    __forceinline wchar_t *get_buffer() { return m_string; } // not in March
    __forceinline bool is_empty() const { return m_string[0] == 0; } // March [1175]: ASSERT(k_maximum_count >= 2)
    __forceinline const wchar_t *append(const wchar_t *source) {
        wchar_t *ustrnzcat(wchar_t *destination, const wchar_t *source, long size);
        ustrnzcat(m_string, source, k_maximum_count);
        m_string[k_maximum_count - 1] = 0;
        return m_string;
    }
    __forceinline void replace(wchar_t original, wchar_t replacement) {
        for (long index = 0; index < k_maximum_count; index++) {
            if (m_string[index] == original)
                m_string[index] = replacement;
        }
    }

    // Declarations only; expected behavior:
    //   print [1153]: ASSERT(format); uvsnzprintf(m_string, k_maximum_count, format, list); return m_string
    //   append_print [1079, 1080]: as c_static_string::append_print, with uvsnzprintf
    //   set: ustrnzcpy(m_string, source, k_maximum_count)
    //   set_char: ascii_string_to_wchar_string(source, m_string, k_maximum_count, NULL)
    //   length: return ustrnlen(m_string, k_maximum_count)
    const wchar_t *print(const wchar_t *format, ...);
    const wchar_t *append_print(const wchar_t *format, ...);
    void set(const wchar_t *source);
    void set_char(const char *source);
    long length() const;
    void trim();
    // Not used by any unit yet; March bodies:
    //   set_utf8: utf8_string_to_wchar_string(source, m_string, k_maximum_count, NULL)
    //   is_equal: return ustrncmp(m_string, string, k_maximum_count) == 0
    //   (index_of, next_index_of [1193]: defined below; March calls wcsstr)
    void set_utf8(const struct utf8 *source);
    __forceinline bool is_equal(const wchar_t *string) const {
        return wcsncmp(m_string, string, k_maximum_count) == 0;
    }
    // March: the clamp is a ?: whose value goes through a temp (MIN).
    __forceinline wchar_t *copy_to(wchar_t *buffer, unsigned int size) const {
        wchar_t *ustrnzcpy(wchar_t *destination, const wchar_t *source, long size);
        size = size > k_maximum_count ? k_maximum_count : size;
        return ustrnzcpy(buffer, m_string, size);
    }
    __forceinline long index_of(const wchar_t *string) const { return next_index_of(string, 0); }
    __forceinline long next_index_of(const wchar_t *string, long index) const {
        long result = -1;
        if (index < length()) {
            const wchar_t *found = wcsstr(m_string + index, string);
            if (found)
                result = found - get_string();
        }
        return result;
    }

    wchar_t m_string[k_maximum_count];
};

// Defined in tu/blamlibxbox/saved_game_files.cpp.
template<>
void c_static_wchar_string<1024>::set_char(const char *source);

// c_static_flags_no_init (1486-1785) is in memory/static_arrays_flags.h for now: object_datum.h holds one
// and cannot include this file while units keep private copies of c_static_array (game_engine_king.cpp).

// March: the base is at offset 0 (fill() is called with this unadjusted); the default
// constructor is empty, the other one is fill(count, value).
template<long k_maximum_count>
class c_static_flags : public c_static_flags_no_init<k_maximum_count> {
public:
    __forceinline c_static_flags() {}
    __forceinline c_static_flags(long count, unsigned char value) { this->fill(count, value); }
};

typedef char static_arrays_size_flags65[(sizeof(c_static_flags<65>) == 12) ? 1 : -1];
typedef char static_arrays_size_array3[(sizeof(c_static_array<long, 3>) == 12) ? 1 : -1];
typedef char static_arrays_size_stack3[(sizeof(c_static_stack<long, 3>) == 16) ? 1 : -1];
typedef char static_arrays_size_cstr7[(sizeof(c_static_string<7>) == 7) ? 1 : -1];
typedef char static_arrays_size_wstr7[(sizeof(c_static_wchar_string<7>) == 14) ? 1 : -1];

#endif // MEMORY_STATIC_ARRAYS_H
