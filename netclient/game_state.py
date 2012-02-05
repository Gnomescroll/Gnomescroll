class GameStateGlobal:

    agentList = None
    agent = None

    @classmethod
    def init(cls):
        cls.agentList = AgentList()

from agents import AgentList
