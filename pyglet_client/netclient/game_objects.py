from vox import GameObjectRender

object_names = {
    0   :   'Generic',
    1   :   'Flag',
    2   :   'Base',
}
    

class GameObject:

    def __init__(self, id, state=None):
        self.id = id
        self.on_ground = True
        self.state = state
        self.type = 0
        self.owner = None

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
        old_owner = self.owner
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
            if owner_id == 0:
                owner = None
            else:
                owner = GameStateGlobal.agentList[owner_id]
                if owner is None:
                    print 'Attempted to assign item to unknown agent owner %d' % (owner_id,)
            self.owner = owner

        self.notify_owners(old_owner)

        GameStateGlobal.itemList.update(self, old_id)

    # updates agent inventories if ownership changes
    def notify_owners(self, old):
        if old == self.owner:
            return
        if old is None: # ground -> agent
            self.owner.inventory._add(self)
        else:   # agent ->
            if self.owner is None:  # ground
                old.inventory._drop(self)
            else:   # different agent
                old.inventory._drop(self)
                self.owner.inventory._add(self)

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
class DetachableObject(GameObject):
    
    def __init__(self, id, radius=1, state=None):
        GameObject.__init__(self, id, state)
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

    def pos(self, xyz=None):
        if self.owner is None:
            return GameObject.pos(self, xyz)
        else:
            if xyz is not None:
                print 'WARNING trying to set item position while item is possessed'
            return self.owner.pos()

class TeamItem:

    def __init__(self, team, own=True, other=True):
        self.team = team
        self.pickup_by_own_team = own
        self.pickup_by_other_team = other

    def can_take(self, new_owner):
        #print new_owner.team, self.team
        return (new_owner.team == self.team and self.pickup_by_own_team) or \
                (new_owner.team != self.team and self.pickup_by_other_team)
                

from game_state import GameStateGlobal
