#ifndef TEXT_ASCII_WCHAR_CONVERSION_H
#define TEXT_ASCII_WCHAR_CONVERSION_H

static __forceinline void ascii_string_to_wchar_string(const char *source, wchar_t *destination, long maximum_count, long *length)
{
    const char *read = source;
    wchar_t *write = destination;
    long remaining = maximum_count;
    long converted_count = 0;
    while (remaining > 0) {
        unsigned char character = (unsigned char)*read;
        if (remaining == 1)
            *write = 0;
        else if (character <= 127)
            *write = (wchar_t)(signed char)*read;
        else
            *write = 0x25a1;
        if (*read == 0)
            break;
        else {
            ++write;
            ++read;
            ++converted_count;
            --remaining;
        }
    }
    if (length)
        *length = converted_count;
}

#endif
