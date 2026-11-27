// ailibxbox/activities.obj: from the records 0x518b28 0x518b90 0x518ca8 0x518dd8
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "ai/behavior_posture.h"
#include "ai/behavior.h"
#include "ai/actor_stimulus.h"
#include "memory/data.h"
#include "tag_files/tag_types.h"
#include "ai/actor_datum.h"
#include "cseries/cseries.h"
// @address 0x518b28 ?activities_try_and_get_posture@@YAPAUbehavior_posture_info@@H@Z
// @address 0x518b90 activity_index_get_by_name
// @address 0x518ca8 ?actor_activity_set@@YA_NJFFF_N@Z
// @address 0x518dd8 ?actor_get_patrol_block@@YAPAUs_tag_block@@JPAF@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "ai/activities.h"
#include "ai/behavior_stimulus_types.h"
#include "ai/squad_definitions.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "scenario/scenario_access.h"
#include "tag_files/tag_groups.h"
#include "game/game_time_inline.h"


extern "C" {
// @address 0x231a8 k_behavior_posture_type_names

}
extern "C" {
// @address 0x23238 k_posture_variant_indices

}
// @address 0x237b0 g_activity_info
// @rdata 0x820237b0..0x820239e0
const s_activity_info g_activity_info[35] = {
    { 0, 0, { 1, 2 } },
    { 1, 0, { 1, 2 } },
    { 1, 0, { 1, 2 } },
    { 1, 1, { 1, 2 } },
    { 1, 0, { 1, 1 } },
    { 1, 1, { 1, 1 } },
    { 2, 0, { 1, 1 } },
    { 2, 0, { 1, 1 } },
    { 2, 1, { 1, 1 } },
    { 2, 0, { 1, 1 } },
    { 2, 0, { 1, 1 } },
    { 2, 0, { 1, 1 } },
    { 2, 0, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 1, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 1, { 1, 1 } },
    { 2, 0, { 1, 0 } },
    { 2, 1, { 3, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 1, { 1, 0 } },
    { 2, 1, { 1, 0 } },
    { 2, 1, { 1, 0 } },
    { 2, 1, { 1, 0 } },
    { 2, 0, { 1, 0 } },
    { 2, 1, { 0, 0 } },
    { 2, 1, { 3, 0 } }
};
// @rdata-end
// @address 0x72f784 ?g_activity_exit_combat_status@@3PAFA
short g_activity_exit_combat_status[4] = { 10, 2, 6, 10 };
extern "C" {
// @address 0x239e0 k_behavior_posture_info

}
extern "C" {
// @address 0x897e20 g_behavior_posture_variant_counts
short g_behavior_posture_variant_counts[35];
}

static __forceinline int activities_posture_out_of_range(short posture_index)
{
    if ((posture_index <= 0)) return 1;
    if ((posture_index >= 35)) return 1;
    return 0;
}


behavior_posture_info *activities_try_and_get_posture(int key)
{
    behavior_posture_info *table = k_behavior_posture_info;
    behavior_posture_info *result = 0;
    int found = 0;
    short i = 0;
    do
    {
        if (table[i].name == key)
            found = i;
        i = (short)(i + 1);
    } while (i < 60);
    if (found != 0)
        result = &table[found];
    return result;
}

unsigned char activity_index_get_by_name(int activity_name, short *found_posture, short *found_variant)
{
    short posture_scan_index;
    const long *posture_variant_table;
    short variant_count;
    short variant_scan_index;
    int variant_info_index;
    unsigned char matched;
    short posture_index;
    short variant_index;
    matched = 0;
    posture_index = 0;
    do
    {
        posture_scan_index = posture_index;
        if (posture_scan_index >= 35)
            break;
        if (activity_name == k_behavior_posture_type_names[posture_scan_index]) {
            if (found_posture != NULL) {
                *found_posture = posture_index;
            }
            if (found_variant != NULL) {
                *found_variant = -1;
            }
            matched = 1;
        } else {
            variant_index = 0;
            posture_variant_table = k_posture_variant_indices[posture_scan_index];
            if (!matched) {
                variant_count = g_behavior_posture_variant_counts[posture_scan_index];
                do {
                    variant_scan_index = variant_index;
                    if (variant_scan_index >= variant_count)
                        break;
                    variant_info_index = posture_variant_table[variant_scan_index];
                    if (k_behavior_posture_info[variant_info_index].name == activity_name) {
                        if (found_posture != NULL) {
                            *found_posture = posture_index;
                        }
                        if (found_variant != NULL) {
                            *found_variant = variant_index;
                        }
                        matched = 1;
                    }
                    variant_index = (short)(variant_scan_index + 1);
                } while (!matched);
            }
        }
        posture_index = (short)(posture_scan_index + 1);
    } while (!matched);
    return matched;
}

bool actor_activity_set(long actor_datum_index, short new_activity_index, short activity_variant, short new_activity_parameter, bool new_activity_flag)
{
    short activity_index = new_activity_index;
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_datum_index);
    if (!activities_posture_out_of_range(activity_index)) {
        if (g_activity_info[activity_index].name == 2) {
            if (g_behavior_posture_variant_counts[activity_index] > 0) {
                if (activity_variant < 0 || activity_variant >= g_behavior_posture_variant_counts[activity_index])
                    activity_variant = 0;
                result = true;
            }
        } else {
            activity_variant = -1;
            result = true;
        }
    }
    if (result) {
        actor->activity_index = new_activity_index;
        actor->activity_variant = activity_variant;
        actor->activity_parameter = new_activity_parameter;
        actor->activity_flag = new_activity_flag;
        stimulus_register(actor_datum_index, _behavior_stimulus_activity,
            (short)game_seconds_to_ticks_round(0.2f), 3);
    }
    return result;
}

s_tag_block *actor_get_patrol_block(long actor_index, short *patrol_point_count)
{
    s_tag_block *patrol_block = 0;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);

    if (patrol_point_count)
        *patrol_point_count = NONE;

    if (actor->activity_index > 0 && (unsigned short)actor->activity_parameter != 0xffff && actor->squad_index != NONE)
    {
        squad_definition *squad = (squad_definition *)tag_block_get_element_with_size(
            &global_scenario_get()->squads, actor->squad_index & 0xffff, sizeof(squad_definition));
        if (actor->cell_index >= 0 && actor->cell_index < squad->fire_team_block.count)
        {
            fire_team_definition *cell = (fire_team_definition *)tag_block_get_element_with_size(
                &squad->fire_team_block, actor->cell_index, sizeof(fire_team_definition));
            if (actor->activity_flag)
            {
                if (actor->starting_location_index >= 0 && actor->starting_location_index < cell->starting_locations.count)
                {
                    actor_starting_location_definition *starting_location = (actor_starting_location_definition *)tag_block_get_element_with_size(
                        &cell->starting_locations, actor->starting_location_index, sizeof(actor_starting_location_definition));
                    patrol_block = &starting_location->patrol_block;
                    if (patrol_point_count)
                        *patrol_point_count = starting_location->patrol_point_count;
                }
            }
            else
            {
                patrol_block = &cell->patrol_block;
                if (patrol_point_count)
                    *patrol_point_count = cell->patrol_point_count;
            }
        }
    }
    return patrol_block;
}
