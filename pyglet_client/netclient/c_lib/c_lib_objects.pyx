cdef extern from "./objects/grenade.h":
    void grenade_tick()
    void grenade_draw()
    void create_grenade(int type, float x, float y, float z, float vx, float vy, float vz)

def tick():
    grenade_tick()

def draw():
    grenade_draw()

def tick():
    grenade_tick()

def _create_grenade(float x, float y, float z, float vx, float vy, float vz):
    create_grenade(1, x,y,z, vx,vy,vz)

