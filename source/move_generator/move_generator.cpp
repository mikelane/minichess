//
// Created by Michael Lane on 5/13/17.
//

#include "move_generator.h"
#include "bitboard_tables.h"
#include <iostream>
#include <random>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <zmq.hpp>

int timecounter = 0;

/**
 * I found this little snippet of goodness on Stack Overflow: http://stackoverflow.com/questions/236129/split-a-string-in-c
 *
 * @param input - A space-separated list of integers as a std::string
 * @return A vector of unsigned ints ... the game state!
 */
State_t parse_input(std::string input) {
  std::istringstream iss(input);
  State_t results{std::istream_iterator<unsigned int>{iss}, std::istream_iterator<unsigned int>{}};
  return results;
}

/**
 * State_t has the following values at each index:
 * 0    : BLACK KING POSITION
 * 1    : BLACK QUEEN POSITION
 * 2    : BLACK BISHOP POSITION
 * 3    : BLACK KNIGHT POSITION
 * 4    : BLACK ROOK POSITION
 * 5-9  : BLACK PAWN POSITIONS
 * 10-14: WHITE PAWN POSITIONS
 * 15   : WHITE ROOK POSITION
 * 16   : WHITE KNIGHT POSITION
 * 17   : WHITE BISHOP POSITION
 * 18   : WHITE QUEEN POSITION
 * 19   : WHITE KING POSITION
 * 20   : MOVE NUMBER
 * 21   : PLAYER ON MOVE {1 WHITE, 2 BLACK}
 * 22   : LOCATION OF ALL OPPONENTS
 * 23   : LOCATION OF ALL EMPTY
 * 24   : TIME LEFT IN MILLISECONDS
 * @param state
 * @return
 */
Ordered_States_t get_ordered_children(State_t state) {
  Ordered_States_t result;
  unsigned int to_move_location;
  std::string move_string = "";
  std::string start_pos = "";
  std::string end_pos = "";
  unsigned int opponent_locs = state[22];
  unsigned int empty_locs = state[23];

  // The state array has the locations of the 10 black players followed
  // by the location of the 10 white players. Search only one or the other for the
  // starting move location

  // Find all the moves available to each piece for the on move player
  for (int my_index = my_player_index[state[21]], end = my_index + 10; my_index < end; ++my_index) {
    if (state[my_index] != 0u) {  // If the piece hasn't been captured
      to_move_location = state[my_index];

      start_pos = TO_STR[to_move_location];
      move_string = TO_STR[to_move_location] + "-";

      // Set up the shadows so that there are none initially. If a 1 is in a cell,
      // that means you can get to that cell.
      unsigned int shadows = (1 << 30) - 1;

      // If the starting piece is not a Knight, mask the shadows of all the pieces
      // (for me and for the opponent) that could be in the way.
      if (my_index != 3 && my_index != 16) {  // Knights can jump over other players
        for (int j = 0; j < 20; ++j) {
          // Don't try to cast your own shadow, make sure the piece in question is on the board
          // and make sure the piece location exists as a key in the SHADOW_MASK dict.
          if (j != my_index && state[j] && SHADOW_MASK[to_move_location].count(state[j])) {
            // If so, add the relevant shadow to the mask
            shadows &= ~SHADOW_MASK[to_move_location][state[j]];
          }
        }
      }

      // For all the possible move locations for a given piece type and location, if there is a
      // legal attack, then add that attack to the list of legal attacks.
      for (unsigned int pos : type_to_attack[my_index][to_move_location]) {
        if (pos & opponent_locs & shadows) {
          int target_index = 0;
          for (; target_index < 20 && pos != state[target_index]; ++target_index);
          bool win = (target_index == 0) || (target_index == 19);
          State_t child_state = make_attack(state, my_index, target_index);
          state_value value = {
              evaluate_state(child_state),
              move_string + TO_STR[pos],
              child_state,
              (target_index == 0) || (target_index == 19),
              true,  // This is an attack
          };
          result.emplace(value);
        }
      }

      // Second verse, same as the first. Except this time, for moves not for attacks.
      for (unsigned int pos : type_to_move[my_index][to_move_location]) {
        if (pos & empty_locs & shadows) {
          State_t child_state = make_move(state, my_index, pos);
          state_value value = {
              evaluate_state(child_state),
              move_string + TO_STR[pos],
              child_state,
              false  // This is not an attack
          };
          result.emplace(value);
        }
      }
    }
  }

  return result;
}

