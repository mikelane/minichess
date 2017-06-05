//
// Created by Michael Lane on 6/2/17.
//

#include "Testing_Player.h"

std::string Testing_Player::get_move_string(const std::string &state_string) {
  std::string result = "";
  parse_input(state_string);
  std::vector<std::string> move_strings = generate_all_move_strings(current_state);
  for(std::string move_string : move_strings) {
    result += move_string;
    result += "\n";
  }
  return result;
}
