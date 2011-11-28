cdef extern from "./physics/vector.h":
    struct Vector:
        float x
        float y
        float z

cdef extern from "./physics/common.h":
    struct State:
        Vector p
        Vector v

cdef extern from "./particles/particles.hpp":
    cdef struct Particle2:
        State state
        unsigned int id

cdef extern from "./particles/grenade.hpp":
    cdef cppclass Grenade:
        Particle2 particle
        void set_owner(int owner)

    cdef cppclass Grenade_list:
        Grenade* get(int id)
        Grenade* create()
        Grenade* create(int id)
        Grenade* create(float x, float y, float z, float vx, float vy, float vz)
        Grenade* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void tick()

cdef extern from "./particles/cspray.hpp":
    cdef cppclass Cspray:
        Particle2 particle

    cdef cppclass Cspray_list:
        Cspray* get(int id)
        Cspray* create()
        Cspray* create(int id)
        Cspray* create(float x, float y, float z, float vx, float vy, float vz)
        Cspray* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void tick()

cdef extern from "./particles/neutron.hpp":
    cdef cppclass Neutron:
        Particle2 particle
        void set_energy(int energy)

    cdef cppclass Neutron_list:
        Neutron* get(int id)
        Neutron* create()
        Neutron* create(int id)
        Neutron* create(float x, float y, float z, float vx, float vy, float vz)
        Neutron* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void draw()
        void tick()


cdef extern from "./state/server_state.hpp" namespace "ServerState":
    Grenade_list grenade_list
    Cspray_list cspray_list
    Neutron_list neutron_list

def tick():
    grenade_list.tick()
    cspray_list.tick()
    neutron_list.tick()
    
def _create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz):
    cdef Neutron* neutron
    neutron = neutron_list.create(x,y,z, vx,vy,vz)
    neutron.set_energy(energy)

def _create_cspray(float x, float y, float z, float vx, float vy, float vz):
    cspray_list.create(x,y,z, vx,vy,vz)

# Does not use TTL!! Can't cook grenades without TTL set
def _create_grenade(float x, float y, float z, float vx, float vy, float vz, int ttl, int owner):
    cdef Grenade* grenade
    grenade = grenade_list.create(x,y,z, vx,vy,vz)
    if grenade is not NULL:
        grenade.set_owner(owner)
        return grenade.particle.id
    return -1

def _destroy_grenade(int gid):
    grenade_list.destroy(gid)

def get_grenade_position(int gid):
    cdef Grenade* grenade
    grenade = grenade_list.get(gid)
    if grenade is not NULL:
        pos = [grenade.particle.state.p.x, grenade.particle.state.p.y, grenade.particle.state.p.z]
    else:
        pos = [0., 0., 0.]
    return pos

