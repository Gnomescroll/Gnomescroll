#!/usr/bin/python
import png
import sys
import os.path
from sprites import sprites

def verify_sprite_conf(sprites):

    def warn(f, i, msg=''):
        print 'WARNING: %s [ %s :: %d ]' % (msg, f, i)
        return True

    fail = False
    ids = []
    for fn, i in sprites:
        i = int(i)
        if i in ids:
            fail = warn(fn, i, 'id already in use')
        ids.append(i)

        if not os.path.exists(fn):
            fail = warn(fn, i, 'file does not exist')
    return fail
    
def generate_spritesheet(sprites):

    def _offset(id):
        x = id % 16
        y = (id - x) / 16
        return (x,y)


    print "start row generation"
    rows = []
    i2=0
    for i in range(0,512):
        rows.insert(i,[])
        rows[i] = [0,0,0,0] * 512

    for pngfile, id in sprites:
        print "Processing Particle: %s, %i" % (pngfile, id)
        x = png.Reader(filename = "./particle/"+pngfile)
        #width, height, pixels, meta = x.asDirect()
        width, height, pixels, meta = x.asRGBA8()
        #print "width,height= %i, %i" % (width, height)
        if width != 32 or height != 32:
            print "%s, width,height= %i, %i" % (pngfile, width, height)
            print "Height, Width must be 32,32!"
            print 'Aborting'
            sys.exit()
        #print "meta= " + str(meta)
        xi,yi = _offset(id)
        #print "xi, yi= %i, %i" % (xi, yi)
        i = 0
        j= 0
        jr = range(32)
        for row in pixels:
            for j in jr:
                r = row[4*j+0]
                g = row[4*j+1]
                b = row[4*j+2]
                a = row[4*j+3]
                #print "r,g,b,a= %i,%i,%i,%i" %(r,g,b,a)
                rows[i+32*yi][4*(j+32*xi)+0] = r
                rows[i+32*yi][4*(j+32*xi)+1] = g
                rows[i+32*yi][4*(j+32*xi)+2] = b
                rows[i+32*yi][4*(j+32*xi)+3] = a
            i += 1
    return rows
    
## open file for writing
def write_png(rows):
    png_out = png.Writer(width=512, height=512, alpha=True, bitdepth=8, transparent=None)

    print "writing out"
    with open('./particles_01.png', 'wb') as f:      # binary mode is important
        png_out.write(f, rows)

    with open('../netclient/media/texture/particles_01.png', 'wb') as f:      # binary mode is important
        png_out.write(f, rows)


def main(sprites):
    if not verify_sprite_conf(sprites):
        print 'Spritesheet generation failed. Please fix errors in sprites configuration'
        sys.exit()
    pixels = generate_spritesheet(sprites)
    write_png(pixels)
    
if __name__ == '__main__':
    main(sprites)
    print 'Done'
