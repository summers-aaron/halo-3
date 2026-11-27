// port/sdk/online (both hosts): game invites (invite.c): the lobby this console's players are in, the lobby codes that
// carry it from one PC to another, the invites received, and accepting one.
//
// On the console an invite is Xbox Live's. XInviteSend sends friends the session the inviter's presence is in, as an
// XSESSION_INFO (its XNKID, the host's XNADDR, the key exchange key); accepting one in the Guide makes XAM post
// XN_LIVE_INVITE_ACCEPTED (XNID(0, _XNAREA_LIVE, 2) = 0x02000002, the accepting user's index as its parameter), and the
// title reads the invite with XInviteGetAcceptedInfo and joins the host. The port does the same, with two ways for an
// invite to travel:
//   - a lobby code: the lobby's XSESSION_INFO and the inviter as text ("H3-...", below), which the Guide copies to the
//     host's clipboard and shows (Copy Lobby Code) and takes pasted or typed (Join by Lobby Code). No server;
//   - Live (HALO3_LIVE_SERVER, live.h): the Live server's friends and invites, sent from the Guide's Friends page and
//     polled for in the background (port_invite_received).
// Accepting either is port_invite_accept, which is what the console's Guide does on Accept.
//
// The lobby. A console's invite names the session that has the inviter's presence: the one its title created with
// XSESSION_CREATE_USES_PRESENCE (xlive_offline.cpp reports XSessionCreate, XSessionModify, XSessionMigrateHost and
// XSessionDelete: port_invite_presence_session). Halo 3's System Link lobby has no XSession at all (its sessions are
// keys registered with XNet), so the game also reports its own squad session, the lobby its players see, once per UI
// update (port/probe/extra/invite_lobby.cpp: port_invite_set_squad). Once the game reports (Halo 3 does from its
// first UI update, "none" included), the lobby an invite names is its squad session alone (a Live squad only while its
// presence session allows invites); a title that never reports one gets the console's rule: the presence session.
//
// A lobby code is "H3-" and Crockford base 32 (0-9 A-Z without I L O U; read back with case, spaces and dashes
// ignored, O as 0, I and L as 1) in groups of five, of:
//   version (1), session class (1: bits 0-1, PORT_INVITE_CLASS_*; bit 2: an inviter XUID follows),
//   XNKID (8), XNKEY (16),
//   the host's XNADDR compacted (lossless): a flags byte (bits 0-1: inaOnline 0, = ina, or given; bits 3-7: how many
//   of abOnline's 20 bytes are given, the rest zero), ina (4), [inaOnline (4)], wPortOnline (2), abEnet (6), abOnline,
//   [inviter XUID (8)], the inviter's gamertag (a length byte, then UTF-8, at most 15 bytes),
//   CRC-32 (IEEE) of everything before it, little-endian.
// The XNADDR's and XUID's bytes are in the console's order (as XSESSION_INFO holds them); a System Link lobby code is
// about 85 characters.
//
// Threads: any. One lock, taken briefly; nothing is called under it but copies.
#ifndef PORT_ONLINE_INVITE_H
#define PORT_ONLINE_INVITE_H

#include "../runtime/port_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_INVITE_SESSION_SIZE 60u // XSESSION_INFO: XNKID (8), the host's XNADDR (36), XNKEY (16)
#define PORT_INVITE_NAME 16u         // a gamertag, NUL included (XUSER_NAME_SIZE)
#define PORT_INVITE_ID 40u           // a Live invite's id, NUL included
#define PORT_INVITE_CODE_MAX 180u    // a lobby code's characters, NUL included (the longest, a Live lobby's: 178)
#define PORT_INVITE_INBOX 8u         // invites kept (the newest)
#define PORT_INVITE_TITLE_ID 0x4D5307E6ul

// The game's e_network_session_class of a lobby.
enum port_invite_class {
    PORT_INVITE_CLASS_NONE = 0,        // offline: nobody can join
    PORT_INVITE_CLASS_SYSTEM_LINK = 1,
    PORT_INVITE_CLASS_LIVE = 2,
};

