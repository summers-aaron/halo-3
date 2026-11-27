// port/tools/shaderpack: builds the PC shader pack (SPIR-V + reflection) for the retail explicit shaders and the
// render-method template permutations of one or more retail Halo 3 .map files.
//
//   shaderpack [--maps mainmenu,010_jungle | --all] [options]
//     --maps-dir DIR   retail maps (default local/h3disc/maps)
//     --hlsl DIR       the staged, patched DX11 HLSL tree (default tmp/shaders-work/hlsl; port/tools/shaderpack/stage.sh)
//     --work DIR       per-shader work directories and the compile cache (default tmp/shaders-work/build)
//     --out FILE       the pack (default tmp/shaders-work/halo3_shaders.pack); FILE.tsv is a readable manifest
//     -j N             parallel DXC workers (default 8, at most 8)
//     --rebuild        ignore the compile cache (and the --cache-dir entries; results are still stored there)
//     --cache-dir DIR  the content-addressed compile cache (cas.odin; default tmp/shaders-work/cas, "none" turns it
//                      off): results keyed by the content of the job and of every HLSL file it includes, shared
//                      between work directories and kept when stage.sh re-stages the tree
//     --only A,B       development: build only the jobs whose tag name contains A or B (the pack is partial)
//     --list           print what would be built and exit
//     --xenos          build the version 3 pack instead (xenos.odin): every retail shader translated from its Xenos
//                      microcode to SPIR-V by port/tools/xenos_spirv, no HLSL or DXC (default --out
//                      tmp/shaders-work/halo3_shaders_xenos.pack); takes --maps / --all, --maps-dir, --executable, -j
//     --xenos-abi 5|6  select the Xenos SPIR-V ABI (default 5)
//     --xenos-modules DIR  with --xenos: also write every unique module as DIR/<module index>.spv (validation)
//     --dxil           build the version 4 pack for Direct3D 12 instead (dxil.odin): every retail shader translated by
//                      port/tools/xenos_spirv's HLSL backend (checked against its SPIR-V translation) and compiled to
//                      DXIL by DXC ($DXC, default DXC_DEFAULT; default --out tmp/shaders-work/halo3_shaders_dxil.pack);
//                      takes the --xenos options
//     --dxil-work DIR  with --dxil: sources, containers, DXC logs and the compile cache (default tmp/shaders-work/dxil)
//     --executable F   the retail executable image (PE, e.g. $HALO3_RETAIL_PE): its embedded D3D shader functions
//                      (CreateVertexShader / CreatePixelShader) get microcode entries; the HLSL files that carry
//                      //@function vs|ps <fnv64> (port/shaders/hlsl_2007) implement them
//
// Every shader is compiled the way local/shaders-pc/pgen.sh (templates) and pexp.sh (explicit shaders) do it, which
// compiled all 34,725 retail permutations to valid SPIR-V (docs/attempts 2026-10-20T1540Z):
//   1. wrapper: the render method's option defines from the rmdf tag (category/option vertex and pixel functions,
//      category_<name> indices, maybe_calc_albedo), then #include of the render method's .fx; explicit shaders
//      include their own .hlsl;
//   2. dxc -P with PARAM_ALLOC_PREPROCESS: every ___PARAM___ / ___SAMPLER___ the shader declares; a second -P without
//      it tells whether the entry function exists at all for this entry point;
//   3. pass A: all parameters in a generated MaterialParameters cbuffer (b10, 16 bytes each), all textures bound;
//   4. reflection of pass A: the MaterialParameters members the module reads and the textures it keeps;
//   5. pass B: compact MaterialParameters (used members only), used textures at compact registers (t0.. pixel,
//      t2.. vertex), unused parameters as constant zero and unused textures on dummy registers (removed by DXC);
//      then spirv-val (Vulkan 1.2 + scalar block layout).
// The pack then gets the final module with its descriptor set moved to the stage's set (pixel shaders: 1) and the
// reflection of the final module (port/shaders/shader_pack.h describes the format and the conventions).
package main

import "core:fmt"
import "core:hash"
import "core:os"
import "core:slice"
import "core:strconv"
import "core:strings"
import "core:sync"
import "core:thread"
import "core:time"
import "vendor:zlib"
import xenos "../xenos_spirv"

BUILDER_VERSION: string : "shaderpack 1"
DXC_DEFAULT :: "/home/aaron/.local/bin/dxc"

Stage :: enum u8 {
	VS = 0,
	PS = 1,
	CS = 2,
}
stage_name := [Stage]string {
	.VS = "vs",
	.PS = "ps",
	.CS = "cs",
}

// Job.kind / pack shader kind
kind_names := [?]string{"explicit", "template", "function"}

// entry point names (e_entry_point) and the HLSL function stems pgen.sh uses for them
entry_names := [?]string{"default", "albedo", "static_default", "static_per_pixel", "static_per_vertex", "static_sh", "static_prt_ambient", "static_prt_linear", "static_prt_quadratic", "dynamic_light", "shadow_generate", "shadow_apply", "active_camo", "lightmap_debug_mode", "static_per_vertex_color", "water_tessellation", "water_shading", "dynamic_light_cinematic"}
entry_ps_fn := [?]string{"default", "albedo", "static_default", "static_per_pixel", "static_per_vertex", "static_sh", "static_prt", "static_prt", "static_prt", "dynamic_light", "shadow_generate", "shadow_apply", "active_camo", "lightmap_debug_mode", "static_per_vertex_color", "water_tessellation", "water_shading", "dynamic_light_cine"}
entry_vs_fn := [?]string{"default", "albedo", "static_default", "static_per_pixel", "static_per_vertex", "static_sh", "static_prt_ambient", "static_prt_linear", "static_prt_quadratic", "dynamic_light", "shadow_generate", "shadow_apply", "active_camo", "lightmap_debug_mode", "static_per_vertex_color", "water_tessellation", "water_shading", "dynamic_light_cine"}
// vertex types (e_vertex_type) as the HLSL names them
vertex_names := [?]string{"world", "rigid", "skinned", "particle_model", "flat_world", "flat_rigid", "flat_skinned", "screen", "debug", "transparent", "particle", "contrail", "light_volume", "chud_vertex_simple", "chud_vertex_fancy", "decorator", "tiny_position", "patchy_fog", "water", "ripple", "implicit", "beam"}

vertex_struct :: proc(vt: int) -> (string, string) {
	switch vertex_names[vt] {
	case "chud_vertex_simple": return "s_chud_vertex_simple", "deform_chud_simple"
	case "chud_vertex_fancy": return "s_chud_vertex_fancy", "deform_chud_fancy"
	}
	return fmt.aprintf("s_%s_vertex", vertex_names[vt]), fmt.aprintf("deform_%s", vertex_names[vt])
}

vertex_index :: proc(name: string) -> int {
	for n, i in vertex_names {if n == name {return i}}
	return -1
}

Status :: enum u8 {
	Pending,
	Ok,
	No_Entry,
	Failed,
}

Job :: struct {
	kind:           u8, // 0 explicit, 1 template, 2 function (a reconstruction of a D3D function the executable embeds)
	stage:          Stage,
	ep:             int,
	vt:             int,
	name:           string, // tag path
	ty:             string, // render method type, "explicit" for explicit shaders
	fx:             string,
	rmdf:           ^Rmdf,
	function_hash:  u64, // kind 2: the microcode it implements (//@function)
	dir:            string,
	// an explicit shader's extra entry point (//@entry) or extra vertex type (another //@generate line, or a vertex
	// type the retail vtsh has its own microcode for): a failure is reported, not fatal
	optional:       bool,
	// a template vertex shader whose retail render method shares one vertex shader across its templates (glvs): the
	// category_<name> constants that microcode reads, left to the HLSL's CATEGORY_PARAM (job_wrapper)
	runtime_categories: map[string]bool,
	// result
	status:         Status,
	cached:         bool,
	message:        string,
	spirv:          []u8,
	refl:           Reflection,
}

Options :: struct {
	maps:      [dynamic]string,
	all:       bool,
	maps_dir:  string,
	dlc_dir:   string,
	hlsl:      string,
	work:      string,
	out:       string,
	jobs:      int,
	rebuild:   bool,
	list:      bool,
	survey:    bool,
	dump:      string,
	executable: string,
	dxc:       string,
	fingerprint: u64,
	cache_dir: string,
	only:      [dynamic]string,
	xenos:     bool, // --xenos: the version 3 pack from the retail microcode (xenos.odin)
	xenos_abi: u32, // --xenos-abi 5|6
	xenos_modules: string, // --xenos-modules DIR: also write each unique module as DIR/<index>.spv (spirv-val)
	xenos_no_opt: bool, // --no-opt: skip spirv-opt -O on --xenos modules
	dxil:      bool, // --dxil: the version 4 pack, the retail microcode translated to HLSL and compiled to DXIL (dxil.odin)
	dxil_work: string, // --dxil-work DIR
	out_given: bool,
	progress:  bool, // --progress: emit "PROGRESS <0..100> <message>" lines for GUI launchers
}

