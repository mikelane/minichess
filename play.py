#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Minichess AI Player"""

import argparse

import zmq

from minichess import log
from minichess.parse import parse_board
from minichess.server import Server

__author__ = "Michael Lane"
__email__ = "mikelane@gmail.com"
__copyright__ = "Copyright 2017, Michael Lane"
__license__ = "MIT"

parser = argparse.ArgumentParser(
    description='''Minitchess player: Please ensure that you are using a python3 interpreter.''')
parser.add_argument('--debug', help='Turn on debugging mode', action='store_true')
parser.add_argument('-v', '--verbosity', action='count', default=0,
                    help='Increase output verbosity, blank shows ERROR only, '
                         '-v shows INFO and ERROR, -vv shows WARNING, INFO, and ERROR, and '
                         '-vvv shows all logs')
parser.add_argument('-p', '--player', default='random', choices=['random', 'negamax', 'ab', 'abttable'],
                    help='Specify the type of player to use')
parser.add_argument('-i', '--interactive', action='store_true',
                    help='Make the player interactive so you can choose what games it plays')
args = parser.parse_args()

if args.debug:
    logger = log.setup_custom_logger('root', level=5)
else:
    logger = log.setup_custom_logger('root', level=args.verbosity)

if __name__ == '__main__':
    # Set up the IPC
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind('tcp://*:5555')

    # Handshake with receiver passing the type of player
    message = socket.recv()
    logger.debug('Expecting READY, received: {}'.format(message.decode()))

    if args.player == 'random':
        player_type = '1'
    elif args.player == 'negamax':
        player_type = '2'
    elif args.player == 'ab':
        player_type = '3'
    elif args.player == 'abttable':
        player_type = '4'
    else:
        socket.send(b'DIE')
        raise ValueError('Invalid player type selected')

    socket.send(player_type.encode())
    message = socket.recv()
    logger.debug('Expecting {}, received: {}'.format(player_type, message.decode()))

    # Get a listing of available players
    with Server() as s:
        available_games = s.list_games()
        game_found = False

        while available_games and not game_found:
            print('\nThe following games are available:')
            for i, game in enumerate(available_games, start=1):
                print(' {}. {}'.format(i, game))

            chosen_game = int(input("Pick a game: "))
            chosen_game -= 1

            if 0 <= chosen_game < len(available_games):
                logger.debug('Valid choice, continuing')
                game_found = True
            else:
                game_found = False
                logger.info('That input did not make sense. Try again')

            game = available_games[chosen_game].split()
            logger.info('Accepting game {} against {}'.format(game[0], game[1]))

            # Accept the game and block until the game board arrives
            board, time_left = s.accept_game(game[0])
            logger.debug('Received this board: \n{}'.format(board))
            logger.debug('Have this many milliseconds left: {}'.format(time_left))

            # Parse the board and send it to the move generator
            parsed_board = parse_board(board, time_left)
            logger.debug('Sending this to the move generator: \n{}'.format(parsed_board))
            socket.send(parsed_board)
            move = socket.recv()
            logger.debug('Move generator send this move: {}'.format(move))

            # Send the move along and get the next board
            board, time = s.send_move(move)
            logger.debug('Received this board:\n{}'.format(board))
            logger.debug('Have this many milliseconds left: {}'.format(time_left))

            # Send resign
            s.send_resign()

