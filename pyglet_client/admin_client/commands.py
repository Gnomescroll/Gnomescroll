'''
Handles admin commands from terminal
'''

class CommandHandler(object):

    def __init__(self):
        self.methods = {}

    def add(self, cmd, action):
        self.methods[cmd] = action
        
    def process(self, cmd):
        cmd, args = self.tokenize(cmd)
        self.take_action(cmd, args)

    def tokenize(self, cmd):
        cmd = cmd.split(' ')
        args = cmd[1:]
        cmd = cmd[0]
        return cmd, args

    def take_action(self, cmd, args):
        if cmd in self.methods:
            try:
                self.methods[cmd](*args)
            except TypeError, e:
                print e
        else:
            self.unknown_cmd(cmd)

    def unknown_cmd(self, cmd):
        print 'Unknown command %s' % (cmd,)