opts: Options

fail :: proc(msg: string, args: ..any) -> ! {
	fmt.eprint("shaderpack: ")
	fmt.eprintfln(msg, ..args)
	os.exit(2)
}

main :: proc() {
	opts = Options{maps_dir = "local/h3disc/maps", hlsl = "tmp/shaders-work/hlsl", work = "tmp/shaders-work/build", out = "tmp/shaders-work/halo3_shaders.pack", jobs = 8, dxc = DXC_DEFAULT, cache_dir = "tmp/shaders-work/cas", xenos_abi = xenos.ABI_VERSION}
	if dxc_env := os.get_env("DXC", context.allocator); dxc_env != "" {opts.dxc = dxc_env}
	args := os.args[1:]
	for i := 0; i < len(args); i += 1 {
		a := args[i]
		value :: proc(args: []string, i: ^int) -> string {
			if i^ + 1 >= len(args) {fail("%s needs a value", args[i^])}
			i^ += 1
			return args[i^]
		}
		switch a {
		case "--maps": for m in strings.split(value(args, &i), ",") {if m != "" {append(&opts.maps, m)}}
		case "--all": opts.all = true
		case "--maps-dir": opts.maps_dir = value(args, &i)
		case "--dlc-dir": opts.dlc_dir = value(args, &i)
		case "--hlsl": opts.hlsl = value(args, &i)
		case "--work": opts.work = value(args, &i)
		case "--out": opts.out = value(args, &i); opts.out_given = true
		case "--xenos": opts.xenos = true
		case "--xenos-abi":
			switch value(args, &i) {
			case "5": opts.xenos_abi = xenos.ABI_VERSION
			case "6": opts.xenos_abi = xenos.ABI_VERSION_BINDLESS
			case: fail("--xenos-abi must be 5 or 6")
			}
			opts.xenos = true
		case "--xenos-modules": opts.xenos_modules = value(args, &i)
		case "--no-opt": opts.xenos_no_opt = true
		case "--dxil": opts.dxil = true
		case "--dxil-work": opts.dxil_work = value(args, &i)
		case "--progress": opts.progress = true
		case "-j":
			n, ok := strconv.parse_int(value(args, &i))
			if !ok || n < 1 {fail("bad -j")}
			opts.jobs = min(n, 32)
		case "--rebuild": opts.rebuild = true
		case "--cache-dir": opts.cache_dir = value(args, &i)
		case "--only": for m in strings.split(value(args, &i), ",") {if m != "" {append(&opts.only, m)}}
		case "--list": opts.list = true
		case "--survey-microcode": opts.survey = true
		case "--dump-microcode": opts.dump = value(args, &i)
		case "--executable": opts.executable = value(args, &i)
		case:
			if strings.has_prefix(a, "-j") {
				n, ok := strconv.parse_int(a[2:])
				if !ok || n < 1 {fail("bad %s", a)}
				opts.jobs = min(n, 32)
			} else {
				fail("unknown argument %s (usage: shaderpack [--maps a,b | --all] [--maps-dir D] [--dlc-dir D] [--hlsl D] [--work D] [--out F] [-j N] [--rebuild] [--cache-dir D] [--only a,b] [--list] [--executable F] [--xenos] [--xenos-abi 5|6] [--progress])", a)
			}
		}
	}
	if opts.all == (len(opts.maps) > 0) {fail("give either --maps a,b,... or --all")}

	// maps
	paths: [dynamic]string
	if opts.all {
		entries, err := os.read_directory_by_path(opts.maps_dir, -1, context.allocator)
		if err != nil {fail("cannot list %s", opts.maps_dir)}
		for e in entries {if strings.has_suffix(e.name, ".map") {append(&paths, strings.concatenate({opts.maps_dir, "/", e.name}))}}
		if opts.dlc_dir != "" {
			if dentries, derr := os.read_directory_by_path(opts.dlc_dir, -1, context.allocator); derr == nil {
				for de in dentries {
					dp := strings.concatenate({opts.dlc_dir, "/", de.name})
					if strings.has_suffix(de.name, ".map") {
						append(&paths, dp)
					} else if sentries, serr := os.read_directory_by_path(dp, -1, context.allocator); serr == nil {
						for se in sentries {
							if strings.has_suffix(se.name, ".map") {
								append(&paths, strings.concatenate({dp, "/", se.name}))
							}
						}
					}
				}
			}
		}
		slice.sort(paths[:])
	} else {
		for m in opts.maps {
			p := m
			if !strings.contains(p, "/") {p = strings.concatenate({opts.maps_dir, "/", p})}
			if !strings.has_suffix(p, ".map") {p = strings.concatenate({p, ".map"})}
			append(&paths, p)
		}
	}
	sh: Map_Shaders
	mc: Microcode_Set
	if opts.xenos && opts.dxil {fail("give either --xenos or --dxil")}
	mc.keep_code = opts.xenos || opts.dxil
	if opts.xenos && !opts.out_given {opts.out = "tmp/shaders-work/halo3_shaders_xenos.pack"}
	if opts.dxil && !opts.out_given {opts.out = "tmp/shaders-work/halo3_shaders_dxil.pack"}
	if opts.dxil_work == "" {opts.dxil_work = "tmp/shaders-work/dxil"}
	start_maps := time.now()
	for p, pi in paths {
		if opts.progress && len(paths) > 0 {
			pct := (pi * 15) / len(paths)
			fmt.printfln("PROGRESS %d Reading maps (%d / %d)...", pct, pi + 1, len(paths))
			os.flush(os.stdout)
		}
		m, ok := map_open(p)
		if !ok {os.exit(2)}
		if !opts.xenos && !opts.dxil && !map_read_shaders(&m, &sh) {os.exit(2)}
		if !map_read_microcode(&m, &mc) {os.exit(2)}
		delete(m.b) // everything kept is cloned
	}
	if opts.xenos {
		// version 3: the retail microcode itself, translated (xenos.odin); no HLSL, DXC or render-method jobs
		if mc.errors > 0 {fmt.eprintfln("shaderpack --xenos: %d retail shader elements could not be read (left out)", mc.errors)}
		if opts.executable != "" {
			n, ok := microcode_read_executable(opts.executable, &mc)
			if !ok {os.exit(2)}
			fmt.printfln("shaderpack: %s: %d embedded shader functions", opts.executable, n)
		}
		fmt.printfln("shaderpack --xenos: read %d maps in %v", len(paths), time.since(start_maps))
		xenos_build(&mc, len(paths))
		return
	}
	if opts.dxil {
		// version 4: as version 3, through the HLSL backend and DXC (dxil.odin)
		if mc.errors > 0 {fmt.eprintfln("shaderpack --dxil: %d retail shader elements could not be read (left out)", mc.errors)}
		if opts.executable != "" {
			n, ok := microcode_read_executable(opts.executable, &mc)
			if !ok {os.exit(2)}
			fmt.printfln("shaderpack: %s: %d embedded shader functions", opts.executable, n)
		}
		fmt.printfln("shaderpack --dxil: read %d maps in %v", len(paths), time.since(start_maps))
		dxil_build(&mc, len(paths))
		return
	}
	if len(sh.explicit) == 0 {fail("no rasterizer_globals (explicit shaders) in the given maps")}
	// explicit shaders outside rasterizer_globals (the HUD's chud_*), after its slots in name order; the microcode
	// read them with explicit -1 and their name (microcode_resolve)
	n_rasg := len(sh.explicit) // the rasterizer_globals slots; the rest are the named explicit shaders
	{
		in_rasg := make(map[string]bool)
		defer delete(in_rasg)
		for x in sh.explicit {if x.name != "" {in_rasg[x.name] = true}}
		extra: [dynamic]string
		defer delete(extra)
		for name in sh.named {if !(name in in_rasg) {append(&extra, name)}}
		slice.sort(extra[:])
		added, no_source := 0, 0
		for name in extra {
			short := name[strings.last_index_byte(name, '\\') + 1:]
			if !os.exists(strings.concatenate({opts.hlsl, "/", short, ".hlsl"})) {
				fmt.eprintfln("shaderpack: %s: no %s.hlsl in the DX11 tree (its draws stay unknown)", name, short)
				no_source += 1
				continue
			}
			append(&sh.explicit, sh.named[name])
			added += 1
		}
		fmt.printfln("shaderpack: %d explicit shaders outside rasterizer_globals (slots %d..), %d without DX11 source", added, len(sh.explicit) - added, no_source)
	}
	if opts.executable != "" {
		n, ok := microcode_read_executable(opts.executable, &mc)
		if !ok {os.exit(2)}
		fmt.printfln("shaderpack: %s: %d embedded shader functions", opts.executable, n)
	}
	if opts.survey {microcode_survey(&mc); return}
	if opts.dump != "" {microcode_dump(&mc, opts.dump); return}

	jobs := make_jobs(&sh, &mc)
	if len(opts.only) > 0 {
		kept: [dynamic]Job
		for &j in jobs {
			for o in opts.only {if strings.contains(j.name, o) {append(&kept, j); break}}
		}
		fmt.printfln("shaderpack: --only: %d of %d jobs", len(kept), len(jobs))
		jobs = kept
	}
	n_explicit, n_template, n_function := 0, 0, 0
	for &j in jobs {
		switch j.kind {
		case 0: n_explicit += 1
		case 1: n_template += 1
		case: n_function += 1
		}
	}
	fmt.printfln("shaderpack: %d maps, %d explicit slots, %d templates, %d render methods -> %d compile jobs (%d explicit, %d template, %d function)", len(paths), len(sh.explicit), len(sh.templates), len(sh.rmdfs), len(jobs), n_explicit, n_template, n_function)
	if opts.list {
		for &j in jobs {fmt.printfln("%s\t%s\t%s\t%s", j.name, entry_names[j.ep], stage_name[j.stage], vertex_names[j.vt])}
		return
	}
	if !os.exists(strings.concatenate({opts.hlsl, "/global_texture.fx"})) {fail("%s is not a staged HLSL tree (run port/tools/shaderpack/stage.sh)", opts.hlsl)}
	opts.fingerprint = tree_fingerprint(opts.hlsl)
	if opts.cache_dir == "none" {opts.cache_dir = ""}
	if opts.cache_dir != "" {cas_init(opts.cache_dir, opts.hlsl)}

	// compile
	start := time.now()
	run_jobs(jobs[:])
	elapsed := time.since(start)

	ok, no_entry, failed, cached := 0, 0, 0, 0
	for &j in jobs {
		switch j.status {
		case .Ok: ok += 1
		case .No_Entry: no_entry += 1
		case .Failed, .Pending: failed += 1
		}
		if j.cached {cached += 1}
	}
	fmt.printfln("shaderpack: %d ok, %d without that entry function, %d failed (%d from cache) in %v", ok, no_entry, failed, cached, elapsed)
	if opts.cache_dir != "" {fmt.printfln("shaderpack: compile cache: %d hits, %d stored", cas.hits, cas.stores)}
	// a stage the retail tag has must exist in the HLSL, except where DX11 replaces a memexport update (the retail
	// vertex + pixel shader pair) with a compute shader
	missing := 0
	for x, i in sh.explicit {
		if x.name == "" {continue}
		has: [Stage]Status
		for &j in jobs {if j.kind == 0 && j.name == x.name && j.ep == 0 && !j.optional {has[j.stage] = j.status}}
		if len(opts.only) > 0 {continue} // a partial build: the explicit slots it left out are not missing
		if i >= n_rasg {
			// a named shader no game code binds through an explicit slot: one without default_vs / default_ps
			// (simple.hlsl: albedo / static entry points of a debug render path) is left out
			if (x.has_ps && has[.PS] != .Ok) || (x.has_vs && has[.VS] != .Ok) {
				fmt.eprintfln("shaderpack: %s: no default_vs / default_ps in the DX11 tree (its draws stay unknown)", x.name)
			}
			continue
		}
		if x.has_ps && has[.PS] != .Ok && has[.CS] != .Ok {fmt.eprintfln("shaderpack: explicit %d %s: no pixel shader", i, x.name); missing += 1}
		if x.has_vs && has[.VS] != .Ok && has[.CS] != .Ok {fmt.eprintfln("shaderpack: explicit %d %s: no vertex or compute shader", i, x.name); missing += 1}
	}
	for &j in jobs {
		if j.status == .Failed || j.status == .Pending {
			fmt.eprintfln("%s %s %s %s %s: %s", j.optional ? "FAIL (optional, left out)" : "FAIL", j.name, entry_names[j.ep], stage_name[j.stage], vertex_names[j.vt], j.message)
			if j.optional {failed -= 1}
		}
	}
	if failed > 0 || missing > 0 {fail("%d shaders failed, %d explicit stages missing; no pack written", failed, missing)}

	write_pack(&sh, jobs[:], &mc)
}

