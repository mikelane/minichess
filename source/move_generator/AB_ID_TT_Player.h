//
// Created by Michael Lane on 6/10/17.
//

#ifndef MOVE_GENERATOR_AB_ID_TT_PLAYER_H
#define MOVE_GENERATOR_AB_ID_TT_PLAYER_H


#include "Player.h"
#include "Negamax_Result.h"
#include "TTable.h"

/**
 * Handle the ab, ID, transposition table player.
 */
class AB_ID_TT_Player : public Player {
public:
  /**
   * Implements the base Player get_move_string interface.
   *
   * @param state_string Incoming value from the IMCS server interface.
   * @return
   */
  std::string get_move_string(const std::string &state_string) override;

private:
  /**
   * The negamax function for this player.
   *
   * @param state
   * @param depth
   * @param alpha
   * @param beta
   * @param node_count
   * @return
   */
  Negamax_Result negamax(const State_t &state, int depth, int alpha, int beta, int &node_count);

  /**
   * This player's transposition table.
   */
  TTable table;
};


#endif //MOVE_GENERATOR_AB_ID_TT_PLAYER_H
