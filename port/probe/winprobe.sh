#!/usr/bin/env bash
# port/probe/winprobe.sh: cross-link the playable 0-stub Windows PE (x86_64 or i686) halo3_probe.exe
# from a configured build-pc/win64 (or win) tree + port/probe/{tu,include,extra,fill}.
#
# Prerequisites:
#   cmake -S port/build -B build-pc/win64 -G Ninja \
#     -DCMAKE_TOOLCHAIN_FILE=port/build/toolchains/x86_64-windows-msvc.cmake \
#     -DHALO3_RETAIL_PE=/path/to/default.pe
#   ninja -C build-pc/win64
#
# Output: ${WINPROBE_OUT:-build-pc/win64/probe}/halo3_probe.exe
set -euo pipefail
script_dir=$(cd "$(dirname "$0")" && pwd)
repo=$(cd "$script_dir/../.." && pwd)
tree=${WINPROBE_TREE:-$repo}
build=${WINPROBE_BUILD:-$repo/build-pc/win64}
src=${WINPROBE_SRC:-$repo/port/probe}
here=${WINPROBE_OUT:-$build/probe}
out=$here/stage1
retail=$(sed -n 's/^HALO3_RETAIL_PE:[A-Z]*=//p' "$build/CMakeCache.txt")
[ -n "$retail" ] || { echo "winprobe: HALO3_RETAIL_PE not found in $build/CMakeCache.txt" >&2; exit 1; }
proc=$(sed -n 's/^CMAKE_SYSTEM_PROCESSOR:[A-Z]*=//p' "$build/CMakeCache.txt")
arch=i686
if [ "$proc" = x86_64 ] || grep -q 'x86_64-windows-msvc' "$build/CMakeCache.txt" || \
   grep -q -- '--target=x86_64-' "$build/build.ninja"; then
  arch=x86_64
fi
if [ "$arch" = x86_64 ]; then
  target=x86_64-pc-windows-msvc
  stub_fmt=coff64
else
  target=i686-pc-windows-msvc
  stub_fmt=coff
fi
mkdir -p "$out" "$here/obj" "$here/fill"

# Build retailfill once if missing or older than its source.
retailfill=$here/retailfill
if [ ! -x "$retailfill" ] || [ "$src/retailfill/main.odin" -nt "$retailfill" ]; then
  echo "winprobe: build retailfill"
  odin build "$src/retailfill" -o:speed -out:"$retailfill"
fi

# Locate mingw-w64 runtime libs matching the target architecture.
if [ "$arch" = x86_64 ]; then
  mingw=${MINGW64_LIB:-}
  if [ -z "$mingw" ]; then
    for d in \
      /opt/mingw64/lib \
      /usr/x86_64-w64-mingw32/sys-root/mingw/lib \
      /usr/x86_64-w64-mingw32/lib \
      /usr/lib/x86_64-w64-mingw32; do
      if [ -f "$d/libmsvcrt.a" ]; then mingw=$d; break; fi
    done
  fi
  [ -n "$mingw" ] && [ -f "$mingw/libmsvcrt.a" ] || {
    echo "winprobe: 64-bit libmsvcrt.a not found (install mingw64-crt / gcc-mingw-w64-x86-64 or set MINGW64_LIB)" >&2
    exit 1
  }
else
  mingw=${MINGW_LIB:-}
  if [ -z "$mingw" ]; then
    for d in \
      /opt/mingw32/lib \
      /usr/i686-w64-mingw32/sys-root/mingw/lib \
      /usr/i686-w64-mingw32/lib \
      /usr/lib/i686-w64-mingw32; do
      if [ -f "$d/libmsvcrt.a" ]; then mingw=$d; break; fi
    done
  fi
  [ -n "$mingw" ] && [ -f "$mingw/libmsvcrt.a" ] || {
    echo "winprobe: 32-bit libmsvcrt.a not found (install mingw32-crt / gcc-mingw-w64-i686 or set MINGW_LIB)" >&2
    exit 1
  }
fi

cd "$build"
rel_inc=$(realpath --relative-to="$build" "$tree/include")

block_of() { awk -v key="build $1:" 'index($0, key) == 1 { on = 1; next } on && /^build / { exit } on { print }' build.ninja; }

# 1. Header & TU overrides: VFS overlay putting each $src/include/<h> at $tree/include/<h>.
(cd "$src/include" && find . -name '*.h' | sed 's|^\./||' | sort) > "$out/header-overrides.txt"
{ printf "{ 'version': 0, 'case-sensitive': 'true', 'roots': [\n"
  sep=
  while IFS= read -r h; do
    printf "%s  { 'type': 'file', 'name': '%s', 'external-contents': '%s' }" "$sep" "$tree/include/$h" "$src/include/$h"
    sep=", "
  done < "$out/header-overrides.txt"
  printf "\n] }\n"; } > "$out/header-overlay.yaml"
