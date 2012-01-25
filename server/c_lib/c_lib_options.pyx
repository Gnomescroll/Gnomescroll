""" Options & Settings """

from libcpp cimport bool

cdef extern from "./game/game.hpp":
    void set_team_kills(bool team_kills)

cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        void set_team_name(int team, char* name)

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    CTF ctf

def load(opts):
    set_team_kills(opts.team_kills)

    ctf.set_team_name(1, opts.team_name_one)
    ctf.set_team_name(2, opts.team_name_two)
