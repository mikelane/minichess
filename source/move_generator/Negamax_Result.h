//
// Created by Michael Lane on 6/9/17.
//

#ifndef MOVE_GENERATOR_NEGAMAX_RESULT_H
#define MOVE_GENERATOR_NEGAMAX_RESULT_H


#include <string>
#include <utility>
#include <random>

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

private:
  int value{0};
  std::string move_string;
};


#endif //MOVE_GENERATOR_NEGAMAX_RESULT_H
