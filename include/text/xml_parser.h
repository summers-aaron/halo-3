#ifndef TEXT_XML_PARSER_H
#define TEXT_XML_PARSER_H

// March xml_parser.obj: the unadjusted base of c_game_tag_parser. The debug asserts
// name text/xml_parser.cpp; no header path is attested. The parser engine remains
// in its owning unit; this leaf defines the shared tag-parser interface.
template<typename t_character>
class c_xml_tag_parser
{
public:
    __forceinline c_xml_tag_parser(const t_character *tag_text) : m_tag_text(tag_text) {}
    virtual bool parse_tag(const t_character *stream, long *parsed, t_character *output, long capacity, long *written) = 0;
    virtual __forceinline ~c_xml_tag_parser() {}
    __forceinline const t_character *get_tag_text() { return m_tag_text; }

    static bool xml_get_hexidecimal_attribute(const t_character *stream, const t_character *name, unsigned long *value);
    static bool xml_get_string_attribute(const t_character *stream, const t_character *name, t_character *value, long capacity);
    // Other March-owned helpers stay declarations until their owning unit adopts this header.
    static bool xml_get_boolean_attribute(const t_character *stream, const t_character *name, bool *value);
    static bool xml_get_byte_attribute(const t_character *stream, const t_character *name, unsigned char *value);
    static bool xml_get_word_attribute(const t_character *stream, const t_character *name, unsigned short *value);
    static bool xml_get_short_attribute(const t_character *stream, const t_character *name, short *value);
    static bool xml_get_dword_attribute(const t_character *stream, const t_character *name, unsigned long *value);
    static bool xml_get_long_attribute(const t_character *stream, const t_character *name, long *value);
    static bool xml_get_qword_attribute(const t_character *stream, const t_character *name, unsigned __int64 *value);
    static bool xml_get_real_attribute(const t_character *stream, const t_character *name, float *value);
    static bool xml_get_double_attribute(const t_character *stream, const t_character *name, double *value);
    static char html_get_special_character(const t_character *stream, long *parsed);
    static void warn_missing_attribute(const t_character *name);

protected:
    const t_character *m_tag_text;                         // on Xbox 360
};

// March xml_parser.obj: the parser engine. Its methods are explicit specializations
// defined in tu/blamlibxbox/xml_parser.cpp; has_overflowed is March's inline accessor
// (emitted in user_interface_text_parser.obj).
template<typename t_character>
class c_xml_parser
{
public:
    c_xml_parser();
    virtual ~c_xml_parser();
    virtual bool parse_non_tag(const t_character *stream, long *parsed, t_character *output, long capacity, long *written);
    bool parse_text(const t_character *text, t_character *output, long capacity);
    bool parse_xml_tag(const t_character *stream, long *parsed, t_character *output, long capacity, long *written);
    void set_parsers(c_xml_tag_parser<t_character> **parsers, long count);
    __forceinline bool has_overflowed() { return (m_flags >> 1) & 1; }

private:
    long m_tag_parser_count;
    c_xml_tag_parser<t_character> **m_tag_parsers;
    unsigned long m_flags;
};

// Defined in tu/blamlibxbox/xml_parser.cpp for the wide-character parser.
template<>
bool c_xml_parser<wchar_t>::parse_non_tag(const wchar_t *text, long *consumed, wchar_t *characters, long max, long *count);
template<>
bool c_xml_parser<wchar_t>::parse_xml_tag(const wchar_t *stream, long *parsed, wchar_t *output, long capacity, long *written);

#endif // TEXT_XML_PARSER_H
