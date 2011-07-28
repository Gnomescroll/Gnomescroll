#!/usr/bin/python

'''
Intervals

Manages methods that should be triggered on a time based interval,
as opposed to every tick of the loop
'''

import time

def now():
    ''' returns time in ms '''
    return time.time() * 1000
    

class Intervals:

    def __init__(self):
        self.events = {}

    def register(self, meth, t):    # time in ms
        self.events.setdefault(t, []).append(meth)

    def set(self):
        self.events = self.events.items()   # convert registered methods to list of tuples
        self.events = sorted(self.events, key=lambda t: t[0])   # sort by ascending time
        self.events = map(list, self.events)    # convert items to lists
        [ev.append(0) for ev in self.events]    # append 3 item, t=0

    def process(self):
        n = now()
        for item in self.events:
            inc, meths, last_t = item
            if n - last_t > inc:
                for meth in meths:
                    meth()
                item[2] = n
