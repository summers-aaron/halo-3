#ifndef NETWORKING_ONLINE_ONLINE_FILE_GAME_API_PARSER_H
#define NETWORKING_ONLINE_ONLINE_FILE_GAME_API_PARSER_H

#include "saved_games/text_stream_types.h"

extern "C" bool online_file_game_api_parser_skip_to_next_section(s_text_stream *stream, const char *prefix);

class c_online_file_game_api_parser : public s_text_stream
{
public:
    c_online_file_game_api_parser() {}
    c_online_file_game_api_parser(char *buffer, long buffer_size) { begin = buffer; next = buffer; size = buffer_size; }
    bool read_long(const char *prefix, long *value);
    bool read_qword(const char *prefix, unsigned __int64 *value);
    bool read_string(const char *prefix, char *value, long count);
    bool read_dword(const char *prefix, unsigned long *value);
    bool read_string(const char *prefix, wchar_t *value, long count);
};
template<long> class c_static_wchar_string;

extern "C" bool online_file_game_api_parser_read_wstring(c_online_file_game_api_parser *parser, const char *key, c_static_wchar_string<256> *message);

#endif // NETWORKING_ONLINE_ONLINE_FILE_GAME_API_PARSER_H
