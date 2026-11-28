#!/usr/bin/env bash
# Builds Halo 3 PC port artifacts inside the pinned multi-stage container image
# (port/build/container/Containerfile: Debian 11 glibc 2.31 + LLVM/Clang/LLD 22.1.8 +
# MinGW-w64 13.0.0 + static Odin + static zlib 1.3.1 + AppImage runtime + Flatpak + NSIS).
#
# Usage:
#   port/build/container/build.sh [target] [ninja arguments...]
#
# Targets:
#   linux        (default) Playable 0-stub 64-bit x86_64 Linux binary -> build-pc/lin64/probe/halo3_probe
#   windows      Playable 0-stub 64-bit x86_64 Windows binary         -> build-pc/win64/probe/halo3_probe.exe
#   appimage     Release 64-bit portable Linux AppImage + .sha256     -> build-pc/dist/Halo3-x86_64.AppImage
#   flatpak      Release 64-bit Linux Flatpak bundle + .sha256        -> build-pc/dist/Halo3-x86_64.flatpak
#   windows-exe  Release 64-bit Windows per-user NSIS installer       -> build-pc/dist/Halo3-Setup.exe
#   windows-zip  Release 64-bit Windows portable zip + .sha256        -> build-pc/dist/Halo3-Windows-Portable.zip
#   release      Build & verify ALL 4 release packages (AppImage, Flatpak, Windows Setup EXE, Windows Portable ZIP)
#   shell        Open an interactive shell inside the pinned build image
#
# Environment:
#   HALO3_RETAIL_PE          Expanded retail default.pe (default: auto-detected or unpacked from HALO3_DISC / local/h3disc)
#   HALO3_DISC               Extracted retail Halo 3 disc dir containing default.xex (fallback if HALO3_RETAIL_PE unset)
#   HALO3_VERSION            Version string stamped into packages (default: git describe --tags --always)
#   FLATPAK_RUNTIME_VERSION  Freedesktop Platform branch for Flatpak (default: 25.08)
#   BUILD_TYPE               CMake build type (default: RelWithDebInfo for dev targets, Release for packages)
#   JOBS                     Parallel build jobs (default: 8)
#   CLEAN                    Set CLEAN=1 to compile from a completely fresh ephemeral build dir inside the container
#   OUT_DIR                  Output directory relative to repo root for release packages (default: build-pc/dist)
#   SOURCE_DATE_EPOCH        Unix timestamp for deterministic archives (default: HEAD commit timestamp)
#   ENGINE                   podman or docker (default: podman if installed, else docker)
#   IMAGE                    Container image tag (default: localhost/halo3-pc-build:debian11-llvm22)
set -euo pipefail

here=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
repo=$(cd "$here/../../.." && pwd)
target=${1:-linux}
[[ $# -gt 0 ]] && shift

case $target in
  linux|windows|appimage|flatpak|windows-exe|windows-zip|release|all|shell) ;;
  -h|--help)
    sed -n '2,30p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'
    exit 0
    ;;
  *)
    echo "build.sh: unknown target '$target' (expected: linux, windows, appimage, flatpak, windows-exe, windows-zip, release, shell)" >&2
    exit 2
    ;;
esac
[[ $target == all ]] && target=release

engine=${ENGINE:-}
if [[ -z $engine ]]; then
  if command -v podman >/dev/null 2>&1; then engine=podman; else engine=docker; fi
fi
command -v "$engine" >/dev/null 2>&1 || { echo "build.sh: $engine is not installed" >&2; exit 1; }

image=${IMAGE:-localhost/halo3-pc-build:debian11-llvm22}
jobs=${JOBS:-8}
clean=${CLEAN:-0}
out_dir=${OUT_DIR:-build-pc/dist}
fp_rt=${FLATPAK_RUNTIME_VERSION:-25.08}
version=${HALO3_VERSION:-$(git -C "$repo" describe --tags --always --dirty 2>/dev/null || echo dev)}
source_date_epoch=${SOURCE_DATE_EPOCH:-$(git -C "$repo" log -1 --format=%ct 2>/dev/null || echo 1700000000)}

if [[ -n ${BUILD_TYPE:-} ]]; then
  build_type=$BUILD_TYPE
else
  case $target in
    appimage|flatpak|windows-exe|windows-zip|release) build_type=Release ;;
    *) build_type=RelWithDebInfo ;;
  esac
fi

echo "==> image $image ($engine)"
"$engine" build -q -t "$image" -f "$here/Containerfile" "$here" >/dev/null

# Keep output files owned by the host user, not root.
if [[ $engine == podman ]]; then
  user=(--userns=keep-id)
else
  user=(--user "$(id -u):$(id -g)")
fi

if [[ $target == shell ]]; then
  exec "$engine" run --rm -it "${user[@]}" -e HOME=/tmp -v "$repo:/src:Z" -w /src "$image" bash
fi

# Locate retail default.pe, or fall back to default.xex from the retail disc (unpacked inside the container).
retail=${HALO3_RETAIL_PE:-}
xex=""
if [[ -z $retail ]]; then
  for cand in "$repo/local/reference/default.pe" "$repo/build-pc/default.pe"; do
    if [[ -f $cand ]]; then retail=$cand; break; fi
  done
fi
if [[ -n $retail && ! -f $retail ]]; then
  echo "build.sh: HALO3_RETAIL_PE not found: $retail" >&2
  exit 1
