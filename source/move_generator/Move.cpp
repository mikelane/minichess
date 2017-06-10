//
// Created by Michael Lane on 6/9/17.
//

#include <tuple>
#include "Move.h"

Move &Move::operator=(const Move &other) {
  move_string = other.move_string;
  mover_idx = other.mover_idx;
  target_idx = other.target_idx;
  end_pos = other.end_pos;
  value = other.value;
  attack = other.attack;
  return *this;
}

Move Move::operator-() const {
  return Move(move_string, mover_idx, target_idx, end_pos, -value, attack);
}

bool Move::operator==(const Move &rhs) const {
  return std::tie(move_string, mover_idx, target_idx, end_pos, value, attack) ==
         std::tie(rhs.move_string, rhs.mover_idx, rhs.target_idx, rhs.end_pos, rhs.value, rhs.attack);
}

bool Move::operator!=(const Move &rhs) const {
  return !(rhs == *this);
}

bool Move::operator<(const Move &rhs) const {
  return value < rhs.value;
}

bool Move::operator>(const Move &rhs) const {
  return rhs < *this;
}

bool Move::operator<=(const Move &rhs) const {
  return !(rhs < *this);
}

bool Move::operator>=(const Move &rhs) const {
  return !(*this < rhs);
}

const std::string &Move::get_move_string() const {
  return move_string;
}

int Move::get_mover_idx() const {
  return mover_idx;
}

int Move::get_target_idx() const {
  return target_idx;
}

unsigned int Move::get_end_pos() const {
  return end_pos;
}

int Move::get_value() const {
  return value;
}

bool Move::is_attack() const {
  return attack;
}
