#ifndef GAME_GAME_CAMPAIGN_ARMAMENTS_ENCODING_H
#define GAME_GAME_CAMPAIGN_ARMAMENTS_ENCODING_H

class c_bitstream;
struct s_campaign_armaments;

// Typed interface used by game.cpp; the network map bridge takes untyped storage.
extern "C" bool game_options_decode_campaign_armaments(c_bitstream *packet, s_campaign_armaments *armaments);
extern "C" void game_options_encode_campaign_armaments(c_bitstream *packet, const s_campaign_armaments *armaments);

#endif // GAME_GAME_CAMPAIGN_ARMAMENTS_ENCODING_H
