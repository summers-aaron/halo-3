#ifndef NETWORKING_TOOLS_NETWORK_BLF_H
#define NETWORKING_TOOLS_NETWORK_BLF_H

#include "cseries/byte_swapping.h"
#include "networking/tools/blf_authentication_types.h"

// Title-server chunk payloads stay in big-endian order in the download cache.
static __forceinline unsigned long network_blf_payload_unsigned_long(unsigned long value)
{
#if defined(_XBOX) && !defined(__clang__)
	return value;
#elif defined(__clang__) || defined(__GNUC__)
	return __builtin_bswap32(value);
#else
	return byte_swap_long(value);
#endif
}

static __forceinline long network_blf_payload_long(long value)
{
#if defined(_XBOX) && !defined(__clang__)
	return value;
#else
	return (int)network_blf_payload_unsigned_long((unsigned long)value);
#endif
}

// Functions defined in tu/networkinglibxbox/network_blf.cpp that other units call.

bool network_blf_verify_start_of_file(const char *buffer, long buffer_count, bool *out_byte_swap, long *out_file_size);
bool network_blf_read_for_known_chunk(const char *buffer, long buffer_count, bool byte_swap, long chunk_type,
	short major_version, long *out_chunk_size, const char **out_chunk_buffer, long *out_chunk_buffer_size,
	short *out_minor_version, bool *out_eof_found);
bool network_blf_read_next_known_chunk(const char *buffer, long buffer_count, bool byte_swap, long chunk_type,
	short major_version, long *out_chunk_size, const char **out_chunk_buffer, long *out_chunk_buffer_size,
	short *out_minor_version, bool *out_eof_found);
bool network_blf_verify_end_of_file( const char *buffer, long buffer_count, bool byte_swap, const char *chunk,
	e_blf_file_authentication_type authentication_type);
void *network_blf_find_chunk_in_file(const char *buffer, unsigned long size, long chunk_type, short major_version,
	e_blf_file_authentication_type authentication_type, unsigned long *out_size);

#endif // NETWORKING_TOOLS_NETWORK_BLF_H
