#!/usr/bin/python

'''
Admin client
'''

from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal

import commands

class Client:

    _exit = False

    def __init__(self):
        self.init_globals()
        self.commands = commands.Commands()
        self.add_commands()

    def init_globals(self):
        NetClientGlobal.init()
        NetOut.init()
        NetEventGlobal.init()
        NetClientGlobal.init1()

    def add_commands(self):
        c = self.commands

        def ping():
            NetOut.miscMessage.ping()

        c.add('ping', ping)
        c.add('exit', self.exit)

    def exit(self):
        self._exit = True

    def run(self):

        while not self._exit:
            try:
                cmd = raw_input('>')
            except EOFError:
                self._exit = True
                print ''
                continue
            self.commands.process(cmd)


if __name__ == '__main__':
    client = Client()
    client.run()
