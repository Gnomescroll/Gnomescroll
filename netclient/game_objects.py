from dat_loader import i_dat

class GameObject(object):

    _types = {
        'GameObject'    :   0,
        'StaticObject'  :   1,
        'EquippableObject': 2,
        'DetachableObject': 3,
        'Flag'          :   4,
        'Base'          :   5,
    }

    _names = None
    
    dat = i_dat

    def __init__(self, id, state=None, **kwargs):
        self.id = id
        self.on_ground = True
        if 'on_ground' in kwargs:
            self.on_ground = bool(kwargs['on_ground'])
        self.state = state
        self.owner = None
        self._set_type()
        self.dat.apply(self)

    def _set_type(self):
        self.type = self._types[self.__class__.__name__]

    @classmethod
    def name_from_type(cls, type):
        if cls._names is None:
            rev = [(b,a) for a,b in cls._types.items()]
            cls._names = dict(rev)
        return cls._names[type]
    
    def pos(self, xyz=None):
        if xyz is not None:
            self.state[0:3] = xyz
        else:
            return self.state[0:3]

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
        print 'notify owners %s %s' % (old, self.owner,)
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

    def __init__(self, id, state=None, **kwargs):
        GameObject.__init__(self, id, state, **kwargs)
        
    def pos(self):
        return GameObject.pos(self)
    

class EquippableObject(GameObject):

    def __init__(self, id, state=None):
        GameObject.__init__(self, id, state)


# pick up / drop
class DetachableObject(GameObject):
    
    def __init__(self, id, state=None, **kwargs):
        GameObject.__init__(self, id, state, **kwargs)

    def take(self, new_owner):
        # ground -> owner
        self.owner = new_owner
        if self.owner is not None:
            self.on_ground = False
        return True

    def can_take(self, new_owner):
        if self.on_ground and self.owner != new_owner:
            return True
        return False

    def can_drop(self, owner):
        if owner is not None and self.owner == owner:
            return True
        return False
        
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

class TeamItem(object):

    def __init__(self, team, **kwargs):
        self.team = team

    def __getattribute__(self, k):
        v = object.__getattribute__(self, k)
        if k == 'team' and v is not None:
            v = GameStateGlobal.teamList[v]
        return v

    def can_take(self, new_owner):
        return (new_owner.team == self.team and self.pickup_by_own_team) or \
                (new_owner.team != self.team and self.pickup_by_other_team)
                

from game_state import GameStateGlobal
