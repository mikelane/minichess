import glob

import os

import play
from minichess import log

logger = log.setup_custom_logger('test', level=5)

context = None
socket = None
game = None

def setup():
    global game
    game = play.Game(player='0')

def teardown():
    global game
    game.socket.send('QUIT'.encode())
    game.__exit__(None, None, None)

def test_board_inputs():
    print('RUNNING!')
    path = os.getcwd() + '/genmoves-tests'
    for input_file in glob.glob('{}/*.in'.format(path)):
        with open(input_file, 'r') as f:
            yield get_all_moves, input_file, f.read()

def get_all_moves(input_filename, board_string):
    global game
    print(input_filename)
    output_filename = input_filename[:-3] + '.out'
    print(output_filename)
    print(board_string)
    with open(output_filename, 'r') as ofile:
        actual_moves = set(ofile.read().strip().split('\n'))
    print(actual_moves)
    move_strings = game.get_all_move_strings(board_string)
    print(move_strings)
    print('difference: {}'.format(move_strings - actual_moves))
    assert actual_moves == move_strings

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

# def test_stupid_move():
#     logger.debug('Path: {}'.format(os.getcwd()))
#     tests_dir = os.getcwd() + '/genmoves-tests'
#     time_left = 300000
#     filename = tests_dir + '/stupid-move.in'
#     logger.debug('opening {}'.format(filename))
#     with open(filename, 'r') as f:
#         board = f.read()
#     parsed_board = parse_board(board, time_left)
#     logger.debug('Sending this to the move generator: \n{}'.format(parsed_board))
#     socket.send(parsed_board.encode())
#     move = str(socket.recv().decode())
#     logger.debug('Move generator send this move: {}'.format(move))
#     print('-'*100)
