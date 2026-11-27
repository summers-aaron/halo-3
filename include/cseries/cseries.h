#ifndef CSERIES_CSERIES_H
#define CSERIES_CSERIES_H

#include <string.h>

#include "cseries/cseries_types.h"

enum {
    _string_id_invalid = -1,
    _string_id_empty_string = 0
};

#ifndef NONE
#define NONE (-1)
#endif
#ifndef BIT
#define BIT(bit) (1UL << (bit))
#endif
#ifndef FLAG
#define FLAG(bit) (1 << (bit))
#endif
#ifndef TEST_BIT
#define TEST_BIT(flags, bit) (((flags) & (1 << (bit))) != 0)
#endif
#ifndef BIG_ENDIAN_VIEW_INDEX
// Index of the index-th most significant element of a byte or halfword view laid over a wider integer
// (console code reads flags_bytes[0] as the high byte of flags). On a little-endian host that element is
// the last one; the console build keeps the literal index.
#if !defined(__BYTE_ORDER__) || __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#define BIG_ENDIAN_VIEW_INDEX(index, count) (index)
#else
#define BIG_ENDIAN_VIEW_INDEX(index, count) ((count) - 1 - (index))
#endif
#endif
#ifndef VALID_INDEX
#define VALID_INDEX(index, count) ((index) >= 0 && (index) < (count))
#endif
#ifndef NUMBEROF
#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))
#endif
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef PIN
#define PIN(v, lo, hi) MIN(MAX((v), (lo)), (hi))
#endif
#ifndef CLAMP
#define CLAMP(v, lo, hi) (((v) < (lo)) ? (lo) : (((v) > (hi)) ? (hi) : (v)))
#endif
// Bungie's assert. Asserts compile out to nothing; keep the assert where the
// source keeps a local or a call only because an assert reads it, so the line
// says why it is there.
#ifndef ASSERT
#define ASSERT(STATEMENT) do { } while (false)
#endif
#ifndef STATIC_ASSERT
#define STATIC_ASSERT(cond, name) typedef char name[(cond) ? 1 : -1]
#endif
#ifndef STATIC_ASSERT_ABI32
#define STATIC_ASSERT_ABI32(cond, name) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]
#endif

#ifndef UNREACHABLE
#define UNREACHABLE() __assume(0)
#endif
#ifndef HALT
#define HALT() __assume(0)
#endif
#ifndef BLAM_ASSUME
#define BLAM_ASSUME(expr) __assume(expr)
#endif
#ifndef BLAM_ASSUME_VALID_REAL
#define BLAM_ASSUME_VALID_REAL(x) __assume((x) == (x))
#endif

#include "cseries/cseries_wrapped.h"
#include "memory/static_arrays.h"

#include "cseries/language_types.h"

struct csstrtok_data {
    char *next;
};

static __forceinline unsigned int csstrnlen(const char *string, unsigned int size) {
    unsigned int length = 0;
    while (length < size) {
        if (*string++ == 0)
            break;
        ++length;
    }
    return length;
}

static __forceinline void *csmemset(void *buffer, int value, unsigned int size) {
    return memset(buffer, value, size);
}

char *csstrtok(char *data, const char *delimiters, bool skip_delimiters, csstrtok_data *state);
char *csstrnlwr(char *string, unsigned int size);
char *csnzprintf(char *buffer, unsigned int size, const char *format, ...);
void cseries_dispose(void);
int ascii_stricmp(const char *first, const char *second);
int csstricmp(const char *s1, const char *s2);
int csstrnicmp(const char *s1, const char *s2, unsigned int n);
char *csstrnzcat(char *destination, const char *source, unsigned int size);

// Array overload keeps the destination capacity with its storage.
template<unsigned int size>
static __forceinline char *csstrnzcat(char (&destination)[size], const char *source)
{
    unsigned int length = csstrnlen(destination, size);
    char *result = strncpy(destination + length, source, size - length);
    destination[size - 1] = 0;
    return result;
}
char *csnzappendf(char *buffer, unsigned int size, const char *format, ...);
const char *get_language_suffix(e_language language, bool english_no_suffix);
const char *get_language_iso_639_1_name(e_language language);
e_language get_e_language_from_xbox_language(unsigned long xbox_language);

// Functions defined in tu/blamlibxbox/cseries.cpp that other units call.

extern "C" int get_dense_index_from_xbox_value(int xbox_value);
extern "C" unsigned char scenario_tags_try_and_load(long campaign_id, long map_id, char const *scenario_path);

// Data defined in tu/blamlibxbox/cseries.cpp that other units use.

union real_argb_color;

extern const real_argb_color *global_real_argb_blue;
extern const real_argb_color *global_real_argb_green;
extern const real_argb_color *global_real_argb_magenta;
extern const real_argb_color *global_real_argb_orange;
extern const real_argb_color *global_real_argb_pink;

#endif // CSERIES_CSERIES_H
