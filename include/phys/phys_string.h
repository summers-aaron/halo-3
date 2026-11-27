#ifndef INCLUDE_PHYS_STRING_H
#define INCLUDE_PHYS_STRING_H

#include "cseries/crt_xdk.h"
#include "phys/phys_base_types.h"

class phys_string {
public:
  struct string_rep {
    int length;
    int capacity;
    int reference_count;
    char data[1];

    void release_string_memory();
    __forceinline void release_ref() { if (--reference_count < 0) release_string_memory(); }
    __forceinline char *add_ref_copy() { ++reference_count; return data; }
    string_rep *clone() const;
    static string_rep *create(int count);
  };

  __forceinline phys_string() : data(((string_rep *)s_emptyStringRep)->add_ref_copy()) {}
  __forceinline phys_string(const char *text) {
    if (text) {
      int length = str_len(text);
      data = string_rep::create(length)->data;
      mem_copy(data, text, length + 1);
    } else {
      data = ((string_rep *)s_emptyStringRep)->add_ref_copy();
    }
  }
  __forceinline phys_string(const phys_string &other) : data(other.get_rep()->add_ref_copy()) {}
  __forceinline ~phys_string() { get_rep()->release_ref(); }
  char *data;
  __forceinline int get_string_length() const { return get_rep()->length; }
  __forceinline const char *string_c_str() const { return data; }
  // March 2007 (0x8345cbb0).
  __forceinline char operator[](int index) const { return data[index]; }
  __forceinline string_rep *get_rep() const { return (string_rep *)(data - offsetof(string_rep, data)); }
  // March 2007 (0x8345cae0): retain the new representation before releasing the old one.
  __forceinline phys_string &operator=(const phys_string &other) {
    char *text = other.get_rep()->add_ref_copy();
    get_rep()->release_ref();
    data = text;
    return *this;
  }
  __forceinline phys_string &operator=(const char *text) {
    if (text && *text) {
      int length = str_len(text);
      if (get_rep()->capacity < length || get_rep()->reference_count > 0) {
        get_rep()->release_ref();
        data = string_rep::create(length)->data;
      }
      mem_copy(data, text, length + 1);
      get_rep()->length = length;
    } else {
      get_rep()->release_ref();
      data = ((string_rep *)s_emptyStringRep)->add_ref_copy();
    }
    return *this;
  }

  static char string_to_upper(char c);
  static char string_to_lower(char c);
  static int sprintf(char *, const char *, ...);
  static int format_string_n(char *, int, const char *, ...);
  static int vsnprintf(char *, int, const char *, void *);
  void printf(const char *, ...);
  static int str_cmp(const char *, const char *);
  static int str_ncmp(const char *, const char *, int);
  static void str_cpy(char *, const char *);
  static void str_ncpy(char *, const char *, int);
  static int atoi(const char *);
  static __forceinline const char *str_str(const char *, const char *);
  static const char *str_chr(const char *, int);
  static const char *str_rchr(const char *, int);
  static void mem_copy(void *, const void *, int);
  static void mem_move(void *, const void *, int);
  static void mem_fill(void *, int, int);
  int array_index_of(char, int, int) const;
  int last_index_of(char, int, int) const;
  phys_bool begins_with(const phys_string &) const;
  phys_string operator+(const phys_string &) const;
  phys_string operator+(const char *) const;
  phys_string &operator+=(const phys_string &);
  phys_string &operator+=(const char *);
  __forceinline phys_string substr(int, int) const;
  enum replace_mode { STRING_REPLACE_FIRST, STRING_REPLACE_ALL };
  phys_string replace(const phys_string &, const phys_string &, replace_mode) const;
  static int str_casecmp(const char *a, const char *b);
  static int str_ncasecmp(const char *a, const char *b, int n);
  static char *str_lwr(char *string);
  static char *str_upr(char *string);
  static int mem_compare(const void *a, const void *b, int n);
  static char *str_dup(const char *string);
  static char *str_ndup(const char *string, int count);
  void set_capacity(int capacity);
  __forceinline void set_string_length(int length);

