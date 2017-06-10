//
// Created by Michael Lane on 5/30/17.
//

#ifndef MOVE_GENERATOR_PLAYER_H
#define MOVE_GENERATOR_PLAYER_H

#include <random>
#include "Move.h"
#include "bitboard_tables.h"

class Player {
public:
  virtual std::string get_move_string(const std::string &state_string) = 0;

  State_t make_move(const State_t &state, const Move &move);

  virtual ~Player() = default;

protected:
  State_t parse_input(const std::string &input);

  std::vector<std::string> generate_all_move_strings(const State_t &state);

  std::vector<Move> generate_all_moves(const State_t &state);

  int eval(const State_t &state);

  int generate_shadow_mask(const State_t &state, int mover_pos);

  std::vector<std::string>
  generate_attack_strings(const State_t &state, int mover_index, int shadow_mask, const std::string &move_string);

  std::vector<std::string>
  generate_move_strings(const State_t &state, int mover_index, int shadow_mask, const std::string &move_string);

  std::vector<Move>
  generate_attacks(const State_t &state, int mover_index, int shadow_mask, const std::string &move_string);

  std::vector<Move>
  generate_moves(const State_t &state, int mover_index, int shadow_mask, const std::string &move_string);

  int calculate_material_value(const State_t &state);

  State_t make_attack(const State_t &state, int attacker_index, int target_index);

  int calculate_heuristic_value(const State_t &state);

  State_t make_move(const State_t &state, int mover_index, int dest_pos);

  int calculate_number_of_attacks(const State_t &state, int idx, bool opponent);

  int calculate_number_of_moves(const State_t &state, int idx);

  State_t current_state{};
  int number_of_nodes = 0;
  std::random_device rd;
  std::mt19937 mt{rd()};
};


#endif //MOVE_GENERATOR_PLAYER_H
