
cdef extern from 'libChrome.h':
    int start()

def test():
    print "test started"
    start()
    print "test finished"
