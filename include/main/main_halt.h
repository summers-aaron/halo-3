#ifndef MAIN_MAIN_HALT_H
#define MAIN_MAIN_HALT_H

// main.cpp either sleeps indefinitely or calls XShowDirtyDiscErrorUI, which
// xbox.h declares DECLSPEC_NORETURN.
__declspec(noreturn) void main_halt_and_display_errors();

#endif // MAIN_MAIN_HALT_H
