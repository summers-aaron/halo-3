#!/usr/bin/env bash
# tmp/probe/probe.sh: scratch diagnostic link tmp/probe/halo3_probe (Linux), never committed.
# build-pc/lin's objects, with every tu/<path>.cpp that has a copy under tmp/probe/tu/<path>.cpp replaced by that
# copy compiled with the exact build.ninja flags of the original unit. Then the same trial link -> stubgen -> datagen
# -> final link as port/build/link-stage1.sh (linux branch).
set -euo pipefail
repo=$(cd "$(dirname "$0")/../.." && pwd)
build=$repo/build-pc/lin
probe=$repo/tmp/probe
out=$probe/stage1
retail=$(sed -n 's/^HALO3_RETAIL_PE:[A-Z]*=//p' "$build/CMakeCache.txt")
mkdir -p "$out" "$probe/obj" "$probe/elf"
cd "$build"

# 1. Override objects.
# Header overrides (tmp/probe/include/<path>.h) apply to every unit that includes include/<path>.h, by build-pc/lin's
# ninja deps, not only to the tmp/probe/tu copies: a header that changes a layout (F38's bit-fields) must reach every
# object that uses the type. Such a unit is compiled from its tmp/probe/tu copy when it has one, else from tu/, through
# a clang VFS overlay that puts each override at its include/ path, so it is read where the unit includes it (a forced
# -include would read it ahead of the unit's own system headers) and no quoted include beside it can find include/'s
# copy.
: > "$out/subst.txt"
(cd "$probe/include" && find . -name '*.h' | sed 's|^\./||' | sort) > "$out/header-overrides.txt"
{ printf "{ 'version': 0, 'case-sensitive': 'true', 'roots': [\n"
  sep=
  while IFS= read -r h; do
    printf "%s  { 'type': 'file', 'name': '%s', 'external-contents': '%s' }" "$sep" "$repo/include/$h" "$probe/include/$h"
    sep=", "
  done < "$out/header-overrides.txt"
  printf "\n] }\n"; } > "$out/header-overlay.yaml"
ninja -t deps 2>/dev/null | awk -v repo="$repo" -v probe="$probe/include" '
  FNR == NR { h["../../include/" $0] = $0; next }
  /^[^ ].*: #deps/ { obj = $1; sub(/:$/, "", obj); next }
  /^    / { d = $1; if (d in h && obj ~ /^CMakeFiles\/halo3_game\.dir/) {
      rel = obj; sub("^CMakeFiles/halo3_game.dir" repo "/tu/", "", rel); sub(/\.o$/, "", rel)
      print rel "\t" probe "/" h[d] } }' "$out/header-overrides.txt" - | sort -u > "$out/header-units.tsv"
# Diagnostic bisection: PROBE_O0_UNITS=<extended regex over tu/-relative paths> compiles the matching units at -O0
# (objects kept apart under obj-O0/), to find which units an optimization miscompiles.
o0=${PROBE_O0_UNITS:-}
hd_text_unit=graphicslibxbox/rasterizer_text.cpp
{ (cd "$probe/tu" && find . -name '*.cpp' | sed 's|^\./||'); cut -f1 "$out/header-units.tsv"
  printf '%s\n' "$hd_text_unit"
  [ -z "$o0" ] || grep -o "^build CMakeFiles/halo3_game.dir$repo/tu/[^:]*\.cpp\.o:" build.ninja \
    | sed "s|^build CMakeFiles/halo3_game.dir$repo/tu/||; s|\.o:\$||" | grep -E "$o0" || true; } | sort -u > "$out/units.txt"
