//
// Created by Michael Lane on 5/28/17.
//

#include "Ttable_Entry.h"

unsigned long long int Ttable_Entry::getHash() const {
  return hash;
}

void Ttable_Entry::setHash(unsigned long long int hash) {
  Ttable_Entry::hash = hash;
}

int Ttable_Entry::getA() const {
  return a;
}

void Ttable_Entry::setA(int a) {
  Ttable_Entry::a = a;
}

int Ttable_Entry::getB() const {
  return b;
}

void Ttable_Entry::setB(int b) {
  Ttable_Entry::b = b;
}

int Ttable_Entry::getValue() const {
  return value;
}

void Ttable_Entry::setValue(int value) {
  Ttable_Entry::value = value;
}

bool Ttable_Entry::isValid() const {
  return valid;
}

void Ttable_Entry::setValid(bool valid) {
  Ttable_Entry::valid = valid;
}

int Ttable_Entry::getDepth() const {
  return depth;
}

void Ttable_Entry::setDepth(int depth) {
  Ttable_Entry::depth = depth;
}

ttable_flag Ttable_Entry::getFlag() const {
  return flag;
}

void Ttable_Entry::setFlag(ttable_flag flag) {
  Ttable_Entry::flag = flag;
}

