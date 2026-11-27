// port/sdk/bink (both hosts): binkread.obj's checksound (retail 0x70e748), the sound service of an open movie
// (bink_service_sound, bink_read.cpp). include/bink/binkread.h declares it `int checksound(...)` (the decompiled
// callers, bink_sound_callback and BinkPause, pass the BINK), so it is defined in a unit of its own that does not
// include that header; on x86 a variadic call passes the pointer where this fixed parameter reads it.
extern "C" int bink_service_sound(void *bink);

// @address 0x70e748 checksound
extern "C" int checksound(void *bink)
{
    return bink_service_sound(bink);
}
