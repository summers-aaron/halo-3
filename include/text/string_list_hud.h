#ifndef TEXT_STRING_LIST_HUD_H
#define TEXT_STRING_LIST_HUD_H

template<long size> class c_static_wchar_string;

extern "C" bool string_list_get_normal_string(long string_list_index, long string_id, c_static_wchar_string<256> *string);
extern "C" bool string_list_get_normal_string_by_language(long string_list_index, long string_id, long language, c_static_wchar_string<256> *string);

#endif // TEXT_STRING_LIST_HUD_H
