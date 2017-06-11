# Minichess - Final Report #

Michael Lane  
CS542  
Spring 2017  

### Program Description ###

My minichess AI is composed of a Python front end and a C++ back end. The Python front
end handles the connection to the IMCS server, parsing the incoming board text,
communicating the relevant information to the back end, and sending the generated move
string back to the server. The C++ back end handles the entire process of generating
the move string as the various types of players: a testing player (used to validate the
move generator), a random player, a negamax player, an alpha beta pruned negamax player,
an alpha beta negamax player with iterative deepening, and an alpha beta negamax player
with iterative deepening and transposition tables.

### Program Features ###

My program has two novel features. The first is the Python front end with a C++ back
end. There is a simple bash script that starts the C++ move generator as a background
process and then starts the Python player. The Python script (which is fully compatible
with Python 2 and 3, and therefore pypy) communicates with the back end via a zeromq
socket. IPC has a lot of overhead; however, the communication between front and back
end is limited to state strings to the back end and move strings to the front end.
There are no more than 40 of these calls in a game, so the time spent on IPC is trivial.

The second novel feature is the (extensive) use of bitboards. The bitboard tables header
file (`minichess/source/move_generator/bitboard_tables.h`) is nearly 3500 lines all on its
own. Briefly, the way my bitboards work is that for a given piece type (which has a
unique index into the game state vector) if it hasn't been removed from the board (and
therefore has a value greater than 0) that piece can look up its location as a key in a
`std::unordered_map`. The values at the key will correspond to all the locations that
the piece of this type can move from its location on the board. All that is required to
generate legal attacks is to know the location of all the opponents and the shadows cast
by any piece that lies on one of the 8 possible move spokes. Something similar is done
for legal moves, except legal moves always end up at an empty cell.

I use bitboards for calculating the shadows and the move heuristic values. A basic
negamax search will complete easily at depth 4 and will occasionally time out at depth 5. 
With alpha beta pruning, I can search to depth 8. With iterative deepening, some of
the mid-game searches can top out at depth 10 or 12.

### Planned Features ###

I really wanted to get to the point of training the board evaluator using Reinforcement
Learning (a la TDGammon). Sadly, a slew of last minute bugs that required a full rewrite
prevented me from doing this. Obviously, my player would be much stronger with a better
move evaluator, so this task is worthwhile in my spare time this summer. 

Once I have a satisfactory state evaluator, I'd also really like to implement another
Reinforcement Learning algorithm in place of the good old fashioned AI player I have
now. Something like a Deep Q Network (DQN), a double-DQN (similar to what was used for
Alpha Go), an Asyncronous Advantage Actor Critic (A3C), or an Evolution Strategies
learner. The main reason I implemented the front end in Python was to take advantage of
libraries to assist with this process. I believe that with one of these strategies, I
could build the world's strongest minichess player.

### Performance ###

My current player's performance is reasonably good. It's not earth shattering, but it is
sufficient for me to be proud of. My average performance in a game, depth-wise, is
between 6 and 8 and my player will touch 30,000 to 300,000 nodes, depending on the
state of the game. 

I struggled with bugs and that required a rewrite. My testing strategy during the
rewrite involved the use of nosetests from the Python side. I found it very convenient
to be able to set up and tear down the connection between the front and the back end
automatically. Using this testing method, during the rewrite, I discovered that my move 
generator was not generating all the moves, which was due to an issue with how I was 
handling promoted pawns. I found other bugs relating to promoted pawns. For instance, I 
found that my shadow masks were completely missing promoted pawns and were causing me 
to send illegal moves. Another issue was that I was not masking the promoted pawn bit 
when I captured pieces during the `make_move()` function. This was causing my back end 
to crash while the front end was blocked waiting for its response.

Overall, I think the move heuristic could use tweaking and I'd like to find a way to
keep the stupid knight from jumping out immediately. But other than that, my player is
pretty solid.
