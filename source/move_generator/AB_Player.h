//
// Created by Michael Lane on 6/5/17.
//

#ifndef MOVE_GENERATOR_AB_PLAYER_H
#define MOVE_GENERATOR_AB_PLAYER_H


#include <string>
#include "bitboard_tables.h"
#include "Player.h"

class AB_Player : public Player {
private:
  std::string best_move_string{"INVALID"};
  int best_move_value{std::numeric_limits<int>::min()};
};



#endif //MOVE_GENERATOR_AB_PLAYER_H
