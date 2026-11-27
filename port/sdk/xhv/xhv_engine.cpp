// port/sdk/xhv: the XHV voice engine (xhv.lib). Built like the game, against the SDK's xhv.h, because the game calls
// the engine through IXHVEngine's virtual methods. The voice itself -- microphone, voice activity, codec, jitter
// buffers -- is host code (xhv_voice.h); this file is the engine's state and its XAudio graph.
//
// How Halo 3 uses the engine (networking/online/voice_xenon.cpp, voice_observer.cpp, voice_queue.cpp):
//   - XHVCreateEngine once (voice_initialize_engine): 4 local, 15 remote talkers, chat mode enabled for both, the
//     default remote talker effect chain = its c_remote_voice_ring_buffer_effect (mixbin: copies the talker's voice
//     into the remote voice ring, channel = player index), the output chain = its headset ring effect.
//   - RegisterLocalTalker per controller, Start/StopLocalProcessingModes(chat) as a player's voice is allowed.
//   - Every frame (voice_update_poll_for_local_chat_data): GetDataReadyFlags, then GetLocalChatData into a 100-byte
//     buffer (10 packets); each 10-byte packet goes to the voice queue and over the network.
//   - RegisterRemoteTalker(0xBEEFCAFE00000000 | player, no chains, no output) and SetRemoteTalkerEffectParam(effect
//     0, param 0, player) so the ring effect knows its player; StartRemoteProcessingModes(chat); each 10-byte packet
//     received goes to SubmitIncomingChatData; SetPlaybackPriority(talker, controller, 0 or NEVER) routes a talker to
//     a local player's headset or not; IsLocalTalking / IsRemoteTalking light the speaker icons; IsHeadsetPresent
//     feeds the controller state. When a local player has no headset, the game plays the talker's voice through the
//     speakers itself (voice_observer: speaker_states, reading the ring the remote talker effect fills).
//
// So each remote talker is an XAudio source voice (16 kHz 16-bit mono, the decoded voice, rendered 20 ms at a time
// from its process callback) running the title's remote talker effect chain, and routed to the engine's headset
// output: a 4-channel submix voice (one channel a local user, XHV_XAUDIO_OUTPUT_CHANNEL_COUNT), whose channel map
// gives a talker volume 1 for each user with a headset whose playback priority for it is not NEVER. On a PC the
// headset is the host's output, so the headset submix mixes into the mastering voice (front left and right). The
// title's output chain is not applied: it adds the title's per-user headset mix (game sounds the title also plays on
// the speakers, m_headset_ring_buffer) to the headset, and on a PC headset and speakers are one device -- applying
// it would play those sounds twice.
#include <xtl.h>
#include <xaudio.h>
#include <xhv.h>

#include "xhv_voice.h"

namespace {

// XHV_VOICECHAT_MODE and XHV_LOOPBACK_MODE are opaque values the title passes back to the engine; each points at a
// tag of its own.
char g_voicechat_mode_tag;
char g_loopback_mode_tag;

const DWORD k_maximum_local_talkers = XHV_MAX_LOCAL_TALKERS;
const DWORD k_maximum_remote_talkers = XHV_MAX_REMOTE_TALKERS;
const DWORD k_maximum_modes = XHV_MAX_PROCESSING_MODES;
const DWORD k_frame_samples = XHV_VOICE_FRAME_SAMPLES;
const DWORD k_pcm_buffers = 3;  // a talker keeps two 20 ms packets queued, the third is being rendered
const LONG k_packets_ahead = 2;

class c_xhv_engine;

struct s_remote_talker {
    bool in_use;
    bool chat;          // StartRemoteProcessingModes(chat)
    XUID xuid;
    DWORD slot;         // xhv_voice.h's
    IXAudioSourceVoice *voice;
    IXAudioSubmixVoice *title_output; // pOutputVoice / SetRemoteTalkerOutputVoice
    XHV_PLAYBACK_PRIORITY priority[k_maximum_local_talkers];
    // the XAudio thread's (under the XAudio lock)
    LONG queued;
    DWORD next_buffer;
    short pcm[k_pcm_buffers][k_frame_samples];
};

class c_xhv_engine final : public IXHVEngine
{
public:
    c_xhv_engine(const XHV_INIT_PARAMS &parameters)
        : m_reference_count(1), m_local_talkers(0), m_local_chat(0), m_output(NULL), m_maximum_decode_packets(0)
    {
        InitializeCriticalSection(&m_lock);
        m_maximum_local_talkers = parameters.dwMaxLocalTalkers < k_maximum_local_talkers
            ? parameters.dwMaxLocalTalkers : k_maximum_local_talkers;
        m_maximum_remote_talkers = parameters.dwMaxRemoteTalkers < k_maximum_remote_talkers
            ? parameters.dwMaxRemoteTalkers : k_maximum_remote_talkers;
        copy_modes(m_local_modes, &m_local_mode_count, parameters.localTalkerEnabledModes,
            parameters.dwNumLocalTalkerEnabledModes);
        copy_modes(m_remote_modes, &m_remote_mode_count, parameters.remoteTalkerEnabledModes,
            parameters.dwNumRemoteTalkerEnabledModes);
        m_default_remote_effects = parameters.pfxDefaultRemoteTalkerFX;
        for (DWORD i = 0; i < k_maximum_remote_talkers; i++) {
            m_talkers[i].in_use = false;
            m_talkers[i].voice = NULL;
        }
        m_headsets = xhv_voice_open();
        create_output();
    }

