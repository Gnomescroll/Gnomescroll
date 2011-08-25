#!/usr/bin/python

'''
Admin client
'''

from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal

import commands

class Client:

    exit = False

    def __init__(self):
        self.init_globals()
        self.commands = commands.Commands()

    def init_globals(self):
        NetClientGlobal.init()
        NetOut.init()
        NetEvent.init()

    def run(self):

        while not self.exit:
            cmd = raw_input('>')
            self.commands.process(cmd)


if __name__ == '__main__':
    client = Client()
    client.run()
