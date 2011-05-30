class GameObject:

    def __init__(self):
        pass

    def json(self):
        return {
            'id'    :   self.id,
        }

class EquippableObject(GameObject):

    def __init__(self):
        pass