    ULONG AddRef() override
    {
        return (ULONG)InterlockedIncrement(&m_reference_count);
    }

    ULONG Release() override
    {
        LONG count = InterlockedDecrement(&m_reference_count);
        if (count == 0) {
            for (DWORD i = 0; i < k_maximum_remote_talkers; i++)
                if (m_talkers[i].in_use)
                    remove_talker(&m_talkers[i]);
            if (m_output != NULL)
                m_output->Release();
            for (DWORD u = 0; u < k_maximum_local_talkers; u++)
                xhv_voice_local_enable(u, 0);
            xhv_voice_close();
            DeleteCriticalSection(&m_lock);
            delete this;
        }
        return (ULONG)count;
    }

    HRESULT Lock(XHV_LOCK_TYPE lock_type) override
    {
        switch (lock_type) {
        case XHV_LOCK_TYPE_LOCK:
            EnterCriticalSection(&m_lock);
            return S_OK;
        case XHV_LOCK_TYPE_TRYLOCK:
            return TryEnterCriticalSection(&m_lock) ? S_OK : E_PENDING;
        case XHV_LOCK_TYPE_UNLOCK:
            LeaveCriticalSection(&m_lock);
            return S_OK;
        default:
            return E_INVALIDARG;
        }
    }

    HRESULT StartLocalProcessingModes(DWORD user_index, CONST PXHV_PROCESSING_MODE modes, DWORD mode_count) override
    {
        return set_local_modes(user_index, modes, mode_count, true);
    }

    HRESULT StopLocalProcessingModes(DWORD user_index, CONST PXHV_PROCESSING_MODE modes, DWORD mode_count) override
    {
        return set_local_modes(user_index, modes, mode_count, false);
    }

    HRESULT StartRemoteProcessingModes(XUID remote_talker, CONST PXHV_PROCESSING_MODE modes, DWORD mode_count) override
    {
        return set_remote_modes(remote_talker, modes, mode_count, true);
    }

    HRESULT StopRemoteProcessingModes(XUID remote_talker, CONST PXHV_PROCESSING_MODE modes, DWORD mode_count) override
    {
        return set_remote_modes(remote_talker, modes, mode_count, false);
    }

    // Every talker the priorities let through is decoded and mixed: a PC has the cycles for all fifteen.
    HRESULT SetMaxDecodePackets(DWORD maximum_decode_packets) override
    {
        m_maximum_decode_packets = maximum_decode_packets;
        return S_OK;
    }

    HRESULT RegisterLocalTalker(DWORD user_index) override
    {
        if (user_index >= m_maximum_local_talkers)
            return E_INVALIDARG;
        EnterCriticalSection(&m_lock);
        m_local_talkers |= 1u << user_index;
        update_local(user_index);
        LeaveCriticalSection(&m_lock);
        return S_OK;
    }

