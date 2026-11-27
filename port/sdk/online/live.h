// port/sdk/online: clean Xbox Live client for halo-3-server (slopboxlive.com / zboxlive.com).
// Plain C types only: included by both host C code (xam.c, xnet.c, lan.c) and game-ABI C++ code
// (xlive_offline.cpp, winsock_offline.cpp).
//
// Activated when HALO3_LIVE_SERVER is set (e.g. "http://127.0.0.1:4000" or "http://slopboxlive.com").
// Optional environment variables:
//   HALO3_GAMERTAG     - desired gamertag (default: the signed-in local profile's name)
//   HALO3_LIVE_ACCOUNT - legacy account JSON to migrate (default live_account.json under HALO3_DATA)
//   HALO3_LIVE_LOG     - set to 1 for verbose Live HTTP/session logging to stderr
#ifndef PORT_ONLINE_LIVE_H
#define PORT_ONLINE_LIVE_H

#include "../runtime/port_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_LIVE_TCP_HANDLE_BASE 0x1000u

enum {
    PORT_LIVE_PROP_INT32 = 1,
    PORT_LIVE_PROP_INT64 = 2,
    PORT_LIVE_PROP_DOUBLE = 3,
    PORT_LIVE_PROP_FLOAT = 5,
};

typedef struct port_live_title_server {
    unsigned int address; // IPv4 in host byte order
    unsigned short port;
    unsigned int flags;
    char description[200];
} port_live_title_server;

typedef struct port_live_property {
    unsigned int id;
    unsigned int type; // PORT_LIVE_PROP_*
    long long int_val;
    double double_val;
} port_live_property;

typedef struct port_live_stats_row {
    unsigned long long xuid;
    long long rating;
    char gamertag[16];
    unsigned int rank;
    unsigned int column_count;
} port_live_stats_row;

// One requested view. The caller supplies room for xuid_count rows and xuid_count * column_count
// properties. Returned row i uses columns starting at i * column_count.
typedef struct port_live_stats_view {
    unsigned int view_id;
    unsigned int column_count;
    unsigned short column_ids[64]; // XUSER_STATS_ATTRS_IN_SPEC
    unsigned int row_count;
    port_live_stats_row *rows;
    port_live_property *columns;
} port_live_stats_view;

typedef struct port_live_context {
    unsigned int id;
    unsigned int value;
} port_live_context;

typedef struct port_live_search_result {
    unsigned char xnkid[8];
    unsigned char xnaddr[36];
    unsigned char xnkey[16];
    unsigned int open_public;
    unsigned int open_private;
    unsigned int filled_public;
    unsigned int filled_private;
} port_live_search_result;

typedef struct port_live_arb_machine {
    unsigned long long machine_id;
    unsigned int player_count;
    unsigned int _pad; // align xuids to 8 bytes on both SysV i386 (live.c) and MSVC i386 (xlive_offline.cpp)
    unsigned long long xuids[16];
} port_live_arb_machine;
typedef char port_live_arb_machine_size_check[sizeof(port_live_arb_machine) == 144 ? 1 : -1];

// ---- lifecycle & identity ----

PORT_MS_ABI int port_live_enabled(void);
PORT_MS_ABI int port_live_signed_in(void);
PORT_MS_ABI int port_live_startup(void);
PORT_MS_ABI void port_live_cleanup(void);
PORT_MS_ABI unsigned long long port_live_xuid(void);
const char *port_live_gamertag(void);
int port_live_user_enabled(unsigned int user_index);
int port_live_user_signed_in(unsigned int user_index);
int port_live_user_connecting(unsigned int user_index);
unsigned long long port_live_user_xuid(unsigned int user_index);
const char *port_live_user_gamertag(unsigned int user_index);
int port_live_profile_identity(unsigned int user_index, const char *profile, unsigned long long *xuid,
    char *gamertag, unsigned int capacity);
void port_live_sign_out(unsigned long user);

typedef enum port_live_auth_status {
    PORT_LIVE_AUTH_NONE,
    PORT_LIVE_AUTH_OK,
    PORT_LIVE_AUTH_RENAMED_SUFFIX,
    PORT_LIVE_AUTH_GAMERTAG_TAKEN,
    PORT_LIVE_AUTH_INVALID_GAMERTAG,
    PORT_LIVE_AUTH_TOKEN_REJECTED,
    PORT_LIVE_AUTH_PROFILE_HAS_ACCOUNT,
    PORT_LIVE_AUTH_RATE_LIMITED,
    PORT_LIVE_AUTH_UNREACHABLE,
    PORT_LIVE_AUTH_SERVER_ERROR,
    PORT_LIVE_AUTH_STORAGE_ERROR,
    PORT_LIVE_AUTH_UPGRADE_REQUIRED,
} port_live_auth_status;

