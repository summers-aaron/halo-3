// port/tools/xiso: pure-Odin Xbox 360 ISO (XDVDFS) extractor.
// Supports raw XISO (offset 0), XGD2 / Redump retail Halo 3 ISOs (offset 0x0FD90000),
// XGD3 (offset 0x02080000), and OG Xbox (offset 0x18300000).
//
// Usage:
//   xiso extract [--progress] <halo3.iso> <out_dir>
//   xiso list                 <halo3.iso>
package main

import "core:fmt"
import "core:os"
import "core:path/filepath"
import "core:strings"

SECTOR_SIZE :: 2048
MAGIC :: "MICROSOFT*XBOX*MEDIA"
ATTR_DIR :: 0x10
COPY_BUF_SIZE :: 4 * 1024 * 1024

KNOWN_OFFSETS := [?]i64{
	0x00000000, // Raw XISO
	0x0FD90000, // XGD2 (Halo 3 retail / Redump)
	0x02080000, // XGD3
	0x18300000, // OG Xbox Redump
}

le16 :: proc(b: []u8, o: int) -> u16 {
	return u16(b[o]) | (u16(b[o + 1]) << 8)
}

le32 :: proc(b: []u8, o: int) -> u32 {
	return u32(b[o]) | (u32(b[o + 1]) << 8) | (u32(b[o + 2]) << 16) | (u32(b[o + 3]) << 24)
}

read_exact_at :: proc(fd: ^os.File, offset: i64, buf: []u8) -> bool {
	total := 0
	for total < len(buf) {
		n, err := os.read_at(fd, buf[total:], offset + i64(total))
		if err != nil || n <= 0 {
			return false
		}
		total += n
	}
	return true
}

find_partition :: proc(fd: ^os.File) -> (part_off: i64, root_sector: u32, root_size: u32, ok: bool) {
	hdr: [SECTOR_SIZE]u8
	for off in KNOWN_OFFSETS {
		if !read_exact_at(fd, off + 32 * SECTOR_SIZE, hdr[:]) {
			continue
		}
		if string(hdr[0:len(MAGIC)]) == MAGIC && string(hdr[0x7EC:0x7EC + len(MAGIC)]) == MAGIC {
			return off, le32(hdr[:], 0x14), le32(hdr[:], 0x18), true
		}
	}
	return 0, 0, 0, false
}

Context :: struct {
	fd:           ^os.File,
	part_off:     i64,
	out_dir:      string,
	list_only:    bool,
	count_only:   bool,
	progress:     bool,
	copy_buf:     []u8,
	files_done:   int,
	bytes_done:   i64,
	total_bytes:  i64,
	last_pct:     int,
	last_emit_mb: i64,
}

emit_progress :: proc(ctx: ^Context, rel_path: string, force := false) {
	if !ctx.progress || ctx.total_bytes <= 0 {
		return
	}
	pct := int((ctx.bytes_done * 100) / ctx.total_bytes)
	if pct > 99 && !force {
		pct = 99
	}
	done_mb := ctx.bytes_done / (1024 * 1024)
	total_mb := ctx.total_bytes / (1024 * 1024)
	if force || pct > ctx.last_pct || done_mb >= ctx.last_emit_mb + 24 {
		ctx.last_pct = pct
		ctx.last_emit_mb = done_mb
		fmt.printfln("PROGRESS %d Extracting %s (%d / %d MB)", pct, rel_path, done_mb, total_mb)
		os.flush(os.stdout)
	}
}

extract_file :: proc(ctx: ^Context, rel_path: string, sector: u32, size: u32) {
	if ctx.count_only {
		ctx.total_bytes += i64(size)
		return
	}
	if ctx.list_only {
		fmt.printfln("%10d  %s", size, rel_path)
		ctx.files_done += 1
		ctx.bytes_done += i64(size)
		return
	}

	dst_path := strings.concatenate({ctx.out_dir, "/", rel_path}, context.temp_allocator)
	parent := filepath.dir(dst_path)
	os.make_directory_all(parent)

	out_f, err := os.open(dst_path, os.O_WRONLY | os.O_CREATE | os.O_TRUNC, os.Permissions_Read_Write_All)
	if err != nil {
		fmt.eprintfln("xiso: cannot create %s", dst_path)
		os.exit(1)
	}
	defer os.close(out_f)

	src_off := ctx.part_off + i64(sector) * SECTOR_SIZE
	rem := i64(size)
	for rem > 0 {
		chunk := int(min(rem, i64(len(ctx.copy_buf))))
		if !read_exact_at(ctx.fd, src_off, ctx.copy_buf[:chunk]) {
			fmt.eprintfln("xiso: truncated read for %s at offset 0x%x", rel_path, src_off)
			os.exit(1)
		}
		written := 0
		for written < chunk {
			n, werr := os.write(out_f, ctx.copy_buf[written:chunk])
			if werr != nil || n <= 0 {
				fmt.eprintfln("xiso: write error for %s", dst_path)
				os.exit(1)
			}
			written += n
		}
		src_off += i64(chunk)
		rem -= i64(chunk)
		ctx.bytes_done += i64(chunk)
		emit_progress(ctx, rel_path)
	}

	ctx.files_done += 1
	if !ctx.progress && size >= 10 * 1024 * 1024 {
		fmt.printfln("  extracted %s (%.1f MiB)", rel_path, f64(size) / (1024.0 * 1024.0))
	}
}

