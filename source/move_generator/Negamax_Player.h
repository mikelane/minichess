//
// Created by Michael Lane on 6/3/17.
//

#ifndef MOVE_GENERATOR_NEGAMAX_PLAYER_H
#define MOVE_GENERATOR_NEGAMAX_PLAYER_H


#include <string>
#include "Player.h"
#include "Negamax_Result.h"

/**
 * The basic negamax search player
 */
class Negamax_Player : public Player {
public:
  /**
   * Implements the public interface of the base Player.
   *
   * @param state_string
   * @return
   */
  std::string get_move_string(const std::string &state_string);

  /**
   * The negamax function.
   *
   * @param state
   * @param depth
   * @param node_count
   * @return
   */
  Negamax_Result negamax(const State_t &state, int depth, int &node_count);
};


#endif //MOVE_GENERATOR_NEGAMAX_PLAYER_H
