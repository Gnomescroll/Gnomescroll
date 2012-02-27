"""
Linux Use Only
"""

import platform
OS = platform.system()

if OS != 'Linux':
    raise ImportError

import sys
import select
import tty
import termios

def isData():
    return select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], [])

def check_stdin():
    if isData():
        c = sys.stdin.read(1)
        if c == '\x1b':         # x1b is ESC, (also arrow keys??)
            return True
        else:
            return False
    return False

