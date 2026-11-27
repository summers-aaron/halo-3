#ifndef NETWORKING_TRANSPORT_TRANSPORT_SECURITY_H
#define NETWORKING_TRANSPORT_TRANSPORT_SECURITY_H

// Functions defined in tu/networkinglibxbox/transport_security.cpp.

struct in_addr;
// The console's secure address: the XNADDR (36 bytes) of the XDK, held as bytes (every unit's copy has byte
// alignment). Bytes 10..15 are the Ethernet address (XNADDR::abEnet), which transport_secure_address_get_unique_identifier
// / extract_identifier copy out as the machine's unique identifier.
struct s_transport_secure_address
{
    unsigned char ina[4];           // XNADDR::ina
    unsigned char ina_online[4];    // XNADDR::inaOnline
    unsigned char port_online[2];   // XNADDR::wPortOnline
    unsigned char identifier[6];    // XNADDR::abEnet
    unsigned char ab_online[20];    // XNADDR::abOnline
};
typedef char transport_security_size_secure_address[(sizeof(s_transport_secure_address) == 0x24) ? 1 : -1];
struct s_transport_secure_identifier;
struct s_transport_session_description;
struct transport_address;
enum e_transport_platform;

bool transport_secure_identifier_retrieve(const transport_address *address, e_transport_platform platform,
    s_transport_secure_identifier *identifier, s_transport_secure_address *secure_address);
bool transport_secure_address_resolve(void);
void transport_security_initialize(void);
bool transport_secure_address_get(s_transport_secure_address *address);
bool transport_secure_address_decode(const s_transport_session_description *description,
    const s_transport_secure_address *secure_address, unsigned short port, transport_address *address);
bool transport_secure_address_retrieve(const transport_address *usable_address, e_transport_platform platform, s_transport_secure_address *secure_address);
bool transport_secure_address_compare(const s_transport_secure_address *address1, const s_transport_secure_address *address2);
unsigned __int64 transport_secure_address_get_local_machine_id(void);
bool transport_secure_connection_initiate(const transport_address *address);
void transport_secure_random(long random_length, unsigned char *random_data);
unsigned __int64 transport_local_machine_nonce_get(void);
char *transport_secure_address_to_string(const s_transport_secure_address *secure_address, char *string,
    long maximum_string_length, bool include_online, bool include_mac);
char *transport_secure_identifier_get_string(const s_transport_secure_identifier *identifier);
bool transport_address_get_insecure_xbox_address(const transport_address *insecure_address, in_addr *xbox_address);
extern "C" unsigned __int64 transport_secure_nonce_generate(void);
void transport_secure_address_reset_for_new_networking_mode(void *);

enum e_transport_secure_connection;
e_transport_secure_connection transport_secure_connection_status(const transport_address *address);
bool transport_secure_key_create(s_transport_session_description *description, e_transport_platform platform);

#endif // NETWORKING_TRANSPORT_TRANSPORT_SECURITY_H
