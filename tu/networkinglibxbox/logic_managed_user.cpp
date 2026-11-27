// networkinglibxbox/logic_managed_user.obj: from the records 0x43ce08
// @flags /TP /O2 /Oc
#include <xtl.h>
#include "shell/shell.h"
#include <stddef.h>
// @address 0x43ce08 ?managed_user_set_property@@YAXW4e_controller_index@@W4e_online_property_id@@PBUs_online_data@@@Z

#include "networking/logic/logic_managed_user.h"
#include "networking/online_property_id_types.h"
#include "networking/logic/logic_managed_user_types.h"
s_managed_user_globals managed_user_globals;
// @address 0x73a5b8 k_online_property_id_map
unsigned long k_online_property_id_map[] =
{
    0x10008103,
    0x10008101,
    0x40008109,
    0x20008108,
    0x10008010,
    0x3000800e,
    0x3000800f,
    0x1000800a,
    0x1000800b,
    0x1000800d,
    0x10000016,
    0x10000003,
    0x10000004,
    0x10000005,
    0x10000006,
    0x10000007,
    0x10000008,
    0x10000009,
    0x1000000a,
    0x1000000b,
    0x1000000c,
    0x1000000d,
    0x1000000e,
    0x1000000f,
    0x10000010,
    0x10000011,
    0x10000012,
    0x10000013,
    0x10000015,
    0x1000002b,
    0x1000002c,
    0x1000002d,
    0x1000002e,
    0x1000002f,
    0x10000030,
    0x10000037,
    0x10000038,
    0x10000039,
    0x1000003a,
    0x1000003b,
    0x1000003c,
    0x1000003d,
    0x1000003e,
    0x1000003f,
    0x10000040,
    0x10000041,
    0x10000042,
    0x10000043,
    0x1000004d,
    0x5000002a,
    0x10000049,
    0x1000004a,
    0x1000004b,
    0x1000004c,
    0x5000004e,
    0x5000004f,
    0x10000047,
    0x10000048,
    0x20000014,
};
static __forceinline unsigned long online_constant_get_property_id(e_online_property_id property_id)
{
    return k_online_property_id_map[property_id];
}
static __forceinline void online_user_set_property(e_controller_index controller_index, e_online_property_id property_id, const s_online_data *value)
{
    unsigned long xbox_property_id = online_constant_get_property_id(property_id);
    XUserSetPropertyEx(controller_index, xbox_property_id, 4, &value->value, NULL);
}

void managed_user_set_property(e_controller_index controller_index, e_online_property_id property_id, const s_online_data *value)
{
    s_managed_user_property_data *property = &managed_user_globals.users[controller_index].properties[property_id];
    property->data = *value;
    property->valid = true;
    online_user_set_property(controller_index, property_id, &property->data);
}
