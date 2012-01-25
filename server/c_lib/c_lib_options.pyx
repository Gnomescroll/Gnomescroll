""" Options & Settings """

from libcpp cimport bool

cdef extern from "./game/game.hpp":
    void set_team_kills(bool team_kills)

def load(opts):
    set_team_kills(opts.team_kills)
