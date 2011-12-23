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

    cdef cppclass CTFTeam:  #inherits Team
        pass

cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        NoTeam none
        CTFTeam one
        CTFTeam two
        void set_team_color(int team, unsigned char r, unsigned char g, unsigned char b)
        void add_agent_to_team(int team, int agent)

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    CTF ctf

def set_team_color(int team, unsigned char r, unsigned char g, unsigned char b):
    ctf.set_team_color(team, r,g,b)

class DummyCTFTeam(object):

    def __init__(self, id):
        self.id = id

    def __getattribute__py(self, k):
        cdef Team t
        if self.id == 1:
            t = <Team>(ctf.one)
        elif self.id == 2:
            t = <Team>(ctf.two)

        if k == 'score':
            return t.score
        elif k == 'name':
            return t.name
        elif k == 'n':
            return t.n
        elif k == 'viewers':
            return  k.viewers
        elif k == 'id':
            return k.id

        raise AttributeError

    def is_viewers(self):
        if self.id == 1:
            return (<Team>(ctf.one)).viewers
        elif self.id == 2:
            return (<Team>(ctf.two)).viewers

ctf_one = DummyCTFTeam(1)
ctf_two = DummyCTFTeam(2)

class DummyNoTeam(object):

    def __getattribute__py(self, k):
        cdef NoTeam t
        t = ctf.none

        if k == 'name':
            return t.name
        elif k == 'n':
            return t.n
        elif k == 'viewers':
            return  k.viewers
        elif k == 'id':
            return k.id

        raise AttributeError

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


def join_team(int agent_id, int team_id):
    ctf.add_agent_to_team(team_id, agent_id)
    
