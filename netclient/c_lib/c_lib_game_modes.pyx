from libcpp cimport bool
cdef extern from "./game/teams.hpp":
    cdef cppclass Team:
        int id
        int score
        char* name
        int n
        bool viewers

    cdef cppclass NoTeam:
        int id
        char* name
        int n
        bool viewers

    cdef cppclass CTFTeam:  # inherits Team
        pass

cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        NoTeam none
        CTFTeam one
        CTFTeam two
        void join_team(int team)

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    CTF ctf

class DummyCTFTeam(object):

    def __init__(self, id):
        self.id = id

    def is_viewers(self):
        if self.id == 1:
            return (<Team>(ctf.one)).viewers
        elif self.id == 2:
            return (<Team>(ctf.two)).viewers

ctf_one = DummyCTFTeam(1)
ctf_two = DummyCTFTeam(2)

class DummyNoTeam(object):

    def is_viewers(self):
        return ctf.none.viewers

ctf_none = DummyNoTeam()

def get_team(int id):
    if id == 0:
        return ctf_none
    elif id == 1:
        return ctf_one
    elif id == 2:
        return ctf_two
    else:
        print "c_lib_game_modes.get_team :: invalid team id %d" % (id,)

def join_team(int team):
    ctf.join_team(team)
