#ifndef TEXT_STRING_LIST_STRINGS_H
#define TEXT_STRING_LIST_STRINGS_H

template<long k_capacity> class c_static_wchar_string;
extern "C" void text_group_get_string(long tag_index, long string_id, c_static_wchar_string<1024> *string);
extern "C" bool string_list_get_normal_string(long string_list_index, long string_index, c_static_wchar_string<256> *string);

#endif // TEXT_STRING_LIST_STRINGS_H
