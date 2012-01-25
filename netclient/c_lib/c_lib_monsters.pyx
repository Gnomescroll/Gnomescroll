from libcpp cimport bool

cdef extern from "./monsters/monsters.hpp" namespace "Monsters":

    cdef cppclass Slime_list:
        void update()

    void test(int n)

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    Slime_list slime_list

def slime_test(int n):
    test(n)

def slime_tick():
    slime_list.update()
