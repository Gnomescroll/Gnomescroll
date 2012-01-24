
cdef extern from "./animations/animations.hpp" namespace "Animations":
    void block_crumble(float x, float y, float z, int n, int cube_id)
    void agent_bleed(float x, float y, float z)

cdef extern from "./export.hpp":
    void animation_tick()
    void animation_draw()

def block_crumbles(pos, int n, int cube_id=1):
    cdef float x,y,z
    x,y,z = pos
    block_crumble(x,y,z, n, cube_id)

def AnimationTick():
    animation_tick()

def AnimationDraw():
    animation_draw()

def bleed(float x, float y, float z):
    agent_bleed(x,y,z)

