import opts
opts = opts.opts

class GameStateGlobal:

    agentList = None
    agent = None

    @classmethod
    def init(cls):
        cls.agentList = AgentList()
        cls.exit = False

from agents import AgentList
