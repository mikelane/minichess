//
// Created by Michael Lane on 6/3/17.
//

#include <iostream>
#include "Negamax_Player.h"
#include "Negamax_Result.h"
#include "Move.h"

std::string Negamax_Player::get_move_string(const std::string &state_string) {
  State_t root_state = parse_input(state_string);
  Move current_move;
  int num_nodes = 0;
  int depth = 4;
  Negamax_Result root_result = negamax(root_state, depth, num_nodes);
  std::cerr << "Returning move_string " << root_result.get_move_string()
            << " with value " << root_result.get_value()
            << ". Nodes evaluated: " << num_nodes << std::endl;
  return root_result.get_move_string();
}

/**
 *  function negamax(state, depth)
 *      result <- negamax_result()
 *      if depth == 0 or state is terminal
 *          result.value <- eval(state)
 *          result.move_string <- ""
 *
 *      best_value = -inf
 *      child_states <- generate child states(state)
 *      foreach child in child_states
 *          child_state = make_move(state, child)
 *          child_result <- -negamax(child_state, depth-1)
 *          if child_result.value > best_value
 *              best_value <- child_result.value
 *              result.value <- best_value
 *              result.move_string <- child.move_string ***IMPORTANT***
 *      return result
 *
 * @param state
 * @param depth
 * @param node_count
 * @param root
 * @return
 */
Negamax_Result Negamax_Player::negamax(const State_t &state, int depth, int &node_count) {
  ++node_count;
  if (depth == 0 || is_terminal(state)) {
    return Negamax_Result(eval(state), "");
  }

  Negamax_Result result;
  int best_value = std::numeric_limits<int>::min();

  std::vector<Move> children = generate_all_moves(state);

  for (Move child : children) {
    State_t child_state = make_move(state, child);
    Negamax_Result child_result = -negamax(child_state, depth - 1, node_count);
    if (child_result.get_value() > best_value) {
      best_value = child_result.get_value();
      result.set_value(best_value);
      result.set_move_string(child.get_move_string());
    }
  }
  return result;
}

bool Negamax_Player::is_terminal(const State_t &state) {
  return (state[MOVE_NUMBER] == 41)
         || (state[BLACK_KING] == 0)
         || (state[WHITE_KING] == 0);
}

