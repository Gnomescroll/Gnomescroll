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
        Commands.init()
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
        NetOut.sendMessage.is_admin()

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

    @classmethod
    def init(cls):
        cls.methods = {
            'ping'  :   cls.ping,
            'id'    :   cls.show_client_id,
            'clear' :   cls.clear_map,
            'set'   :   cls.set_map,
        }
        return cls

    @classmethod
    def _register(cls, handler):
        for name, meth in cls.methods.items():
            handler.add(name, meth)

    @classmethod
    def ping(cls):
        NetOut.miscMessage.ping()

    @classmethod
    def show_client_id(cls):
        print 'Client_id: %s' % (NetClientGlobal.client_id,)

    @classmethod
    def clear_map(cls):
        NetOut.adminMessage.clear_map()

    @classmethod
    def set_map(cls, x,y,z, val):
        x,y,z,val = map(int, [x,y,z,val])
        NetOut.adminMessage.set_map(x,y,z,val)


if __name__ == '__main__':
    client = Client()
    client.run()
