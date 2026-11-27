# Host build of the game source (tu/) for 32-bit PC, against the Xbox SDK API that port/sdk implements.
#
#   cmake -S port/build -B build-pc/windows -G Ninja --toolchain port/build/toolchains/i686-windows-msvc.cmake
#   cmake -S port/build -B build-pc/linux   -G Ninja --toolchain port/build/toolchains/i686-linux-gnu.cmake
#   ninja -C build-pc/windows -k 0
#
# Stage 1 (plan §5): every unit compiles; the SDK is declared by the port's clean-room headers (port/sdk/xbox-api/include:
# no XDK on the build machine), with port/sdk overrides ahead of them; implementations and stubs come next.
# -DHALO3_XDK_HEADERS=ON compiles against an XDK install (XDK_DIR) instead, only to A/B the objects.
cmake_minimum_required(VERSION 3.24)
# The game is a retail (NDEBUG) build: the release DLL runtime, so no _DEBUG (which would turn on the SDK's debug-only
# inline code, e.g. d3d9.h's PIX methods).
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
# Optimized with debug information by default (-O2 -g); configure with -DCMAKE_BUILD_TYPE=Debug for -O0.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()
project(halo3_pc C CXX)

# Every target, in every configuration (directory options come before the targets' own, so the game's
# -U__SSE__ -U__SSE2__ -U__MMX__ below still hide SSE from the Xbox headers' preprocessor):
#   - code generation for 32-bit x86 with SSE2: scalar float math in SSE registers, each operation rounded to float or
#     double as the Xbox's FPU does (x87 keeps 80-bit intermediates), and the VMX128 helpers (port/sdk/vmx) inlined as
#     SSE code;
#   - the semantics the decompiled PowerPC code relies on, whatever the optimizer: pointer casts between unrelated
#     types (-fno-strict-aliasing), wrapping signed arithmetic (-fwrapv), null checks the optimizer must keep
#     (-fno-delete-null-pointer-checks), and a defined value in every local before its first store
#     (-ftrivial-auto-var-init=zero): the Xbox code zeroes vectors as v ^ v of an unset v, which PowerPC computes as 0
#     but an optimizer may fold to anything (GAME-CHANGES F89).
add_compile_options(-msse2 -mfpmath=sse -fno-strict-aliasing -fwrapv -fno-delete-null-pointer-checks
                    -ftrivial-auto-var-init=zero)

