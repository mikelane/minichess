//
// Created by Michael Lane on 5/13/17.
//

#ifndef MOVE_GENERATOR_MOVE_GENERATOR_H
#define MOVE_GENERATOR_MOVE_GENERATOR_H

#include "bitboard_tables.h"
#include "Zobrist_Table.h"
#include "ttable.h"
#include <vector>
#include <tuple>
#include <unordered_map>
#include <string>
#include <zmq.hpp>
#include <map>
#include <queue>

bool black_has_been_promoted(int type, unsigned int location);

bool white_has_been_promoted(int type, unsigned int location);

int material_evaluation(State_t state);

State_t parse_input(std::string);

Ordered_States_t get_ordered_children(State_t state, unsigned long long int hash, Zobrist_Table &zhasher);

std::vector<State_t> get_children(State_t state);

State_t make_attack(const State_t &state, int attacker_index, int target_index);

State_t make_move(const State_t &state, int mover_index, int dest_pos);

int negamax(State_t state, int depth, int alpha, int beta, Zobrist_Table & zhasher);

int alpha_Beta(state_value &state, int depth, int alpha, int beta, int &node_count, int & cache_hits, Zobrist_Table &zhasher, ttable & TTable);

std::string
get_move(unsigned int time_left, unsigned int *piece_locations, unsigned int on_move, unsigned int empty_locs,
         unsigned int opponent_locs, unsigned int player_type, std::string string);

zmq::message_t make_message(std::string msg);

#endif  //MOVE_GENERATOR_MOVE_GENERATOR_H
