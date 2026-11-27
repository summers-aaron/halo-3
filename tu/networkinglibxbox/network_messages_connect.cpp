// networkinglibxbox/network_messages_connect.obj: from the records 0x3ef290 0x3ef2d0 0x3ef360 0x3ef3a0 0x3ef430 0x3ef470 0x3ef500 0x3ef550 0x3ef610
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
// @address 0x3ef430 ?encode@c_network_message_connect_establish@@SAXPAVc_bitstream@@JPBX@Z
// @address 0x3ef500 ?encode@c_network_message_connect_closed@@SAXPAVc_bitstream@@JPBX@Z

#include "networking/messages/network_messages_connect.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/network_globals.h"
void c_network_message_connect_request::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_connect_request *message = (const s_network_message_connect_request *)message_storage;
    stream->write_integer("identifier", message->identifier, 32);
    stream->write_integer("flags", message->flags, 4);
}

bool c_network_message_connect_request::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_connect_request *message = (s_network_message_connect_request *)message_storage;
    message->identifier = stream->read_integer("identifier", 32);
    message->flags = stream->read_integer("flags", 4);
    bool result = !stream->error_occurred();
    return result;
}

void c_network_message_connect_refuse::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_connect_refuse *message = (const s_network_message_connect_refuse *)message_storage;
    stream->write_integer("remote-identifier", message->remote_identifier, 32);
    stream->write_integer("reason", message->reason, 3);
}

bool c_network_message_connect_refuse::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_connect_refuse *message = (s_network_message_connect_refuse *)message_storage;
    message->remote_identifier = stream->read_integer("remote-identifier", 32);
    message->reason = stream->read_integer("reason", 3);
    bool result = !stream->error_occurred();
    return result;
}

void c_network_message_connect_establish::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_connect_establish *message = (const s_network_message_connect_establish *)message_storage;
    stream->write_integer("remote-identifier", message->remote_identifier, 32);
    stream->write_integer("identifier", message->identifier, 32);
}

bool c_network_message_connect_establish::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_connect_establish *message = (s_network_message_connect_establish *)message_storage;
    message->remote_identifier = stream->read_integer("remote-identifier", 32);
    message->identifier = stream->read_integer("identifier", 32);
    return !stream->error_occurred();
}

void c_network_message_connect_closed::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_connect_closed *message = (const s_network_message_connect_closed *)message_storage;
    stream->write_integer("remote-identifier", message->remote_identifier, 32);
    stream->write_integer("identifier", message->identifier, 32);
    stream->write_integer("closure-reason", message->closure_reason, 5);
}

bool c_network_message_connect_closed::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_connect_closed *message = (s_network_message_connect_closed *)message_storage;
    message->remote_identifier = stream->read_integer("remote-identifier", 32);
    message->identifier = stream->read_integer("identifier", 32);
    message->closure_reason = stream->read_integer("closure-reason", 5);
    return !stream->error_occurred() && message->closure_reason >= 0 && message->closure_reason < 20;
}

typedef void (*network_message_connect_encode_proc)(c_bitstream *,long,const void *);
typedef bool (*network_message_connect_decode_proc)(c_bitstream *,long,void *);
typedef bool (*network_message_connect_compare_proc)(long,void *,void *);
typedef void (*network_message_connect_dispose_proc)(long,void *);
// @address 0x8c4450 g_network_message_type_collection
static __forceinline void network_message_connect_register_type(c_network_message_type_collection *c, long id, const char *name, long flags,
    long size, long maximum, network_message_connect_encode_proc encode, network_message_connect_decode_proc decode,
    network_message_connect_compare_proc compare, network_message_connect_dispose_proc dispose)
{
 s_network_message_type_definition *type=&c->m_message_types[id];
 type->name=name;type->flags=flags;type->minimum_size=size;type->maximum_size=maximum;
 type->encode=encode;type->decode=decode;type->compare=compare;type->dispose=dispose;
 type->initialized=true;
}
// @address 0x3ef610 network_message_types_register_connect
extern "C" void network_message_types_register_connect(c_network_message_type_collection *message_collection) {
 c_network_message_type_collection **home = &message_collection;
 c_network_message_type_collection *c=g_network_message_type_collection;
 network_message_connect_register_type(c,4,"connect-request",0,8,8,&c_network_message_connect_request::encode,&c_network_message_connect_request::decode,0,0);
 network_message_connect_register_type(c,5,"connect-refuse",0,8,8,&c_network_message_connect_refuse::encode,&c_network_message_connect_refuse::decode,0,0);
 network_message_connect_register_type(
     c, 6, "connect-establish", 0, 8, 8, &c_network_message_connect_establish::encode, &c_network_message_connect_establish::decode, 0, 0);
 network_message_connect_register_type(c,7,"connect-closed",0,12,12,&c_network_message_connect_closed::encode,&c_network_message_connect_closed::decode,0,0);
}
