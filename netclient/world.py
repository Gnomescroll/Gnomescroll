import opts
opts = opts.opts

import c_lib.c_lib_agents as cAgents

from profiler import P

class World():

    def __init__(self):
        pass
        
    def tick(self):
        if GameStateGlobal.agent is not None:
            #GameStateGlobal.agent.nearby_objects()
            GameStateGlobal.agent.update_sound()

    def draw(self):
        P.event("World.draw(), draw agents")
        if opts.draw_agents:
            cAgents.draw_agents()

from game_state import GameStateGlobal
