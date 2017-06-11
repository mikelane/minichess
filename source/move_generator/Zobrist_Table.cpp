//
// Created by Michael Lane on 5/28/17.
//

#include <iostream>
#include "Zobrist_Table.h"

Zobrist_Table::Zobrist_Table() {
  zobrist_black = random_int();
  for (int i = 0; i < 30; ++i) {
    for (int j = 0; j < 13; ++j) {
      table[i][j] = random_int();
    }
  }
}

/**
 * I modified some code from Geeks for Geeks to make it work for me.
 * http://www.geeksforgeeks.org/minimax-algorithm-in-game-theory-set-5-zobrist-hashing/
 * @return A random unsigned long long int from [0, 2^64).
 */
unsigned long long int Zobrist_Table::random_int() {
  std::uniform_int_distribution<unsigned long long int> dist(0, UINT64_MAX);
  return dist(mt);
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
unsigned long long int Zobrist_Table::hash_state(State_t state) {
  unsigned long long int result = 0u;
  for (int i = 0; i < 20; ++i) {
    if (state[i]) { // If the piece exists
      if ((4 < i) && (i < 10) && (state[i] > (1 << 30))) {
        result ^= (table[TO_IDX[state[i]]][piece_type_zobrist_index[1]]);
      } else if ((9 < i) && (i < 15) && (state[i] > (1 << 30))) {
        result ^= (table[TO_IDX[state[i]]][piece_type_zobrist_index[18]]);
      } else {  // Piece as listed above.
        result ^= (table[TO_IDX[state[i]]][piece_type_zobrist_index[i]]);
      }
    }
  }
  // Handle empty cells:
  for (int mask = (1 << 29), i = 29; mask > 0; mask >>= 1, --i) {
    if (state[LOCATION_OF_EMPTY] & mask) {
      result ^= table[i][12];  // xor in location of empty cell
    }
  }
  // Handle black on move
  if (state[PLAYER_ON_MOVE] == 2) {
    result ^= zobrist_black;
  }
  return result;
}

unsigned long long int
Zobrist_Table::update_hash(unsigned long long int hash, int source_type, int source_loc, int dest_type, int dest_loc) {
  return hash
         ^ table[source_loc][source_type]
         ^ table[source_loc][12]
         ^ table[dest_loc][dest_type]
         ^ table[dest_loc][source_type]
         ^ zobrist_black;
}



