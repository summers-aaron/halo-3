#ifndef NETWORKING_ONLINE_ONLINE_GUIDE_XENON_SERVICES_H
#define NETWORKING_ONLINE_ONLINE_GUIDE_XENON_SERVICES_H

extern "C++" {
struct s_online_guide_state;
struct s_compose_mapping
{
    long string_index;
    unsigned short option_type;
};
struct c_static_wchar_string_1024;

extern s_online_guide_state g_online_guide_globals;
extern "C" void text_group_get_string(long tag_index, long string_id, c_static_wchar_string_1024 *text);
}

#endif // NETWORKING_ONLINE_ONLINE_GUIDE_XENON_SERVICES_H
