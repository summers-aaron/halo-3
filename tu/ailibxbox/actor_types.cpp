// ailibxbox/actor_types.obj: from the records 0x505b80
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "ai/actor_datum.h"
#include "ai/squad_types.h"

#include <string.h>
#include "ai/objectives.h"
#include "ai/squads.h"
#include "ai/actor_types.h"
#include "ai/objective_runtime.h"
#include "ai/objective_definitions.h"
#include "ai/objective_access.h"
#include "memory/data.h"
#include "ai/behavior.h"
extern "C" {
// @address 0x22ba0 k_behavior_follow_ranges
const float k_behavior_follow_ranges[2] = { 16.0f, 100.0f };
}
// @address 0x24268 ?g_actor_type_default_team@@3QBFB
// @rdata 0x82024268..0x820242d0
const short g_actor_type_default_team[] = {
    3, 7, 3, 3, 3, 3, 1, 2, 2, 4, 4, 4, 5,
    5, -1, 3, 7, 7, 7, 4, 4, 4, 4, 3, 5, 0
};
// @address 0x2429c ?g_actor_type_leader_type@@3QBFB
const short g_actor_type_leader_type[] = {
    -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, 17, -1, -1, -1, -1, -1, -1, -1, -1, 0
};
// @rdata-end
// @address 0x505b80 ?actor_is_leader@@YA_NJJ@Z
bool actor_is_leader(long actor_index, long leader_index)
{
    actor_datum *leader = datum_get<actor_datum>(tls_actor_data(), leader_index);
    actor_datum *member = datum_get<actor_datum>(tls_actor_data(), actor_index);
    bool result = false;
    if (leader->squad_index != -1 && member->squad_index != -1) {
        squad_datum *leader_squad = datum_get<squad_datum>(tls_squad_data(), leader->squad_index);
        squad_datum *member_squad = datum_get<squad_datum>(tls_squad_data(), leader->squad_index);
        if (::squad_valid_task(leader_squad) &&
            ::squad_get_task(leader_squad) == ::squad_get_task(member_squad) &&
            objective_get_task_record(leader_squad->objective, leader_squad->task)->leader == leader_index)
            result = true;
    }
    return result;
}
