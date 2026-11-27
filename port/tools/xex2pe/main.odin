// port/tools/xex2pe: decrypts and expands a retail or devkit Xbox 360 XEX2 executable (basic compression)
// into its mapped PE image (indexed by RVA from the image base).
//
// Usage: xex2pe <default.xex> <default.pe>
package main

import "core:crypto/aes"
import "core:fmt"
import "core:os"

RETAIL_KEY := [16]u8{
	0x20, 0xB1, 0x85, 0xA5, 0x9D, 0x28, 0xFD, 0xC3,
	0x40, 0x58, 0x3F, 0xBB, 0x08, 0x96, 0xBF, 0x91,
}
DEVKIT_KEY := [16]u8{}

be16 :: proc(b: []u8, o: int) -> u16 {
	return u16(b[o]) << 8 | u16(b[o + 1])
}

be32 :: proc(b: []u8, o: int) -> u32 {
	return u32(b[o]) << 24 | u32(b[o + 1]) << 16 | u32(b[o + 2]) << 8 | u32(b[o + 3])
}

aes_ecb_decrypt :: proc(key: []u8, block: []u8) -> [16]u8 {
	ctx: aes.Context_ECB
	aes.init_ecb(&ctx, key)
	out: [16]u8
	aes.decrypt_ecb(&ctx, out[:], block[:16])
	return out
}

aes_cbc_decrypt :: proc(key: []u8, data: []u8) -> []u8 {
	ctx: aes.Context_ECB
	aes.init_ecb(&ctx, key)
	n := (len(data) / 16) * 16
	out := make([]u8, n)
	prev: [16]u8
	block: [16]u8
	for off := 0; off < n; off += 16 {
		chunk := data[off : off + 16]
		aes.decrypt_ecb(&ctx, block[:], chunk)
		for i in 0 ..< 16 {
			out[off + i] = block[i] ~ prev[i]
		}
		copy(prev[:], chunk)
	}
	return out
}

xex_to_image :: proc(xex: []u8) -> ([]u8, string) {
	if len(xex) < 0x18 || string(xex[:4]) != "XEX2" {
		return nil, "not an XEX2 file"
	}
	pe_offset := int(be32(xex, 8))
	security := int(be32(xex, 0x10))
	count := int(be32(xex, 0x14))
	if pe_offset <= 0 || pe_offset >= len(xex) || security + 0x160 > len(xex) {
		return nil, "truncated XEX2 header"
	}

	format_off := -1
	for i in 0 ..< count {
		k := be32(xex, 0x18 + 8 * i)
		v := be32(xex, 0x1c + 8 * i)
		if k == 0x3ff {
			format_off = int(v)
			break
		}
	}
	if format_off < 0 || format_off + 8 > len(xex) {
		return nil, "missing XEX2 file format header"
	}

	fmt_size := int(be32(xex, format_off))
	encryption := be16(xex, format_off + 4)
	compression := be16(xex, format_off + 6)
	if compression != 1 {
		return nil, fmt.tprintf("compression type %d not supported (expected basic compression = 1)", compression)
	}
	if format_off + fmt_size > len(xex) {
		return nil, "truncated XEX2 compression table"
	}

	raw := xex[pe_offset:]
	raw = raw[:(len(raw) / 16) * 16]
	file_key := xex[security + 0x150 : security + 0x160]

	keys: [dynamic][]u8
	defer delete(keys)
	if encryption == 1 {
		append(&keys, RETAIL_KEY[:])
		append(&keys, DEVKIT_KEY[:])
	} else {
		append(&keys, nil)
	}

	for key in keys {
		payload: []u8
		owned_payload := false
		if key != nil {
			session_key := aes_ecb_decrypt(key, file_key)
			payload = aes_cbc_decrypt(session_key[:], raw)
			owned_payload = true
		} else {
			payload = raw
		}

		blocks := (fmt_size - 8) / 8
		total_size := 0
		for i in 0 ..< blocks {
			data_sz := int(be32(xex, format_off + 8 + 8 * i))
			zero_sz := int(be32(xex, format_off + 12 + 8 * i))
			total_size += data_sz + zero_sz
		}

		image := make([]u8, total_size)
		src_cur := 0
		dst_cur := 0
		valid := true
		for i in 0 ..< blocks {
			data_sz := int(be32(xex, format_off + 8 + 8 * i))
			zero_sz := int(be32(xex, format_off + 12 + 8 * i))
			if src_cur + data_sz > len(payload) {
				valid = false
				break
			}
			copy(image[dst_cur : dst_cur + data_sz], payload[src_cur : src_cur + data_sz])
			src_cur += data_sz
			dst_cur += data_sz + zero_sz
		}
		if owned_payload {
			delete(payload)
		}
		if valid && len(image) >= 2 && image[0] == 'M' && image[1] == 'Z' {
			return image, ""
		}
		delete(image)
	}

	return nil, "decryption did not produce a valid MZ/PE image"
}

main :: proc() {
	if len(os.args) != 3 {
		fmt.eprintln("usage: xex2pe <in.xex> <out.pe>")
		os.exit(2)
	}
	xex, err := os.read_entire_file(os.args[1], context.allocator)
	if err != nil {
		fmt.eprintfln("xex2pe: cannot read %s", os.args[1])
		os.exit(1)
	}
	image, xerr := xex_to_image(xex)
	if xerr != "" {
		fmt.eprintfln("xex2pe: %s: %s", os.args[1], xerr)
		os.exit(1)
	}
	if werr := os.write_entire_file(os.args[2], image); werr != nil {
		fmt.eprintfln("xex2pe: cannot write %s", os.args[2])
		os.exit(1)
	}
	fmt.printfln("xex2pe: extracted %s (%d bytes)", os.args[2], len(image))
}
