#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Defines the errors thrown by minichess"""

# Imports

__author__ = "Michael Lane"
__email__ = "mikelane@gmail.com"
__copyright__ = "Copyright 2017, Michael Lane"
__license__ = "MIT"


class GameOver(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


class InvalidMove(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


class ServerError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


class PlayerError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)
