// port/sdk/runtime (Linux host): printf-style formatting with the Xbox C runtime's meaning, for narrow and wide
// (16-bit wchar_t) output. glibc's formatter cannot stand in for it: wchar_t is 32 bits there, and the MS format
// language differs (%s and %S swap meaning between narrow and wide formats; I64 / I32 / I / w size prefixes; %p is 8
// upper-case hex digits; floating-point exponents have at least three digits, "1.000000e+000"; long double is
// double). One engine serves every formatting entry point the game calls on this host: vsprintf, _vsnprintf,
// vsprintf_s, _vsprintf_s_l (crt_linux.c), RtlVsnprintf (the kernel's) and _vsnwprintf_s.
//
// Floating-point digits come from glibc's snprintf for one conversion at a time (correctly rounded, as the MS CRT
// is), with the exponent widened and the field padded here. Infinities and NaNs print as glibc names them ("inf",
// "nan"), not as the 2007 CRT did ("1.#INF00"); no game format string prints one on purpose.

#include <stdarg.h>
#include <string.h>

extern int snprintf(char *, size_t, const char *, ...);

#ifdef __x86_64__
typedef __builtin_ms_va_list port_ms_va_list;
#else
typedef va_list port_ms_va_list;
#endif

typedef unsigned short port_wchar; // the game's wchar_t

typedef struct port_sink {
    void *buffer;
    size_t capacity; // characters that fit; more are counted, not stored
    size_t length;   // characters produced
    int wide;
} port_sink;

static void port_put(port_sink *sink, unsigned int c)
{
    if (sink->length < sink->capacity) {
        if (sink->wide)
            ((port_wchar *)sink->buffer)[sink->length] = (port_wchar)c;
        else
            ((char *)sink->buffer)[sink->length] = (char)c;
    }
    sink->length++;
}

static void port_put_repeat(port_sink *sink, unsigned int c, int count)
{
    for (int i = 0; i < count; i++)
        port_put(sink, c);
}

// The format string, narrow or wide.
static unsigned int port_format_at(const void *format, size_t i, int wide)
{
    return wide ? ((const port_wchar *)format)[i] : (unsigned char)((const char *)format)[i];
}

typedef struct port_spec {
    int left, plus, space, alternate, zero;
    int width, precision; // precision -1: none
    int size;             // 'H' hh, 'h', 'l', 'L' (ll / I64 / j), 'w' (wide), 'n' (none: int, or pointer-size I)
    unsigned int type;
} port_spec;

// Pads a field of `length` characters (with `prefix_length` of sign / 0x before the digits) to the width.
static void port_pad_before(port_sink *sink, const port_spec *spec, int length)
{
    if (!spec->left && spec->width > length)
        port_put_repeat(sink, ' ', spec->width - length);
}
static void port_pad_after(port_sink *sink, const port_spec *spec, int length)
{
    if (spec->left && spec->width > length)
        port_put_repeat(sink, ' ', spec->width - length);
}

static void port_format_integer(port_sink *sink, const port_spec *spec, unsigned long long magnitude, int negative)
{
    char digits[32];
    int count = 0;
    const unsigned int type = spec->type;
    const unsigned int base = type == 'o' ? 8u : (type == 'x' || type == 'X' || type == 'p') ? 16u : 10u;
    const char *alphabet = (type == 'X' || type == 'p') ? "0123456789ABCDEF" : "0123456789abcdef";
    while (magnitude != 0) {
        digits[count++] = alphabet[magnitude % base];
        magnitude /= base;
    }
    int precision = spec->precision;
    if (type == 'p')
        precision = 8;
    // precision: the minimum number of digits ("%.0d" of 0 prints nothing); none: at least one
    int zeros = precision >= 0 ? (precision > count ? precision - count : 0) : (count == 0 ? 1 : 0);
    char prefix[3] = {0};
    int prefix_length = 0;
    if (type == 'd' || type == 'i') {
        if (negative)
            prefix[prefix_length++] = '-';
        else if (spec->plus)
            prefix[prefix_length++] = '+';
        else if (spec->space)
            prefix[prefix_length++] = ' ';
    } else if (spec->alternate && count != 0 && (type == 'x' || type == 'X')) {
        prefix[prefix_length++] = '0';
        prefix[prefix_length++] = (char)type;
    } else if (spec->alternate && type == 'o' && zeros == 0) {
        zeros = 1; // "%#o" begins with a 0
    }
    int length = prefix_length + zeros + count;
    if (spec->zero && !spec->left && precision < 0 && spec->width > length) {
        zeros += spec->width - length;
        length = spec->width;
    }
    port_pad_before(sink, spec, length);
    for (int i = 0; i < prefix_length; i++)
        port_put(sink, (unsigned char)prefix[i]);
    port_put_repeat(sink, '0', zeros);
    while (count > 0)
        port_put(sink, (unsigned char)digits[--count]);
    port_pad_after(sink, spec, length);
}

