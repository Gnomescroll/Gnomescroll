from game_objects import GameObject


class Projectile(GameObject):

    def __init__(self, x, y, z): #more args
        pass

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
            pass
        except AssertionError:
            print 'projectile update :: state is wrong length'
            return
