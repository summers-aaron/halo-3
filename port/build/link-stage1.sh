#!/usr/bin/env bash
# port/build/link-stage1.sh TARGET BUILD_DIR RETAIL_PE: the stage-1 link (glue; the analysis is port/tools/stubgen).
#   1. trial link of the game and runtime objects, keeping going past undefined symbols, to list them;
#   2. stubgen defines each one (functions report their name and stop; data is zeroed storage, and data with retail
#      values is poisoned: port/tools/datagen surveys this link's data stubs in RETAIL_PE, the retail image);
#   3. the final link with the stubs: BUILD_DIR/halo3.exe (windows) or BUILD_DIR/halo3 (linux).
# Duplicate definitions are allowed for now (the Xbox build places by address; see port/GAME-CHANGES.md).
# Work files: BUILD_DIR/stage1/.
set -euo pipefail
target=$1 build=$(cd "$2" && pwd) retail=$3 arch=${4:-i686}
port=$(cd "$(dirname "$0")/.." && pwd)
out=$build/stage1
mkdir -p "$out"
if [ ! -f "$retail" ]; then
  echo "link-stage1: no retail image at $retail (configure with -DHALO3_RETAIL_PE=...): data stubs cannot be poisoned" >&2
  exit 1
fi

for tool in stubgen datagen; do
  if [ ! -x "$build/tools/$tool" ] || [ "$port/tools/$tool/main.odin" -nt "$build/tools/$tool" ]; then
    mkdir -p "$build/tools"
    odin build "$port/tools/$tool" -out:"$build/tools/$tool" -o:speed
  fi
done
stubgen=$build/tools/stubgen

# The SDK units the port replaces (sdk-replaced-units.txt) compile with the game but are not linked.
find "$build/CMakeFiles/halo3_game.dir" "$build/CMakeFiles/halo3_runtime.dir" "$build/CMakeFiles/halo3_data.dir" \
  "$build/CMakeFiles/halo3_gpu.dir" "$build/CMakeFiles/halo3_sdk.dir" -name '*.obj' -o -name '*.o' \
  | grep -v '/CMakeFiles/[^/]*\.dir/CMakeC' \
  | awk -v game="$build/CMakeFiles/halo3_game.dir/" '
      FNR == NR { if ($0 !~ /^#/ && NF) units["/" $1] = 1; next }
      index($0, game) == 1 {
        unit = $0; sub(/\.(o|obj)$/, "", unit)
        for (u in units) if (substr(unit, length(unit) - length(u) + 1) == u) next
      }
      { print }' "$port/build/sdk-replaced-units.txt" - \
  | sort > "$out/objects.txt"
echo "link-stage1: $(wc -l < "$out/objects.txt") objects ($target/$arch;" \
  "$(grep -c '^tu/' "$port/build/sdk-replaced-units.txt") SDK units replaced by the port left out)"

case $target in
windows)
  if [ "$arch" = x86_64 ]; then
    mingw=${MINGW64_LIB:-}
    if [ -z "$mingw" ]; then
      for d in /opt/mingw64/lib /usr/x86_64-w64-mingw32/sys-root/mingw/lib /usr/x86_64-w64-mingw32/lib /usr/lib/x86_64-w64-mingw32; do
        if [ -f "$d/libmsvcrt.a" ]; then mingw=$d; break; fi
      done
    fi
    link() {  # link OUTPUT EXTRA...: msvcrt.dll and kernel32.dll through MinGW-w64 64-bit import libraries
      local output=$1; shift
      lld-link /nologo /machine:x64 /subsystem:console /entry:port_entry /nodefaultlib /demangle:no \
        /force:multiple /base:0x00400000 /dynamicbase:no /highentropyva:no /debug /out:"$output" @"$objects" "$@" \
        "$mingw/libmsvcrt.a" "$mingw/libmingwex.a" "$mingw/libkernel32.a"
    }
    format=coff64; exe=$build/halo3.exe; stubs=$out/stubs.obj; stub_target=x86_64-pc-windows-msvc
    objects=$out/objects.txt
    trial_flags=(/force:unresolved /errorlimit:0)
  else
    mingw=${MINGW_LIB:-/usr/i686-w64-mingw32/sys-root/mingw/lib}
    link() {  # link OUTPUT EXTRA...: msvcrt.dll and kernel32.dll through MinGW's import libraries
      local output=$1; shift
      lld-link /nologo /machine:x86 /subsystem:console /entry:port_entry /nodefaultlib /safeseh:no /demangle:no \
        /force:multiple /largeaddressaware /debug /out:"$output" @"$objects" "$@" "$mingw/libmsvcrt.a" "$mingw/libmingwex.a" "$mingw/libkernel32.a"
    }
    format=coff; exe=$build/halo3.exe; stubs=$out/stubs.obj; stub_target=i686-pc-windows-msvc
    objects=$out/objects.txt
    trial_flags=(/force:unresolved /errorlimit:0)
    # Win32 calls forwarded verbatim to the host.
    "$stubgen" forward "$port/sdk/kernel/win32_forward.txt" > "$out/forward.s"
    clang --target=$stub_target -c "$out/forward.s" -o "$out/forward.obj"
    echo "$out/forward.obj" >> "$out/objects.txt"
  fi
  ;;
linux)
  # MS-mangled names carry `@`, which ELF linkers read as symbol versions: link renamed copies (elf-names.sh).
  # On x86_64 Linux, game/SDK objects use Win64 ms_abi so their libc/libm calls are redirected to __ms_* wrappers.
  mkdir -p "$out/elf"
  if [ "$arch" = x86_64 ]; then
    ms_map=$port/sdk/runtime/ms_libc_linux64.map
    sed "s|^$build/CMakeFiles/||" "$out/objects.txt" | grep -E '^(halo3_game|halo3_sdk)\.dir/' \
      | xargs -P8 -I{} "$port/build/elf-names.sh" "$build/CMakeFiles/{}" "$out/elf/{}" "$ms_map"
    sed "s|^$build/CMakeFiles/||" "$out/objects.txt" | grep -vE '^(halo3_game|halo3_sdk)\.dir/' \
      | xargs -P8 -I{} "$port/build/elf-names.sh" "$build/CMakeFiles/{}" "$out/elf/{}"
  else
    sed "s|^$build/CMakeFiles/||" "$out/objects.txt" \
      | xargs -P8 -I{} "$port/build/elf-names.sh" "$build/CMakeFiles/{}" "$out/elf/{}"
  fi
  sed "s|^$build/CMakeFiles/|$out/elf/|" "$out/objects.txt" > "$out/objects-elf.txt"
  # The executable exports nothing. lld puts every executable definition a shared library refers to into .dynsym,
  # so the game's own CRT (time, rand, vsprintf, wcs*, sinf, ...) would interpose on libc for every library the
  # port dlopens (libvulkan, the ICD, libX11). The one export glibc needs is _IO_stdin_used: on i386 libc reads it
  # to choose the current stdio ABI over the libc5-era one.
  printf '{\n  global: _IO_stdin_used;\n  local: *;\n};\n' > "$out/hide.ver"
  if [ "$arch" = x86_64 ]; then
    stub_target=x86_64-linux-gnu
    format=elf64
  else
    stub_target=i686-linux-gnu
    format=elf
  fi
  link() {  # link OUTPUT EXTRA...: glibc (libdl: the Vulkan loader, port/sdk/runtime/vulkan_host_linux.c); the
            # wrapped main is port/sdk/runtime/entry_linux.c
    local output=$1; shift
    clang --target="$stub_target" -fuse-ld=lld -no-pie -o "$output" @"$objects" "$@" -lpthread -lm -ldl \
      -Wl,--allow-multiple-definition,--no-dependent-libraries,--wrap=main -Wl,--version-script="$out/hide.ver"
  }
  exe=$build/halo3; stubs=$out/stubs.o
  objects=$out/objects-elf.txt
  trial_flags=(-Wl,--error-limit=0)
  ;;
