#!/usr/bin/python

'''
Chat server
'''

class Chat:

    channels = {}
    clients = {}

    def __init__(self):
        pass

    def received(self, msg):
        print "chat received"
        print msg

    def send(self):
        pass

    def pm(self):
        pass

    def broadcast(self):
        pass

    def connect(self):
        client = ChatClient()


class ChatClient:

    def __init__(self):
        pass

