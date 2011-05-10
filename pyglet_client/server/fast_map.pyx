

cdef extern from "./clib/fast_map.c":
    int test()

cpdef test2():
    return test()

#cdef extern inline from "./clib/fast_map.c":

cdef extern from "./clib/fast_map.c":
    int hash_cord(int)

cpdef hash_test(x):
    return hash_cord(x)
