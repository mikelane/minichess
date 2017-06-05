//
// Created by Michael Lane on 5/30/17.
//

#include <sstream>
#include "Player.h"

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
 */

/**
 * I found this little snippet of goodness on Stack Overflow: http://stackoverflow.com/questions/236129/split-a-string-in-c
 *
 * @param input - A space-separated list of integers as a std::string
 * @return A vector of unsigned ints ... the game state!
 */
State_t &Player::parse_input(const std::string &input) {
  std::istringstream iss(input);
  current_state = {std::istream_iterator<unsigned int>{iss}, std::istream_iterator<unsigned int>{}};
  return current_state;
}

/**
 * Generate all of the possible move strings for a given state.
 * @param state
 * @return
 */
std::vector<std::string> Player::generate_all_move_strings(const State_t &state) {
  std::vector<std::string> result, attack_strings, move_strings;
  // Get the starting index for the player on move
  int mover_index = my_player_index[state[21]];
  int end = mover_index + 10;
  std::string start_pos = "";
  std::string move_string = "";

  // Iterate over the pieces putting together move strings from bitboards
  for (; mover_index < end; ++mover_index) {
    if (state[mover_index]) {
      start_pos = TO_STR[state[mover_index]];
      move_string = start_pos + "-";

      int shadow_mask = generate_shadow_mask(state, mover_index);

      // Append attack strings
      std::vector<std::string> attacks = generate_attack_strings(state, mover_index, shadow_mask, move_string);
      attack_strings.insert(attack_strings.end(), attacks.begin(), attacks.end());

      // Append move strings
      std::vector<std::string> moves = generate_move_strings(state, mover_index, shadow_mask, move_string);
      move_strings.insert(move_strings.end(), moves.begin(), moves.end());
    }
  }
  result.insert(result.end(), attack_strings.begin(), attack_strings.end());
  result.insert(result.end(), move_strings.begin(), move_strings.end());
  return result;
}

std::vector<move> Player::generate_all_moves(const State_t &state) {
  std::vector<move> result, all_attacks, all_moves;
  // Get the starting index for the player on move
  int mover_index = my_player_index[state[21]];
  int end = mover_index + 10;
  std::string start_pos_str = "";
  std::string move_string = "";

  // Iterate over the pieces putting together move strings from bitboards
  for (; mover_index < end; ++mover_index) {
    if (state[mover_index]) {
      start_pos_str = TO_STR[state[mover_index]];
      move_string = start_pos_str + "-";

      int shadow_mask = generate_shadow_mask(state, mover_index);

      // Append attack strings
      std::vector<move> attacks = generate_attacks(state, mover_index, shadow_mask, move_string);
      all_attacks.insert(all_attacks.end(), attacks.begin(), attacks.end());

      // Append move strings
      std::vector<move> moves = generate_moves(state, mover_index, shadow_mask, move_string);
      all_moves.insert(all_moves.end(), moves.begin(), moves.end());
    }
  }
  result.insert(result.end(), all_attacks.begin(), all_attacks.end());
  result.insert(result.end(), all_moves.begin(), all_moves.end());
  return result;
}


/**
 * Given a piece at a given location look at all the other pieces that could potentially be
 * in the way and create a shadow of 0's behind those pieces. Note, knights don't create shadows.
 * @param state
 * @param mover_index
 * @return
 */
int Player::generate_shadow_mask(const State_t &state, int mover_index) {
  unsigned int shadow_mask = (1 << 30) - 1;

  // Generate the shadow cast by any piece on a move spoke
  if (mover_index != 3 && mover_index != 16) {  // Knights can jump over other players
    for (int j = 0; j < 20; ++j) {
      if (j != mover_index && state[j] && SHADOW_MASK[state[mover_index]].count(state[j])) {
        shadow_mask &= ~SHADOW_MASK[state[mover_index]][state[j]];
      }
    }
  }
  return shadow_mask;
}

