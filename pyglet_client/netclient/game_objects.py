class GameObject:

    def __init__(self):
        pass

class EquippableObject(GameObject):

    def __init__(self):
        GameObject.__init__(self)


# pick up / drop
class DetachableObject(GameObject):
    
    def __init__(self):
        GameObject.__init__(self)

    def take(self, new_owner):
        # ground -> owner
        self.owner = new_owner
        
    def drop(self):
        self.pos(self.owner.pos())
        self.owner = None
