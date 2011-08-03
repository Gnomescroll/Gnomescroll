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
