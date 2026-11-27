// networkinglibxbox/online_arbitration_xenon.obj: from the records 0x407e60 0x407e70 0x407ef0
// @flags /TP /O2 /Oc
#define NOD3D
#include <xtl.h>
#include <string.h>
#include "networking/online/online_xenon.h"
#include "networking/session/network_managed_session.h"
#include "networking/online/online_arbitration_xenon.h"
#include "networking/online/online_arbitration_types.h"

s_online_arbitration_globals g_online_arbitration_globals;

// @address 0x407e60 ?get_context_string@c_arbitration_overlapped_task@@UBAPBDXZ
const char *c_arbitration_overlapped_task::get_context_string() const
{
    return "arbitration";
}

// @address 0x407e70 ?start@c_arbitration_overlapped_task@@UAAKPAX@Z
unsigned long c_arbitration_overlapped_task::start(void *overlapped)
{
    unsigned long results_buffer_size = sizeof(m_results_buffer);
    unsigned long result = E_FAIL;
    if (online_local_xuid_is_online_enabled())
    {
        void *session = m_session;
        if (managed_session_is_valid_handle(session))
        {
            result = XSessionArbitrationRegister(session, 0, m_session_nonce,
                &results_buffer_size, (XSESSION_REGISTRATION_RESULTS *)m_results_buffer, (XOVERLAPPED *)overlapped);
        }
    }
    return result;
}

// @address 0x407ef0 online_arbitration_registration_get_result
unsigned char online_arbitration_registration_get_result(s_online_arbitration_result *result)
{
    memset(result, 0, sizeof(*result));
    c_arbitration_overlapped_task *registration_task = g_online_arbitration_globals.registration;
    XSESSION_REGISTRATION_RESULTS *registration_results = (XSESSION_REGISTRATION_RESULTS *)registration_task->m_results_buffer;
    result->registrant_count = 0;
    for (unsigned long machine_index = 0; machine_index < registration_results->wNumRegistrants; machine_index++)
    {
        const XSESSION_REGISTRANT *machine = &registration_results->rgRegistrants[machine_index];
        for (unsigned long user_index = 0; user_index < machine->bNumUsers; user_index++)
        {
            s_online_arbitration_registrant *registrant = &result->registrants[result->registrant_count];
            registrant->machine_id = machine->qwMachineID;
            registrant->trustworthiness = machine->bTrustworthiness;
            registrant->xuid = machine->rgUsers[user_index];
            result->registrant_count++;
        }
    }
    return 1;
}