// ---- jobs ----

make_jobs :: proc(sh: ^Map_Shaders, mc: ^Microcode_Set) -> [dynamic]Job {
	// entry points a render method definition provides through its shared shaders (glvs / glps), which the
	// templates' own pixl may lack (shadow_generate)
	shared_eps := make(map[string]u32)
	defer delete(shared_eps)
	for &ms in mc.shaders {
		for &src in ms.sources {
			if src.kind == .Shared && src.ep >= 0 && src.ep < 32 {shared_eps[src.name] |= 1 << u32(src.ep)}
		}
	}
	jobs: [dynamic]Job
	// explicit shaders: one job per unique name and stage (the table repeats some names)
	done: map[string]bool
	for x in sh.explicit {
		if x.name == "" || x.name in done {continue}
		done[x.name] = true
		short := x.name[strings.last_index_byte(x.name, '\\') + 1:]
		fx := strings.concatenate({short, ".hlsl"})
		vt := explicit_vertex_type(fx)
		for st in Stage {
			append(&jobs, Job{kind = 0, stage = st, ep = 0, vt = vt, name = x.name, ty = "explicit", fx = fx, dir = fmt.aprintf("%s/explicit/%s/%s", opts.work, short, stage_name[st])})
		}
		// the other entry points the source declares (//@entry: water_ripple's ripple_apply / ripple_slope /
		// underwater, shadow_apply's ambient blur, chud_turbulence's apply passes), which the game binds with
		// set_explicit_shaders(slot, .., entry point) and the retail pixl / vtsh carry their own microcode for
		for ep in explicit_entry_points(fx) {
			if ep == 0 {continue}
			for st in ([]Stage{.VS, .PS}) {
				append(&jobs, Job{kind = 0, stage = st, ep = ep, vt = vt, name = x.name, ty = "explicit", fx = fx, optional = true, dir = fmt.aprintf("%s/explicit/%s/%s.%s", opts.work, short, stage_name[st], entry_names[ep])})
			}
		}
		// the vertex shader's other vertex types: every //@generate line, and each (entry point, vertex type) the
		// retail vtsh has microcode for (shield_impact world / skinned, overhead_map_geometry rigid)
		for ev in explicit_extra_vertex_types(fx, x.name, vt, mc) {
			append(&jobs, Job{kind = 0, stage = .VS, ep = ev[0], vt = ev[1], name = x.name, ty = "explicit", fx = fx, optional = true, dir = fmt.aprintf("%s/explicit/%s/vs.%s.%s", opts.work, short, entry_names[ev[0]], vertex_names[ev[1]])})
		}
	}
	runtime_categories := shared_vs_runtime_categories(mc)
	// templates, in name order
	names: [dynamic]string
	for k in sh.templates {append(&names, k)}
	slice.sort(names[:])
	for name in names {
		t := sh.templates[name]
		rest := name[len("shaders\\"):]
		us := strings.index(rest, "_templates\\")
		if !strings.has_prefix(name, "shaders\\") || us < 0 {fail("unexpected template name %s", name)}
		ty := rest[:us]
		r, ok := &sh.rmdfs[strings.concatenate({"shaders\\", ty})]
		if !ok {fail("template %s: no render_method_definition shaders\\%s in the maps", name, ty)}
		fx: string
		switch ty {
		case "particle": fx = "particle.fx"
		case "contrail": fx = "contrail.fx"
		case: fx = strings.concatenate({ty, ".fx"})
		}
		tuple := rest[us + len("_templates\\_"):]
		key, _ := strings.replace_all(tuple, "\\", "_")
		if len(strings.split(tuple, "_")) != len(r.categories) {fail("template %s: %d options for %d categories", name, len(strings.split(tuple, "_")), len(r.categories))}
		if len(r.vertex_types) == 0 {fail("render method %s has no vertex types", r.name)}
		for ep in r.entry_points {
			if ep >= 32 || (t.entry_points | shared_eps[r.name]) & (1 << ep) == 0 {continue}
			// one pixel shader per entry point (compiled for the first vertex type, as pgen.sh did), one vertex
			// shader per entry point and vertex type
			v0 := int(r.vertex_types[0])
			append(&jobs, Job{kind = 1, stage = .PS, ep = int(ep), vt = v0, name = name, ty = ty, fx = fx, rmdf = r, dir = fmt.aprintf("%s/%s/%s/%s.ps.%s", opts.work, ty, key, entry_names[ep], vertex_names[v0])})
			for v in r.vertex_types {
				append(&jobs, Job{kind = 1, stage = .VS, ep = int(ep), vt = int(v), name = name, ty = ty, fx = fx, rmdf = r, runtime_categories = runtime_categories[r.name], dir = fmt.aprintf("%s/%s/%s/%s.vs.%s", opts.work, ty, key, entry_names[ep], vertex_names[v])})
			}
		}
	}
	// function reconstructions: one job per //@function line (stage and the microcode it implements), compiled like
	// an explicit shader (default_vs / default_ps, vertex type from //@generate)
	files, err := os.read_directory_by_path(opts.hlsl, -1, context.allocator)
	if err != nil {fail("cannot list %s", opts.hlsl)}
	slice.sort_by(files, proc(a, b: os.File_Info) -> bool {return a.name < b.name})
	for f in files {
		if !strings.has_suffix(f.name, ".hlsl") {continue}
		src, rerr := os.read_entire_file(strings.concatenate({opts.hlsl, "/", f.name}), context.allocator)
		if rerr != nil {fail("cannot read %s/%s", opts.hlsl, f.name)}
		text := string(src)
		if !strings.contains(text, "//@function ") {
			delete(src)
			continue
		}
		short := f.name[:len(f.name) - len(".hlsl")]
		vt := explicit_vertex_type(f.name)
		for line in strings.split_lines(text) {
			if !strings.has_prefix(line, "//@function ") {continue}
			fields := strings.fields(line[len("//@function "):])
			if len(fields) < 2 {fail("%s: //@function needs a stage and a microcode hash", f.name)}
			st: Stage
			switch fields[0] {
			case "vs": st = .VS
			case "ps": st = .PS
			case: fail("%s: //@function stage %q (vs or ps)", f.name, fields[0])
			}
			h, ok := strconv.parse_u64_of_base(fields[1], 16)
			if !ok {fail("%s: //@function hash %q", f.name, fields[1])}
			append(&jobs, Job{kind = 2, stage = st, ep = 0, vt = vt, name = short, ty = "function", fx = f.name, function_hash = h, dir = fmt.aprintf("%s/function/%s/%s", opts.work, short, stage_name[st])})
		}
	}
	return jobs
}

