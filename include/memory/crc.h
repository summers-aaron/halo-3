#ifndef MEMORY_CRC_H
#define MEMORY_CRC_H

// Functions defined in tu/blamlibxbox/crc.cpp that other units call.

unsigned long crc_checksum_buffer(unsigned long crc, const void *buffer, long buffer_size);
unsigned long fast_checksum_buffer(unsigned long checksum, const void *buffer, long buffer_size);

extern "C" {
extern bool g_crc_table_initialized;
extern unsigned long g_crc_table[256];
}

#endif // MEMORY_CRC_H
