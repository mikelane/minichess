#include <iostream>
#include <zmq.hpp>
#include <sstream>
#include <string>
#include "move_generator.h"

int main() {
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
  std::cerr << "Acknowledging player type " << std::endl;
  socket.send(response);

  // Try to get the game state vector
  zmq::message_t game_state;
  socket.recv(&game_state);
  std::string game_state_str = std::string(static_cast<char *>(game_state.data()), game_state.size());
  std::cerr << "Received: " << game_state_str << std::endl;

  // Parse the state vector @todo egads add times!
  std::vector<unsigned int> game_state_vector = parse_input(game_state_str);

  // Extract the relevant game state values
  unsigned int empty_cells = game_state_vector.back();
  game_state_vector.pop_back();
  unsigned int opponent_locs = game_state_vector.back();
  game_state_vector.pop_back();
  unsigned int player_on_move = game_state_vector.back();
  game_state_vector.pop_back();
  unsigned int move_number = game_state_vector.back();
  game_state_vector.pop_back();

  std::stringstream move_stream{get_move(game_state_vector.data(), player_on_move, empty_cells, opponent_locs, player_type)};
  zmq::message_t move_message((void*)move_stream.str().c_str(), move_stream.str().size()+1, NULL);

  std::cerr << "Testing ... " << std::endl;
  std::cerr << "ME: " << std::endl
            << (const char*) move_message.data() << std::endl;

  socket.send(move_message);

  std::cerr << "DONE" << std::endl;

  return 0;
}