cdef extern from "./objects/grenade.h":
    void grenade_tick()
    void grenade_draw()
    void create_grenade(int type, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/neutron.h":
    void neutron_tick()
    void neutron_draw()
    void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/cspray.h":
    void cspray_tick()
    void cspray_draw()
    void create_cspray(int type, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./agent/agent.h":
    void agent_tick()
    void agent_draw()
    int create_agent(float x, float y, float z)
    void set_agent_state(int id, float xangle, float yangle)

cdef extern from "./agent/agent_vox.h":
    void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize)
    void set_limb_properties(int id, int part, float length, float ax, float ay, float az)
    void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a)
    void set_agent_box_anchor_point(int id, int part, float ax,float ay,float az)

def tick():
    grenade_tick()
    neutron_tick()
    cspray_tick()
    agent_tick()

def draw():
    grenade_draw()
    neutron_draw()
    cspray_draw()
    agent_draw()

def _create_grenade(float x, float y, float z, float vx, float vy, float vz):
    create_grenade(1, x,y,z, vx,vy,vz)

def _create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz):
    create_neutron(type,energy, x,y,z, vx,vy,vz)

def _create_cspray(int type, float x, float y, float z, float vx, float vy, float vz):
    create_cspray(type, x,y,z, vx,vy,vz)

#agent


def default_vox_model_init(int id, int part, int xdim, int ydim, int zdim, float vosize):
    init_agent_vox_volume(id, part, xdim,ydim,zdim, vosize)
    for x in range(0,8):
        for y in range(0,8):
            for z in range(0,8):
                a = 255;r = 32*x;g = 32*y;b = 32*z
                set_agent_vox_volume(id, 0, x,y,z, r,g,b,a)

def _set_agent_model(int id):
    cdef float vosize = 0.2
    cdef int part
    cdef int xdim, ydim, zdim

    '''
    init_agent_vox_volume(id, 0, 8,8,8, vosize)
    init_agent_vox_volume(id, 1, 16,16,24, vosize)
    init_agent_vox_volume(id, 2, 12,4,4, vosize)
    init_agent_vox_volume(id, 3, 12,4,4, vosize)
    init_agent_vox_volume(id, 4, 12, 4,4, vosize)
    init_agent_vox_volume(id, 5, 12, 4,4, vosize)
    '''

    default_vox_model_init(id, 0, 8,8,8, vosize)
    default_vox_model_init(id, 1, 16,16,24, vosize)
    default_vox_model_init(id, 2, 12,4,4, vosize)
    default_vox_model_init(id, 3, 12,4,4, vosize)
    default_vox_model_init(id, 4, 12, 4,4, vosize)
    default_vox_model_init(id, 5, 12, 4,4, vosize)

    '''
    for x in range(0,8):
        for y in range(0,8):
            for z in range(0,8):
                a = 255;r = 32*x;g = 32*y;b = 32*z
                set_agent_vox_volume(id, 0, x,y,z, r,g,b,a)
    '''

    fx = 1.0
    fy = 0
    fz = 0
    set_agent_box_anchor_point(id, 1, fx, fy, fz)

    lu = [
    [0, 0,0,3.0],
    [0, 0,0,1.5],
    [2, -1.0,0,1.5],
    [2, 1,0,1.5],
    [2, 0,0,1.0],
    [2, 0,0,1.0],
    ]
    for part in range(0,6):
        length, ax,ay,az = lu[part]
        set_limb_properties(id, part, length,ax,ay,az)

    #length, anchor x,y,z
    '''
    set_limb_properties(id, 1, 0, float ax, float ay, float az) #torso
    set_limb_properties(id, 2, 0.0, float ax, float ay, float az) #larm
    set_limb_properties(id, 3, float length, float ax, float ay, float az) #rarm
    set_limb_properties(id, 4, float length, float ax, float ay, float az) #lleg
    set_limb_properties(id, 5, float length, float ax, float ay, float az) #rleg
    '''

def _create_agent(float x, float y, float z):
    cdef int id
    id = create_agent(x,y,z)
    _set_agent_model(id)
    return id

def _set_agent_state(int id, float xangle, float yangle):
    set_agent_state(id, xangle, yangle)

### map gen
'''
cdef extern from "./map_gen/density.h":
    void init_map_gen_density()
    void compute_density_map()
    void map_density_visualize(int density, float min, float max)


def _compute_density_map():
    compute_density_map()

def _map_density_visualize(int density, float min, float max):
    map_density_visualize(density, min, max)
'''
