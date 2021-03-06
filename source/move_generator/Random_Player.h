//
// Created by Michael Lane on 5/30/17.
//

#ifndef MOVE_GENERATOR_RANDOM_PLAYER_H
#define MOVE_GENERATOR_RANDOM_PLAYER_H

#include <string>
#include <random>
#include "Player.h"
#include "bitboard_tables.h"

/**
 * A random move player.
 */
class Random_Player : public Player {
public:
  /**
   * Implements the public interface of the base player.
   *
   * @param state_string
   * @return
   */
  std::string get_move_string(const std::string &state_string);

  std::string get_random_move(std::vector<std::string> &moves);

  int get_random_number(int max);

private:
  std::vector<std::string> moves{};
};


#endif //MOVE_GENERATOR_RANDOM_PLAYER_H
