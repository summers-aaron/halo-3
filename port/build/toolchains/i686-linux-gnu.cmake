# 32-bit native Linux. The game code needs the MS C++ ABI: forward-declared enums are int-sized, class layout pads
# the vtable pointer to the class's alignment, one virtual deleting destructor, MS bitfields. Clang gives that ABI
# with ELF objects through the i686-pc-windows-msvc-elf triple, which CMakeLists.txt adds to the game library only
# (CMake drops a --target that simulates MSVC on Linux); __declspec(thread) becomes ELF TLS. The port's own
# runtime (port/sdk) is ordinary i686-linux-gnu code, and ld.lld links everything against 32-bit glibc
# (glibc-devel.i686).
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i686)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER_TARGET i686-linux-gnu)
set(CMAKE_CXX_COMPILER_TARGET i686-linux-gnu)
set(HALO3_PORT_TARGET linux)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
