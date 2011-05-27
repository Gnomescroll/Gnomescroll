class Player:

    def __init__(self, id=None, kills=0, deaths=0, agent=None, **args):

        self.id = id
        self.kills = kills
        self.deaths = deaths
        self.agent = agent


    def tick(self):
        pass

    def update_info(self, player):
        if 'id' in player:
            self.id = player['id']
        if 'kills' in player:
            self.kills = player['kills']
        if 'deaths' in player:
            self.deaths = player['deaths']