fi
if [[ -z $retail ]]; then
  disc=${HALO3_DISC:-$repo/local/h3disc}
  if [[ -f "$disc/default.xex" ]]; then
    xex=$(cd "$(dirname "$disc/default.xex")" && pwd)/default.xex
  else
    echo "build.sh: no retail default.pe or disc default.xex found." >&2
    echo "          Set HALO3_RETAIL_PE=/path/to/default.pe or HALO3_DISC=/path/to/halo3-disc." >&2
    exit 1
  fi
fi

mounts=(-v "$repo:/src:Z")
if [[ -n $retail ]]; then
  retail=$(cd "$(dirname "$retail")" && pwd)/$(basename "$retail")
  retail_sha=$(sha256sum "$retail" | cut -d' ' -f1)
  mounts+=(-v "$retail:/retail/default.pe:ro,Z")
  echo "==> target=$target build_type=$build_type jobs=$jobs version=$version clean=$clean"
  echo "    retail PE: $retail (sha256 $retail_sha)"
else
  xex_sha=$(sha256sum "$xex" | cut -d' ' -f1)
  mounts+=(-v "$xex:/retail/default.xex:ro,Z")
  echo "==> target=$target build_type=$build_type jobs=$jobs version=$version clean=$clean"
  echo "    retail XEX: $xex (sha256 $xex_sha; will unpack default.pe inside container)"
fi

