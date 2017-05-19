#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""IMCS server class

This class manages the connection to the server as well as sending
and formatting the IO. This code was inspired in part by chromakode
skirmish, https://github.com/chromakode/skirmish.
"""
from __future__ import print_function

import configparser
import logging
import socket
import sys

__author__ = "Michael Lane"
__email__ = "mikelane@gmail.com"
__copyright__ = "Copyright 2017, Michael Lane"
__license__ = "MIT"

if sys.version_info[:2] < (3,3):
    python_version = 2
else:
    python_version = 3

class Server:
    imcs_socket = socket.socket()
    try:
        imcs_stream = imcs_socket.makefile(mode='rw', newline='\r\n')
    except TypeError:
        imcs_stream = imcs_socket.makefile(mode='rw', bufsize=0)


    def __init__(self):
        self.logger = logging.getLogger('root')
        # Server and Player info MUST be stored in a settings.ini file in the root dir.
        config = configparser.ConfigParser()
        config.read('../settings.ini')
        self.host = config['SERVER']['host']
        self.port = int(config['SERVER']['port'])
        self.username = config['PLAYER']['username']
        self.password = config['PLAYER']['password']

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
        self.send('quit')
        self.imcs_stream.close()

    @classmethod
    def send(cls, command):
        if python_version == 3:
            print(command, file=cls.imcs_stream, flush=True)
        else:
            print(command, file=cls.imcs_stream)

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
        :return: None 
        """
        games = {}
        self.send('list')
        data = self.imcs_stream.readline().rstrip()
        self.logger.info(data)
        while data:
            data = self.imcs_stream.readline().rstrip()
            self.logger.info(data)
            parsed_data = data.split()
            if parsed_data[-1] == '[offer]':
                games[parsed_data[0]] = {
                    'opponent name': parsed_data[1],
                    'offered color': parsed_data[2],
                    'my time': parsed_data[3],
                    'opponent time': parsed_data[4],
                    'opponent rank': parsed_data[5],
                    'status': parsed_data[6]
                }
            if data == '.':
                self.logger.info(data)
                return games

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