get_filename_component(HALO3_REPO "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(HALO3_PORT "${HALO3_REPO}/port")
option(HALO3_REPRODUCIBLE "Normalize embedded source paths for deterministic builds" OFF)
if(HALO3_REPRODUCIBLE)
  add_compile_options("-ffile-prefix-map=${HALO3_REPO}=.")
endif()
# The Xbox SDK API the game is compiled against: clean-room declarations of exactly what the game and port/sdk use,
# ABI- and code-identical to the XDK's: every object compiles to the same bytes (port/sdk/xbox-api/README.md).
set(HALO3_SDK_HEADERS "${HALO3_PORT}/sdk/xbox-api/include" CACHE PATH "Clean-room Xbox 360 SDK headers")
option(HALO3_XDK_HEADERS "Compile against the XDK headers in XDK_DIR instead of HALO3_SDK_HEADERS (A/B checks)" OFF)
set(HALO3_RETAIL_PE "${HALO3_REPO}/local/reference/default.pe" CACHE FILEPATH
  "The retail image, loaded (indexed by RVA): the stage-1 link poisons data stubs that have retail values")
if(HALO3_XDK_HEADERS)
  set(XDK_DIR "${HALO3_REPO}/local/xdk/XDK" CACHE PATH "Xbox 360 XDK (headers only; from the owner's install)")
  if(NOT EXISTS "${XDK_DIR}/include/xbox/xtl.h")
    message(FATAL_ERROR "XDK headers not found under ${XDK_DIR}/include/xbox")
  endif()
  set(halo3_sdk_headers "${XDK_DIR}/include/xbox")
  set(halo3_crt_intrin "${XDK_DIR}/Source/crt/intrin.h")
  set(halo3_sdk_define)
else()
  if(NOT EXISTS "${HALO3_SDK_HEADERS}/xtl.h")
    message(FATAL_ERROR "clean-room SDK headers not found under ${HALO3_SDK_HEADERS}")
  endif()
  set(halo3_sdk_headers "${HALO3_SDK_HEADERS}")
  set(halo3_crt_intrin "${HALO3_SDK_HEADERS}/intrin.h")
  set(halo3_sdk_define PORT_ZERO_XDK=1)
endif()
if(NOT DEFINED HALO3_PORT_TARGET)
  message(FATAL_ERROR "use a toolchain file from port/build/toolchains")
endif()

# The game and the SDK headers see a big-endian PowerPC Xbox: the target's x86 macros are removed and the PPC ones
# defined. __stdcall is dropped: PowerPC has one calling convention, and the game redeclares SDK functions without it.
# __forceinline becomes plain inline: the Xbox build inlines some __forceinline functions defined in one unit into
# callers in other units (link-time code generation); as plain inline functions, the unit that defines and uses one
# emits a shared copy that the other units link to.
# _STATIC_CPPLIB: the C++ library is linked statically (the Xbox has no C++ runtime DLL, and the port links none): on
# Windows the DLL C runtime defines _DLL, and a unit that reads the C++ library headers before xtl.h sets _XBOX_VER
# would otherwise declare that library imported from msvcp*.dll, an import nothing resolves (port/sdk/runtime/
# stl_throw.cpp: std::char_traits<char>::length). The SDK headers' own yvals.h says the same, but only after
# crtdefs.h has chosen the import macros. No effect where _DLL is not defined (Linux).
set(halo3_defines _M_PPC=1 _M_PPCBE _XBOX XBOX NDEBUG _WIN32 _Complex=_cplx_arg __stdcall= _stdcall=
    __forceinline=inline _STATIC_CPPLIB
    "PORT_XDK_CRT_INTRIN_H=\"${halo3_crt_intrin}\"" ${halo3_sdk_define})
set(halo3_flags
    -fms-extensions -fms-compatibility -fms-compatibility-version=14 -fno-builtin -nobuiltininc
    -U_M_X64 -U_M_AMD64 -U_M_IX86 -U__x86_64__ -U__i386__ -U_X86_ -U__SSE__ -U__SSE2__ -U__MMX__
    -Wno-everything -ferror-limit=0 -fno-caret-diagnostics -fdiagnostics-show-option
    # No unit uses dynamic_cast, typeid, try or catch, and the retail build has no C++ exceptions. Without them
    # the MS ABI also needs no "largest" COMDATs (vftables with RTTI) or EH funclets, which ELF cannot express.
    -fno-rtti -fno-exceptions
    # A load or store through a pointer assumes at most 4-byte alignment (objects keep their declared alignment). The
    # Xbox moves vector data with lvx / stvx, which never fault, and the code reaches vectors through
    # pointers to storage the PC build does not lay out 16-aligned (float arrays, 12-byte points, pooled objects);
    # assuming the type's 16 made those accesses movaps, which faults (GAME-CHANGES F92).
    -fmax-type-align=4)
if(HALO3_PORT_ARCH STREQUAL "x86_64")
  list(APPEND halo3_defines HALO3_64BIT=1)
endif()
if(HALO3_PORT_TARGET STREQUAL "linux")
  # The game code needs the MS C++ ABI; on Linux it is compiled for <arch>-pc-windows-msvc-elf (MS ABI, ELF objects),
  # while the port runtime stays <arch>-linux-gnu. The later --target wins over the toolchain's.
  # SEH (__try) has no Linux runtime: port/sdk/include/port_seh.h maps it to plain blocks.
  if(HALO3_PORT_ARCH STREQUAL "x86_64")
    list(APPEND halo3_flags --target=x86_64-pc-windows-msvc-elf -include "${HALO3_PORT}/sdk/include/port_seh.h")
  else()
    list(APPEND halo3_flags --target=i686-pc-windows-msvc-elf -include "${HALO3_PORT}/sdk/include/port_seh.h")
  endif()
endif()

# Search order: port overrides, the game's headers, case forwarders (and the base for the XDK-relative
# <../../Source/crt/intrin.h>), decompiler types, then the SDK headers.
set(halo3_includes
    "${HALO3_PORT}/sdk/include"
    "${HALO3_REPO}/include")
set(halo3_system_includes
    "${HALO3_PORT}/sdk/xdk-compat/include/xbox"
    "${HALO3_REPO}/metadata/judge-inputs"
    "${halo3_sdk_headers}")

file(GLOB_RECURSE halo3_units RELATIVE "${HALO3_REPO}" CONFIGURE_DEPENDS "${HALO3_REPO}/tu/*.cpp")
list(SORT halo3_units)
set(halo3_sources)
foreach(unit IN LISTS halo3_units)
  set(path "${HALO3_REPO}/${unit}")
  list(APPEND halo3_sources "${path}")
  # Per-unit compiler flags from the unit's `// @flags` line: /TC compiles it as C, /J makes char unsigned.
  file(STRINGS "${path}" unit_flags REGEX "^// @flags" LIMIT_COUNT 1)
  if(unit_flags MATCHES " /TC( |$)")
    set_source_files_properties("${path}" PROPERTIES LANGUAGE C)
  endif()
  if(unit_flags MATCHES " /J( |$)")
    set_property(SOURCE "${path}" APPEND PROPERTY COMPILE_OPTIONS -funsigned-char)
  endif()
endforeach()
list(LENGTH halo3_sources halo3_unit_count)
message(STATUS "halo3: ${halo3_unit_count} units, target ${HALO3_PORT_TARGET}")

# The game's inline functions other units call (cross-unit-inline.txt): kept out of line in their defining unit, so an
# optimized build still emits the copy the other units link to. A response file (written here, at configure time) keeps
# the MS-mangled names away from the shell; every unit depends on it.
file(STRINGS "${HALO3_PORT}/build/cross-unit-inline.txt" halo3_cross_unit_inline REGEX "^[^#]")
set(halo3_cross_unit_inline_rsp "${CMAKE_BINARY_DIR}/cross-unit-inline.rsp")
set(halo3_rsp_text "")
foreach(name IN LISTS halo3_cross_unit_inline)
  string(APPEND halo3_rsp_text "-mllvm \"-force-attribute=${name}:noinline\"\n")
endforeach()
file(WRITE "${halo3_cross_unit_inline_rsp}.new" "${halo3_rsp_text}")
configure_file("${halo3_cross_unit_inline_rsp}.new" "${halo3_cross_unit_inline_rsp}" COPYONLY) # rewritten only when it changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${HALO3_PORT}/build/cross-unit-inline.txt")
list(APPEND halo3_flags "@${halo3_cross_unit_inline_rsp}")

add_library(halo3_game OBJECT ${halo3_sources})
set_source_files_properties(${halo3_sources} PROPERTIES OBJECT_DEPENDS "${halo3_cross_unit_inline_rsp}")
target_compile_definitions(halo3_game PRIVATE ${halo3_defines})
target_compile_options(halo3_game PRIVATE ${halo3_flags})
target_include_directories(halo3_game PRIVATE ${halo3_includes})
target_include_directories(halo3_game SYSTEM PRIVATE ${halo3_system_includes})

# The port's own code (port/sdk): host code for the toolchain's target, never the Xbox SDK headers. A file named
# *_windows.c or *_linux.c builds for that target only; any other .c builds for both.
file(GLOB halo3_runtime_sources CONFIGURE_DEPENDS "${HALO3_PORT}/sdk/*/*.c")
list(FILTER halo3_runtime_sources EXCLUDE REGEX "_(windows|linux)\\.c$")
file(GLOB halo3_runtime_target_sources CONFIGURE_DEPENDS "${HALO3_PORT}/sdk/*/*_${HALO3_PORT_TARGET}.c")
list(APPEND halo3_runtime_sources ${halo3_runtime_target_sources})
add_library(halo3_runtime OBJECT ${halo3_runtime_sources})
option(HALO3_RELEASE_BUILD "Hide debug/developer-only Guide settings and CLI options" OFF)
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR HALO3_RELEASE_BUILD)
  target_compile_definitions(halo3_runtime PRIVATE HALO3_RELEASE_BUILD=1)
