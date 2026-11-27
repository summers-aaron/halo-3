// hkbase/hkString.obj: from the records 0x60e1c0 0x60e1e0 0x60e200 0x60e208 0x60e250 0x60e298 0x60e2c8 0x60e2d0 0x60e3b8 0x60e4b8 0x60e4d8 0x60e4e8 0x60e510 0x60e520 0x60e548 0x60e550 0x60e558 0x60e560 0x60e5a8 0x60e5f0 0x60e5f8 0x60e600 0x60e608 0x60e648 0x60e690 0x60e6d8 0x60e730 0x60e790 0x60e820 0x60e8a8 0x60e930 0x60e988 0x60ea10 0x60ea78 0x60ea80 0x60eaf8 0x60ecf8 0x60ed90 0x60ee48 0x60ef50 0x60f070 0x60f108 0x60f1a8 0x60f248 0x60f2d0
// @flags /TP /O2 /Oc
#include <stdarg.h>
#include <stdio.h>
#include "phys/phys_base.h"
#include "phys/phys_memory_class.h"
#include <string.h>
#include <stdlib.h>
#include "phys/phys_thread_memory.h"
// @address 0x60e250 ?sprintf@phys_string@@SAHPADPBDZZ
// @address 0x60e2d0 ?str_casecmp@phys_string@@SAHPBD0@Z
// @address 0x60e3b8 ?str_ncasecmp@phys_string@@SAHPBD0H@Z
// @address 0x60e4e8 ?str_len@phys_string@@SAHPBD@Z
// @address 0x60e520 ?atof@phys_string@@SAMPBD@Z
// @address 0x60e560 ?str_lwr@phys_string@@SAPADPAD@Z
// @address 0x60e5a8 ?str_upr@phys_string@@SAPADPAD@Z
// @address 0x60e608 ?mem_compare@phys_string@@SAHPBX0H@Z
// @address 0x60e6d8 ?begins_with@phys_string@@QBA?AVphys_bool@@PBD@Z
// @address 0x60e730 ?string_ends_with@phys_string@@QBA?AVphys_bool@@ABV1@@Z
// @address 0x60e790 ?string_ends_with@phys_string@@QBA?AVphys_bool@@PBD@Z
// @address 0x60e820 ?str_dup@phys_string@@SAPADPBD@Z
// @address 0x60e930 ?release_string_memory@string_rep@phys_string@@QAAXXZ
// @address 0x60e988 ?clone@string_rep@phys_string@@QBAPAU12@XZ
// @address 0x60ea10 ?create@string_rep@phys_string@@SAPAU12@H@Z
// @address 0x60f108 ?to_upper_case@phys_string@@QBA?AV1@XZ
// @address 0x60f1a8 ?to_lower_case@phys_string@@QBA?AV1@XZ
// @address 0x60ed90 ??Hphys_string@@QBA?AV0@PBD@Z
// @address 0x60f248 ?replace@phys_string@@QBA?AV1@DDW4replace_mode@1@@Z
// @address 0x60e1c0 ?string_to_upper@phys_string@@SADD@Z
// @address 0x60e1e0 ?string_to_lower@phys_string@@SADD@Z
// @address 0x60e200 ?vsnprintf@phys_string@@SAHPADHPBDPAX@Z
// @address 0x60e298 ?str_cmp@phys_string@@SAHPBD0@Z
// @address 0x60e2c8 ?str_ncmp@phys_string@@SAHPBD0H@Z
// @address 0x60e4b8 ?str_cpy@phys_string@@SAXPADPBD@Z
// @address 0x60e4d8 ?str_ncpy@phys_string@@SAXPADPBDH@Z
// @address 0x60e510 ?atoi@phys_string@@SAHPBD@Z
// @address 0x60e548 ?str_str@phys_string@@SAPBDPBD0@Z
// @address 0x60e550 ?str_chr@phys_string@@SAPBDPBDH@Z
// @address 0x60e558 ?str_rchr@phys_string@@SAPBDPBDH@Z
// @address 0x60e5f0 ?mem_copy@phys_string@@SAXPAXPBXH@Z
// @address 0x60e5f8 ?mem_move@phys_string@@SAXPAXPBXH@Z
// @address 0x60e600 ?mem_fill@phys_string@@SAXPAXHH@Z
// @address 0x60e648 ?array_index_of@phys_string@@QBAHDHH@Z
// @address 0x60e690 ?last_index_of@phys_string@@QBAHDHH@Z
// @address 0x60ea78 ?begins_with@phys_string@@QBA?AVphys_bool@@ABV1@@Z
// @address 0x60ecf8 ??Hphys_string@@QBA?AV0@ABV0@@Z
// @address 0x60ee48 ??Yphys_string@@QAAAAV0@ABV0@@Z
// @address 0x60ef50 ??Yphys_string@@QAAAAV0@PBD@Z
// @address 0x60f070 ?substr@phys_string@@QBA?AV1@HH@Z
// @address 0x60f2d0 ?replace@phys_string@@QBA?AV1@ABV1@0W4replace_mode@1@@Z
#pragma intrinsic(strcmp)
#pragma intrinsic(strcpy)
#pragma intrinsic(strlen)
#pragma intrinsic(memcmp)

