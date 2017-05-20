#include <iostream>
#include <zmq.hpp>
#include <sstream>
#include "move_generator.h"

int main() {
//  std::cout << "TESTING AB" << std::endl;
//
//  State_t parsed_state = parse_input("536870912 268435456 134217728 67108864 33554432 16777216 4194304 2097152 32768 0 131072 16384 256 64 32 16 8 4 8192 1 2 1 1063288832 10296962 268327");
//  state_value root_state = {
//      evaluate_state(parsed_state),
//      "",
//      parsed_state
//  };
//

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

  // Enter the game loop
  while (game_state_str != "QUIT") {
    // Parse the state vector
    State_t parsed_state = parse_input(game_state_str);

    Ordered_States_t children = get_ordered_children(parsed_state);
    state_value best = {
        -50600,
    };

    int counter = 0;

    while(!children.empty()) {
      state_value next_child = children.top();
      std::cerr << children.size() << " children remaining" <<  std::endl;
      children.pop();
      double next_child_value = alpha_Beta(next_child, 8, -50600.0, 50600.0, ++counter);
      if(next_child_value > best.value) {
        best = next_child;
      }
    }

    std::stringstream result{best.move_string};
    zmq::message_t move_message((void *) result.str().c_str(), result.str().size(), NULL);
    std::cerr << "Sending move: " << (const char *) move_message.data() << std::endl;
    socket.send(move_message);

    // Try to get the game state vector
    zmq::message_t game_state;
    socket.recv(&game_state);
    std::string game_state_str = std::string(static_cast<char *>(game_state.data()), game_state.size());
    std::cerr << "Received: " << game_state_str << std::endl;
  }

  std::cerr << "Closing the socket" << std::endl;
  socket.close();

  return 0;
}