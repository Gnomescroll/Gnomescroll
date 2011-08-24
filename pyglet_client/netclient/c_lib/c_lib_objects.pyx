cdef extern from "./objects/grenade.h":
    void grenade_tick()
    void grenade_draw()
    void create_grenade(int type, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/neutron.h":
    void neutron_tick()
    void neutron_draw()
    void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/agent.h":
    void agent_tick()
    void agent_draw()
    int create_agent(float x, float y, float z)
    void set_agent_state(int id, float xangle, float yangle)

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

def _create_agent(float x, float y, float z):
    id = create_agent(x,y,z)
    return id

def _set_agent_state(int id, float xangle, float yangle):
    set_agent_state(id, xangle, yangle)