/**
 * Given a piece, determine all the locations of valid attacks. Valid attacks are moves that end
 * at an opponent's location and that aren't blocked by any other piece.
 *
 * state[22] is the location of all opponent pieces.
 *
 * @param state
 * @param mover_index
 * @param shadow_mask
 * @param move_string
 * @return
 */
std::vector<std::string>
Player::generate_attack_strings(const State_t &state, int mover_index, int shadow_mask,
                                const std::string &move_string) {
  std::vector<std::string> result;
  // Iterate over possible destination locations for a given piece type at a given location
  // to determine if there are available attacks.
  for (unsigned int pos : type_to_attack[mover_index][state[mover_index]]) {
    if (pos & state[22] & shadow_mask) {
      int target_index = 0;
      for (; target_index < 20 && pos != state[target_index]; ++target_index);
      result.push_back(move_string + TO_STR[pos]);
    }
  }
  return result;
}

/**
 * Given a piece, determine all the locations of valid non-attack moves. A non-attack move is a move that
 * ends at an empty cell and where the mover is not blocked by any piece along the way.
 *
 * state[23] is the location of all empty cells.
 *
 * @param state
 * @param mover_index
 * @param shadow_mask
 * @param move_string
 * @return
 */
std::vector<std::string>
Player::generate_move_strings(const State_t &state, int mover_index, int shadow_mask, const std::string &move_string) {
  std::vector<std::string> result;
  // Iterate over possible empty destination cells for a given type at a given location.
  for (unsigned int pos : type_to_move[mover_index][state[mover_index]]) {
    if (pos & state[23] & shadow_mask) {
      result.push_back(move_string + TO_STR[pos]);
    }
  }
  return result;
}

std::vector<move>
Player::generate_attacks(const State_t &state, int mover_index, int shadow_mask, const std::string &move_string) {
  std::vector<move> result;

  for (unsigned int end_pos : type_to_attack[mover_index][state[mover_index]]) {
    if (end_pos & state[22] & shadow_mask) {
      int target_index = 0;
      for (; target_index < 20 && end_pos != state[target_index]; ++target_index);
      move resulting_move = {
          make_attack(state, mover_index, target_index),
          move_string + TO_STR[end_pos],
          state[20] + 1,
          state[mover_index],
          mover_index,
          end_pos,
          target_index,
          true,
          ((4 < mover_index) && (mover_index < 10) && (end_pos > (1 << 30))) ||
          ((9 < mover_index) && (mover_index < 15) && (end_pos > (1 << 30))),
          (state[0] == 0) || (state[19] == 0) || (state[20] + 1 > 40),
          calculate_value(state, mover_index, end_pos),
          0
      };
      result.push_back(resulting_move);
    }
  }
  return result;
}

std::vector<move>
Player::generate_moves(const State_t &state, int mover_index, int shadow_mask, const std::string &move_string) {
  std::vector<move> result;

  for (unsigned int end_pos : type_to_move[mover_index][state[mover_index]]) {
    if (end_pos & state[23] & shadow_mask) {
      move resulting_move = {
          make_move(state, mover_index, end_pos),
          move_string + TO_STR[end_pos],
          state[20] + 1,
          state[mover_index],
          mover_index,
          end_pos,
          -1,
          false,
          ((4 < mover_index) && (mover_index < 10) && (end_pos > (1 << 30))) ||
          ((9 < mover_index) && (mover_index < 15) && (end_pos > (1 << 30))),
          (state[0] == 0) || (state[19] == 0) || (state[20] + 1 > 40),
          calculate_value(state, mover_index, end_pos),
          0,
      };
      result.push_back(resulting_move);
    }
  }
  return result;
}

State_t Player::make_attack(const State_t &state, int attacker_index, int target_index) {
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

  // If this is a pawn and it's at either the top or bottom, make it a promoted pawn
  if ((4 < attacker_index && attacker_index < 15)
      && ((result[attacker_index] > (1 << 19) || ((1 << 5) > result[attacker_index])))) {
    result[attacker_index] |= (1 << 30);  // promoted pawn
  }

  // Update the move number
  ++result[20];
  return result;
}

