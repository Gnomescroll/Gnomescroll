""" Options & Settings """

from libcpp cimport bool

cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        bool auto_assign
        
cdef extern from "./state/client_state.hpp" namespace "ClientState":
    CTF ctf

def load(opts):
    ctf.auto_assign = opts.auto_assign_team
