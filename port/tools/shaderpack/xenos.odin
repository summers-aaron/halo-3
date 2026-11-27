package main

// shaderpack --xenos: the shader pack of version 3, translated ahead of time from the retail Xenos microcode itself
// (port/tools/xenos_spirv, the AOT Xenos microcode -> Vulkan 1.3 SPIR-V translator) instead of compiled from Bungie's
// DX11 HLSL with DXC (version 2, the rest of this tool). No stage.sh, HLSL tree or DXC: every unique retail vertex and
// pixel shader of the given maps (and the executable's embedded D3D shader functions with --executable) becomes one
// SPIR-V module that reads the Xbox register files directly through the universal pipeline layout of
// port/shaders/xenos_spirv_abi.h, so the pack has no gathers, cbuffer tables or sampler maps: only the microcode table
// (stage, FNV-1a 64 of the physical microcode, the key port_vk_shader_register computes) -> module + what the device
// needs to bind it (port/shaders/shader_pack.h, "version 3").
//
// File (little-endian, tables 4-byte aligned): header (s_shader_pack_file_header_v3) | module table (as version 2) |
// xenos table (sorted by hash_hi, hash_lo, stage) | texture uses | vertex elements | literal constants | memexport
// stream registers | rectangle-list geometry shaders (vertex shader output mask, module) | ABI 6 module metadata,
// entry overlays and descriptor keys (ABI 6 only) | string table | blob (one zlib stream per unique module).

import "core:fmt"
import "core:hash"
import "core:os"
import "core:slice"
import "core:strings"
import "core:sync"
import "core:thread"
import "core:time"
import "vendor:zlib"
import xenos "../xenos_spirv"

XENOS_PACK_VERSION :: 3
XENOS_HEADER_SIZE :: 104
XENOS_HEADER_SIZE_ABI6 :: 128

// s_shader_pack_file_xenos flags
XENOS_FLAG_KILLS :: 1
XENOS_FLAG_MEMEXPORT :: 2
XENOS_FLAG_DEPTH :: 4
XENOS_FLAG_LABELS :: 8
XENOS_FLAG_POINT_SIZE :: 16
XENOS_FLAG_BDA :: 32

Xenos_Result :: struct {
	words:  []u32,
	meta:   xenos.Meta,
	error:  string,
	module: int,
}

Xenos_Pool :: struct {
	set:      ^Microcode_Set,
	results:  []Xenos_Result,
	next:     int,
	done:     int,
	last_pct: int,
}

xenos_worker :: proc(pool: ^Xenos_Pool) {
	total := len(pool.results)
	for {
		i := sync.atomic_add(&pool.next, 1)
		if i >= total {return}
		s := &pool.set.shaders[i]
		r := xenos.translate_shader(s.object, s.ucode, s.stage == .PS ? .Pixel : .Vertex, abi = opts.xenos_abi)
		pool.results[i] = Xenos_Result{words = r.words, meta = r.meta, error = r.error}
		if opts.progress && total > 0 {
			done := sync.atomic_add(&pool.done, 1) + 1
			pct := 15 + (done * 45) / total
			prev := sync.atomic_load(&pool.last_pct)
			if pct > prev {
				if _, ok := sync.atomic_compare_exchange_strong(&pool.last_pct, prev, pct); ok {
					fmt.printfln("PROGRESS %d Translating Vulkan shaders (%d / %d)...", pct, done, total)
					os.flush(os.stdout)
				}
			}
		}
		free_all(context.temp_allocator)
	}
}

union_module_reads :: proc(a: ^xenos.Module_Meta, b: xenos.Module_Meta) {
	for mask, i in b.direct_float_mask {a.direct_float_mask[i] |= mask}
	if b.relative_hi_exclusive != 0 {
		a.relative_lo = a.relative_hi_exclusive == 0 ? b.relative_lo : min(a.relative_lo, b.relative_lo)
		a.relative_hi_exclusive = max(a.relative_hi_exclusive, b.relative_hi_exclusive)
	}
	for mask, i in b.bool_mask {a.bool_mask[i] |= mask}
	a.loop_mask |= b.loop_mask
	a.system_rows |= b.system_rows
	for mask, i in b.fetch_rows {a.fetch_rows[i] |= mask}
	for mask, i in b.vertex_fetch_rows {a.vertex_fetch_rows[i] |= mask}
}