State_t Player::make_move(const State_t &state, int mover_index, int dest_pos) {
  State_t result = state;
  // xor state[mover_index] with itself to clear it.
  result[mover_index] ^= result[mover_index];
  // xor state[mover_index] with destination position to make the move
  result[mover_index] ^= dest_pos;

  // Bookkeeping, same as make_attack
  result[21] = opponent[state[21]];
  result[22] ^= result[22];  // zero opponents locations

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

  // If this is a pawn and it's at either the top or bottom, make it a promoted pawn
  if ((4 < mover_index && mover_index < 15)
      && ((result[mover_index] > (1 << 19) || ((1 << 5) > result[mover_index])))) {
    result[mover_index] |= (1 << 30);  // promoted pawn
  }

  // Update move number
  ++result[20];

  return result;
}

int Player::calculate_material_value(const State_t &state) {
  int result = 0;

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

int Player::calculate_heuristic_value(const State_t &state) {
  int result = 0;
  for (int i = my_player_index[state[21]], end = i + 10; i < end; ++i) {
    if (state[20] >= 30 && i == 0) { // use black king end game heuristics
      result += heuristic_dispatch[20][state[i]];
    } else if (state[20] >= 30 && i == 19) {  // use white king end game heuristics
      result += heuristic_dispatch[21][state[i]];
    } else {
      result += heuristic_dispatch[i][state[i]];
    }
  }
  return result;
}

int Player::calculate_number_of_attacks(const State_t &state, int idx, bool opponent) {
  int result = 0;
  unsigned int locs;
  if (opponent) {
    locs = state[22];
  } else {
    locs = ~(state[22] | state[23]);
  }
  for (int i = idx, end = i + 10; i < end; ++i) {
    int attacks = 0;
    unsigned int combined_attacks = combined_attack_dispatch[i][state[i]];
    combined_attacks &= ~(state[i]);
    int shadows = generate_shadow_mask(state, i);
    attacks |= (locs & combined_attacks & shadows);
    result += __builtin_popcount(attacks);
  }
  return result;
}

int Player::calculate_number_of_moves(const State_t &state, int idx) {
  int result = 0;
  for (int i = idx, end = i + 10; i < end; ++i) {
    int moves = 0;
    unsigned int combined_moves = combined_move_dispatch[i][state[i]];
    int shadows = generate_shadow_mask(state, i);
    moves |= (state[23] & combined_moves & shadows);
    result += __builtin_popcount(moves);
  }
  return result;
}

int Player::calculate_value(const State_t &state, int mover_index, unsigned int end_pos) {
  State_t new_state = make_move(state, mover_index, end_pos);
  int result = 0;

  // Material value
  result += calculate_material_value(new_state);

  // Heuristic Value
//  result += calculate_heuristic_value(new_state);

  // Penalize a king or queen move early on in the game
//  if (mover_index == 0 || mover_index == 19) {
//    result += (2 * new_state[20] * new_state[20] - 500);
//  } else if (mover_index == 1 || mover_index == 18) {
//    result += (0.5 * new_state[20] * new_state[20] - 500);
//  }

  // Opponent mobility
  // Number of opponent attacks
//  int num_opponent_attacks = calculate_number_of_attacks(new_state, opponent_player_index[new_state[21]], false);
  // subtract 25 * number of attacks from result
//  result += -25 * num_opponent_attacks;

  // number of opponent's moves
//  int num_opponent_moves = calculate_number_of_moves(new_state, opponent_player_index[new_state[21]]);
//  result += -10 * num_opponent_moves;
//
  // Number of attacks I can make from this position
//  int num_defended = calculate_number_of_attacks(new_state, my_player_index[new_state[21]], false);
  // Add back in 25 for each of the defenses I can mount
//  result += 25 * num_defended;

  return result;
}

