//
// Created by Michael Lane on 6/5/17.
//

#ifndef MOVE_GENERATOR_AB_PLAYER_H
#define MOVE_GENERATOR_AB_PLAYER_H


#include <string>
#include "bitboard_tables.h"
#include "Player.h"

class AB_Player : public Player {
public:
  std::string get_move_string(const std::string &state_string);

  move negamax(const move &parent_move, int depth, int alpha, int beta, bool root);

private:
  std::string best_move_string{"INVALID"};
  int best_move_value{std::numeric_limits<int>::min()};
};



#endif //MOVE_GENERATOR_AB_PLAYER_H
