//
// Created by Michael Lane on 5/13/17.
//

#include "move_generator.h"
#include <iostream>
#include <random>
#include <algorithm>

/**
 * The get move function returns the string that the server needs. For example a move
 * from B2 to C3 would be B2-C3.
 * @param piece_locations - This is an array of piece locations where the index indicates the
 *      piece type and the value at that index is the location of that piece type on the board.
 *      The piece types should be arranged this way: [k,q,b,n,r,p,p,p,p,p,P,P,P,P,P,R,N,B,Q,K].
 *      The locations on the board are denoted by 30-bit integers. The value should correspond
 *      to one of the 30 board locations. 1 << 30 is the top left corner (A6), 1 << 0 is the
 *      bottom right corner (E1).
 * @param on_move - The player that is on move. 1 is Black, 2 is White.
 * @param empty_locs - An integer that corresponds to the 30-bit board with 1 in all the
 *      cells that that are empty.
 * @param opponent_locs - An integer that corresponds to the 30-bit board with 1 in all the
 *      cells that have an opponent in them
 * @TODO Add negamax option
 * @TODO add aB pruning
 * @TODO add Iterative Deepening
 * @TODO add timing
 * @return - A string corresponding to a single move in the format A6-B5.
 */
std::string get_move(const unsigned int piece_locations[],
                     const unsigned int on_move,
                     const unsigned int empty_locs,
                     const unsigned int opponent_locs) {
  unsigned int to_move_location;

  std::vector<std::string> legal_moves = {};
  std::vector<std::string> legal_attacks = {};

  std::string result = "";

  // The piece_locations array has the locations of the 10 black players followed
  // by the location of the 10 white players. Search only one or the other for the
  // starting move location
  int i = (on_move == 1) ? BLACK_IDX : WHITE_IDX;
  int end = i + 10;

  // Find all the moves available to each piece for the on move player
  for (; i < end; ++i) {
    if (piece_locations[i] != 0u) {  // If the piece hasn't been captured
      to_move_location = piece_locations[i];
      result = TO_STR[to_move_location] + "-";

      // Set up the shadows so that there are none initially. If a 1 is in a cell,
      // that means you can get to that cell.
      unsigned int shadows = (1 << 30) - 1;

      // If the starting piece is not a Knight, mask the shadows of all the pieces
      // (for me and for the opponent) that could be in the way.
      if (i != 3 && i != 16) {  // Knights can jump over other players
        for (int j = 0; j < 20; ++j) {
          // Don't try to cast your own shadow, make sure the piece in question is on the board
          // and make sure the piece location exists as a key in the SHADOW_MASK dict.
          if (j != i && piece_locations[j] && SHADOW_MASK[to_move_location].count(piece_locations[j])) {
            // If so, add the relevant shadow to the mask
            shadows &= ~SHADOW_MASK[to_move_location][piece_locations[j]];
          }
        }
      }

      // For all the possible move locations for a given piece type and location, if there is a
      // legal attack, then add that attack to the list of legal attacks.
      for (unsigned int pos : type_to_attack[i][to_move_location]) {
        if (pos & opponent_locs & shadows) {
          legal_attacks.push_back(result + TO_STR[pos]);
        }
      }

      // Second verse, same as the first. Except this time, for moves not for attacks.
      for (unsigned int pos : type_to_move[i][to_move_location]) {
        if (pos & empty_locs & shadows) {
          legal_moves.push_back(result + TO_STR[pos]);
        }
      }
    }
  }

  if (legal_attacks.size() || legal_moves.size()) {
    legal_attacks.insert(legal_attacks.end(), legal_moves.begin(), legal_moves.end());
    std::mt19937 r{std::random_device{}()};
    std::shuffle(std::begin(legal_attacks), std::end(legal_attacks), r);
    return legal_attacks[0];
  } else {
    return "ERROR";
  }
}
