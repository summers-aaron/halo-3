#ifndef CSERIES_XDK_SESSION_H
#define CSERIES_XDK_SESSION_H

// Xbox session operations used by the managed-session synchronizer.
extern "C" unsigned long XSessionModify(void *session, unsigned long flags, unsigned long public_slot_count,
    unsigned long private_slot_count, void *overlapped);
extern "C" unsigned long XSessionJoinRemote(void *session, long player_count, const unsigned __int64 *player_xuids, const int *private_slots, void *overlapped);
extern "C" unsigned long XSessionLeaveRemote(void *session, long player_count, const unsigned __int64 *player_xuids, void *overlapped);

#endif // CSERIES_XDK_SESSION_H
