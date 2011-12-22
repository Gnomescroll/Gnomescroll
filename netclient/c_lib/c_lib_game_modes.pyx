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

    cdef cppclass CTFTeam:
        pass

cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        NoTeam none
        CTFTeam one
        CTFTeam two
        void set_team_color(int team, unsigned char r, unsigned char g, unsigned char b)

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    CTF ctf
#cdef extern from "./state/server_state.hpp" namespace "ServerState":
#    CTF ctf

class CTFTeamWrapper(object):

    def __getattribute__py(self, k):
        if k not in ['score', 'id', 'n', 'name']:
            raise AttributeError
    
        team = object.__getattribute__(self, 'team')
        if team == 1:
            if k == 'score':
                return (<Team>(ctf.one)).score
            elif k == 'id':
                return (<Team>(ctf.one)).id
            elif k == 'n':
                return (<Team>(ctf.one)).n
            elif k == 'name':
                return (<Team>(ctf.one)).name
        elif team == 2:
            if k == 'score':
                return (<Team>(ctf.two)).score
            elif k == 'id':
                return (<Team>(ctf.two)).id
            elif k == 'n':
                return (<Team>(ctf.two)).n
            elif k == 'name':
                return (<Team>(ctf.two)).name
        else:
            print "Cython TeamWrapper __getattribute__ ERROR: team id invalid %d" % (team,)
            raise ValueError

        raise AttributeError

class NoTeamWrapper(object):

    def __getattribute__py(self, k):
        if k not in ['id', 'n', 'name']:
            raise AttributeError
    
        cdef NoTeam t
        t = ctf.none
            
        if k == 'id':
            return t.id
        elif k == 'n':
            return t.n
        elif k == 'name':
            return t.name
        raise AttributeError
