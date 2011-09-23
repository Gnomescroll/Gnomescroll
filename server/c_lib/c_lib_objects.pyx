cdef extern from "./physics/vector.h":
    struct Vector:
        float x
        float y
        float z

cdef extern from "./objects/grenade.h":
    void grenade_tick()
    int create_grenade(int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max)
    void destroy_grenade(int gid)
    Vector* _get_grenade_position(int gid)

cdef extern from "./objects/neutron.h":
    void neutron_tick()
    void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/cspray.h":
    void cspray_tick()
    void create_cspray(int type, float x, float y, float z, float vx, float vy, float vz)

'''
cdef extern from "./agent/agent.h":
    void agent_tick()
'''

def tick():
    grenade_tick()
    neutron_tick()
    cspray_tick()
    #agent_tick()

def _create_grenade(float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max):
    return create_grenade(1, x,y,z, vx,vy,vz, ttl, ttl_max)

def _destroy_grenade(int gid):
    destroy_grenade(gid)

def _create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz):
    create_neutron(type,energy, x,y,z, vx,vy,vz)

def _create_cspray(int type, float x, float y, float z, float vx, float vy, float vz):
    create_cspray(type, x,y,z, vx,vy,vz)

def get_grenade_position(int gid):
    cdef Vector *pos
    pos = _get_grenade_position(gid)
    p = [pos.x, pos.y, pos.z]
    return p
