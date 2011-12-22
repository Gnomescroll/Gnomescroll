'''
Dat files
'''

#all__ = ['weapons', 'items', 'projectiles', 'cubes']
all__ = ['weapons', 'items', 'cubes']

#_imports = ['weapon', 'item', 'projectile', 'cube']
_imports = ['weapon', 'item', 'cube']
_prop = 'dat'

import weapon
import item
#import projectile
import cube


def _set():
    #global weapons, items, projectiles, cubes
    global weapons, items, cubes
    weapons = weapon.dat
    items = item.dat
    #projectiles = projectile.dat
    cubes = cube.dat

def _reload():
    print "RELOADING"
    #global weapon, item, projectile, cube
    global weapon, item, cube
    reload(weapon)
    reload(item)
    #reload(projectile)
    reload(cube)

try:
    _init
except NameError:
    _init = None
else:
    _reload()
finally:
    _set()
