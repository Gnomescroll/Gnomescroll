import pyglet
from pyglet.gl import *

from cube_dat import CubeGlobals

from player import Player

base_dir = "./"

import random

class World():

    terrainMap = None
    mapChunkManager = None

    def init(self):
        CubeGlobals.setTextureGrid(self.texture_grid)
        self.terrainMap = CubeGlobals.terrainMap
        self.mapChunkManager = CubeGlobals.mapChunkManager
        CubeGlobals.setTextureGrid(self.texture_grid)

    def __init__(self):
        tile_image = pyglet.image.load(base_dir + 'texture/textures_01.png')
        tile_image_grid = pyglet.image.ImageGrid(tile_image, 16, 16)
        tile_texture_grid = pyglet.image.TextureGrid(tile_image_grid)
        self.texture_grid = tile_texture_grid
        self.texture_grid_mipmap = tile_image.get_mipmapped_texture()

        self.players = []
        self.mipmap = 6

        self.init()

    def toggle_mipmap(self):
        self.mipmap = (self.mipmap+1) % 7
        print "mipmap= %i" % self.mipmap

    def tick(self):
        self.mapChunkManager.update_chunk()
        pass

    def draw(self):
        if False:
            for x in range(-20, 20):
                self.draw_point(x, 0, 0, 255, 0)
            for y in range(-20, 20):
                self.draw_point(0, y, 0, 0, 255)

        self.draw_chunk()
        self.draw_players()

    def test_chunk(self):
        if False:
            self.test_chunk2()
            return

        print "Start chunk generation"
        x_max = 64
        y_max = 64
        z_max = 64
        for xa in range(0, x_max):
            for ya in range(0, y_max):
                for za in range(0, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 16:
                        rnd2 = random.randint(1,4)
                        self.terrainMap.set(xa,ya,za, rnd2)
                        self.mapChunkManager.set_map(xa,ya,za)
        print "Finished chunk generation"

    def test_chunk2(self):
        print "Start chunk generation"
        x_max = 16
        y = 5
        z = 5
        for xa in range(0, x_max):
            rnd2 = random.randint(0,4)
            self.terrainMap.set(xa, y, z, 1)
            self.mapChunkManager.set_map(xa, y, z)
        print "Finished chunk generation"

    def draw_chunk(self):

#        for mct in self.mct_array.values(): #update only one buffer per frame
#            if mct.update == True:
#                mct.update_vertex_buffer(self.batch)
#                break


        #gluBuild2DMipmaps(GL_TEXTURE_2D, depth, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        #err = gluBuild2DMipmaps(GL_TEXTURE_2D,GLU_RGBA,  2048, 2048, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        #print "err= " + str(err)

        #glShadeModel(GL_FLAT);glShadeModel(GL_SMOOTH);
        glEnable(GL_CULL_FACE);
        glEnable(self.texture_grid.target)

        _mipmap = self.mipmap
        if _mipmap == 0:
            glBindTexture(self.texture_grid.target, self.texture_grid.id)
        elif _mipmap == 1:
            glBindTexture(self.texture_grid_mipmap.target, self.texture_grid_mipmap.id)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        elif _mipmap == 2:
            glBindTexture(self.texture_grid_mipmap.target, self.texture_grid_mipmap.id)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        elif _mipmap == 3:
            glBindTexture(self.texture_grid_mipmap.target, self.texture_grid_mipmap.id)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        elif _mipmap == 4:
            glBindTexture(self.texture_grid_mipmap.target, self.texture_grid_mipmap.id)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        elif _mipmap == 5:
            glBindTexture(self.texture_grid_mipmap.target, self.texture_grid_mipmap.id)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        elif _mipmap == 6:
            glBindTexture(self.texture_grid_mipmap.target, self.texture_grid_mipmap.id)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        #if _mipmap:
            #glBindTexture(self.texture_grid_mipmap.target, self.texture_grid_mipmap.id)
            #glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        #else:
            #glBindTexture(self.texture_grid.target, self.texture_grid.id)

        #for mct in self.mct_array.values():
        #    if mct.empty == False:
        #        mct.vertexList.draw(pyglet.gl.GL_QUADS)

        ##self.batch.draw()

        self.mapChunkManager.draw_batch.draw()

        #glDisable(GL_SMOOTH)
        glDisable(GL_CULL_FACE)
        glDisable(self.texture_grid.target)
        #self.draw_players()

    def draw_point(self, x, y, r, g, b):
        z=0
        pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
            ('v3f', (x, y, z)),
            ('c3B', (r, g, b)) )

    def add_player(self, player =None):
        if Player == None:
            self.players.append(Player())
        else:
            self.players.append(player)

    def draw_players(self):
        for p in self.players:
            p.draw()
