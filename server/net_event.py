#'''
#Incoming net events
#'''

#import c_lib.terrain_map as terrain_map

#class NetEvent:

    #messageHandler = None
    #adminMessageHandler = None
    #miscMessageHandler = None
    #mapMessageHandler = None

    #@classmethod
    #def init(cls):
        #cls.messageHandler = MessageHandler()
        #cls.adminMessageHandler = AdminMessageHandler()
        #cls.miscMessageHandler = MiscMessageHandler()
        #cls.mapMessageHandler = MapMessageHandler()

    #@classmethod
    #def register_json_events(cls, events, interface=None):
        #for name, function in events.items():
            #if function is None and interface is not None:
                #function = getattr(interface, name)
            #cls.messageHandler.json_events[name] = function

#from net_server import NetServer
#from net_out import NetOut

## routes messages by msg.cmd
#class MessageHandler:

    #def __init__(self):
        #self.json_events = {} #map strings to functions

    #def process_json(self, msg, connection):
        #cmd = msg.get('cmd', None)
        ##print "MessageHandler.process_json: " + str(msg)
        #if cmd is None:
            #print 'Warning: message missing cmd. message: %s' % msg
            #return
        ##use json_events when possible
        #if self.json_events.has_key(cmd):
            #self.json_events[cmd](msg, connection)
        #else:
            #print "MessageHandler.process_json: cmd unknown = %s" % (str(msg),)

#'''
#Common Errors
#'''

#class ErrorNames:

    #def key_missing(self, key):
        #return '%s missing' % (key,)

    #def wrong_size(self, key, size=None, actual_size=None):
        #msg = '%s of wrong size' % (key,)
        #if size is not None and actual_size is not None:
            #msg = '%s; should be %d, but is %d' % (msg, size, actual_size,)
        #return msg

    #def not_iterable(self, key):
        #return '%s is not iterable' % (key,)

#err = ErrorNames()


#'''
#Processors, for common message items/keys
#'''

#class ProcessedIterableMessage:
    #__slots__ = ['error', 'iterable']
    #def __init__(self, error, iterable):
        #self.error = error
        #self.iterable = iterable

#class Processors:

## for iterable items
    #def iterable(self, msg, key, size, err_key=None):
        #if err_key is None:
            #err_key = key
        #err_msg, obj = None, None

        #try:
            #obj = msg[key]
            #angle = list(obj)
            #actual_size = len(obj)
            #if actual_size != size:
                #err_msg = err.wrong_size(err_key, size, actual_size)
        #except KeyError:
            #err_msg = err.key_missing(err_key)
        #except TypeError:
            #err_msg = err.not_iterable(err_key)

        #return ProcessedIterableMessage(err_msg, obj)

#processor = Processors()


#'''
    #Decorators
    #Use @logError('msg_name') first

    #The other decorators insert arguments, so make sure to align the decorated
    #function's arguments with the order in which the decorators were applied
#'''

## THIS IS NOT A DECORATOR, it is just used by decorators
#def _add_arg(args, item):
    #args = list(args)
    #args.append(item)
    #return args

## prints full error
#def logError(msg_name):
    #def outer(f):
        #def wrapped(self, msg, conn, *args, **kwargs):
            #err = f(self, msg, conn, *args, **kwargs)
            #if err is not None:
                #print '%s :: %s' % (msg_name, err,)
        #return wrapped
    #return outer

## simple require of msg key, use when no extra validation/processing beyond key existance
## the decorated function must include matching arguments positionally,
## in order of decoration, and after any other positional arguments (i.e. connection or player)
#def requireKey(key, err_key=None):
    #if err_key is None:
        #err_key = key
    #def outer(f, *args, **kwargs):
        #def wrapped(self, msg, *args, **kwargs):
            #try:
                #thing = msg[key]
            #except KeyError:
                #return err.key_missing(err_key)
            #args = _add_arg(args, thing)
            ##print msg, args, kwargs
            #return f(self, msg, *args, **kwargs)
        #return wrapped
    #return outer

#def processIterable(key, size, err_key=None):
    #def outer(f, *args, **kwargs):
        #def wrapped(self, msg, *args, **kwargs):
            #p = processor.iterable(msg, key, size, err_key)
            #if p.error:
                #return p.error
            #args = _add_arg(args, p.iterable)
            #return f(self, msg, *args, **kwargs)
        #return wrapped
    #return outer


#'''
#Message Handlers
#'''

#class GenericMessageHandler:

    #def __init__(self):
        #self.register_events()

    #def _events(self):
        #return {}

    #def register_events(self):
        #NetEvent.register_json_events(self.events(), self)


#class AdminMessageHandler(GenericMessageHandler):

    #def events(self):
        #return {
            #'save_map': self._save_map,
        #}

    #@requireKey('name')
    #def _save_map(self, msg, conn, name):
        #terrain_map.save_to_disk(name)

#class MiscMessageHandler(GenericMessageHandler):

    #def events(self):
        #return {
            #'ping'  :   self.ping,
        #}

    #@logError('ping')
    #@requireKey('timestamp')
    #def ping(self, msg, connection, ts):
        #connection.sendMessage.ping(ts)

#class MapMessageHandler(GenericMessageHandler):

    #def events(self):
        #return {
            #'request_chunk' :   self.request_chunk,
            #'request_chunk_list':  self.send_chunk_list,
        #}

    #@logError('request_chunk')
    #@processIterable('value', 3)
    #def request_chunk(self, msg, connection, block):
        #x,y,z = block
        #connection.sendMessage.send_chunk(x,y,z)

    #@logError('send_chunk_list')
    #def send_chunk_list(self, msg, connection):
        #connection.sendMessage.send_chunk_list()
