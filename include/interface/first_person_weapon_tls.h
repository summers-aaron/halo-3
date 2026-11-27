#ifndef INTERFACE_FIRST_PERSON_WEAPON_TLS_H
#define INTERFACE_FIRST_PERSON_WEAPON_TLS_H

#include "interface/first_person_weapon_runtime_types.h"

extern "C" { extern __declspec(thread) fpw_first_person *g_first_person_weapon_data; }

static __forceinline fpw_first_person *tls_first_person_weapon_data()
{
    return g_first_person_weapon_data;
}

#endif
