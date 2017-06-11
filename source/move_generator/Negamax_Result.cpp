//
// Created by Michael Lane on 6/9/17.
//

#include <utility>
#include "Negamax_Result.h"

int Negamax_Result::get_value() const {
  return value;
}

void Negamax_Result::set_value(int value) {
  Negamax_Result::value = value;
}

const std::string &Negamax_Result::get_move_string() const {
  return move_string;
}

void Negamax_Result::set_move_string(const std::string &move_string) {
  Negamax_Result::move_string = move_string;
}

bool Negamax_Result::operator==(const Negamax_Result &rhs) const {
  return value == rhs.value &&
         move_string == rhs.move_string;
}

bool Negamax_Result::operator!=(const Negamax_Result &rhs) const {
  return !(rhs == *this);
}

Negamax_Result Negamax_Result::operator-() {
  return Negamax_Result(-value, move_string);
}

Negamax_Result &Negamax_Result::operator=(const Negamax_Result &other) {
  if (&other != this) {
    value = other.value;
    move_string = other.move_string;
  }
  return *this;
}

bool Negamax_Result::isLoss() const {
  return loss;
}

void Negamax_Result::setLoss(bool loss) {
  Negamax_Result::loss = loss;
}

bool Negamax_Result::isWin() const {
  return win;
}

void Negamax_Result::setWin(bool win) {
  Negamax_Result::win = win;
}

