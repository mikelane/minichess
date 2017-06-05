//
// Created by Michael Lane on 6/5/17.
//

#include <iostream>
#include <random>
#include "AB_Player.h"

std::string AB_Player::get_move_string(const std::string &state_string) {
  parse_input(state_string);
  move current_move;
  current_move.state = current_state;
  move result = negamax(current_move, 2, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true);
  return best_move_string;
}

move AB_Player::negamax(const move &parent_move, int depth, int alpha, int beta, bool root) {
  if (depth == 0 || parent_move.terminal) {
    return parent_move;
  }

  std::vector<move> children = generate_all_moves(parent_move.state);
  std::sort(children.begin(), children.end());

  int best_value = std::numeric_limits<int>::min();
  move curr_best_move;

  int child_counter = 0;
  for (move child : children) {
    move resulting_move = -negamax(child, depth - 1, -beta, -alpha, false);
    if (root) {
      if (resulting_move.value > best_move_value) {
        best_move_value = resulting_move.value;
        best_move_string = resulting_move.move_string;
      }
    }

    best_value = std::max(resulting_move.value, best_value);
    // If the resulting move value is the best value, then this child is best
    // Return this child with its value set accordingly
    if(resulting_move.value == best_value) {
      child.value = best_value;
      curr_best_move = child;
    }

    alpha = std::max(alpha, resulting_move.value);
    if (alpha >= beta) {
      break;
    }
  }

  return curr_best_move;
}

