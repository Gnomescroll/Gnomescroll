import opts
opts = opts.opts

import random
import c_lib.c_lib_agents as cAgents

from profiler import P

base_dir = "./"

class World():

    terrainMap = None
    mapChunkManager = None

    def init(self):
        pass


    def __init__(self):
        self.players = []
        self.agents = []
        self.mipmap = 6
        self.gl_smooth = 0

        self.init()

    def tick(self):
        for p in GameStateGlobal.projectileList.values():
            p.tick()

        if GameStateGlobal.agent is not None:
            GameStateGlobal.agent.nearby_objects()
            GameStateGlobal.agent.update_sound()

    def draw(self, first_person=False):
        P.event("World.draw(), draw agents")
        self.draw_agents(first_person)
        P.event("World.draw(), draw projectiles")
        self.draw_projectiles()
        P.event("World.draw(), draw items")
        self.draw_items()

    #deprecate
    def draw_transparent_blocks(self):
        pass
    def toggle_mipmap(self):
        print "Input.py line 272: deprecated, use for other graphics options"
    def toggle_gl_smooth(self):
        print "Input.py line 272: deprecated, use for other graphics options"

    def draw_projectiles(self):
        for proj in GameStateGlobal.projectileList.values():
            proj.draw()

    def draw_items(self):
        pass
        #print "num items= %i" % (len(GameStateGlobal.itemList.values()))
        #for item in GameStateGlobal.itemList.values():
            #if item.on_ground:
                #item.draw()

    #deprecate
    def draw_chunk(self):
        pass

    def add_player(self, player):
        self.players.append(player)

    def add_agent(self, agent=None):
        self.agents.append(agent)

    def draw_agents(self, first_person=False):
        
        if opts.draw_agents:
            cAgents.draw_agents()

import projectiles

#from cube_dat import CubeGlobal
from game_state import GameStateGlobal
#from map_chunk_manager import MapChunkManagerGlobal

from players import Player
