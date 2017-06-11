//
// Created by Michael Lane on 6/10/17.
//

#ifndef MOVE_GENERATOR_AB_ID_PLAYER_H
#define MOVE_GENERATOR_AB_ID_PLAYER_H


#include "Player.h"
#include "Negamax_Result.h"

/**
 * Run a negamax search with alpha beta pruning and iterative deepening.
 */
class AB_ID_Player : public Player {
public:
  std::string get_move_string(const std::string &state_string) override;

private:
  /**
   * The negamax algorithm. The iterative deepening is driven in part by
   * the get_move_string function. The negamax function provides a method
   * for interrupting the function when time has expired.
   *
   * @param state The vector of unsigned ints that is the game state
   * @param depth The desired depth of the negamax search.
   * @param alpha
   * @param beta
   * @param node_count
   * @return
   */
  Negamax_Result negamax(const State_t &state, int depth, int alpha, int beta, int &node_count);
};


#endif //MOVE_GENERATOR_AB_ID_PLAYER_H
