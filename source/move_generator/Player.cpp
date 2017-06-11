//
// Created by Michael Lane on 5/30/17.
//

#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include "Move.h"
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
State_t Player::parse_input(const std::string &input) {
  std::istringstream iss(input);
  return {
      std::istream_iterator<unsigned int>{iss},
      std::istream_iterator<unsigned int>{}
  };
}

/**
 * Generate all of the possible move strings for a given state.
 * @param state
 * @return
 */
std::vector<std::string> Player::generate_all_move_strings(const State_t &state) {
  std::vector<std::string> result, attack_strings, move_strings;
  // Get the starting index for the player on Move
  int mover_index = my_player_index[state[PLAYER_ON_MOVE]];
  int end = mover_index + 10;
  std::string start_pos = "";
  std::string move_string = "";

  // Iterate over the pieces putting together Move strings from bitboards
  for (; mover_index < end; ++mover_index) {
    if (state[mover_index]) {
      start_pos = TO_STR[state[mover_index]];
      move_string = start_pos + "-";

      int shadow_mask = generate_shadow_mask(state, mover_index);

      // Append attack strings
      std::vector<std::string> attacks = generate_attack_strings(state, mover_index, shadow_mask, move_string);
      attack_strings.insert(attack_strings.end(), attacks.begin(), attacks.end());

      // Append Move strings
      std::vector<std::string> moves = generate_move_strings(state, mover_index, shadow_mask, move_string);
      move_strings.insert(move_strings.end(), moves.begin(), moves.end());
    }
  }
  result.insert(result.end(), attack_strings.begin(), attack_strings.end());
  result.insert(result.end(), move_strings.begin(), move_strings.end());
  return result;
}

std::vector<Move> Player::generate_all_moves(const State_t &state) {
  if (state[BLACK_KING] == 0 || state[WHITE_KING] == 0) {
    std::cerr << "Trying to generate moves from a terminal state!" << std::endl;
  }
  std::vector<Move> result, all_attacks, all_moves;
  // Get the starting index for the player on Move
  int mover_index = my_player_index[state[PLAYER_ON_MOVE]];
  int end = mover_index + 10;
  std::string start_pos_str = "";
  std::string move_string = "";

  // Iterate over the pieces putting together Move strings from bitboards
  for (; mover_index < end; ++mover_index) {
    if (state[mover_index]) {
      start_pos_str = TO_STR[state[mover_index]];
      move_string = start_pos_str + "-";

      int shadow_mask = generate_shadow_mask(state, mover_index);

      // Append attack strings
      std::vector<Move> attacks = generate_attacks(state, mover_index, shadow_mask, move_string);
      all_attacks.insert(all_attacks.end(), attacks.begin(), attacks.end());

      // Append Move strings
      std::vector<Move> moves = generate_moves(state, mover_index, shadow_mask, move_string);
      all_moves.insert(all_moves.end(), moves.begin(), moves.end());
    }
  }
  std::shuffle(all_attacks.begin(), all_attacks.end(), mt);
  std::sort(all_attacks.begin(), all_attacks.end());
  std::shuffle(all_moves.begin(), all_moves.end(), mt);
  std::sort(all_moves.begin(), all_moves.end());
  result.insert(result.end(), all_attacks.begin(), all_attacks.end());
  result.insert(result.end(), all_moves.begin(), all_moves.end());
  if (result.size() < 1) {
    std::cerr << "Failed to generate any legal moves!" << std::endl;
    std::cerr << "State: " << std::endl;
    for (unsigned int val : state) {
      std::cerr << val << " ";
    }
    std::cerr << std::endl;
  }
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

  // Generate the shadow cast by any piece on a Move spoke
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
 * state[LOCATION_OF_OPPONENTS] is the location of all opponent pieces.
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
    if (pos & state[LOCATION_OF_OPPONENTS] & shadow_mask) {
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
 * state[LOCATION_OF_EMPTY] is the location of all empty cells.
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
    if (pos & state[LOCATION_OF_EMPTY] & shadow_mask) {
      result.push_back(move_string + TO_STR[pos]);
    }
  }
  return result;
}

std::vector<Move>
Player::generate_attacks(const State_t &state, int mover_index, int shadow_mask, const std::string &move_string) {
  std::vector<Move> result;

  for (unsigned int end_pos : type_to_attack[mover_index][state[mover_index]]) {
    if (end_pos & state[LOCATION_OF_OPPONENTS] & shadow_mask) {
      int target_index = 0;
      for (; target_index < 20 && end_pos != (((1 << 30) -1) & state[target_index]); ++target_index);
      if (target_index > 19) {
        //pass
      }
      State_t new_state = make_attack(state, mover_index, target_index);
      result.push_back(
          Move(
              move_string + TO_STR[end_pos],  // move_string
              mover_index,                    // mover_idx
              target_index,                   // target_idx
              end_pos,                        // end_pos
              eval(new_state),                // value
              true                            // attack
          ));
    }
  }
  return result;
}

