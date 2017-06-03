//
// Created by Michael Lane on 5/30/17.
//

#ifndef MOVE_GENERATOR_PLAYER_H
#define MOVE_GENERATOR_PLAYER_H


#include "bitboard_tables.h"

class Player {
public:
  virtual std::string get_move_string(std::string state_string) = 0;
  virtual ~Player(){};

protected:
  void parse_input(std::string input);
  std::vector<std::string> generate_all_move_strings(State_t &state);

  int generate_shadow_mask(State_t state, int mover_pos);

  std::vector<std::string>
  generate_attack_strings(State_t state, int mover_index, int shadow_mask, std::string move_string);

  std::vector<std::string>
  generate_move_strings(State_t state, int mover_index, int shadow_mask, std::string move_string);

  State_t current_state{};
};


#endif //MOVE_GENERATOR_PLAYER_H