while IFS= read -r rel <&3; do
  orig=$repo/tu/$rel
  src=$orig
  [ -f "$probe/tu/$rel" ] && src=$probe/tu/$rel
  [ "$rel" != "$hd_text_unit" ] || src=$repo/port/probe/tu/$rel
  objrel=CMakeFiles/halo3_game.dir$repo/tu/$rel.o
  block=$(awk -v key="build $objrel:" 'index($0, key) == 1 { on = 1; next } on && /^build / { exit } on { print }' build.ninja)
  [ -n "$block" ] || { echo "probe: no build block for $rel" >&2; exit 1; }
  get() { printf '%s\n' "$block" | sed -n "s/^  $1 = //p"; }
  forced=$(awk -F'\t' -v rel="$rel" -v overlay="$out/header-overlay.yaml" '$1 == rel { printf " -ivfsoverlay %s", overlay; exit }' "$out/header-units.tsv")
  obj=$probe/obj/$rel.o elf=$probe/elf/$rel.o optimize=
  if [ -n "$o0" ] && printf '%s\n' "$rel" | grep -qE "$o0"; then obj=$probe/obj-O0/$rel.o elf=$probe/elf-O0/$rel.o optimize=" -O0"; fi
  mkdir -p "$(dirname "$obj")"
  # the object also records what it was compiled from (a retired tmp/probe/tu copy leaves an object newer than tu/)
  recipe="$src$forced$optimize $(get FLAGS)"
  if [ ! "$obj" -nt "$src" ] || [ "$(cat "$obj.recipe" 2>/dev/null)" != "$recipe" ] ||
     [ "$(find "$repo/include" "$probe/include" "$repo/port/sdk/include" -newer "$obj" -name '*.h' | head -1)" ]; then
    echo "probe: compile $rel${forced:+ (header overrides)}"
    eval "clang++ --target=i686-linux-gnu $(get DEFINES) -iquote $(dirname "$orig") -I$probe/include$forced $(get INCLUDES) $(get FLAGS)$optimize -c '$src' -o '$obj'"
    printf '%s\n' "$recipe" > "$obj.recipe"
  fi
  mkdir -p "$(dirname "$elf")"
  "$repo/port/build/elf-names.sh" "$obj" "$elf"
  printf '%s\t%s\n' "$build/stage1/elf/${objrel#CMakeFiles/}" "$elf" >> "$out/subst.txt"
done 3< "$out/units.txt"
echo "probe: $(wc -l < "$out/subst.txt") overrides ($(cut -f1 "$out/header-units.tsv" | sort -u | wc -l) through header overrides)"