put_module_meta :: proc(table: ^[dynamic]u8, meta: xenos.Module_Meta) {
	for mask in meta.direct_float_mask {put32(table, mask)}
	put16(table, meta.relative_lo)
	put16(table, meta.relative_hi_exclusive)
	for mask in meta.bool_mask {put32(table, mask)}
	put32(table, meta.loop_mask)
	put32(table, meta.system_rows)
	for mask in meta.fetch_rows {put32(table, mask)}
	for mask in meta.vertex_fetch_rows {put32(table, mask)}
	put32(table, meta.first_image_key)
	put32(table, meta.first_sampler_key)
	put16(table, meta.image_count)
	put16(table, meta.sampler_count)
	put32(table, meta.reserved_zero)
}

// Translates every shader of the set and writes the version 3 pack to opts.out (+ .tsv manifest).
xenos_build :: proc(mc: ^Microcode_Set, maps: int) {
	n := len(mc.shaders)
	if n == 0 {fail("no retail shaders in the given maps")}
	abi6 := opts.xenos_abi == xenos.ABI_VERSION_BINDLESS
	t0 := time.tick_now()
	pool := Xenos_Pool{set = mc, results = make([]Xenos_Result, n), last_pct = 14}
	threads: [dynamic]^thread.Thread
	for _ in 0 ..< max(opts.jobs, 1) {append(&threads, thread.create_and_start_with_poly_data(&pool, xenos_worker))}
	for th in threads {thread.join(th); thread.destroy(th)}
	translate_time := time.tick_since(t0)
	failed := 0
	by_model: [6]int
	for &r, i in pool.results {
		if r.error != "" {
			failed += 1
			s := &mc.shaders[i]
			fmt.eprintfln("FAIL %016x %s %s: %s", s.hash, stage_name[s.stage], len(s.sources) > 0 ? s.sources[0].name : "?", r.error)
			continue
		}
		by_model[r.meta.model] += 1
	}
	if failed > 0 {fail("%d of %d shaders did not translate; no pack written", failed, n)}

	// modules: the translated shaders (deduplicated by content), then the tessellation helpers
	Module :: struct {
		code:       []u8,
		hash:       u64,
		compressed: []u8,
		meta:       xenos.Module_Meta,
		image_keys: []u32,
		sampler_keys: []u32,
	}
	modules: [dynamic]Module
	by_hash: map[u64][dynamic]int
	add_module :: proc(modules: ^[dynamic]Module, by_hash: ^map[u64][dynamic]int, words: []u32,
	                   meta: xenos.Module_Meta = {}, image_keys: []u32 = nil, sampler_keys: []u32 = nil) -> int {
		code := slice.to_bytes(words)
		h := hash.fnv64a(code)
		if list, ok := by_hash[h]; ok {
			for mi in list {
				m := &modules[mi]
				if slice.equal(m.code, code) && slice.equal(m.image_keys, image_keys) && slice.equal(m.sampler_keys, sampler_keys) {
					union_module_reads(&m.meta, meta)
					return mi
				}
			}
		}
		mi := len(modules)
		append(modules, Module{code = code, hash = h, meta = meta, image_keys = image_keys, sampler_keys = sampler_keys})
		if !(h in by_hash) {by_hash[h] = {}}
		append(&by_hash[h], mi)
		return mi
	}
	raw_total := 0
	for &r in pool.results {
		r.module = add_module(&modules, &by_hash, r.words, r.meta.module_meta, r.meta.image_keys[:], r.meta.sampler_keys[:])
		raw_total += len(r.words) * 4
	}
	tess_vs_meta: xenos.Module_Meta
	tess_vs_words := xenos.helper_tess_vs(meta = abi6 ? &tess_vs_meta : nil)
	tess_vs := add_module(&modules, &by_hash, tess_vs_words, tess_vs_meta)
	tess_tcs := add_module(&modules, &by_hash, xenos.helper_tess_tcs())
	// rectangle-list geometry shaders, one per distinct vertex shader output set (sorted)
	rect_masks: [dynamic]u32
	for &r in pool.results {
		if r.meta.model != .Vertex {continue}
		if !slice.contains(rect_masks[:], r.meta.vs_outputs) {append(&rect_masks, r.meta.vs_outputs)}
	}
	slice.sort(rect_masks[:])
	rect_table: [dynamic]u8
	for mask in rect_masks {
		put32(&rect_table, mask)
		put32(&rect_table, u32(add_module(&modules, &by_hash, xenos.helper_rect_gs(mask))))
	}

	// optimize unique SPIR-V modules offline with spirv-opt --target-env=vulkan1.3 -O (preserving SpecId 0 and module indices)
	did_opt := false
	if !opts.xenos_no_opt {
		spirv_opt := ""
		when ODIN_OS == .Windows {
			opt_exe := "spirv-opt.exe"
		} else {
			opt_exe := "spirv-opt"
		}
		// Prefer spirv-opt bundled next to shaderpack, else on PATH; skip gracefully on end-user PCs without spirv-tools.
		if len(os.args) > 0 {
			exe := os.args[0]
			if slash := strings.last_index_any(exe, "/\\"); slash >= 0 {
				cand := strings.concatenate({exe[:slash + 1], opt_exe}, context.temp_allocator)
				if os.exists(cand) {spirv_opt = strings.clone(cand)}
			}
		}
		if spirv_opt == "" {
			probe_cmd := [?]string{opt_exe, "--version"}
			exited, code, serr := dxil_spawn(probe_cmd[:])
			if exited && code == 0 && serr == "" {spirv_opt = opt_exe}
		}
		if spirv_opt == "" {
			fmt.println("shaderpack --xenos: spirv-opt not found; skipping offline SPIR-V optimization")
		} else {
			did_opt = true
			opt_dir := "tmp/work/spv_opt"
			if slash := strings.last_index_any(opts.out, "/\\"); slash >= 0 {
				opt_dir = strings.concatenate({opts.out[:slash + 1], "spv_opt"}, context.temp_allocator)
			}
			if !os.exists(opt_dir) {_ = os.make_directory_all(opt_dir)}
			Opt_Pool :: struct {
				modules:  []Module,
				dir:      string,
				tool:     string,
				next:     int,
				done:     int,
				last_pct: int,
				failed:   int,
				pre_sz:   i64,
				post_sz:  i64,
			}
			opt_worker :: proc(pool: ^Opt_Pool) {
				total := len(pool.modules)
				for {
					i := sync.atomic_add(&pool.next, 1)
					if i >= total {return}
					m := &pool.modules[i]
					in_path := fmt.tprintf("%s/%d.in.spv", pool.dir, i)
					out_path := fmt.tprintf("%s/%d.out.spv", pool.dir, i)
					if os.write_entire_file(in_path, m.code) != nil {
						fmt.eprintfln("spirv-opt: cannot write %s", in_path)
						sync.atomic_add(&pool.failed, 1)
						free_all(context.temp_allocator)
						continue
					}
					cmd := [?]string{pool.tool, "--target-env=vulkan1.3", "-O", in_path, "-o", out_path}
					exited, code, serr := dxil_spawn(cmd[:])
					_ = os.remove(in_path)
					if !exited || code != 0 || serr != "" {
						fmt.eprintfln("spirv-opt failed on module %d (exited=%v code=%d err=%s)", i, exited, code, serr)
						_ = os.remove(out_path)
						sync.atomic_add(&pool.failed, 1)
						free_all(context.temp_allocator)
						continue
					}
					opt_data, rerr := os.read_entire_file(out_path, context.allocator)
					_ = os.remove(out_path)
					if rerr != nil || len(opt_data) == 0 {
						fmt.eprintfln("spirv-opt: cannot read %s", out_path)
						sync.atomic_add(&pool.failed, 1)
						free_all(context.temp_allocator)
						continue
					}
					sync.atomic_add(&pool.pre_sz, i64(len(m.code)))
					sync.atomic_add(&pool.post_sz, i64(len(opt_data)))
					m.code = opt_data
					m.hash = hash.fnv64a(opt_data)
					if opts.progress && total > 0 {
						done := sync.atomic_add(&pool.done, 1) + 1
						pct := 60 + (done * 20) / total
						prev := sync.atomic_load(&pool.last_pct)
						if pct > prev {
							if _, ok := sync.atomic_compare_exchange_strong(&pool.last_pct, prev, pct); ok {
								fmt.printfln("PROGRESS %d Optimizing shaders (%d / %d)...", pct, done, total)
								os.flush(os.stdout)
							}
						}
					}
					free_all(context.temp_allocator)
				}
			}
			t_opt := time.tick_now()
			opool := Opt_Pool{modules = modules[:], dir = opt_dir, tool = spirv_opt, last_pct = 59}
			othreads: [dynamic]^thread.Thread
			for _ in 0 ..< max(opts.jobs, 1) {append(&othreads, thread.create_and_start_with_poly_data(&opool, opt_worker))}
			for th in othreads {thread.join(th); thread.destroy(th)}
			_ = os.remove(opt_dir)
			opt_time := time.tick_since(t_opt)
			if opool.failed > 0 {fail("spirv-opt failed on %d of %d modules", opool.failed, len(modules))}
			pct := opool.pre_sz > 0 ? 100.0 * f64(opool.pre_sz - opool.post_sz) / f64(opool.pre_sz) : 0.0
			fmt.printfln("shaderpack --xenos: spirv-opt -O: %d modules, %.2f MB -> %.2f MB (%.1f%% smaller) in %v on %d threads", len(modules), f64(opool.pre_sz) / 1e6, f64(opool.post_sz) / 1e6, pct, opt_time, max(opts.jobs, 1))
		}
	}

	// compress (zlib level 9, as version 2), the modules shared out among the threads
	Compress_Pool :: struct {
		modules:  []Module,
		next:     int,
		done:     int,
		base_pct: int,
		span_pct: int,
		last_pct: int,
	}
	compress_worker :: proc(pool: ^Compress_Pool) {
		total := len(pool.modules)
		for {
			i := sync.atomic_add(&pool.next, 1)
			if i >= total {return}
			m := &pool.modules[i]
			bound := zlib.compressBound(zlib.uLong(len(m.code)))
			out := make([]u8, int(bound))
			out_len := zlib.uLongf(bound)
			if zlib.compress2(raw_data(out), &out_len, raw_data(m.code), zlib.uLong(len(m.code)), 9) != zlib.OK {fail("zlib compress2 failed")}
			m.compressed = out[:int(out_len)]
			if opts.progress && total > 0 {
				done := sync.atomic_add(&pool.done, 1) + 1
				pct := pool.base_pct + (done * pool.span_pct) / total
				prev := sync.atomic_load(&pool.last_pct)
				if pct > prev {
					if _, ok := sync.atomic_compare_exchange_strong(&pool.last_pct, prev, pct); ok {
						fmt.printfln("PROGRESS %d Compressing shaders (%d / %d)...", pct, done, total)
						os.flush(os.stdout)
					}
				}
			}
		}
	}
	t1 := time.tick_now()
	cbase := did_opt ? 80 : 60
	cpool := Compress_Pool{modules = modules[:], base_pct = cbase, span_pct = 98 - cbase, last_pct = cbase - 1}
	cthreads: [dynamic]^thread.Thread
	for _ in 0 ..< max(opts.jobs, 1) {append(&cthreads, thread.create_and_start_with_poly_data(&cpool, compress_worker))}
	for th in cthreads {thread.join(th); thread.destroy(th)}
	compress_time := time.tick_since(t1)

	// the xenos table, by (hash_hi, hash_lo, stage)
	order := make([]int, n)
	for i in 0 ..< n {order[i] = i}
	context.user_ptr = mc
	slice.sort_by(order, proc(a, b: int) -> bool {
		set := (^Microcode_Set)(context.user_ptr)
		x, y := &set.shaders[a], &set.shaders[b]
		if x.hash >> 32 != y.hash >> 32 {return x.hash >> 32 < y.hash >> 32}
		if u32(x.hash) != u32(y.hash) {return u32(x.hash) < u32(y.hash)}
		return x.stage < y.stage
	})
	st: Strings
	intern(&st, "")
	xtable, textures, elements, defs, streams, overlays: [dynamic]u8
	ntex, nel, ndef, nstream: u32
	for i in order {
		s := &mc.shaders[i]
		r := &pool.results[i]
		m := &r.meta
		flags: u32
		if m.kills {flags |= XENOS_FLAG_KILLS}
		if m.memexport {flags |= XENOS_FLAG_MEMEXPORT}
		if m.depth_output {flags |= XENOS_FLAG_DEPTH}
		if m.labels {flags |= XENOS_FLAG_LABELS}
		if m.point_size {flags |= XENOS_FLAG_POINT_SIZE}
		if m.uses_bda {flags |= XENOS_FLAG_BDA}
		put32(&xtable, u32(s.hash)); put32(&xtable, u32(s.hash >> 32))
		put32(&xtable, u32(r.module))
		append(&xtable, u8(s.stage), u8(m.model), m.param_gen < 0 ? 0xff : u8(m.param_gen), u8(m.color_outputs))
		put32(&xtable, flags)
		put32(&xtable, m.vs_outputs)
		put32(&xtable, m.ps_inputs)
		put32(&xtable, u32(s.size))
		put32(&xtable, intern(&st, len(s.sources) > 0 ? s.sources[0].name : ""))
		put32(&xtable, ntex)
		put16(&xtable, u16(len(m.textures))); put16(&xtable, u16(len(m.elements)))
		put32(&xtable, nel)
		put32(&xtable, ndef)
		put16(&xtable, u16(len(m.defs))); put16(&xtable, u16(len(m.memexport_streams)))
		put32(&xtable, nstream)
		for u in m.textures {append(&textures, u8(u.fetch), u8(u.dim), u8(u.sampler), 0); ntex += 1}
		for e in m.elements {append(&elements, u8(e.usage), u8(e.index), u8(e.group), 0); nel += 1}
		for d in m.defs {
			put32(&defs, d.reg)
			for v in d.value {put32(&defs, v)}
			ndef += 1
		}
		if abi6 {
			for mask in m.overlay.literal_row_mask {put32(&overlays, mask)}
		}
		for reg in m.memexport_streams {put32(&streams, reg); nstream += 1}
	}
	align4(&st.data)
	module_table, blob: [dynamic]u8
	blob_off: u32 = 0
	unique_raw := 0
	for &m in modules {
		put32(&module_table, blob_off)
		put32(&module_table, u32(len(m.compressed)))
		put32(&module_table, u32(len(m.code)))
		put32(&module_table, hash.crc32(m.code))
		put32(&module_table, u32(m.hash))
		put32(&module_table, u32(m.hash >> 32))
		append(&blob, ..m.compressed)
		blob_off += u32(len(m.compressed))
		unique_raw += len(m.code)
	}
	header_size := abi6 ? XENOS_HEADER_SIZE_ABI6 : XENOS_HEADER_SIZE
	file: [dynamic]u8
	resize(&file, header_size)
	module_offset := u32(len(file)); append(&file, ..module_table[:])
	xenos_offset := u32(len(file)); append(&file, ..xtable[:])
	texture_offset := u32(len(file)); append(&file, ..textures[:])
	element_offset := u32(len(file)); append(&file, ..elements[:])
	def_offset := u32(len(file)); append(&file, ..defs[:])
	stream_offset := u32(len(file)); append(&file, ..streams[:])
	rect_offset := u32(len(file)); append(&file, ..rect_table[:])
	abi6_header: [6]u32
	if abi6 {
		module_meta_offset := u32(len(file))
		image_keys, sampler_keys: [dynamic]u32
		for &m in modules {
			m.meta.first_image_key = u32(len(image_keys))
			m.meta.first_sampler_key = u32(len(sampler_keys))
			m.meta.image_count = u16(len(m.image_keys))
			m.meta.sampler_count = u16(len(m.sampler_keys))
			put_module_meta(&file, m.meta)
			if len(m.image_keys) > 0 {append(&image_keys, ..m.image_keys)}
			if len(m.sampler_keys) > 0 {append(&sampler_keys, ..m.sampler_keys)}
		}
		entry_overlay_offset := u32(len(file))
		append(&file, ..overlays[:])
		image_key_offset := u32(len(file))
		for key in image_keys {put32(&file, key)}
		sampler_key_offset := u32(len(file))
		for key in sampler_keys {put32(&file, key)}
		abi6_header = {module_meta_offset, entry_overlay_offset, u32(len(image_keys)), image_key_offset, u32(len(sampler_keys)), sampler_key_offset}
	}
	string_offset := u32(len(file)); append(&file, ..st.data[:])
	align4(&file)
	blob_offset := u32(len(file)); append(&file, ..blob[:])
	header: [dynamic]u8
	for v in ([?]u32{0x50533348, XENOS_PACK_VERSION, u32(header_size), u32(len(file)), opts.xenos_abi, u32(len(modules)), module_offset, u32(n), xenos_offset, ntex, texture_offset, nel, element_offset, ndef, def_offset, nstream, stream_offset, u32(len(st.data)), string_offset, u32(len(blob)), blob_offset}) {put32(&header, v)}
	put32(&header, hash.crc32(file[header_size:blob_offset]))
	put32(&header, u32(tess_vs)); put32(&header, u32(tess_tcs))
	put32(&header, u32(len(rect_masks))); put32(&header, rect_offset)
	if abi6 {for v in abi6_header {put32(&header, v)}}
	if len(header) != header_size {fail("internal: version 3 header is %d bytes", len(header))}
	copy(file[:header_size], header[:])
	if os.write_entire_file(opts.out, file[:]) != nil {fail("cannot write %s", opts.out)}
	if opts.progress {
		fmt.println("PROGRESS 100 Shader pack ready")
		os.flush(os.stdout)
	}
	if opts.xenos_modules != "" {
		os.make_directory(opts.xenos_modules)
		for &m, mi in modules {
			path := fmt.tprintf("%s/%d.spv", opts.xenos_modules, mi)
			if os.write_entire_file(path, m.code) != nil {fail("cannot write %s", path)}
		}
	}

	// readable manifest
	mb := strings.builder_make()
	fmt.sbprintln(&mb, "# microcode_fnv64\tstage\tmodel\tmodule\tflags\tparam_gen\tcolor_out\tvs_out\tps_in\ttextures\telements\tdefs\tmemexport_streams\tsource")
	models := [6]string{"vertex", "", "tess_eval", "", "fragment", "compute"}
	dim_names := [4]string{"2d", "3d", "cube", "stacked"}
	for i in order {
		s := &mc.shaders[i]
		r := &pool.results[i]
		m := &r.meta
		fmt.sbprintf(&mb, "%016x\t%s\t%s\t%d\t", s.hash, stage_name[s.stage], models[m.model], r.module)
		fmt.sbprintf(&mb, "%s%s%s%s%s\t%d\t%x\t%x\t%x\t", m.kills ? "k" : "", m.memexport ? "m" : "", m.depth_output ? "d" : "", m.labels ? "l" : "", m.point_size ? "p" : "", m.param_gen, m.color_outputs, m.vs_outputs, m.ps_inputs)
		for u, k in m.textures {fmt.sbprintf(&mb, "%stf%d:%s/%d", k > 0 ? "," : "", u.fetch, dim_names[u.dim & 3], u.sampler)}
		fmt.sbprint(&mb, "\t")
		for e, k in m.elements {fmt.sbprintf(&mb, "%s%v%d", k > 0 ? "," : "", e.usage, e.index)}
		fmt.sbprintf(&mb, "\t%d\t", len(m.defs))
		for reg, k in m.memexport_streams {fmt.sbprintf(&mb, "%sc%d", k > 0 ? "," : "", reg)}
		fmt.sbprintfln(&mb, "\t%s", len(s.sources) > 0 ? s.sources[0].name : "")
	}
	write_file(strings.concatenate({opts.out, ".tsv"}), strings.to_string(mb))

	stages: [2]int
	for &s in mc.shaders {stages[min(int(s.stage), 1)] += 1}
	fmt.printfln("shaderpack --xenos: %d maps: %d unique retail shaders (vs %d, ps %d) translated in %v on %d threads, 0 failed", maps, n, stages[0], stages[1], translate_time, max(opts.jobs, 1))
	fmt.printfln("shaderpack --xenos: execution models: vertex %d, tessellation evaluation %d, fragment %d, compute %d; + 2 tessellation helpers, %d rectangle-list geometry shaders", by_model[0], by_model[2], by_model[4], by_model[5], len(rect_masks))
	fmt.printfln("shaderpack --xenos: %s: %d modules, SPIR-V %.1f MB (%.1f MB unique), %.2f MB compressed in %v; pack %.2f MB (ABI version %d)", opts.out, len(modules), f64(raw_total) / 1e6, f64(unique_raw) / 1e6, f64(len(blob)) / 1e6, compress_time, f64(len(file)) / 1e6, opts.xenos_abi)
}