// the vertex type an explicit shader declares with //@generate (pexp.sh)
explicit_vertex_type :: proc(fx: string) -> int {
	src, err := os.read_entire_file(strings.concatenate({opts.hlsl, "/", fx}), context.allocator)
	if err != nil {fail("explicit shader source %s/%s not found", opts.hlsl, fx)}
	defer delete(src)
	text := string(src)
	gen := ""
	if i := strings.index(text, "//@generate "); i >= 0 {
		rest := text[i + len("//@generate "):]
		n := 0
		for n < len(rest) && (rest[n] == '_' || (rest[n] >= 'a' && rest[n] <= 'z') || (rest[n] >= '0' && rest[n] <= '9')) {n += 1}
		gen = rest[:n]
	}
	name: string
	switch gen {
	case "chud_fancy": name = "chud_vertex_fancy"
	case "chud_simple": name = "chud_vertex_simple"
	case "implicit_vertex": name = "implicit"
	case "tiny_position_only": name = "tiny_position"
	case "sky", "": name = "world"
	case:
		name = gen
		if strings.has_prefix(name, "s_") {name = name[2:]}
		if strings.has_suffix(name, "_vertex") {name = name[:len(name) - len("_vertex")]}
	}
	vt := vertex_index(name)
	if vt < 0 {fail("%s: unknown //@generate vertex type %q", fx, gen)}
	return vt
}

// ---- workers ----

Pool :: struct {
	jobs:  []Job,
	next:  int,
	done:  int,
}

run_jobs :: proc(jobs: []Job) {
	pool := Pool{jobs = jobs}
	threads: [dynamic]^thread.Thread
	for _ in 0 ..< opts.jobs {
		append(&threads, thread.create_and_start_with_poly_data(&pool, worker))
	}
	for t in threads {
		thread.join(t)
		thread.destroy(t)
	}
	fmt.println()
}

worker :: proc(pool: ^Pool) {
	for {
		i := sync.atomic_add(&pool.next, 1)
		if i >= len(pool.jobs) {return}
		compile_job(&pool.jobs[i])
		free_all(context.temp_allocator)
		d := sync.atomic_add(&pool.done, 1) + 1
		if d % 250 == 0 || d == len(pool.jobs) {fmt.printf("\rshaderpack: %d / %d", d, len(pool.jobs))}
	}
}

run :: proc(cmd: []string, dir := "") -> (ok: bool, msg: string) {
	state, stdout, stderr, err := os.process_exec(os.Process_Desc{command = cmd, working_dir = dir}, context.temp_allocator)
	if err != nil {return false, fmt.aprintf("cannot run %s: %v", cmd[0], err)}
	if !state.exited || state.exit_code != 0 {
		text := string(stderr)
		if len(text) == 0 {text = string(stdout)}
		// the first error line, without the tree's path prefix
		first := text
		if e := strings.index(text, "error"); e >= 0 {
			ls := strings.last_index_byte(text[:e], '\n') + 1
			le := strings.index_byte(text[e:], '\n')
			first = le < 0 ? text[ls:] : text[ls:e + le]
		} else if le := strings.index_byte(text, '\n'); le >= 0 {
			first = text[:le]
		}
		return false, strings.clone(first)
	}
	return true, ""
}

write_file :: proc(path: string, text: string) -> bool {
	return os.write_entire_file(path, transmute([]u8)text) == nil
}

// ---- compile cache: begin (cas.odin hashes this region: a change here invalidates the compile cache)

job_defines :: proc(j: ^Job) -> [dynamic]string {
	vstruct, deform := vertex_struct(j.vt)
	defs: [dynamic]string
	append(&defs, "-DDX_VERSION=11", "-Dpc", fmt.tprintf("-Dvertex_type=%s", vstruct), fmt.tprintf("-Ddeform=%s", deform), fmt.tprintf("-Dentry_point=%s", entry_names[j.ep]), "-DSHADER_30")
	if vertex_names[j.vt] == "beam" {append(&defs, "-DVERTEX_TYPE_s_beam_vertex=21")} // the struct exists, the number does not (rmdf: 21)
	switch j.stage {
	case .PS: append(&defs, "-DPIXEL_SHADER")
	case .VS: append(&defs, "-DVERTEX_SHADER")
	case .CS: append(&defs, "-DCOMPUTE_SHADER")
	}
	return defs
}

job_function :: proc(j: ^Job) -> string {
	if j.kind != 1 && (j.kind != 0 || j.ep == 0 || j.stage == .CS) {return fmt.tprintf("default_%s", stage_name[j.stage])}
	return j.stage == .PS ? fmt.tprintf("%s_ps", entry_ps_fn[j.ep]) : fmt.tprintf("%s_vs", entry_vs_fn[j.ep])
}

// the option wrapper (pgen.sh step 1) or the explicit shader include (pexp.sh)
job_wrapper :: proc(j: ^Job) -> string {
	b := strings.builder_make(context.temp_allocator)
	fmt.sbprintfln(&b, "// generated: %s", j.name)
	if j.kind == 1 {
		r := j.rmdf
		tuple := j.name[strings.last_index(j.name, "\\_") + 2:]
		sel := strings.split(tuple, "_", context.temp_allocator)
		option := make([]int, len(r.categories), context.temp_allocator)
		for c in 0 ..< len(r.categories) {
			v, ok := strconv.parse_int(sel[c])
			if !ok || v < 0 || v >= len(r.categories[c].options) {fail("template %s: option %s out of range for category %s", j.name, sel[c], r.categories[c].name)}
			option[c] = v
		}
		for &cat, c in r.categories {
			o := &cat.options[option[c]]
			if cat.vs_fn != "" && o.vs_fn != "" {fmt.sbprintfln(&b, "#define %s %s", cat.vs_fn, o.vs_fn)}
			if cat.ps_fn != "" && o.ps_fn != "" {fmt.sbprintfln(&b, "#define %s %s", cat.ps_fn, o.ps_fn)}
		}
		// category/option indices for render methods that test options with TEST_CATEGORY_OPTION (decal, particle, ...)
		for &cat, c in r.categories {
			// a shared vertex shader reads its categories as constants the game sets per draw (particle.fx: "If the
			// categories are not defined by the preprocessor, treat them as shader constants set by the game"), so
			// one module serves every template, as the one retail microcode does
			if !(strings.concatenate({"category_", cat.name}, context.temp_allocator) in j.runtime_categories) {fmt.sbprintfln(&b, "#define category_%s %d", cat.name, option[c])}
			for &o, oi in cat.options {if is_identifier(o.name) {fmt.sbprintfln(&b, "#define category_%s_option_%s %d", cat.name, o.name, oi)}}
		}
		// lighting passes compute albedo themselves for non-opaque blend modes and first-person-sometimes/always objects
		calc := false
		for &cat, c in r.categories {
			o := cat.options[option[c]].name
			if (cat.name == "blend_mode" && o != "opaque") || (cat.name == "misc" && (o == "first_person_sometimes" || o == "first_person_always")) {calc = true}
		}
		if calc {fmt.sbprintln(&b, "#define maybe_calc_albedo")}
	}
	fmt.sbprintfln(&b, "#include \"%s\"", j.fx)
	return strings.to_string(b)
}

