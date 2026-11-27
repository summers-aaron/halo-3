#ifndef NETWORKING_ONLINE_ONLINE_URL_NAME_GLOBALS_H
#define NETWORKING_ONLINE_ONLINE_URL_NAME_GLOBALS_H

struct s_online_url_name_globals
{
    char base_path[0x40];
    char name[64];
};

extern "C" {
extern s_online_url_name_globals g_online_url_name_globals;
}

#endif // NETWORKING_ONLINE_ONLINE_URL_NAME_GLOBALS_H
