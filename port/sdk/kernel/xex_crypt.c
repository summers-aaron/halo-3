// port/sdk/kernel (both hosts): XEX2 resource section loader (XexGetModuleHandle, XexGetModuleSection) and Xbox
// SHA-1 / RSA-2048 verification primitives (XHashMemory*, XeCryptSha*, XeCryptBn*).
//
// Reads "d:\default.xex" from the user's retail disc at runtime, decrypts and expands its image once in memory, and
// serves the XEX resource table ("00" SHA-1 salt & RSA-2048 public key, "01" map hash manifest, etc.) directly.
// Zero proprietary bytes live in this file; the 16-byte XEX2 retail key is the standard public format constant
// already used in tools/rust/debugnames-rs.
#include "../runtime/port_runtime.h"
#include "map_swap.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

extern void *PORT_MS_ABI CreateFileA(const char *name, u32 access, u32 share, void *sec, u32 disp, u32 flags, void *tmpl);
extern u32 PORT_MS_ABI GetFileSize(void *handle, u32 *high);
extern int PORT_MS_ABI ReadFile(void *handle, void *buf, u32 bytes, u32 *done, void *overlapped);
extern int PORT_MS_ABI CloseHandle(void *handle);
extern void *PORT_MS_ABI GetProcessHeap(void);
extern void *PORT_MS_ABI RtlAllocateHeap(void *heap, u32 flags, u32 bytes);
extern int PORT_MS_ABI RtlFreeHeap(void *heap, u32 flags, void *mem);

#define PORT_INVALID_HANDLE ((void *)-1)
#define PORT_STATUS_SUCCESS 0l
#define PORT_STATUS_NOT_FOUND ((long)0xc0000225)
#define PORT_XEX_BASE 0x82000000ul

// ---------------------------------------------------------------------------------------------------------------
// Minimal AES-128 decrypt (for XEX2 retail image expansion).

