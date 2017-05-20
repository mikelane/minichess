#include <iostream>
#include <zmq.hpp>
#include <sstream>
#include "move_generator.h"

int main() {
//  Ordered_States_t testing = get_ordered_children(parse_input("536870912 268435456 134217728 67108864 33554432 16777216 4194304 2097152 32768 0 131072 16384 256 64 32 16 8 4 8192 1 2 1 1063288832 10296962 268327"));
//  while(!testing.empty()) {
//    state_value child_state = testing.top();
//    testing.pop();
//    std::cout << child_state.value << std::endl;
//  }

  std::cout << "TESTING AB" << std::endl;

  State_t parsed_state = parse_input("536870912 268435456 134217728 67108864 33554432 16777216 4194304 2097152 32768 0 131072 16384 256 64 32 16 8 4 8192 1 2 1 1063288832 10296962 268327");
  state_value root_state = {
      evaluate_state(parsed_state),
      "",
      parsed_state
  };

  state_value result;

  double val = alpha_Beta(root_state, 6, -50600.0, 50600.0, result);
  std::cerr << "alpha beta result: " << val << std::endl;

  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REQ);

  std::cerr << "Connecting to server ... " << std::endl;
  socket.connect("tcp://localhost:5555");

  // Handshake
  // First send ready message
  { // Force request to go out of scope after it is sent.
    zmq::message_t request(5);
    memcpy(request.data(), "READY", 5);
    std::cerr << "Sending READY..." << std::endl;
    socket.send(request);
  }

  // The response should be the type of player
  // 1 - Random
  // 2 - Negamax
  // 3 - aB pruning
  // 4 - aB pruning with TTables
  zmq::message_t response;
  socket.recv(&response);
  std::string player_type = std::string(static_cast<char *>(response.data()), response.size());
  std::cerr << "Received: " << player_type << std::endl;
  if (player_type == "DIE") {
    std::cerr << "Killed by Server" << std::endl;
    exit(1);
  } else {
    std::cerr << "Acknowledging player type " << std::endl;
    socket.send(response);
  }

  // Try to get the game state vector
  zmq::message_t game_state;
  socket.recv(&game_state);
  std::string game_state_str = std::string(static_cast<char *>(game_state.data()), game_state.size());
  std::cerr << "Received: " << game_state_str << std::endl;

  // Parse the state vector
  State_t game_state_vector = parse_input(game_state_str);

//  // Extract the relevant game state values
//  unsigned int time_left = game_state_vector.back();
//  game_state_vector.pop_back();
//  unsigned int empty_cells = game_state_vector.back();
//  game_state_vector.pop_back();
//  unsigned int opponent_locs = game_state_vector.back();
//  game_state_vector.pop_back();
//  unsigned int player_on_move = game_state_vector.back();
//  game_state_vector.pop_back();
//  unsigned int move_number = game_state_vector.back();
//  game_state_vector.pop_back();

  // Get the next move
//  std::string move = get_move(time_left, game_state_vector.data(), player_on_move, empty_cells, opponent_locs,
//                              move_number, player_type);
////  zmq::message_t move_message = make_message(move);
//  std::stringstream result{move};
//  zmq::message_t move_message((void *) result.str().c_str(), result.str().size(), NULL);
//  std::cerr << "Sending Move: " << (const char *) move_message.data() << std::endl;
//  socket.send(move_message);
//
//  std::cerr << "DONE" << std::endl;

  return 0;
}