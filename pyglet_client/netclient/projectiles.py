from game_objects import GameObject

projectile_dat = {

    1: {
    'speed' : 15,
    'damage' : 20,
    'ttl_max' : 400 #time to live in ticks
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

    def __init__(self, x, y, z, x_angle, y_angle, id): #more args
        global projectile_dat
        assert projectile_dat.has_key(id)
        p = projectile_dat[id]
        #load projectile settings

        vx = cos( self.x_angle * pi) * cos( self.y_angle * pi)
        vy = sin( self.x_angle * pi) * cos( self.y_angle * pi)

        self.state = [x,y,z,vx,vy,vz]
        self.speed = p.speed
        self.damage = p.damage
        self.ttl = 0
        self.ttl_max = p.ttl_max
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
    #run this once per frame for each projectile
    def tick(self):
        self.state = [x,y,z,vx,vy,vz]

        fps = 60. # frame per second
        speed = self.speed / fps

        self.ttl += 1
        if self.ttl > self.ttl_max:
            self.delete()

    def delete(self):
        pass

def draw_projectile(self):
    pass
