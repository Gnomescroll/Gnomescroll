import pyglet
from pyglet.gl import *

from dat import CubeProperties, CubeRenderCache, TerrainMap, MapChunkManager, MapChunk
from dat import convert_index

from player import Player

base_dir = "../"

import random

class World():

    def __init__(self):
        self.batch = pyglet.graphics.Batch()
        #texture loading
        tile_image = pyglet.image.load(base_dir + 'texture/textures_01.png')
        tile_image_grid = pyglet.image.ImageGrid(tile_image, 16, 16)
        tile_texture_grid = pyglet.image.TextureGrid(tile_image_grid)
        self.texture_grid = tile_texture_grid

        self.cubeProperties = CubeProperties()
        self.cubeRenderCache = CubeRenderCache(self.cubeProperties, self.texture_grid) #needs texture grid
        #test
        self.terrainMap = TerrainMap()
        #MapChunkManager(terrainMap, cubeProperties)
        self.players = []

    def tick(self):
        pass


    def test_chunk(self):
        MapChunk.cubeProperties = self.cubeProperties
        MapChunk.cubeRenderCache = self.cubeRenderCache

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
                                rnd = rnd = random.randint(0,3)
                                self.mct_array[(xa,ya)].set_tile(x,y,z,rnd)

    def draw_chunk(self):
        for mct in self.mct_array.values(): #update only one buffer per frame
            if mct.update == True:
                mct.update_vertex_buffer(self.batch)
                break

        glEnable(GL_CULL_FACE);
        glEnable(self.texture_grid.target)
        glBindTexture(self.texture_grid.target, self.texture_grid.id)

        #for mct in self.mct_array.values():
        #    if mct.empty == False:
        #        mct.vertexList.draw(pyglet.gl.GL_QUADS)
        self.batch.draw()

        glDisable(GL_CULL_FACE);
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
