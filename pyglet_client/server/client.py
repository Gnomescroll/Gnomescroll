#from sys import stdout
#class Echo(Protocol):
    #def dataReceived(self, data):
        #stdout.write(data

#from twisted.internet.protocol import Protocol

#class WelcomeMessage(Protocol):
    #def connectionMade(self):
        #self.transport.write("Hello server, I am the client!\r\n")
        #self.transport.loseConnection


from twisted.internet.protocol import Factory, Protocol

from twisted.internet import reactor, protocol

class TestProtocol(protocol.Protocol):

    def connectionMade(self):

    def sendMessage(self, msg):
        self.transport.write("MESSAGE %s\n" % msg)


class TestFactory(protocol.ClientFactory):
    protocol = TestProtocol

    def clientConnectionFailed(self, connector, reason):
        print "Connection failed - goodbye!"
        reactor.stop()

    def clientConnectionLost(self, connector, reason):
        print "Connection lost - goodbye!"
        reactor.stop()

#factory = TestFactory()
#reactor.connectTCP("localhost", 8066, factory)

i = TestProtocol()

reactor.connectTCP("localhost", 8066, i)
#reactor.callLater(4, instance.sendMessage('test message'))

#d = point.connect(factory)
#d.addCallback(gotProtocol)

reactor.run()

