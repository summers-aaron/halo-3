// networkinglibxbox/network_storage_files.obj: from the records 0x411280 0x4112c8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
#include "networking/logic/storage/network_storage_files.h"
#include "math/real_math_types.h"



struct s_matchmaking_hopper_statistics
{
    long total_population;
    long hopper_population;
};

void matchmaking_hopper_statistics_encode_function(c_bitstream *packet, const void *data, long data_size)
{
    const s_matchmaking_hopper_statistics *statistics = (const s_matchmaking_hopper_statistics *)data;
    packet->write_integer("total_population", statistics->total_population + 1, 32);
    packet->write_integer("hopper_population", statistics->hopper_population + 1, 32);
}

bool matchmaking_hopper_statistics_decode_function(c_bitstream *packet, void *data, long data_size)
{
    bool success = true;
    s_matchmaking_hopper_statistics *statistics = (s_matchmaking_hopper_statistics *)data;
    statistics->total_population = packet->read_integer("total_population", 32) - 1;
    statistics->hopper_population = packet->read_integer("hopper_population", 32) - 1;
    success = success && !packet->error_occurred();
    return success;
}
