#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Short description

Long description
"""

import argparse
import log

__author__ = "Michael Lane"
__email__ = "mikelane@gmail.com"
__copyright__ = "Copyright 2017, Michael Lane"
__license__ = "MIT"

parser = argparse.ArgumentParser(description='''Minitchess player: Please ensure that you are using a python3 interpreter.''')
parser.add_argument('--debug', help='Turn on debugging mode', action='store_true')
parser.add_argument('-v', '--verbosity', action='count', default=0,
                    help='Increase output verbosity, blank shows ERROR only, '
                         '-v shows INFO and ERROR, -vv shows WARNING, INFO, and ERROR, and '
                         '-vvv shows all logs')
args = parser.parse_args()

if args.debug:
    logger = log.setup_custom_logger('root', level=5)
else:
    logger = log.setup_custom_logger('root', level=args.verbosity)

if __name__ == '__main__':
    pass