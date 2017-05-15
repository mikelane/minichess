#include <iostream>
#include "move_generator.h"

int main() {
  player me = BLACK;
  player opponent = WHITE;
  std::vector<unsigned int> state = {A6, B6, C6, D6, E6, A5, B5, C5, D5, E5, A2, B2, C2, D2, E2, A1, B1, C1, D1, E1, 1,
                                     1, 1023, 1047552};
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