#ifndef NETWORKING_ONLINE_VOICE_ENGINE_XENON_H
#define NETWORKING_ONLINE_VOICE_ENGINE_XENON_H

#include <xtl.h>
#include <xaudio.h>
#include <xhv.h>
#include "shell/shell_types.h"
#include "networking/voice_processing_mode_types.h"
#include "networking/online/voice_data_types.h"

// c_xenon_voice_engine: the XHV voice engine wrapper (networking\online\voice_engine_xenon.cpp). Every method is
// inline except remote_player_start_processing_mode, which voice_engine_xenon.cpp defines; only the methods some
// unit uses are listed.
class c_xenon_voice_engine
{
public:
    __forceinline bool submit_voice(unsigned __int64 xuid, const unsigned char *data, long size, e_voice_data_type type) {
        bool success = false;
        if (initialized) {
            unsigned long submitted_size = size;
            HRESULT result = engine->SubmitIncomingChatData(xuid, data, &submitted_size);
            if (result >= 0 && submitted_size == size) success = true;
        }
        return success;
    }
    __forceinline const XHV_PROCESSING_MODE *xhv_processing_mode_from_voice_processing_mode(e_voice_processing_mode mode) {
        const XHV_PROCESSING_MODE *xhv_mode = 0;
        switch (mode) {
        case _voice_processing_mode_chat: xhv_mode = &_xhv_voicechat_mode; break;
        // match: 0x41df30: folds the mode switch to a straight-line chat-mode load; without it cmpwi/bne null guards appear and r10/r11 reshuffle
        default: __assume(0);
        }
        return xhv_mode;
    }
    __forceinline bool local_user_start_processing_mode(long controller_index, e_voice_processing_mode mode) {
        bool success = false;
        if (initialized) {
            const XHV_PROCESSING_MODE *xhv_mode = xhv_processing_mode_from_voice_processing_mode(mode);
            HRESULT result = engine->StartLocalProcessingModes(controller_index, (PXHV_PROCESSING_MODE)xhv_mode, 1);
            if (result >= 0) success = true;
        }
        return success;
    }
    __forceinline bool local_user_stop_processing_mode(long controller_index, e_voice_processing_mode mode) {
        bool success = false;
        if (initialized) {
            const XHV_PROCESSING_MODE *xhv_mode = xhv_processing_mode_from_voice_processing_mode(mode);
            HRESULT result = engine->StopLocalProcessingModes(controller_index, (PXHV_PROCESSING_MODE)xhv_mode, 1);
            if (result >= 0) success = true;
        }
        return success;
    }
    __forceinline bool unregister_remote_talker(unsigned __int64 xuid) {
        bool success = false;
        if (initialized) {
            XAUDIOFXPARAM param;
            param.Flags = 0xFFFFFFFF;
            engine->SetRemoteTalkerEffectParam(xuid, FALSE, 0, 0, XAUDIOFXPARAMTYPE_FLAG, &param);
            HRESULT result = engine->UnregisterRemoteTalker(xuid);
            if (result >= 0) success = true;
        }
        return success;
    }
    __forceinline bool register_remote_talker(unsigned __int64 xuid, long player_index) {
        bool success = false;
        if (initialized) {
            HRESULT result = engine->RegisterRemoteTalker(xuid, 0, 0, 0);
            if (result >= 0) {
                XAUDIOFXPARAM param;
                param.Flags = player_index;
                engine->SetRemoteTalkerEffectParam(xuid, FALSE, 0, 0, XAUDIOFXPARAMTYPE_FLAG, &param);
                success = true;
            }
        }
        return success;
    }
    __forceinline bool local_user_talking(e_controller_index controller_index) {
        bool talking = false;
        if (initialized) talking = engine->IsLocalTalking(controller_index) != FALSE;
        return talking;
    }
    bool remote_player_start_processing_mode(unsigned __int64 xuid, e_voice_processing_mode mode);
    __forceinline bool remote_player_talking(unsigned __int64 xuid) {
        bool talking = false;
        if (initialized) talking = engine->IsRemoteTalking(xuid) != FALSE;
        return talking;
    }
    __forceinline bool local_user_voice_device_connected(e_controller_index controller_index) {
        bool connected = false;
        if (initialized)
            connected = engine->IsHeadsetPresent(controller_index) != FALSE;
        return connected;
    }
    IXHVEngine *engine;
    bool initialized;
};

// The processing modes voice_xenon.cpp hands to XHVCreateEngine for local and remote talkers.
extern const XHV_PROCESSING_MODE g_voice_local_processing_modes[1];
extern const XHV_PROCESSING_MODE g_voice_remote_processing_modes[1];

#endif // NETWORKING_ONLINE_VOICE_ENGINE_XENON_H