static void port_format_float(port_sink *sink, const port_spec *spec, double value)
{
    // the digits, sign and point from glibc, without the field width
    char format[16], text[512];
    int f = 0;
    format[f++] = '%';
    if (spec->plus)
        format[f++] = '+';
    if (spec->space)
        format[f++] = ' ';
    if (spec->alternate)
        format[f++] = '#';
    format[f++] = '.';
    format[f++] = '*';
    format[f++] = (char)spec->type;
    format[f] = 0;
    int length = snprintf(text, sizeof(text) - 2, format, spec->precision >= 0 ? spec->precision : 6, value);
    if (length < 0 || length >= (int)sizeof(text) - 2)
        length = 0;
    // MS exponents have at least three digits: e+05 -> e+005
    if (spec->type != 'f' && spec->type != 'F' && spec->type != 'a' && spec->type != 'A') {
        char *exponent = 0;
        for (char *p = text; *p != 0; p++)
            if ((*p == 'e' || *p == 'E') && (p[1] == '+' || p[1] == '-'))
                exponent = p + 2;
        if (exponent != 0 && strlen(exponent) == 2) {
            memmove(exponent + 1, exponent, 3);
            exponent[0] = '0';
            length++;
        }
    }
    const int finite = value - value == 0;
    int sign_length = (text[0] == '-' || text[0] == '+' || text[0] == ' ') ? 1 : 0;
    int zeros = 0, total = length;
    if (spec->zero && !spec->left && finite && spec->width > length) {
        zeros = spec->width - length;
        total = spec->width;
    }
    port_pad_before(sink, spec, total);
    for (int i = 0; i < sign_length; i++)
        port_put(sink, (unsigned char)text[i]);
    port_put_repeat(sink, '0', zeros);
    for (int i = sign_length; i < length; i++)
        port_put(sink, (unsigned char)text[i]);
    port_pad_after(sink, spec, total);
}

// A string argument: narrow (bytes, each its own character) or wide (16-bit units, narrowed to '?' above 0x7f in a
// narrow output, as the "C" locale's wctomb cannot convert them).
static void port_format_string(port_sink *sink, const port_spec *spec, const void *text, int wide_argument)
{
    static const char k_null[] = "(null)";
    if (text == 0) {
        text = k_null;
        wide_argument = 0;
    }
    int length = 0;
    while ((spec->precision < 0 || length < spec->precision)
           && (wide_argument ? ((const port_wchar *)text)[length] : ((const char *)text)[length]) != 0)
        length++;
    port_pad_before(sink, spec, length);
    for (int i = 0; i < length; i++) {
        unsigned int c = wide_argument ? ((const port_wchar *)text)[i] : (unsigned char)((const char *)text)[i];
        if (!sink->wide && wide_argument && c > 0x7f)
            c = '?';
        port_put(sink, c);
    }
    port_pad_after(sink, spec, length);
}

static void port_format_character(port_sink *sink, const port_spec *spec, unsigned int c, int wide_argument)
{
    if (!sink->wide && wide_argument && c > 0x7f)
        c = '?';
    if (!wide_argument)
        c &= 0xff;
    port_pad_before(sink, spec, 1);
    port_put(sink, c);
    port_pad_after(sink, spec, 1);
}

