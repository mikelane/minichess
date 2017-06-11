//
// Created by Michael Lane on 5/28/17.
//

#ifndef MOVE_GENERATOR_TTABLE_H
#define MOVE_GENERATOR_TTABLE_H

#include <unordered_map>
#include <random>
#include "TTable_Entry.h"
#include "Zobrist_Table.h"

/**
 * The class that manages the transposition tables.
 */
class TTable {
public:
  void insert(TTable_Entry &new_entry, const State_t &state);

  TTable_Entry get_entry(const State_t &state);

private:
  Zobrist_Table zobrist_table;
  unsigned long long mask{(1 << 21) - 1};
  std::unordered_map<unsigned int, TTable_Entry> entries{};
};

#endif //MOVE_GENERATOR_TTABLE_H
