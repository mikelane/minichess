//
// Created by Michael Lane on 6/3/17.
//

#include "Negamax_Player.h"

std::string Negamax_Player::get_move_string(const std::string &state_string) {
  parse_input(state_string);
  move current_move;
  current_move.state = current_state;
  move result = negamax(current_move, 4);
  return result.move_string;
}

move Negamax_Player::negamax(const move &parent_move, int depth) {
  if (depth == 0 || parent_move.terminal) {
    return parent_move;
  }

  move best_move;
  best_move.value = std::numeric_limits<int>::min();
  std::vector<move> children = generate_all_moves(parent_move.state);
  for (move child : children) {
    ++number_of_nodes;
    move result = -negamax(child, depth - 1);
    best_move = std::max(result, best_move);
  }

  return best_move;
}

