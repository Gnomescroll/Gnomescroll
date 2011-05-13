

cdef extern from "./clib/fast_map.c":
    int test()

cpdef test2():
    return test()
