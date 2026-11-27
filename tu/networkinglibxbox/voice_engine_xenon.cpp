// networkinglibxbox/voice_engine_xenon.obj: from the records 0x41df30
// @compiler xdk6130
// @flags /TP /O2 /J /Oc
// @address 0x41df30 ?remote_player_start_processing_mode@c_xenon_voice_engine@@QAA_N_KW4e_voice_processing_mode@@@Z

#include "networking/online/voice_engine_xenon.h"
#include "networking/online/voice_xenon.h"

const XHV_PROCESSING_MODE g_voice_local_processing_modes[1] = { XHV_VOICECHAT_MODE };
const XHV_PROCESSING_MODE g_voice_remote_processing_modes[1] = { XHV_VOICECHAT_MODE };

bool c_xenon_voice_engine::remote_player_start_processing_mode(unsigned __int64 xuid, e_voice_processing_mode mode)
{
    bool success = false;
    if (g_voice_engine.initialized)
    {
        const XHV_PROCESSING_MODE *xhv_mode = g_voice_engine.xhv_processing_mode_from_voice_processing_mode(mode);
        HRESULT result = g_voice_engine.engine->StartRemoteProcessingModes(xuid, (PXHV_PROCESSING_MODE)xhv_mode, 1);
        if (result >= 0)
            success = true;
    }
    return success;
}
