#ifndef NETWORKING_ONLINE_VOICE_ROUTING_H
#define NETWORKING_ONLINE_VOICE_ROUTING_H

struct s_voice_route
{
    unsigned short routed_peer_mask;
    unsigned short repeater_peer_mask;
};

class c_voice_routing
{
public:
    void initialize();
    void dispose();
    void update();
    void get_voice_route_by_player_mask(unsigned long player_mask, s_voice_route *voice_route);
    void get_voice_route_by_peer_mask(unsigned long peer_mask, s_voice_route *voice_route);

    bool m_initialized;

private:
    unsigned long get_desired_peer_mask_by_player_mask(unsigned long player_mask);
    static void consume_voice_channels_by_desired_peer_mask(unsigned long *desired_peer_mask,
        unsigned long eligible_peer_mask, unsigned short *routed_peer_mask,
        long *available_outgoing_voice_channel_count, bool voice_repeater_available,
        long repeater_peer_index, bool *use_voice_repeater);
};

typedef char voice_route_size[(sizeof(s_voice_route) == 4) ? 1 : -1];
typedef char voice_routing_size[(sizeof(c_voice_routing) == 1) ? 1 : -1];

#endif // NETWORKING_ONLINE_VOICE_ROUTING_H
