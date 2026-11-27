# port/sdk/xbox-api — clean-room Xbox 360 SDK headers

`include/` declares the Xbox 360 SDK API exactly as far as the game (`tu/`) and the port's SDK implementations
(`port/sdk/*/*.cpp`) use it: the C/C++ runtime (CRT, the Dinkumware C++ library subset), Win32/Xbox system and online
APIs, VMX128 / PPC intrinsics and XNA math, Direct3D 9 for Xenos, XGraphics, D3DX, XAudio / X3DAudio and XHV. It
replaces the XDK on the include path (port/build/CMakeLists.txt, `HALO3_SDK_HEADERS`), so the PC build needs only a
compiler toolchain and the owner's own Halo 3 disc data — no Microsoft SDK.

## Contract

Every object compiled against these headers is **byte-identical** to the object compiled against the XDK 6132 headers,
for both targets (Linux `i686-pc-windows-msvc-elf`, Windows `i686-pc-windows-msvc`; COFF compared without its
timestamp). That one check covers the whole ABI — struct sizes and offsets, bit-field order, enum values, tag names in
C++ mangled names, C vs C++ linkage, dllimport under the Windows DLL runtime, vtable order — and the exact behaviour of
every SDK inline function compiled into the game (D3D state shadows and pending bits, XNA math on the VMX path that
saved films replay, std::string, CRT forwarders).

To re-check after a change (needs an XDK install, only for the A/B):

    cmake -S port/build -B build-pc/ref-lin -G Ninja --toolchain port/build/toolchains/i686-linux-gnu.cmake \
          -DHALO3_XDK_HEADERS=ON -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -DNDEBUG" -DCMAKE_C_FLAGS_RELWITHDEBINFO="-O2 -DNDEBUG"
    cmake -S port/build -B build-pc/cr-lin  -G Ninja --toolchain port/build/toolchains/i686-linux-gnu.cmake \
          -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -DNDEBUG" -DCMAKE_C_FLAGS_RELWITHDEBINFO="-O2 -DNDEBUG"
    ninja -C build-pc/ref-lin halo3_game halo3_sdk && ninja -C build-pc/cr-lin halo3_game halo3_sdk
    # then cmp every CMakeFiles/**/*.o pair

## Rules for editing

- Clean room: declarations are written from the API's contract and from facts (names, values, signatures, layouts,
  observed behaviour); no Microsoft header text, comments or SAL annotations.
- Declare what is used; an omitted name fails to compile, it never miscompiles. Keep each header's include graph and
  guard macros as the game expects (units rely on transitive includes and test `_WINNT_`, `_D3D9_H_`, …).
- C++-only overloads and templates sit in `extern "C++"` (some units include SDK headers inside `extern "C"`).
- `port_crt_*.h` are internal pieces shared by several runtime headers.
- After any change, the A/B above must stay at zero differing objects on both targets.
