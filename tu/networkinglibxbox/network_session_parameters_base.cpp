// networkinglibxbox/network_session_parameters_base.obj: from the records 0x3d8c30 0x3d8ca0 0x3d8d20
// @flags /TP /O2
// @address 0x3d8c30 ?set_allowed@c_network_session_parameter_base@@QBA_NXZ
// @address 0x3d8ca0 ?change_request_allowed@c_network_session_parameter_base@@QBA_NXZ
// @address 0x3d8d20 ?get_session_description@c_network_session_parameter_base@@IBAPBDXZ
#include "cseries/cseries_types.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "networking/transport/transport_security.h"
#include "networking/session/network_session_parameters_base.h"
#include "networking/session/network_session.h"
#include "networking/session/network_managed_session.h"

struct s_transport_secure_identifier;
#include "networking/session/network_session_parameter_access.h"
#include "networking/online/online_managed_session_types.h"
static __forceinline bool state_is_active(c_network_session const *session)
{
  return (unsigned int)session->current_local_state() >= 4 && (unsigned int)session->current_local_state() < 10;
}

bool c_network_session_parameter_base::set_allowed() const
{
  c_network_session const *session = m_session;
  return state_is_active(session) && session->is_host();
}

bool c_network_session_parameter_base::change_request_allowed() const
{
  bool result = false;
  c_network_session *session = m_session;
  if (state_is_active(session)) {
    if (m_change_request_access == 2)
      result = true;
    else if (m_change_request_access == 1)
      result = session->is_leader();
  }
  return result;
}

const char *c_network_session_parameter_base::get_session_description() const
{
    const char *result = "UNKNOWN";
    const c_network_session *session = m_session;
    if (session) {
        long managed_index = session->managed_session_index();
        bool has_managed_session = false;
        if (managed_index != -1) {
            if (((unsigned char)(online_session_manager_globals.sessions[managed_index].flags >> 4) & 1) != 0)
                has_managed_session = true;
        }
        if (has_managed_session) {
            if (managed_index != -1)
                return transport_secure_identifier_get_string(
                    &online_session_manager_globals.sessions[managed_index].actual.description.secure_identifier);
            result = "00:00:00:00:00:00:00:00";
        }
    }
    return result;
}
