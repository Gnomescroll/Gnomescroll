import pyglet
from pyglet.gl import *

#from dat import CubeProperties, CubeRenderCache, TerrainMap, MapChunkManager, MapChunk
#from dat import convert_index

from cube_dat import CubeGlobals, MapChunkManager

from player import Player

base_dir = "./"

import random

class World():

    def __init__(self):
        self.batch = pyglet.graphics.Batch()
        #texture loading
        tile_image = pyglet.image.load(base_dir + 'texture/textures_01.png')
        tile_image_grid = pyglet.image.ImageGrid(tile_image, 16, 16)
        tile_texture_grid = pyglet.image.TextureGrid(tile_image_grid)
        self.texture_grid = tile_texture_grid

        self.texture_grid_mipmap = tile_image.get_mipmapped_texture()

        ##self.cubeProperties = CubeProperties()
        ##self.cubeRenderCache = CubeRenderCache(self.cubeProperties, self.texture_grid) #needs texture grid
        #test
        ##self.terrainMap = TerrainMap()
        #MapChunkManager(terrainMap, cubeProperties)

        CubeGlobals.textureGrid = self.textureGrid #setup CubeGlobals

        self.terrainMap = CubeGlobals.terrainMap
        self.players = []

        self.mipmap = 6

    def toggle_mipmap(self):
        self.mipmap = (self.mipmap+1) % 7
        print "mipmap= %i" % self.mipmap
    def tick(self):
        pass

##DEPRECATED
    def DEPRECATED_test_chunk(self):
        print "Start chunk generation"
        self.mct_array = {}
        for xa in range(0, 8):
            for ya in range(0, 8):
                self.mct_array[(xa,ya)] = MapChunk(8*xa,8*ya)
                for x in range(0, 8):
                    for y in range(0, 8):
                        for z in range(0, 8):
                            rnd = random.randint(0,64)
                            #rnd = 3
                            if rnd < 16:
                                rnd = rnd = random.randint(0,4)
                                self.mct_array[(xa,ya)].set_tile(x,y,z,rnd)
##DEPRECATED

    def test_chunk(self):
        print "Start chunk generation"
        x_max = 1024
        y_max = 1024
        z_max = 32
        for xa in range(0, x_max):
            for ya in range(0, y_max):
                for za in range(0, z_max):
                    rnd = random.randint(0,64)
                        if rnd < 16:
                            rnd2 = random.randint(0,4)
                                self.terrainMap.set(xa,ya,za, rnd2)

    def draw_chunk(self):
        self.MapChunkManager.update_chunk()

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
        self.batch.draw()

        #glDisable(GL_SMOOTH)
        glDisable(GL_CULL_FACE)
        glDisable(self.texture_grid.target)

    def draw(self):
        if False:
            for x in range(-20, 20):
                self.draw_point(x, 0, 0, 255, 0)
            for y in range(-20, 20):
                self.draw_point(0, y, 0, 0, 255)


        self.draw_chunk()
        self.draw_players()
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
