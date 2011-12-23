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

def join_team(int team):
    ctf.join_team(team)