State_t make_attack(const State_t & state, int attacker_index, int target_index) {
  State_t result = state;
  // xor attacker with itself, zeros it
  result[attacker_index] ^= result[attacker_index];
  // xor attacker with target moves the attacker
  result[attacker_index] ^= result[target_index];
  // xor target with itself zeros it
  result[target_index] ^= result[target_index];

  // Bookkeeping. Update the player on move. Update the opponents location. Update the empty cells
  result[21] = opponent[result[21]];
  result[22] ^= result[22];  // zero opponents locations
//  int opponent_index = opponent_player_index[result[21]];
//  int end = opponent_index + 10;
  for (int opponent_index = opponent_player_index[result[21]], end = opponent_index + 10;
       opponent_index < end; ++opponent_index) {
    result[22] |= result[opponent_index];
  }
  // Update the location of the empty cells (negation of all cells with pieces)
  result[23] ^= result[23];
  for (int i = 0; i < 20; ++i) {
    result[23] |= result[i];
  }
  // Since we're dealing with 32 bit ints, don't forget to mask off the top 2 bits
  result[23] = ~result[23] & ((1 << 30) - 1);

  return result;
}

State_t make_move(const State_t & state, int mover_index, int dest_pos) {
  State_t result = state;
  // xor state[mover_index] with itself to clear it.
  result[mover_index] ^= result[mover_index];
  // xor state[mover_index] with destination position to make the move
  result[mover_index] ^= dest_pos;

  // Bookkeeping, same as make_attack
  result[21] = opponent[state[21]];
  result[22] ^= result[22];  // zero opponents locations
//  int opponent_index = opponent_player_index[state[21]];
//  int end = opponent_index + 10;
  for (int opponent_index = opponent_player_index[result[21]], end = opponent_index + 10;
       opponent_index < end; ++opponent_index) {
    result[22] |= result[opponent_index];
  }
  // Update the location of the empty cells (negation of all cells with pieces)
  result[23] ^= result[23];
  for (int i = 0; i < 20; ++i) {
    result[23] |= result[i];
  }
  // Since we're dealing with 32 bit ints, don't forget to mask off the top 2 bits
  result[23] = ~result[23] & ((1 << 30) - 1);

  return result;
}

double alpha_Beta(state_value & state, int depth, double alpha, double beta, int & node_count) {
  if (depth == 0 || state.win) {
    return state.value;
  }

  Ordered_States_t ordered_child_states = get_ordered_children(state.state);

  double best_value = -50600.0; // -inf
  while(!ordered_child_states.empty()) {
    state_value next_state = ordered_child_states.top();
    ordered_child_states.pop();
    double value = -alpha_Beta(next_state, depth - 1, -beta, -alpha, ++node_count);
    best_value = std::max(best_value, value);
    alpha = std::max(alpha, value);
    if(alpha >= beta) {
      break;
    }
  }

  return best_value;
}


/**
 * A utility function to be able to change a string into a zmq message
 *
 * @param msg The string to package
 * @return The zeromq message type of that string
 */
zmq::message_t make_message(std::string msg) {
  std::stringstream result{msg};
  zmq::message_t move_message((void *) result.str().c_str(), result.str().size() + 1, NULL);
  return move_message;
}

bool black_has_been_promoted(int type, unsigned int location) {
  return (type > 4) && (location > (1 << 30));
}

bool white_has_been_promoted(int type, unsigned int location) {
  return (type > 9 && type < 15) && (location > (1 << 30));
}

double evaluate_state(State_t state) {
  double result = 0.0;

  for (int i = 0, q_idx = 1; i < 20; ++i) {
    if (i == 10) { q_idx = 18; }
    if (state[i] == 0) {
      // pass
    } else if (state[i] > 0 && state[i] < (1 << 30)) {
      if (state[21] == 1) {
        result += white_on_move_values[i];
      } else {
        result += black_on_move_values[i];
      }
    } else {
      if (state[21] == 1) {
        result += white_on_move_values[q_idx];
      } else {
        result += black_on_move_values[q_idx];
      }
    }
  }

  return result;
}