    HRESULT UnregisterLocalTalker(DWORD user_index) override
    {
        if (user_index >= m_maximum_local_talkers)
            return E_INVALIDARG;
        EnterCriticalSection(&m_lock);
        m_local_talkers &= ~(1u << user_index);
        m_local_chat &= ~(1u << user_index);
        update_local(user_index);
        LeaveCriticalSection(&m_lock);
        return S_OK;
    }

    HRESULT RegisterRemoteTalker(XUID remote_talker, LPCXAUDIOVOICEFXCHAIN remote_talker_effects,
        LPCXAUDIOVOICEFXCHAIN talker_pair_effects, LPXAUDIOSUBMIXVOICE output_voice) override
    {
        (void)talker_pair_effects; // the voice is mono and mixed once for every user: there is no per-pair voice
        HRESULT result = S_OK;
        EnterCriticalSection(&m_lock);
        DWORD count = 0;
        s_remote_talker *free_slot = NULL;
        for (DWORD i = 0; i < k_maximum_remote_talkers; i++) {
            if (m_talkers[i].in_use) {
                count++;
                if (m_talkers[i].xuid == remote_talker)
                    result = E_INVALIDARG;
            } else if (free_slot == NULL) {
                free_slot = &m_talkers[i];
            }
        }
        if (SUCCEEDED(result) && (count >= m_maximum_remote_talkers || free_slot == NULL))
            result = E_OUTOFMEMORY;
        if (SUCCEEDED(result)) {
            sync_headsets_locked();
            add_talker(free_slot, remote_talker, remote_talker_effects != NULL ? remote_talker_effects
                : m_default_remote_effects, output_voice);
        }
        LeaveCriticalSection(&m_lock);
        return result;
    }

    HRESULT UnregisterRemoteTalker(XUID remote_talker) override
    {
        EnterCriticalSection(&m_lock);
        s_remote_talker *talker = find_remote(remote_talker);
        if (talker != NULL)
            remove_talker(talker);
        LeaveCriticalSection(&m_lock);
        return talker != NULL ? S_OK : E_INVALIDARG;
    }

    HRESULT GetRemoteTalkers(PDWORD remote_talker_count, PXUID remote_talkers) override
    {
        if (remote_talker_count == NULL)
            return E_INVALIDARG;
        EnterCriticalSection(&m_lock);
        DWORD count = 0;
        for (DWORD i = 0; i < k_maximum_remote_talkers; i++) {
            if (!m_talkers[i].in_use)
                continue;
            if (remote_talkers != NULL)
                remote_talkers[count] = m_talkers[i].xuid;
            count++;
        }
        *remote_talker_count = count;
        LeaveCriticalSection(&m_lock);
        return S_OK;
    }

    BOOL IsHeadsetPresent(DWORD user_index) override
    {
        sync_headsets();
        return user_index < m_maximum_local_talkers && (m_headsets & (1u << user_index)) != 0;
    }

    BOOL IsLocalTalking(DWORD user_index) override
    {
        if (user_index >= m_maximum_local_talkers || !(m_local_talkers & (1u << user_index)))
            return FALSE;
        return xhv_voice_local_talking(user_index) ? TRUE : FALSE;
    }

    BOOL IsRemoteTalking(XUID remote_talker) override
    {
        EnterCriticalSection(&m_lock);
        s_remote_talker *talker = find_remote(remote_talker);
        BOOL talking = talker != NULL && talker->chat && xhv_voice_remote_talking(talker->slot) ? TRUE : FALSE;
        LeaveCriticalSection(&m_lock);
        return talking;
    }

    HRESULT SetRemoteTalkerOutputVoice(XUID remote_talker, LPXAUDIOSUBMIXVOICE output_voice) override
    {
        EnterCriticalSection(&m_lock);
        s_remote_talker *talker = find_remote(remote_talker);
        if (talker != NULL) {
            talker->title_output = output_voice;
            if (talker->voice != NULL)
                set_talker_outputs(talker);
        }
        LeaveCriticalSection(&m_lock);
        return talker != NULL ? S_OK : E_INVALIDARG;
    }

