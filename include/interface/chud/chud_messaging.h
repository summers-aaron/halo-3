#ifndef INTERFACE_CHUD_CHUD_MESSAGING_H
#define INTERFACE_CHUD_CHUD_MESSAGING_H

#include "interface/chud/chud_messaging_handle_types.h"

// Functions defined in tu/interfacelibxbox/chud_messaging.cpp.

bool network_get_unique_player_name(wchar_t *name, long size, long name_count, const wchar_t **names);
void chud_messaging_special_load(bool p0);
void chud_post_action_response(long user_index);

class c_chud_messaging_manager;
typedef c_chud_messaging_manager s_chud_text_block;
template<long k_maximum_count> class c_static_wchar_string;

void chud_get_string(long user_index, long string_index, c_static_wchar_string<256> *string);

extern "C" void chud_messaging_set_text(c_chud_messaging_manager *block, const wchar_t *text, long value, long unused, long index);
extern "C" void chud_messaging_special_save(bool p0);
extern "C" void chud_messaging_special_progress(bool p0);
extern "C" void chud_messaging_post_medal_for_user(long user_index, long medal_value);
extern "C" bool chud_messaging_is_text_equal(const wchar_t *text, const wchar_t *other_text);
extern "C" bool chud_messaging_clamp_index(s_chud_messaging_handle handle, long index, long *result);

enum e_chud_message_context;

void chud_messaging_post(long user_index, long string_index, e_chud_message_context context);

class c_chud_messaging_manager;
extern "C" void chud_messaging_manager_update_messages(c_chud_messaging_manager *messaging_manager, long user_index);
extern "C" long chud_messaging_manager_get_new_medal_index(c_chud_messaging_manager *manager);

extern "C" unsigned char g_chud_preserve_state_messages;

#endif // INTERFACE_CHUD_CHUD_MESSAGING_H
