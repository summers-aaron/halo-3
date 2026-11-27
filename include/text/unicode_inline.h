#ifndef TEXT_UNICODE_INLINE_H
#define TEXT_UNICODE_INLINE_H

__forceinline void wchar_string_to_ascii_string(const wchar_t *string, char *buffer, long maximum_count, long *characters_written)
{
    const wchar_t *source = string;
    char *destination = buffer;
    long remaining = maximum_count;
    long written = 0;
    while (remaining > 0)
    {
        unsigned short character = *source;
        if (remaining == 1)
            *destination = 0;
        else if (character <= 127)
            *destination = (char)character;
        else
            *destination = '?';
        source++;
        destination++;
        remaining--;
        written++;
        if (character == 0)
            break;
    }
    if (characters_written)
        *characters_written = written;
}

#endif // TEXT_UNICODE_INLINE_H