typedef char *va_list;


static __forceinline char to_lower(char c)
{
  if (c >= 'A' && c <= 'Z')
    return c + 32;
  return c;
}

static __forceinline char char_to_upper(char c)
{
  if (c >= 'a' && c <= 'z')
    return c - 32;
  return c;
}


static __forceinline void copy_cstr(char *destination, const char *source)
{
  const char *s = source;
  char c;
  do {
    c = *s;
    destination[s - source] = c;
    s++;
  } while (c != 0);
}



char phys_string::s_emptyStringRep[13] = { 0 };

extern "C" {
}

// 0x60e1c0
char phys_string::string_to_upper(char c)
{
  if (c >= 'a' && c <= 'z') return c - 32;
  return c;
}

// 0x60e1e0
char phys_string::string_to_lower(char c)
{
  if (c >= 'A' && c <= 'Z') return c + 32;
  return c;
}

// 0x60e200
int phys_string::vsnprintf(char *buffer, int count, const char *format, void *args)
{
  va_list arguments = (va_list)args;
  int result = _vsnprintf(buffer, count, format, arguments);
  return result;
}

// @address 0x60e208 ?format_string_n@phys_string@@SAHPADHPBDZZ
int phys_string::format_string_n(char *buffer, int count, const char *format, ...)
{
    va_list arglist;
    __va_start(&arglist, &format);
#pragma warning(push)
#pragma warning(disable:4996)
    return _vsnprintf(buffer, count, format, arglist);
#pragma warning(pop)
}

int phys_string::sprintf(char *buf, const char *format_str, ...)
{
    va_list arglist;
    __va_start(&arglist, &format_str);
    return vsprintf(buf, format_str, arglist);
}

// 0x60e298
int phys_string::str_cmp(const char *a, const char *b)
{
  return strcmp(a, b);
}

// 0x60e2c8
int phys_string::str_ncmp(const char *a, const char *b, int count)
{
  return strncmp(a, b, count);
}

int phys_string::str_casecmp(const char *a, const char *b)
{
    int offset = a - b;
    for (; b[offset] != 0 || *b != 0; b++) {
        if (to_lower(b[offset]) < to_lower(*b)) return -1;
        if (to_lower(b[offset]) > to_lower(*b)) return 1;
    }
    return 0;
}

int phys_string::str_ncasecmp(const char *a, const char *b, int n)
{
  for (int i = 0; (a[i] != 0 || b[i] != 0) && i < n; i++) {
    if (to_lower(a[i]) < to_lower(b[i]))
      return -1;
    if (to_lower(a[i]) > to_lower(b[i]))
      return 1;
  }
  return 0;
}

// 0x60e4b8
void phys_string::str_cpy(char *destination, const char *source)
{
  strcpy(destination, source);
}

// 0x60e4d8
void phys_string::str_ncpy(char *destination, const char *source, int count)
{
  if (count != 0) strncpy(destination, source, count);
}

// 0x60e4e8
int phys_string::str_len(const char *s)
{
    return strlen(s);
}