// Formats into `buffer` (capacity characters; nothing is terminated) and returns the length of the whole result.
static size_t port_ms_vformat(void *buffer, size_t capacity, const void *format, int wide, port_ms_va_list arguments)
{
    port_sink sink = {buffer, capacity, 0, wide};
    for (size_t i = 0;;) {
        unsigned int c = port_format_at(format, i++, wide);
        if (c == 0)
            break;
        if (c != '%') {
            port_put(&sink, c);
            continue;
        }
        port_spec spec = {0, 0, 0, 0, 0, 0, -1, 'n', 0};
        for (;; i++) {
            c = port_format_at(format, i, wide);
            if (c == '-') spec.left = 1;
            else if (c == '+') spec.plus = 1;
            else if (c == ' ') spec.space = 1;
            else if (c == '#') spec.alternate = 1;
            else if (c == '0') spec.zero = 1;
            else break;
        }
        if (c == '*') {
            spec.width = va_arg(arguments, int);
            if (spec.width < 0) {
                spec.left = 1;
                spec.width = -spec.width;
            }
            c = port_format_at(format, ++i, wide);
        } else {
            while (c >= '0' && c <= '9') {
                spec.width = spec.width * 10 + (int)(c - '0');
                c = port_format_at(format, ++i, wide);
            }
        }
        if (c == '.') {
            spec.precision = 0;
            c = port_format_at(format, ++i, wide);
            if (c == '*') {
                spec.precision = va_arg(arguments, int);
                if (spec.precision < 0)
                    spec.precision = -1;
                c = port_format_at(format, ++i, wide);
            } else {
                while (c >= '0' && c <= '9') {
                    spec.precision = spec.precision * 10 + (int)(c - '0');
                    c = port_format_at(format, ++i, wide);
                }
            }
        }
        if (c == 'h') {
            spec.size = 'h';
            c = port_format_at(format, ++i, wide);
            if (c == 'h') {
                spec.size = 'H';
                c = port_format_at(format, ++i, wide);
            }
        } else if (c == 'l') {
            spec.size = 'l';
            c = port_format_at(format, ++i, wide);
            if (c == 'l') {
                spec.size = 'L';
                c = port_format_at(format, ++i, wide);
            }
        } else if (c == 'I' && port_format_at(format, i + 1, wide) == '6' && port_format_at(format, i + 2, wide) == '4') {
            spec.size = 'L';
            i += 3;
            c = port_format_at(format, i, wide);
        } else if (c == 'I' && port_format_at(format, i + 1, wide) == '3' && port_format_at(format, i + 2, wide) == '2') {
            spec.size = 'n';
            i += 3;
            c = port_format_at(format, i, wide);
        } else if (c == 'I' || c == 'z' || c == 't') {
            spec.size = 'n'; // pointer-sized: 32 bits
            c = port_format_at(format, ++i, wide);
        } else if (c == 'j' || c == 'q') {
            spec.size = 'L';
            c = port_format_at(format, ++i, wide);
        } else if (c == 'L') {
            spec.size = 'D'; // long double, which is double in the MS ABI
            c = port_format_at(format, ++i, wide);
        } else if (c == 'w') {
            spec.size = 'w';
            c = port_format_at(format, ++i, wide);
        }
        if (c == 0)
            break;
        i++;
        spec.type = c;
        switch (c) {
        case 'd': case 'i': {
            long long value = spec.size == 'L' ? va_arg(arguments, long long) : (long long)va_arg(arguments, int);
            if (spec.size == 'h')
                value = (short)value;
            else if (spec.size == 'H')
                value = (signed char)value;
            unsigned long long magnitude = value < 0 ? 0 - (unsigned long long)value : (unsigned long long)value;
            port_format_integer(&sink, &spec, magnitude, value < 0);
            break;
        }
        case 'u': case 'o': case 'x': case 'X': {
            unsigned long long value = spec.size == 'L' ? va_arg(arguments, unsigned long long)
                                                        : (unsigned long long)va_arg(arguments, unsigned int);
            if (spec.size == 'h')
                value = (unsigned short)value;
            else if (spec.size == 'H')
                value = (unsigned char)value;
            port_format_integer(&sink, &spec, value, 0);
            break;
        }
        case 'p':
            port_format_integer(&sink, &spec, (unsigned long long)(size_t)va_arg(arguments, void *), 0);
            break;
        case 'e': case 'E': case 'f': case 'F': case 'g': case 'G': case 'a': case 'A':
            port_format_float(&sink, &spec, va_arg(arguments, double));
            break;
        case 'c': case 'C': {
            // %c is the output's own width, %C the other one; h and l/w name narrow and wide explicitly
            int wide_argument = spec.size == 'h' ? 0 : (spec.size == 'l' || spec.size == 'w') ? 1 : (c == 'c') == wide;
            port_format_character(&sink, &spec, (unsigned int)va_arg(arguments, int), wide_argument);
            break;
        }
        case 's': case 'S': {
            int wide_argument = spec.size == 'h' ? 0 : (spec.size == 'l' || spec.size == 'w') ? 1 : (c == 's') == wide;
            port_format_string(&sink, &spec, va_arg(arguments, const void *), wide_argument);
            break;
        }
        case 'n':
            // the 2005 CRT refuses %n by default (_set_printf_count_output); it writes nothing here
            (void)va_arg(arguments, void *);
            break;
        default:
            port_put(&sink, c); // %% and unknown conversions print the character
            break;
        }
    }
    return sink.length;
}

