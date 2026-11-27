#ifndef CSERIES_CSERIES_WINDOWS_H
#define CSERIES_CSERIES_WINDOWS_H

// Functions defined in tu/blamlibxbox/cseries_windows.cpp that other units call.

struct s_system_memory_information;

__declspec(noreturn) void system_exit(int code);
void system_memory_information_get(s_system_memory_information *information);

#endif // CSERIES_CSERIES_WINDOWS_H
