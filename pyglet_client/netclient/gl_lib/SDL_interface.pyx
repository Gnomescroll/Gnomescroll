

cdef extern int test_function(int x)
cdef extern int opengl_test(int x)
cdef extern int opengl_test2(int x)

cpdef t1(int x):
    return test_function(x)

cpdef t2(int x):
    return opengl_test(x)

cpdef t3(int x):
    return opengl_test2(x)