endif()
if(HALO3_PORT_ARCH STREQUAL "x86_64")
  target_compile_definitions(halo3_runtime PRIVATE HALO3_64BIT=1)
  target_compile_options(halo3_runtime PRIVATE -Wall -Wextra -Werror)
endif()
target_compile_options(halo3_runtime PRIVATE -fms-extensions -fno-builtin -include "${HALO3_PORT}/sdk/runtime/port_abi.h")
if(HALO3_PORT_TARGET STREQUAL "linux")
  # The game's MS-ABI code keeps the stack 4-byte aligned; the System V ABI promises 16. Host code realigns on entry.
  target_compile_options(halo3_runtime PRIVATE -mstackrealign)
  # Only the host ABI sees FreeType's headers. The shared library is optional at run time.
  find_path(HALO3_FREETYPE_INCLUDE_DIR ft2build.h PATH_SUFFIXES freetype2)
  if(HALO3_FREETYPE_INCLUDE_DIR)
    include(CheckIncludeFile)
    set(CMAKE_REQUIRED_INCLUDES "${HALO3_FREETYPE_INCLUDE_DIR}")
    check_include_file(freetype/freetype.h HALO3_FREETYPE_HEADERS_USABLE)
    unset(CMAKE_REQUIRED_INCLUDES)
    if(HALO3_FREETYPE_HEADERS_USABLE)
      set_source_files_properties(
        "${HALO3_PORT}/sdk/runtime/hd_font.c" "${HALO3_PORT}/sdk/runtime/hd_font_linux.c"
        PROPERTIES COMPILE_DEFINITIONS PORT_HAVE_FREETYPE_HEADERS=1
                   INCLUDE_DIRECTORIES "${HALO3_FREETYPE_INCLUDE_DIR}")
    endif()
  endif()
