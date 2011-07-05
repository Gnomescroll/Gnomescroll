
cdef extern from 'libChrome.h':
    int start()
    int _init()
    int _update()
    int _draw()

def test():
    print "test started"
    start()
    print "test finished"

def init():
    _init()

def update():
    _update()

def draw():
    _draw()
