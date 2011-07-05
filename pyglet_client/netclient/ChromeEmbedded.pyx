
cdef extern from 'libChrome.h':
    struct chromeDisplay:
        int x
        int y
        int width
        int height
        unsigned int tex_id

cdef extern from 'libChrome.h':
    int start()
    int _init2()
    int _update()
    int _draw()
    chromeDisplay* _create_webview(int x,int y, int width, int height)
    int _update_webview(chromeDisplay* webview)
    int _draw_webview(chromeDisplay* webview)

def test():
    print "test started"
    start()
    print "test finished"

def init():
    _init2()

def update():
    _update()

def draw():
    _draw()

cdef class cWindow:
    cdef chromeDisplay* display
    cpdef int x
    cpdef int y
    cpdef int width
    cpdef int height

    def __init__(self, x, y, width, height):
        self.display = _create_webview(x,y, width, height)

    def update(self):
        _update_webview(self.display)

    def draw(self):
        _draw_webview(self.display)

    def __del__(self):
        pass

    def resize(self):
        pass

