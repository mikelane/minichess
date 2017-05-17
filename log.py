#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Set up the logging environment

Copied from Stack Overflow:
http://stackoverflow.com/questions/7621897/python-logging-module-globally"""

# Imports
import logging


__author__ = "Michael Lane"
__email__ = "mikelane@gmail.com"
__copyright__ = "Copyright 2017, Michael Lane"
__license__ = "MIT"


def setup_custom_logger(name:str, level:int) -> logging.Logger:
    formatter = logging.Formatter(fmt='%(asctime)s - %(levelname)s - %(module)s - %(message)s')

    handler = logging.StreamHandler()
    handler.setFormatter(formatter)

    logger = logging.getLogger(name=name)

    if level >= 4:
        logger.setLevel(logging.DEBUG)
    elif level == 3:
        logger.setLevel(logging.INFO)
    elif level == 2:
        logger.setLevel(logging.WARNING)
    elif level == 1:
        logger.setLevel(logging.ERROR)
    else:
        logger.setLevel(logging.CRITICAL)

    logger.addHandler(handler)
    return logger