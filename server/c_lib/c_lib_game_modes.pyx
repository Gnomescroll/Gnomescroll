cdef extern from "./game/teams.hpp":
    cdef cppclass Team:
        int id
        int score
        char* name
        int n

    cdef cppclass NoTeam:
        int id
        char* name
        int n

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

#    CTFTeam* team

    def __init__(self, id):
#        if id == 1:
#            self.team = cython.address(ctf.one)
#        elif id == 2:
#            self.team = cython.address(ctf.two)
#        else:
#            print "DummyCTFTeam init :: invalid id %d" % (id,)
        pass
    def is_viewers(self):
        return False

ctf_one = DummyCTFTeam(1)
ctf_two = DummyCTFTeam(2)

class DummyNoTeam(object):

#    NoTeam* team

    def __init__(self):
#        self.team = cython.address(ctf.none)
        pass
    def is_viewers(self):
        return True

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
    
