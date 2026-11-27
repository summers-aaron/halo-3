// port/sdk/xam: the console's local profiles and who is signed in on users 0..3 (profiles.c), shared with XAM (xam.c),
// the guide (guide.c) and the profile settings and other user-facing services (port/sdk/online/xlive_offline.cpp).
//
// Profiles: the built-in "Player 1" .. "Player 4" (Player 1's gamertag is HALO3_PROFILE_NAME when given) and those the
// guide creates, which the data directory keeps (profiles/catalog.txt). Each has a gamertag and its own offline XUID,
// stable across runs, under which the port keeps its saves. A user (a controller's player) is signed in to a profile
// (one profile per user at a time), as a guest of a signed-in profile, or not at all. A guest is the console's: the
// gamertag "<sponsor's gamertag> (<n>)", guest number n = 1..3 counted per sponsor, no offline XUID of its own (a title
// asking for it gets INVALID_XUID), the sponsor's sign-in state, nothing saved; signing the sponsor out signs its guests
// out with it. At start-up users 0..HALO3_LOCAL_PLAYERS-1 (default user 0) are signed in to the built-in profiles of the
// same number; HALO3_NO_PROFILE (set to anything but 0) leaves the console with no built-in profile and nobody signed in.
// Every function may be called from any thread.
#ifndef PORT_XAM_PROFILE_H
#define PORT_XAM_PROFILE_H

// An offline XUID (the 0xE000... range a console gives profiles that have no Xbox Live account): Player 1's default.
#define PORT_PROFILE_OFFLINE_XUID 0xE000000000000001ull
#define PORT_PROFILE_USERS 4u
#define PORT_PROFILE_BUILTIN 4u        // "Player 1" .. "Player 4", catalog indices 0..3
#define PORT_PROFILE_CATALOG_MAX 16u   // built-in and created profiles together
#define PORT_PROFILE_GAMERTAG_MAX 15u  // XUSER_MAX_NAME_LENGTH
#define PORT_PROFILE_GUESTS 3u         // guests per sponsor

// How a user is signed in.
enum {
    PORT_SIGNIN_NONE = 0,
    PORT_SIGNIN_PROFILE = 1,
    PORT_SIGNIN_GUEST = 2,
};

// It crosses the port's two i686 ABIs (xlive_offline.cpp is built like the game, MSVC layout; xam.c as host code, System
// V): the XUID first, the size a multiple of 8, so both lay it out alike.
typedef struct port_profile_user_info {
    unsigned long long xuid;     // the port's key for the user's data: a profile's offline XUID; a guest's is its
                                 // sponsor's with the guest number in bits 54..55 (never shown to the title)
    int kind;                    // PORT_SIGNIN_*
    unsigned int profile;        // PORT_SIGNIN_PROFILE: the catalog index; a guest: its sponsor's profile
    unsigned int sponsor;        // PORT_SIGNIN_GUEST: the sponsoring user's index
    unsigned int guest_number;   // PORT_SIGNIN_GUEST: 1..3
    unsigned int serial;         // a new value every time the user signs in (the port's stores reload on a change)
    char name[PORT_PROFILE_GAMERTAG_MAX + 1]; // the gamertag ("<sponsor> (<n>)" for a guest)
    unsigned int reserved;       // 0
} port_profile_user_info;
typedef char port_profile_user_info_layout[sizeof(port_profile_user_info) == 48 ? 1 : -1];

