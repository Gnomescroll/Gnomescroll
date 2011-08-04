import vox

object_names = {
    1   :   'Flag',
    2   :   'Base',
}
    

class GameObject:

    def __init__(self, id):
        self.id = id
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

    def update_info(self, **obj):
        old_id = self.id
        if 'id' in obj:
            self.id = obj['id']
        if 'on_ground' in obj:
            self.on_ground = bool(obj['on_ground'])
        if 'state' in obj:
            self.state = obj['state']
        if 'pos' in obj:
            self.state[0:3] = obj['pos']

        GameStateGlobal.itemList.update(self, old_id)


class StaticObject(GameObject):

    def __init__(self, id):
        GameObject.__init__(self, id)
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

    def __init__(self, id):
        GameObject.__init__(self, id)


# pick up / drop
class DetachableObject(GameObject, GameObjectRender):
    
    def __init__(self, id, radius=1, model=None):
        GameObject.__init__(self, id)
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
