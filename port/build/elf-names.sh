#!/usr/bin/env bash
# port/build/elf-names.sh OBJECT OUTPUT (glue): copy an ELF object with every `@` in its symbol names replaced by `!`.
# The game's C++ names are MS-mangled (`?name@@YAXXZ`), and ELF linkers read `name@@version` as a symbol version: lld
# would bind every `?f@@...` overload to one symbol `?f`. MS mangling never uses `!`, so the rename is one-to-one;
# port/tools/stubgen reads `!` back as `@`. Skips the copy when OUTPUT is newer than OBJECT.
set -euo pipefail
object=$1 output=$2 extra_map=${3:-}
if [ -n "$extra_map" ]; then
  [ "$output" -nt "$object" ] && [ "$output" -nt "$extra_map" ] && exit 0
else
  [ "$output" -nt "$object" ] && exit 0
fi
mkdir -p "$(dirname "$output")"
# One "old new" line per name that has an `@` (llvm-objcopy's --redefine-syms format).
llvm-nm --format=just-symbols "$object" 2>/dev/null | grep -F '@' | sort -u \
  | awk '{ old = $0; gsub("@", "!"); print old " " $0 }' > "$output.map" || true
if [ -n "$extra_map" ]; then
  cat "$extra_map" >> "$output.map"
fi
llvm-objcopy --redefine-syms="$output.map" "$object" "$output"
