from twisted.protocols.basic import LineReceiver
from twisted.internet import reactor, protocol
from twisted.protocols.basic import Int32StringReceiver

class TestServer(Int32StringReceiver):
    numProtocols = 0

    def connectionMade(self):
        self.numProtocols += 1
        if self.numProtocols > 100:
            print "client connected but too many users"

            self.sendString("Too many connections, try later")
            #self.transport.write("Too many connections, try later")
            self.transport.loseConnection()
        else:
            print "client connected"

    def connectionLost(self, reason):
        self.numProtocols += -1
        print "connection lost"

    def stringReceived(self, string):
        print "string received: %s" % string
        pass

    #def lineReceived(self, line):
        #print "line received"
        #if self.answers.has_key(line):
            #self.sendLine(self.answers[line])
        #else:
            #self.sendLine(self.answers[None])

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
