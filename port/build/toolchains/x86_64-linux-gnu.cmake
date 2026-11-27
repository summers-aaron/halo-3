# 64-bit native Linux (x86_64).
# Game code (halo3_game, halo3_sdk) uses x86_64-pc-windows-msvc-elf (MS C++ ABI, LLP64: 4-byte long, 8-byte pointers,
# ELF64 objects). Host runtime / GPU / data code (halo3_runtime, halo3_data, halo3_gpu) uses native x86_64-linux-gnu
# so it links directly against 64-bit glibc, Vulkan, X11, PulseAudio, OpenSSL, etc.
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER_TARGET x86_64-linux-gnu)
set(CMAKE_CXX_COMPILER_TARGET x86_64-linux-gnu)
set(HALO3_PORT_TARGET linux)
set(HALO3_PORT_ARCH x86_64)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
