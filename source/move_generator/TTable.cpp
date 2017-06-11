//
// Created by Michael Lane on 5/28/17.
//

#include <iostream>
#include "TTable.h"


void TTable::insert(TTable_Entry new_entry) {
  unsigned long long int key = new_entry.getHash() & mask;
  TTable_Entry existing_entry = entries[key];
  if (!existing_entry.isValid()) {
    entries[key] = new_entry;
    return;
  }

  if (new_entry.getDepth() >= existing_entry.getDepth()) {
    entries[key] = new_entry;
  }
}

TTable_Entry TTable::get_entry(const State_t &state) {
  unsigned long long hash = zobrist_table.hash_state(state);
  TTable_Entry entry = entries[hash & mask];
  if (entry.getHash() == hash) {
    return entry;
  } else {
    return {};
  }
}


