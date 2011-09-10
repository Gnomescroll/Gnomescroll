'''
Dat files
'''

all__ = ['weapons', 'items', 'projectiles', 'cubes']

_imports = ['weapon', 'item', 'projectile', 'cube']
_prop = 'dat'

import weapon
import item
import projectile
import cube


def _set():
    global weapons, items, projectiles, cubes
    weapons = weapon.dat
    items = item.dat
    projectiles = projectile.dat
    cubes = cube.dat

def _reload():
    global weapon, item, projectile, cube
    reload(weapon)
    reload(item)
    reload(projectile)
    reload(cube)

try:
    _init
except NameError:
    _init = None
else:
    _reload()
finally:
    _set()
