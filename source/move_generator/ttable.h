//
// Created by Michael Lane on 5/28/17.
//

#ifndef MOVE_GENERATOR_TTABLE_H
#define MOVE_GENERATOR_TTABLE_H

#include <unordered_map>
#include <random>
#include "Ttable_Entry.h"

class ttable {
public:
  void insert(Ttable_Entry entry);

  Ttable_Entry &get_entry(unsigned long long int hash);

private:
  unsigned long long int mask{(1 << 21) - 1};
  std::unordered_map<unsigned int, Ttable_Entry> entries{};
};

#endif //MOVE_GENERATOR_TTABLE_H
