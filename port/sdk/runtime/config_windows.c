// port/sdk/runtime (Windows host): the environment, folders and files port_config.c asks about (config_host.h).
#include "config_host.h"
#include "../kernel/win32_host.h"

__declspec(dllimport) char *__cdecl getenv(const char *name);
__declspec(dllimport) int __cdecl _putenv(const char *assignment);

PORT_HOST_IMPORT(unsigned long, GetCurrentDirectoryA, 8, (unsigned long size, char *buffer));
PORT_HOST_IMPORT(unsigned long, GetFileAttributesA, 4, (const char *name));

#define CONFIG_FILE_ATTRIBUTE_DIRECTORY 0x10ul
#define CONFIG_INVALID_FILE_ATTRIBUTES 0xfffffffful

static unsigned long length_of(const char *text)
{
    unsigned long n = 0;
    while (text[n] != 0)
        n++;
    return n;
}

const char *port_config_host_getenv(const char *name)
{
    return getenv(name);
}

// msvcrt's _putenv keeps the CRT's copy of the environment (getenv) and the process's (SetEnvironmentVariable) in
// step. The "NAME=value" strings live in a pool that is never freed: whether the CRT copies the string has differed
// between its versions, and the variables are set once, at start-up.
int port_config_host_setenv(const char *name, const char *value)
{
    static char pool[16384];
    static unsigned long used;
    const unsigned long name_length = length_of(name), value_length = length_of(value);
    const unsigned long need = name_length + 1 + value_length + 1;
    if (need > sizeof(pool) - used)
        return 0;
    char *assignment = pool + used;
    __builtin_memcpy(assignment, name, name_length);
    assignment[name_length] = '=';
    __builtin_memcpy(assignment + name_length + 1, value, value_length + 1);
    used += need;
    // "NAME=" alone would remove the variable: an empty value is left unset instead (every reader takes "" as unset)
    return value_length == 0 || _putenv(assignment) == 0;
}

int port_config_host_config_directory(char *path, unsigned long size)
{
    const char *base = getenv("APPDATA");
    if (base == 0 || base[0] == 0)
        return 0;
    static const char suffix[] = "\\Halo3";
    const unsigned long length = length_of(base);
    if (length + sizeof(suffix) > size)
        return 0;
    __builtin_memcpy(path, base, length);
    __builtin_memcpy(path + length, suffix, sizeof(suffix));
    return 1;
}

int port_config_host_current_directory(char *path, unsigned long size)
{
    const unsigned long n = host_GetCurrentDirectoryA(size, path);
    return n != 0 && n < size;
}

int port_config_host_display(void)
{
    return 1;
}

int port_config_host_file_exists(const char *path)
{
    const unsigned long attributes = host_GetFileAttributesA(path);
    return attributes != CONFIG_INVALID_FILE_ATTRIBUTES && (attributes & CONFIG_FILE_ATTRIBUTE_DIRECTORY) == 0;
}

int port_config_host_absolute(const char *path)
{
    const int letter = (path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z');
    return path[0] == '\\' || path[0] == '/' || (letter && path[1] == ':');
}
