from nose import tools
import minichess
import zmq
from minichess import log
from minichess.parse import parse_board
import os

logger = log.setup_custom_logger('test', level=5)

context = None
socket = None

def setup():
    global context
    global socket
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind('tcp://*:5555')

    # Handshake with receiver passing the type of player
    message = socket.recv().decode()
    logger.debug('Expecting READY, received: {}'.format(message.decode()))
    player_type = '3'

    socket.send(player_type)
    message = socket.recv().decode()
    logger.debug('Expecting {}, received: {}'.format(player_type, message.decode()))


def teardown():
    global socket
    socket.send("QUIT".encode())
    socket.close()


# def test_move_generator():
#     logger.debug('Path: {}'.format(os.getcwd()))
#     tests_dir = os.getcwd() + '/genmoves-tests'
#     time_left = 300000
#     for file in os.listdir(tests_dir):
#         logger.debug(file)
#         if '.in' not in file:
#             continue
#         out_file = '{}.out'.format(file[:-3])
#         with open('{}/{}'.format(tests_dir, file), 'r') as f:
#             board = f.read()
#         with open('{}/{}'.format(tests_dir, out_file), 'r') as of:
#             valid_moves = of.read().strip().split()
#         if not valid_moves:
#             valid_moves = ["LOSS!"]
#         parsed_board = parse_board(board, time_left)
#         logger.debug('Sending this to the move generator: \n{}'.format(parsed_board))
#         socket.send(parsed_board.encode())
#         move = str(socket.recv().decode())
#         logger.debug('Move generator send this move: {}'.format(move))
#         assert move in valid_moves
#         print('-'*100)

def test_stupid_move():
    logger.debug('Path: {}'.format(os.getcwd()))
    tests_dir = os.getcwd() + '/genmoves-tests'
    time_left = 300000
    filename = tests_dir + '/stupid-move.in'
    logger.debug('opening {}'.format(filename))
    with open(filename, 'r') as f:
        board = f.read()
    parsed_board = parse_board(board, time_left)
    logger.debug('Sending this to the move generator: \n{}'.format(parsed_board))
    socket.send(parsed_board.encode())
    move = str(socket.recv().decode())
    logger.debug('Move generator send this move: {}'.format(move))
    print('-'*100)