// An invite: the lobby and who sent it. It crosses the port's two i686 ABIs (xlive_offline.cpp is built like the
// game, MSVC layout; invite.c as host code, System V): the 64-bit members first, the size a multiple of 8.
typedef struct port_invite {
    unsigned long long inviter_xuid;                  // 0: not known
    unsigned long long invitee_xuid;                  // the accepting user's XUID (port_invite_accept fills it)
    unsigned char session[PORT_INVITE_SESSION_SIZE];  // XSESSION_INFO
    int session_class;                                // PORT_INVITE_CLASS_*: SYSTEM_LINK or LIVE
    unsigned int title_id;                            // PORT_INVITE_TITLE_ID
    char inviter[PORT_INVITE_NAME];                   // UTF-8 gamertag, "" when not known
    char id[PORT_INVITE_ID];                          // a Live invite's id; "" for a lobby code
    unsigned int received;                            // GetTickCount when it arrived (inbox)
    int from_game_invite;                             // XINVITE_INFO::fFromGameInvite: 1 for an invite or a code
    int reserved;                                     // 0 (the size a multiple of 8 in both ABIs)
} port_invite;
typedef char port_invite_layout[
    __builtin_offsetof(port_invite, session) == 16 && sizeof(port_invite) == 152 ? 1 : -1];

// What a lobby is, as the game or XSessionCreate reports it.
typedef struct port_invite_lobby {
    unsigned char session[PORT_INVITE_SESSION_SIZE]; // XSESSION_INFO (the host's)
    int session_class;                               // PORT_INVITE_CLASS_*
    int host;                                        // this console hosts it
} port_invite_lobby;
typedef char port_invite_lobby_layout[sizeof(port_invite_lobby) == 68 ? 1 : -1];

// ---- the lobby ----

// The game's squad session (invite_lobby.cpp), once per UI update: 0 or a lobby of class NONE when the players are in
// no lobby another console could join.
PORT_MS_ABI void port_invite_set_squad(const port_invite_lobby *lobby);
// A Live session (xlive_offline.cpp): created or changed (XSessionCreate, XSessionModify, XSessionMigrateHost) with
// its XSESSION_CREATE_* flags, or deleted (XSessionDelete). `handle` is the title's session handle. Only a session with
// XSESSION_CREATE_USES_PRESENCE is the presence session; invites name it unless XSESSION_CREATE_INVITES_DISABLED.
PORT_MS_ABI void port_invite_presence_session(unsigned int handle, const unsigned char session[PORT_INVITE_SESSION_SIZE],
    unsigned int flags, int host);
PORT_MS_ABI void port_invite_presence_session_deleted(unsigned int handle);
// The lobby an invite names now (see the top). Returns 1 with it in *lobby, 0 when there is none; *why (when given)
// then says why, for the Guide ("" when there is a lobby).
PORT_MS_ABI int port_invite_current_lobby(port_invite_lobby *lobby, const char **why);
// The Live presence session invites name (XInviteSend's): 1 with it in *session.
PORT_MS_ABI int port_invite_presence(unsigned char session[PORT_INVITE_SESSION_SIZE]);

// ---- lobby codes ----

