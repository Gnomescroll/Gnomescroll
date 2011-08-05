import vox_lib

vox_loader = vox_lib.Vox_loader()

class VoxRender:

    def __init__(self, model):
        self.vox_model_file = model
        self.vox = vox_loader.load(model)
        self.vox.set_object(self)

    def update_vox(self):
        x,y,z = self.pos()
        self.vox.move(x,y,z, 0)

    def draw_vox(self):
        self.update_vox()
        self.vox.draw()

class GameObjectRender(VoxRender):

    def __init__(self, model=None):
        if model is not None:
            VoxRender.__init__(self, model)
            self._draw = True
        else:
            self.vox = None
            self._draw = False
            
    def draw(self):
        if self._draw and self.vox is not None:
            self.update_vox()
            self.draw_vox()

    def update(self):
        self.update_vox()
