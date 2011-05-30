from game_objects import GameObject

projectile_dat = {

    1: {
    'speed' : 15,
    'damage' : 20,
#    'splash' : {
#    'radius' : 3,
#    'damage' : 15,
#    'force' : 150,
    },

    2 : {
    'speed' : 15,
    'damage' : 20,
    },


    }

}


class Projectile(GameObject):

    def __init__(self, x, y, z, vx, vy, vz, id): #more args
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