typedef struct port_live_auth_result {
    port_live_auth_status status;
    char detail[128];
} port_live_auth_result;

// During Guide creation, authenticate and persist the account before committing the local catalog.
typedef struct {
    unsigned long long xuid;
    char gamertag[32];
    char token[129]; // 128 token bytes plus NUL
} port_live_account;

int port_live_prepare_profile(const char *profile, port_live_account *account, port_live_auth_result *result);
// Attach an authenticated account to the freshly selected profile. A stale slot generation rejects it.
int port_live_activate_profile(unsigned int user_index, unsigned int generation, const char *profile,
    const port_live_account *account, const port_live_auth_result *result);

// Schedule presence and service registration after releasing the Guide lock.
void port_live_finish_signin(unsigned int user_index);

// A profile switch, sign-out or new connect invalidates an earlier generation and its result.
unsigned int port_live_user_generation(unsigned int user_index);
int port_live_user_auth_result(unsigned int user_index, unsigned int generation, port_live_auth_result *out);

// Returns 0 on success, -1 on failure. A null account path uses the profile's account file.
int port_live_connect_user(unsigned int user_index, const char *gamertag, const char *account_path);
// Binds the result to `profile` (the local name), which can differ from the requested Live gamertag.
// The caller supplies the slot's generation when the profile was selected. A stale request is rejected.
int port_live_connect_user_async(unsigned int user_index, const char *profile, const char *gamertag,
    unsigned int generation, void (*done)(unsigned int user_index, unsigned int generation, void *context), void *context);

// Track user contexts/properties (XUserSetContext* / XUserSetProperty*) to attach to hosted sessions.
void port_live_set_context(unsigned int user_index, unsigned int id, unsigned int value);
void port_live_set_property_int32(unsigned int user_index, unsigned int id, int value);
void port_live_set_property_int64(unsigned int user_index, unsigned int id, long long value);
void port_live_set_property_double(unsigned int user_index, unsigned int id, double value);
void port_live_set_property_float(unsigned int user_index, unsigned int id, float value);

// ---- LSP title servers & TCP stream sockets (winsock_offline.cpp) ----

PORT_MS_ABI int port_live_fetch_title_servers(unsigned int title_id, port_live_title_server *out, int capacity, int *count_out);
PORT_MS_ABI int port_live_server_to_inaddr(unsigned int server_ip, unsigned int service_id, unsigned int *secure_out);

PORT_MS_ABI int port_live_is_tcp_handle(unsigned int handle);
PORT_MS_ABI int port_live_tcp_socket(unsigned int *handle_out);
PORT_MS_ABI int port_live_tcp_close(unsigned int handle);
PORT_MS_ABI int port_live_tcp_shutdown(unsigned int handle, int how);
PORT_MS_ABI int port_live_tcp_connect(unsigned int handle, unsigned int address, unsigned short port);
PORT_MS_ABI int port_live_tcp_set_nonblocking(unsigned int handle, int nonblocking);
PORT_MS_ABI int port_live_tcp_pending(unsigned int handle, unsigned int *bytes_out);
PORT_MS_ABI int port_live_tcp_send(unsigned int handle, const void *data, int size, int *sent_out);
PORT_MS_ABI int port_live_tcp_recv(unsigned int handle, void *buffer, int size, int *received_out);
PORT_MS_ABI int port_live_tcp_poll(unsigned int handle, int *readable, int *writable, int *error);

// ---- XGI sessions & matchmaking ----

// Sets each view's row_count, including on a failed request (empty views).
PORT_MS_ABI void port_live_read_stats(const unsigned long long *xuids, unsigned int xuid_count,
    port_live_stats_view *views, unsigned int view_count);

PORT_MS_ABI int port_live_session_create(const unsigned char xnkid[8], unsigned int flags, unsigned int user_index,
    unsigned long long host_xuid, const unsigned char xnaddr[36], const unsigned char xnkey[16],
    unsigned int max_public, unsigned int max_private);
PORT_MS_ABI int port_live_session_modify(const unsigned char xnkid[8], unsigned int flags, unsigned int max_public,
    unsigned int max_private);
