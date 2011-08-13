
sprites = [
('star1.png', 0),
('star2.png', 1),
('star3.png', 2),
('star4.png', 3),
('star5.png', 4),
('star6.png', 5),

#laser effects
('laser1.png', 16),
('laser2.png', 17),
('laser3.png', 18),
#calibration
('test1.png', 255),
]

def _offset(id):
    x = id % 16
    y = (id - x) / 16
    return (x,y)

import png
png_out = png.Writer(width=512, height=512, alpha=True, bitdepth=8, transparent=None)

if True:
    print "start row generation"
    rows = []
    i2=0
    for i in range(0,512):
        rows.insert(i,[])
        rows[i] = [0,0,0,0] * 512

    print "works"

## open file for writing
for pngfile, id in sprites:
    print "Processing Particle: %s, %i" % (pngfile, id)
    x = png.Reader(filename = "./particle/"+pngfile)
    #width, height, pixels, meta = x.asDirect()
    width, height, pixels, meta = x.asRGBA8()
    #print "width,height= %i, %i" % (width, height)
    if width != 32 or height != 32:
        print "%s, width,height= %i, %i" % (pngfile, width, height)
        print "Height, Width must be 32,32!"
        assert False
    #print "meta= " + str(meta)
    xi,yi = _offset(id)
    #print "xi, yi= %i, %i" % (xi, yi)
    #for i in range(0,32):
    #    for j in range(0,32):
    i = 0
    j= 0
    for row in pixels:
        for j in range(32):
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
            #rows[i+xi][j+yi] = value
       #     j+=1
        #i+=1

 #           rows[i+xi][j+yi] = pixels[i,j]
#write out
print "writing out"
f = open('./particles_01.png', 'wb')      # binary mode is important
png_out.write(f, rows)

f = open('../netclient/texture/particles_01.png', 'wb')      # binary mode is important
png_out.write(f, rows)

if False:
    f = open('particle_sheet.png', 'wb')      # binary mode is important
    w = png.Writer(512, 512, greyscale=True)
    w.write(f, [range(256)])
    f.close()

