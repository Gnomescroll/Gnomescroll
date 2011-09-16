cdef extern from "./physics/vector.h":
    struct Vector:
        float x
        float y
        float z

cdef extern from "./objects/grenade.h":
    Vector* _get_grenade_position(int gid)
    void grenade_tick()
    void grenade_draw()
    int create_grenade(int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max)
    void destroy_grenade(int gid)

cdef extern from "./objects/neutron.h":
    void neutron_tick()
    void neutron_draw()
    void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/cspray.h":
    void cspray_tick()
    void cspray_draw()
    void create_cspray(int type, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/blood.h":
    void blood_tick()
    void blood_draw()
    void create_blood(int type, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/shrapnel.h":
    void shrapnel_tick()
    void shrapnel_draw()
    void create_shrapnel(int type, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/circuit_tree.h":
    void circuit_tree_generate(int type, int seed)
    void circuit_tree_draw()

'''
cdef extern from "./agent/agent_physics.h":
    void agent_tick()
'''

cdef extern from "./agent/agent_draw.h":
    void agent_draw() #draw all agents
    void draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle)
    void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height)
    void draw_agent_cube_selection(int x, int y, int z, int r, int g, int b)
    void draw_agent_cube_side_selection(int x, int y, int z, int cx, int cy, int cz, int r, int g, int b)

'''
cdef extern from "./agent/control_state.h":
    void set_agent_control_state(int* cs, float xangle, float yangle)
'''

'''
from dat.agent_dim import lu1, lu2, lu3, vosize, skel_tick
agent_list = []


def _set_agent_control_state(l, xangle,yangle):
    cdef int x[6]
    for i in range(0,6):
        x[i] = int(l[i])
    set_agent_control_state(x, xangle, yangle)

def agent_skeleton_update():
    global agent_list,lu2,lu3
    skel_tick()
    for id in agent_list:
        for part in range(0,6):
            length, ax,ay,az= lu2[part]
            set_agent_limb_anchor_point(id, part, length,ax,ay,az)
        for part in range(0,6):
            fx,fy,fz,nx,ny,nz = lu3[part]
            set_agent_limb_direction(id, part, fx, fy, fz, nx,ny,nz)
'''

def tick():
    grenade_tick()
    neutron_tick()
    cspray_tick()
    blood_tick()
    shrapnel_tick()
    #agent_skeleton_update()
    #agent_tick()

def draw():
    grenade_draw()
    cspray_draw()
    neutron_draw()
    blood_draw()
    shrapnel_draw()
    agent_draw()

def _create_grenade(float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max):
    return create_grenade(1, x,y,z, vx,vy,vz, ttl, ttl_max)

def _destroy_grenade(int gid):
    destroy_grenade(gid)

def _create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz):
    create_neutron(type,energy, x,y,z, vx,vy,vz)

def _create_cspray(int type, float x, float y, float z, float vx, float vy, float vz):
    create_cspray(type, x,y,z, vx,vy,vz)

def _create_blood(int type, float x, float y, float z, float vx, float vy, float vz):
    create_blood(type, x,y,z, vx,vy,vz)

def _create_shrapnel(int type, float x, float y, float z, float vx, float vy, float vz):
    create_shrapnel(type, x,y,z, vx,vy,vz)

def _generate_circuit_tree(int type, int seed):
    circuit_tree_generate(type, seed)

def _draw_circuit_tree():
    circuit_tree_draw()

def get_grenade_position(int gid):
    cdef Vector* pos
    pos = _get_grenade_position(gid)
    p = [pos.x, pos.y, pos.z]
    return p


#def _update_agent(int id, float x, float y, float z):
#    update_agent(id, x,y,z)
