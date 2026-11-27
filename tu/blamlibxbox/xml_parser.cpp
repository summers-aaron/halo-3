// blamlibxbox/xml_parser.obj: from the records 0x57f628 0x57f7d0 0x57f8f8 0x57fbe8 0x57fe90 0x57ff78 0x57ffd0
// @flags /TP /O2 /Oc
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "text/xml_parser.h"

static const wchar_t k_unicode_byte_order_mark = 0xFEFF;
static const wchar_t k_unicode_reversed_byte_order_mark = 0xFFFE;

#define xml_is_space(c) ((c) == 160 || iswspace(c) != 0)

// @address 0x57ff78 ??_G?$c_xml_parser@_W@@UAAPAXI@Z

// @address 0x57f628 ?xml_get_string_attribute@?$c_xml_tag_parser@_W@@SA_NPB_W0PA_WJ@Z
template<>
bool c_xml_tag_parser<wchar_t>::xml_get_string_attribute(const wchar_t *stream, const wchar_t *name, wchar_t *value, long maximum_value_characters)
{
    bool result = false;
    const wchar_t *end = wcschr(stream, L'>');
    if (end) {
        const wchar_t *attribute;
        while ((attribute = wcsstr(stream, name)) != 0 && attribute < end) {
            attribute += wcslen(name);
            while (xml_is_space(*attribute))
                attribute++;
            if (*attribute == L'=') {
                long count = 0;
                attribute++;
                while (xml_is_space(*attribute))
                    attribute++;
                if (*attribute == L'"' || *attribute == L'\'') {
                    wchar_t quote = *attribute;
                    attribute++;
                    while (*attribute != quote && count < maximum_value_characters - 1) {
                        value[count] = *attribute;
                        count++;
                        attribute++;
                    }
                } else {
                    while (iswalnum(*attribute) && count < maximum_value_characters - 1) {
                        value[count] = *attribute;
                        count++;
                        attribute++;
                    }
                }
                value[count] = 0;
                result = true;
                break;
            } else {
                stream = attribute;
            }
        }
    }
    return result;
}

template<>
bool c_xml_tag_parser<wchar_t>::xml_get_hexidecimal_attribute(const wchar_t *stream, const wchar_t *name, unsigned long *value)
{
    bool result = false;
    const wchar_t *end = wcschr(stream, L'>');
    if (end) {
        const wchar_t *attribute;
        while ((attribute = wcsstr(stream, name)) != 0 && attribute < end) {
            attribute += wcslen(name);
            while (xml_is_space(*attribute))
                attribute++;
            if (*attribute == L'=') {
                attribute++;
                while (xml_is_space(*attribute))
                    attribute++;
                if (*attribute == L'"')
                    attribute++;
                if (*attribute == L'#')
                    attribute++;
                unsigned long attribute_value = wcstoul(attribute, 0, 16);
                if (attribute_value != 0 || *_errno() == 0) {
                    *value = attribute_value;
                    result = true;
                }
                break;
            } else {
                stream = attribute;
            }
        }
    }
    return result;
}

