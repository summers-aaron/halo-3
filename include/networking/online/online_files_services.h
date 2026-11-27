#ifndef NETWORKING_ONLINE_ONLINE_FILES_SERVICES_H
#define NETWORKING_ONLINE_ONLINE_FILES_SERVICES_H

extern "C++" {
class c_content_catalogue;
struct s_online_files_encrypted_writer;
struct s_file_reference;
enum e_game_content_type;

extern "C" void XeCryptBnQw_SwapDwQwLeBe(void *output, const void *input, long count);
extern "C" int XeCryptBnQwNeRsaPubCrypt(void *output, const void *input, const void *key);
extern "C" const char *get_current_language_suffix(bool);
template<long k_capacity> class c_static_string;
extern "C" void transport_security_get_address_strings(
    c_static_string<16> *insecure_address, c_static_string<128> *secure_address);
extern "C" long content_catalogue_find_item_by_file_reference(const c_content_catalogue *catalogue, const s_file_reference *file);
extern "C" void user_interface_networking_post_error_message(void *error, long string_id);
extern "C" void wchar_string_to_utf8_string(const unsigned short *, unsigned char *, int, int);
extern "C" void content_catalogue_trim_whitespace(unsigned short *string);
extern "C" bool saved_game_files_create_content_item_file(long, e_game_content_type, const wchar_t *, const char *, s_file_reference *, long *);
extern "C" void content_catalogue_delete_item(c_content_catalogue *, long, bool);
extern "C" int XNetRandom(unsigned char *, unsigned int);
}

#endif // NETWORKING_ONLINE_ONLINE_FILES_SERVICES_H
