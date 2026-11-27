#ifndef XBOX_ONLINE_SERVICES_H
#define XBOX_ONLINE_SERVICES_H

struct XUSER_ACHIEVEMENT
{
    unsigned long dwUserIndex;
    unsigned long dwAchievementId;
};
extern "C" unsigned long XUserWriteAchievements(unsigned long count, const XUSER_ACHIEVEMENT *achievements, void *overlapped);

struct XACHIEVEMENT_DETAILS
{
    unsigned long dwId;
    wchar_t *pwszLabel;
    wchar_t *pwszDescription;
    wchar_t *pwszUnachieved;
    unsigned long dwImageId;
    unsigned long dwCred;
    struct { unsigned long dwLowDateTime; unsigned long dwHighDateTime; } ftAchieved;
    unsigned long dwFlags;
};

extern "C" unsigned long XUserCreateAchievementEnumerator(unsigned long title, long user, unsigned __int64 xuid, unsigned long detail_flags,
    unsigned long start, unsigned long count, unsigned long *buffer_size, void **handle);
extern "C" unsigned long XEnumerate(void *handle, void *buffer, unsigned long size, unsigned long *count, void *overlapped);
extern "C" unsigned long XInviteSend(unsigned long user_index, unsigned long flags, void *invitees, void *message, void *overlapped);
extern "C" unsigned long XShowGamerCardUI(unsigned long user_index, unsigned __int64 xuid);
extern "C" unsigned long XShowFriendsUI(unsigned long user_index);
extern "C" unsigned long XTitleServerCreateEnumerator(unsigned long title, unsigned long count, unsigned long *buffer_size, void **handle);
extern "C" unsigned long XUserSetContextEx(unsigned long, unsigned long, unsigned long, void *);

#endif // XBOX_ONLINE_SERVICES_H
