cdef extern from "./physics/vector.h":
    struct Vector:
        float x
        float y
        float z

cdef extern from "./physics/common.h":
    struct State:
        Vector p
        Vector v

#cdef extern from "./objects/neutron.h":
#    void neutron_tick()
#    void neutron_draw()
#    void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz)

#cdef extern from "./objects/blood.h":
#    void blood_tick()
#    void blood_draw()
#    void create_blood(int type, float x, float y, float z, float vx, float vy, float vz)

#cdef extern from "./objects/circuit_tree.h":
#    void circuit_tree_generate(int type, int seed)
#    void circuit_tree_draw()

cdef extern from "./objects/particles.hpp":
    cdef struct Particle2:
        State state
        unsigned int id

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

cdef extern from "./objects/grenade.hpp":
    cdef cppclass Grenade:
        Particle2 particle

    cdef cppclass Grenade_list:
        Grenade* get(int id)
        Grenade* create()
        Grenade* create(int id)
        Grenade* create(float x, float y, float z, float vx, float vy, float vz)
        Grenade* create(int id, float x, float y, float z, float vx, float vy, float vz)
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


cdef extern from "./state/cython_imports.hpp" namespace "ClientState":
    Cspray_list cspray_list
    Grenade_list grenade_list
    Shrapnel_list shrapnel_list
#    Neutron_list neutron_list
#    Blood_list blood_list


def tick():
#    neutron_tick()
#    blood_tick()

    shrapnel_list.tick()
    grenade_list.tick()
    cspray_list.tick()
    
def draw():
#    neutron_draw()
#    blood_draw()

    shrapnel_list.draw()
    grenade_list.draw()
    cspray_list.draw()

def _create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz):
    pass
#    create_neutron(type,energy, x,y,z, vx,vy,vz)

def _create_cspray(float x, float y, float z, float vx, float vy, float vz):
    cspray_list.create(x,y,z, vx,vy,vz)

def _create_blood(int type, float x, float y, float z, float vx, float vy, float vz):
    pass
#    create_blood(type, x,y,z, vx,vy,vz)

def _create_shrapnel(float x, float y, float z, float vx, float vy, float vz):
    shrapnel_list.create(x,y,z, vx,vy,vz)

def _generate_circuit_tree(int type, int seed):
    pass
#    circuit_tree_generate(type, seed)

def _draw_circuit_tree():
    pass
#    circuit_tree_draw()

# Does not use TTL!! Can't cook grenades without TTL set
def _create_grenade(float x, float y, float z, float vx, float vy, float vz, int ttl):
    cdef Grenade* grenade
    grenade = grenade_list.create(x,y,z, vx,vy,vz)
    if grenade is not NULL:
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
