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
        if id not in [1,2]:
            print "DummyCTFTeam object -- id invalid %d" % (id,)
            raise ValueError
        self.id = id

    def __getattribute__(self, k):

        id = object.__getattribute__(self, 'id')
        cdef Team t
        if id == 1:
            t = <Team>(ctf.one)
        elif id == 2:
            t = <Team>(ctf.two)

        if k == 'score':
            return t.score
        elif k == 'name':
            return t.name
        elif k == 'n':
            return t.n
        elif k == 'viewers':
            return t.viewers
        elif k == 'id':
            return k.id
        elif k.startswith('__'):
            return object.__getattribute__(self, k)

        raise AttributeError

ctf_one = DummyCTFTeam(1)
ctf_two = DummyCTFTeam(2)

class DummyNoTeam(object):

    def __getattribute__(self, k):

        cdef NoTeam t
        t = ctf.none

        if k == 'name':
            return t.name
        elif k == 'n':
            return t.n
        elif k == 'viewers':
            return t.viewers
        elif k == 'id':
            return k.id
        elif k.startswith('__'):
            return object.__getattribute__(self, k)
        raise AttributeError

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
