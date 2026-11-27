// port/sdk/xam: the guide's dialogs as XAM sees them (xam.c), for the calls that show them: the virtual keyboard
// (port/sdk/online/keyboard_ui.cpp) and the guide (guide.c). The guide shows one dialog at a time: none opens while
// another shows. Opening one tells every listener for system notifications XN_SYS_UI (1), closing it XN_SYS_UI (0);
// while one shows the controllers read as idle (XamInputGetState: the guide has the input), and the buttons, triggers
// and sticks held when it closes stay hidden from the title, each until it is let go (the B that closed the guide does
// not also back out of the title's menu, the stick that scrolled the guide does not move its selection). Any thread may
// open or close one.
#ifndef PORT_XAM_XAM_UI_H
#define PORT_XAM_XAM_UI_H

#ifdef __cplusplus
extern "C" {
#endif

// The virtual keyboard. Opens it and posts XN_SYS_UI (1). Returns 0, posting nothing, when a guide dialog shows already
// (the call then fails with ERROR_ACCESS_DENIED).
PORT_MS_ABI int port_xam_ui_open(void);
// Closes it and posts XN_SYS_UI (0).
PORT_MS_ABI void port_xam_ui_close(void);
// Whether it shows.
PORT_MS_ABI int port_xam_ui_showing(void);

// The guide (guide.h), interactive: begin returns 0, posting nothing, when a guide dialog shows already; otherwise the
// guide shows and XN_SYS_UI (1) is posted. End posts XN_SYS_UI (0).
int port_xam_guide_begin(void);
void port_xam_guide_end(void);
// The guide, completing a request at once (nobody operates it): XN_SYS_UI (1) now and XN_SYS_UI (0) once each listener
// has drained its queue in a later XNotifyGetNext loop (the title's UI expects to see the guide open for a frame).
// Returns 0, posting nothing, when a guide dialog shows already.
int port_xam_guide_pulse(void);
// A sign-in change (xam_profile.h): when `changed` (a bit per user) is not 0, every listener for system notifications
// is told XN_SYS_SIGNINCHANGED with the users signed in now (its dwValid).
void port_xam_signin_changed(unsigned long changed);
// A game invite accepted by user `user_index` (port/sdk/online/invite.c): every listener for Live notifications is told
// XN_LIVE_INVITE_ACCEPTED (0x02000002) with the user's index; the title then reads it with XInviteGetAcceptedInfo.
void port_xam_invite_accepted(unsigned long user_index);
// The controller whose buttons changed last: the player who asked, for a call naming XUSER_INDEX_ANY.
PORT_MS_ABI unsigned int port_xam_last_used_controller(void);
// Polls every controller for the guide (XamInputGetState, whose result goes nowhere): for a call that waits for the
// guide on a thread that may be the one polling the controllers.
PORT_MS_ABI void port_xam_guide_poll(void);

#ifdef __cplusplus
}
#endif

#endif
