import opts
opts = opts.opts

class GameStateGlobal:

    agentList = None

    def __init__(self):
        GameStateGlobal.agentList = AgentList()


from agents import AgentList
