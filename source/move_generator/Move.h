//
// Created by Michael Lane on 6/9/17.
//

#ifndef MOVE_GENERATOR_MOVE_H
#define MOVE_GENERATOR_MOVE_H


#include <string>
#include <utility>

class Move {
private:
  std::string move_string{""};
  int mover_idx{-1};
  int target_idx{-1};
  unsigned int end_pos{0};
  int value{0};
  bool attack{false};

public:
  const std::string &get_move_string() const;

  int get_mover_idx() const;

  int get_target_idx() const;

  unsigned int get_end_pos() const;

  int get_value() const;

  bool is_attack() const;

  Move() = default;

  Move(std::string move_string, int mover_idx, int target_idx, unsigned int end_pos, int value, bool attack)
      : move_string(std::move(move_string)),
        mover_idx(mover_idx),
        target_idx(target_idx),
        end_pos(end_pos),
        value(value),
        attack(attack) {};

  Move &operator=(const Move &other);

  bool operator==(const Move &rhs) const;

  bool operator!=(const Move &rhs) const;

  bool operator<(const Move &rhs) const;

  bool operator>(const Move &rhs) const;

  bool operator<=(const Move &rhs) const;

  bool operator>=(const Move &rhs) const;

  /**
 * Negates the value. Handy for negamax function.
 *
 * @return
 */
  Move operator-() const;
};


#endif //MOVE_GENERATOR_MOVE_H
