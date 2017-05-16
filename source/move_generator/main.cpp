#include <iostream>
#include "move_generator.h"

int main() {
  player me = BLACK;
  player opponent = WHITE;
  std::vector<unsigned int> state = {A5, 0, 0, C3, E4, E3, 0, 0, 0, 0, A4, E2, 0, 0, 0, C1, 0, 0, 0, E1,
                                     18, 1, 524325, 1056402394};
  unsigned int empty_cells = state.back();
  state.pop_back();
  unsigned int opponent_locs = state.back();
  state.pop_back();
  unsigned int player_on_move = state.back();
  state.pop_back();
  unsigned int move_number = state.back();
  state.pop_back();
  std::cout << "Testing ... " << std::endl;
  std::cout << "ME: " << std::endl
            << get_move(state.data(), me, empty_cells, opponent_locs) << std::endl;
  return 0;
}