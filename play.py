#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Short description

Long description
"""

import argparse

import zmq

from minichess import log
from minichess.parse import parse_board

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
args = parser.parse_args()

if args.debug:
    logger = log.setup_custom_logger('root', level=5)
else:
    logger = log.setup_custom_logger('root', level=args.verbosity)

if __name__ == '__main__':
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind('tcp://*:5555')

    message = socket.recv()
    logger.debug('Expecting READY, received: {}'.format(message.decode()))

    socket.send(b'1')

    message = socket.recv()
    logger.debug('Expecting 1, received: {}'.format(message.decode()))

    logger.debug('Testing parse function')
    board = '''1 W
kqbnr
ppppp
.....
.....
PPPPP
RNBQK'''
    parsed_board = parse_board(board)
    logger.debug('Sending: \n{}'.format(parsed_board))
    socket.send(parsed_board.encode())

    move = socket.recv_string()

    logger.debug('Received move {}'.format(move))