std::vector<Move>
Player::generate_moves(const State_t &state, int mover_index, int shadow_mask, const std::string &move_string) {
  std::vector<Move> result;

  for (unsigned int end_pos : type_to_move[mover_index][state[mover_index]]) {
    if (end_pos & state[LOCATION_OF_EMPTY] & shadow_mask) {
      State_t new_state = make_move(state, mover_index, end_pos);
      result.push_back(
          Move(
              move_string + TO_STR[end_pos],  // move_string
              mover_index,                    // mover_idx
              -1,                             // target_idx
              end_pos,                        // end_pos
              eval(new_state),                // value
              false                           // attack
          ));
    }
  }
  return result;
}

State_t Player::make_attack(const State_t &state, int attacker_index, int target_index) {
  State_t result = state;
  // xor attacker with itself, zeros it
  result[attacker_index] ^= result[attacker_index];
  // xor attacker with target moves the attacker (mask off promoted pawn)
  result[attacker_index] ^= (result[target_index] & ((1 << 30) - 1));
  // xor target with itself zeros it
  result[target_index] ^= result[target_index];

  if((((0 <= attacker_index) && (attacker_index < 5))
      || ((15 <= attacker_index) && (attacker_index < 20)))
      && (result[attacker_index] >= (1 << 30))) {
    std::cerr << std::endl << "-------------------------------------------------------------" << std::endl;
    std::cerr << "make attack error!" << std::endl;
    std::cerr << "state[" << attacker_index << "]: " << state[attacker_index] << std::endl;
    std::cerr << " state[" << target_index << "]: " << state[target_index] << std::endl;
    std::cerr << "result[" << attacker_index << "]: " << result[attacker_index] << std::endl;
    std::cerr << " result[" << target_index << "]: " << result[target_index] << std::endl;

    std::cerr << "Incoming state: ";
    for (unsigned int v : state) {
      std::cerr << v << " ";
    }
    std::cerr << "Resulting state: ";
    for (unsigned int v : result) {
      std::cerr << v << " ";
    }
    std::cerr << std::endl;
  }

  // Bookkeeping. Update the player on Move. Update the opponents location. Update the empty cells
  result[PLAYER_ON_MOVE] = opponent[result[PLAYER_ON_MOVE]];
  result[LOCATION_OF_OPPONENTS] ^= result[LOCATION_OF_OPPONENTS];  // zero opponents locations
  for (int opponent_index = opponent_player_index[result[PLAYER_ON_MOVE]], end = opponent_index + 10;
       opponent_index < end; ++opponent_index) {
    result[LOCATION_OF_OPPONENTS] |= result[opponent_index];
  }

  // Update the location of the empty cells (negation of all cells with pieces)
  result[LOCATION_OF_EMPTY] ^= result[LOCATION_OF_EMPTY];
  for (int i = 0; i < 20; ++i) {
    result[LOCATION_OF_EMPTY] |= result[i];
  }

  // Since we're dealing with 32 bit ints, don't forget to mask off the top 2 bits
  result[LOCATION_OF_EMPTY] = ~result[LOCATION_OF_EMPTY] & ((1 << 30) - 1);

  // If this is a pawn and it's at either the top or bottom, make it a promoted pawn
  if ((4 < attacker_index && attacker_index < 15)
      && ((result[attacker_index] > (1 << 19) || ((1 << 5) > result[attacker_index])))) {
    result[attacker_index] |= (1 << 30);  // promoted pawn
  }

  // Update the Move number
  ++result[MOVE_NUMBER];
  return result;
}

