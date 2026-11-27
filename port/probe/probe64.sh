#!/usr/bin/env bash
# port/probe/probe64.sh: native 64-bit (x86_64) Linux probe link -> tmp/probe64/halo3_probe.
# Reads sources from port/probe/{tu,include,extra,fill,retailfill} and base objects from build-pc/lin64,
# keeping 32-bit tmp/probe and play/halo3_probe untouched.
set -euo pipefail
repo=$(cd "$(dirname "$0")/../.." && pwd)
psrc=$repo/port/probe
build=${PROBE64_BUILD:-$repo/build-pc/lin64}
probe=${PROBE64_OUT:-$repo/tmp/probe64}
out=$probe/stage1
ms_map=$repo/port/sdk/runtime/ms_libc_linux64.map
retail=$(sed -n 's/^HALO3_RETAIL_PE:[A-Z]*=//p' "$build/CMakeCache.txt")
mkdir -p "$out" "$probe/obj" "$probe/elf" "$probe/fill"
cd "$build"
rel_inc=$(realpath --relative-to="$build" "$repo/include")

# 1. Override objects (from port/probe/tu and port/probe/include).
: > "$out/subst.txt"
(cd "$psrc/include" && find . -name '*.h' | sed 's|^\./||' | sort) > "$out/header-overrides.txt"
{ printf "{ 'version': 0, 'case-sensitive': 'true', 'roots': [\n"
  sep=
  while IFS= read -r h; do
    printf "%s  { 'type': 'file', 'name': '%s', 'external-contents': '%s' }" "$sep" "$repo/include/$h" "$psrc/include/$h"
    sep=", "
  done < "$out/header-overrides.txt"
  printf "\n] }\n"; } > "$out/header-overlay.yaml"
ninja -t deps 2>/dev/null | awk -v repo="$repo" -v rel_inc="$rel_inc" -v probe="$psrc/include" '
  FNR == NR { h[repo "/include/" $0] = $0; h[rel_inc "/" $0] = $0; next }
  /^[^ ].*: #deps/ { obj = $1; sub(/:$/, "", obj); next }
  /^    / { d = $1; if (d in h && obj ~ /^CMakeFiles\/halo3_game\.dir/) {
      rel = obj; sub("^CMakeFiles/halo3_game.dir" repo "/tu/", "", rel); sub(/\.o$/, "", rel)
      print rel "\t" probe "/" h[d] } }' "$out/header-overrides.txt" - | sort -u > "$out/header-units.tsv"
o0=${PROBE_O0_UNITS:-}
{ (cd "$psrc/tu" && find . -name '*.cpp' | sed 's|^\./||'); cut -f1 "$out/header-units.tsv"
  [ -z "$o0" ] || grep -o "^build CMakeFiles/halo3_game.dir$repo/tu/[^:]*\.cpp\.o:" build.ninja \
    | sed "s|^build CMakeFiles/halo3_game.dir$repo/tu/||; s|\.o:\$||" | grep -E "$o0" || true; } | sort -u > "$out/units.txt"
while IFS= read -r rel <&3; do
  orig=$repo/tu/$rel
  src=$orig
  [ -f "$psrc/tu/$rel" ] && src=$psrc/tu/$rel
  objrel=CMakeFiles/halo3_game.dir$repo/tu/$rel.o
  block=$(awk -v key="build $objrel:" 'index($0, key) == 1 { on = 1; next } on && /^build / { exit } on { print }' build.ninja)
  [ -n "$block" ] || { echo "probe64: no build block for $rel" >&2; exit 1; }
  get() { printf '%s\n' "$block" | sed -n "s/^  $1 = //p"; }
  forced=$(awk -F'\t' -v rel="$rel" -v overlay="$out/header-overlay.yaml" '$1 == rel { printf " -ivfsoverlay %s", overlay; exit }' "$out/header-units.tsv")
  obj=$probe/obj/$rel.o elf=$probe/elf/$rel.o optimize=
  if [ -n "$o0" ] && printf '%s\n' "$rel" | grep -qE "$o0"; then obj=$probe/obj-O0/$rel.o elf=$probe/elf-O0/$rel.o optimize=" -O0"; fi
  mkdir -p "$(dirname "$obj")"
  recipe="$src$forced$optimize $(get FLAGS)"
  if [ ! "$obj" -nt "$src" ] || [ "$(cat "$obj.recipe" 2>/dev/null)" != "$recipe" ] ||
     [ "$(find "$repo/include" "$psrc/include" "$repo/port/sdk/include" "$repo/port/sdk/xbox-api/include" -newer "$obj" -name '*.h' | head -1)" ]; then
    echo "probe64: compile $rel${forced:+ (header overrides)}"
    eval "clang++ --target=x86_64-linux-gnu $(get DEFINES) -iquote $(dirname "$orig") -I$psrc/include$forced $(get INCLUDES) $(get FLAGS)$optimize -c '$src' -o '$obj'"
    printf '%s\n' "$recipe" > "$obj.recipe"
  fi
  mkdir -p "$(dirname "$elf")"
  "$repo/port/build/elf-names.sh" "$obj" "$elf" "$ms_map"
  printf '%s\t%s\n' "$build/stage1/elf/${objrel#CMakeFiles/}" "$elf" >> "$out/subst.txt"
