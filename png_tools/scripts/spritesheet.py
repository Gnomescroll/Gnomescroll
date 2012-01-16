'''
Spritesheet generator class
'''

import sys
import os
sys.path.insert(0, './../conf/')

import png
import sys
import os.path

class Spritesheet(object):

    '''
        Use example:
        Spritesheet('particles', './particle/', particles_conf).create()
    '''

    def __init__(self, name, source_texture_path, sprites):
        self.name = name
        self.sources = source_texture_path
        if self.sources[-1] != '/':
            self.sources += '/'
        if not os.path.exists(self.sources):
            print 'Texture sources path not found :: %s' % (self.sources,)
            sys.exit()
        self.sprites = sprites
        self._set_empty_pixels()

    def verify(self):

        def warn(f, i, msg=''):
            print 'WARNING: %s [ %s :: %d ]' % (msg, f, i)
            return False

        passed = True
        ids = []
        for fn, i in self.sprites:
            i = int(i)
            if i in ids:
                passed = warn(fn, i, 'id already in use')
            ids.append(i)

            if not os.path.exists(self.sources + fn):
                passed = warn(fn, i, 'file does not exist')

        if not passed:
            print 'Spritesheet generation failed. Please fix errors in sprites configuration'
            sys.exit()

        return passed

    def _set_empty_pixels(self):
        rows = []
        i2=0
        for i in range(0,512):
            rows.insert(i,[])
            rows[i] = [0,0,0,0] * 512
        self.pixels = rows

    def generate(self):

        def _offset(id):
            x = id % 16
            y = (id - x) / 16
            return (x,y)

        for pngfile, id in self.sprites:
            print "Processing Particle: %s, %i" % (pngfile, id)
            x = png.Reader(filename = self.sources + pngfile)

            width, height, pixels, meta = x.asRGBA8()
            if width != 32 or height != 32:
                print "%s, width,height= %i, %i" % (pngfile, width, height)
                print "Height, Width must be 32,32!"
                print 'Aborting'
                sys.exit()

            xi,yi = _offset(id)
            i = 0
            j= 0
            jr = range(32)
            for row in pixels:
                for j in jr:
                    r = row[4*j+0]
                    g = row[4*j+1]
                    b = row[4*j+2]
                    a = row[4*j+3]
                    self.pixels[i+32*yi][4*(j+32*xi)+0] = r
                    self.pixels[i+32*yi][4*(j+32*xi)+1] = g
                    self.pixels[i+32*yi][4*(j+32*xi)+2] = b
                    self.pixels[i+32*yi][4*(j+32*xi)+3] = a
                i += 1
        return self.pixels

    def write_png(self, pixels=None):
        if pixels is None:
            pixels = self.pixels

        png_out = png.Writer(width=512, height=512, alpha=True, bitdepth=8, transparent=None)

        print "writing out"
        with open('./%s_01.png' % (self.name,), 'wb') as f:      # binary mode is important
            png_out.write(f, pixels)

        with open('../netclient/media/texture/%s_01.png' % (self.name,), 'wb') as f:      # binary mode is important
            png_out.write(f, pixels)

    def create(self):
        self.verify()
        self.generate()
        self.write_png()

    def write_out(self, filename):
        pixels = self.pixels
        png_out = png.Writer(width=512, height=512, alpha=True, bitdepth=8, transparent=None)
        with open(filename, 'wb') as f:      # binary mode is important
            png_out.write(f, pixels)
