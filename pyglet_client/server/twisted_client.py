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
from twisted.protocols.basic import Int32StringReceiver

#sendString(self, data)
#Send an int32-prefixed string to the other end of the connection.

class TestProtocol(Int32StringReceiver):

    def connectionMade(self):
        reactor.callLater(4, self.sendString,'test message')
        print "connected"


    def stringReceived(self, string):
        print "string received: %s" % string
        pass

#    def sendString(self, string):
#        pass

    def lengthLimitExceeded(self, length):
        print "string length exceeds max length" #default implementation closes connection
        pass

class TestFactory(protocol.ClientFactory):
    protocol = TestProtocol

    def clientConnectionFailed(self, connector, reason):
        print "Connection failed - goodbye!"
        reactor.stop()

    def clientConnectionLost(self, connector, reason):
        print "Connection lost - goodbye!"
        reactor.stop()

factory = TestFactory()
reactor.connectTCP("localhost", 8066, factory)

#reactor.callLater(4, x.sendString('test message'))
reactor.run()

#d = point.connect(factory)
#d.addCallback(gotProtocol)