is_identifier :: proc(s: string) -> bool {
	if s == "" {return false}
	for c in transmute([]u8)s {if !(c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {return false}}
	return true
}

Param_Decl :: struct {
	type, name: string, // name may carry [N]
}
Sampler_Decl :: struct {
	kind, name: string,
}

// ___PARAM___(type name) / ___SAMPLER___(kind name) from the PARAM_ALLOC_PREPROCESS output, first occurrence order
scan_decls :: proc(text: string, marker: string) -> [dynamic][2]string {
	out := make([dynamic][2]string, context.temp_allocator)
	seen := make(map[string]bool, allocator = context.temp_allocator)
	rest := text
	for {
		i := strings.index(rest, marker)
		if i < 0 {break}
		rest = rest[i + len(marker):]
		e := strings.index_byte(rest, ')')
		if e < 0 {break}
		body := strings.trim_space(rest[:e])
		rest = rest[e:]
		if body in seen {continue}
		seen[body] = true
		fields := strings.fields(body, context.temp_allocator)
		if len(fields) != 2 {continue}
		append(&out, [2]string{fields[0], fields[1]})
	}
	return out
}

// whether the preprocessed text defines `fn(` (as a word)
has_function :: proc(text, fn: string) -> bool {
	rest := text
	base := 0
	for {
		i := strings.index(rest, fn)
		if i < 0 {return false}
		at := base + i
		before_ok := at == 0 || !(text[at - 1] == '_' || (text[at - 1] >= 'a' && text[at - 1] <= 'z') || (text[at - 1] >= '0' && text[at - 1] <= '9'))
		k := at + len(fn)
		for k < len(text) && text[k] == ' ' {k += 1}
		if before_ok && k < len(text) && text[k] == '(' {return true}
		base = at + len(fn)
		rest = text[base:]
	}
}

// the final wrapper (pgen.sh emit()): all = keep every parameter and texture (pass A)
emit_final :: proc(j: ^Job, params: [][2]string, samplers: [][2]string, used_p, used_t: map[string]bool, all: bool, wrapper_file: string) -> string {
	b := strings.builder_make(context.temp_allocator)
	fmt.sbprintfln(&b, "// final: %s %s %s %s", j.name, entry_names[j.ep], stage_name[j.stage], vertex_names[j.vt])
	fmt.sbprintln(&b, "#define texture2D Texture2D\t// DXC: legacy lowercase resource keywords crash or fail inside structs (FXC accepted them)")
	fmt.sbprintln(&b, "#define texture3D Texture3D")
	fmt.sbprintln(&b, "#include \"global_texture.fx\"")
	fmt.sbprintln(&b, "cbuffer MaterialParameters : register(b10)\t// generated, 16 bytes per parameter")
	fmt.sbprintln(&b, "{")
	slot_type :: proc(t: string) -> string {
		if strings.has_prefix(t, "int") {return "int4"}
		if t == "bool" {return "bool4"}
		return "float4"
	}
	split_name :: proc(n: string) -> (string, string) {
		if i := strings.index_byte(n, '['); i >= 0 {return n[:i], n[i:]}
		return n, ""
	}
	count := 0
	for p in params {
		nn, arr := split_name(p[1])
		if !all && !(nn in used_p) {continue}
		fmt.sbprintfln(&b, "\t%s MP_%s%s;", slot_type(p[0]), nn, arr)
		count += 1
	}
	if count == 0 {fmt.sbprintln(&b, "\tfloat4 MP__empty;")}
	fmt.sbprintln(&b, "};")
	for p in params {
		t := p[0]
		nn, arr := split_name(p[1])
		sw := ""
		if t == "float" || t == "int" || t == "bool" {sw = ".x"} else if strings.has_suffix(t, "2") {sw = ".xy"} else if strings.has_suffix(t, "3") {sw = ".xyz"}
		if arr != "" {sw = ""}
		if all || nn in used_p {
			fmt.sbprintfln(&b, "#define ___%s (MP_%s%s)", nn, nn, sw)
		} else if arr != "" {
			fmt.sbprintfln(&b, "#define ___%s (MP__empty_array_%s)", nn, nn)
		} else {
			fmt.sbprintfln(&b, "#define ___%s ((%s)0)", nn, t)
		}
	}
	// unused arrays: a static zero array of the declared shape
	for p in params {
		nn, arr := split_name(p[1])
		if arr == "" || all || nn in used_p {continue}
		st := slot_type(p[0])
		fmt.sbprintfln(&b, "static const %s MP__empty_array_%s%s = (%s%s)0;", st, nn, arr, st, arr)
	}
	tb := j.stage == .PS ? 0 : 2
	s := 0
	unused_kinds := make(map[string]int, allocator = context.temp_allocator)
	for smp in samplers {
		k, n := smp[0], smp[1]
		tt, st, ss := "Texture2D<float4>", "texture_sampler_2d", "SamplerState"
		switch k {
		case "3D": tt, st = "Texture3D<float4>", "texture_sampler_3d"
		case "CUBE": tt, st = "TextureCube<float4>", "texture_sampler_cube"
		case "2D_ARRAY": tt, st = "Texture2DArray<float4>", "texture_sampler_2d_array"
		case "COMPARISON_2D": st, ss = "texture_sampler_comparison_2d", "SamplerComparisonState"
		}
		if all || n in used_t {
			slot := tb + s
			if tb == 0 && slot > 12 {slot = 19 + (s - 13)}
			fmt.sbprintfln(&b, "%s MT_%s : register(t%d);", tt, n, slot)
			fmt.sbprintfln(&b, "%s MS_%s : register(s%d);", ss, n, s < 13 ? s : 12)
			fmt.sbprintfln(&b, "static const %s %s = {{ MS_%s, MT_%s }};", st, n, n, n)
			s += 1
		} else {
			if !(k in unused_kinds) {
				unused_kinds[k] = len(unused_kinds) + 1
				fmt.sbprintfln(&b, "%s MT__unused_%s : register(t%d);", tt, k, 90 + len(unused_kinds))
				fmt.sbprintfln(&b, "%s MS__unused_%s : register(s15);", ss, k)
			}
			fmt.sbprintfln(&b, "static const %s %s = {{ MS__unused_%s, MT__unused_%s }};", st, n, k, k)
		}
	}
	fmt.sbprintfln(&b, "#include \"%s\"", wrapper_file)
	return strings.to_string(b)
}

dxc_compile :: proc(j: ^Job, defs: []string, src, out: string) -> (bool, string) {
	cmd: [dynamic]string
	cmd.allocator = context.temp_allocator
	append(&cmd, opts.dxc, "-spirv", "-T", fmt.tprintf("%s_6_0", stage_name[j.stage]), "-E", job_function(j), "-HV", "2018", "-flegacy-macro-expansion", "-Wno-return-type", "-fvk-use-dx-layout", "-fvk-b-shift", "0", "all", "-fvk-t-shift", "100", "all", "-fvk-s-shift", "200", "all", "-fvk-u-shift", "300", "all", "-fvk-auto-shift-bindings", "-fspv-target-env=vulkan1.2", "-I", hlsl_abs)
	append(&cmd, ..defs)
	append(&cmd, src, "-Fo", out)
	return run(cmd[:])
}

hlsl_abs: string

job_key :: proc(j: ^Job, wrapper: string) -> string {
	h := hash.fnv64a(transmute([]u8)wrapper)
	h = hash.fnv64a(transmute([]u8)BUILDER_VERSION, h)
	h = hash.fnv64a(transmute([]u8)opts.dxc, h)
	fp := opts.fingerprint
	h = hash.fnv64a(([^]u8)(&fp)[:8], h)
	return fmt.tprintf("%016x %s %d %d %s", h, stage_name[j.stage], j.ep, j.vt, j.name)
}

compile_job :: proc(j: ^Job) {
	wrapper := job_wrapper(j)
	defs := job_defines(j)
	defer delete(defs)
	// the content-addressed cache first (cas.odin): a hit needs no work directory
	cas_key := cas.dir != "" ? cas_job_key(j, wrapper, defs[:]) : ""
	if cas_key != "" && !opts.rebuild && cas_lookup(j, cas_key) {return}

	// workers create sibling directories concurrently: make_directory_all stops at a parent another worker made
	// between its check and its mkdir, so try again until the directory exists (each attempt gets further)
	for attempt := 0; attempt < 8 && !os.exists(j.dir); attempt += 1 {_ = os.make_directory_all(j.dir)}
	if !os.exists(j.dir) {j.status = .Failed; j.message = "cannot create work directory"; return}
	key := job_key(j, wrapper)
	key_path := fmt.tprintf("%s/result.key", j.dir)
	final_spv := fmt.tprintf("%s/final.spv", j.dir)

	// the per-job cache of this work directory (tree fingerprint); a hit also fills the content-addressed cache
	if !opts.rebuild {
		if old, err := os.read_entire_file(key_path, context.temp_allocator); err == nil {
			text := string(old)
			if text == fmt.tprintf("%s\nno-entry\n", key) {
				j.status = .No_Entry
				j.cached = true
				if cas_key != "" {cas_store(j, cas_key, "none", nil)}
				return
			}
			if text == fmt.tprintf("%s\nok\n", key) {
				if spv, serr := os.read_entire_file(final_spv, context.allocator); serr == nil {
					if finish_job(j, spv, false) {
						j.cached = true
						if cas_key != "" {cas_store(j, cas_key, "spv", j.spirv)}
						return
					}
				}
			}
		}
	}
	os.remove(key_path)

	g := fmt.tprintf("%s/g.hlsl", j.dir)
	if !write_file(g, wrapper) {j.status = .Failed; j.message = "cannot write wrapper"; return}

	// 2. declared parameters and textures; whether the entry function exists
	pre := fmt.tprintf("%s/pre.hlsl", j.dir)
	pcmd: [dynamic]string
	pcmd.allocator = context.temp_allocator
	append(&pcmd, opts.dxc, "-P", "-flegacy-macro-expansion", "-I", hlsl_abs)
	append(&pcmd, ..defs[:])
	append(&pcmd, "-DPARAM_ALLOC_PREPROCESS", "-Fi", pre, g)
	run(pcmd[:]) // declarations are best effort, as in pgen.sh: a failure shows up in pass A
	params, samplers: [dynamic][2]string
	if text, err := os.read_entire_file(pre, context.temp_allocator); err == nil {
		params = scan_decls(string(text), "___PARAM___(")
		samplers = scan_decls(string(text), "___SAMPLER___(")
	}
	os.remove(pre)
	resize(&pcmd, 5) // dxc -P -flegacy-macro-expansion -I <hlsl>
	append(&pcmd, ..defs[:])
	append(&pcmd, "-Fi", pre, g)
	pre_ok, pre_msg := run(pcmd[:])
	text, terr := os.read_entire_file(pre, context.temp_allocator)
	os.remove(pre)
	if terr != nil || !pre_ok {
		// the preprocessor failed or its output is gone: no entry function this time, but nothing is remembered (a
		// transient failure, e.g. a second shaderpack in the same work directory, must not stick in either cache)
		j.status = .No_Entry
		j.message = fmt.aprintf("preprocess: %s", pre_ok ? "no output" : pre_msg)
		return
	}
	if !has_function(string(text), job_function(j)) {
		j.status = .No_Entry
		write_file(key_path, fmt.tprintf("%s\nno-entry\n", key))
		if cas_key != "" {cas_store(j, cas_key, "none", nil)}
		return
	}

	// 3. pass A: everything declared
	none := make(map[string]bool, allocator = context.temp_allocator)
	fa := fmt.tprintf("%s/fa.hlsl", j.dir)
	a_spv := fmt.tprintf("%s/a.spv", j.dir)
	write_file(fa, emit_final(j, params[:], samplers[:], none, none, true, "g.hlsl"))
	if ok, msg := dxc_compile(j, defs[:], fa, a_spv); !ok {j.status = .Failed; j.message = fmt.aprintf("pass A: %s", msg); return}

	// 4. reflection: what pass A reads
	used_p := make(map[string]bool, allocator = context.temp_allocator)
	used_t := make(map[string]bool, allocator = context.temp_allocator)
	{
		code, err := os.read_entire_file(a_spv, context.allocator)
		if err != nil {j.status = .Failed; j.message = "pass A: no output"; return}
		defer delete(code)
		s: Spv
		defer spv_destroy(&s)
		if !spv_parse(code, &s) {j.status = .Failed; j.message = "pass A: bad SPIR-V"; return}
		names: [dynamic]string
		names.allocator = context.temp_allocator
		spv_used_material_parameters(&s, &names)
		for n in names {used_p[strings.clone(n, context.temp_allocator)] = true}
		clear(&names)
		spv_kept_material_textures(&s, &names)
		for n in names {used_t[strings.clone(n, context.temp_allocator)] = true}
	}
	os.remove(a_spv)
	os.remove(fa)

	// 5. pass B: compact
	f := fmt.tprintf("%s/f.hlsl", j.dir)
	b_spv := fmt.tprintf("%s/b.spv", j.dir)
	write_file(f, emit_final(j, params[:], samplers[:], used_p, used_t, false, "g.hlsl"))
	if ok, msg := dxc_compile(j, defs[:], f, b_spv); !ok {j.status = .Failed; j.message = fmt.aprintf("pass B: %s", msg); return}
	code, err := os.read_entire_file(b_spv, context.allocator)
	if err != nil {j.status = .Failed; j.message = "pass B: no output"; return}
	os.remove(b_spv)
	if !finish_job(j, code, true) {return}
	// spirv-val on the final module (after the descriptor set move)
	if os.write_entire_file(final_spv, j.spirv) != nil {j.status = .Failed; j.message = "cannot write final.spv"; return}
	if ok, msg := run({"spirv-val", "--target-env", "vulkan1.2", "--scalar-block-layout", final_spv}); !ok {
		j.status = .Failed
		j.message = fmt.aprintf("spirv-val: %s", msg)
		return
	}
	write_file(key_path, fmt.tprintf("%s\nok\n", key))
	if cas_key != "" {cas_store(j, cas_key, "spv", j.spirv)}
}

// final module: descriptor set by stage, reflection; takes ownership of code
finish_job :: proc(j: ^Job, code: []u8, move_set: bool) -> bool {
	s: Spv
	defer spv_destroy(&s)
	if !spv_parse(code, &s) {delete(code); j.status = .Failed; j.message = "bad final SPIR-V"; return false}
	if move_set {spv_set_descriptor_set(&s, j.stage == .PS ? 1 : 0)}
	r: Reflection
	if !spv_reflect(&s, &r, j.stage) {delete(code); j.status = .Failed; j.message = "unsupported resource type in the final module"; return false}
	// names point into the module: clone them
	for &p in r.params {p.name = strings.clone(p.name)}
	for &b in r.bindings {b.name = strings.clone(b.name)}
	for &i in r.inputs {i.semantic = strings.clone(i.semantic)}
	for &c in r.cbuffers {
		c.name = strings.clone(c.name)
		for &m in c.members {m.name = strings.clone(m.name)}
	}
	j.spirv = code
	j.refl = r
	j.status = .Ok
	return true
}

// ---- compile cache: end

// names, sizes and modification times of the HLSL tree: a changed source invalidates the compile cache
tree_fingerprint :: proc(dir: string) -> u64 {
	abs, err := os.get_absolute_path(dir, context.allocator)
	if err != nil {fail("cannot resolve %s", dir)}
	hlsl_abs = abs
	entries, lerr := os.read_directory_by_path(dir, -1, context.allocator)
	if lerr != nil {fail("cannot list %s", dir)}
	slice.sort_by(entries, proc(a, b: os.File_Info) -> bool {return a.name < b.name})
	h := hash.fnv64a(transmute([]u8)BUILDER_VERSION)
	for e in entries {
		h = hash.fnv64a(transmute([]u8)e.name, h)
		v := [2]i64{e.size, time.to_unix_nanoseconds(e.modification_time)}
		h = hash.fnv64a(([^]u8)(&v)[:16], h)
	}
	return h
}

// ---- the pack ----

Pack_Shader :: struct {
	kind, stage, ep, vt: u8,
	explicit_index:      u16,
	name:                string,
	job:                 ^Job,
}

name_hash :: proc(s: string) -> u32 {
	h: u32 = 0x811c9dc5
	for c in transmute([]u8)s {
		x := c == '/' ? '\\' : c
		h = (h ~ u32(x)) * 0x01000193
	}
	return h
}

Strings :: struct {
	data:    [dynamic]u8,
	offsets: map[string]u32,
}

intern :: proc(st: ^Strings, s: string) -> u32 {
	if o, ok := st.offsets[s]; ok {return o}
	o := u32(len(st.data))
	append(&st.data, ..transmute([]u8)s)
	append(&st.data, 0)
	st.offsets[s] = o
	return o
}

put32 :: proc(b: ^[dynamic]u8, v: u32) {append(b, u8(v), u8(v >> 8), u8(v >> 16), u8(v >> 24))}
put16 :: proc(b: ^[dynamic]u8, v: u16) {append(b, u8(v), u8(v >> 8))}
align4 :: proc(b: ^[dynamic]u8) {for len(b) % 4 != 0 {append(b, 0)}}

write_pack :: proc(sh: ^Map_Shaders, jobs: []Job, mc: ^Microcode_Set) {
	// shader entries: every explicit slot x stage that compiled, every template job that compiled
	entries: [dynamic]Pack_Shader
	for x, i in sh.explicit {
		if x.name == "" {continue}
		for &j in jobs {
			if j.kind == 0 && j.name == x.name && j.status == .Ok {
				append(&entries, Pack_Shader{kind = 0, stage = u8(j.stage), ep = u8(j.ep), vt = u8(j.vt), explicit_index = u16(i), name = x.name, job = &j})
			}
		}
	}
	for &j in jobs {
		if j.kind != 0 && j.status == .Ok {
			append(&entries, Pack_Shader{kind = j.kind, stage = u8(j.stage), ep = u8(j.ep), vt = u8(j.vt), explicit_index = 0xffff, name = j.name, job = &j})
		}
	}
	slice.sort_by(entries[:], proc(a, b: Pack_Shader) -> bool {
		if a.kind != b.kind {return a.kind < b.kind}
		if a.name != b.name {return a.name < b.name}
		if a.ep != b.ep {return a.ep < b.ep}
		if a.stage != b.stage {return a.stage < b.stage}
		if a.vt != b.vt {return a.vt < b.vt}
		return a.explicit_index < b.explicit_index
	})

	// unique modules by content
	Module :: struct {
		code:       []u8,
		hash:       u64,
		compressed: []u8,
	}
	modules: [dynamic]Module
	by_hash: map[u64][dynamic]int
	module_of := make(map[^Job]int)
	raw_total := 0
	for &e in entries {
		if e.job in module_of {continue}
		code := e.job.spirv
		h := hash.fnv64a(code)
		found := -1
		if list, ok := by_hash[h]; ok {
			for mi in list {if slice.equal(modules[mi].code, code) {found = mi; break}}
		}
		if found < 0 {
			found = len(modules)
			append(&modules, Module{code = code, hash = h})
			if !(h in by_hash) {by_hash[h] = {}}
			append(&by_hash[h], found)
		}
		module_of[e.job] = found
		raw_total += len(code)
	}
	// compress (zlib, best), the modules shared out among the job threads (each module on its own: the same bytes)
	Compress_Pool :: struct {
		modules: []Module,
		next:    int,
	}
	compress_worker :: proc(pool: ^Compress_Pool) {
		for {
			i := sync.atomic_add(&pool.next, 1)
			if i >= len(pool.modules) {return}
			m := &pool.modules[i]
			bound := zlib.compressBound(zlib.uLong(len(m.code)))
			out := make([]u8, int(bound))
			out_len := zlib.uLongf(bound)
			if zlib.compress2(raw_data(out), &out_len, raw_data(m.code), zlib.uLong(len(m.code)), 9) != zlib.OK {fail("zlib compress2 failed")}
			m.compressed = out[:int(out_len)]
		}
	}
	compress_pool := Compress_Pool{modules = modules[:]}
	compress_threads: [dynamic]^thread.Thread
	for _ in 0 ..< max(opts.jobs, 1) {
		append(&compress_threads, thread.create_and_start_with_poly_data(&compress_pool, compress_worker))
	}
	for t in compress_threads {
		thread.join(t)
		thread.destroy(t)
	}
	blob: [dynamic]u8
	unique_raw := 0
	for &m in modules {unique_raw += len(m.code)}

	st: Strings
	intern(&st, "")
	records: [dynamic]u8
	nrecords: u32 = 0
	shader_table: [dynamic]u8
	for &e in entries {
		r := &e.job.refl
		first := nrecords
		for p in r.params {
			put32(&records, intern(&st, p.name)); put32(&records, name_hash(p.name)); put32(&records, p.offset); put32(&records, p.size | p.kind << 24)
			nrecords += 1
		}
		for b in r.bindings {
			put32(&records, intern(&st, b.name)); put32(&records, name_hash(b.name))
			put32(&records, b.set << 16 | b.binding)
			put32(&records, b.descriptor_type | b.role << 8 | b.image_dim << 16 | min(b.count, 255) << 24)
			nrecords += 1
		}
		for i in r.inputs {
			put32(&records, intern(&st, i.semantic)); put32(&records, name_hash(i.semantic)); put32(&records, i.location); put32(&records, i.component_count | i.base_type << 8)
			nrecords += 1
		}
		append(&shader_table, e.kind, e.stage, e.ep, e.vt)
		put16(&shader_table, e.explicit_index)
		put16(&shader_table, 0)
		put32(&shader_table, intern(&st, e.name))
		put32(&shader_table, name_hash(e.name))
		put32(&shader_table, u32(module_of[e.job]))
		put32(&shader_table, r.material_size)
		put32(&shader_table, first)
		put16(&shader_table, u16(len(r.params)))
		put16(&shader_table, u16(len(r.bindings)))
		put16(&shader_table, u16(len(r.inputs)))
		put16(&shader_table, 0)
	}

	HEADER_SIZE :: 96

	// retail microcode -> pack shader, and the cbuffer gathers (microcode.odin, cbuffer.odin)
	cstats: Cbuffer_Stats
	cbuffers, cb_ok := cbuffer_build(jobs, mc, &cstats)
	if !cb_ok {fail("cannot build the cbuffer gather tables")}
	slice.sort_by(cbuffers[:], proc(a, b: Cbuffer_Table) -> bool {return name_hash(a.name) < name_hash(b.name)})
	table_index := make(map[string]int)
	for &c, i in cbuffers {table_index[c.name] = i}
	mstats: Microcode_Stats
	micro := microcode_resolve(mc, sh, entries[:], module_of, cbuffers[:], table_index, &mstats)
	agree, disagree := cbuffer_check_retail(cbuffers[:], micro[:], mc, entries[:])
	gathers, samplers, micro_table, cbuffer_table: [dynamic]u8
	ngathers, nsamplers: u32
	put_gather :: proc(b: ^[dynamic]u8, g: Gather) {put16(b, u16(g.dst)); append(b, u8(g.size), u8(g.src)); put16(b, u16(g.reg)); put16(b, u16(g.buffer))}
	for &m in micro {
		put32(&micro_table, u32(m.hash)); put32(&micro_table, u32(m.hash >> 32))
		put32(&micro_table, m.shader_index < 0 ? 0xffffffff : u32(m.shader_index))
		append(&micro_table, u8(m.stage), u8(m.kind), m.ep < 0 ? 0xff : u8(m.ep), m.vt < 0 ? 0xff : u8(m.vt))
		put32(&micro_table, u32(m.size))
		put32(&micro_table, ngathers)
		put16(&micro_table, u16(len(m.gathers))); put16(&micro_table, u16(len(m.samplers)))
		put32(&micro_table, nsamplers)
		put16(&micro_table, u16(m.unmatched)); put16(&micro_table, 0)
		for g in m.gathers {put_gather(&gathers, g); ngathers += 1}
		for s in m.samplers {put16(&samplers, u16(s.binding)); put16(&samplers, u16(s.sampler)); nsamplers += 1}
	}
	for &c, ci in cbuffers {
		for &g in c.gathers {g.buffer = u32(ci)}
		put32(&cbuffer_table, intern(&st, c.name)); put32(&cbuffer_table, name_hash(c.name)); put32(&cbuffer_table, c.size); put32(&cbuffer_table, ngathers)
		put16(&cbuffer_table, u16(len(c.gathers))); put16(&cbuffer_table, u16(len(c.unresolved)))
		for g in c.gathers {put_gather(&gathers, g); ngathers += 1}
	}
	align4(&st.data)
	module_table: [dynamic]u8
	blob_off: u32 = 0
	for &m in modules {
		put32(&module_table, blob_off)
		put32(&module_table, u32(len(m.compressed)))
		put32(&module_table, u32(len(m.code)))
		put32(&module_table, hash.crc32(m.code))
		put32(&module_table, u32(m.hash))
		put32(&module_table, u32(m.hash >> 32))
		append(&blob, ..m.compressed)
		blob_off += u32(len(m.compressed))
	}
	align4(&st.data)

	file: [dynamic]u8
	resize(&file, HEADER_SIZE)
	module_offset := u32(len(file)); append(&file, ..module_table[:])
	shader_offset := u32(len(file)); append(&file, ..shader_table[:])
	record_offset := u32(len(file)); append(&file, ..records[:])
	micro_offset := u32(len(file)); append(&file, ..micro_table[:])
	cbuffer_offset := u32(len(file)); append(&file, ..cbuffer_table[:])
	gather_offset := u32(len(file)); append(&file, ..gathers[:])
	sampler_offset := u32(len(file)); append(&file, ..samplers[:])
	string_offset := u32(len(file)); append(&file, ..st.data[:])
	align4(&file)
	blob_offset := u32(len(file)); append(&file, ..blob[:])
	header: [dynamic]u8
	for v in ([?]u32{0x50533348, 2, HEADER_SIZE, u32(len(file)), u32(len(modules)), module_offset, u32(len(entries)), shader_offset, nrecords, record_offset, u32(len(st.data)), string_offset, u32(len(blob)), blob_offset, u32(len(sh.explicit))}) {put32(&header, v)}
	put32(&header, hash.crc32(file[HEADER_SIZE:blob_offset]))
	for v in ([?]u32{u32(len(micro)), micro_offset, u32(len(cbuffers)), cbuffer_offset, ngathers, gather_offset, nsamplers, sampler_offset}) {put32(&header, v)}
	copy(file[:HEADER_SIZE], header[:])
	if os.write_entire_file(opts.out, file[:]) != nil {fail("cannot write %s", opts.out)}

	// readable manifest
	mb := strings.builder_make()
	fmt.sbprintln(&mb, "# kind\tname\texplicit_index\tentry_point\tstage\tvertex_type\tmodule\tmaterial_bytes\tparams\tbindings\tinputs")
	for &e in entries {
		r := &e.job.refl
		fmt.sbprintf(&mb, "%s\t%s\t%d\t%s\t%s\t%s\t%d\t%d\t", kind_names[e.kind], e.name, e.kind == 0 ? int(e.explicit_index) : -1, entry_names[e.ep], stage_name[Stage(e.stage)], vertex_names[e.vt], module_of[e.job], r.material_size)
		for p, i in r.params {fmt.sbprintf(&mb, "%s%s@%d", i == 0 ? "" : " ", p.name, p.offset)}
		fmt.sbprint(&mb, "\t")
		for b, i in r.bindings {fmt.sbprintf(&mb, "%s%s=%d.%d:%d", i == 0 ? "" : " ", b.name, b.set, b.binding, b.descriptor_type)}
		fmt.sbprint(&mb, "\t")
		for x, i in r.inputs {fmt.sbprintf(&mb, "%s%s@%d", i == 0 ? "" : " ", x.semantic, x.location)}
		fmt.sbprintln(&mb)
	}
	write_file(strings.concatenate({opts.out, ".tsv"}), strings.to_string(mb))

	stages: [3]int
	kind_entries: [3]int
	for &e in entries {stages[e.stage] += 1; kind_entries[e.kind] += 1}
	fmt.printfln("shaderpack: %s: %d shaders (%d explicit, %d template, %d function; vs %d, ps %d, cs %d), %d unique modules", opts.out, len(entries), kind_entries[0], kind_entries[1], kind_entries[2], stages[0], stages[1], stages[2], len(modules))
	fmt.printfln("shaderpack: SPIR-V %.1f MB referenced, %.1f MB unique, %.2f MB compressed; pack %.2f MB", f64(raw_total) / 1e6, f64(unique_raw) / 1e6, f64(len(blob)) / 1e6, f64(len(file)) / 1e6)
	for k in Microcode_Kind {
		fmt.printfln("shaderpack: retail %v shaders: vs %d / %d, ps %d / %d resolved to pack shaders", k, mstats.resolved[k][.VS], mstats.resolved[k][.VS] + mstats.unresolved[k][.VS], mstats.resolved[k][.PS], mstats.resolved[k][.PS] + mstats.unresolved[k][.PS])
	}
	fmt.printfln("shaderpack: microcode table: %d entries; %d / %d MaterialParameters members and %d / %d textures matched to Xbox constants by name; %d vertex type mismatches, %d shared vertex shaders whose DX11 module depends on the template", len(micro), mstats.mp_matched, mstats.mp_members, mstats.textures_matched, mstats.textures, mstats.vertex_type_mismatch, mstats.template_dependent)
	fmt.printfln("shaderpack: cbuffer gathers: %d cbuffers, %d / %d members resolved to Xbox registers (%d padding), %d without a declaration, %d layout conflicts; %d gather records; retail explicit constant tables agree %d, disagree %d", cstats.buffers, cstats.resolved, cstats.members, cstats.padding, cstats.no_declaration, cstats.layout_conflicts, ngathers, agree, disagree)
	fmt.printfln("shaderpack: %d members resolved from the retail constant tables by name; %d per-shader register overrides in %d retail shaders", cstats.resolved_retail, mstats.cbuffer_overrides, mstats.shaders_with_overrides)
	cb := strings.builder_make()
	fmt.sbprintln(&cb, "# cbuffer\tsize\tgathers (dst+size<-file reg)\tunresolved members")
	for &c in cbuffers {
		fmt.sbprintf(&cb, "%s\t%d\t", c.name, c.size)
		for g, i in c.gathers {fmt.sbprintf(&cb, "%s%d+%d<-%v%d", i == 0 ? "" : " ", g.dst, g.size, g.src, g.reg)}
		fmt.sbprint(&cb, "\t")
		for u, i in c.unresolved {fmt.sbprintf(&cb, "%s%s", i == 0 ? "" : " ", u)}
		fmt.sbprintln(&cb)
	}
	write_file(strings.concatenate({opts.out, ".cbuffers.tsv"}), strings.to_string(cb))
	mb2 := strings.builder_make()
	fmt.sbprintln(&mb2, "# microcode_fnv64\tstage\tkind\tsource\tentry_point\tvertex_type\tpack_shader\tgathers\tsamplers\tunmatched\tunmatched_names")
	for &m in micro {
		pn := m.shader_index >= 0 ? entries[m.shader_index].name : "-"
		fmt.sbprintfln(&mb2, "%016x\t%s\t%v\t%s\t%d\t%d\t%s\t%d\t%d\t%d\t%s", m.hash, stage_name[m.stage], m.kind, m.source, m.ep, m.vt, pn, len(m.gathers), len(m.samplers), m.unmatched, strings.join(m.unmatched_names[:], ","))
	}
	write_file(strings.concatenate({opts.out, ".microcode.tsv"}), strings.to_string(mb2))
}

// the entry points an explicit shader's source declares (//@entry <name>, MCC's own metadata); 0 (default) always
explicit_entry_points :: proc(fx: string) -> [dynamic]int {
	out: [dynamic]int
	append(&out, 0)
	src, err := os.read_entire_file(strings.concatenate({opts.hlsl, "/", fx}), context.allocator)
	if err != nil {return out}
	defer delete(src)
	for line in strings.split_lines(string(src), context.temp_allocator) {
		l := strings.trim_space(line)
		if !strings.has_prefix(l, "//@entry ") {continue}
		name := strings.trim_space(l[len("//@entry "):])
		for e, i in entry_names {
			if e != name || i == 0 {continue}
			dup := false
			for x in out {if x == i {dup = true}}
			if !dup {append(&out, i)}
		}
	}
	return out
}

// the (entry point, vertex type) vertex shaders an explicit shader needs besides (default, its first //@generate
// type): its other //@generate lines, and every (declared entry point, vertex type) the retail vtsh has microcode for
explicit_extra_vertex_types :: proc(fx, name: string, vt: int, mc: ^Microcode_Set) -> [dynamic][2]int {
	out: [dynamic][2]int
	add :: proc(out: ^[dynamic][2]int, ep, v, vt: int) {
		if v == vt {return} // (default, vt) is the base job, (entry, vt) the //@entry jobs
		for x in out {if x == {ep, v} {return}}
		append(out, [2]int{ep, v})
	}
	src, err := os.read_entire_file(strings.concatenate({opts.hlsl, "/", fx}), context.allocator)
	if err == nil {
		defer delete(src)
		for line in strings.split_lines(string(src), context.temp_allocator) {
			l := strings.trim_space(line)
			if !strings.has_prefix(l, "//@generate ") {continue}
			if v := generate_vertex_type(strings.trim_space(l[len("//@generate "):])); v >= 0 {add(&out, 0, v, vt)}
		}
	}
	eps := explicit_entry_points(fx)
	defer delete(eps)
	for &ms in mc.shaders {
		if ms.stage != .VS {continue}
		for &s in ms.sources {
			if s.kind != .Explicit || s.name != name || s.vt < 0 || s.ep < 0 {continue}
			for e in eps {if e == s.ep {add(&out, s.ep, s.vt, vt)}}
		}
	}
	return out
}

// a //@generate word -> vertex type (the mapping of explicit_vertex_type), -1 when unknown
generate_vertex_type :: proc(word: string) -> int {
	n := 0
	for n < len(word) && (word[n] == '_' || (word[n] >= 'a' && word[n] <= 'z') || (word[n] >= '0' && word[n] <= '9')) {n += 1}
	name: string
	switch gen := word[:n]; gen {
	case "chud_fancy": name = "chud_vertex_fancy"
	case "chud_simple": name = "chud_vertex_simple"
	case "implicit_vertex": name = "implicit"
	case "tiny_position_only": name = "tiny_position"
	case "sky", "": name = "world"
	case:
		name = gen
		if strings.has_prefix(name, "s_") {name = name[2:]}
		if strings.has_suffix(name, "_vertex") {name = name[:len(name) - len("_vertex")]}
	}
	return vertex_index(name)
}

// per render method definition: the category_<name> constants its shared vertex shaders (glvs) read, by the retail
// constant tables (particle: albedo, blend_mode, fog, frame_blend, lighting, self_illumination,
// specialized_rendering; contrail, beam, light_volume: blend_mode, fog; water: global_shape, waveshape). The DX11
// tree declares each as a CATEGORY_PARAM when the preprocessor does not define it.
shared_vs_runtime_categories :: proc(mc: ^Microcode_Set) -> map[string]map[string]bool {
	out := make(map[string]map[string]bool)
	for &ms in mc.shaders {
		if ms.stage != .VS {continue}
		for &s in ms.sources {
			if s.kind != .Shared {continue}
			for c in s.constants {
				if !strings.has_prefix(c.name, "category_") || c.set == REGSET_SAMPLER {continue}
				if !(s.name in out) {out[s.name] = make(map[string]bool)}
				set := &out[s.name]
				set[c.name] = true
			}
		}
	}
	return out
}
