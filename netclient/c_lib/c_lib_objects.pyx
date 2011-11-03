cdef extern from "./physics/vector.h":
    struct Vector:
        float x
        float y
        float z

cdef extern from "./physics/common.h":
    struct State:
        Vector p
        Vector v

cdef extern from "./objects/particles.hpp":
    cdef struct Particle2:
        State state
        unsigned int id

cdef extern from "./objects/grenade.hpp":
    cdef cppclass Grenade:
        Particle2 particle
        void set_ttl(int ttl)

    cdef cppclass Grenade_list:
        Grenade* get(int id)
        Grenade* create()
        Grenade* create(int id)
        Grenade* create(float x, float y, float z, float vx, float vy, float vz)
        Grenade* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void draw()
        void tick()
        
cdef extern from "./objects/cspray.hpp":
    cdef cppclass Cspray:
        Particle2 particle

    cdef cppclass Cspray_list:
        Cspray* get(int id)
        Cspray* create()
        Cspray* create(int id)
        Cspray* create(float x, float y, float z, float vx, float vy, float vz)
        Cspray* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void draw()
        void tick()

cdef extern from "./objects/shrapnel.hpp":
    cdef cppclass Shrapnel:
        Particle2 particle

    cdef cppclass Shrapnel_list:
        Shrapnel* get(int id)
        Shrapnel* create()
        Shrapnel* create(int id)
        Shrapnel* create(float x, float y, float z, float vx, float vy, float vz)
        Shrapnel* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void draw()
        void tick()
        
cdef extern from "./objects/neutron.hpp":
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

cdef extern from "./objects/blood.hpp":
    cdef cppclass Blood:
        Particle2 particle

    cdef cppclass Blood_list:
        Blood* get(int id)
        Blood* create()
        Blood* create(int id)
        Blood* create(float x, float y, float z, float vx, float vy, float vz)
        Blood* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void draw()
        void tick()

cdef extern from "./objects/minivox.hpp":
    cdef cppclass Minivox:
        Particle2 particle

    cdef cppclass Minivox_list:
        Minivox* get(int id)
        Minivox* create()
        Minivox* create(int id)
        Minivox* create(float x, float y, float z, float vx, float vy, float vz)
        Minivox* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void draw()
        void tick()

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    Cspray_list cspray_list
    Grenade_list grenade_list
    Shrapnel_list shrapnel_list
    Blood_list blood_list
    Neutron_list neutron_list
    Minivox_list minivox_list

def tick():
    neutron_list.tick()
    blood_list.tick()
    shrapnel_list.tick()
    grenade_list.tick()
    cspray_list.tick()
    minivox_list.tick()
    
def draw():
    neutron_list.draw()
    blood_list.draw()
    shrapnel_list.draw()
    grenade_list.draw()
    cspray_list.draw()
    minivox_list.draw()

def _create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz):
    cdef Neutron* neutron
    neutron = neutron_list.create(x,y,z, vx,vy,vz)
    if neutron is not NULL:
        neutron.set_energy(energy)

def _create_cspray(float x, float y, float z, float vx, float vy, float vz):
    cspray_list.create(x,y,z, vx,vy,vz)

def _create_blood(float x, float y, float z, float vx, float vy, float vz):
    blood_list.create(x,y,z, vx,vy,vz)

def _create_shrapnel(float x, float y, float z, float vx, float vy, float vz):
    shrapnel_list.create(x,y,z, vx,vy,vz)
    
def _create_minivox(float x, float y, float z, float vx, float vy, float vz):
    minivox_list.create(x,y,z, vx,vy,vz)

# Does not use TTL!! Can't cook grenades without TTL set
def _create_grenade(float x, float y, float z, float vx, float vy, float vz, int ttl):
    cdef Grenade* grenade
    grenade = grenade_list.create(x,y,z, vx,vy,vz)
    if grenade is not NULL:
        grenade.set_ttl(ttl)
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
        pos = None
    return pos


'''
Circuit Tree
'''
cdef extern from "./objects/circuit_tree.hpp":
    void circuit_tree_generate(int type, int seed)
    void circuit_tree_draw()

def _generate_circuit_tree(int type, int seed):
    circuit_tree_generate(type, seed)

def _draw_circuit_tree():
    circuit_tree_draw()