PORT_MS_ABI int port_live_session_join(const unsigned char xnkid[8], const unsigned long long *xuids, const int *private_slots,
    unsigned int count);
PORT_MS_ABI int port_live_session_leave(const unsigned char xnkid[8], const unsigned long long *xuids, unsigned int count);
PORT_MS_ABI int port_live_session_start(const unsigned char xnkid[8]);
PORT_MS_ABI int port_live_session_end(const unsigned char xnkid[8]);
PORT_MS_ABI int port_live_session_delete(const unsigned char xnkid[8]);
PORT_MS_ABI int port_live_session_migrate(const unsigned char xnkid[8], unsigned int user_index, unsigned long long host_xuid,
    const unsigned char xnaddr[36], const unsigned char xnkey[16]);
PORT_MS_ABI int port_live_session_arbitrate(const unsigned char xnkid[8], unsigned long long nonce,
    port_live_arb_machine *machines, unsigned int max_machines, unsigned int *machine_count_out,
    unsigned int *total_players_out);
PORT_MS_ABI int port_live_session_write_stats(const unsigned char xnkid[8], unsigned long long xuid, unsigned int view_count,
    const unsigned int *view_ids, const unsigned int *prop_counts, const port_live_property *props);
PORT_MS_ABI int port_live_session_search(unsigned int proc_index, unsigned int max_results, unsigned int num_users,
    unsigned long long exclude_xuid, const port_live_property *props, unsigned int prop_count,
    const port_live_context *ctxs, unsigned int ctx_count, port_live_search_result *results, unsigned int results_cap,
    unsigned int *count_out);
PORT_MS_ABI int port_live_session_search_by_id(const unsigned char xnkid[8], port_live_search_result *result_out, int *found_out);
void port_live_qos_listen_changed(const unsigned char xnkid[8], int enabled);

// HALO3_LIVE_LOG: how long a call held its caller, or took to complete. port_live_trace_start is 0 when the log is off
// (Live off too); port_live_trace_end logs "live: <what> <how> after <ms> ms" for a start that is not 0.
PORT_MS_ABI unsigned long long port_live_trace_start(void);
PORT_MS_ABI void port_live_trace_end(const char *what, const char *how, unsigned long long start);

// ---- the Live worker (live.c) ----
//
// Queues run(a copy of data, size bytes: at most PORT_LIVE_POST_MAX, kept 8-byte aligned) on the Live worker thread,
// after the calls queued before it, and returns 0; they are made one at a time, in that order. It does not wait: past
// the queue's ring the calls wait in a list. A call posted by a job runs after the calls queued before it, so a job
// must never wait for one it posts (port_live_on_worker tells). With no worker thread (it could not be started), runs
// it at once on the caller's thread and returns 1. run may make HTTP requests (port_live_session_*).
#define PORT_LIVE_POST_MAX 4096u
PORT_MS_ABI int port_live_post(void (PORT_MS_ABI *run)(const void *data), const void *data, unsigned int size);
// Whether the caller is the Live worker thread.
PORT_MS_ABI int port_live_on_worker(void);

// ---- friends & game invites (invite.h's Live channel; the routes are listed in live.c) ----
//
// Each returns 0, -1 when not signed in or the server could not be reached, or PORT_LIVE_UNSUPPORTED when the server
// has no such route (404, remembered: the next call returns it at once). Each is HTTP: never under a lock. Invites
// received are polled for on the heartbeat thread once signed in (every 5 s), into invite.h's inbox.

#define PORT_LIVE_UNSUPPORTED (-2)

typedef struct port_live_friend {
    unsigned long long xuid;
    char gamertag[32];
    int online;
    int reserved;
} port_live_friend;
typedef char port_live_friend_size_check[sizeof(port_live_friend) == 48 ? 1 : -1];

int port_live_friends(port_live_friend *out, unsigned int capacity, unsigned int *count_out);
// Sends a game invite for `session` (an XSESSION_INFO, 60 bytes, of class `session_class`: invite.h) to `to_xuid`.
PORT_MS_ABI int port_live_invite_send(unsigned long long to_xuid, const unsigned char session[60], int session_class);
// An invite received was accepted or declined: the server stops delivering it.
int port_live_invite_ack(const char *id, int accepted);
// Whether the server has the routes (until it has answered 404).
int port_live_invites_supported(void);
int port_live_friends_supported(void);

#ifdef __cplusplus
}
#endif

#endif
