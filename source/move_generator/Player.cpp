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
void Player::parse_input(std::string input) {
  std::istringstream iss(input);
  current_state = {std::istream_iterator<unsigned int>{iss}, std::istream_iterator<unsigned int>{}};
}

/**
 * Generate all of the possible move strings for a given state.
 * @param state
 * @return
 */
std::vector<std::string> Player::generate_all_move_strings(State_t &state) {
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

/**
 * Given a piece at a given location look at all the other pieces that could potentially be
 * in the way and create a shadow of 0's behind those pieces. Note, knights don't create shadows.
 * @param state
 * @param mover_index
 * @return
 */
int Player::generate_shadow_mask(State_t state, int mover_index) {
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
Player::generate_attack_strings(State_t state, int mover_index, int shadow_mask, std::string move_string) {
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
Player::generate_move_strings(State_t state, int mover_index, int shadow_mask, std::string move_string) {
  std::vector<std::string> result;
  // Iterate over possible empty destination cells for a given type at a given location.
  for (unsigned int pos : type_to_move[mover_index][state[mover_index]]) {
    if (pos & state[23] & shadow_mask) {
      result.push_back(move_string + TO_STR[pos]);
    }
  }
  return result;
}

