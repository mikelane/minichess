//
// Created by Michael Lane on 6/2/17.
//

#ifndef MOVE_GENERATOR_TESTING_PLAYER_H
#define MOVE_GENERATOR_TESTING_PLAYER_H


#include <string>
#include "Player.h"

/**
 * A player used only to test the move generator.
 */
class Testing_Player : public Player{
public:
  /**
   * Implements the public interface of the base Player.
   *
   * @param state_string
   * @return
   */
  std::string get_move_string(const std::string &state_string);
};


#endif //MOVE_GENERATOR_TESTING_PLAYER_H