done 3< "$out/units.txt"
echo "probe64: $(wc -l < "$out/subst.txt") overrides ($(cut -f1 "$out/header-units.tsv" | sort -u | wc -l) through header overrides)"

# Probe-only objects (port/probe/extra/*.cpp).
ref=CMakeFiles/halo3_game.dir$repo/tu/blamlibxbox/particle_system.cpp.o
block=$(awk -v key="build $ref:" 'index($0, key) == 1 { on = 1; next } on && /^build / { exit } on { print }' build.ninja)
get() { printf '%s\n' "$block" | sed -n "s/^  $1 = //p"; }
extra_defs=
if grep -qE '^(CMAKE_BUILD_TYPE:STRING=Release|HALO3_RELEASE_BUILD:BOOL=ON)$' "$build/CMakeCache.txt" 2>/dev/null; then
  extra_defs="-DHALO3_RELEASE_BUILD=1"
fi
: > "$out/extra.txt"
for src in "$psrc"/extra/*.cpp; do
  [ -e "$src" ] || continue
  obj=$probe/obj/extra_$(basename "$src").o
  if [ ! "$obj" -nt "$src" ] || [ "$repo/port/sdk/runtime/port_abi.h" -nt "$obj" ] ||
     [ "$repo/port/sdk/runtime/hd_font.h" -nt "$obj" ] ||
     [ "$psrc/extra/hd_text_atlas.h" -nt "$obj" ] ||
     [ "$repo/port/sdk/xam/guide_ingame.h" -nt "$obj" ] ||
     [ "$(cat "$obj.defs" 2>/dev/null)" != "$extra_defs" ]; then
    echo "probe64: compile extra $(basename "$src")"
    eval "clang++ --target=x86_64-linux-gnu $(get DEFINES) $extra_defs $(get INCLUDES) $(get FLAGS) -include '$repo/port/sdk/runtime/port_abi.h' -iquote '$repo/port/sdk' -c '$src' -o '$obj'"
    printf '%s\n' "$extra_defs" > "$obj.defs"
  fi
  "$repo/port/build/elf-names.sh" "$obj" "$probe/elf/extra_$(basename "$src").o" "$ms_map"
  echo "$probe/elf/extra_$(basename "$src").o" >> "$out/extra.txt"
done

# Compute defined symbols of extra + substituted objects BEFORE retailfill -64 so clean builds
# resolve 64-bit mangled names from port/probe/extra and port/probe/tu identically on the first pass.
{ cat "$out/extra.txt"; awk -F'\t' 'FNR == NR { s[$1] = $2; next } ($0 in s) { print s[$0]; next } { print }' \
  "$out/subst.txt" "$build/stage1/objects-elf.txt"; } > "$out/objects-base.txt"
xargs -a "$out/objects-base.txt" llvm-nm --defined-only --extern-only --format=just-symbols 2>/dev/null \
  | sort -u > "$out/defined-base.txt"

# Retail-datum fillers (port/probe/fill/*.spec, retailfill -64).
if [ ! -x "$probe/retailfill" ] || [ "$psrc/retailfill/main.odin" -nt "$probe/retailfill" ]; then
  odin build "$psrc/retailfill" -out:"$probe/retailfill" -o:speed
fi
: > "$out/fill.txt"
for spec in "$psrc"/fill/*.spec; do
  [ -e "$spec" ] || continue
  name=$(basename "${spec%.spec}")
  base=$probe/fill/$name
  "$probe/retailfill" -64 "$spec" "$retail" "$repo" "$out/defined-base.txt" > "$base.s" 2> "$base.log" \
    || { cat "$base.log" >&2; exit 1; }
  clang --target=x86_64-linux-gnu -c "$base.s" -o "$base.o"
  echo "$base.o" >> "$out/fill.txt"
done
echo "probe64: $(wc -l < "$out/fill.txt") fillers"

# 64-bit linker aliases (32-bit pointer mangling -> 64-bit __ptr64 mangling).
sed -e 's/?global_scenario!!3PAUscenario!!A/?global_scenario!!3PEAUscenario!!EA/g' \
    -e 's/?global_tag_instances!!3PBUcache_file_tag_instance!!B/?global_tag_instances!!3PEBUcache_file_tag_instance!!EB/g' \
    "$psrc/fill/aliases.ld" > "$out/aliases64.ld"

cat "$out/objects-base.txt" "$out/fill.txt" > "$out/objects-elf.txt"
objects=$out/objects-elf.txt
printf '{\n  global: _IO_stdin_used;\n  local: *;\n};\n' > "$out/hide.ver"
link() {
  local output=$1; shift
  clang --target=x86_64-linux-gnu -fuse-ld=lld -no-pie -o "$output" @"$objects" "$out/aliases64.ld" "$@" -lpthread -lm -ldl \
    -Wl,--allow-multiple-definition,--no-dependent-libraries,--wrap=main,--build-id="${PROBE64_BUILD_ID:-sha1}" \
    -Wl,--version-script="$out/hide.ver"
}

# 2. Trial link, stubs.
link "$out/trial" -Wl,--error-limit=0 > "$out/trial.log" 2>&1 || true
grep -o 'undefined symbol: .*' "$out/trial.log" | sed 's/undefined symbol: //' | sort -u > "$out/undefined.txt"
xargs -a "$objects" llvm-objdump -r > "$out/relocs.txt"
xargs -a "$objects" llvm-nm --defined-only --extern-only --format=just-symbols 2>/dev/null | sort -u > "$out/defined.txt"
awk 'FNR == NR { if ($0 !~ /^#/ && NF) deny[$1] = 1; next } NF >= 3 && ($3 in deny) { print $3 }' \
  "$repo/port/sdk/runtime/host-deny-linux.txt" "$out/relocs.txt" | sort -u | comm -23 - "$out/defined.txt" > "$out/denied.txt"
sort -u "$out/undefined.txt" "$out/denied.txt" -o "$out/undefined.txt"
echo "probe64: $(wc -l < "$out/undefined.txt") undefined symbols"
owners=$repo/metadata/matching/data-owners.tsv
"$build/tools/stubgen" elf64 "$out/undefined.txt" "$out/relocs.txt" "$owners" "$out/defined.txt" "$out/aliases.ld" "$out/stubs.tsv" \
  > /dev/null 2> "$out/stubgen-list.log"
awk -F'\t' '$1 == "data" { print $2 }' "$out/stubs.tsv" > "$out/data-stubs.txt"
"$build/tools/datagen" survey "$out/data-stubs.txt" "$retail" "$repo" > "$out/retail-data.tsv" 2> "$out/retail-data.log"
"$build/tools/stubgen" elf64 "$out/undefined.txt" "$out/relocs.txt" "$owners" "$out/defined.txt" "$out/aliases.ld" "$out/stubs.tsv" \
  "$out/retail-data.tsv" > "$out/stubs.s"
clang --target=x86_64-linux-gnu -c "$out/stubs.s" -o "$out/stubs.o"
echo "probe64: $(awk -F'\t' '$1 == "function"' "$out/stubs.tsv" | wc -l) function stubs, $(wc -l < "$out/data-stubs.txt") data stubs"

# 3. Final link.
link "$probe/halo3_probe" "$out/stubs.o" "$out/aliases.ld" -Wl,-Map="$out/halo3.map" > "$out/link.log" 2>&1 \
  || { tail -20 "$out/link.log"; exit 1; }
echo "probe64: $probe/halo3_probe"
