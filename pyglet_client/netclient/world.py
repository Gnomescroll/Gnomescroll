import default_settings as settings

if settings.pyglet:
    import pyglet
    from pyglet.gl import *
else:
    import SDL

base_dir = "./"

import random

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
        for a in GameStateGlobal.agentList.values():
            a.tick()
        if GameStateGlobal.agent is not None:
            GameStateGlobal.agent.nearby_objects()

    def draw(self, first_person=False):
        self.draw_agents(first_person)
        self.draw_projectiles()
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
        for item in GameStateGlobal.itemList.values():
            if item.on_ground:
                item.draw()

    #deprecate
    def draw_chunk(self):
        pass

    def add_player(self, player):
        self.players.append(player)

    def add_agent(self, agent=None):
        self.agents.append(agent)

    def draw_agents(self, first_person=False):
        for agent in GameStateGlobal.agentList.values():
            if not agent.dead and not (agent.you and first_person) and \
                (agent.team and not agent.team.is_viewers()):
                agent.draw()

import projectiles

#from cube_dat import CubeGlobal
from game_state import GameStateGlobal
#from map_chunk_manager import MapChunkManagerGlobal

from players import Player
