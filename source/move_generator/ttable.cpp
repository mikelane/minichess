//
// Created by Michael Lane on 5/28/17.
//

#include "ttable.h"


void ttable::insert(Ttable_Entry entry) {
  entries[entry.getHash() & mask] = entry;
}

Ttable_Entry &ttable::get_entry(unsigned long long int hash) {
  return entries[hash & mask];
}


