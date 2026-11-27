#ifndef NETWORKING_LOGIC_NETWORK_ELECTION_REFUSE_REASON_TYPES_H
#define NETWORKING_LOGIC_NETWORK_ELECTION_REFUSE_REASON_TYPES_H
// Why a peer refuses election participation (c_network_session::handle_election). Names are
// the March k_election_refuse_reason_strings table entries with '-' as '_'; entry 0 shares
// the join table's "no-reason-given". Every value below is stored under the same condition
// March stores it, so the values match March exactly.
enum e_network_election_refuse_reason
{
    _network_election_refuse_none = 0,
    _network_election_refuse_unknown_session = 1, // March entry 1 "no-session"; landed name kept
    _network_election_refuse_not_established = 2, // March entry 2 "not-established"
    _network_election_refuse_leaving = 3,         // March entry 3 "leaving"
    _network_election_refuse_still_connected = 4, // March entry 4 "still-connected"
    _network_election_refuse_local_host = 5,      // March entry 5 "local-host"
    _network_election_refuse_alternate_host = 6,  // March entry 6 "alternate-host"
    _network_election_refuse_unknown_stranger = 9, // March entry 9 "unknown-stranger"
};
#endif