walk_dir_table :: proc(ctx: ^Context, prefix: string, table: []u8, offset: int) {
	if offset < 0 || offset + 14 > len(table) {
		return
	}
	if table[offset] == 0xFF && table[offset + 1] == 0xFF {
		return
	}

	left := int(le16(table, offset + 0)) * 4
	right := int(le16(table, offset + 2)) * 4
	sector := le32(table, offset + 4)
	size := le32(table, offset + 8)
	attr := table[offset + 12]
	name_len := int(table[offset + 13])

	if left > 0 && left < len(table) && left != offset {
		walk_dir_table(ctx, prefix, table, left)
	}

	if name_len > 0 && offset + 14 + name_len <= len(table) {
		name := string(table[offset + 14 : offset + 14 + name_len])
		if name != "." && name != ".." && !strings.contains_any(name, "/\\") {
			rel := name if prefix == "" else strings.concatenate({prefix, "/", name}, context.temp_allocator)
			if (attr & ATTR_DIR) != 0 {
				if size > 0 {
					walk_dir_sector(ctx, rel, sector, size)
				} else if !ctx.list_only && !ctx.count_only {
					dir_path := strings.concatenate({ctx.out_dir, "/", rel}, context.temp_allocator)
					os.make_directory_all(dir_path)
				}
			} else {
				extract_file(ctx, rel, sector, size)
			}
		}
	}

	if right > 0 && right < len(table) && right != offset {
		walk_dir_table(ctx, prefix, table, right)
	}
}

walk_dir_sector :: proc(ctx: ^Context, prefix: string, sector: u32, size: u32) {
	if size == 0 || size > 64 * 1024 * 1024 {
		return
	}
	table := make([]u8, int(size))
	defer delete(table)
	if !read_exact_at(ctx.fd, ctx.part_off + i64(sector) * SECTOR_SIZE, table) {
		fmt.eprintfln("xiso: failed reading directory %s (sector %d, size %d)", prefix, sector, size)
		os.exit(1)
	}
	walk_dir_table(ctx, prefix, table, 0)
}

main :: proc() {
	if len(os.args) < 3 {
		fmt.eprintln("usage: xiso extract [--progress] <halo3.iso> <out_dir>")
		fmt.eprintln("       xiso list                 <halo3.iso>")
		os.exit(2)
	}
	cmd := os.args[1]
	progress := false
	pos: [dynamic]string
	for a in os.args[2:] {
		if a == "--progress" {
			progress = true
		} else {
			append(&pos, a)
		}
	}

	list_only := false
	iso_path := ""
	out_dir := ""
	switch cmd {
	case "list":
		if len(pos) != 1 {
			fmt.eprintln("usage: xiso list <halo3.iso>")
			os.exit(2)
		}
		list_only = true
		iso_path = pos[0]
	case "extract":
		if len(pos) != 2 {
			fmt.eprintln("usage: xiso extract [--progress] <halo3.iso> <out_dir>")
			os.exit(2)
		}
		iso_path = pos[0]
		out_dir = pos[1]
	case:
		fmt.eprintfln("xiso: unknown command '%s' (expected 'extract' or 'list')", cmd)
		os.exit(2)
	}

	fd, err := os.open(iso_path, os.O_RDONLY)
	if err != nil {
		fmt.eprintfln("xiso: cannot open %s", iso_path)
		os.exit(1)
	}
	defer os.close(fd)

	part_off, root_sector, root_size, ok := find_partition(fd)
	if !ok {
		fmt.eprintfln("xiso: %s is not a valid Xbox 360 ISO (MICROSOFT*XBOX*MEDIA header not found)", iso_path)
		os.exit(1)
	}
	if !list_only {
		fmt.printfln("xiso: found Xbox 360 game partition at offset 0x%x, extracting to %s...", part_off, out_dir)
		os.make_directory_all(out_dir)
	}

	ctx := Context{
		fd        = fd,
		part_off  = part_off,
		out_dir   = out_dir,
		list_only = list_only,
		progress  = progress,
		copy_buf  = make([]u8, COPY_BUF_SIZE),
		last_pct  = -1,
	}
	defer delete(ctx.copy_buf)

	if progress && !list_only {
		ctx.count_only = true
		walk_dir_sector(&ctx, "", root_sector, root_size)
		ctx.count_only = false
		fmt.printfln("PROGRESS 0 Extracting Halo 3 disc (0 / %d MB)", ctx.total_bytes / (1024 * 1024))
		os.flush(os.stdout)
	}

	walk_dir_sector(&ctx, "", root_sector, root_size)
	if !list_only {
		if progress {
			fmt.printfln("PROGRESS 100 Extracted %d files (%.1f MiB)", ctx.files_done, f64(ctx.bytes_done) / (1024.0 * 1024.0))
			os.flush(os.stdout)
		}
		fmt.printfln("xiso: extracted %d files (%.1f MiB)", ctx.files_done, f64(ctx.bytes_done) / (1024.0 * 1024.0))
	}
}