#ifdef __cplusplus
extern "C" {
#endif

// ---- users ----

// Whether `user_index` (0..3) is signed in (a profile or a guest).
PORT_MS_ABI int port_profile_signed_in(unsigned long user_index);
// A bit per signed-in user: XN_SYS_SIGNINCHANGED's dwValid.
unsigned long port_profile_signed_in_mask(void);
// Copies user `user_index`'s sign-in. Returns 0 (info->kind PORT_SIGNIN_NONE, the rest zero) when nobody is signed in.
int port_profile_user(unsigned long user_index, port_profile_user_info *info);
// Whether the user is signed in as a guest.
PORT_MS_ABI int port_profile_user_is_guest(unsigned long user_index);
// port_profile_user's xuid and serial: 0 when the user is not signed in.
PORT_MS_ABI unsigned long long port_profile_user_xuid(unsigned long user_index);
PORT_MS_ABI unsigned long port_profile_user_serial(unsigned long user_index);
// Whether `xuid` names user `user_index`: the key of its data (port_profile_user_xuid) or, for user 0 signed in to Xbox
// Live, the account's XUID (what XUserGetXUID reports then). Implemented by xam.c, which knows Xbox Live.
PORT_MS_ABI int port_profile_user_owns_xuid(unsigned int user_index, unsigned long long xuid);

// Sign-in changes, as the guide makes them; they post no notification (xam_ui.h does). Each returns 0
// (ERROR_SUCCESS) or a Win32 error, and puts in *changed (when given) a bit per user whose sign-in changed:
//   - sign in: `user_index` to catalog profile `profile` (replacing whoever was signed in there, with their guests).
//     ERROR_INVALID_PARAMETER for an index out of range, ERROR_ALREADY_ASSIGNED when the profile is signed in on
//     another user, ERROR_NOT_FOUND when no profile exists (HALO3_NO_PROFILE and none created);
//   - sign in a guest: `user_index` becomes a guest of the sponsor port_profile_guest_sponsor picks, with the lowest
//     guest number that sponsor has free. ERROR_NO_SUCH_USER when no profile is signed in to sponsor one,
//     ERROR_TOO_MANY_NAMES (68) when that sponsor has 3 guests already;
//   - sign out: `user_index` and the guests it sponsors. ERROR_NOT_LOGGED_ON when nobody is signed in there.
unsigned long port_profile_sign_in(unsigned long user_index, unsigned long profile, unsigned long *changed);
unsigned long port_profile_sign_in_guest(unsigned long user_index, unsigned long *changed);
unsigned long port_profile_sign_out(unsigned long user_index, unsigned long *changed);
// The user a guest signing in on `user_index` would be the guest of: the lowest-numbered user other than it signed in
// to a profile (a guest cannot sponsor one); -1 when there is none.
long port_profile_guest_sponsor(unsigned long user_index);
// "<sponsor> (<n>)", the sponsor's gamertag shortened (never the suffix) so the whole fits 15 characters.
void port_profile_guest_name(const char *sponsor, unsigned long guest_number, char name[PORT_PROFILE_GAMERTAG_MAX + 1]);

// ---- the catalog ----

// How many profiles exist (built-in ones first), and profile `index`'s gamertag and offline XUID. Get returns 0 for an
// index out of range.
unsigned long port_profile_count(void);
int port_profile_get(unsigned long index, char gamertag[PORT_PROFILE_GAMERTAG_MAX + 1], unsigned long long *xuid);
// The user profile `index` is signed in on, -1 when none.
long port_profile_signed_in_on(unsigned long index);
// Whether `gamertag` is one a console accepts: 1..15 letters, digits and spaces, starting with a letter, not ending in a
// space, no two spaces in a row.
int port_profile_gamertag_valid(const char *gamertag);
// Check local availability before registering an online name. Creation rechecks under the catalog lock.
unsigned long port_profile_create_error(const char *gamertag);
// Creates a profile (the catalog file rewritten atomically) and puts its index in *index. Its offline XUID is derived
// from the gamertag as HALO3_PROFILE_NAME's is, so the same name finds the same saves. ERROR_INVALID_NAME for a gamertag
// port_profile_gamertag_valid refuses, ERROR_ALREADY_EXISTS for one in use (letter case ignored; the catalog file's
// unlisted lines count), ERROR_TOO_MANY_NAMES when PORT_PROFILE_CATALOG_MAX - PORT_PROFILE_BUILTIN profiles were created
// already, the error reading the catalog file when it could not be read (it is never overwritten then), or the error
// writing it (nothing created then).
unsigned long port_profile_create(const char *gamertag, unsigned long *index);
// The first "Player <n>" (n from 5) no profile has: the name the guide offers for a new profile.
void port_profile_suggest_gamertag(char gamertag[PORT_PROFILE_GAMERTAG_MAX + 1]);

// ---- start-up settings ----

// Enabled by default on PC (set HALO3_COOP_4P=0 to restore the Xbox 360 2-player cap): a campaign takes up to 4 local
// split-screen players, where the Xbox 360 game takes 2 (its lobby's "too many local players for co-op"). Read at the
// first call. The game declares it itself (tu/networkinglibxbox/life_cycle_handler_pre_game.cpp,
// LIFE_CYCLE_PRE_GAME_MAXIMUM_CAMPAIGN_LOCAL_PLAYERS).
PORT_MS_ABI int port_coop_4p_enabled(void);
// XShowSigninUI: opens the guide's sign-in panes (guide.h), or, when nobody can operate the guide, signs in the users of
// connected controllers that are not signed in (xam.c). Returns a Win32 error code.
PORT_MS_ABI unsigned int port_profile_show_signin(unsigned int panes, unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif
