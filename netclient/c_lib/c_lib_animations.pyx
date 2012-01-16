
cdef extern from "./animations/animations.hpp" namespace "Animations":
    void block_crumble(float x, float y, float z, int n)

cdef extern from "./export.hpp":
    void animation_tick()
    void animation_draw()

def block_crumble(pos, int n):
    cdef float x,y,z
    x,y,z = pos
    block_crumble(x,y,z, n)

def AnimationTick():
    animation_tick()

def AnimationDraw():
    animation_draw()