// 0x60e510
int phys_string::atoi(const char *string)
{
  return strtoul(string, 0, 0);
}

float phys_string::atof(const char *string)
{
  return (float)strtod(string, 0);
}

// 0x60e548
__forceinline const char *phys_string::str_str(const char *string, const char *substring)
{
  return strstr(string, substring);
}

// 0x60e550
const char *phys_string::str_chr(const char *string, int character)
{
  return strchr(string, character);
}

// 0x60e558
const char *phys_string::str_rchr(const char *string, int character)
{
  return strrchr(string, character);
}

char *phys_string::str_lwr(char *string)
{
  if (*string != 0) {
    char *p = string;
    do {
      *p = to_lower(*p);
      p++;
    } while (*p != 0);
  }
  return string;
}

char *phys_string::str_upr(char *string)
{
  if (*string != 0) {
    char *p = string;
    do {
      *p = char_to_upper(*p);
      p++;
    } while (*p != 0);
  }
  return string;
}

// 0x60e5f0
void phys_string::mem_copy(void *destination, const void *source, int count)
{
  memcpy(destination, source, count);
}

// 0x60e5f8
void phys_string::mem_move(void *destination, const void *source, int count)
{
  memmove(destination, source, count);
}

// 0x60e600
void phys_string::mem_fill(void *destination, int value, int count)
{
  memset(destination, value, count);
}

int phys_string::mem_compare(const void *a, const void *b, int n)
{
  return memcmp(a, b, n);
}

// 0x60e648
int phys_string::array_index_of(char character, int start, int end) const
{
  int index;
  for (index = start; index < get_string_length() && index < end; index++) {
    if (data[index] == character) return index;
  }
  return -1;
}

// 0x60e690
int phys_string::last_index_of(char character, int start, int end) const
{
  if (end > get_string_length()) end = get_string_length();
  int index;
  for (index = end - 1; index >= start; index--) {
    if (data[index] == character) return index;
  }
  return -1;
}

phys_bool phys_string::begins_with(const char *prefix) const
{
  int i = 0;
  while (prefix[i] != 0) {
    if (i >= get_string_length() || (unsigned char)data[i] != (unsigned char)prefix[i])
      return phys_bool(false);
    i++;
  }
  return phys_bool(true);
}

phys_bool phys_string::string_ends_with(const phys_string &suffix) const
{
  int suffix_length = suffix.get_string_length();
  int length = get_string_length();
  if (length < suffix_length)
    return phys_bool(false);
  int offset = length - suffix_length;
  for (int i = 0; i < suffix_length; i++) {
    if ((unsigned char)data[offset + i] != (unsigned char)suffix.data[i])
      return phys_bool(false);
  }
  return phys_bool(true);
}

phys_bool phys_string::string_ends_with(const char *suffix) const
{
  int length = (int)strlen(suffix);
  int offset = get_string_length() - length;
  if (offset < 0)
    return phys_bool(false);
  int i = 0;
  while (suffix[i] != 0) {
    if (data[offset + i] != suffix[i])
      return phys_bool(false);
    i++;
  }
  return phys_bool(true);
}

char *phys_string::str_dup(const char *string)
{
  int length = (int)strlen(string);
  phys_thread_memory *memory = &phys_thread_memory::get_singleton_instance();
  char *copy = (char *)((phys_thread_memory *)memory)->allocate(length + 1, (phys_memory_class)20);
  copy_cstr(copy, string);
  return copy;
}

// @address 0x60e8a8 ?str_ndup@phys_string@@SAPADPBDH@Z
char *phys_string::str_ndup(const char *string, int count)
{
  int n = strlen(string);
  if (n > count) n = count;
  phys_thread_memory *memory = &phys_thread_memory::get_singleton_instance();
  char *copy = (char *)((phys_thread_memory *)memory)->allocate(n + 1, (phys_memory_class)20);
  if (n != 0) strncpy(copy, string, n);
  copy[n] = '\0';
  return copy;
}