# Probe-only objects, compiled like a game unit (particle_system.cpp's flags). The Guide renderer comes from the
# checkout so it uses the same toast layout as the SDK. Other extras come from tmp/probe/extra.
guide_src=$repo/port/probe/extra/guide_ingame_ui.cpp
ref=CMakeFiles/halo3_game.dir$repo/tu/blamlibxbox/particle_system.cpp.o
block=$(awk -v key="build $ref:" 'index($0, key) == 1 { on = 1; next } on && /^build / { exit } on { print }' build.ninja)
get() { printf '%s\n' "$block" | sed -n "s/^  $1 = //p"; }
: > "$out/extra.txt"
for src in "$guide_src" "$probe"/extra/*.cpp; do
  [ -e "$src" ] || continue
  [ "$src" != "$probe/extra/guide_ingame_ui.cpp" ] || continue
  obj=$probe/obj/extra_$(basename "$src").o
  if [ ! "$obj" -nt "$src" ] || [ "$repo/port/sdk/runtime/port_abi.h" -nt "$obj" ] ||
     [ "$repo/port/sdk/runtime/hd_font.h" -nt "$obj" ] ||
     [ "$repo/port/sdk/xam/guide_ingame.h" -nt "$obj" ]; then
    echo "probe: compile extra $(basename "$src")"
    eval "clang++ --target=i686-linux-gnu $(get DEFINES) $(get INCLUDES) $(get FLAGS) -include '$repo/port/sdk/runtime/port_abi.h' -iquote '$repo/port/sdk' -c '$src' -o '$obj'"
  fi
  "$repo/port/build/elf-names.sh" "$obj" "$probe/elf/extra_$(basename "$src").o"
  echo "$probe/elf/extra_$(basename "$src").o" >> "$out/extra.txt"
done

# Retail-datum fillers (tmp/probe/fill/*.spec, retailfill): first on the line, so they win over zero stubs.
: > "$out/fill.txt"
for spec in "$probe"/fill/*.spec; do
  [ -e "$spec" ] || continue
  base=${spec%.spec}
  defs=$out/defined.txt; [ -f "$defs" ] || defs=$build/stage1/defined.txt
  "$probe/retailfill/retailfill" "$spec" "$retail" "$repo" "$defs" > "$base.s" 2> "$base.log" \
    || { cat "$base.log" >&2; exit 1; }
  clang --target=i686-linux-gnu -c "$base.s" -o "$base.o"
  echo "$base.o" >> "$out/fill.txt"
done
echo "probe: $(wc -l < "$out/fill.txt") fillers"
{ cat "$out/fill.txt" "$out/extra.txt"; awk -F'\t' 'FNR == NR { s[$1] = $2; next } ($0 in s) { print s[$0]; next } { print }' \
  "$out/subst.txt" "$build/stage1/objects-elf.txt"; } > "$out/objects-elf.txt"
objects=$out/objects-elf.txt
printf '{\n  global: _IO_stdin_used;\n  local: *;\n};\n' > "$out/hide.ver"
link() {
  local output=$1; shift
  clang --target=i686-linux-gnu -fuse-ld=lld -no-pie -o "$output" @"$objects" "$probe/fill/aliases.ld" "$@" -lpthread -lm -ldl \
    -Wl,--allow-multiple-definition,--no-dependent-libraries,--wrap=main -Wl,--version-script="$out/hide.ver"
}

# 2. Trial link, stubs (as link-stage1.sh).
link "$out/trial" -Wl,--error-limit=0 > "$out/trial.log" 2>&1 || true
grep -o 'undefined symbol: .*' "$out/trial.log" | sed 's/undefined symbol: //' | sort -u > "$out/undefined.txt"
xargs -a "$objects" llvm-objdump -r > "$out/relocs.txt"
xargs -a "$objects" llvm-nm --defined-only --extern-only --format=just-symbols 2>/dev/null | sort -u > "$out/defined.txt"
awk 'FNR == NR { if ($0 !~ /^#/ && NF) deny[$1] = 1; next } NF >= 3 && ($3 in deny) { print $3 }' \
  "$repo/port/sdk/runtime/host-deny-linux.txt" "$out/relocs.txt" | sort -u | comm -23 - "$out/defined.txt" > "$out/denied.txt"
sort -u "$out/undefined.txt" "$out/denied.txt" -o "$out/undefined.txt"
echo "probe: $(wc -l < "$out/undefined.txt") undefined symbols"
owners=$repo/metadata/matching/data-owners.tsv
retail=$(sed -n 's/^HALO3_RETAIL_PE:[A-Z]*=//p' "$build/CMakeCache.txt")
"$build/tools/stubgen" elf "$out/undefined.txt" "$out/relocs.txt" "$owners" "$out/defined.txt" "$out/aliases.ld" "$out/stubs.tsv" \
  > /dev/null 2> "$out/stubgen-list.log"
awk -F'\t' '$1 == "data" { print $2 }' "$out/stubs.tsv" > "$out/data-stubs.txt"
"$build/tools/datagen" survey "$out/data-stubs.txt" "$retail" "$repo" > "$out/retail-data.tsv" 2> "$out/retail-data.log"
"$build/tools/stubgen" elf "$out/undefined.txt" "$out/relocs.txt" "$owners" "$out/defined.txt" "$out/aliases.ld" "$out/stubs.tsv" \
  "$out/retail-data.tsv" > "$out/stubs.s"
clang --target=i686-linux-gnu -c "$out/stubs.s" -o "$out/stubs.o"
echo "probe: $(awk -F'\t' '$1 == "function"' "$out/stubs.tsv" | wc -l) function stubs, $(wc -l < "$out/data-stubs.txt") data stubs"

# 3. Final link.
link "$probe/halo3_probe" "$out/stubs.o" "$out/aliases.ld" -Wl,-Map="$out/halo3.map" > "$out/link.log" 2>&1 \
  || { tail -20 "$out/link.log"; exit 1; }
echo "probe: $probe/halo3_probe"
