//
// Created by Michael Lane on 6/9/17.
//

#ifndef MOVE_GENERATOR_NEGAMAX_RESULT_H
#define MOVE_GENERATOR_NEGAMAX_RESULT_H

#include <string>
#include <utility>
#include <random>

/**
 * A class to help deal with the results of the negamax function. It is
 * used to return relevant information other than the value.
 */
class Negamax_Result {
public:
  Negamax_Result() = default;

  Negamax_Result(int value, std::string move_string) : value(value), move_string(std::move(move_string)) {}

  Negamax_Result &operator=(const Negamax_Result &other);

  int get_value() const;

  void set_value(int value);

  const std::string &get_move_string() const;

  void set_move_string(const std::string &move_string);

  bool operator==(const Negamax_Result &rhs) const;

  bool operator!=(const Negamax_Result &rhs) const;

  Negamax_Result operator-();

  bool isLoss() const;

  void setLoss(bool loss);

  bool isWin() const;

  void setWin(bool win);

private:
  int value{0};
  bool loss{false};
  bool win{false};
  std::string move_string;
};


#endif //MOVE_GENERATOR_NEGAMAX_RESULT_H
