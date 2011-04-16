from math import pi
from pyglet import clock
from pyglet.gl import (
    glBlendFunc, glClear, glClearColor, glEnable, glPopMatrix,
    glPushMatrix, glRotatef, glTranslatef,
    GL_BLEND, GL_COLOR_BUFFER_BIT, GL_LINE_SMOOTH, GL_ONE_MINUS_SRC_ALPHA,
    GL_SRC_ALPHA,
)

rad2deg = 180 / pi

class Renderer(object):

    def __init__(self):
        glClearColor(0.1, 0.2, 0.5, 0.0)
        glEnable(GL_LINE_SMOOTH)
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    def on_draw(self, camera, win_width, win_height):
        glClear(GL_COLOR_BUFFER_BIT)
        camera.update_position()
        camera.focus()

        #maze.shape.get_batch().draw()

#        for creature in maze.creatures:
#            glPushMatrix()
#            glTranslatef(creature.x, creature.y, 0)
#            glRotatef(creature.angle * rad2deg, 0, 0, 1)
#            creature.shape.get_batch().draw()
#            glPopMatrix()

        camera.hud_mode()
