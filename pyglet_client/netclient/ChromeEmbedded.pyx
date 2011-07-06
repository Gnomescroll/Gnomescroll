
cdef extern from 'libChrome.h':
    struct chromeDisplay:
        int x
        int y
        int width
        int height
        unsigned int tex_id

cdef extern from 'libChrome.h':
    int _init2()
    int _update()
    chromeDisplay* _create_webview(int x,int y, int width, int height)
    int _update_webview(chromeDisplay* webview)
    int _draw_webview(chromeDisplay* webview)
    int _set_window_focus(chromeDisplay* window)
    int _defocus()

def init():
    _init2()

def update():
    _update()

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

    def focus(self):
        _set_window_focus(self.display)

    def defocus(self):
        _defocus()

    def __del__(self):
        pass

    def resize(self):
        pass

