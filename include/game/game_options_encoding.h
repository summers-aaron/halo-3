#ifndef INCLUDE_GAME_GAME_OPTIONS_ENCODING_H
#define INCLUDE_GAME_GAME_OPTIONS_ENCODING_H

class c_bitstream;

extern "C" bool game_options_decode_campaign_armaments(c_bitstream *packet, void *data);
extern "C" void game_options_encode_campaign_armaments(c_bitstream *stream, const void *entries);
extern "C" unsigned char game_options_decode(c_bitstream *stream, void *data);

#endif // INCLUDE_GAME_GAME_OPTIONS_ENCODING_H