"$engine" run --rm "${user[@]}" \
  -e HOME=/tmp \
  -e TZ=UTC \
  -e LC_ALL=C.UTF-8 \
  -e SOURCE_DATE_EPOCH="$source_date_epoch" \
  "${mounts[@]}" \
  -w /src \
  "$image" bash -euo pipefail -c '
    target=$1 build_type=$2 jobs=$3 clean=$4 out_dir=$5 version=$6 fp_rt=$7; shift 7

    # Resolve /retail/default.pe (unpack from /retail/default.xex if needed).
    retail_pe=/retail/default.pe
    if [[ ! -f $retail_pe ]]; then
      mkdir -p /tmp/halo3-xex
      odin build /src/port/tools/xex2pe -out:/tmp/halo3-xex/xex2pe -o:speed
      /tmp/halo3-xex/xex2pe /retail/default.xex /tmp/halo3-xex/default.pe
      retail_pe=/tmp/halo3-xex/default.pe
    fi
    echo "    using retail PE sha256=$(sha256sum "$retail_pe" | cut -d" " -f1)"

    xe_abi=$(awk "/^#define XE_ABI_VERSION/ {print \$3}" /src/port/shaders/xenos_spirv_abi.h)
    xenos_rev=$(LC_ALL=C cat /src/port/tools/xenos_spirv/*.odin /src/port/tools/shaderpack/xenos.odin | sha256sum | cut -c1-8)
    shader_abi="${xe_abi}-${xenos_rev}"
    iso_ts=$(date -u -d "@$SOURCE_DATE_EPOCH" +"%Y-%m-%dT%H:%M:%SZ")

    # -------------------------------------------------------------------------
    # Icon rasterization (PNGs for Flatpak/Linux + multi-res .ico for Windows)
    # -------------------------------------------------------------------------
    ICONS_READY=0
    ICON_DIR=/tmp/halo3-icons
    ensure_icons() {
      if (( ICONS_READY )); then return 0; fi
      mkdir -p "$ICON_DIR"
      for sz in 16 32 48 64 128 256 512; do
        rsvg-convert -w "$sz" -h "$sz" /src/port/packaging/halo3.svg -o "$ICON_DIR/halo3_${sz}.png"
      done
      odin build /src/port/tools/png2ico -out:"$ICON_DIR/png2ico" -o:speed
      "$ICON_DIR/png2ico" "$ICON_DIR/halo3.ico" \
        "$ICON_DIR/halo3_16.png" "$ICON_DIR/halo3_32.png" \
        "$ICON_DIR/halo3_48.png" "$ICON_DIR/halo3_64.png" \
        "$ICON_DIR/halo3_128.png" "$ICON_DIR/halo3_256.png"
      ICONS_READY=1
    }

    # -------------------------------------------------------------------------
    # Core Engine & Tool Builders
    # -------------------------------------------------------------------------
    configure_and_build_linux64() {
      local bdir=$1 odir=$2 repro=$3; shift 3
      mkdir -p "$bdir" "$odir"
      local extra_cmake=()
      if [[ $repro == 1 ]]; then
        extra_cmake+=(-DHALO3_REPRODUCIBLE=ON)
      fi
      cmake -S /src/port/build -B "$bdir" -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE=/src/port/build/toolchains/x86_64-linux-gnu.cmake \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DHALO3_RETAIL_PE="$retail_pe" \
        "${extra_cmake[@]}" \
        > "$bdir/configure.log" 2>&1 \
        || { tail -40 "$bdir/configure.log"; exit 1; }
      echo "==> ninja (linux x86_64, $build_type, -j$jobs)"
      ninja -C "$bdir" -j "$jobs" "$@"
      echo "==> probe64 (0-stub playable x86_64 Linux binary)"
      PROBE64_BUILD="$bdir" PROBE64_OUT="$odir" bash /src/port/probe/probe64.sh
    }

    configure_and_build_windows() {
      local bdir=$1 odir=$2 repro=$3 dbg=$4 res_file=$5; shift 5
      mkdir -p "$bdir" "$odir"
      local extra_cmake=()
      if [[ $repro == 1 ]]; then
        extra_cmake+=(-DHALO3_REPRODUCIBLE=ON)
      fi
      cmake -S /src/port/build -B "$bdir" -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE=/src/port/build/toolchains/x86_64-windows-msvc.cmake \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DHALO3_RETAIL_PE="$retail_pe" \
        -DCMAKE_RC_COMPILER=llvm-rc \
        "${extra_cmake[@]}" \
        > "$bdir/configure.log" 2>&1 \
        || { tail -40 "$bdir/configure.log"; exit 1; }
      echo "==> ninja (windows x86_64, $build_type, -j$jobs)"
      ninja -C "$bdir" -j "$jobs" "$@"
      echo "==> winprobe (0-stub playable 64-bit Windows binary, debug=$dbg)"
      WINPROBE_BUILD="$bdir" WINPROBE_OUT="$odir" WINPROBE_DEBUG="$dbg" WINPROBE_RES="$res_file" \
        bash /src/port/probe/winprobe.sh
    }

    stage_fonts() {
      local dest=$1
      local src=/src/port/data/fonts
      install -Dm644 "$src/halo3.otf" "$dest/halo3.otf"
      if [[ -f "$src/D-DIN.ttf" ]]; then
        install -m644 "$src/D-DIN.ttf" "$src/COPYING.txt" "$dest/"
      fi
    }

    build_linux_tools_and_launcher() {
      local tdir=$1
      mkdir -p "$tdir"
      local launcher_defs=()
      if [[ $build_type == Release ]]; then
        launcher_defs+=(-DHALO3_RELEASE_BUILD=1)
      fi
      echo "==> Building 64-bit Linux tools & GUI launcher (halo3-launcher, xiso, xex2pe, shaderpack, extract-odst-dlc)..."
      clang --target=x86_64-linux-gnu -O2 -Wall -Wextra "${launcher_defs[@]}" \
        -I/usr/include/freetype2 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include \
        -fuse-ld=lld -Wl,--build-id=sha1 \
        /src/port/packaging/launcher_linux.c -o "$tdir/halo3-launcher" -lpthread -lm -ldl
      clang --target=x86_64-linux-gnu -O2 -Wall -Wextra \
        -fuse-ld=lld -Wl,--build-id=sha1 \
        /src/port/tools/extract-odst-dlc/extract_odst_dlc.c -o "$tdir/extract-odst-dlc"
      for t in xiso xex2pe; do
        odin build "/src/port/tools/$t" -out:"$tdir/$t" -o:speed \
          -extra-linker-flags:"-fuse-ld=lld -Wl,--build-id=sha1"
      done
      # Link shaderpack against static /opt/static-zlib/linux/libz.a (zero libz.so.1 runtime dependency).
      odin build /src/port/tools/shaderpack -build-mode:obj -out:"$tdir/shaderpack.o" -o:speed
      clang --target=x86_64-linux-gnu -fuse-ld=lld -Wl,--build-id=sha1 \
        -o "$tdir/shaderpack" "$tdir/shaderpack.o" "$HALO3_STATIC_ZLIB_LINUX" -lpthread -lm -ldl
    }

    build_windows_tools_and_launcher() {
      local wdir=$1 res_launcher=$2
      mkdir -p "$wdir"
      local launcher_defs=()
      if [[ $build_type == Release ]]; then
        launcher_defs+=(-DHALO3_RELEASE_BUILD=1)
      fi
      echo "==> Cross-compiling 64-bit Windows tools & GUI launcher (Halo3.exe, xiso.exe, xex2pe.exe, shaderpack.exe, extract-odst-dlc.exe)..."
      clang --target=x86_64-pc-windows-msvc -c /src/port/packaging/win64_crt_glue.s -o "$wdir/win64_crt_glue.obj"

      clang --target=x86_64-w64-mingw32 --sysroot=/opt/mingw64 -isystem /opt/mingw64/include \
        -O2 -Wall -Wextra "${launcher_defs[@]}" -c /src/port/packaging/launcher_windows.c -o "$wdir/launcher_windows.obj"
      lld-link /nologo /machine:x64 /subsystem:windows /entry:WinMainCRTStartup /nodefaultlib /Brepro \
        /out:"$wdir/Halo3.exe" "$wdir/launcher_windows.obj" "$wdir/win64_crt_glue.obj" "$res_launcher" \
        "$MINGW64_LIB/crt2.o" "$MINGW64_LIB/libmingw32.a" "$MINGW64_LIB/libmsvcrt.a" "$MINGW64_LIB/libmingwex.a" \
        "$MINGW64_LIB/libkernel32.a" "$MINGW64_LIB/libuser32.a" "$MINGW64_LIB/libgdi32.a" \
        "$MINGW64_LIB/libshell32.a" "$MINGW64_LIB/libcomdlg32.a" "$MINGW64_LIB/libole32.a" "$MINGW64_LIB/libadvapi32.a"

      clang --target=x86_64-w64-mingw32 --sysroot=/opt/mingw64 -isystem /opt/mingw64/include \
        -O2 -Wall -Wextra -c /src/port/tools/extract-odst-dlc/extract_odst_dlc.c -o "$wdir/extract_odst_dlc.obj"
      lld-link /nologo /machine:x64 /subsystem:console /entry:mainCRTStartup /nodefaultlib /Brepro \
        /out:"$wdir/extract-odst-dlc.exe" "$wdir/extract_odst_dlc.obj" "$wdir/win64_crt_glue.obj" \
        "$MINGW64_LIB/crt2.o" "$MINGW64_LIB/libmingw32.a" "$MINGW64_LIB/libmsvcrt.a" "$MINGW64_LIB/libmingwex.a" \
        "$MINGW64_LIB/libkernel32.a"

      for t in xiso xex2pe shaderpack; do
        odin build "/src/port/tools/$t" -target:windows_amd64 -build-mode:obj -out:"$wdir/$t.obj" -o:speed
      done
      link_win64_tool() {
        local name=$1; shift
        lld-link /nologo /machine:x64 /subsystem:console /entry:mainCRTStartup /nodefaultlib /force:multiple /Brepro \
          /out:"$wdir/$name.exe" "$wdir/$name.obj" "$wdir/win64_crt_glue.obj" "$@" \
          "$MINGW64_LIB/crt2u.o" "$MINGW64_LIB/libmingw32.a" "$MINGW64_LIB/libmsvcrt.a" "$MINGW64_LIB/libmingwex.a" \
          "$MINGW64_LIB/libkernel32.a" "$MINGW64_LIB/libntdll.a" "$MINGW64_LIB/libshell32.a" "$MINGW64_LIB/libadvapi32.a" \
          "$MINGW64_LIB/libuser32.a" "$MINGW64_LIB/libbcrypt.a" "$MINGW64_LIB/libsynchronization.a"
      }
      link_win64_tool xiso
      link_win64_tool xex2pe
      link_win64_tool shaderpack "$HALO3_STATIC_ZLIB_WIN64"
    }

    # -------------------------------------------------------------------------
    # Shared Verification Gates
    # -------------------------------------------------------------------------
    verify_linux_binaries() {
      local bindir=$1 stubs_tsv=$2
      echo "==> Verifying 64-bit Linux binaries (0 stubs, GLIBC <= 2.31, clean DT_NEEDED)..."
      local fn_stubs
      fn_stubs=$(awk -F"\t" "\$1 == \"function\" { c++ } END { print c+0 }" "$stubs_tsv")
      if (( fn_stubs != 0 )); then
        echo "VERIFY FAIL: $fn_stubs function stubs in $stubs_tsv" >&2
        exit 1
      fi
      for b in halo3 halo3-launcher xiso xex2pe shaderpack extract-odst-dlc; do
        local p="$bindir/$b"
        local ftype
        ftype=$(file "$p")
        if [[ $ftype != *"ELF 64-bit LSB"*"x86-64"* ]]; then
          echo "VERIFY FAIL: $b is not ELF64 x86-64 ($ftype)" >&2
          exit 1
        fi
        local max_glibc
        max_glibc=$(llvm-objdump -T "$p" | grep -oE "GLIBC_[0-9.]+" | sort -Vu | tail -n 1 || true)
        if [[ -n $max_glibc ]]; then
          local highest
          highest=$(printf "%s\nGLIBC_2.31\n" "$max_glibc" | sort -Vu | tail -n 1)
          if [[ $highest != "GLIBC_2.31" ]]; then
            echo "VERIFY FAIL: $b requires $max_glibc (exceeds GLIBC_2.31 ceiling)" >&2
            exit 1
          fi
        fi
        local bad_needed
        bad_needed=$(readelf -d "$p" | awk "/\(NEEDED\)/ { gsub(/^.*\[|\].*$/, \"\"); if (\$0 != \"libc.so.6\" && \$0 != \"libm.so.6\" && \$0 != \"ld-linux-x86-64.so.2\" && \$0 != \"libpthread.so.0\" && \$0 != \"libdl.so.2\") print }")
        if [[ -n $bad_needed ]]; then
          echo "VERIFY FAIL: $b has unexpected DT_NEEDED: $bad_needed" >&2
          exit 1
        fi
        echo "    [ok] $b: ELF64 x86-64, max=${max_glibc:-none}, clean DT_NEEDED"
      done
    }

    verify_windows_binaries() {
      local stage=$1 stubs_tsv=$2
      echo "==> Verifying 64-bit Windows binaries (0 stubs, PE32+ x86-64, zero third-party DLLs, GUI launcher)..."
      local fn_stubs
      fn_stubs=$(awk -F"\t" "\$1 == \"function\" { c++ } END { print c+0 }" "$stubs_tsv")
      if (( fn_stubs != 0 )); then
        echo "VERIFY FAIL: $fn_stubs function stubs in $stubs_tsv" >&2
        exit 1
      fi
      if [[ -e "$stage/bin/zlib1.dll" ]]; then
        echo "VERIFY FAIL: zlib1.dll should not be needed (static zlib)" >&2
        exit 1
      fi
      local ltype
      ltype=$(file -b "$stage/Halo3.exe")
      if [[ $ltype != *"PE32+ executable (GUI) x86-64"* ]]; then
        echo "VERIFY FAIL: Halo3.exe must be PE32+ GUI x86-64 (got: $ltype)" >&2
        exit 1
      fi
      local allowed_dlls="KERNEL32.dll msvcrt.dll ADVAPI32.dll bcrypt.dll ntdll.dll USER32.dll SHELL32.dll GDI32.dll comdlg32.dll ole32.dll"
      for rel in Halo3.exe bin/halo3.exe bin/xiso.exe bin/xex2pe.exe bin/shaderpack.exe bin/extract-odst-dlc.exe; do
        local p="$stage/$rel"
        local ptype
        ptype=$(file -b "$p")
        if [[ $ptype != *"PE32+ executable"*"x86-64"* ]]; then
          echo "VERIFY FAIL: $rel must be 64-bit PE32+ x86-64 (got: $ptype)" >&2
          exit 1
        fi
        local imports bad=""
        imports=$(llvm-readobj --coff-imports "$p" | awk "/^[[:space:]]*Name:/ { print \$2 }" | sort -u)
        for dll in $imports; do
          case " $allowed_dlls " in
            *" $dll "*) ;;
            *) bad="$bad $dll" ;;
          esac
        done
        if [[ -n $bad ]]; then
          echo "VERIFY FAIL: $rel has non-system DLL imports:$bad" >&2
          exit 1
        fi
        echo "    [ok] $rel: $(echo "$ptype" | cut -d, -f1-2), imports=[$(echo $imports)]"
      done
    }

    # -------------------------------------------------------------------------
    # Shared Linux Stage (built once, packaged into BOTH AppImage and Flatpak)
    # -------------------------------------------------------------------------
    LIN_STAGE_READY=0
    LIN_BIN_STAGE=/tmp/halo3-lin-stage/bin
    LIN_STUBS_TSV=""
    ensure_linux_stage() {
      if (( LIN_STAGE_READY )); then return 0; fi
      ensure_icons
      local base_root="/src/build-pc/release"
      if [[ $clean == 1 ]]; then base_root="/tmp/halo3-clean-lin64"; fi
      local bdir="$base_root/lin64"
      local odir="$base_root/lin64-probe"
      local tdir="/tmp/halo3-lin-tools"

      configure_and_build_linux64 "$bdir" "$odir" 1 "$@"
      build_linux_tools_and_launcher "$tdir"

      mkdir -p "$LIN_BIN_STAGE"
      cp "$odir/halo3_probe"       "$LIN_BIN_STAGE/halo3"
      cp "$tdir/halo3-launcher"    "$LIN_BIN_STAGE/halo3-launcher"
      cp "$tdir/xiso"              "$LIN_BIN_STAGE/xiso"
      cp "$tdir/xex2pe"            "$LIN_BIN_STAGE/xex2pe"
      cp "$tdir/shaderpack"        "$LIN_BIN_STAGE/shaderpack"
      cp "$tdir/extract-odst-dlc"  "$LIN_BIN_STAGE/extract-odst-dlc"
      llvm-strip --strip-unneeded \
        "$LIN_BIN_STAGE/halo3" "$LIN_BIN_STAGE/halo3-launcher" \
        "$LIN_BIN_STAGE/xiso" "$LIN_BIN_STAGE/xex2pe" \
        "$LIN_BIN_STAGE/shaderpack" "$LIN_BIN_STAGE/extract-odst-dlc"
      chmod 0755 "$LIN_BIN_STAGE/halo3" "$LIN_BIN_STAGE/halo3-launcher" \
                 "$LIN_BIN_STAGE/xiso" "$LIN_BIN_STAGE/xex2pe" \
                 "$LIN_BIN_STAGE/shaderpack" "$LIN_BIN_STAGE/extract-odst-dlc"

      LIN_STUBS_TSV="$odir/stage1/stubs.tsv"
      verify_linux_binaries "$LIN_BIN_STAGE" "$LIN_STUBS_TSV"
      LIN_STAGE_READY=1
    }

    package_appimage() {
      ensure_linux_stage "$@"
      local appdir="/tmp/halo3-appdir"
      local dist="/src/$out_dir"
      mkdir -p "$dist" "$appdir/usr/bin" "$appdir/usr/share/halo3"

      cp "$LIN_BIN_STAGE/halo3"            "$appdir/usr/bin/halo3"
      cp "$LIN_BIN_STAGE/halo3-launcher"   "$appdir/usr/bin/halo3-launcher"
      cp "$LIN_BIN_STAGE/xiso"             "$appdir/usr/bin/xiso"
      cp "$LIN_BIN_STAGE/xex2pe"           "$appdir/usr/bin/xex2pe"
      cp "$LIN_BIN_STAGE/shaderpack"       "$appdir/usr/bin/shaderpack"
      cp "$LIN_BIN_STAGE/extract-odst-dlc" "$appdir/usr/bin/extract-odst-dlc"
      if [[ -f /src/port/data/pipeline_keys.bin ]]; then
        cp /src/port/data/pipeline_keys.bin "$appdir/usr/bin/pipeline_keys.bin"
        chmod 0644 "$appdir/usr/bin/pipeline_keys.bin"
      fi

      stage_fonts "$appdir/usr/bin/fonts"

      printf "%s\n" "$version"               > "$appdir/usr/share/halo3/VERSION"
      printf "%s\n" "$shader_abi"            > "$appdir/usr/share/halo3/SHADER_ABI"
      cp /etc/halo3-build-manifest           "$appdir/usr/share/halo3/BUILD_MANIFEST"
      cp /src/port/packaging/AppRun          "$appdir/AppRun"
      cp /src/port/packaging/halo3.desktop   "$appdir/halo3.desktop"
      cp /src/port/packaging/halo3.svg       "$appdir/halo3.svg"
      ln -sf halo3.svg                       "$appdir/.DirIcon"

      chmod 0755 "$appdir" "$appdir/usr" "$appdir/usr/bin" "$appdir/usr/share" "$appdir/usr/share/halo3" \
                 "$appdir/AppRun" "$appdir/usr/bin/halo3" "$appdir/usr/bin/halo3-launcher" \
                 "$appdir/usr/bin/xiso" "$appdir/usr/bin/xex2pe" \
                 "$appdir/usr/bin/shaderpack" "$appdir/usr/bin/extract-odst-dlc"
      chmod 0644 "$appdir/halo3.desktop" "$appdir/halo3.svg" \
                 "$appdir/usr/share/halo3/VERSION" "$appdir/usr/share/halo3/SHADER_ABI" \
                 "$appdir/usr/share/halo3/BUILD_MANIFEST"
      desktop-file-validate "$appdir/halo3.desktop"
      find "$appdir" -exec touch -h -d "@$SOURCE_DATE_EPOCH" {} +

      echo "==> Packing deterministic AppImage (zstd SquashFS + pinned type2-runtime)..."
      env -u SOURCE_DATE_EPOCH mksquashfs "$appdir" /tmp/halo3-app.squashfs \
        -comp zstd -Xcompression-level 19 \
        -reproducible -all-root -no-xattrs -noappend -processors 1 \
        -mkfs-time "$SOURCE_DATE_EPOCH" -all-time "$SOURCE_DATE_EPOCH" \
        -quiet
      local out_img="$dist/Halo3-x86_64.AppImage"
      cat "$HALO3_APPIMAGE_RUNTIME" /tmp/halo3-app.squashfs > "$out_img"
      chmod 0755 "$out_img"
      touch -d "@$SOURCE_DATE_EPOCH" "$out_img"

      local ver_out
      ver_out=$("$out_img" --appimage-extract-and-run --version)
      echo "    [ok] AppImage --version: $ver_out"
      (cd "$dist" && sha256sum Halo3-x86_64.AppImage > Halo3-x86_64.AppImage.sha256)
      echo "==> Built $out_img ($(du -h "$out_img" | cut -f1))"
      cat "$dist/Halo3-x86_64.AppImage.sha256"
    }

    package_flatpak() {
      ensure_linux_stage "$@"
      local app_id="io.github.summers_aaron.Halo3"
      local fpdir="/tmp/halo3-fpdir"
      local fprepo="/tmp/halo3-fprepo"
      local dist="/src/$out_dir"
      mkdir -p "$dist" "$fpdir/files/bin" "$fpdir/files/share/halo3" \
               "$fpdir/files/share/applications" "$fpdir/files/share/metainfo" \
               "$fpdir/files/share/icons/hicolor/scalable/apps" "$fpdir/var"

      cat > "$fpdir/metadata" <<EOF
[Application]
name=${app_id}
runtime=org.freedesktop.Platform/x86_64/${fp_rt}
sdk=org.freedesktop.Sdk/x86_64/${fp_rt}
command=halo3-launcher
EOF

      cp "$LIN_BIN_STAGE/halo3"            "$fpdir/files/bin/halo3"
      cp "$LIN_BIN_STAGE/halo3-launcher"   "$fpdir/files/bin/halo3-launcher"
      cp "$LIN_BIN_STAGE/xiso"             "$fpdir/files/bin/xiso"
      cp "$LIN_BIN_STAGE/xex2pe"           "$fpdir/files/bin/xex2pe"
      cp "$LIN_BIN_STAGE/shaderpack"       "$fpdir/files/bin/shaderpack"
      cp "$LIN_BIN_STAGE/extract-odst-dlc" "$fpdir/files/bin/extract-odst-dlc"
      if [[ -f /src/port/data/pipeline_keys.bin ]]; then
        cp /src/port/data/pipeline_keys.bin "$fpdir/files/bin/pipeline_keys.bin"
        chmod 0644 "$fpdir/files/bin/pipeline_keys.bin"
      fi

      stage_fonts "$fpdir/files/bin/fonts"

      printf "%s\n" "$version"               > "$fpdir/files/share/halo3/VERSION"
      printf "%s\n" "$shader_abi"            > "$fpdir/files/share/halo3/SHADER_ABI"
      cp /etc/halo3-build-manifest           "$fpdir/files/share/halo3/BUILD_MANIFEST"

      cp "/src/port/packaging/${app_id}.desktop"      "$fpdir/files/share/applications/${app_id}.desktop"
      cp "/src/port/packaging/${app_id}.metainfo.xml" "$fpdir/files/share/metainfo/${app_id}.metainfo.xml"
      cp /src/port/packaging/halo3.svg                "$fpdir/files/share/icons/hicolor/scalable/apps/${app_id}.svg"
      for sz in 64 128 256 512; do
        mkdir -p "$fpdir/files/share/icons/hicolor/${sz}x${sz}/apps"
        cp "$ICON_DIR/halo3_${sz}.png" "$fpdir/files/share/icons/hicolor/${sz}x${sz}/apps/${app_id}.png"
        chmod 0755 "$fpdir/files/share/icons/hicolor/${sz}x${sz}" "$fpdir/files/share/icons/hicolor/${sz}x${sz}/apps"
        chmod 0644 "$fpdir/files/share/icons/hicolor/${sz}x${sz}/apps/${app_id}.png"
      done

      desktop-file-validate "$fpdir/files/share/applications/${app_id}.desktop"
      chmod 0755 "$fpdir" "$fpdir/files" "$fpdir/files/bin" "$fpdir/files/share" \
                 "$fpdir/files/share/halo3" "$fpdir/files/share/applications" "$fpdir/files/share/metainfo" \
                 "$fpdir/files/share/icons" "$fpdir/files/share/icons/hicolor" \
                 "$fpdir/files/share/icons/hicolor/scalable" "$fpdir/files/share/icons/hicolor/scalable/apps" \
                 "$fpdir/var" \
                 "$fpdir/files/bin/halo3" "$fpdir/files/bin/halo3-launcher" \
                 "$fpdir/files/bin/xiso" "$fpdir/files/bin/xex2pe" \
                 "$fpdir/files/bin/shaderpack" "$fpdir/files/bin/extract-odst-dlc"
      chmod 0644 "$fpdir/metadata" "$fpdir/files/share/halo3/VERSION" "$fpdir/files/share/halo3/SHADER_ABI" \
                 "$fpdir/files/share/halo3/BUILD_MANIFEST" \
                 "$fpdir/files/share/applications/${app_id}.desktop" \
                 "$fpdir/files/share/metainfo/${app_id}.metainfo.xml" \
                 "$fpdir/files/share/icons/hicolor/scalable/apps/${app_id}.svg"

      echo "==> Building deterministic Flatpak bundle ($app_id, runtime org.freedesktop.Platform/x86_64/$fp_rt)..."
      flatpak build-finish "$fpdir" \
        --share=ipc \
        --socket=wayland \
        --socket=x11 \
        --socket=pulseaudio \
        --device=all \
        --share=network \
        --talk-name=org.freedesktop.portal.Desktop \
        --env=HALO3_VULKAN=1 \
        --env=PORT_STUB_CONTINUE=1 >/dev/null

      find "$fpdir" -exec touch -h -d "@$SOURCE_DATE_EPOCH" {} +
      mkdir -p "$fprepo"
      ostree --repo="$fprepo" init --mode=archive-z2
      flatpak build-export --disable-sandbox --timestamp="$iso_ts" --no-update-summary "$fprepo" "$fpdir" >/dev/null
      local out_fp="$dist/Halo3-x86_64.flatpak"
      local ft_ts
      ft_ts=$(date -u -d "@$SOURCE_DATE_EPOCH" +"%Y-%m-%d %H:%M:%S")
      faketime -f "$ft_ts" flatpak build-bundle \
        --runtime-repo=https://flathub.org/repo/flathub.flatpakrepo \
        "$fprepo" "$out_fp" "$app_id" >/dev/null
      chmod 0644 "$out_fp"
      touch -d "@$SOURCE_DATE_EPOCH" "$out_fp"

      echo "    [ok] Flatpak exported metadata & desktop entry verified"
      (cd "$dist" && sha256sum Halo3-x86_64.flatpak > Halo3-x86_64.flatpak.sha256)
      echo "==> Built $out_fp ($(du -h "$out_fp" | cut -f1))"
      cat "$dist/Halo3-x86_64.flatpak.sha256"
    }

    # -------------------------------------------------------------------------
    # Shared Windows Stage (built once, packaged into BOTH Setup.exe & Portable.zip)
    # -------------------------------------------------------------------------
    WIN_STAGE_READY=0
    WIN_STAGE_PARENT=/tmp/halo3-win-stage
    WIN_STAGE="$WIN_STAGE_PARENT/Halo3-Windows-Portable"
    ensure_windows_stage() {
      if (( WIN_STAGE_READY )); then return 0; fi
      ensure_icons
      local base_root="/src/build-pc/release"
      if [[ $clean == 1 ]]; then base_root="/tmp/halo3-clean-win64"; fi
      local bdir="$base_root/win64"
      local odir="$base_root/win64-probe"
      local wdir="/tmp/halo3-win-tools"
      local rcdir="/tmp/halo3-win-rc"
      mkdir -p "$rcdir" "$WIN_STAGE/bin"

      cp "$ICON_DIR/halo3.ico" /src/port/packaging/halo3.rc /src/port/packaging/halo3.manifest "$rcdir/"
      llvm-rc -I /opt/mingw64/include -I "$rcdir" -DHALO3_RC_VER_STR="\"$version\"" \
        /FO "$rcdir/halo3_launcher.res" "$rcdir/halo3.rc"
      llvm-rc -I /opt/mingw64/include -I "$rcdir" -DHALO3_RC_ICON_ONLY \
        /FO "$rcdir/halo3_game.res" "$rcdir/halo3.rc"

      configure_and_build_windows "$bdir" "$odir" 1 0 "$rcdir/halo3_game.res" "$@"
      build_windows_tools_and_launcher "$wdir" "$rcdir/halo3_launcher.res"

      cp "$wdir/Halo3.exe"                        "$WIN_STAGE/Halo3.exe"
      cp "$odir/halo3_probe.exe"                  "$WIN_STAGE/bin/halo3.exe"
      cp "$wdir/xiso.exe"                         "$WIN_STAGE/bin/xiso.exe"
      cp "$wdir/xex2pe.exe"                       "$WIN_STAGE/bin/xex2pe.exe"
      cp "$wdir/shaderpack.exe"                   "$WIN_STAGE/bin/shaderpack.exe"
      cp "$wdir/extract-odst-dlc.exe"             "$WIN_STAGE/bin/extract-odst-dlc.exe"
      cp /src/port/packaging/update-windows.ps1   "$WIN_STAGE/bin/update.ps1"
      if [[ -f /src/port/data/pipeline_keys.bin ]]; then
        cp /src/port/data/pipeline_keys.bin       "$WIN_STAGE/bin/pipeline_keys.bin"
        chmod 0644 "$WIN_STAGE/bin/pipeline_keys.bin"
      fi
      stage_fonts "$WIN_STAGE/bin/fonts"
      printf "%s\n" "$version"                    > "$WIN_STAGE/VERSION"
      printf "%s\n" "$shader_abi"                 > "$WIN_STAGE/SHADER_ABI"
      cp /etc/halo3-build-manifest                "$WIN_STAGE/BUILD_MANIFEST.txt"

      chmod 0755 "$WIN_STAGE" "$WIN_STAGE/bin" "$WIN_STAGE/Halo3.exe" \
                 "$WIN_STAGE/bin/halo3.exe" "$WIN_STAGE/bin/xiso.exe" \
                 "$WIN_STAGE/bin/xex2pe.exe" "$WIN_STAGE/bin/shaderpack.exe" \
                 "$WIN_STAGE/bin/extract-odst-dlc.exe"
      chmod 0644 "$WIN_STAGE/bin/update.ps1" "$WIN_STAGE/VERSION" \
                 "$WIN_STAGE/SHADER_ABI" "$WIN_STAGE/BUILD_MANIFEST.txt"
      find "$WIN_STAGE" -exec touch -h -d "@$SOURCE_DATE_EPOCH" {} +

      verify_windows_binaries "$WIN_STAGE" "$odir/stage1/stubs.tsv"
      WIN_STAGE_READY=1
    }

    package_windows_zip() {
      ensure_windows_stage "$@"
      local dist="/src/$out_dir"
      mkdir -p "$dist"
      local out_zip="$dist/Halo3-Windows-Portable.zip"
      local tmp_zip="/tmp/Halo3-Windows-Portable.zip"
      (cd "$WIN_STAGE_PARENT" && find Halo3-Windows-Portable -type f | LC_ALL=C sort | zip -9 -X -q "$tmp_zip" -@)
      cp "$tmp_zip" "$out_zip"
      touch -d "@$SOURCE_DATE_EPOCH" "$out_zip"
      unzip -tq "$out_zip"
      echo "    [ok] Halo3-Windows-Portable.zip integrity verified"
      (cd "$dist" && sha256sum Halo3-Windows-Portable.zip > Halo3-Windows-Portable.zip.sha256)
      echo "==> Built $out_zip ($(du -h "$out_zip" | cut -f1))"
      cat "$dist/Halo3-Windows-Portable.zip.sha256"
    }

    package_windows_exe() {
      ensure_windows_stage "$@"
      local dist="/src/$out_dir"
      mkdir -p "$dist"
      local out_exe="$dist/Halo3-Setup.exe"
      echo "==> Building deterministic Windows per-user NSIS installer (Halo3-Setup.exe)..."
      makensis -V2 \
        -DAPP_VERSION="$version" \
        -DSTAGE_DIR="$WIN_STAGE" \
        -DICON_FILE="$ICON_DIR/halo3.ico" \
        -DOUT_FILE="$out_exe" \
        /src/port/packaging/halo3-installer.nsi
      chmod 0755 "$out_exe"
      touch -d "@$SOURCE_DATE_EPOCH" "$out_exe"
      local stype
      stype=$(file -b "$out_exe")
      if [[ $stype != *"PE32 executable (GUI) Intel 80386"* ]]; then
        echo "VERIFY FAIL: Halo3-Setup.exe unexpected format: $stype" >&2
        exit 1
      fi
      echo "    [ok] Halo3-Setup.exe: $(echo "$stype" | cut -d, -f1-2)"
      (cd "$dist" && sha256sum Halo3-Setup.exe > Halo3-Setup.exe.sha256)
      echo "==> Built $out_exe ($(du -h "$out_exe" | cut -f1))"
      cat "$dist/Halo3-Setup.exe.sha256"
    }

    case $target in
      linux)
        bdir=/src/build-pc/lin64; odir=/src/build-pc/lin64/probe
        if [[ $clean == 1 ]]; then bdir=/tmp/halo3-clean-lin64; odir=/src/build-pc/lin64/probe; fi
        configure_and_build_linux64 "$bdir" "$odir" 0 "$@"
        echo "==> Built $odir/halo3_probe"
        ;;
      windows)
        bdir=/src/build-pc/win64; odir=/src/build-pc/win64/probe
        if [[ $clean == 1 ]]; then bdir=/tmp/halo3-clean-win64; odir=/src/build-pc/win64/probe; fi
        configure_and_build_windows "$bdir" "$odir" 0 1 "" "$@"
        echo "==> Built $odir/halo3_probe.exe"
        ;;
      appimage)
        package_appimage "$@"
        ;;
      flatpak)
        package_flatpak "$@"
        ;;
      windows-exe)
        package_windows_exe "$@"
        ;;
      windows-zip)
        package_windows_zip "$@"
        ;;
      release)
        package_appimage "$@"
        package_flatpak "$@"
        package_windows_exe "$@"
        package_windows_zip "$@"
        ;;
    esac
  ' build "$target" "$build_type" "$jobs" "$clean" "$out_dir" "$version" "$fp_rt" "$@"
