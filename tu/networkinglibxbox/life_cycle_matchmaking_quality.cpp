// networkinglibxbox/life_cycle_matchmaking_quality.obj: from the records 0x43cd30 0x43cdb8
// @flags /TP /O2
// @address 0x43cd30 ?initialize@c_matchmaking_quality@@QAAXXZ
// @address 0x43cdb8 ?submit_to_webstats@c_matchmaking_quality@@QAAXXZ
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "cseries/cseries_types.h"
#include "networking/transport/transport_security.h"

#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include "networking/logic/life_cycle_matchmaking_quality_services.h"

void c_matchmaking_quality::initialize()
{
    m_needs_submission = false;
    memset(&m_aborted, 0, k_serialized_data_size);
    s_transport_secure_address secure_address;
    if (transport_secure_address_get(&secure_address))
        set_matchmaking_quality_address_from_secure_address(&secure_address, &m_local_address);
}

void c_matchmaking_quality::submit_to_webstats()
{
    m_needs_submission = true;
    unsigned char submitted = game_lsp_submit_matchmaking_quality(&m_aborted);
    if (submitted != '\0')
        m_needs_submission = false;
}
