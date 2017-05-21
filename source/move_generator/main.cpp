#include <iostream>
#include <zmq.hpp>
#include <sstream>
#include <algorithm>
#include <limits>
#include "move_generator.h"

//unsigned long long int zobrist_table[]
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
  if (player_type == "DIE") {
    std::cerr << "Killed by Server" << std::endl;
    exit(1);
  } else {
    std::cerr << "Acknowledging player type " << std::endl;
    socket.send(response);
  }

  std::string game_state_str = "";
  std::string move_hist[2] = {"", ""};

  // Enter the game loop
  while (game_state_str != "QUIT") {
    // Try to get the game state vector
    zmq::message_t game_state;
    socket.recv(&game_state);
    std::string game_state_str = std::string(static_cast<char *>(game_state.data()), game_state.size());
    std::cerr << "Received: " << game_state_str << std::endl;
    if (game_state_str == "QUIT") {
      std::cerr << "Quitting" << std::endl;
      return 0;
    }

    // Parse the state vector
    State_t parsed_state = parse_input(game_state_str);

    double alpha = std::numeric_limits<int>::min();
    double beta = std::numeric_limits<int>::max();

    Ordered_States_t children = get_ordered_children(parsed_state);
    double best_value = alpha;
    state_value best;

    int counter = 0;
    int depth = 6;
    std::cerr << "AB Search with depth " << depth << std::endl;

    while(!children.empty()) {
      state_value next_child = children.top();
      std::cerr << children.size() << " children remaining" <<  std::endl;
      children.pop();
      if(!next_child.attack && next_child.move_string == move_hist[0]) {
        continue;
      }
      double next_child_value = -alpha_Beta(next_child, depth, alpha, beta, ++counter);
      if(next_child_value > best_value) {
        best_value = next_child_value;
        best = next_child;
      }
      alpha = (next_child_value > alpha) ? next_child_value : alpha;
      std::cerr << "NODE COUNT: " << counter << std::endl;
      counter = 0;
    }

    std::stringstream result;

    if(best.move_string == "") {
      result.str("LOSS!");
    } else {
      result.str(best.move_string);
    }

    move_hist[0] = move_hist[1];
    move_hist[1] = best.attack ? "" : best.move_string;

    zmq::message_t move_message((void *) result.str().c_str(), result.str().size(), NULL);
    std::cerr << "Sending move: " << (const char *) move_message.data() << std::endl;
    socket.send(move_message);
  }

  std::cerr << "Closing the socket" << std::endl;
  socket.close();

  return 0;
}