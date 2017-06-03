#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Minichess AI Player"""

import argparse

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
        for moves_remaining in range(40, 0, -1):
            # Parse the board and send it to the move generator
            parsed_board = parse_board(board, time_left)
            assert time_left == 300000
            self.logger.debug('Sending this to the move generator: \n{}'.format(parsed_board))
            self.socket.send(parsed_board.encode())
            move = str(self.socket.recv().decode())
            self.logger.debug('Move generator returned this move: {}'.format(move))

            # Send the move along and get the next board
            try:
                board, time_left = self.socket.send_move(move)
            except (GameOver, InvalidMove, ServerError) as e:
                self.logger.error(e)
                print(e)

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

    def play_automatic_game(self, type, opponents=None):
        pass

    def user_choose_game(self):
        """
        Output an interactive list of available games and let the user choose which game to play

        Returns
        -------
        List[str]: A list with game number and the opponent's name.
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

        return available_games[chosen_game].split()

    def play_interactive_game(self):
        """
        Interactive games are accepting games only.

        Returns
        -------
        """
        game_number, opponent_name = self.user_choose_game()
        self.logger.info('Accepting game {} against {}'.format(game_number, opponent_name))

        # Accept the game and block until the game board arrives
        color, move_number, board, time_left = self.imcs_server.accept_game(game_number)
        assert move_number == 1

        self.logger.debug('Received this board: \n{}'.format(board))
        self.logger.debug('Have this many milliseconds left: {}'.format(time_left))

        # Game loop
        # for moves_remaining in range(40, -1, -1):
        #     # Parse the board and send it to the move generator
        #     parsed_board = parse_board(board, time_left)
        #     logger.debug('Sending this to the move generator: \n{}'.format(parsed_board))
        #
        #     # TODO finish this...


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
                        help='Automatically play games. Without this option, you will choose games from a menu.')
    parser.add_argument('-o', '--opponent', nargs='+',
                        help='A (space separated) list of player names to play against when playing as automatic player.')
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
        print('END')
        # if args.type == 'automatic':
        #     play_automatic_game(args.auto_type, args.opponent)
        # else:  # interactive player
        #     play_interactive_game()

        # Get a listing of available players
        # try:
        #     with Server(logger=logger) as s:
        #         print('END')
        # except KeyboardInterrupt:
        #     logger.error('KeyboardInterrupt detected. Closing.')

        #
        #         # Begin the game loop
        #         counter = 41
        #         while counter > 0:
        #             # Parse the board and send it to the move generator
        #             parsed_board = parse_board(board, time_left)
        #             logger.debug('Sending this to the move generator: \n{}'.format(parsed_board))
        #             socket.send(parsed_board.encode())
        #             move = str(str(socket.recv().decode()))
        #             logger.debug('Move generator send this move: {}'.format(move))
        #
        #             # Send the move along and get the next board
        #             try:
        #                 board, time_left = s.send_move(move)
        #             except AssertionError as e:
        #                 logger.error('AssertionError'.format(e.message))
        #                 socket.send('QUIT')
        #                 logger.error('SHUTTING DOWN')
        #                 break
        #
        #             if board == "GAME OVER":
        #                 logger.debug('Game Over. Shutting down')
        #                 socket.send('QUIT')
        #                 break
        #             logger.debug('Received this board:\n{}'.format(board))
        #             logger.debug('Have this many milliseconds left: {}'.format(time_left))
        #             counter -= 1

        # logger.info('======== GAME OVER ========\n')