    HRESULT SetRemoteTalkerEffectParam(XUID remote_talker, BOOL pair_effect, XAUDIOVOICEFXINDEX effect_index,
        XAUDIOFXPARAMID parameter_id, XAUDIOFXPARAMTYPE parameter_type, LPCXAUDIOFXPARAM parameter) override
    {
        EnterCriticalSection(&m_lock);
        s_remote_talker *talker = find_remote(remote_talker);
        HRESULT result = talker != NULL ? S_OK : E_INVALIDARG;
        // the talker's own chain runs on its voice; there are no pair voices (and so no pair effects) to set
        if (talker != NULL && !pair_effect && talker->voice != NULL)
            result = XAudioVoice_SetEffectParam(talker->voice, effect_index, parameter_id, parameter_type, parameter);
        LeaveCriticalSection(&m_lock);
        return result;
    }

    DWORD GetDataReadyFlags() override
    {
        sync_headsets();
        DWORD flags = 0;
        for (DWORD u = 0; u < m_maximum_local_talkers; u++)
            if ((m_local_chat & m_local_talkers & (1u << u)) && xhv_voice_local_packets(u) != 0)
                flags |= 1u << (u + XHV_VOICECHAT_DATA_READY_OFFSET);
        return flags;
    }

    HRESULT GetLocalChatData(DWORD user_index, PBYTE data, PDWORD size, PDWORD packets) override
    {
        if (user_index >= m_maximum_local_talkers || size == NULL || data == NULL)
            return E_INVALIDARG;
        DWORD room = *size / XHV_VOICECHAT_MODE_PACKET_SIZE, count = 0;
        if (room == 0)
            return E_INVALIDARG;
        if (m_local_chat & m_local_talkers & (1u << user_index))
            count = xhv_voice_local_read(user_index, data, room);
        *size = count * XHV_VOICECHAT_MODE_PACKET_SIZE;
        if (packets != NULL)
            *packets = count;
        return count != 0 ? S_OK : E_PENDING;
    }

    HRESULT SetPlaybackPriority(XUID remote_talker, DWORD user_index, XHV_PLAYBACK_PRIORITY priority) override
    {
        if (user_index >= m_maximum_local_talkers)
            return E_INVALIDARG;
        EnterCriticalSection(&m_lock);
        sync_headsets_locked();
        s_remote_talker *talker = find_remote(remote_talker);
        if (talker != NULL && talker->priority[user_index] != priority) {
            talker->priority[user_index] = priority;
            set_talker_volumes(talker);
        }
        LeaveCriticalSection(&m_lock);
        return talker != NULL ? S_OK : E_INVALIDARG;
    }

    // Whole packets are taken; a talker not in chat mode has its voice consumed and dropped.
    HRESULT SubmitIncomingChatData(XUID remote_talker, CONST BYTE *data, PDWORD size) override
    {
        if (size == NULL || (data == NULL && *size != 0))
            return E_INVALIDARG;
        EnterCriticalSection(&m_lock);
        s_remote_talker *talker = find_remote(remote_talker);
        HRESULT result = talker != NULL ? S_OK : E_INVALIDARG;
        if (talker != NULL) {
            DWORD packet_count = *size / XHV_VOICECHAT_MODE_PACKET_SIZE;
            if (talker->chat)
                xhv_voice_remote_submit(talker->slot, data, packet_count);
            *size = packet_count * XHV_VOICECHAT_MODE_PACKET_SIZE;
        }
        LeaveCriticalSection(&m_lock);
        return result;
    }

private:
    static void copy_modes(LPVOID *target, DWORD *target_count, const XHV_PROCESSING_MODE *source,
        DWORD source_count)
    {
        *target_count = 0;
        for (DWORD i = 0; source != NULL && i < source_count && *target_count < k_maximum_modes; i++)
            target[(*target_count)++] = source[i];
    }

    static bool enabled(const LPVOID *enabled_modes, DWORD enabled_count,
        const XHV_PROCESSING_MODE *modes, DWORD mode_count)
    {
        if (modes == NULL || mode_count == 0)
            return false;
        for (DWORD i = 0; i < mode_count; i++) {
            bool found = false;
            for (DWORD j = 0; j < enabled_count && !found; j++)
                found = modes[i] == enabled_modes[j];
            if (!found)
                return false;
        }
        return true;
    }

    static bool has_chat(const XHV_PROCESSING_MODE *modes, DWORD mode_count)
    {
        for (DWORD i = 0; i < mode_count; i++)
            if (modes[i] == XHV_VOICECHAT_MODE)
                return true;
        return false;
    }

