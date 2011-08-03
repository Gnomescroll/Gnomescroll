import vox

object_names = {
    1   :   'Flag',
    2   :   'Base',
}
    

class GameObject:

    def __init__(self):
        self.on_ground = True
        self.state = [0 for i in range(9)]

    def pos(self, xyz=None):
        if xyz is not None:
            self.state[0:3] = xyz
        else:
            return self.state[0:3]

    @classmethod
    def name_from_type(self, type):
        return object_names[type]


class StaticObject(GameObject):

    def __init__(self):
        GameObject.__init__(self)
        self.immobile = True
        
    def pos(self):
        return GameObject.pos(self)
    

class GameObjectRender(vox.VoxRender):

    def __init__(self, model=None):
        if model is not None:
            vox.VoxRender.__init__(self, model)
            self.draw = True
        else:
            self.vox = None
            self.draw = False
            
    def draw(self):
        if self.draw and self.vox is not None:
            self.update_vox()
            self.draw_vox()

    def update(self):
        self.update_vox()


class EquippableObject(GameObject):

    def __init__(self):
        GameObject.__init__(self)


# pick up / drop
class DetachableObject(GameObject, GameObjectRender):
    
    def __init__(self, radius=1, model=None):
        GameObject.__init__(self)
        GameObjectRender.__init__(self, model)
        self.radius = radius

        self.auto_grab = False
        self.drop_on_death = True

    def take(self, new_owner):
        # ground -> owner
        self.owner = new_owner
        if self.owner is not None:
            self.on_ground = False
        
    def drop(self):
        self.pos(self.owner.pos())
        self.owner = None
        self.on_ground = True