enum port_invite_code_result {
    PORT_INVITE_CODE_OK = 0,
    PORT_INVITE_CODE_EMPTY,      // nothing typed
    PORT_INVITE_CODE_PREFIX,     // does not start with H3-
    PORT_INVITE_CODE_CHARACTER,  // a character no code has
    PORT_INVITE_CODE_SHORT,      // too short: a part is missing
    PORT_INVITE_CODE_LONG,       // longer than any code
    PORT_INVITE_CODE_CHECKSUM,   // a character is wrong
    PORT_INVITE_CODE_VERSION,    // made by a newer port
    PORT_INVITE_CODE_INVALID,    // well-formed but not a lobby (no class, no host address)
};
// The code of `invite` (its session, session_class, inviter and inviter_xuid) into `code` (capacity characters, NUL
// included; PORT_INVITE_CODE_MAX always suffices). Returns its length, 0 when it does not fit or names no lobby.
PORT_MS_ABI unsigned int port_invite_code_encode(const port_invite *invite, char *code, unsigned int capacity);
// `text` read as a lobby code: PORT_INVITE_CODE_OK with *invite filled (from_game_invite 1, title_id the game's, the
// rest as encoded; id "", received 0) or what is wrong with it.
PORT_MS_ABI int port_invite_code_decode(const char *text, port_invite *invite);
// A sentence for the Guide saying what a decode result means.
PORT_MS_ABI const char *port_invite_code_message(int result);
// The code shown as lines of at most `groups_per_line` groups, as even as can be ("H3-XXXXX-XXXXX-..." broken after
// a dash), into `out`.
PORT_MS_ABI void port_invite_code_lines(const char *code, unsigned int groups_per_line, char *out, unsigned int capacity);
// The lobby code of the current lobby with user `user_index` as the inviter. Returns its length, 0 with *why (when
// given) set when there is no lobby.
PORT_MS_ABI unsigned int port_invite_lobby_code(unsigned int user_index, char *code, unsigned int capacity, const char **why);

// ---- invites received (Live) ----

// An invite that arrived (live.c's poll). Returns 1 when it is new (an id not received lately: the server delivers
// an invite until it is answered): it is kept, the newest PORT_INVITE_INBOX (the oldest goes, answered as declined),
// and a Game Invite toast shows. Not under any lock of the caller's (an invite answered is HTTP).
PORT_MS_ABI int port_invite_received(const port_invite *invite);
// The invites kept, newest first. Returns how many were copied.
PORT_MS_ABI unsigned int port_invite_inbox(port_invite *out, unsigned int capacity);
// How many invites are kept.
PORT_MS_ABI unsigned int port_invite_inbox_count(void);
// Removes the invite with id `id` (accepted or declined); with Live, the server is told (`accepted`), once per id.
// Not under any lock of the caller's: the server is told over HTTP.
PORT_MS_ABI void port_invite_answered(const char *id, int accepted);

// ---- accepting ----

enum port_invite_error {
    PORT_INVITE_OK = 0,
    PORT_INVITE_E_NOT_SIGNED_IN,  // the accepting user is not signed in
    PORT_INVITE_E_SYSTEM_LINK,    // a System Link lobby, and System Link is off on this PC (HALO3_LAN=1)
    PORT_INVITE_E_LIVE,           // a Live lobby, and the user is not signed in to Live
    PORT_INVITE_E_OWN,            // the user's own lobby
    PORT_INVITE_E_INVALID,        // not a lobby
};
// Whether user `user_index` can accept `invite`: PORT_INVITE_OK or why not.
PORT_MS_ABI int port_invite_check(unsigned int user_index, const port_invite *invite);
PORT_MS_ABI const char *port_invite_error_message(int error);
// Accepts `invite` for user `user_index`, as the console's Guide does: checked (port_invite_check), kept as the user's
// accepted invite (port_invite_accepted, XInviteGetAcceptedInfo) and XN_LIVE_INVITE_ACCEPTED (the user's index) posted
// to the title's listeners; the title then joins the lobby. Returns PORT_INVITE_OK or why not (nothing posted).
PORT_MS_ABI int port_invite_accept(unsigned int user_index, const port_invite *invite);
// The invite user `user_index` accepted last (XInviteGetAcceptedInfo): 1 with it in *invite, 0 when none.
PORT_MS_ABI int port_invite_accepted(unsigned int user_index, port_invite *invite);

// HALO3_INVITE_TRACE=1: a line on stderr for each lobby change, code made or read, invite received and accepted.
PORT_MS_ABI int port_invite_trace(void);

#ifdef __cplusplus
}
#endif

#endif