    HRESULT set_local_modes(DWORD user_index, const XHV_PROCESSING_MODE *modes, DWORD mode_count, bool start)
    {
        if (user_index >= m_maximum_local_talkers || !enabled(m_local_modes, m_local_mode_count, modes, mode_count))
            return E_INVALIDARG;
        EnterCriticalSection(&m_lock);
        if (has_chat(modes, mode_count)) {
            if (start)
                m_local_chat |= 1u << user_index;
            else
                m_local_chat &= ~(1u << user_index);
            update_local(user_index);
        }
        LeaveCriticalSection(&m_lock);
        return S_OK;
    }

    HRESULT set_remote_modes(XUID remote_talker, const XHV_PROCESSING_MODE *modes, DWORD mode_count, bool start)
    {
        if (!enabled(m_remote_modes, m_remote_mode_count, modes, mode_count))
            return E_INVALIDARG;
        EnterCriticalSection(&m_lock);
        s_remote_talker *talker = find_remote(remote_talker);
        if (talker != NULL && has_chat(modes, mode_count))
            talker->chat = start;
        LeaveCriticalSection(&m_lock);
        return talker != NULL ? S_OK : E_INVALIDARG;
    }

    // Under the lock.
    void update_local(DWORD user_index)
    {
        DWORD bit = 1u << user_index;
        xhv_voice_local_enable(user_index, (m_local_talkers & m_local_chat & bit) != 0);
    }

    // Tracks live microphone changes made from Guide Settings (xhv_voice_refresh).
    void sync_headsets()
    {
        if (xhv_voice_headset_mask() == m_headsets)
            return;
        EnterCriticalSection(&m_lock);
        sync_headsets_locked();
        LeaveCriticalSection(&m_lock);
    }

    // Under the lock.
    void sync_headsets_locked()
    {
        DWORD now = xhv_voice_headset_mask();
        if (now == m_headsets)
            return;
        m_headsets = now;
        update_output_volumes();
        for (DWORD i = 0; i < k_maximum_remote_talkers; i++)
            if (m_talkers[i].in_use)
                set_talker_volumes(&m_talkers[i]);
    }

    // Under the lock.
    void update_output_volumes()
    {
        if (m_output == NULL)
            return;
        XAUDIOCHANNELVOLUMEENTRY volumes[2 * k_maximum_local_talkers];
        for (DWORD u = 0; u < k_maximum_local_talkers; u++) {
            XAUDIOVOLUME volume = (m_headsets & (1u << u)) ? 1.0f : 0.0f;
            volumes[2 * u].EntryIndex = (XAUDIOCHANNEL)(2 * u);
            volumes[2 * u].Volume = volume;
            volumes[2 * u + 1].EntryIndex = (XAUDIOCHANNEL)(2 * u + 1);
            volumes[2 * u + 1].Volume = volume;
        }
        XAUDIOCHANNELVOLUME channel_volume = {(XAUDIOCHANNEL)(2 * k_maximum_local_talkers), volumes};
        XAUDIOVOICEOUTPUTVOLUMEENTRY entry = {0, &channel_volume};
        XAUDIOVOICEOUTPUTVOLUME output_volume = {1, &entry};
        XAudioVoice_SetVoiceOutputVolume(m_output, &output_volume);
    }

    // Under the lock.
    s_remote_talker *find_remote(XUID remote_talker)
    {
        for (DWORD i = 0; i < k_maximum_remote_talkers; i++)
            if (m_talkers[i].in_use && m_talkers[i].xuid == remote_talker)
                return &m_talkers[i];
        return NULL;
    }

