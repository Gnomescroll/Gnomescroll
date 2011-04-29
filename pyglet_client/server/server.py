from twisted.protocols.basic import LineReceiver

from twisted.internet import reactor, protocol

class TestServer(LineReceiver):

    answers = {'How are you?': 'Fine', None : "I don't know what you mean"}

#    def connectionMade(self):
#        print "client connected"

    def connectionMade(self):
        self.factory.numProtocols = self.factory.numProtocols+1
        if self.factory.numProtocols > 100:
            print "client connected but too many users"
            self.transport.write("Too many connections, try later")
            self.transport.loseConnection()
        else:
            print "client connected"

    def connectionLost(self, reason):
        self.factory.numProtocols = self.factory.numProtocols-1
        print "connection lost"


    def lineReceived(self, line):
        print "line received"
        if self.answers.has_key(line):
            self.sendLine(self.answers[line])
        else:
            self.sendLine(self.answers[None])

class TestFactory(protocol.ServerFactory):
    proticol = TestServer

    def over_ride_methods():
        pass

factory = protocol.ServerFactory()
factory.protocol = TestServer
reactor.listenTCP(8066,factory)
reactor.run()


#factory = Factory()
#factory.protocol = Answer
# 8007 is the port you want to run under. Choose something >1024
#endpoint = TCP4ServerEndpoint(reactor, 8066)
#endpoint.listen(factory)
#reactor.run
