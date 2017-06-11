/**
 * The main function for the back end.
 * Mike Lane
 *
 */

#include <iostream>
#include <zmq.hpp>
#include <sstream>
#include "Player.h"
#include "Random_Player.h"
#include "Testing_Player.h"
#include "Negamax_Player.h"
#include "AB_Player.h"
#include "AB_ID_Player.h"
#include "AB_ID_TT_Player.h"

int main() {
  int exit_code = 0;

  // zeromq boilerplate.
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

  /**
   * The response should be the type of player:
   * 1: Random
   * 2: Negamax
   * 3: Negamax with alpha beta pruning
   * 4: Negamax with alpha beta pruning and iterative deepening
   * 5: Negamax with alpha beta pruning, iterative deepening, and transposition tables.
   */
  zmq::message_t response;
  socket.recv(&response);
  std::string player_type = std::string(static_cast<char *>(response.data()), response.size());
  std::cerr << "Received: " << player_type << std::endl;
  if (player_type == "DIE") {
    std::cerr << "Killed by Server" << std::endl;
    return 1;
  }

  Player *player;

  // Use dynamic binding to create the appropriate player.
  if (player_type == "0") {
    player = new Testing_Player();
    std::cerr << "Created a new Testing_Player" << std::endl;
  } else if (player_type == "1") {
    player = new Random_Player();
    std::cerr << "Created a new Random_Player" << std::endl;
  } else if (player_type == "2") {
    player = new Negamax_Player();
    std::cerr << "Created a new Negamax_Player" << std::endl;
  } else if (player_type == "3") {
    player = new AB_Player();
    std::cerr << "Created a new AB_Player" << std::endl;
  } else if (player_type == "4") {
    player = new AB_ID_Player();
    std::cerr << "Created a new AB_ID_Player" << std::endl;
  } else if (player_type == "5") {
    player = new AB_ID_TT_Player();
    std::cerr << "Created a new AB_ID_TT_Player" << std::endl;
  } else {
    std::cerr << "Player type not recognized or not implemented! Quitting." << std::endl;
    return 1;
  }

  // The last part of the handshake with the front end.
  std::cerr << "Acknowledging player type " << std::endl;
  socket.send(response);


  // Enter the game loop
  while (true) {
    // Try to get the game state vector
    zmq::message_t game_state;
    socket.recv(&game_state);
    std::string game_state_str{std::string(static_cast<char *>(game_state.data()), game_state.size())};
    std::cerr << "Received: " << game_state_str << std::endl;
    if (game_state_str == "QUIT") {
      std::cerr << "Quitting" << std::endl;
      std::cerr << "Closing the socket" << std::endl;
      socket.close();
      exit_code = 1;
      break;
    }

    // Call the appropriate function
    std::string move_string{player->get_move_string(game_state_str)};
    zmq::message_t move_message(move_string.size());
    memcpy(move_message.data(), move_string.c_str(), move_string.size());
    std::cerr << "Responding with: " << move_string << std::endl;
    socket.send(move_message);
  }

  delete player;
  return exit_code;
}
