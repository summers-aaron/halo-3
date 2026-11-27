// binkxenon/binkacd.obj: from the records 0x713b08
// @compiler xdk6130
// @flags /TP /O1 /Oc
#include "bink/radmem.h"

extern "C" void BinkAudioDecompressClose(void *audio)
{
    radfree(audio);
}