ninja -t deps 2>/dev/null | awk -v tree="$tree" -v rel_inc="$rel_inc" '
  FNR == NR { h[tree "/include/" $0] = 1; h[rel_inc "/" $0] = 1; next }
  /^[^ ].*: #deps/ { obj = $1; sub(/:$/, "", obj); next }
  /^    / { if (($1 in h) && obj ~ /^CMakeFiles\/halo3_game\.dir/) {
      rel = obj; sub("^CMakeFiles/halo3_game.dir" tree "/tu/", "", rel); sub(/\.obj$/, "", rel); print rel } }' \
  "$out/header-overrides.txt" - | sort -u > "$out/header-units.txt"
{ (cd "$src/tu" && find . -name '*.cpp' | sed 's|^\./||'); cat "$out/header-units.txt"; } | sort -u > "$out/units.txt"

: > "$out/subst.txt"
while IFS= read -r rel <&3; do
  orig=$tree/tu/$rel
  unit=$orig; [ -f "$src/tu/$rel" ] && unit=$src/tu/$rel
  objrel=CMakeFiles/halo3_game.dir$tree/tu/$rel.obj
  block=$(block_of "$objrel")
  [ -n "$block" ] || { echo "winprobe: no build block for $rel" >&2; exit 1; }
  get() { printf '%s\n' "$block" | sed -n "s/^  $1 = //p"; }
  overlay=
  grep -qxF "$rel" "$out/header-units.txt" && overlay=" -ivfsoverlay $out/header-overlay.yaml"
  obj=$here/obj/$rel.obj
  mkdir -p "$(dirname "$obj")"
  recipe="$unit$overlay $(get FLAGS)"
  if [ ! "$obj" -nt "$unit" ] || [ "$(cat "$obj.recipe" 2>/dev/null)" != "$recipe" ] ||
     [ "$(find "$tree/include" "$src/include" "$repo/port/sdk/include" "$repo/port/sdk/xbox-api/include" -newer "$obj" -name '*.h' | head -1)" ]; then
    echo "winprobe: compile $rel${overlay:+ (header overrides)}"
    eval "clang++ --target=$target $(get DEFINES) -iquote $(dirname "$orig") -I$src/include$overlay $(get INCLUDES) $(get FLAGS) -c '$unit' -o '$obj'"
    printf '%s\n' "$recipe" > "$obj.recipe"
  fi
  printf '%s\t%s\n' "$build/$objrel" "$obj" >> "$out/subst.txt"
done 3< "$out/units.txt"
echo "winprobe: $(wc -l < "$out/subst.txt") overrides ($(wc -l < "$out/header-units.txt") through header overrides)"

# 2. Probe-only objects (extra/*.cpp), compiled like particle_system.cpp.
block=$(block_of "CMakeFiles/halo3_game.dir$tree/tu/blamlibxbox/particle_system.cpp.obj")
get() { printf '%s\n' "$block" | sed -n "s/^  $1 = //p"; }
extra_defs=
if grep -qE '^(CMAKE_BUILD_TYPE:STRING=Release|HALO3_RELEASE_BUILD:BOOL=ON)$' "$build/CMakeCache.txt" 2>/dev/null; then
  extra_defs="-DHALO3_RELEASE_BUILD=1"
