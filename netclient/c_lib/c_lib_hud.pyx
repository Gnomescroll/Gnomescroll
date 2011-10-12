
'''
HUD Cube Selector
'''

cdef extern from "./hud/cube_select.hpp":
    void _init_cube_select()
    int _get_selected_cube_id()
    void _cube_select_set_hud(int pos, int cube_id, int tex_id)
    void _draw_cube_selector(float x, float y, float size, int mode)
    void _hud_control_input(int pos)

#returns the id of the cube which is selected in hud
def get_selected_cube_id():
    return _get_selected_cube_id()

#sets the selected hud position, from 0 to 63
def hud_control_input(int pos):
    _hud_control_input(pos)

def cube_select_set_hud(int pos, int cube_id, int tex_id):
    _cube_select_set_hud(pos, cube_id, tex_id)

def draw_cube_selector(float x, float y, float size=1, int mode=0):
    _draw_cube_selector(x, y, size, mode)


'''
HUD textures
'''

'''
Inventory
'''

cdef extern from "./hud/inventory.hpp":
    int draw_inventory(float x, float y)

def _draw_inventory(float x, float y):
    draw_inventory(x,y)

'''
HUD global
'''

def init_hud():
    _init_cube_select()