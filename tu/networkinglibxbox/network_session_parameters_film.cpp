// networkinglibxbox/network_session_parameters_film.obj: from the records 0x4446e0 0x4448a8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x2209d0 ?read_string_wchar@c_bitstream@@QAAXPBDPAGJ@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
#include "memory/bitstream.h"
#include "networking/session/network_session_parameters_film.h"

struct s_session_parameters_film_record
{
    long kind;
    long value_a;
    long value_b;
    long value_c;
    char name[256];
    char table[0x100];
    long count;
    long value_d;
};

#include "memory/bitstream_wide_strings.h"

// @address 0x4448a8 saved_film_decode
extern "C" bool saved_film_decode(c_bitstream *packet, s_session_parameters_film_record *record)
{
    record->kind = packet->read_integer("kind", 4);
    record->value_a = packet->read_integer("value-a", 32);
    record->value_b = packet->read_integer("value-b", 32);
    record->value_c = packet->read_integer("value-c", 2);
    packet->read_string("name", record->name, 256);
    packet->read_string_wchar("film-name", (unsigned short *)record->table, 128);
    record->count = packet->read_integer("count", 3) - 1;
    record->value_d = packet->read_integer("value-d", 32);
    bool success = true;
    success = success && !packet->error_occurred();
    return success;
}

// @address 0x4446e0 saved_film_encode
extern "C" void saved_film_encode(c_bitstream *packet, const s_session_parameters_film_record *record)
{
    packet->write_integer("kind", record->kind, 4);
    packet->write_integer("value-a", record->value_a, 32);
    packet->write_integer("value-b", record->value_b, 32);
    packet->write_integer("value-c", record->value_c, 2);
    packet->write_string("name", record->name, 256);
    packet->write_string_wchar("film-name", (const unsigned short *)record->table, 128);
    packet->write_integer("count", record->count + 1, 3);
    packet->write_integer("value-d", record->value_d, 32);
}