static const u8 k_aes_sbox_inv[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const u8 k_aes_sbox[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static u8 g_mul9[256], g_mul11[256], g_mul13[256], g_mul14[256];

static u8 gf_mul(u8 a, u8 b)
{
    u8 p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        u8 hi = a & 0x80;
        a <<= 1;
        if (hi) a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

static void aes128_init_tables(void)
{
    for (int i = 0; i < 256; i++) {
        g_mul9[i]  = gf_mul((u8)i, 9);
        g_mul11[i] = gf_mul((u8)i, 11);
        g_mul13[i] = gf_mul((u8)i, 13);
        g_mul14[i] = gf_mul((u8)i, 14);
    }
}

static void aes128_expand_key(const u8 key[16], u8 rk[176])
{
    __builtin_memcpy(rk, key, 16);
    u8 rcon = 1;
    for (int i = 16; i < 176; i += 4) {
        u8 t[4] = { rk[i - 4], rk[i - 3], rk[i - 2], rk[i - 1] };
        if (i % 16 == 0) {
            u8 u = t[0];
            t[0] = k_aes_sbox[t[1]] ^ rcon;
            t[1] = k_aes_sbox[t[2]];
            t[2] = k_aes_sbox[t[3]];
            t[3] = k_aes_sbox[u];
            rcon = gf_mul(rcon, 2);
        }
        for (int j = 0; j < 4; j++)
            rk[i + j] = rk[i - 16 + j] ^ t[j];
    }
}

static void aes128_decrypt_block(const u8 rk[176], const u8 in[16], u8 out[16])
{
    u8 s[16], t[16];
    for (int i = 0; i < 16; i++)
        s[i] = in[i] ^ rk[160 + i];

    for (int round = 9; round >= 1; round--) {
        // InvShiftRows + InvSubBytes
        t[0]  = k_aes_sbox_inv[s[0]];  t[1]  = k_aes_sbox_inv[s[13]]; t[2]  = k_aes_sbox_inv[s[10]]; t[3]  = k_aes_sbox_inv[s[7]];
        t[4]  = k_aes_sbox_inv[s[4]];  t[5]  = k_aes_sbox_inv[s[1]];  t[6]  = k_aes_sbox_inv[s[14]]; t[7]  = k_aes_sbox_inv[s[11]];
        t[8]  = k_aes_sbox_inv[s[8]];  t[9]  = k_aes_sbox_inv[s[5]];  t[10] = k_aes_sbox_inv[s[2]];  t[11] = k_aes_sbox_inv[s[15]];
        t[12] = k_aes_sbox_inv[s[12]]; t[13] = k_aes_sbox_inv[s[9]];  t[14] = k_aes_sbox_inv[s[6]];  t[15] = k_aes_sbox_inv[s[3]];

        for (int i = 0; i < 16; i++)
            t[i] ^= rk[round * 16 + i];

        // InvMixColumns
        for (int c = 0; c < 4; c++) {
            u8 a0 = t[c * 4 + 0], a1 = t[c * 4 + 1], a2 = t[c * 4 + 2], a3 = t[c * 4 + 3];
            s[c * 4 + 0] = g_mul14[a0] ^ g_mul11[a1] ^ g_mul13[a2] ^ g_mul9[a3];
            s[c * 4 + 1] = g_mul9[a0]  ^ g_mul14[a1] ^ g_mul11[a2] ^ g_mul13[a3];
            s[c * 4 + 2] = g_mul13[a0] ^ g_mul9[a1]  ^ g_mul14[a2] ^ g_mul11[a3];
            s[c * 4 + 3] = g_mul11[a0] ^ g_mul13[a1] ^ g_mul9[a2]  ^ g_mul14[a3];
        }
    }

    out[0]  = k_aes_sbox_inv[s[0]]  ^ rk[0];  out[1]  = k_aes_sbox_inv[s[13]] ^ rk[1];
    out[2]  = k_aes_sbox_inv[s[10]] ^ rk[2];  out[3]  = k_aes_sbox_inv[s[7]]  ^ rk[3];
    out[4]  = k_aes_sbox_inv[s[4]]  ^ rk[4];  out[5]  = k_aes_sbox_inv[s[1]]  ^ rk[5];
    out[6]  = k_aes_sbox_inv[s[14]] ^ rk[6];  out[7]  = k_aes_sbox_inv[s[11]] ^ rk[7];
    out[8]  = k_aes_sbox_inv[s[8]]  ^ rk[8];  out[9]  = k_aes_sbox_inv[s[5]]  ^ rk[9];
    out[10] = k_aes_sbox_inv[s[2]]  ^ rk[10]; out[11] = k_aes_sbox_inv[s[15]] ^ rk[11];
    out[12] = k_aes_sbox_inv[s[12]] ^ rk[12]; out[13] = k_aes_sbox_inv[s[9]]  ^ rk[13];
    out[14] = k_aes_sbox_inv[s[6]]  ^ rk[14]; out[15] = k_aes_sbox_inv[s[3]]  ^ rk[15];
}

static u16 rd_be16(const u8 *p) { return (u16)(((u16)p[0] << 8) | p[1]); }
static u32 rd_be32(const u8 *p) { return ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | p[3]; }
static u64 rd_be64(const u8 *p) { return ((u64)rd_be32(p) << 32) | rd_be32(p + 4); }
static void wr_be32(u8 *p, u32 v) { p[0] = (u8)(v >> 24); p[1] = (u8)(v >> 16); p[2] = (u8)(v >> 8); p[3] = (u8)v; }
static void wr_be64(u8 *p, u64 v) { wr_be32(p, (u32)(v >> 32)); wr_be32(p + 4, (u32)v); }

// ---------------------------------------------------------------------------------------------------------------
// XEX2 resource table & random-access CBC section loader.

typedef struct port_xex_resource {
    char name[9];
    u32 va;
    u32 size;
    u8 *data;
} port_xex_resource;

static volatile char port_xex_busy;
static int port_xex_loaded;
static port_xex_resource port_xex_resources[32];
static u32 port_xex_resource_count;

static void port_xex_load_once(void)
{
    while (__atomic_test_and_set(&port_xex_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
    if (port_xex_loaded) {
        __atomic_clear(&port_xex_busy, __ATOMIC_RELEASE);
        return;
    }
    port_xex_loaded = 1;
    aes128_init_tables();

    void *file = CreateFileA("d:\\default.xex", 0x80000000ul, 1, 0, 3, 0x80, 0);
    if (file == PORT_INVALID_HANDLE) {
        __atomic_clear(&port_xex_busy, __ATOMIC_RELEASE);
        return;
    }
    u32 xex_size = GetFileSize(file, 0);
    u8 *xex = xex_size != 0xfffffffful && xex_size > 0x200
        ? (u8 *)RtlAllocateHeap(GetProcessHeap(), 0, xex_size) : 0;
    u32 bytes_read = 0;
    if (xex == 0 || !ReadFile(file, xex, xex_size, &bytes_read, 0) || bytes_read != xex_size) {
        if (xex) RtlFreeHeap(GetProcessHeap(), 0, xex);
        CloseHandle(file);
        __atomic_clear(&port_xex_busy, __ATOMIC_RELEASE);
        return;
    }
    CloseHandle(file);

    if (xex[0] == 'X' && xex[1] == 'E' && xex[2] == 'X' && xex[3] == '2') {
        u32 pe_offset = rd_be32(xex + 8);
        u32 security = rd_be32(xex + 0x10);
        u32 header_count = rd_be32(xex + 0x14);
        u32 res_offset = 0, fmt_offset = 0;
        for (u32 i = 0; i < header_count && 0x18 + 8 * (i + 1) <= xex_size; i++) {
            u32 key = rd_be32(xex + 0x18 + 8 * i);
            u32 val = rd_be32(xex + 0x1c + 8 * i);
            if (key == 0x000002fful) res_offset = val;
            if (key == 0x000003fful) fmt_offset = val;
        }
        if (res_offset != 0 && res_offset + 4 <= xex_size) {
            u32 info_size = rd_be32(xex + res_offset);
            u32 n = info_size >= 4 ? (info_size - 4) / 16 : 0;
            if (n > 32) n = 32;
            for (u32 i = 0; i < n && res_offset + 4 + 16 * (i + 1) <= xex_size; i++) {
                const u8 *entry = xex + res_offset + 4 + 16 * i;
                __builtin_memcpy(port_xex_resources[i].name, entry, 8);
                port_xex_resources[i].name[8] = 0;
                port_xex_resources[i].va = rd_be32(entry + 8);
                port_xex_resources[i].size = rd_be32(entry + 12);
                port_xex_resources[i].data = 0;
                port_xex_resource_count++;
            }
        }
        if (fmt_offset != 0 && fmt_offset + 8 <= xex_size && pe_offset + 16 <= xex_size && security + 0x160 <= xex_size) {
            u32 fmt_size = rd_be32(xex + fmt_offset);
            u16 encryption = rd_be16(xex + fmt_offset + 4);
            u16 compression = rd_be16(xex + fmt_offset + 6);
            if (compression == 1 && fmt_size >= 8 && encryption <= 1) {
                u32 blocks = (fmt_size - 8) / 8;
                u32 total_img = 0;
                for (u32 i = 0; i < blocks; i++)
                    total_img += rd_be32(xex + fmt_offset + 8 + 8 * i) + rd_be32(xex + fmt_offset + 12 + 8 * i);

                u32 raw_len = ((xex_size - pe_offset) / 16) * 16;
                const u8 *raw = xex + pe_offset;
                u8 session_rk[176];
                int header_ok = 1;

                if (encryption == 1) {
                    static const u8 k_retail_key[16] = {
                        0x20,0xB1,0x85,0xA5,0x9D,0x28,0xFD,0xC3,0x40,0x58,0x3F,0xBB,0x08,0x96,0xBF,0x91
                    };
                    u8 master_rk[176], session_key[16], blk0[16];
                    aes128_expand_key(k_retail_key, master_rk);
                    aes128_decrypt_block(master_rk, xex + security + 0x150, session_key);
                    aes128_expand_key(session_key, session_rk);
                    aes128_decrypt_block(session_rk, raw, blk0);
                    header_ok = (blk0[0] == 'M' && blk0[1] == 'Z');
                } else {
                    header_ok = (raw[0] == 'M' && raw[1] == 'Z');
                }

                if (header_ok) {
                    for (u32 r = 0; r < port_xex_resource_count; r++) {
                        if (port_xex_resources[r].va < PORT_XEX_BASE || port_xex_resources[r].size == 0)
                            continue;
                        u32 res_off = port_xex_resources[r].va - PORT_XEX_BASE;
                        u32 res_end = res_off + port_xex_resources[r].size;
                        if (res_end > total_img)
                            continue;

                        u8 *buf = (u8 *)RtlAllocateHeap(GetProcessHeap(), 0x08 /* HEAP_ZERO_MEMORY */, port_xex_resources[r].size);
                        if (buf == 0)
                            continue;

                        u32 src_pos = 0, dst_pos = 0;
                        for (u32 i = 0; i < blocks && dst_pos < res_end; i++) {
                            u32 d = rd_be32(xex + fmt_offset + 8 + 8 * i);
                            u32 z = rd_be32(xex + fmt_offset + 12 + 8 * i);
                            u32 ov_start = dst_pos > res_off ? dst_pos : res_off;
                            u32 ov_end = (dst_pos + d) < res_end ? (dst_pos + d) : res_end;
                            if (ov_start < ov_end && src_pos + d <= raw_len) {
                                u32 raw_start = src_pos + (ov_start - dst_pos);
                                u32 raw_end = src_pos + (ov_end - dst_pos);
                                if (encryption == 0) {
                                    __builtin_memcpy(buf + (ov_start - res_off), raw + raw_start, raw_end - raw_start);
                                } else {
                                    u32 blk_first = raw_start & ~15u;
                                    u32 blk_last = (raw_end + 15u) & ~15u;
                                    for (u32 blk = blk_first; blk < blk_last && blk + 16u <= raw_len; blk += 16u) {
                                        u8 pt[16];
                                        aes128_decrypt_block(session_rk, raw + blk, pt);
                                        if (blk > 0) {
                                            for (int b = 0; b < 16; b++)
                                                pt[b] ^= raw[blk - 16u + b];
                                        }
                                        u32 copy_s = blk > raw_start ? blk : raw_start;
                                        u32 copy_e = (blk + 16u) < raw_end ? (blk + 16u) : raw_end;
                                        __builtin_memcpy(buf + (ov_start - res_off) + (copy_s - raw_start),
                                            pt + (copy_s - blk), copy_e - copy_s);
                                    }
                                }
                            }
                            src_pos += d;
                            dst_pos += d + z;
                        }
                        port_xex_resources[r].data = buf;
                    }
                }
            }
        }
    }

    RtlFreeHeap(GetProcessHeap(), 0, xex);
    __atomic_clear(&port_xex_busy, __ATOMIC_RELEASE);
}

long PORT_MS_ABI XexGetModuleHandle(char *module_name, void **module)
{
    if (module == 0)
        return PORT_STATUS_NOT_FOUND;
    if (module_name == 0 || module_name[0] == 0) {
        *module = (void *)PORT_XEX_BASE;
        return PORT_STATUS_SUCCESS;
    }
    *module = 0;
    return PORT_STATUS_NOT_FOUND;
}

long PORT_MS_ABI XexGetModuleSection(void *module, const char *section_name, void **section_data, u32 *section_size)
{
    if (module != (void *)PORT_XEX_BASE || section_name == 0)
        return PORT_STATUS_NOT_FOUND;
    port_xex_load_once();

    for (u32 i = 0; i < port_xex_resource_count; i++) {
        int match = 1;
        for (int k = 0; k < 8; k++) {
            char a = port_xex_resources[i].name[k];
            char b = section_name[k];
            if (a != b) { match = 0; break; }
            if (a == 0) break;
        }
        if (match && port_xex_resources[i].data != 0) {
            if (section_data) *section_data = port_xex_resources[i].data;
            if (section_size) *section_size = port_xex_resources[i].size;
            return PORT_STATUS_SUCCESS;
        }
    }
    return PORT_STATUS_NOT_FOUND;
}

// ---------------------------------------------------------------------------------------------------------------
// SHA-1: XHashMemory* & XeCryptSha* (88-byte state: { u32 count; u32 state[5]; u8 buffer[64]; }).

typedef struct port_sha1_state {
    u32 count;
    u32 state[5];
    u8 buffer[64];
} port_sha1_state;

_Static_assert(sizeof(port_sha1_state) == 88, "XHASH_STATE / XECRYPT_SHA_STATE is 88 bytes");

static u32 rotl32(u32 v, int n) { return (v << n) | (v >> (32 - n)); }

static void sha1_block(u32 state[5], const u8 block[64])
{
    u32 w[80];
    for (int i = 0; i < 16; i++)
        w[i] = rd_be32(block + i * 4);
    for (int i = 16; i < 80; i++)
        w[i] = rotl32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

    u32 a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
    for (int i = 0; i < 80; i++) {
        u32 f, k;
        if (i < 20)      { f = (b & c) | ((~b) & d); k = 0x5a827999u; }
        else if (i < 40) { f = b ^ c ^ d;            k = 0x6ed9eba1u; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8f1bbcdcu; }
        else             { f = b ^ c ^ d;            k = 0xca62c1d6u; }
        u32 t = rotl32(a, 5) + f + e + k + w[i];
        e = d; d = c; c = rotl32(b, 30); b = a; a = t;
    }
    state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
}

void XeCryptShaInit(void *raw_state)
{
    port_sha1_state *s = (port_sha1_state *)raw_state;
    s->count = 0;
    s->state[0] = 0x67452301u;
    s->state[1] = 0xefcdab89u;
    s->state[2] = 0x98badcfeu;
    s->state[3] = 0x10325476u;
    s->state[4] = 0xc3d2e1f0u;
}

void XeCryptShaUpdate(void *raw_state, const void *data, u32 size)
{
    port_sha1_state *s = (port_sha1_state *)raw_state;
    const u8 *p = (const u8 *)data;
    u32 buf_used = s->count & 63u;
    s->count += size;

    if (buf_used > 0) {
        u32 need = 64u - buf_used;
        if (size < need) {
            __builtin_memcpy(s->buffer + buf_used, p, size);
            return;
        }
        __builtin_memcpy(s->buffer + buf_used, p, need);
        sha1_block(s->state, s->buffer);
        p += need;
        size -= need;
    }
    while (size >= 64u) {
        sha1_block(s->state, p);
        p += 64u;
        size -= 64u;
    }
    if (size > 0)
        __builtin_memcpy(s->buffer, p, size);
}

void XeCryptShaFinal(void *raw_state, u8 *out, u32 out_size)
{
    port_sha1_state copy = *(const port_sha1_state *)raw_state;
    u64 bit_len = (u64)copy.count * 8ull;
    u8 pad = 0x80;
    XeCryptShaUpdate(&copy, &pad, 1);
    u8 zero = 0;
    while ((copy.count & 63u) != 56u)
        XeCryptShaUpdate(&copy, &zero, 1);
    u8 len_be[8];
    wr_be64(len_be, bit_len);
    XeCryptShaUpdate(&copy, len_be, 8);

    u8 digest[20];
    for (int i = 0; i < 5; i++)
        wr_be32(digest + i * 4, copy.state[i]);
    if (out != 0 && out_size > 0)
        __builtin_memcpy(out, digest, out_size < 20u ? out_size : 20u);
}

void XeCryptSha(const void *in1, u32 size1, const void *in2, u32 size2, const void *in3, u32 size3, u8 *out, u32 out_size)
{
    port_sha1_state s;
    XeCryptShaInit(&s);
    if (in1 && size1) XeCryptShaUpdate(&s, in1, size1);
    if (in2 && size2) XeCryptShaUpdate(&s, in2, size2);
    if (in3 && size3) XeCryptShaUpdate(&s, in3, size3);
    XeCryptShaFinal(&s, out, out_size);
}

void PORT_MS_ABI XHashMemoryBegin(void *state) { XeCryptShaInit(state); }
void PORT_MS_ABI XHashMemoryUpdate(void *state, const void *data, u32 size)
{
    if (size == PORT_MAP_SWAP_HEADER_SIZE) {
        // a cache-file header in host order is hashed as signed: in disk order (map_swap.h)
        unsigned char scratch[PORT_MAP_SWAP_HEADER_SIZE];
        XeCryptShaUpdate(state, port_map_swap_before_hash(data, size, scratch), size);
        return;
    }
    XeCryptShaUpdate(state, port_map_swap_before_hash(data, size, 0), size);
}
void PORT_MS_ABI XHashMemoryEnd(const void *state, u8 *digest)
{
    port_sha1_state copy = *(const port_sha1_state *)state;
    XeCryptShaFinal(&copy, digest, 20);
}

// ---------------------------------------------------------------------------------------------------------------
// RSA-2048 public verification (XeCryptBnQw_SwapDwQwLeBe, XeCryptBnQwNeRsaPubCrypt, XeCryptBnDwLePkcs1Verify).

void PORT_MS_ABI XeCryptBnQw_SwapDwQwLeBe(const void *input, void *output, u32 cqw)
{
    // Byte-swaps each 64-bit QWORD in place (swapping the two 32-bit DWORDs and byte-swapping each DWORD),
    // converting an array of cqw little-endian QWORDs (DwLe/QwLe) <-> big-endian QWORDs (PowerPC QwNe).
    const u8 *src = (const u8 *)input;
    u8 *dst = (u8 *)output;
    for (u32 q = 0; q < cqw; q++) {
        u8 tmp[8];
        for (int b = 0; b < 8; b++)
            tmp[b] = src[q * 8u + (u32)(7 - b)];
        __builtin_memcpy(dst + q * 8u, tmp, 8);
    }
}

static u32 inv32_neg(u32 n0)
{
    // Computes -n0^{-1} mod 2^32 via Newton's method.
    u32 x = 1;
    for (int i = 0; i < 5; i++)
        x *= 2u - n0 * x;
    return ~x + 1u;
}

static int ge_limbs32(const u32 *a, const u32 *b, u32 n)
{
    for (int i = (int)n - 1; i >= 0; i--) {
        if (a[i] != b[i])
            return a[i] > b[i];
    }
    return 1;
}

static void sub_limbs32(u32 *a, const u32 *b, u32 n)
{
    u64 borrow = 0;
    for (u32 i = 0; i < n; i++) {
        u64 bi = (u64)b[i] + borrow;
        u64 next_borrow = (u64)a[i] < bi;
        a[i] = (u32)((u64)a[i] - bi);
        borrow = next_borrow;
    }
}

static void mont_mul32(u32 *out, const u32 *a, const u32 *b, const u32 *mod, u32 k0, u32 n)
{
    u32 t[132];
    __builtin_memset(t, 0, sizeof(t));
    for (u32 i = 0; i < n; i++) {
        u64 carry = 0;
        for (u32 j = 0; j < n; j++) {
            u64 prod = (u64)a[i] * b[j] + t[i + j] + carry;
            t[i + j] = (u32)prod;
            carry = prod >> 32;
        }
        u64 sum = (u64)t[i + n] + carry;
        t[i + n] = (u32)sum;
        t[i + n + 1] += (u32)(sum >> 32);

        u32 m = t[i] * k0;
        carry = 0;
        for (u32 j = 0; j < n; j++) {
            u64 prod = (u64)m * mod[j] + t[i + j] + carry;
            t[i + j] = (u32)prod;
            carry = prod >> 32;
        }
        sum = (u64)t[i + n] + carry;
        t[i + n] = (u32)sum;
        t[i + n + 1] += (u32)(sum >> 32);
    }

    u32 res[128];
    for (u32 i = 0; i < n; i++)
        res[i] = t[n + i];
    if (t[2 * n] != 0 || ge_limbs32(res, mod, n))
        sub_limbs32(res, mod, n);
    __builtin_memcpy(out, res, n * sizeof(u32));
}

int PORT_MS_ABI XeCryptBnQwNeRsaPubCrypt(const void *input, void *output, const void *key_ptr)
{
    const u8 *key = (const u8 *)key_ptr;
    u32 cqw = rd_be32(key + 0);
    u32 exp = rd_be32(key + 4);
    u32 n = cqw * 2u; // number of 32-bit limbs
    if (cqw == 0 || cqw > 64 || exp == 0)
        return 0;

    u32 mod[128], base_val[128];
    for (u32 i = 0; i < cqw; i++) {
        u64 mq = rd_be64(key + 16 + i * 8);
        u64 bq = rd_be64((const u8 *)input + i * 8);
        mod[i * 2 + 0] = (u32)mq;
        mod[i * 2 + 1] = (u32)(mq >> 32);
        base_val[i * 2 + 0] = (u32)bq;
        base_val[i * 2 + 1] = (u32)(bq >> 32);
    }
    if ((mod[0] & 1u) == 0 || ge_limbs32(base_val, mod, n))
        return 0;

    u32 r2[128];
    __builtin_memset(r2, 0, sizeof(r2));
    sub_limbs32(r2, mod, n); // R mod N
    for (u32 bit = 0; bit < n * 32u; bit++) {
        u32 carry = 0;
        for (u32 i = 0; i < n; i++) {
            u32 next = r2[i] >> 31;
            r2[i] = (r2[i] << 1) | carry;
            carry = next;
        }
        if (carry || ge_limbs32(r2, mod, n))
            sub_limbs32(r2, mod, n);
    }

    u32 k0 = inv32_neg(mod[0]);
    u32 one[128], acc[128], cur[128];
    __builtin_memset(one, 0, sizeof(one));
    one[0] = 1u;

    mont_mul32(acc, one, r2, mod, k0, n);
    mont_mul32(cur, base_val, r2, mod, k0, n);

    int top = 31;
    while (top > 0 && ((exp >> top) & 1u) == 0)
        top--;
    for (int b = top; b >= 0; b--) {
        mont_mul32(acc, acc, acc, mod, k0, n);
        if ((exp >> b) & 1u)
            mont_mul32(acc, acc, cur, mod, k0, n);
    }
    mont_mul32(acc, acc, one, mod, k0, n);

    u8 *out_bytes = (u8 *)output;
    for (u32 i = 0; i < cqw; i++) {
        u64 out_q = ((u64)acc[i * 2 + 1] << 32) | acc[i * 2 + 0];
        wr_be64(out_bytes + i * 8, out_q);
    }
    return 1;
}

static int pkcs1_verify(const void *hash, const void *signature, u32 size)
{
    // Standard SHA-1 DigestInfo ASN.1 DER prefix (15 bytes) + 20-byte digest = 35 bytes.
    static const u8 k_oid_sha1[15] = {
        0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14
    };
    if (size < 46u || hash == 0 || signature == 0)
        return 0;
    const u8 *sig = (const u8 *)signature;
    const u8 *dig = (const u8 *)hash;

    // Big-endian PKCS#1 v1.5 layout: 00 01 FF* 00 <oid[15]> <hash[20]>.
    if (sig[0] == 0x00 && sig[1] == 0x01) {
        u32 pad_end = size - 36u;
        for (u32 i = 2; i < pad_end; i++) {
            if (sig[i] != 0xff)
                return 0;
        }
        if (sig[pad_end] != 0x00)
            return 0;
        for (u32 i = 0; i < 15u; i++) {
            if (sig[pad_end + 1u + i] != k_oid_sha1[i])
                return 0;
        }
        for (u32 i = 0; i < 20u; i++) {
            if (sig[pad_end + 16u + i] != dig[i])
                return 0;
        }
        return 1;
    }

    // Reversed-byte (little-endian) layout in case caller passed DwLe directly without post-swap:
    if (sig[size - 1u] == 0x00 && sig[size - 2u] == 0x01) {
        for (u32 i = 0; i < 20u; i++) {
            if (sig[19u - i] != dig[i])
                return 0;
        }
        for (u32 i = 0; i < 15u; i++) {
            if (sig[34u - i] != k_oid_sha1[i])
                return 0;
        }
        if (sig[35u] != 0x00)
            return 0;
        for (u32 i = 36u; i < size - 2u; i++) {
            if (sig[i] != 0xff)
                return 0;
        }
        return 1;
    }
    return 0;
}

int PORT_MS_ABI XeCryptBnDwLePkcs1Verify(const void *hash, const void *signature, u32 size)
{
    // a verified map's tag section is converted to host order here (map_swap.h)
    return port_map_swap_after_verify(pkcs1_verify(hash, signature, size));
}

// ---------------------------------------------------------------------------------------------------------------
// AES-128 CBC (XeCryptAesKey, XeCryptAesCbc), random bytes (XeCryptRandom) and PKCS #1 signature blocks
// (XeCryptBnDwLePkcs1Format): the kernel exports data_compress.cpp and online_files.cpp use for the encrypted
// film / screenshot uploads and their RSA-wrapped keys.

// GF(2^8) multiplication tables for the inverse cipher, built once.
static void aes128_tables_once(void)
{
    static volatile int state; // 0 not built, 1 building, 2 built
    int expected = 0;
    if (__atomic_load_n(&state, __ATOMIC_ACQUIRE) == 2)
        return;
    if (__atomic_compare_exchange_n(&state, &expected, 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        aes128_init_tables();
        __atomic_store_n(&state, 2, __ATOMIC_RELEASE);
        return;
    }
    while (__atomic_load_n(&state, __ATOMIC_ACQUIRE) != 2)
        __builtin_ia32_pause();
}

static u8 aes_xtime(u8 a) { return (u8)((a << 1) ^ ((a & 0x80) ? 0x1b : 0)); }

static void aes128_encrypt_block(const u8 rk[176], const u8 in[16], u8 out[16])
{
    u8 s[16], t[16];
    for (int i = 0; i < 16; i++)
        s[i] = in[i] ^ rk[i];
    for (int round = 1; round <= 10; round++) {
        // SubBytes + ShiftRows (column-major state: byte r of column c at 4c + r)
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 4; r++)
                t[c * 4 + r] = k_aes_sbox[s[((c + r) % 4) * 4 + r]];
        if (round != 10) {
            // MixColumns
            for (int c = 0; c < 4; c++) {
                u8 a0 = t[c * 4], a1 = t[c * 4 + 1], a2 = t[c * 4 + 2], a3 = t[c * 4 + 3];
                u8 all = a0 ^ a1 ^ a2 ^ a3;
                s[c * 4 + 0] = a0 ^ all ^ aes_xtime(a0 ^ a1);
                s[c * 4 + 1] = a1 ^ all ^ aes_xtime(a1 ^ a2);
                s[c * 4 + 2] = a2 ^ all ^ aes_xtime(a2 ^ a3);
                s[c * 4 + 3] = a3 ^ all ^ aes_xtime(a3 ^ a0);
            }
        } else {
            __builtin_memcpy(s, t, 16);
        }
        for (int i = 0; i < 16; i++)
            s[i] ^= rk[round * 16 + i];
    }
    __builtin_memcpy(out, s, 16);
}

// XECRYPT_AES_STATE: 352 bytes, the encryption and decryption key schedules (11 round keys each). Both directions
// here run from the one forward schedule, kept in both halves.
void PORT_MS_ABI XeCryptAesKey(void *aes_state, const void *key)
{
    aes128_tables_once();
    u8 *state = (u8 *)aes_state;
    aes128_expand_key((const u8 *)key, state);
    __builtin_memcpy(state + 176, state, 176);
}

void XeCryptAesEcb(const void *aes_state, const void *input, void *output, int encrypt)
{
    aes128_tables_once();
    if (encrypt)
        aes128_encrypt_block((const u8 *)aes_state, (const u8 *)input, (u8 *)output);
    else
        aes128_decrypt_block((const u8 *)aes_state + 176, (const u8 *)input, (u8 *)output);
}

// CBC over whole 16-byte blocks (a trailing partial block is left alone, as the kernel's is); `feedback` is the IV
// on entry and the last ciphertext block on return, so a stream continues across calls. In place is allowed.
void PORT_MS_ABI XeCryptAesCbc(const void *aes_state, const void *input, u32 size, void *output, u8 *feedback, int encrypt)
{
    aes128_tables_once();
    const u8 *schedule = (const u8 *)aes_state;
    const u8 *in = (const u8 *)input;
    u8 *out = (u8 *)output;
    for (u32 offset = 0; offset + 16 <= size; offset += 16) {
        u8 block[16];
        if (encrypt) {
            for (int i = 0; i < 16; i++)
                block[i] = in[offset + i] ^ feedback[i];
            aes128_encrypt_block(schedule, block, out + offset);
            __builtin_memcpy(feedback, out + offset, 16);
        } else {
            u8 cipher[16];
            __builtin_memcpy(cipher, in + offset, 16);
            aes128_decrypt_block(schedule + 176, cipher, block);
            for (int i = 0; i < 16; i++)
                out[offset + i] = block[i] ^ feedback[i];
            __builtin_memcpy(feedback, cipher, 16);
        }
    }
}

void PORT_MS_ABI XeCryptRandom(void *buffer, u32 size)
{
    port_host_random(buffer, size);
}

// A PKCS #1 v1.5 block-type-1 signature block for a SHA-1 digest, in the kernel's DwLe form (the big-endian block
// byte-reversed): digest reversed at 0..19, the DigestInfo prefix reversed after it, 0x00, 0xff padding, then 0x01
// and 0x00 at the top. Formats: 0 DigestInfo with the NULL algorithm parameters (the usual 15-byte prefix), 1
// without them (13 bytes), 2 the bare digest. XeCryptBnDwLePkcs1Verify accepts what format 0 writes.
void PORT_MS_ABI XeCryptBnDwLePkcs1Format(const void *hash, u32 format, void *buffer, u32 size)
{
    static const u8 k_digest_info_null[15] = {
        0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14
    };
    static const u8 k_digest_info[13] = { 0x30, 0x1f, 0x30, 0x07, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x04, 0x14 };
    const u8 *prefix = format == 0 ? k_digest_info_null : format == 1 ? k_digest_info : 0;
    const u32 prefix_size = format == 0 ? 15u : format == 1 ? 13u : 0u;
    u8 *block = (u8 *)buffer;
    const u8 *digest = (const u8 *)hash;
    if (block == 0 || size < 20u + prefix_size + 3u + 8u)
        return; // too small for a block with the minimum 8 bytes of padding
    __builtin_memset(block, 0xff, size);
    for (u32 i = 0; i < 20u; i++)
        block[i] = digest[19u - i];
    for (u32 i = 0; i < prefix_size; i++)
        block[20u + i] = prefix[prefix_size - 1u - i];
    block[20u + prefix_size] = 0x00;
    block[size - 2u] = 0x01;
    block[size - 1u] = 0x00;
}