  phys_bool begins_with(const char *prefix) const;
  phys_bool string_ends_with(const phys_string &suffix) const;
  phys_bool string_ends_with(const char *suffix) const;
  phys_string to_lower_case() const;
  phys_string to_upper_case() const;

private:
  __forceinline explicit phys_string(string_rep *string_storage_rep) : data(string_storage_rep->data) {}
  /* the empty string's Rep: zero length, capacity and reference count, then the terminator */
  static char s_emptyStringRep[13];
public:
    static float atof(const char *string);
    phys_string replace(char from, char to, replace_mode mode) const;
    static void mem_clear_bytes(void *dst, int n);
    static int str_len(const char *s);
    // March 0x83517230: numWords scalar stores, advancing through the destination.
    static __forceinline void mem_fill4(void *dst, int value, int fill_word_count) {
        int *d = static_cast<int *>(dst);
        for (int i = fill_word_count - 1; i >= 0; --i) *d++ = value;
    }
    // March 0x834ee238: copy numWords 32-bit words.
    static __forceinline void mem_copy4(void *dst, const void *src, int fill_word_count) {
        const unsigned int *s = static_cast<const unsigned int *>(src);
        unsigned int *d = static_cast<unsigned int *>(dst);
        for (int i = 0; i < fill_word_count; ++i) *d++ = *s++;
    }
    // March 2007 (hkString::memCpy16): numQuads 16-byte blocks copied as words.
    static __forceinline void mem_copy16(void *dst, const void *src, int copy_quad_count) {
        const unsigned int *src_u32 = reinterpret_cast<const unsigned int *>(src);
        unsigned int *dst_u32 = reinterpret_cast<unsigned int *>(dst);
        {
            for (int i = 0; i < copy_quad_count; i++) {
                dst_u32[0] = src_u32[0];
                dst_u32[1] = src_u32[1];
                dst_u32[2] = src_u32[2];
                dst_u32[3] = src_u32[3];
                dst_u32 += 4;
                src_u32 += 4;
            }
        }
    }

    // numQuads (at least one) 16-byte blocks copied as words.
    static __forceinline void mem_copy16_nonempty(void *dst, const void *src, int copy_quad_count) {
        const unsigned int *src_u32 = reinterpret_cast<const unsigned int *>(src);
        unsigned int *dst_u32 = reinterpret_cast<unsigned int *>(dst);
        {
            do {
                dst_u32[0] = src_u32[0];
                dst_u32[1] = src_u32[1];
                dst_u32[2] = src_u32[2];
                dst_u32[3] = src_u32[3];
                dst_u32 += 4;
                src_u32 += 4;
            } while (--copy_quad_count > 0);
        }
    }

    // numQuads (at least one) 16-byte blocks copied as words; the count is unsigned.
    static __forceinline void mem_copy16_nonempty(void *dst, const void *src, unsigned int copy_quad_count) {
        const unsigned int *src_u32 = reinterpret_cast<const unsigned int *>(src);
        unsigned int *dst_u32 = reinterpret_cast<unsigned int *>(dst);
        {
            do {
                dst_u32[0] = src_u32[0];
                dst_u32[1] = src_u32[1];
                dst_u32[2] = src_u32[2];
                dst_u32[3] = src_u32[3];
                dst_u32 += 4;
                src_u32 += 4;
            } while (--copy_quad_count);
        }
    }

    // A fixed size (a multiple of 16, at most 128) copied in 16-byte blocks.
    template <int size>
    static __forceinline void mem_copy16(void *dst, const void *src) {
        phys_string::mem_copy16_nonempty(dst, src, size / 16);
    }
};

#endif // INCLUDE_HAVOK_HK_STRING_H
