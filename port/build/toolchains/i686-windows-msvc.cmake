# Cross toolchain: 32-bit Windows (MSVC ABI) objects from clang on Linux; lld-link links; Wine runs the result.
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER_TARGET i686-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET i686-pc-windows-msvc)
set(CMAKE_LINKER lld-link)
set(HALO3_PORT_TARGET windows)
# Compiler checks would try to link before the port has a runtime.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
