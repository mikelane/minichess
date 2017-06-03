//
// Created by Michael Lane on 5/28/17.
//

#include <iostream>
#include "ttable.h"


void ttable::insert(Ttable_Entry new_entry) {
  unsigned long long int key = new_entry.getHash() & mask;
  Ttable_Entry existing_entry = entries[key];
  if (!existing_entry.isValid()) {
    entries[key] = new_entry;
    return;
  }

  if (new_entry.getDepth() >= existing_entry.getDepth()) {
    entries[key] = new_entry;
  }
}

Ttable_Entry ttable::get_entry(unsigned long long int hash) {
  Ttable_Entry entry = entries[hash & mask];
  if (entry.getHash() == hash) {
    return entry;
  } else {
    return {};
  }
}


