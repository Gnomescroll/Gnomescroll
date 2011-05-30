'''
Projectiles
'''

from game_state import GenericObjectList

class ProjectileList(GenericObjectList):

    def __init__(self):
        from projectiles import Projectile
        GenericObjectList.__init__(self)
        self._metaname = 'ProjectileList'
        self._itemname = 'Projectile'
        self._object_type = Projectile

    def create(self, *args, **projectile):
        projectile = self._add(*args, **projectile)
        return projectile

    def destroy(self, projectile):
        self._remove(projectile)
        return projectile

from game_objects import GameObject

class Projectile(GameObject):

    def __init__(self, x, y, z): #more args
        pass

    def update(self, **args):
        try:
            state = args['state']
            state = list(state)
            assert len(state) == 6
        except KeyError:
            pass
        except TypeError:
            print 'projectile update :: state is not iterable'
            pass
        except AssertionError:
            print 'projectile update :: state is wrong length'
            return
