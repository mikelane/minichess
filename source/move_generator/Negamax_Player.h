//
// Created by Michael Lane on 6/3/17.
//

#ifndef MOVE_GENERATOR_NEGAMAX_PLAYER_H
#define MOVE_GENERATOR_NEGAMAX_PLAYER_H


#include <string>
#include "Player.h"

class Negamax_Player : public Player {
public:
  std::string get_move_string(const std::string &state_string);

  move negamax(const move &parent_move, int depth);
};


#endif //MOVE_GENERATOR_NEGAMAX_PLAYER_H
