#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""IMCS server class

This class manages the connection to the server as well as sending
and formatting the IO. This code was inspired in part by chromakode
skirmish, https://github.com/chromakode/skirmish.
"""
from __future__ import print_function

import logging
import socket
import sys
import datetime
import configparser
import re

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

    def __init__(self):
        self.logger = logging.getLogger('root')
        # Server and Player info MUST be stored in a settings.ini file in the root dir.
        config = configparser.ConfigParser()
        config.read('settings.ini')
        self.host = config['SERVER']['host']
        self.port = int(config['SERVER']['port'])
        self.username = config['PLAYER']['username']
        self.password = config['PLAYER']['password']
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
        self.logger.info(self.imcs_stream.readline())
        self.send('me {} {}'.format(self.username, self.password))
        self.logger.info(self.imcs_stream.readline())
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        This is run whenever the with block ends. This makes it simple to do cleanup
        operations. For this server, that means sending the quit signal and closing the stream.
        :param exc_type: 
        :param exc_val: 
        :param exc_tb: 
        :return: None
        """
        self.logger.info('Disconnecting from IMCS')
        self.send('quit')
        self.imcs_stream.close()

    @classmethod
    def send(cls, command):
        if python_version == 3:
            print(command, file=cls.imcs_stream, flush=True)
        else:
            print(command, file=cls.imcs_stream)
            cls.imcs_stream.flush()

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
        :return: List of strings of results
        """
        results = []
        self.send('list')
        data = self.imcs_stream.readline().rstrip()
        self.logger.info(data)

        while data:
            data = self.imcs_stream.readline().rstrip()
            self.logger.debug(data)
            if '[offer]' in data:
                results.append(data)
            elif data == '.':
                self.logger.debug(data)
                return results

    def accept_game(self, game_number, color=None):
        """
        Accept the game with the passed in game number
        
        Parameters
        ----------
        game_number The number of the game to accept

        Returns
        -------
        Tuple of Board string and float number of milliseconds left on the timer 

        """
        if color:
            self.send('accept {} {}'.format(game_number, color))
        else:
            self.send('accept {}'.format(game_number))

        info_line = self.imcs_stream.readline().strip()
        assert info_line[:3] in ['105', '106']

        if '105' in info_line:  # I am White
            self.logger.info('Playing as White')
            self.color = 'W'
            b = self.imcs_stream.readline()  # Blank line
            self.logger.debug('Expecting blank. Received: {}'.format(b))
        elif '106' in info_line:  # I am Black
            self.logger.info('Playing as Black')
            self.color = 'B'
            opponent_move = self.imcs_stream.readline().strip()  # this may block
            self.logger.debug('Expecting opponent move. Received {}'.format(opponent_move))
            b = self.imcs_stream.readline()  # blank line
            self.logger.debug('Expecting blank line. Received {}'.format(b))

        # Read the board
        self.logger.debug('Attempting to read the board')
        board = ''
        timer_string = ''
        keep_reading = True
        while keep_reading:
            line = self.imcs_stream.readline()
            if line[0] == '?':
                timer_string = line.strip()
                keep_reading = False
            else:
                board += line

        # Parse the timer and get my time left in milliseconds
        my_time = re.split('\D', timer_string.split()[1])
        time_left = datetime.timedelta(minutes=float(my_time[0]), seconds=float(my_time[1]), milliseconds=float(my_time[2]))
        return board, int(time_left.total_seconds() * 1000)

    def offer_game(self, color='?', duration=None):
        if duration:
            self.logger.info('Offering color {} and duration {}'.format(color, duration))
            self.send('offer {} {}'.format(color, duration))
        else:
            self.logger.info('Offering color {}'.format(color))
            self.send('offer {}'.format(color))
        message = self.imcs_stream.readline().rstrip()
        assert message[:3] == '103'
        while True:
            message = self.imcs_stream.readline().rstrip()
            if message:
                self.logger.info(message)
                message = message.split()
                assert message[0] in ['105', '106']
                if message[0] == '105':
                    self.imcs_stream.readline()  # blank line
                    return None, self.imcs_stream.readline().rstrip()  # initial board state
                if message[0] == '106':
                    opponent_move = self.imcs_stream.readline().rstrip()[2:]
                    self.imcs_stream.readline()  # blank line
                    return opponent_move, self.imcs_stream.readline().rstrip()

    def send_move(self, move):
        self.logger.info('Sending move: {}'.format(move))
        self.send(move)

        # @todo keep track of opponent's moves
        opponent_move = self.imcs_stream.readline().strip()  # opponent's move
        self.logger.debug('Expecting opponent move string. Received {}'.format(opponent_move))
        assert 'illegal move' not in opponent_move
        self.logger.debug('Should be opponents move: {}'.format(opponent_move))

        b = self.imcs_stream.readline()  # blank or win message
        if 'wins' in b:
            if self.color in b:
                self.logger.info('YOU WON!')
            else:
                self.logger.info('You lost')
            return "GAME OVER", 0

        board = ""
        for _ in range(7):
            board += self.imcs_stream.readline()
        self.imcs_stream.readline()  # blank line

        # Parse the timer and get my time left in milliseconds
        my_time = re.split('\D', self.imcs_stream.readline().strip().split()[1])
        time_left = datetime.timedelta(minutes=float(my_time[0]), seconds=float(my_time[1]), milliseconds=float(my_time[2]))
        return board, int(time_left.total_seconds() * 1000)

    def send_resign(self):
        self.logger.info('Resigning the game')
        self.send('resign')


if __name__ == '__main__':
    import log

    logger = log.setup_custom_logger('root', level=5)
    with Server() as s:
        s.get_help()
        games = s.list_games()
        logger.debug(games)
        opponent_move, board = s.offer_game(color='B')
        logger.debug('opponent move: {}'.format(opponent_move))
        logger.debug('board:\n{}'.format(board))
