//
// Created by Michael Lane on 6/5/17.
//

#include <iostream>
#include <random>
#include <cassert>
#include "AB_Player.h"

std::string AB_Player::get_move_string(const std::string &state_string) {
  State_t root_state = parse_input(state_string);
  Move current_move;

  int num_nodes = 0;
  int depth = 6;
  int alpha = std::numeric_limits<int>::min();
  int beta = std::numeric_limits<int>::max();

  Negamax_Result root_result = negamax(root_state, depth, alpha, beta, num_nodes);

  std::cerr << "Returning move_string " << root_result.get_move_string()
            << " with value " << root_result.get_value()
            << ". Nodes evaluated: " << num_nodes << std::endl;

  return root_result.get_move_string();
}

/**
 *  function negamax(state, depth, alpha, beta)
 *      result <- negamax_result()
 *      if depth == 0 or state is terminal
 *          result.value <- eval(state)
 *          result.move_string <- ""
 *
 *      best_value = -inf
 *      child_states <- generate child states(state)
 *      foreach child in child_states
 *          child_state = make_move(state, child)
 *          child_result <- -negamax(child_state, depth-1, -beta, -alpha)
 *          if child_result.value > best_value
 *              best_value <- child_result.value
 *              result.value <- best_value
 *              result.move_string <- child.move_string ***IMPORTANT***
 *          if alpha >= beta
 *              break
 *      return result
 *
 * @param state
 * @param depth
 * @param node_count
 * @param root
 * @return
 */
Negamax_Result AB_Player::negamax(const State_t &state, int depth, int alpha, int beta, int &node_count) {
  ++node_count;
  if (depth == 0 || is_terminal(state)) {
    Negamax_Result result(eval(state), "");
    // Keep track of shortest win, longest loss
    if(depth > 0) {
      if (state[PLAYER_ON_MOVE] == 1 && state[WHITE_KING] == 0) {
        // White loses, multiply the score and depth
        result.set_value(depth * result.get_value());
      } else if (state[PLAYER_ON_MOVE] == 2 && state[BLACK_KING] == 0) {
        // Black loses, multiply the score and depth
        result.set_value(depth * result.get_value());
      }
    }
    return Negamax_Result(eval(state), "");
  }

  Negamax_Result result;

  // children <- legal moves from state
  std::vector<Move> children = generate_all_moves(state);

  // extract some move child from children
  Move child = children.front();
  children.erase(children.begin());

  // Make the move
  State_t child_state = make_move(state, child);

  // Get the negamax value of that move
  result = -negamax(child_state, depth - 1, -beta, -alpha, node_count);
  result.set_move_string(child.get_move_string());

  int best_value = result.get_value();

  // if result's value > beta, return child result
  if (best_value > beta) {
    return result;
  }

  // a <- max(a, child result value)
  alpha = std::max(alpha, result.get_value());

  for (Move child : children) {
    child_state = make_move(state, child);
    Negamax_Result child_result = -negamax(child_state, depth - 1, -beta, -alpha, node_count);
    child_result.set_move_string(child.get_move_string());

    if (child_result.get_value() >= beta) {
      result = child_result;
      return result;
    }

    if (child_result.get_value() > best_value) {
      best_value = child_result.get_value();
      result = child_result;
    }

    alpha = std::max(alpha, child_result.get_value());
  }
  return result;
}

