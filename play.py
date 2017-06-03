#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Minichess AI Player"""

import argparse
import random

import zmq

from minichess import log
from minichess.parse import parse_board
from minichess.server import Server
from minichess.errors import GameOver, InvalidMove, ServerError, PlayerError

__author__ = "Michael Lane"
__email__ = "mikelane@gmail.com"
__copyright__ = "Copyright 2017, Michael Lane"
__license__ = "MIT"

args = None


class Game:
    player_type_dict = {
        'tester': '0',
        'random': '1',
        'negamax': '2',
        'ab': '3',
        'abid': '4',
        'abidttable': '5'
    }

    def __init__(self, args=None, player=None, local=False, automatic=False, opponent=None, auto_offer=False, name=None,
                 password=None):
        """
        Construct a player. Prefer constructor parameters over command line arguments. The parameters
        are the same as what is defined in the parser above.

        Parameters
        ----------
        player
        automatic
        opponent
        auto_offer
        name
        password
        """
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REP)
        self.socket.bind('tcp://*:5555')
        self.local = local
        if args:
            self.player = self.player_type_dict[args.player]
            self.automatic = args.automatic
            self.opponent = args.opponent
            self.auto_offer = args.auto_offer
            self.name = args.name
            self.password = args.password
            self.logger_level = 5 if args.debug else args.verbosity
        else:
            self.player = player
            self.automatic = automatic
            self.opponent = opponent
            self.auto_offer = auto_offer
            self.name = name
            self.password = password
            self.logger_level = 5
        self.logger = log.setup_custom_logger('root', level=self.logger_level)
        self.handshake()

    def __enter__(self):
        if not self.player == '0':
            print('\n====== GAME BEGIN =====\n')
            self.imcs_server = Server(self.logger).__enter__()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if not self.player == '0':
            self.imcs_server.__exit__(None, None, None)
            print('\n====== GAME OVER =====\n')

    def handshake(self):
        """
        Handshake with backend move generator.

        Returns
        -------
        None
        """
        message = str(self.socket.recv().decode())
        self.logger.debug('Expecting READY, received: {}'.format(message.decode()))
        self.socket.send(self.player.encode())
        message = str(self.socket.recv().decode())
        self.logger.debug('Expecting {}, received: {}'.format(self.player, message.decode()))

    def play(self):
        if self.auto_offer:
            self.play_auto_offer_game()
        elif self.automatic:
            self.play_automatic_game(opponent=self.opponent)
        else:
            self.play_interactive_game()

    def game_loop(self, board, time_left):
        """
        Runs the game loop for a maximum of 40 moves

        Parameters
        ----------
        board: The board string that you initially get from the server.
        time_left: Should be 300000 milliseconds.

        Returns
        -------

        """
        # Game loop
        parsed_board = parse_board(board, time_left)
        assert time_left == 300000

        for moves_remaining in range(40, 0, -1):
            # Parse the board and send it to the move generator
            parsed_board = parse_board(board, time_left)
            self.logger.debug('Sending this to the move generator: \n{}'.format(parsed_board))
            self.socket.send(parsed_board.encode())
            move = str(self.socket.recv().decode())
            self.logger.debug('Move generator returned this move: {}'.format(move))

            # Send the move along and get the next board
            try:
                opponent_move, color, move_number, board, time_left = self.imcs_server.send_move(move)
                print(board)
                print('Time remaining: {} ms'.format(time_left))
            except (GameOver, InvalidMove, ServerError) as e:
                self.logger.error(e)
                print(e)
                return

    def get_all_move_strings(self, board):
        """
        With player type '0', pass a parsed board to the backend to get
        all the move strings. The player type must be '0'.

        Parameters
        ----------
        board: A board string like the IMCS would return to you.

        Returns
        -------
        A string of all the available moves.

        Raises
        ------
        AssertionError
        """
        assert self.player == '0'
        parsed_board = parse_board(board, 300000)
        self.socket.send(parsed_board.encode())
        result = str(self.socket.recv().decode()).strip().split('\n')
        return set(result)

    def auto_choose_game(self, opponent=None):
        """
        Chooses a game from the available games. If you pass in an opponent or a list of opponents,
        this chooses a game against that opponent or one of those opponents at random.

        Parameters
        ----------
        opponent: A list of opponent names to play against.

        Returns
        -------
        Tuple[str, str, str]: game_number, opponent_name, my_color

        """
        game_found = False
        chosen_game = None

        while not game_found:
            available_games_str = self.imcs_server.list_games()
            available_games = [game.strip().split() for game in available_games_str]
            indexes_of_desired_opponents = []
            indexes_of_offers = []

            self.logger.debug('\nThe following games are available:')
            for i, (game_number, opponent_name, opponent_color, time1, time2, rating, offer_type) in enumerate(
                    available_games):
                self.logger.debug(
                    ' {}. {} {} {} {} {} {} {}'.format(i, game_number, opponent_name, opponent_color, time1, time2,
                                                       rating, offer_type))
                if opponent:
                    if opponent_name in opponent and offer_type == '[offer]':
                        indexes_of_desired_opponents.append(i)
                else:
                    if offer_type == '[offer]':
                        indexes_of_offers.append(i)

            if opponent:
                if indexes_of_desired_opponents:
                    chosen_game = available_games[random.choice(indexes_of_desired_opponents)]
                    game_found = True
            else:
                if indexes_of_offers:
                    chosen_game = available_games[random.choice(indexes_of_offers)]
                    game_found = True

        game_number = chosen_game[0]
        opponent_name = chosen_game[1]
        opponent_color = chosen_game[2]
        my_color = 'W' if opponent_color == 'B' else 'B'
        return game_number, opponent_name, my_color

    def play_auto_offer_game(self, color='?', duration=None):
        opponent_move, color, move_number, board, time_left = self.imcs_server.offer_game(color, duration)

        print('Received this board:\n{}'.format(board))
        print('Time Left: {} ms'.format(time_left))

        self.game_loop(board, time_left)

    def play_automatic_game(self, opponent=None):
        """
        Play an automatic accepting game

        Parameters
        ----------
        opponent: A list of opponent name strings. If none of the opponent names are available,
                  the player will keep refreshing the list until the opponent is found. So it's
                  best to use this only when you know a given opponent will be available.

        Returns
        -------
        None

        """
        game_number, opponent_name, self.my_color = self.auto_choose_game(opponent)
        print('Playing Game {} against {} as color {}'.format(game_number, opponent_name, self.my_color))
        print('Game log: http://imcs.svcs.cs.pdx.edu/minichess/logs/{}'.format(game_number))

        opponent_move, color, move_number, board, time_left = self.imcs_server.accept_game(game_number)
        assert color == self.my_color
        assert move_number == 1
        assert time_left == 300000

        print('Received this board:\n{}'.format(board))
        print('Time Left: {} ms'.format(time_left))

        self.game_loop(board, time_left)

    def user_choose_game(self):
        """
        Output an interactive list of available games and let the user choose which game to play

        Returns
        -------
        Tuple[str, str, str]: A tuple with game number, the opponent's name, and my_color
        """
        available_games = self.imcs_server.list_games()
        game_found = False
        chosen_game = None

        while available_games and not game_found:
            print('\nThe following games are available:')
            for i, game in enumerate(available_games, start=1):
                print(' {}. {}'.format(i, game))

            chosen_game = int(input("Pick a game: "))
            chosen_game -= 1

            if 0 <= chosen_game < len(available_games):
                print('Valid choice, continuing')
                game_found = True
            else:
                game_found = False
                print('That input did not make sense. Try again')

        game_number, opponent_name, opponent_color, time1, time2, rating, offer_type = available_games[
            chosen_game].strip().split()
        my_color = 'W' if opponent_color == 'B' else 'B'
        return game_number, opponent_name, my_color

    def play_interactive_game(self):
        """
        Interactive games are accepting games only.

        Returns
        -------
        """
        game_number, opponent_name, self.my_color = self.user_choose_game()
        print('Playing Game {} against {} as color {}'.format(game_number, opponent_name, self.my_color))
        print('Game log: http://imcs.svcs.cs.pdx.edu/minichess/logs/{}'.format(game_number))

        # Accept the game and block until the game board arrives
        opponent_move, color, move_number, board, time_left = self.imcs_server.accept_game(game_number)
        assert move_number == 1
        assert self.my_color == color
        assert time_left == 300000

        print('Received this board:\n{}'.format(board))
        print('Time remaining: {} ms'.format(time_left))

        self.game_loop(board, time_left)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Minichess player:')
    parser.add_argument('--debug', help='Turn on debugging mode', action='store_true')
    parser.add_argument('-v', '--verbosity', action='count', default=5,
                        help='Increase output verbosity, blank shows DEBUG only, '
                             '-v shows ERROR and CRITICAL. -vv shows WARNING, ERROR, and CRITICAL. -vvv shows INFO, WARNING, '
                             'ERROR, and CRITICAL. -vvvv shows all levels including DEBUG.')
    parser.add_argument('-p', '--player', default='random',
                        choices=['tester', 'random', 'negamax', 'ab', 'abid', 'abidttable'],
                        help='Specify the type of player to use. Does what\'s on the tin. The tester doesn\'t connect to '
                             'IMCS; instead, it causes the move generator to return all possible moves of a given board.')
    parser.add_argument('-a', '--automatic', action='store_true',
                        help='Play auto-accept games. Without this option, you will choose games from a menu.')
    parser.add_argument('-o', '--opponent', nargs='+',
                        help='A (space separated) list of player names to play against when playing as automatic player. '
                             'Without this set, the player will pick games at random.')
    parser.add_argument('-ao', '--auto-offer', action='store_true',
                        help='This player connects and offers games automatically.')
    parser.add_argument('-n', '--name', nargs=1,
                        help='The username to use for this player. Set this if you want to override the settings.ini')
    parser.add_argument('-pw', '--password', nargs=1,
                        help='The password to use with the overridden username. Required if --name option is set.')
    args = parser.parse_args()

    if args.name and not args.password:
        raise PlayerError('If you pass a player name, you must also pass a password')

    with Game(args) as game:
        game.play()