    // The headset output: users' channels to the front pair of the mastering voice (NULL: XAudio is not running).
    void create_output()
    {
        XAUDIOCHANNELMAPENTRY entries[2 * k_maximum_local_talkers];
        for (DWORD u = 0; u < k_maximum_local_talkers; u++) {
            XAUDIOVOLUME volume = (m_headsets & (1u << u)) ? 1.0f : 0.0f;
            entries[2 * u].InputChannel = (XAUDIOCHANNEL)u;
            entries[2 * u].OutputChannel = 0;
            entries[2 * u].Volume = volume;
            entries[2 * u + 1].InputChannel = (XAUDIOCHANNEL)u;
            entries[2 * u + 1].OutputChannel = 1;
            entries[2 * u + 1].Volume = volume;
        }
        XAUDIOCHANNELMAP map = {(XAUDIOCHANNEL)(2 * k_maximum_local_talkers), entries};
        XAUDIOVOICEOUTPUTENTRY output_entry = {NULL, &map};
        XAUDIOVOICEOUTPUT output = {1, &output_entry};
        XAUDIOSUBMIXVOICEINIT init = {};
        init.Format.SampleType = XAUDIOSAMPLETYPE_FLOAT;
        init.Format.ChannelCount = (XAUDIOCHANNEL)XHV_XAUDIO_OUTPUT_CHANNEL_COUNT;
        init.Format.SampleRate = 48000;
        init.MaxOutputVoiceCount = 1;
        init.MaxChannelMapEntryCount = (XAUDIOCHANNEL)(2 * k_maximum_local_talkers);
        init.SubmixStage = 1;
        init.pVoiceOutput = &output;
        if (FAILED(XAudioCreateSubmixVoice(&init, &m_output)))
            m_output = NULL;
    }

    static void CALLBACK talker_process(LPCXAUDIOVOICEPROCESSCALLBACK data)
    {
        s_remote_talker *talker = (s_remote_talker *)data->pVoiceContext;
        while (talker->voice != NULL && talker->queued < k_packets_ahead) {
            short *pcm = talker->pcm[talker->next_buffer];
            xhv_voice_remote_render(talker->slot, pcm);
            XAUDIOPACKET packet = {};
            packet.pBuffer = pcm;
            packet.BufferSize = k_frame_samples * sizeof(short);
            if (FAILED(XAudioSourceVoice_SubmitPacket(talker->voice, &packet, 0)))
                break;
            talker->queued++;
            talker->next_buffer = (talker->next_buffer + 1) % k_pcm_buffers;
        }
    }

    static void CALLBACK talker_packet_done(LPCXAUDIOVOICEPACKETCALLBACK data)
    {
        s_remote_talker *talker = (s_remote_talker *)data->pVoiceContext;
        if (talker->queued > 0)
            talker->queued--;
    }

    // Under the lock.
    void add_talker(s_remote_talker *talker, XUID xuid, LPCXAUDIOVOICEFXCHAIN effects, LPXAUDIOSUBMIXVOICE title_output)
    {
        talker->in_use = true;
        talker->chat = false;
        talker->xuid = xuid;
        talker->slot = (DWORD)(talker - m_talkers);
        talker->voice = NULL;
        talker->title_output = title_output;
        for (DWORD u = 0; u < k_maximum_local_talkers; u++)
            talker->priority[u] = XHV_PLAYBACK_PRIORITY_MAX;
        talker->queued = 0;
        talker->next_buffer = 0;
        xhv_voice_remote_open(talker->slot);
        if (m_output == NULL)
            return; // no XAudio: voice is taken (IsRemoteTalking follows arrivals) but not played
        XAUDIOSOURCEVOICEINIT init = {};
        init.Format.SampleType = XAUDIOSAMPLETYPE_16BITPCM;
        init.Format.ChannelCount = 1;
        init.Format.SampleRate = XHV_PCM_SAMPLE_RATE;
        init.MaxOutputVoiceCount = 2;
        init.MaxChannelMapEntryCount = (XAUDIOCHANNEL)k_maximum_local_talkers;
        init.MaxPacketCount = (XAUDIOPACKETINDEX)(k_packets_ahead + 1);
        init.pEffectChain = effects;
        init.pfnProcessCallback = talker_process;
        init.pfnPacketCompletionCallback = talker_packet_done;
        init.pContext = talker;
        IXAudioSourceVoice *voice = NULL;
        if (FAILED(XAudioCreateSourceVoice(&init, &voice)) || voice == NULL)
            return;
        talker->voice = voice; // from here the process callback fills it
        set_talker_outputs(talker);
        XAudioSourceVoice_Start(voice, 0);
    }

