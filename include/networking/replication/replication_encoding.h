#ifndef NETWORKING_REPLICATION_REPLICATION_ENCODING_H
#define NETWORKING_REPLICATION_REPLICATION_ENCODING_H

// Functions defined in tu/networkinglibxbox/replication_encoding.cpp.

class c_bitstream;
struct s_player_motion;
struct s_player_predicted_aim_assist;
struct s_player_prediction;

void predicted_aim_assist_encode(c_bitstream *packet, const s_player_predicted_aim_assist *aim);
bool predicted_aim_assist_decode(c_bitstream *packet, s_player_predicted_aim_assist *aim);
void player_prediction_encode(c_bitstream *packet, const s_player_prediction *prediction, bool predicted_entity);
void player_motion_encode(c_bitstream *packet, const s_player_motion *motion);
bool player_motion_decode(c_bitstream *packet, s_player_motion *motion);
bool player_prediction_decode(c_bitstream *packet, s_player_prediction *prediction, bool predicted_entity);
bool player_prediction_compare_identical(const s_player_prediction *left, const s_player_prediction *right);
bool player_prediction_valid(const s_player_prediction *prediction);

#endif // NETWORKING_REPLICATION_REPLICATION_ENCODING_H