endif()

# Load-time data conversion (port/data: cache-file byte order, Xenos texture untiling and transcoding): host C++ for
# the toolchain's target with C linkage at its interface, never the Xbox SDK headers, and no C++ runtime (no exceptions,
# RTTI or dynamic initializers).
file(GLOB halo3_data_sources CONFIGURE_DEPENDS "${HALO3_PORT}/data/*.cpp")
add_library(halo3_data OBJECT ${halo3_data_sources})
if(HALO3_PORT_ARCH STREQUAL "x86_64")
  target_compile_definitions(halo3_data PRIVATE HALO3_64BIT=1)
endif()
target_compile_options(halo3_data PRIVATE -fno-exceptions -fno-rtti -fno-threadsafe-statics -include "${HALO3_PORT}/sdk/runtime/port_abi.h")
if(HALO3_PORT_TARGET STREQUAL "linux")
  # Called from the game's MS-ABI code, whose stack is only 4-byte aligned (see halo3_runtime).
  target_compile_options(halo3_data PRIVATE -mstackrealign)
endif()

# The PC renderer (port/sdk/d3d: the Xenos GPU frontend vulkan_device.cpp and xenos_*.cpp, the backend seam gpu_*.cpp,
# the Vulkan 1.3 backend vulkan_*.cpp, the Direct3D 12 backend d3d12_*.cpp) and the shader pack it draws with
# (port/shaders): host C++ like port/data, never the Xbox SDK headers; the graphics API is loaded at run time
# (port/sdk/runtime/vulkan_host_*.c). The zlib interface (inflating the pack) is the game's own include/zlib/zlib.h,
# implemented by port/sdk/runtime/zlib_shim.cpp.
file(GLOB halo3_gpu_sources CONFIGURE_DEPENDS "${HALO3_PORT}/shaders/*.cpp" "${HALO3_PORT}/sdk/d3d/vulkan_*.cpp"
     "${HALO3_PORT}/sdk/d3d/gpu_*.cpp" "${HALO3_PORT}/sdk/d3d/xenos_*.cpp" "${HALO3_PORT}/sdk/d3d/d3d12_*.cpp")
add_library(halo3_gpu OBJECT ${halo3_gpu_sources})
if(HALO3_PORT_ARCH STREQUAL "x86_64")
  target_compile_definitions(halo3_gpu PRIVATE HALO3_64BIT=1)
endif()
target_compile_options(halo3_gpu PRIVATE -fno-exceptions -fno-rtti -fno-threadsafe-statics -include "${HALO3_PORT}/sdk/runtime/port_abi.h")
target_include_directories(halo3_gpu PRIVATE "${HALO3_REPO}/include")
if(HALO3_PORT_TARGET STREQUAL "linux")
  # Called from the game's MS-ABI code, whose stack is only 4-byte aligned (see halo3_runtime).
  target_compile_options(halo3_gpu PRIVATE -mstackrealign)
endif()

# SDK implementations that need the SDK's own layouts (port/sdk/*/*.cpp: the D3D device the game's inline D3D code
# reads and writes): built like the game, against the SDK headers. The renderer is host code (halo3_gpu).
file(GLOB halo3_sdk_sources CONFIGURE_DEPENDS "${HALO3_PORT}/sdk/*/*.cpp")
list(FILTER halo3_sdk_sources EXCLUDE REGEX "/sdk/d3d/(vulkan|gpu|xenos|d3d12)_[^/]*\\.cpp$")
add_library(halo3_sdk OBJECT ${halo3_sdk_sources})
target_compile_definitions(halo3_sdk PRIVATE ${halo3_defines})
target_compile_options(halo3_sdk PRIVATE ${halo3_flags} "SHELL:-include ${HALO3_PORT}/sdk/runtime/port_abi.h")
target_include_directories(halo3_sdk PRIVATE ${halo3_includes})
target_include_directories(halo3_sdk SYSTEM PRIVATE ${halo3_system_includes})

# Stage-1 link: a trial link lists what is undefined, port/tools/stubgen defines it, the final link makes the binary
# (port/build/link-stage1.sh). Rerun after every change to tu/ or port/sdk.
add_custom_target(halo3 ALL
  COMMAND "${HALO3_PORT}/build/link-stage1.sh" "${HALO3_PORT_TARGET}" "${CMAKE_BINARY_DIR}" "${HALO3_RETAIL_PE}" "${HALO3_PORT_ARCH}"
  DEPENDS halo3_game halo3_runtime halo3_data halo3_gpu halo3_sdk
  USES_TERMINAL
  COMMENT "stage-1 link with generated stubs")
