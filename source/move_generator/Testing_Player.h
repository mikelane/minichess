//
// Created by Michael Lane on 6/2/17.
//

#ifndef MOVE_GENERATOR_TESTING_PLAYER_H
#define MOVE_GENERATOR_TESTING_PLAYER_H


#include <string>
#include "Player.h"

class Testing_Player : public Player{
public:
  std::string get_move_string(std::string state_string);

};


#endif //MOVE_GENERATOR_TESTING_PLAYER_H
