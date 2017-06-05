#include <iostream>
#include <zmq.hpp>
#include <sstream>
#include "Player.h"
#include "Random_Player.h"
#include "Testing_Player.h"
#include "Negamax_Player.h"
#include "AB_Player.h"

int main() {
  int exit_code = 0;
  std::cerr << "TEST" << std::endl;
  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REQ);
//  Zobrist_Table zhasher;
//  ttable TTable;

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
  // 4 - aB pruning with iterative deepening
  // 4 - aB pruning with iterative deepending and TTables
  zmq::message_t response;
  socket.recv(&response);
  std::string player_type = std::string(static_cast<char *>(response.data()), response.size());
  std::cerr << "Received: " << player_type << std::endl;
  if (player_type == "DIE") {
    std::cerr << "Killed by Server" << std::endl;
    return 1;
  }

  Player * player;

  if (player_type == "0") {
    player = new Testing_Player();
    std::cerr << "Created a new Testing_Player" << std::endl;
  } else if (player_type == "1") {
    player = new Random_Player();
    std::cerr << "Created a new Random_Player" << std::endl;
  } else if(player_type == "2") {
    player = new Negamax_Player();
    std::cerr << "Created a new Negamax_Player" << std::endl;
  } else if (player_type == "3") {
    player = new AB_Player();
    std::cerr << "Created a new AB_Player" << std::endl;
  } else {
    std::cerr << "Player type not recognized or not implemented! Quitting." << std::endl;
    return 1;
  }
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

    std::string move_string{player->get_move_string(game_state_str)};
    zmq::message_t move_message(move_string.size());
    memcpy(move_message.data(), move_string.c_str(), move_string.size());
    std::cerr << "Responding with: " << move_string << std::endl;
    socket.send(move_message);

//    unsigned long long int root_hash = zhasher.hash_state(parsed_state);
//    std::cerr << "Root state hash: " << root_hash << std::endl;
//
//     This will (and should) get a blank and invalid Ttable_entry.
//    Ttable_Entry t = TTable.get_entry(root_hash);
//
//    int alpha = std::numeric_limits<int>::min();
//    int beta = std::numeric_limits<int>::max();
//
//    Ordered_States_t children = get_ordered_children(parsed_state, root_hash, zhasher, TTable);
//    int best_value = alpha;
//    state_value best;
//
//    int counter = 0;
//    int cache_hits = 0;
//    int depth = 5;
//    std::cerr << "AB Search with depth " << depth << std::endl;
//
//    while(!children.empty()) {
//      state_value next_child = children.top();
//      std::cerr << children.size() << " children remaining" <<  std::endl;
//      children.pop();
//      if(!next_child.attack && next_child.move_string == move_hist[0]) {
//        continue;
//      }
//      int next_child_value = -alpha_Beta(next_child, depth, alpha, beta, ++counter, cache_hits, zhasher, TTable);
//      if(next_child_value > best_value) {
//        best_value = next_child_value;
//        best = next_child;
//      }
//      alpha = std::max(next_child_value, alpha); // (next_child_value > alpha) ? next_child_value : alpha;
//      std::cerr << "NODE COUNT: " << counter << std::endl;
//      std::cerr << "CACHE HITS: " << cache_hits << std::endl;
//      counter = 0;
//    }
//
    // Transposition Table Store
//    t.setValid(true);
//    t.setHash(root_hash);
//    t.setValue(best_value);
//    if (best_value <= alpha) {
//      t.setFlag(ttable_flag::UPPER_BOUND);
//    } else if (best_value >= beta) {
//      t.setFlag(ttable_flag::LOWER_BOUND);
//    } else {
//      t.setFlag(ttable_flag::EXACT_VALUE);
//    }
//    t.setDepth(depth);
//    TTable.insert(t);
//
//    std::stringstream result;
//
//    if(best.move_string == "") {
//      result.str("LOSS!");
//      std::cerr << "Closing the socket" << std::endl;
//      socket.close();
//      return 1;
//    }

//    result.str(best.move_string);
//
//    move_hist[0] = move_hist[1];
//    move_hist[1] = best.attack ? "" : best.move_string;
//
//    zmq::message_t move_message((void *) result.str().c_str(), result.str().size(), NULL);
//    std::cerr << "Sending move: " << (const char *) move_message.data() << std::endl;
//    socket.send(move_message);
  }

  delete player;
  return exit_code;
}
