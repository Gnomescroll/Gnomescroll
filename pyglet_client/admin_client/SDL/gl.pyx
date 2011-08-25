cimport libc.stdlib

## SDL functions ##
cdef extern int _get_ticks()

def get_ticks():
    return _get_ticks()