    // Under the lock. Releasing the voice waits out the XAudio frame in progress, so no callback runs after it.
    void remove_talker(s_remote_talker *talker)
    {
        if (talker->voice != NULL) {
            IXAudioSourceVoice *voice = talker->voice;
            XAudioVoice_Release(voice);
            talker->voice = NULL;
        }
        xhv_voice_remote_close(talker->slot);
        talker->in_use = false;
    }

    // Under the lock: the headset output (one map entry a user) and the title's output voice, if it gave one.
    void set_talker_outputs(s_remote_talker *talker)
    {
        XAUDIOCHANNELMAPENTRY headset_entries[k_maximum_local_talkers];
        for (DWORD u = 0; u < k_maximum_local_talkers; u++) {
            headset_entries[u].InputChannel = 0;
            headset_entries[u].OutputChannel = (XAUDIOCHANNEL)u;
            headset_entries[u].Volume = talker_volume(talker, u);
        }
        XAUDIOCHANNELMAPENTRY title_entry = {0, 0, 1.0f};
        XAUDIOCHANNELMAP maps[2] = {{(XAUDIOCHANNEL)k_maximum_local_talkers, headset_entries}, {1, &title_entry}};
        XAUDIOVOICEOUTPUTENTRY entries[2] = {{m_output, &maps[0]}, {talker->title_output, &maps[1]}};
        XAUDIOVOICEOUTPUT output = {(XAUDIOVOICEINDEX)(talker->title_output != NULL ? 2 : 1), entries};
        XAudioVoice_SetVoiceOutput(talker->voice, &output);
    }

    // Under the lock.
    void set_talker_volumes(s_remote_talker *talker)
    {
        if (talker->voice == NULL)
            return;
        XAUDIOCHANNELVOLUMEENTRY volumes[k_maximum_local_talkers];
        for (DWORD u = 0; u < k_maximum_local_talkers; u++) {
            volumes[u].EntryIndex = (XAUDIOCHANNEL)u;
            volumes[u].Volume = talker_volume(talker, u);
        }
        XAUDIOCHANNELVOLUME channel_volume = {(XAUDIOCHANNEL)k_maximum_local_talkers, volumes};
        XAUDIOVOICEOUTPUTVOLUMEENTRY entry = {0, &channel_volume};
        XAUDIOVOICEOUTPUTVOLUME volume = {1, &entry};
        XAudioVoice_SetVoiceOutputVolume(talker->voice, &volume);
    }

    XAUDIOVOLUME talker_volume(const s_remote_talker *talker, DWORD user_index) const
    {
        bool heard = (m_headsets & (1u << user_index)) != 0 &&
                     talker->priority[user_index] != XHV_PLAYBACK_PRIORITY_NEVER;
        return heard ? 1.0f : 0.0f;
    }

    LONG m_reference_count;
    CRITICAL_SECTION m_lock;
    DWORD m_maximum_local_talkers, m_maximum_remote_talkers;
    LPVOID m_local_modes[k_maximum_modes], m_remote_modes[k_maximum_modes]; // the enabled modes
    DWORD m_local_mode_count, m_remote_mode_count;
    DWORD m_local_talkers; // a bit per registered user index
    DWORD m_local_chat;    // a bit per user index in chat mode
    DWORD m_headsets;      // a bit per user index with a headset (xhv_voice_open)
    LPCXAUDIOVOICEFXCHAIN m_default_remote_effects;
    IXAudioSubmixVoice *m_output;
    DWORD m_maximum_decode_packets;
    s_remote_talker m_talkers[k_maximum_remote_talkers];
};

} // namespace

extern "C" {
CONST LPVOID _xhv_voicechat_mode = &g_voicechat_mode_tag;
CONST LPVOID _xhv_loopback_mode = &g_loopback_mode_tag;
}

// The engine's work runs on threads of its own (the voice runtime's capture thread, XAudio's), so there is no worker
// thread for the title to wait on: phWorkerThread gets no handle.
HRESULT XHVCreateEngine(PXHV_INIT_PARAMS parameters, PHANDLE worker_thread, PIXHVENGINE *engine)
{
    if (parameters == NULL || engine == NULL)
        return E_INVALIDARG;
    if (worker_thread != NULL)
        *worker_thread = NULL;
    c_xhv_engine *created = new c_xhv_engine(*parameters);
    if (created == NULL)
        return E_OUTOFMEMORY;
    *engine = created;
    return S_OK;
}