fi
: > "$out/extra.txt"
for unit in "$src"/extra/*.cpp; do
  [ -e "$unit" ] || continue
  obj=$here/obj/extra_$(basename "$unit").obj
  if [ ! "$obj" -nt "$unit" ] || [ "$repo/port/sdk/runtime/port_abi.h" -nt "$obj" ] ||
     [ "$repo/port/sdk/runtime/hd_font.h" -nt "$obj" ] ||
     [ "$src/extra/hd_text_atlas.h" -nt "$obj" ] ||
     [ "$repo/port/sdk/xam/guide_ingame.h" -nt "$obj" ] ||
     [ "$(cat "$obj.defs" 2>/dev/null)" != "$extra_defs" ]; then
    echo "winprobe: compile extra $(basename "$unit")"
    eval "clang++ --target=$target $(get DEFINES) $extra_defs -I$src/include $(get INCLUDES) $(get FLAGS) -include '$repo/port/sdk/runtime/port_abi.h' -iquote '$repo/port/sdk' -c '$unit' -o '$obj'"
    printf '%s\n' "$extra_defs" > "$obj.defs"
  fi
  echo "$obj" >> "$out/extra.txt"
done

# 3. Retail-datum fillers: compute defined symbols from extra + substituted objects first (matching probe64.sh)
#    so clean builds resolve 64-bit mangled names from port/probe/extra and port/probe/tu on the first pass.
{ cat "$out/extra.txt"
  awk -F'\t' 'FNR == NR { s[$1] = $2; next } ($0 in s) { print s[$0]; next } { print }' "$out/subst.txt" "$build/stage1/objects.txt"
} > "$out/objects-base.txt"
xargs -a "$out/objects-base.txt" llvm-nm --defined-only --extern-only --format=just-symbols 2>/dev/null \
  | sort -u > "$out/defined-base.txt"
if [ "$arch" = x86_64 ]; then
  sed -E '/^\?/ s/@/!/g' "$out/defined-base.txt" | sort -u > "$out/defined-elf.txt"
  rf_flags=(-win64)
  awk_arch=(-v arch64=1)
else
  sed -E '/^\?/ s/@/!/g; /^\?/! s/^_//' "$out/defined-base.txt" | sort -u > "$out/defined-elf.txt"
  rf_flags=()
  awk_arch=()
fi
: > "$out/fill.txt"
for spec in "$src"/fill/*.spec; do
  base=$here/fill/$(basename "${spec%.spec}")
  "$retailfill" "${rf_flags[@]}" "$spec" "$retail" "$repo" "$out/defined-elf.txt" > "$base.elf.s" 2> "$base.log" \
    || { cat "$base.log" >&2; exit 1; }
  awk "${awk_arch[@]}" -f "$script_dir/coff-asm.awk" "$base.elf.s" > "$base.s"
  clang --target=$target -c "$base.s" -o "$base.obj"
  echo "$base.obj" >> "$out/fill.txt"
done
awk "${awk_arch[@]}" -f "$script_dir/coff-aliases.awk" "$src/fill/aliases.ld" > "$here/fill/aliases.s"
clang --target=$target -c "$here/fill/aliases.s" -o "$here/fill/aliases.obj"
echo "$here/fill/aliases.obj" >> "$out/fill.txt"
echo "winprobe: $(wc -l < "$out/fill.txt") fillers ($arch)"

if [ "$arch" = x86_64 ]; then
  cat "$out/objects-base.txt" "$out/fill.txt" > "$out/objects.txt"
else
  cat "$out/fill.txt" "$out/objects-base.txt" > "$out/objects.txt"
fi
objects=$out/objects.txt
link() {  # as port/build/link-stage1.sh (windows)
  local output=$1; shift
  local dbg_flags=(/debug)
  if [ "${WINPROBE_DEBUG:-1}" = 0 ]; then
    dbg_flags=(/Brepro)
  fi
  if [ "$arch" = x86_64 ]; then
    lld-link /nologo /machine:x64 /subsystem:console /entry:port_entry /nodefaultlib /demangle:no \
      /force:multiple /base:0x00400000 /dynamicbase:no /highentropyva:no "${dbg_flags[@]}" /out:"$output" @"$objects" "$@" \
      "$mingw/libmsvcrt.a" "$mingw/libmingwex.a" "$mingw/libkernel32.a"
  else
    lld-link /nologo /machine:x86 /subsystem:console /entry:port_entry /nodefaultlib /safeseh:no /demangle:no \
      /force:multiple /largeaddressaware "${dbg_flags[@]}" /out:"$output" @"$objects" "$@" \
      "$mingw/libmsvcrt.a" "$mingw/libmingwex.a" "$mingw/libkernel32.a"
  fi
}

# 4. Trial link, stubs (as link-stage1.sh), final link.
link "$out/trial" /force:unresolved /errorlimit:0 > "$out/trial.log" 2>&1 || true
grep -o 'undefined symbol: .*' "$out/trial.log" | sed 's/undefined symbol: //' | sort -u > "$out/undefined.txt"
echo "winprobe: $(wc -l < "$out/undefined.txt") undefined symbols ($arch)"
xargs -a "$objects" llvm-objdump -r > "$out/relocs.txt"
xargs -a "$objects" llvm-nm --defined-only --extern-only --format=just-symbols 2>/dev/null | sort -u > "$out/defined.txt"
owners=$tree/metadata/matching/data-owners.tsv
"$build/tools/stubgen" "$stub_fmt" "$out/undefined.txt" "$out/relocs.txt" "$owners" "$out/defined.txt" "$out/aliases.ld" "$out/stubs.tsv" \
  > /dev/null 2> "$out/stubgen-list.log"
awk -F'\t' '$1 == "data" { print $2 }' "$out/stubs.tsv" > "$out/data-stubs.txt"
"$build/tools/datagen" survey "$out/data-stubs.txt" "$retail" "$tree" > "$out/retail-data.tsv" 2> "$out/retail-data.log"
"$build/tools/stubgen" "$stub_fmt" "$out/undefined.txt" "$out/relocs.txt" "$owners" "$out/defined.txt" "$out/aliases.ld" "$out/stubs.tsv" \
  "$out/retail-data.tsv" > "$out/stubs.s" 2> "$out/stubgen.log"
cat "$out/stubgen.log"
clang --target=$target -c "$out/stubs.s" -o "$out/stubs.obj"
res_args=()
[ -n "${WINPROBE_RES:-}" ] && [ -f "$WINPROBE_RES" ] && res_args=("$WINPROBE_RES")
link "$here/halo3_probe.exe" "$out/stubs.obj" "${res_args[@]}" /map:"$out/halo3.map" > "$out/link.log" 2>&1 || { tail -20 "$out/link.log"; exit 1; }
echo "winprobe: $here/halo3_probe.exe"
