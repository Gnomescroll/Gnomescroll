cdef extern from "./objects/grenade.h":
    void grenade_tick()
    void grenade_draw()
    void create_grenade(int type, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/neutron.h":
    void neutron_tick()
    void neutron_draw()
    void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./agent/agent.h":
    void agent_tick()
    void agent_draw()
    int create_agent(float x, float y, float z)
    void set_agent_state(int id, float xangle, float yangle)

cdef extern from "./agent/agent_vox.h":
    void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize)
    void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a)
    void set_agent_box_anchor_point(int id, int part, float ax,float ay,float az, float fx,float fy,float fz)

def tick():
    grenade_tick()
    neutron_tick()
    agent_tick()

def draw():
    grenade_draw()
    neutron_draw()
    agent_draw()

def _create_grenade(float x, float y, float z, float vx, float vy, float vz):
    create_grenade(1, x,y,z, vx,vy,vz)

def _create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz):
    create_neutron(type,energy, x,y,z, vx,vy,vz)

#agent

def _set_agent_model(int id):
    cdef float vosize = 0.2
    cdef int part
    cdef int xdim, ydim, zdim
    for part in range(0,6):
        xdim=8;ydim=8;zdim=8;
        init_agent_vox_volume(id, part, xdim, ydim, zdim, vosize)

    for x in range(0,8):
        for y in range(0,8):
            for z in range(0,8):
                a = 255
                r = 32*x
                g = 32*y
                b = 32*z
                #r,g,b = [0,255,0]
                set_agent_vox_volume(id, 0, x,y,z, r,g,b,a)
    ax = 0
    ay = 0
    az = 2.5
    fx = 1.0
    fy = 0
    fz = 0
    set_agent_box_anchor_point(id, 1, ax,ay,az, fx,fy,fz)

def _create_agent(float x, float y, float z):
    cdef int id
    id = create_agent(x,y,z)
    _set_agent_model(id)
    return id

def _set_agent_state(int id, float xangle, float yangle):
    set_agent_state(id, xangle, yangle)

