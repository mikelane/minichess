//
// Created by Michael Lane on 5/28/17.
//

#ifndef MOVE_GENERATOR_ZOBRIST_TABLE_H
#define MOVE_GENERATOR_ZOBRIST_TABLE_H


#include <random>
#include "bitboard_tables.h"

typedef std::vector<unsigned int> State_t;

class Zobrist_Table {

public:
  Zobrist_Table();

  unsigned long long int hash_state(State_t state);

  unsigned long long int
  update_hash(unsigned long long int hash, int source_type, int source_loc, int dest_type, int dest_loc);

  unsigned long long int random_int();

private:
  std::random_device rd;
  std::mt19937 mt{rd()};
  unsigned long long zobrist_black;
  unsigned long long table[30][13]{};
};


#endif //MOVE_GENERATOR_ZOBRIST_TABLE_H
