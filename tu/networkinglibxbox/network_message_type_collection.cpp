// networkinglibxbox/network_message_type_collection.obj: from the records 0x3eeb48 0x3eebb8 0x3eec58
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x3eeb48 ?encode_message@c_network_message_type_collection@@QBAXPAVc_bitstream@@W4e_network_message_type@@JPBX@Z
// @address 0x3eebb8 ?decode_message@c_network_message_type_collection@@QBA_NPAVc_bitstream@@PAW4e_network_message_type@@PAJPAX@Z
// @address 0x3eec58 ?decode_message_header@c_network_message_type_collection@@QBA_NPAVc_bitstream@@PAW4e_network_message_type@@PAJ@Z
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "memory/bitstream.h"
#include "networking/messages/network_message_type_collection.h"
#include "math/real_math_types.h"


__forceinline void c_network_message_type_collection::encode_message_header(c_bitstream *packet, e_network_message_type type, long size) const
{
    packet->write_integer("type", type, 8);
    packet->write_integer("size", size, 16);
}
void c_network_message_type_collection::encode_message(c_bitstream *packet, e_network_message_type type, long size, const void *message) const
{
    const s_network_message_type_definition *definition = &m_message_types[type];
    encode_message_header(packet, type, size);
    if (!(definition->flags & 2))
        definition->encode(packet, size, message);
}
bool c_network_message_type_collection::decode_message(
    // match: 0x3eebb8: __restrict reuses *size from r30 for memset/decode args (one lwz) instead of two lwz reloads + r28/r29 swap
    c_bitstream *packet, e_network_message_type *message_type, long *__restrict message_storage_size, void *message_storage) const
{
    bool result = false;
    if (decode_message_header(packet, message_type, message_storage_size))
    {
        const s_network_message_type_definition *type_definition = &m_message_types[*message_type];
        result = true;
        if (!(type_definition->flags & 2))
        {
            memset(message_storage, 0, *message_storage_size);
            result = type_definition->decode(packet, *message_storage_size, message_storage);
        }
    }
    return result;
}
// match: 0x3eec58: __restrict delays the *size store past the error_occurred loads (lwz 8/18h, slwi) instead of right after the second read
bool c_network_message_type_collection::decode_message_header(c_bitstream *packet, e_network_message_type *type, long *__restrict size) const
{
    bool result = false;
    *type = (e_network_message_type)packet->read_integer("type", 8);
    *size = packet->read_integer("size", 16);
    if (!packet->error_occurred() && *type >= 0 && *type < k_network_message_type_count)
    {
        const s_network_message_type_definition *definition = &m_message_types[*type];
        if (definition->initialized && *size >= definition->minimum_size && *size <= definition->maximum_size)
            result = !packet->error_occurred();
    }
    return result;
}
