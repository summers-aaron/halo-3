package main

// png2ico: packs one or more PNG files (e.g. 16, 32, 48, 64, 128, 256 px) into a
// deterministic Windows .ico file (PNG-in-ICO format, supported on Windows Vista+/10/11,
// llvm-rc, and NSIS 3+).
//
// Usage: png2ico <out.ico> <in1.png> [in2.png ...]

import "core:encoding/endian"
import "core:fmt"
import "core:os"

PNG_SIG :: [8]u8{0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'}

Entry :: struct {
	w, h: u32,
	data: []u8,
}

fail :: proc(format: string, args: ..any) -> ! {
	fmt.eprint("png2ico: ")
	fmt.eprintfln(format, ..args)
	os.exit(1)
}

main :: proc() {
	if len(os.args) < 3 {
		fail("usage: png2ico <out.ico> <in1.png> [in2.png ...]")
	}
	out_path := os.args[1]
	in_paths := os.args[2:]
	if len(in_paths) > 64 {
		fail("too many input images (%d)", len(in_paths))
	}

	entries := make([]Entry, len(in_paths))
	total_data := 0
	for p, i in in_paths {
		data, err := os.read_entire_file(p, context.allocator)
		if err != nil {
			fail("cannot read %s", p)
		}
		if len(data) < 24 {
			fail("%s: file too small to be a PNG", p)
		}
		sig := PNG_SIG
		for k in 0 ..< 8 {
			if data[k] != sig[k] {
				fail("%s: not a valid PNG signature", p)
			}
		}
		if data[12] != 'I' || data[13] != 'H' || data[14] != 'D' || data[15] != 'R' {
			fail("%s: missing IHDR chunk", p)
		}
		w, ok_w := endian.get_u32(data[16:20], .Big)
		h, ok_h := endian.get_u32(data[20:24], .Big)
		if !ok_w || !ok_h || w == 0 || h == 0 || w > 256 || h > 256 {
			fail("%s: invalid icon dimensions %dx%d (must be 1..256)", p, w, h)
		}
		entries[i] = Entry{w = w, h = h, data = data}
		total_data += len(data)
	}

	header_size := 6 + 16 * len(entries)
	out := make([]u8, header_size + total_data)

	// ICONDIR
	endian.put_u16(out[0:2], .Little, 0)
	endian.put_u16(out[2:4], .Little, 1) // type = ICO
	endian.put_u16(out[4:6], .Little, u16(len(entries)))

	// ICONDIRENTRY array + raw PNG payloads
	offset := u32(header_size)
	for e, i in entries {
		dir := out[6 + i * 16 : 6 + (i + 1) * 16]
		dir[0] = e.w == 256 ? 0 : u8(e.w)
		dir[1] = e.h == 256 ? 0 : u8(e.h)
		dir[2] = 0 // color count (0 for >= 8bpp)
		dir[3] = 0 // reserved
		endian.put_u16(dir[4:6], .Little, 1)   // color planes
		endian.put_u16(dir[6:8], .Little, 32)  // bits per pixel
		endian.put_u32(dir[8:12], .Little, u32(len(e.data)))
		endian.put_u32(dir[12:16], .Little, offset)
		copy(out[offset : offset + u32(len(e.data))], e.data)
		offset += u32(len(e.data))
	}

	if os.write_entire_file(out_path, out) != nil {
		fail("cannot write %s", out_path)
	}
}
