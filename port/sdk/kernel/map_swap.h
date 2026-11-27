// port/sdk/kernel (both hosts): cache-file byte order at the SDK boundary.
//
// Retail .map files are big-endian; the game reads them raw (port/GAME-CHANGES.md D1-D3 are the proper hooks in the
// game code, still to land in tu/). Until then this layer converts them where the bytes cross the SDK, without the
// game knowing:
//
//   header reads   A ReadFile of exactly 0x3000 bytes at file offset 0 that yields a big-endian s_cache_file_header
//                  ('head' / 'foot' in disk order) is converted to host order in the caller's buffer
//                  (port_cache_swap_header): cache_files_windows_read_header, game_state_read_from_file (the d:\maps
//                  files and the utility-drive copies alike).
//   header writes  A WriteFile of exactly 0x3000 bytes at offset 0 of a host-order header writes it in disk order
//                  (from a scratch copy; the caller's buffer is untouched): the utility-drive map copy
//                  (cache_files_windows_update_map_copy, _cache_file_copy_write_header) keeps cache0:\ files in the
//                  same byte order as the disc, so its body bytes, copied verbatim, and its header agree.
//   hashing        XHashMemoryUpdate over a 0x3000-byte host-order header hashes it in disk order (the digest and
//                  RSA signature are over disk bytes): cache_file_header_verify_signature, scenario_tags_load's
//                  section 0. That header (scenario_tags_load hashes the active map's) is remembered, and the
//                  following updates that walk its tag section from tag_memory_base are counted.
//   tags           When XeCryptBnDwLePkcs1Verify accepts a signature after the remembered map's whole tag section
//                  (sections 1 and 2) was hashed, and its tags header still reads 'tags' in disk order, the section is
//                  converted in place (port_cache_swap_tags) before the verification returns: scenario_tags_load
//                  checks tags_header->signature == 'tags' right after.
//
// Everything is idempotent with D1-D3: host-order headers are never converted again, disk-order headers are never
// "restored", a tag section whose signature already reads host order is left alone. HALO3_NO_AUTO_MAP_SWAP=1 turns
// the layer off. Asynchronous Windows overlapped I/O that completes later (ERROR_IO_PENDING) is not seen; the game
// reads and writes headers synchronously (async_helpers.cpp: ReadFile / WriteFile with no OVERLAPPED).
#ifndef PORT_KERNEL_MAP_SWAP_H
#define PORT_KERNEL_MAP_SWAP_H

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_MAP_SWAP_HEADER_SIZE 0x3000u

// After a read of `requested` bytes at file offset `offset` that returned `done` bytes into `buffer`.
void port_map_swap_after_read(void *buffer, unsigned long requested, unsigned long done, unsigned long long offset);

// Before a write of `bytes` from `buffer` at file offset `offset`: the bytes to write instead (`scratch`, 0x3000
// bytes the caller keeps until the write completes) or `buffer` itself.
const void *port_map_swap_before_write(const void *buffer, unsigned long bytes, unsigned long long offset,
    void *scratch);

// Before XHashMemoryUpdate hashes `size` bytes at `data`: the bytes to hash (`scratch` or `data`).
const void *port_map_swap_before_hash(const void *data, unsigned long size, void *scratch);

// After XeCryptBnDwLePkcs1Verify returned `verified`. Returns the result to give the caller (0 if converting the
// verified map's tag section failed).
int port_map_swap_after_verify(int verified);

#ifdef __cplusplus
}
#endif

#endif
