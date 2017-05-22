# Minichess AI #
#### CS542 Advanced AI: Combinatorial Games ####

### About ###

This is my project for CS542. This minichess AI will play games on the Portland State University Internet MiniChess Server, and eventually it will play well enough to compete.

### Installation ###

The installation utilizes ZeroMQ: http://zeromq.org/. Please ensure that ZeroMQ is installed and that the C++ binding (https://github.com/zeromq/cppzmq) is also installed by ensuring that the zmq.hpp file is in one of the paths found in the `#include <...> search starts here:` section when you execute: 

    cpp -v /dev/null -o /dev/null

The preferred compilation is via the make file. The Makefile is in the project root directory so you can issue one of the following commands:
* `make` <-- The default is the same as `make optimized`
* `make debug`
* `make odebug`
* `make optimized`
* `make standard`

There is a cmake file if you choose to use it.You're on your own with that.
	
For the Python portion, you'll need Python 2.7.13, Python 3.6.0, or Pypy2 5.7.0. To install issue the following CLI commands from the python environment you have:

    python setup.py build
    python setup.py install
	
### Playing a Game ###

Currently, this AI only supports accepting games and you must do it manually. This is still in the debugging stage, so the `play` bash script will start the c++ file as well as the python file with the `--debug` and `--player ab` flags. So all you have to do to start playing a game is to execute the following command:

    ./play

You'll see a menu of available games. Enter the number that corresponds with the game you want to play and hit enter and the game will play to completion.