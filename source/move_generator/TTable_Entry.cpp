//
// Created by Michael Lane on 5/28/17.
//

#include "TTable_Entry.h"

unsigned long long int TTable_Entry::getHash() const {
  return hash;
}

void TTable_Entry::setHash(unsigned long long int hash) {
  TTable_Entry::hash = hash;
}

int TTable_Entry::getA() const {
  return a;
}

void TTable_Entry::setA(int a) {
  TTable_Entry::a = a;
}

int TTable_Entry::getB() const {
  return b;
}

void TTable_Entry::setB(int b) {
  TTable_Entry::b = b;
}

int TTable_Entry::getValue() const {
  return value;
}

void TTable_Entry::setValue(int value) {
  TTable_Entry::value = value;
}

bool TTable_Entry::isValid() const {
  return valid;
}

void TTable_Entry::setValid(bool valid) {
  TTable_Entry::valid = valid;
}

int TTable_Entry::getDepth() const {
  return depth;
}

void TTable_Entry::setDepth(int depth) {
  TTable_Entry::depth = depth;
}

ttable_flag TTable_Entry::getFlag() const {
  return flag;
}

void TTable_Entry::setFlag(ttable_flag flag) {
  TTable_Entry::flag = flag;
}