// ---- entry points ----

int PORT_MS_ABI port_ms_vsprintf(char *destination, const char *format, port_ms_va_list arguments) __asm__("vsprintf");
int PORT_MS_ABI port_ms_vsprintf(char *destination, const char *format, port_ms_va_list arguments)
{
    size_t length = port_ms_vformat(destination, 0x7fffffff, format, 0, arguments);
    destination[length] = 0;
    return (int)length;
}

// _vsnprintf (and the kernel's RtlVsnprintf): at most count characters. A result that fits with room for the
// terminator is terminated; one of exactly count characters is not; a longer one is cut at count, unterminated, and
// -1 returned.
int PORT_MS_ABI _vsnprintf(char *destination, size_t count, const char *format, port_ms_va_list arguments)
{
    size_t length = port_ms_vformat(destination, count, format, 0, arguments);
    if (length < count)
        destination[length] = 0;
    return length <= count ? (int)length : -1;
}

int PORT_MS_ABI RtlVsnprintf(char *destination, size_t count, const char *format, port_ms_va_list arguments)
{
    return _vsnprintf(destination, count, format, arguments);
}

// vsprintf_s: the whole result and its terminator must fit in size; otherwise the buffer is emptied and -1 returned
// (the release CRT's invalid parameter handler, which the game does not install, would end the process).
int PORT_MS_ABI vsprintf_s(char *destination, size_t size, const char *format, port_ms_va_list arguments)
{
    if (destination == 0 || size == 0)
        return -1;
    size_t length = port_ms_vformat(destination, size, format, 0, arguments);
    if (length >= size) {
        destination[0] = 0;
        return -1;
    }
    destination[length] = 0;
    return (int)length;
}

int PORT_MS_ABI _vsprintf_s_l(char *destination, size_t size, const char *format, void *locale, port_ms_va_list arguments)
{
    (void)locale;
    return vsprintf_s(destination, size, format, arguments);
}

// _vsnwprintf_s: size is the buffer's capacity in characters, count the most to write (_TRUNCATE, (size_t)-1: as
// many as fit). The result is always terminated when the buffer has room for anything:
//   - it fits (within count and size - 1): written, its length returned;
//   - count == _TRUNCATE or count < size: cut at min(count, size - 1), terminated, -1 returned;
//   - otherwise (count >= size and it does not fit): the buffer is emptied and -1 returned.
#define PORT_TRUNCATE ((size_t)-1)
int PORT_MS_ABI _vsnwprintf_s(port_wchar *destination, size_t size, size_t count, const port_wchar *format, port_ms_va_list arguments)
{
    if (destination == 0 || size == 0 || format == 0)
        return -1;
    size_t limit = size - 1;
    if (count != PORT_TRUNCATE && count < limit)
        limit = count;
    size_t length = port_ms_vformat(destination, limit, format, 1, arguments);
    if (length <= limit) {
        destination[length] = 0;
        return (int)length;
    }
    if (count == PORT_TRUNCATE || count < size) {
        destination[limit] = 0;
        return -1;
    }
    destination[0] = 0;
    return -1;
}
