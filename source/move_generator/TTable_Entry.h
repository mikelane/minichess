//
// Created by Michael Lane on 5/28/17.
//

#ifndef MOVE_GENERATOR_TTABLE_ENTRY_H
#define MOVE_GENERATOR_TTABLE_ENTRY_H


#include "bitboard_tables.h"

enum ttable_flag {
  LOWER_BOUND = -1,
  EXACT_VALUE = 0,
  UPPER_BOUND = 1,
};

class TTable_Entry {
public:
  unsigned long long int getHash() const;

  void setHash(unsigned long long int hash);

  int getA() const;

  void setA(int a);

  int getB() const;

  void setB(int b);

  int getValue() const;

  void setValue(int value);

  bool isValid() const;

  void setValid(bool valid);

  int getDepth() const;

  void setDepth(int depth);

  ttable_flag getFlag() const;

  void setFlag(ttable_flag flag);

private:
  unsigned long long hash{0ull};
  int depth{0};
  int a{0};
  int b{0};
  int value{0};
  bool valid{false};
  ttable_flag flag{EXACT_VALUE};
};


#endif //MOVE_GENERATOR_TTABLE_ENTRY_H
