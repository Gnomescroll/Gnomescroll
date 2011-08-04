from vox import GameObjectRender

object_names = {
    1   :   'Flag',
    2   :   'Base',
}
    

class GameObject:

    def __init__(self, id, state=None):
        self.id = id
        self.on_ground = True
        self.state = state

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
            if self.on_ground:
                self.owner = None
        if 'state' in obj:
            self.state = obj['state']
        if 'pos' in obj:
            self.state[0:3] = obj['pos']
        if 'owner' in obj:
            owner_id = obj['owner']
            owner = GameStateGlobal.agentList[owner_id]
            if owner is None:
                print 'Attempted to assign item to unknown agent owner %d' % (owner_id,)
            self.owner = owner

        GameStateGlobal.itemList.update(self, old_id)

    def take(self, new_owner):
        return False
    def drop(self):
        return False

class StaticObject(GameObject):

    def __init__(self, id, state=None):
        GameObject.__init__(self, id, state)
        self.immobile = True
        
    def pos(self):
        return GameObject.pos(self)
    

class EquippableObject(GameObject):

    def __init__(self, id, state=None):
        GameObject.__init__(self, id, state)


# pick up / drop
class DetachableObject(GameObject, GameObjectRender):
    
    def __init__(self, id, radius=1, state=None, model=None):
        GameObject.__init__(self, id, state)
        GameObjectRender.__init__(self, model)
        self.radius = radius

        self.auto_grab = False
        self.drop_on_death = True

    def take(self, new_owner):
        # ground -> owner
        self.owner = new_owner
        if self.owner is not None:
            self.on_ground = False
        return True
        
    def drop(self):
        self.pos(self.owner.pos())
        self.owner = None
        self.on_ground = True
        return True
