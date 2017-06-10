//
// Created by Michael Lane on 6/5/17.
//

#ifndef MOVE_GENERATOR_AB_PLAYER_H
#define MOVE_GENERATOR_AB_PLAYER_H


#include <string>
#include "bitboard_tables.h"
#include "Player.h"
#include "Negamax_Result.h"

class AB_Player : public Player {
public:
  std::string get_move_string(const std::string &state_string);

  Negamax_Result negamax(const State_t &state, int depth, int alpha, int beta, int &node_count);
};



#endif //MOVE_GENERATOR_AB_PLAYER_H
