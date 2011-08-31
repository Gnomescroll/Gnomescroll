cdef extern from "./objects/grenade.h":
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


cdef extern from "./agent/agent.h":
    void agent_tick()
    void agent_draw()
    int create_agent(float x, float y, float z)
    void set_agent_state(int id, float xangle, float yangle)

cdef extern from "./agent/agent_vox.h":
    void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize)
    void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a)

    void set_agent_limb_direction(int id, int part, float fx,float fy,float fz, float nx,float ny, float nz)
    void set_agent_limb_anchor_point(int id, int part, float length, float ax,float ay,float az)

cdef extern from "./agent/agent_draw.h":
    void draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle)
    void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height)
    void draw_agent_cube_selection(int x, int y, int z, int r, int g, int b)
    void draw_agent_cube_side_selection(int x, int y, int z, int cx, int cy, int cz, int r, int g, int b)

from dat.agent_dim import lu1, lu2, lu3, vosize, skel_tick
agent_list = []


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

def tick():
    grenade_tick()
    neutron_tick()
    cspray_tick()
    blood_tick()
    agent_skeleton_update()
    agent_tick()

def draw():
    grenade_draw()
    neutron_draw()
    cspray_draw()
    blood_draw()
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


#agent

def default_vox_model_init(int id, int part, int xdim, int ydim, int zdim, float vosize):
    init_agent_vox_volume(id, part, xdim,ydim,zdim, vosize)
    for x in range(0,xdim):
        for y in range(0,ydim):
            for z in range(0,zdim):
                a = 255
                r = 32*x
                g = 32*y
                b = 32*z
                set_agent_vox_volume(id, part, x,y,z, r,g,b,a)

def _set_agent_model(int id):
    #cdef float vosize = .0625
    cdef int part
    cdef int xdim, ydim, zdim

    global lu1, lu2, lu3, vosize

    for part in range(0,6):
        xdim,ydim,zdim = lu1[part]
        default_vox_model_init(id, part, xdim,ydim,zdim, vosize)

    for part in range(0,6):
        length, ax,ay,az= lu2[part]
        set_agent_limb_anchor_point(id, part, length,ax,ay,az)
    for part in range(0,6):
        fx,fy,fz, nx,ny,nz = lu3[part]
        set_agent_limb_direction(id, part, fx, fy, fz, nx,ny,nz)

def _create_agent(float x, float y, float z):
    cdef int id
    id = create_agent(x,y,z)
    _set_agent_model(id)
    ##
    global agent_list
    agent_list.append(id)
    return id

def _set_agent_state(int id, float xangle, float yangle):
    set_agent_state(id, xangle, yangle)


def _draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle):
    draw_agent_aiming_direction(x, y, z, xangle, yangle)

def _draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height):
    draw_agent_bounding_box(x, y, z, radius, head_height, height)

def _draw_agent_cube_selection(int x, int y, int z, int r, int g, int b):
    draw_agent_cube_selection(x,y,z, r,g,b)

def _draw_agent_cube_side_selection(int x, int y, int z, int cx, int cy, int cz, int r, int g, int b):
    draw_agent_cube_side_selection( x,  y,  z,  cx,  cy,  cz,  r,  g,  b)
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
