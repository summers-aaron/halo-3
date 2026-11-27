// networkinglibxbox/voice_availability.obj: from the records 0x41dfa0 0x41e008
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "game/game_types.h"
// @address 0x41e008 ?update_local_outgoing_player_masks@c_voice_availability@@AAAXXZ
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "networking/online/voice_xenon.h"
#include "networking/voice_processing_mode_types.h"
#include "networking/online/voice_interface_types.h"
#include "networking/online/voice_availability.h"
#include "networking/online/voice_observer_types.h"
namespace
{
    static __forceinline bool voice_availability_player_is_local(long player_index, bool voice_interface_initialized)
    {
        bool local = false;
        if (voice_interface_initialized)
        {
            local = g_voice_interface_globals.player_states[player_index].status_flags.test(_voice_player_local);
        }
        return local;
    }
}

static __forceinline bool voice_availability_player_is_valid(long player_index, bool voice_interface_initialized)
{
    bool result = false;
    if (voice_interface_initialized)
        result = (g_voice_interface_globals.player_states[player_index].status_flags.m_storage & 1) != 0;
    return result;
}
static __forceinline bool voice_availability_player_has_voice(long player_index, bool voice_interface_initialized)
{
    bool result = false;
    if (voice_interface_initialized)
        result = ((g_voice_interface_globals.player_states[player_index].status_flags.m_storage >> 3) & 1) != 0;
    return result;
}
static __forceinline unsigned long voice_availability_player_mute_mask(long player_index, bool voice_interface_initialized)
{
    unsigned long mask = 0;
    if (voice_interface_initialized)
        mask = g_voice_interface_globals.player_states[player_index].muted_players;
    return mask;
}
static __forceinline unsigned long voice_availability_player_flags(long player_index, bool voice_interface_initialized)
{
    unsigned long flags = 0;
    if (voice_availability_player_is_valid(player_index, voice_interface_initialized))
        flags = g_voice_interface_globals.player_states[player_index].voice_flags;
    return flags;
}
static __forceinline bool voice_availability_player_flag(long player_index, long flag, bool voice_interface_initialized)
{
    bool result = false;
    if (voice_interface_initialized)
        result = ((voice_availability_player_flags(player_index, voice_interface_initialized) >> flag) & 1) != 0;
    return result;
}
static __forceinline bool voice_availability_player_muted(long player_index, long other_player_index, bool voice_interface_initialized)
{
    bool result = false;
    if (voice_interface_initialized)
    {
        if (voice_availability_player_is_valid(player_index, voice_interface_initialized))
            result = (g_voice_interface_globals.player_states[player_index].muted_players & (1 << other_player_index)) != 0;
    }
    return result;
}

void c_voice_availability::update_local_outgoing_player_masks()
{
    // match: 0x41e008: folds this to &g_voice_availability so initialized and outgoing_player_masks use an absolute lis/addi base instead of r3 loads
    __assume(this == &g_voice_availability);
    if (initialized)
    {
        bool voice_interface_initialized = g_voice_interface_globals.initialized;
        unsigned long remote_player_mask = 0;
        for (long player_index = 0; player_index < 16; ++player_index)
            if (voice_availability_player_is_valid(player_index, voice_interface_initialized) &&
                !voice_availability_player_is_local(player_index, voice_interface_initialized))
                remote_player_mask |= 1 << player_index;

        memset(outgoing_player_masks.get_elements(), 0, sizeof(outgoing_player_masks));
        for (long local_player_index = 0; local_player_index < 16; ++local_player_index)
        {
            if (voice_availability_player_is_valid(local_player_index, voice_interface_initialized) &&
                voice_availability_player_is_local(local_player_index, voice_interface_initialized) &&
                voice_availability_player_has_voice(local_player_index, voice_interface_initialized))
            {
                unsigned long outgoing_player_mask = remote_player_mask;
                unsigned long mute_mask = voice_availability_player_mute_mask(local_player_index, voice_interface_initialized);
                mute_mask = ~mute_mask;
                outgoing_player_mask &= mute_mask;
                if (voice_lifecycle_in_game())
                {
                    outgoing_player_mask &= g_voice_observer.get_player_in_game_mask(local_player_index);
                    voice_interface_initialized = g_voice_interface_globals.initialized;
                }
                else if (voice_lifecycle_out_of_game())
                    outgoing_player_mask &= g_voice_observer.get_player_out_of_game_mask(local_player_index);
                if (voice_lifecycle_in_matchmaking() && voice_availability_player_flag(local_player_index, 4, voice_interface_initialized))
                    outgoing_player_mask &= g_voice_observer.get_player_team_mask(local_player_index) | voice_get_player_peer_mask(local_player_index);
                for (long remote_player_index = 0; remote_player_index < 16; ++remote_player_index)
                {
                    if (outgoing_player_mask & (1 << remote_player_index))
                    {
                        bool team_only_restricted = false;
                        if (voice_lifecycle_in_matchmaking() &&
                            voice_availability_player_flag(remote_player_index, 4, voice_interface_initialized) &&
                            !((unsigned long)voice_get_player_peer_mask(remote_player_index) & (1 << local_player_index)))
                        {
                            if (voice_lifecycle_in_game())
                            {
                                team_only_restricted = !voice_players_are_allies(local_player_index, remote_player_index);
                                voice_interface_initialized = g_voice_interface_globals.initialized;
                            }
                            else if (voice_lifecycle_out_of_game())
                                team_only_restricted = !voice_players_on_same_team(local_player_index, remote_player_index);
                        }
                        if (voice_availability_player_muted(remote_player_index, local_player_index, voice_interface_initialized) ||
                            voice_availability_player_flag(remote_player_index, 1, voice_interface_initialized) ||
                            voice_availability_player_flag(remote_player_index, 2, voice_interface_initialized) || team_only_restricted)
                            outgoing_player_mask &= ~(1 << remote_player_index);
                    }
                }
                outgoing_player_masks[local_player_index] = outgoing_player_mask;
            }
        }
    }
}
unsigned long c_voice_availability::get_outgoing_player_mask(long local_player_index)
{
    // match: 0x41dfa0: folds this to &g_voice_availability so outgoing_player_masks indexes off an absolute addi base instead of mr r9,r3
    __assume(this == &g_voice_availability);
    unsigned long outgoing_player_mask = 0;
    if (initialized)
    {
        if (voice_availability_player_is_local(local_player_index, g_voice_interface_globals.initialized))
        {
            outgoing_player_mask = outgoing_player_masks[local_player_index];
        }
    }
    return outgoing_player_mask;
}
