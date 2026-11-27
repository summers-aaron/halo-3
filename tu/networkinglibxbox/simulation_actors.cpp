// networkinglibxbox/simulation_actors.obj: from the records 0x41c810
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/game_time_types.h"
#include "game/game_time_access.h"
#include "memory/tls_globals.h"
#include "units/unit_control.h"
// @address 0x41c810 ?generate_control@c_simulation_actor@@QAA_NPAJPAUunit_control_data@@@Z
#include <string.h>
#include "simulation/game_interface/simulation_game_interface.h"
#include "networking/replication/replication_motion.h"
#include "simulation/simulation_world_types.h"
#include "networking/replication/replication_control_view.h"
#include "simulation/simulation_view.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "simulation/simulation_actors.h"

#include "simulation/simulation_distributed_view.h"

__forceinline c_replication_control_view *c_simulation_view::get_control_view()
{
    return &m_distributed_view->control;
}

bool c_simulation_actor::generate_control(long *unit_index, unit_control_data *control)
{
    bool result = false;
    c_simulation_view *authority_view = m_world->get_authority_view();
    if (authority_view != 0)
    {
        c_replication_control_view *control_view = authority_view->get_control_view();
        if (control_view != 0)
        {
            long control_index = get_control_index();
            s_player_motion motion;
            if (control_view->retrieve_motion(control_index, &motion))
            {
                unit_control_data simulated_control;
                simulation_game_reconstruct_simulated_control(&motion, &simulated_control);
                memcpy(&m_control, &simulated_control, sizeof(unit_control_data));
                m_control_time = game_time_get();
            }
        }
    }
    if (m_control_time != -1 &&
        game_ticks_to_seconds((float)(game_time_get() - m_control_time)) <
            g_network_configuration.simulation_actor_control_timeout)
    {
        *unit_index = m_unit_index;
        memcpy(control, &m_control, sizeof(unit_control_data));
        result = true;
    }
    return result;
}
