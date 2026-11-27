// xhv/voicld.obj: from the records 0x6f11e8
// @flags /TP /O1 /Oc

// @address 0x6f11e8 FindBand4VoicLD
extern "C" int FindBand4VoicLD(int maximum_band, float pitch)
{
    int band = (int)((512.0f / pitch) * 3.9f + 0.5f);
    if (band > maximum_band)
        band = maximum_band;
    return band;
}
