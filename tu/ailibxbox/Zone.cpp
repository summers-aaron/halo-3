// ailibxbox/Zone.obj: from the records 0xcc0c0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "tag_files/tag_groups.h"
#include "ai/Zone.h"



#include "scenario/scenario_tag.h"

#include "scenario/scenario.h"

#include "ai/zone_access.h"

// @address 0x72cfbc ?ZONE_AREA_REF_NONE@@3Uzone_area_ref@@A
zone_area_ref ZONE_AREA_REF_NONE = { { { NONE, NONE } } };

area_definition *zone_get_area(short zone_index, short area_index)
{
    area_definition *area = 0;
    scenario *scenario_data = global_scenario_get();
    if (zone_index >= 0 && zone_index < scenario_data->zones.count)
    {
        zone_definition *zone = zone_definition_get((unsigned short)zone_index);
        if (area_index >= 0 && area_index < zone->areas.count)
        {
            area = TAG_BLOCK_GET_ELEMENT(&zone->areas, area_index, area_definition);
        }
    }
    return area;
}
