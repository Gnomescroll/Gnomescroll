from game_objects import GameObject


class Projectile(GameObject):

    def __init__(self, id, x, y, z, vx, vy, vz): #more args
        self.id = id
        self.state = [x, y, z, vx, vy, vz]

    def update(self, **args):
        try:
            state = args['state']
            state = list(state)
            assert len(state) == 6
            self.state = state
        except KeyError:
            pass
        except TypeError:
            print 'projectile update :: state is not iterable'
        except AssertionError:
            print 'projectile update :: state is wrong length'
