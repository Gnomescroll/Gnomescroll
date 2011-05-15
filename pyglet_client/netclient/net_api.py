class NetApiGlobal:
    netOut = None

    def __init__(self):
        NetApiGlobal.netOut = NetOut()
    @classmethod
    def init(self):
        NetApiGlobal.netOut = netOut().init()


class NetOut:

    @classmethod
    def init(self):
        pass
    def __init(self):
        pass
