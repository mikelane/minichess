//
// Created by Michael Lane on 6/10/17.
//

#include <iostream>
#include <cassert>
#include "AB_ID_Player.h"

std::string AB_ID_Player::get_move_string(const std::string &state_string) {
  State_t root_state = parse_input(state_string);
  // Set the time limit for this move
  set_time_limit(root_state);
  my_player_color = root_state[PLAYER_ON_MOVE];
  Move current_move;

  int num_nodes = 0;
  int depth = 1;
  int alpha = std::numeric_limits<int>::min();
  int beta = std::numeric_limits<int>::max();

  Negamax_Result root_result = negamax(root_state, depth, alpha, beta, num_nodes);

  while(root_state[MOVE_NUMBER] + ++depth <= 41) {
    Negamax_Result candidate_result = negamax(root_state, depth, alpha, beta, num_nodes);
    // Sense timeout conditions
    if (get_millisecond_time() > timelimit || candidate_result.get_move_string() == "XXX") {
      --depth; // for printing purposes
      break;
    }

    // Short circuit on a loss.
    if (candidate_result.isLoss()) {
      return root_result.get_move_string();
    }

    // Short circuit on a win
    if (candidate_result.isWin()) {
      return candidate_result.get_move_string();
    }

    root_result = candidate_result;
  }

  std::cerr << "Returning move_string " << root_result.get_move_string()
            << " with value " << root_result.get_value()
            << ". Nodes evaluated: " << num_nodes
            << " Depth: " << depth << std::endl;

  return root_result.get_move_string();
}

Negamax_Result AB_ID_Player::negamax(const State_t &state, int depth, int alpha, int beta, int &node_count) {
  using namespace std::chrono;
  ++node_count;
  ++timecounter;

  if (timecounter > 1000) {
    timecounter = 0;
    timecache = get_millisecond_time();
  }

  if(timecache > timelimit) {
    return Negamax_Result(0, "XXX");
  }

  if (depth == 0 || is_terminal(state)) {
    Negamax_Result result(eval(state), "");
    // This state is a loss if my king is missing.
    result.setLoss(state[my_king_index[my_player_color]] == 0ul);
    // This state is a win if my opponent's king is missing
    result.setWin(state[opponent_king_index[my_player_color]] == 0ul);
    // It can't be the case that both kings are missing.
    return result;
  }

  Negamax_Result result;

  // children <- legal moves from state
  std::vector<Move> children = generate_all_moves(state);
  assert (children.size() > 0);

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

/**
 * This uses a little help from Stack Overflow:
 * https://stackoverflow.com/questions/31255486/c-how-do-i-convert-a-stdchronotime-point-to-long-and-back
 * @param state
 * @return
 */
void AB_ID_Player::set_time_limit(const State_t &state) {
  using namespace std::chrono;
  // Get current time with precision of milliseconds
  auto now = time_point_cast<milliseconds>(system_clock::now());
  // Calculate the time limits;
  if (state[MOVE_NUMBER] <= 5) {
    timelimit = now.time_since_epoch().count() + 5000ll;
    return;
  }

  if(state[MOVE_NUMBER] <= 10) {
    timelimit = now.time_since_epoch().count() + 15000ll;
    return;
  }

  if(state[MOVE_NUMBER] <= 15) {
    timelimit = now.time_since_epoch().count() + 10000ll;
    return;
  }

  if(state[MOVE_NUMBER] <= 20) {
    timelimit = now.time_since_epoch().count() + 9000ll;
    return;
  }

  if(state[MOVE_NUMBER] <= 25) {
    timelimit = now.time_since_epoch().count() + 4000ll;
    return;
  }

  if(state[MOVE_NUMBER] <= 30) {
    timelimit = now.time_since_epoch().count() + 5000ll;
    return;
  }

  if(state[MOVE_NUMBER] <= 35) {
    timelimit = now.time_since_epoch().count() + 6000ll;
    return;
  }

  timelimit = now.time_since_epoch().count() + 4000ll;
}

long long AB_ID_Player::get_millisecond_time() {
  using namespace std::chrono;
  // Get current time with precision of milliseconds
  auto now = time_point_cast<milliseconds>(system_clock::now());
  return now.time_since_epoch().count();
}
