#pragma once

struct s_runtime_state_globals
{
    bool valid;
    void *buffer;
    long size;
    unsigned long checksum;
};

extern s_runtime_state_globals g_runtime_state_globals;
