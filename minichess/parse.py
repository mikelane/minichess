#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Minichess Board Parsing Utility"""

import numpy as np
from collections import OrderedDict

__author__ = "Michael Lane"
__email__ = "mikelane@gmail.com"
__copyright__ = "Copyright 2017, Michael Lane"
__license__ = "MIT"

# Some setup and convenience dicts
positions = [(r, c) for r in range(6) for c in range(5)]
strings = ['{}{}'.format(c, r) for r in range(6, 0, -1) for c in 'abcde']
ints = [1 << i for i in range(29, -1, -1)]

pos_to_str = {p: s for p, s in zip(positions, strings)}
# print(pos_to_str)
pos_to_int = {p: i for p, i in zip(positions, ints)}
# print(pos_to_int)
str_to_pos = {s: p for s, p in zip(strings, positions)}
# print(str_to_pos)
str_to_int = {s: i for s, i in zip(strings, ints)}
# print(str_to_int)
int_to_str = {i: s for i, s in zip(ints, strings)}
# print(int_to_str)
int_to_pos = {i: p for i, p in zip(ints, positions)}

opponent_color = {'B': 'W', 'W': 'B'}
player_number = {'W': 1, 'B': 2}
piece_types = {'B': 'kqbnrp', 'W': 'KQBNRP'}


def arr_to_int(parsed_board, piece_types):
    """
    Given a parsed board and a string of piece types, return an integer that represents
    those locations. Location a6 is bit 29, a6 is bit 28, ... , E1 is bit 1.
    
    Parameters
    ----------
    parsed_board: A 2d numpy ndarray of strings.
    piece_types:  A string of allowed piece types similar to 'kqbnrp' or 'KQBNRP' or '.'

    Returns
    -------
    An integer that corresponds to the parsed board.
    """
    result = np.zeros(parsed_board.shape, dtype=np.int)
    for piece_type in piece_types:
        result |= (parsed_board == piece_type)  # Numpy makes this task easy
    # Stringify the flattened array, index out the brackets, remove the spaces and
    # let python evaluate the binary string.
    return eval('0b{}'.format(str(result.astype(np.int).ravel())[1:-1].replace(' ', '')))


def get_opponent_locations(parsed_board, color_of_opponent):
    """
    A utility function that uses arr_to_int to return the appropriate value
    
    Parameters
    ----------
    parsed_board : The numpy ndarray of strings that represents the board.
    color_of_opponent: The color of the opponent.

    Returns
    -------
    An integer that represents the opponent locations of the parsed bitboard.

    """
    assert color_of_opponent in 'BW'
    return arr_to_int(parsed_board, piece_types[color_of_opponent])


def get_empty_locations(parsed_board):
    """
    A wrapper for arr_to_int that passes the empty cell string, '.'
    Parameters
    ----------
    parsed_board: The numpy ndarray of strings that represents the board.

    Returns
    -------
    An integer that represents the empty cells of the parsed bitboard
    """
    return arr_to_int(parsed_board, '.')


def parse_board(board):
    """
    Take a board as a string similar to this:
    
        1 B
        kqb.r
        ppq.q
        Q....
        ..N..
        .PPP.
        R.BQK
    
    and convert it into a list of 24 integers that make up a minichess bitboard.
    
    Parameters
    ----------
    board: This is the string value of the board

    Returns
    -------
    List of length 24 where the 24 elements represent the following:
    - The first 20 integers are the locations of the piece types in this order
      (where lowercase is black, uppercase is white):
        k, q, b, n, r, p, p, p, p, p, P, P, P, P, P, R, N, B, Q, K
    - The 21st integer is the move number of the game
    - The 22nd integer represents the color of the player on move, 1 for white, 2 for black
    - the 23rd integer is the location of all of the opponents of the player on move
    - the 24th integer is the location of all the empty cells
    """

    # Set up an intermediate container
    black_pieces = OrderedDict()
    for piece_type in 'kqbnrp':
        black_pieces[piece_type] = None
    white_pieces = OrderedDict()
    for piece_type in 'PRNBQK':
        white_pieces[piece_type] = None

    # Do the initial parsing of the board string
    # move_number, my_color, *board = board.split()
    split_board = board.split()
    move_number, my_color, split_board = split_board[0], split_board[1], split_board[2:]
    move_number = int(move_number)
    parsed_board = np.array([list(row) for row in split_board])  # numpy makes it all worth it.

    # Get the 23rd and 24th values of the return list
    opponent_locations = get_opponent_locations(parsed_board, opponent_color[my_color])
    empty_locations = get_empty_locations(parsed_board)

    # Start filling in the intermediate container for black
    for piece in 'kqbnrp':
        positions = np.argwhere(parsed_board == piece)  # Returns an array of 2d array locations
        if positions.size:
            black_pieces[piece] = np.apply_along_axis(lambda p: pos_to_int[tuple(p)], 1, positions)
        else:  # Need to have 0 if the piece isn't there.
            black_pieces[piece] = np.array([0])

    # Handle the case of pawns that have been promoted
    if black_pieces['q'].size > 1:
        black_pieces['q'], black_promoted_pawns = np.split(black_pieces['q'], (1,))
        # Promoted pawns live in the pawn locations in the returned list but with a flag at bit 30
        black_promoted_pawns = np.apply_along_axis(lambda i: i | (1 << 30), 0, black_promoted_pawns)
        black_pieces['p'] = np.append(black_promoted_pawns, black_pieces['p'])

    # We have to make sure that all 20 spots in the piece list are filled. Pad 0 when pawns are missing
    if black_pieces['p'].size < 5:
        black_pieces['p'] = np.pad(black_pieces['p'], (0, 5 - black_pieces['p'].size), 'constant', constant_values=0)

    # Second verse, same as the first. All the above, but for white.
    for piece in 'PRNBQK':
        positions = np.argwhere(parsed_board == piece)
        if positions.size:
            white_pieces[piece] = np.apply_along_axis(lambda p: pos_to_int[tuple(p)], 1, positions)
        else:
            white_pieces[piece] = np.array([0])

    if white_pieces['Q'].size > 1:
        white_pieces['Q'], white_promoted_pawns = np.split(white_pieces['Q'], (1,))
        white_promoted_pawns = np.apply_along_axis(lambda i: i | (1 << 30), 0, white_promoted_pawns)
        white_pieces['P'] = np.append(white_promoted_pawns, white_pieces['P'])

    if white_pieces['P'].size < 5:
        white_pieces['P'] = np.pad(white_pieces['P'], (0, 5 - white_pieces['P'].size), 'constant', constant_values=0)

    # Concatenate the lists into one long list and return it.
    return ' '.join(
        list(map(str, list(
            np.append(
                np.concatenate(tuple(black_pieces.values())),
                np.concatenate(tuple(white_pieces.values())))) + [move_number,
                                                                  player_number[my_color],
                                                                  opponent_locations,
                                                                  empty_locations])))

if __name__ == '__main__':
    board = '''1 W
kqbnr
ppppp
.....
.....
PPPPP
RNBQK'''
    parsed_board = parse_board(board)
    print(parsed_board)