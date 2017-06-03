//
// Created by Michael Lane on 5/30/17.
//

#include "Random_Player.h"

/**
 * Implements the public interface of Player. Random player picks a move string
 * at random from the list of all possible move strings.
 * @param state
 * @return
 */
std::string Random_Player::get_move_string(std::string state_string) {
  parse_input(state_string);
  std::vector<std::string> all_move_strings = generate_all_move_strings(current_state);
  return get_random_move(all_move_strings);
}

/**
 * Return a move string at random from a list of move strings.
 * @param moves
 * @return
 */
std::string Random_Player::get_random_move(std::vector<std::string> &move_strings) {
  int i = get_random_number((int) (moves.size() - 1));
  return moves[i];
}

/**
 * Get a random int from 0 up to AND INCLUDING max.
 * @param max
 * @return
 */
int Random_Player::get_random_number(int max) {
  std::uniform_int_distribution<int> dist(0, max);
  return dist(mt);
}