State_t Player::make_move(const State_t &state, int mover_index, int dest_pos) {
  State_t result = state;
  // xor state[mover_index] with itself to clear it.
  result[mover_index] ^= result[mover_index];
  // xor state[mover_index] with destination position to make the Move
  result[mover_index] ^= dest_pos;

  // Bookkeeping, same as make_attack
  result[PLAYER_ON_MOVE] = opponent[state[PLAYER_ON_MOVE]];
  result[LOCATION_OF_OPPONENTS] ^= result[LOCATION_OF_OPPONENTS];  // zero opponents locations

  for (int opponent_index = opponent_player_index[result[PLAYER_ON_MOVE]], end = opponent_index + 10;
       opponent_index < end; ++opponent_index) {
    result[LOCATION_OF_OPPONENTS] |= result[opponent_index];
  }

  // Update the location of the empty cells (negation of all cells with pieces)
  result[LOCATION_OF_EMPTY] ^= result[LOCATION_OF_EMPTY];
  for (int i = 0; i < 20; ++i) {
    result[LOCATION_OF_EMPTY] |= result[i];
  }

  // Since we're dealing with 32 bit ints, don't forget to mask off the top 2 bits
  result[LOCATION_OF_EMPTY] = ~result[LOCATION_OF_EMPTY] & ((1 << 30) - 1);

  // If this is a pawn and it's at either the top or bottom, make it a promoted pawn
  if ((4 < mover_index && mover_index < 15)
      && ((result[mover_index] > (1 << 19) || ((1 << 5) > result[mover_index])))) {
    result[mover_index] |= (1 << 30);  // promoted pawn
  }

  // Update Move number
  ++result[MOVE_NUMBER];

  return result;
}

State_t Player::make_move(const State_t &state, const Move &move) {
  if (move.is_attack()) {
    return make_attack(state, move.get_mover_idx(), move.get_target_idx());
  }
  return make_move(state, move.get_mover_idx(), move.get_end_pos());
}

int Player::calculate_material_value(const State_t &state) {
  int result = 0;

  for (int i = 0, q_idx = 1; i < 20; ++i) {
    if (i == 10) { q_idx = 18; }
    if (state[i] == 0) {
      // pass
    } else if (state[i] > 0 && state[i] < (1 << 30)) {
      if (state[PLAYER_ON_MOVE] == 1) {
        result += white_on_move_values[i];
      } else {
        result += black_on_move_values[i];
      }
    } else {
      if (state[PLAYER_ON_MOVE] == 1) {
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
  for (int i = my_player_index[state[PLAYER_ON_MOVE]], end = i + 10; i < end; ++i) {
    if (state[MOVE_NUMBER] <= 10) {
      result += begin_heuristic_dispatch[i][state[i]];
    } else if (state[MOVE_NUMBER] <= 25) {
      result += middle_heuristic_dispatch[i][state[i]];
    } else {
      result += end_heuristic_dispatch[i][state[i]];
    }
  }
  return result;
}

int Player::calculate_number_of_attacks(const State_t &state, int idx, bool opponent) {
  int result = 0;
  unsigned int locs;
  if (opponent) {
    locs = state[LOCATION_OF_OPPONENTS];
  } else {
    locs = ~(state[LOCATION_OF_OPPONENTS] | state[LOCATION_OF_EMPTY]);
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
    moves |= (state[LOCATION_OF_EMPTY] & combined_moves & shadows);
    result += __builtin_popcount(moves);
  }
  return result;
}

int Player::eval(const State_t &state) {
  int result = 0;

  // Material value
  result += calculate_material_value(state);

  // Heuristic Value
  result += calculate_heuristic_value(state);

  // Opponent mobility
  // Number of opponent attacks
  int num_opponent_attacks = calculate_number_of_attacks(state, opponent_player_index[state[PLAYER_ON_MOVE]], false);
  // subtract 25 * number of attacks from result
  result += -25 * num_opponent_attacks;

  // Number of attacks I can make from this position
  int num_defended = calculate_number_of_attacks(state, my_player_index[state[PLAYER_ON_MOVE]], false);
  // Add back in 25 for each of the defenses I can mount
  result += 25 * num_defended;

  // number of opponent's moves
  int num_opponent_moves = calculate_number_of_moves(state, opponent_player_index[state[PLAYER_ON_MOVE]]);
  result += -10 * num_opponent_moves;

  // number of my moves
  int num_my_moves = calculate_number_of_moves(state, my_player_index[state[PLAYER_ON_MOVE]]);
  result += 10 * num_my_moves;

  return result;
}

bool Player::is_terminal(const State_t &state) {
  return (state[MOVE_NUMBER] == 41)
         || (state[BLACK_KING] == 0)
         || (state[WHITE_KING] == 0);
}


/**
 * This uses a little help from Stack Overflow:
 * https://stackoverflow.com/questions/31255486/c-how-do-i-convert-a-stdchronotime-point-to-long-and-back
 * @param state
 * @return
 */
void Player::set_time_limit(const State_t &state) {
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

long long Player::get_millisecond_time() {
  using namespace std::chrono;
  // Get current time with precision of milliseconds
  auto now = time_point_cast<milliseconds>(system_clock::now());
  return now.time_since_epoch().count();
}
