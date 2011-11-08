
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
## Texture Loader ##
cdef extern from "./SDL/SDL.h":
    struct SDL_Surface:
        int w
        int h

cdef extern from "./hud/texture_loader.h":
    int _init_image_loader()
    SDL_Surface* _load_image(char *file)
    int _create_block_texture(char *file) #deprecate
    int _create_texture(SDL_Surface* surface)

cdef SDL_Surface* load_image(char* file):
    cdef SDL_Surface* surface
    surface = _load_image(file)
    return surface

cdef create_texture(SDL_Surface* surface, type =None): #eventually support mippapped textures
    return _create_texture(surface)



'''
Inventory
'''

cdef extern from "./hud/inventory.hpp":
    int draw_inventory(float x, float y)
    int toggle_inventory_hud()

def _draw_inventory(float x, float y):
    draw_inventory(x,y)

def _toggle_inventory_hud():
    print 'cython inventory hud toggle'
    if toggle_inventory_hud():
        print 'inventory hud on'
    else:
        print 'inventory hud off'

'''
HUD global
'''

def init_hud():
    _init_cube_select()

"""
HUD.pyx
"""

cdef extern from './hud/texture_loader.h':
    void _draw_loaded_hud_texture(int x, int y)
    void _load_hud_texture(char *file)



cdef extern from './hud/text.h':
    int _init_text()
    SDL_Surface* _create_text_surface(char* text,r,g,b,a)
    int _draw_text_surface(SDL_Surface* surface, int x, int y)
    int _free_text_surface(SDL_Surface* surface)
    int _draw_text(char* text, float x, float y, float height, float width, float depth, int r, int g, int b, int a)

# update this
cdef extern from './SDL/draw_functions.h':
    int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z)

cdef extern from './hud/block_selector.h':
    void _draw_block_selector(int x, int y)
    void _load_block_selector_texture(char *file, int scale)

def create_text_surface(string):
    pass

def draw_loaded_hud_texture(x, y):
    _draw_loaded_hud_texture(x, y)

def load_hud_texture(file):
    _load_hud_texture(file)

def draw_block_selector(x, y):
    _draw_block_selector(x, y)

def load_block_selector_texture(file, scale=1):
    _load_block_selector_texture(file, scale)

class Text:

    def __init__(self, text='', x=0, y=0, color=(255,255,255,255)):
        self.height = 10
        self.width = 10
        self.depth = -0.1
        if len(color) == 4:
            self.color = color
        else:
            self.color = list(color)
            self.color.append(255) # default alpha

        self.text = text
        self.x = x
        self.y = y

    def draw(self):
        r,g,b,a  = self.color
        _draw_text(self.text, self.x, self.y, self.height, self.width, self.depth, r,g,b,a)

class Reticle:

    def __init__(self, file, x=0, y=0):
        self.x = x
        self.y = y
        load_hud_texture(file)

    def draw(self):
        draw_loaded_hud_texture(self.x, self.y)

class BlockSelector:

    def __init__(self, file, x=0, y=0, scale=1):
        self.x = x
        self.y = y
        self.scale = scale
        load_block_selector_texture(file, scale)

    def draw(self):
        draw_block_selector(self.x, self.y)

cdef class Texture:
    cdef int id
    cdef int w
    cdef int h
    cdef SDL_Surface* surface

    def __init__(Texture self, char * file):
        self.surface = load_image(file)
        self.w = self.surface.w
        self.h = self.surface.h
        self.id = _create_texture(self.surface)

    def draw(self, x0, y0, x1, y1, z=-0.5):
        _blit_sprite(self.id, x0, y0, x1, y1, z)

class Textures:
    hud_tex = None

    def init(self):
        print "Initing Textures"
        self.hud_tex = Texture("./media/texture/target.png")