// match: 0x60eaf8: inline expands the deallocation into printf, both operator+= and replace; out of line each calls it (28 to 84 bytes shorter)
#if defined(_XBOX) && !defined(__clang__)
inline void phys_string::string_rep::release_string_memory()
#else
void phys_string::string_rep::release_string_memory()
#endif
{
  int n = capacity + 13;             /* 12-byte header and the terminator */
  phys_thread_memory *tm = &phys_thread_memory::get_singleton_instance();
  ((phys_thread_memory *)tm)->free_chunk((void *)(this), n, (phys_memory_class)20);
}

phys_string::string_rep *phys_string::string_rep::clone() const
{
    int size = capacity + 13;          /* 12-byte header and the terminator */
    phys_thread_memory *memory = &phys_thread_memory::get_singleton_instance();
    string_rep *copy = (string_rep *)((phys_thread_memory *)memory)->allocate_chunk(size, (phys_memory_class)20);
    copy->length = length;
    copy->capacity = capacity;
    copy->reference_count = 0;
    memcpy(copy->data, data, length + 1);
    return copy;
}

phys_string::string_rep *phys_string::string_rep::create(int count)
{
    int reserved = count;
    if (count < 51) {
        reserved = 51;
    }
    phys_thread_memory *memory = &phys_thread_memory::get_singleton_instance();
    string_rep *block = (string_rep *)((phys_thread_memory *)memory)->allocate_chunk(reserved + 13,
        (phys_memory_class)20);   /* 12-byte header and the terminator */
    block->length = count;
    block->capacity = reserved;
    block->reference_count = 0;
    return block;
}

// 0x60ea78
phys_bool phys_string::begins_with(const phys_string &prefix) const
{
  return begins_with(prefix.data);
}

// @address 0x60ea80 ?set_capacity@phys_string@@QAAXH@Z
void phys_string::set_capacity(int capacity)
{
  if (get_rep()->capacity < capacity) {
    phys_string::string_rep *string_storage_rep = get_rep();
    if (--string_storage_rep->reference_count < 0) {
      int bytes = string_storage_rep->capacity + 13;
      phys_thread_memory *memory = &phys_thread_memory::get_singleton_instance();
      ((phys_thread_memory *)memory)->free_chunk((void *)(string_storage_rep), bytes, (phys_memory_class)20);
    }
    data = phys_string::string_rep::create(capacity)->data;
  }
}

// 0x60ecf8
phys_string phys_string::operator+(const phys_string &other) const
{
  int length = get_string_length();
  int other_len = other.get_string_length();
  int total = length + other_len;
  string_rep *string_storage_rep = string_rep::create(total);
  char *text = string_storage_rep->data;
  memcpy(text, data, length);
  memcpy(text + length, other.data, other_len + 1);
  return phys_string(string_storage_rep);
}

// 0x60ed90
phys_string phys_string::operator+(const char *other) const
{
  int length = get_string_length();
  int other_len = strlen(other);
  int total = length + other_len;
  int capacity = total;
  if (total < 51) capacity = 51;
  phys_thread_memory *memory = &phys_thread_memory::get_singleton_instance();
  phys_string::string_rep *string_storage_rep = (phys_string::string_rep *)((phys_thread_memory *)memory)->allocate_chunk(capacity + 13, (phys_memory_class)20);
  char *text = string_storage_rep->data;
  string_storage_rep->length = total;
  string_storage_rep->capacity = capacity;
  string_storage_rep->reference_count = 0;
  memcpy(text, data, length);
  memcpy(text + length, other, other_len + 1);
  return phys_string(string_storage_rep);
}

// 0x60ee48
phys_string &phys_string::operator+=(const phys_string &other)
{
  int length = get_string_length();
  int other_len = other.get_string_length();
  int total = length + other_len;
  if (total > get_rep()->capacity || get_rep()->reference_count > 0) {
    int count = total > length ? total : length;
    string_rep *string_storage_rep = string_rep::create(count * 2);
    char *text = string_storage_rep->data;
    memcpy(text, data, length);
    get_rep()->release_ref();
    data = string_storage_rep->data;
  }
  memcpy(data + length, other.data, other_len + 1);
  get_rep()->length = total;
  return *this;
}

