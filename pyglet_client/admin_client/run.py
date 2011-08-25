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
        self.commands = Commands
        self.commandHandler = commands.CommandHandler()
        self.add_commands()

    def init_globals(self):
        NetClientGlobal.init()
        NetOut.init()
        NetEventGlobal.init()
        NetClientGlobal.init1()

    def add_commands(self):
        self.commands._register(self.commandHandler)
        self.commandHandler.add('exit', self.exit)

    def exit(self):
        self._exit = True

    def connect(self):
        NetClientGlobal.connect()

    def run(self):
        self.connect()

        while not self._exit:
            NetClientGlobal.connection.attempt_recv()
            try:
                cmd = raw_input('>').strip()
            except EOFError:
                self._exit = True
                print ''
                continue
            if cmd:
                self.commandHandler.process(cmd)

class Commands(object):

    methods = [
        'ping',
        'id',
    ]

    @classmethod
    def _register(cls, handler):
        for meth in cls.methods:
            handler.add(meth, getattr(cls, meth))

    @classmethod
    def ping(cls):
        NetOut.miscMessage.ping()

    @classmethod
    def id(cls):
        print 'Client_id: %s' % (NetClientGlobal.client_id,)



if __name__ == '__main__':
    client = Client()
    client.run()
