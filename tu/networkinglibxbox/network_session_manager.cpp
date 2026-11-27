// networkinglibxbox/network_session_manager.obj: from the records 0x3fd310
// @flags /TP /O2 /Oc

#include "networking/session/network_session.h"

#include "networking/session/network_session_manager.h"
#include "networking/transport/transport_security_types.h"

c_network_session *c_network_session_manager::get_session(const s_transport_secure_identifier *session_id)
{
    c_network_session *result = 0;
    for (unsigned int index = 0; index < 3; index = index + 1)
    {
        c_network_session *session = this->m_sessions[index];
        if (session != 0 && (unsigned char)session->compare_session_id(session_id) != 0)
        {
            result = this->m_sessions[index];
            break;
        }
    }
    return result;
}