*)
  echo "link-stage1: unknown target $target" >&2; exit 2 ;;
esac

# 1. Trial link.
link "$out/trial" "${trial_flags[@]}" > "$out/trial.log" 2>&1 || true
grep -o 'undefined symbol: .*' "$out/trial.log" | sed 's/undefined symbol: //' | sort -u > "$out/undefined.txt"
echo "link-stage1: $(wc -l < "$out/undefined.txt") undefined symbols"

# 2. Stubs.
xargs -a "$objects" llvm-objdump -r > "$out/relocs.txt"
xargs -a "$objects" llvm-nm --defined-only --extern-only --format=just-symbols 2>/dev/null | sort -u > "$out/defined.txt"
if [ "$target" = linux ]; then
  # Host functions glibc defines with another meaning (host-deny-linux.txt) are stubbed where the game refers to them,
  # unless a port object defines the game's meaning (port/sdk/online/winsock_offline.cpp): that definition binds.
  awk 'FNR == NR { if ($0 !~ /^#/ && NF) deny[$1] = 1; next } NF >= 3 && ($3 in deny) { print $3 }' \
    "$port/sdk/runtime/host-deny-linux.txt" "$out/relocs.txt" | sort -u | comm -23 - "$out/defined.txt" > "$out/denied.txt"
  sort -u "$out/undefined.txt" "$out/denied.txt" -o "$out/undefined.txt"
  echo "link-stage1: $(wc -l < "$out/denied.txt") host functions denied"
fi
# The first stubgen run lists the stubs (stubs.tsv); datagen surveys the data among them in the retail image, so a
# datum is judged by the name this tree gives it; the second run poisons the data with retail values.
owners=$port/../metadata/matching/data-owners.tsv
"$stubgen" "$format" "$out/undefined.txt" "$out/relocs.txt" "$owners" "$out/defined.txt" "$out/aliases.ld" "$out/stubs.tsv" \
  > /dev/null 2> "$out/stubgen-list.log"
awk -F'\t' '$1 == "data" { print $2 }' "$out/stubs.tsv" > "$out/data-stubs.txt"
"$build/tools/datagen" survey "$out/data-stubs.txt" "$retail" "$port/.." > "$out/retail-data.tsv" 2> "$out/retail-data.log"
"$stubgen" "$format" "$out/undefined.txt" "$out/relocs.txt" "$owners" "$out/defined.txt" "$out/aliases.ld" "$out/stubs.tsv" \
  "$out/retail-data.tsv" > "$out/stubs.s"
clang --target=$stub_target -c "$out/stubs.s" -o "$stubs"

# 3. Final link.
if [ "$target" = linux ]; then
  link "$exe" "$stubs" "$out/aliases.ld" -Wl,-Map="$out/halo3.map" > "$out/link.log" 2>&1 || { tail -20 "$out/link.log"; exit 1; }
else
  link "$exe" "$stubs" /map:"$out/halo3.map" > "$out/link.log" 2>&1 || { tail -20 "$out/link.log"; exit 1; }
fi
echo "link-stage1: $exe"
