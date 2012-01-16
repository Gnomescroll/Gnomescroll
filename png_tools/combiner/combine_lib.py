
def kernel(blend_mode, r1,g1,b1,a1, r2,g2,b2,a2):
    if blend_mode == 1: #average in together
        r = (r1+r2)/2
        g = (g1+g2)/2
        b = (b1+b2)/2
        a = 0
    if blend_mode == 2: #combine by alpha of 2nd image
        a2 = a2/255
        r = ((1-a2)*r1+a2*r2)
        g = ((1-a2)*g1+a2*g2)
        b = ((1-a2)*b1+a2*b2)
        a= 255

    if blend_mode == 3:
        if r2 <255 or g2 <255 or b2 <255:
            #print "1"
            r=r2
            g=g2
            b=b2
        else:
            r=r1
            g=g1
            b=b1
        a=255
    #print "%i, %i, %i,%i" % (r,g,b,a)
    return [r,g,b,a]

import png

def blend(png1, png2, out_png, blend_mode):
    if True:
        #print "start row generation"
        rows = []
        i2=0
        for i in range(0,32):
            rows.insert(i,[])
            rows[i] = [0,0,0,0] * 32


    def pixel_to_array(pixel, width, height):
        i = []
        _i=0
        for row in pixel:
            i.insert(_i, [])
            j=0
            for j in range(width):
                r = row[4*j+0]
                g = row[4*j+1]
                b = row[4*j+2]
                a = row[4*j+3]
                i[_i].insert(4*j+0,r)
                i[_i].insert(4*j+1,g)
                i[_i].insert(4*j+2,b)
                i[_i].insert(4*j+3,a)
            _i+=1
        return i

    x = png.Reader(filename = png1)

    width1, height1, pixels1, meta1 = x.asRGBA8()
    if width1 != 32 or height1 != 32:
        print "%s, width,height= %i, %i" % (png1, width1, height1)
        print "Height, Width must be 32,32!"
        assert False

    y = png.Reader(filename = png2)

    width2, height2, pixels2, meta2 = y.asRGBA8()

    if width2 != 32 or height2 != 32:
        print "%s, width,height= %i, %i, %s, width,height= %i, %i" % (png1, width1, height1, png2, width2, height2)
        print "Height, Width must be same!"
        assert False

    pixels1 = pixel_to_array(pixels1, width1, height1)
    pixels2 = pixel_to_array(pixels2, width2, height2)

    i = 0
    j= 0
    row = 0

    for row_n in range(0,height1):
        row1 = pixels1[row_n]
        row2 = pixels2[row_n]
        for j in range(0,width1):
            r1 = row1[4*j+0]
            g1 = row1[4*j+1]
            b1 = row1[4*j+2]
            a1 = row1[4*j+3]
            r2 = row2[4*j+0]
            g2 = row2[4*j+1]
            b2 = row2[4*j+2]
            a2 = row2[4*j+3]

            ro,go,bo,ao = kernel(blend_mode, r1,g1,b1,a1, r2,g2,b2,a2)
            rows[row_n][4*j+0] = ro
            rows[row_n][4*j+1] = go
            rows[row_n][4*j+2] = bo
            rows[row_n][4*j+3] = ao

    png_out = png.Writer(width=width1, height=height1, alpha=True, bitdepth=8, transparent=None)
    #print "writing out"
    f = open(out_png, 'wb')      # binary mode is important
    png_out.write(f, rows)
