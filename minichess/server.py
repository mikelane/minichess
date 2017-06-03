#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""IMCS server class

This class manages the connection to the server as well as sending
and formatting the IO. This code was inspired in part by chromakode
skirmish, https://github.com/chromakode/skirmish.
"""
from __future__ import print_function

import datetime
import os
import re
import socket
import sys

import configparser

from minichess.errors import GameOver, InvalidMove, ServerError

__author__ = "Michael Lane"
__email__ = "mikelane@gmail.com"
__copyright__ = "Copyright 2017, Michael Lane"
__license__ = "MIT"

if sys.version_info[:2] < (3, 3):
    python_version = 2
else:
    python_version = 3


class Server:
    imcs_socket = socket.socket()
    try:
        imcs_stream = imcs_socket.makefile(mode='rw', buffersize=0)
    except TypeError:
        imcs_stream = imcs_socket.makefile(mode='rw', bufsize=0)

    def __init__(self, logger, username=None, password=None):
        self.logger = logger
        config = configparser.ConfigParser()
        try:
            config.read_file(open('settings.ini'))
        except IOError as e:
            self.logger.debug(e)
            print('No settings.ini file detected. Creating it. This will be done only once.')
            self.host = input('IMCS HOST: ')
            config.set('SERVER', 'host', self.host)
            self.port = input('IMCS PORT: ')
            config.set('SERVER', 'port', self.port)
            self.username = input('Username: ')
            config.set('PLAYER', 'username', self.username)
            self.password = input('Password: ')
            config.set('PLAYER', 'password', self.password)

            project_root = os.path.dirname(sys.modules['__main__'].__file__)
            with open(os.path.join(project_root, 'settings.ini')) as f:
                config.write(f)
        else:
            self.host = config['SERVER']['host']
            self.port = int(config['SERVER']['port'])
            self.username = config['PLAYER']['username']
            self.password = config['PLAYER']['password']

        # Overwrite username and password here if they're passed in.
        self.username = username if username else self.username
        self.password = password if password else self.password

        self.color = None

    def __enter__(self):
        """
        This makes it so the user can do something like this:
        
            with Server() as s:
                s.get_help()
                
        The __enter__() function will set up the connection and capture and output the preamble
        lines. It then returns a reference to itself to the caller. The __exit__() function does
        the cleanup operations.
        :return: self
        """
        self.imcs_socket.connect((self.host, self.port))
        response = self.imcs_stream.readline()
        print(response)

        # Try to login with credentials from settings.ini
        self.logger.debug('Trying to login. Sending "me {} {}"'.format(self.username, self.password))
        self.send('me {} {}'.format(self.username, self.password))

        # Make sure login was successful. If not and you need to register, do that and try again.
        response = self.imcs_stream.readline()
        print(response)

        # Handle error condition
        if '400 no such username' in response:
            self.logger.debug('Sending register {} {}'.format(self.username, self.password))
            self.send('register {} {}'.format(self.username, self.password))
            response = self.imcs_stream.readline()
            print(response)
            if '402 username already exists' in response:
                self.logger.error('Error when trying to register with name {}'.format(self.username))
                raise ServerError(response)  # Just die

        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        This is run whenever the with block ends. This makes it simple to do cleanup
        operations. For this server, that means sending the quit signal and closing the stream.

        Parameters
        ----------
        exc_type: Exception type
        exc_val: Exception value
        exc_tb: Exception traceback

        Returns
        -------
        None
        """
        if exc_type:
            self.logger.error(exc_type, exc_val)
        print('Disconnectinng from IMCS')
        self.send('quit')
        self.imcs_stream.close()

    @classmethod
    def send(cls, command):
        """
        Send a message to the IMCS server using the imcs_stream file descriptor

        Parameters
        ----------
        command: The string command to send

        Returns
        -------
        None
        """
        try:
            if python_version == 3:
                print(command, file=cls.imcs_stream, flush=True)
            else:
                print(command, file=cls.imcs_stream)
                cls.imcs_stream.flush()
        except IOError:
            pass

    def get_game_info(self):
        """
        Called to gather the information about the current game from the IMCS info line

        Returns
        -------
        List[str]: [result_code, player_color, time_remaining, opponent_time_remaining, message]
        """
        info_line = self.imcs_stream.readline().strip()
        print(info_line)
        assert '105' in info_line or '106' in info_line
        result = info_line.split(maxsplit=4)

        # When playing as white, the next line is blank. Make sure of that.
        if '105' in info_line:
            blank = self.imcs_stream.readline()
            self.logger.debug('Expecting blank line. Received: "{}".'.format(blank))
            print(blank)
            self.detect_end_game_states(blank)

        return result

    def detect_end_game_states(self, message):
        """
        Use this function whenever an ending game state might be reached. If no
        exceptions are thrown, then no end game state or error state has been
        reached.

        Parameters
        ----------
        message: A string response from the server.

        Returns
        -------
        None

        Raises
        ------
        server.InvalidMove
        server.GameOver
        SystemError
        """
        if message == '\r\n':
            return

        if 'illegal move' in message:  # Server returned an illegal move warning.
            self.logger.debug('Last move sent was an illegal move!')
            raise InvalidMove(message)
        elif '=' in message:  # Server returned a final game message
            if '{} wins'.format(self.color) in message:
                game_result = 'YOU WON!'
            elif 'draw' in message:
                game_result = 'Game ended in a draw'
            else:
                game_result = 'You lost!'
            self.logger.debug('Raising GameOver exception: {}'.format(game_result))
            raise GameOver(game_result)

    def get_opponent_move(self):
        """
        It is at the state of getting the opponent move that one of 4 results could be returned

        Returns
        -------
        str version of opponent move. e.g. c4-c3

        Raises
        ------
        server.GameOver
        server.InvalidMove
        SystemError
        """
        self.logger.debug('Expecting opponent move string. Could get illegal move or game result.')

        # This will block waiting for the opponent.
        result = self.imcs_stream.readline().strip()
        print(result)

        # Throws exceptions if end game states are reached
        self.detect_end_game_states(result)

        # No end game state, return opponent's move
        opponent_move = result.split()[1]
        next_line = self.imcs_stream.readline()  # This is usually blank. Will start with '=' if a Game Over state.

        # Throws exceptions if end game states are reached
        self.detect_end_game_states(next_line)

        return opponent_move

    def get_help(self):
        """
        Send the command 'help' to the server and log the response
        :return: None
        """
        self.send('help')
        data = self.imcs_stream.readline().rstrip()
        while data:
            self.logger.info(data)
            data = self.imcs_stream.readline().rstrip()
            if data == '.':
                self.logger.info(data)
                return

    def list_games(self):
        """
        Send the command 'list' to the server and log the results.

        Returns
        -------
        List of strings of results
        """
        results = []
        self.logger.debug('Requesting available games from the server.')
        self.send('list')
        data = self.imcs_stream.readline().rstrip()
        print(data)
        while data:
            data = self.imcs_stream.readline().rstrip()
            print(data)
            if '[offer]' in data:
                results.append(data)
            elif data == '.':
                print(data)
                return results

    def read_board(self):
        """
        Use this function to read the input when a board state is expected.

        Returns
        -------
        List[str, int, str, float]: List of your color, the move number,
                                    the board string, and time left in milliseconds
        """
        self.logger.debug('Attempting to read the board')
        board = ''
        timer_string = ''
        keep_reading = True
        while keep_reading:
            line = self.imcs_stream.readline()
            self.detect_end_game_states(line)
            if line[0] == '?':
                timer_string = line.strip()
                keep_reading = False
            else:
                board += line

        move_number, _ = board.split(' ')
        move_number = int(move_number)

        # Parse the timer and get my time left in milliseconds
        my_time = re.split('\D', timer_string.split()[1])
        time_left = datetime.timedelta(minutes=float(my_time[0]), seconds=float(my_time[1]),
                                       milliseconds=float(my_time[2]))

        return [self.color, move_number, board, time_left.total_seconds() * 1000]

    def accept_game(self, game_number, color=None):
        """
        Accept the game with the passed in game number
        
        Parameters
        ----------
        game_number The number of the game to accept

        Returns
        -------
        List of the opponent's move, your player's color, Board string and float
        number of milliseconds left on the timer

        """
        if color:
            self.send('accept {} {}'.format(game_number, color))
        else:
            self.send('accept {}'.format(game_number))

        result_code, self.color, time_remaining, opponent_time_remaining, message = self.get_game_info()

        opponent_move = None

        if result_code == '105':
            self.logger.info('Playing as White')
        else:
            self.logger.info('Playing as Black')
            opponent_move = self.get_opponent_move()  # Blocking. Also gets empty line.

        # The next expected input is the board.
        return [opponent_move] + self.read_board()

    def offer_game(self, color='?', duration=None):
        """
        Offer a game on the IMCS Server.

        Parameters
        ----------
        color: The desired color. Can be 'B', 'W', or '?'
        duration: Amount of total time per side in seconds. Use only if you choose to send some
                  duration other than the standard 5 minutes.

        Returns
        -------
        None
        """
        if duration:
            self.logger.info('Offering color {} and duration {}'.format(color, duration))
            self.send('offer {} {}'.format(color, duration))
        else:
            self.logger.info('Offering color {}'.format(color))
            self.send('offer {}'.format(color))

        message = self.imcs_stream.readline().rstrip()
        self.detect_end_game_states(message)
        assert message[:3] == '103'

        # block waiting for game info
        result_code, self.color, time_remaining, opponent_time_remaining, message = self.get_game_info()

        opponent_move = None

        if result_code == '105':
            self.logger.info('Playing as White')
        else:
            self.logger.info('Playing as Black')
            opponent_move = self.get_opponent_move()  # Blocking. Also gets empty line.

        # The next expected input is the board.
        return [opponent_move] + self.read_board()

    def send_move(self, move):
        """
        Send a move string to the IMCS Server.

        Parameters
        ----------
        move: A move string in the format the IMCS server expects. Example c2-c3

        Returns
        -------
        List in the format of opponent's move, color, move number, board string, time remaining in milliseconds

        """
        self.logger.info('Sending move: {}'.format(move))
        self.send(move)

        response = self.imcs_stream.readline().strip()  # Normally this is the opponent's move
        self.logger.debug('Expecting opponent move string. Received {}'.format(response))

        # There could be an end game state at this point.
        self.detect_end_game_states(response)

        # Now we know that the response is an opponent move.
        return [response] + self.read_board()

    def send_resign(self):
        """
        Send the resign command to the server.

        Returns
        -------
        None
        """
        self.logger.info('Resigning the game')
        self.send('resign')


if __name__ == '__main__':
    import log

    logger = log.setup_custom_logger('root', level=5)
    with Server(logger=logger) as s:
        s.get_help()
        games = s.list_games()
        logger.debug(games)
        opponent_move, board = s.offer_game(color='B')
        logger.debug('opponent move: {}'.format(opponent_move))
        logger.debug('board:\n{}'.format(board))
