#!/usr/bin/python

'''
Admin client
'''

from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal

import commands

import time

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
            'perlin':   cls.perlin,
            'save'  :   cls.save_map,
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

    @classmethod
    def perlin(cls, block_val=6):
        block_val = int(block_val)
        #NetOut.adminMessage.set_map_bulk(perlin_map())
        perlin_map(block_val)

    @classmethod
    def save_map(cls, name=''):
        if not name:
            name = 'map_%0.2f' % (time.time())
            name = name.replace('.', '_')
        name = name.replace('/', '_')
        NetOut.adminMessage.save_map(name)

def perlin_map(block_val):

    import time
    def check_setct(setct, map):
        if setct == 64:
            NetOut.adminMessage.set_map_bulk(map)
            time.sleep(0.03)
            return True
        return False
    
    map = []
    setct = 0
    print 'start map gen'
    max_height = 15
    from map_gen import Gen
    from random import random
    g = Gen(salt=random())
    h=0
    for i in range(128):
        for j in range(128):
            h = g.getHeight(i,j)
            h = abs(h)
            h *= 100
            h %= max_height
            h = int(h)
            for k in range(h+1):
                map.append([i,j,k, block_val])
                setct += 1
                if check_setct(setct, map):
                    setct = 0
                    map = []
                #terrain_map.set(i, j, k, 2)
            if h==0 or h==1:
                #terrain_map.set(i,j, 2, 211)
                map.append([i,j, 2, 211])
                setct += 1
                if check_setct(setct, map):
                    setct = 0
                    map = []

    NetOut.adminMessage.set_map_bulk(map)
    print 'done map gen'
    
if __name__ == '__main__':
    client = Client()
    client.run()
