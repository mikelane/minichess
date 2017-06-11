//
// Created by Michael Lane on 6/10/17.
//

#ifndef MOVE_GENERATOR_AB_ID_PLAYER_H
#define MOVE_GENERATOR_AB_ID_PLAYER_H


#include "Player.h"
#include "Negamax_Result.h"

class AB_ID_Player : public Player {
public:
  std::string get_move_string(const std::string &state_string) override;

private:
  long long timecounter{0ll};
  long long timecache{0ll};
  long long timelimit{0ll};

  Negamax_Result negamax(const State_t &state, int depth, int alpha, int beta, int &node_count);

  void set_time_limit(const State_t &state);

  long long get_millisecond_time();

};


#endif //MOVE_GENERATOR_AB_ID_PLAYER_H