// 0x60ef50
phys_string &phys_string::operator+=(const char *other)
{
  int length = get_string_length();
  int other_len = (int)strlen(other);
  int total = length + other_len;
  if (total > get_rep()->capacity || get_rep()->reference_count > 0) {
    int count = total > length ? total : length;
    string_rep *string_storage_rep = string_rep::create(count * 2);
    char *text = string_storage_rep->data;
    memcpy(text, data, length);
    get_rep()->release_ref();
    data = string_storage_rep->data;
  }
  memcpy(data + length, other, other_len + 1);
  get_rep()->length = total;
  return *this;
}

// 0x60f070
__forceinline phys_string phys_string::substr(int start, int count) const
{
  if (count > get_string_length() - start) count = get_string_length() - start;
  string_rep *string_storage_rep = string_rep::create(count);
  char *text = string_storage_rep->data;
  memcpy(text, data + start, count);
  text[count] = 0;
  return phys_string(string_storage_rep);
}

phys_string phys_string::to_lower_case() const
{
    string_rep *string_storage_rep = get_rep()->clone();
    char *text = string_storage_rep->data;
    int index = 0;
    if (get_string_length() > 0) {
        do {
            text[index] = to_lower(text[index]);
            index = index + 1;
        } while (index < get_string_length());
    }
    return phys_string(string_storage_rep);
}


// 0x60f248
phys_string phys_string::replace(char from, char to, replace_mode all) const
{
  phys_string::string_rep *string_storage_rep = get_rep()->clone();
  int n = get_string_length();
  char *text = string_storage_rep->data;
  int i = 0;
  if (0 < n) {
    do {
      if (text[i] == from && (text[i] = to, all == 0)) break;
      i++;
    } while (i < get_string_length());
  }
  return phys_string(string_storage_rep);
}
// 0x60f2d0
phys_string phys_string::replace(const phys_string &from, const phys_string &to, replace_mode mode) const
{
  phys_string result;
  int start = 0;
  const char *text = data;
  const char *pattern = from.data;
  const char *found = str_str(text, pattern);
  while (found != 0) {
    int count = found - text - start;
    result += substr(start, count);
    result += to;
    start += count + from.get_string_length();
    if (mode == STRING_REPLACE_FIRST) break;
    found = str_str(text + start, pattern);
  }
  if (start != 0) {
    result += text + start;
    return result;
  } else {
    return *this;
  }
}

phys_string phys_string::to_upper_case() const
{
    string_rep *string_storage_rep = get_rep()->clone();
    char *text = string_storage_rep->data;
    int index = 0;
    if (get_string_length() > 0) {
        do {
            text[index] = char_to_upper(text[index]);
            index = index + 1;
        } while (index < get_string_length());
    }
    return phys_string(string_storage_rep);
}

static __forceinline phys_string::string_rep *alloc_rep(int count)
{
  int reserved = count;
  if (count < 51)
    reserved = 51;
  phys_string::string_rep *block = (phys_string::string_rep *)phys_thread_memory::get_singleton_instance().allocate_chunk(reserved + 13, PHYS_MEM_STRING);
  block->length = count;
  block->capacity = reserved;
  block->reference_count = 0;
  return block;
}

__forceinline void phys_string::set_string_length(int n)
{
  get_rep()->release_ref();
  data = alloc_rep(n)->data;
}

// @address 0x60eaf8 ?printf@phys_string@@QAAXPBDZZ
void phys_string::printf(const char *format_str, ...)
{
  if (get_rep()->reference_count > 0 || get_rep()->capacity + 13 < 51) {
    set_string_length(255);
  }
  va_list args;
  __va_start(&args, &format_str);
  for (;;) {
    int size = get_rep()->capacity + 13;
    int chars_written = _vsnprintf(data, size, format_str, args);
    if (chars_written >= 0 && chars_written < size) {
      get_rep()->length = chars_written;
      break;
    } else if (chars_written < 0) {
      get_rep()->release_ref();
      data = alloc_rep(size * 2 > 255 ? size * 2 : 255)->data;
    } else {
      set_string_length(chars_written);
    }
  }
}
