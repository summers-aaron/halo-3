#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_FILM_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_FILM_H

// Functions defined in tu/networkinglibxbox/network_session_parameters_film.cpp.

class c_bitstream;
struct s_session_parameters_film_record;

extern "C" bool saved_film_decode(c_bitstream *packet, s_session_parameters_film_record *record);
extern "C" void saved_film_encode(c_bitstream *packet, const s_session_parameters_film_record *record);

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_FILM_H
