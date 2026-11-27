// port/sdk/runtime/port_abi.h: x86_64 Linux calling-convention bridge between the game/SDK objects
// (compiled with --target=x86_64-pc-windows-msvc-elf, Win64 ms_abi) and the Linux host runtime/data/gpu objects
// (compiled with x86_64-linux-gnu, SysV AMD64 ABI). On i686 and Windows, both sides already share one ABI.
#ifndef PORT_ABI_H
#define PORT_ABI_H

#if defined(__x86_64__) && !defined(_WIN32)
#define PORT_MS_ABI __attribute__((ms_abi))
#define PORT_SYSV_ABI __attribute__((sysv_abi))
#else
#define PORT_MS_ABI
#define PORT_SYSV_ABI
#endif

#endif