// @address 0x57f8f8 ?html_get_special_character@?$c_xml_tag_parser@_W@@SADPB_WPAJ@Z
template<>
// match: 0x57f8f8: stream cannot alias the parsed-character count; avoids a reload.
char c_xml_tag_parser<wchar_t>::html_get_special_character(const wchar_t *__restrict stream, long *out_text_stream_characters_parsed)
{
    char result = 0;
    *out_text_stream_characters_parsed = 0;
    if (*stream == L'&') {
        wchar_t less_than[] = L"&lt;";
        wchar_t greater_than[] = L"&gt;";
        wchar_t ampersand[] = L"&amp;";
        wchar_t quote[] = L"&quot;";
        wchar_t nonbreaking_space[] = L"&nbsp;";
        wchar_t copyright[] = L"&copy;";
        if (_wcsnicmp(stream, less_than, 4) == 0) {
            *out_text_stream_characters_parsed = 4;
            result = '<';
        } else if (_wcsnicmp(stream, greater_than, 4) == 0) {
            *out_text_stream_characters_parsed = 4;
            result = '>';
        } else if (_wcsnicmp(stream, ampersand, 5) == 0) {
            *out_text_stream_characters_parsed = 5;
            result = '&';
        } else if (_wcsnicmp(stream, quote, 6) == 0) {
            *out_text_stream_characters_parsed = 6;
            result = '"';
        } else if (_wcsnicmp(stream, nonbreaking_space, 6) == 0) {
            *out_text_stream_characters_parsed = 6;
            result = ' ';
        } else if (_wcsnicmp(stream, copyright, 6) == 0) {
            *out_text_stream_characters_parsed = 6;
            result = (char)169;
        } else {
            wchar_t digits[4] = {0};
            if (stream[1] == L'#') {
                digits[0] = stream[2];
                if (iswdigit(digits[0])) {
                    digits[1] = stream[3];
                    if (iswdigit(digits[1])) {
                        if (stream[4] == L';' ||
                            ((digits[2] = stream[4]), iswdigit(digits[2])) && stream[5] == L';') {
                            const wchar_t *end = wcschr(stream, L';');
                            long value = wcstol(digits, 0, 10);
                            if (value >= 0 && value <= 255) {
                                *out_text_stream_characters_parsed = end - stream + 1;
                                result = (char)value;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

namespace {
static __forceinline bool xml_is_nonbreaking_space(wchar_t character)
{
    return character == L' ' || character == L'\t';
}
static __forceinline bool xml_is_breaking_space(wchar_t character)
{
    return xml_is_space(character) && !xml_is_nonbreaking_space(character);
}
}


template<>
bool c_xml_parser<wchar_t>::parse_text(const wchar_t *text, wchar_t *output, long capacity)
{
    const wchar_t *start = text;
    long written = 0;
    bool result = true;
    m_flags &= ~2;
    switch (*start) {
    case k_unicode_byte_order_mark: text = start + 1; break;
    case k_unicode_reversed_byte_order_mark: result = false; break;
    }
    long consumed;
    long characters_written;
    while (result && *text) {
        result = false;
        if (*text == L'<') {
            consumed = 0;
            result = parse_xml_tag(text, &consumed, output + written, capacity - written, &characters_written);
            if (result) {
                written += characters_written;
            } else {
                char tag[32];
                unsigned long index;
                for (index = 0; index < 31; ++index) {
                    tag[index] = (char)text[index];
                    if (tag[index] == 0 || tag[index] == '>')
                        break;
                }
                tag[index] = 0;
            }
        }
        if (!result && !((long)m_flags & 1)) {
            consumed = 0;
            while (xml_is_nonbreaking_space(text[consumed])) {
                if (written < capacity) {
                    output[written] = text[consumed];
                    ++written;
                    ++consumed;
                } else {
                    break;
                }
            }
            while (xml_is_breaking_space(text[consumed]))
                ++consumed;
            if (consumed > 0) {
                result = true;
            } else {
                consumed = 0;
                result = parse_non_tag(text, &consumed, output + written, capacity - written, &characters_written);
                if (result)
                    written += characters_written;
                else
                    m_flags |= 1;
            }
        }
        if (written == capacity) {
            --written;
            if (*text)
                m_flags |= 2;
            break;
        } else if (result) {
            text += consumed;
        }
    }
    if (result)
        output[written] = 0;
    return result;
}

template<>
bool c_xml_parser<wchar_t>::parse_non_tag(const wchar_t *text, long *consumed, wchar_t *characters, long max, long *count)
{
    long i = 0;
    long n = 0;
    while (text[i] != 0) {
        if (text[i] == L'<' && i != 0)
            break;
        long skip = 0;
        wchar_t special = (wchar_t)c_xml_tag_parser<wchar_t>::html_get_special_character(&text[i], &skip);
        if (special != 0) {
            if (n >= max)
                break;
            characters[n] = special;
            n++;
            i += skip;
        } else {
            if (n >= max)
                break;
            characters[n] = text[i];
            n++;
            i++;
        }
    }
    *consumed = i;
    *count = n;
    return i > 0;
}

template<>
c_xml_parser<wchar_t>::c_xml_parser() :
    m_tag_parser_count(0),
    m_tag_parsers(0),
    m_flags(0)
{
}

template<>
c_xml_parser<wchar_t>::~c_xml_parser()
{
}

template<>
void c_xml_parser<wchar_t>::set_parsers(c_xml_tag_parser<wchar_t> **parsers, long count)
{
    m_tag_parser_count = count;
    m_tag_parsers = parsers;
}

template<>
bool c_xml_parser<wchar_t>::parse_xml_tag(const wchar_t *stream, long *parsed, wchar_t *output, long capacity, long *written)
{
    bool result = false;
    *parsed = 0;
    *written = 0;
    if (!((long)m_flags & 1) &&
        (iswalpha(stream[1]) || stream[1] == L'/' || stream[1] == L'?' || stream[1] == L'!')) {
        for (long parser_index = 0; parser_index < m_tag_parser_count; parser_index++) {
            long length = wcslen(m_tag_parsers[parser_index]->get_tag_text());
            if (_wcsnicmp(stream, m_tag_parsers[parser_index]->get_tag_text(), length) == 0 &&
                (xml_is_space(stream[length]) || stream[length] == L'/' || stream[length] == L'>')) {
                result = m_tag_parsers[parser_index]->parse_tag(stream, parsed, output, capacity, written);
                if (!result)
                    m_flags |= 1;
                break;
            }
        }
    }
    return result;
}
