""" Options & Settings """

from libcpp cimport bool

cdef extern from "./game/game.hpp":
    bool team_kills

def load(opts):
    team_kills = opts.team_kills
