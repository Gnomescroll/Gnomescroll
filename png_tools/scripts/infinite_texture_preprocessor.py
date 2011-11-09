#!/usr/bin/python

import png
import sys
import os.path


#print "%02d" % (1,)

#open a png file

texture_dir = "./infinite_texture/"

texture_source_dirs = []
for x in os.listdir(texture_dir):
    if os.path.isdir(texture_dir + x):
        print texture_dir + x + "/"
        texture_source_dirs.append(texture_dir + x + "/")
        try:
            #os.mkdir(texture_dir + x + "/temp")
            os.mkdir(texture_dir + x + "/out")
        except:
            pass
#exit()

for tex_dir in texture_source_dirs:

    import random

    print "Processing %s" % (tex_dir)

    seed = None

    random_x1 = -1
    random_y1 = -1

    random_x2 = -1
    random_y2 = -1

    try:
        execfile(tex_dir+"config.py") #should have seed = number
        random.seed(seed)
    except:
        print "No config.py file"
        random.seed(1) #use default seed

    if os.path.isfile(tex_dir+'source_tile_1.png') and os.path.isfile(tex_dir+'source_tile_2.png'):
        print "Using user defined intial tiles"
        user_source_tiles = 1
    else:
        user_source_tiles = 0
        print "No user defined initial tiles: generating source tiles from source_texture.png"
        if random_x1 == -1 or random_y1 == -1:
            print "Using Random source tiles from source_texture.png: seed = %s" % (str(seed))

            x = png.Reader(filename = tex_dir+"source_texture.png")
            #get x,y dimensions
            width, height, pixels, meta = x.asRGBA8()

            random_x1 = random.randint(0, width-32)
            random_y1 = random.randint(0, height-32)

            random_x2 = random.randint(0, width-32)
            random_y2 = random.randint(0, height-32)
        else:
            print "Using user defined source tile cordinates"
    ##add support for user defined color1 and color2

    '''
    random_x1 = 0
    random_y1 = 0

    random_x2 = 32
    random_y2 = 32
    '''
    def empty_32():
        rows = []
        i2=0
        for i in range(0,32):
            rows.insert(i,[])
            rows[i] = [0,0,0,0] * 32
        return rows

    def empty_64():
        rows = []
        i2=0
        for i in range(0,64):
            rows.insert(i,[])
            rows[i] = [0,0,0,0] * 64
        return rows



    if user_source_tiles == 0:

        print "source width, height = %i, %i" % (width, height)
        print "source_tile_1: x1, y1 = %i, %i" % (random_x1, random_y1)
        print "source_tile_2: x2, y2 = %i, %i" % (random_x2, random_y2)

        print "Creating Source Tiles From Source Texture"

        #print "Moving source image into array"
        pixel2 = []
        for i in range(0,height):
            pixel2.insert(i,[])
            pixel2[i] = [0,0,0,0] * width
        #print "stage 1 done"
        i = 0
        for row in pixels:
            for j in range(0,width): #x is x
                r = row[4*(j)+0]
                g = row[4*(j)+1]
                b = row[4*(j)+2]
                a = row[4*(j)+3]
                pixel2[i][4*j+0] = r
                pixel2[i][4*j+1] = g
                pixel2[i][4*j+2] = b
                pixel2[i][4*j+3] = a
            i += 1

        #print "Source image copied"


        color1 = empty_32()
        color2 = empty_32()



        #load 32x32 from source textures

        #print "Loading color 1"
        #load color1
        xoff,yoff = random_x1, random_y1
        i = 0
        j= 0

        for row in color1:
            for j in range(32): #x is x
                r = pixel2[i+yoff][4*(j+xoff)+0]
                g = pixel2[i+yoff][4*(j+xoff)+1]
                b = pixel2[i+yoff][4*(j+xoff)+2]
                a = pixel2[i+yoff][4*(j+xoff)+3]
                color1[i][4*j+0] = r
                color1[i][4*j+1] = g
                color1[i][4*j+2] = b
                color1[i][4*j+3] = a
            i += 1

        #print "Loading color 2"
        #load color 2
        xoff,yoff = random_x2, random_y2
        i = 0
        j= 0
        for row in color2:
            for j in range(32): #x is x
                r = pixel2[i+yoff][4*(j+xoff)+0]
                g = pixel2[i+yoff][4*(j+xoff)+1]
                b = pixel2[i+yoff][4*(j+xoff)+2]
                a = pixel2[i+yoff][4*(j+xoff)+3]
                color2[i][4*j+0] = r
                color2[i][4*j+1] = g
                color2[i][4*j+2] = b
                color2[i][4*j+3] = a
            i += 1

    if user_source_tiles == 1:
        print "Loading User Defined Source Tiles"

        color1 = empty_32()
        color2 = empty_32()

        x = png.Reader(filename = tex_dir+"source_tile_1.png")
        #get x,y dimensions
        width, height, pixels, meta = x.asRGBA8()

        pixel2 = []
        for i in range(0,32):
            pixel2.insert(i,[])
            pixel2[i] = [0,0,0,0] * 32
        #print "stage 1 done"
        i = 0
        for row in pixels:
            for j in range(0,32): #x is x
                r = row[4*(j)+0]
                g = row[4*(j)+1]
                b = row[4*(j)+2]
                a = row[4*(j)+3]
                color1[i][4*j+0] = r
                color1[i][4*j+1] = g
                color1[i][4*j+2] = b
                color1[i][4*j+3] = a
            i += 1

        x = png.Reader(filename = tex_dir+"source_tile_2.png")
        #get x,y dimensions
        width, height, pixels, meta = x.asRGBA8()

        pixel2 = []
        for i in range(0,32):
            pixel2.insert(i,[])
            pixel2[i] = [0,0,0,0] * 32
        #print "stage 1 done"
        i = 0
        for row in pixels:
            for j in range(0,32): #x is x
                r = row[4*(j)+0]
                g = row[4*(j)+1]
                b = row[4*(j)+2]
                a = row[4*(j)+3]
                color2[i][4*j+0] = r
                color2[i][4*j+1] = g
                color2[i][4*j+2] = b
                color2[i][4*j+3] = a
            i += 1

    png_out = png.Writer(width=32, height=32, alpha=True, bitdepth=8, transparent=None)
    with open(tex_dir+'out/source_tile_1.png', 'wb') as f:      # binary mode is important
        png_out.write(f, color1)
    with open(tex_dir+'out/source_tile_2.png', 'wb') as f:      # binary mode is important
        png_out.write(f, color2)


    intermediate_tile = empty_64()

    _list = []

    mlist = [
    1,1,1,0,
    1,0,0,1,
    0,1,0,0,
    1,1,0,0,

    0,1,0,1,
    1,0,0,0,
    0,0,0,0,
    0,0,1,0,

    1,0,1,1,
    0,0,1,1,
    0,0,0,1,
    0,1,1,0,

    1,1,1,1,
    1,1,0,1,
    1,0,1,0,
    0,1,1,1,
    ]

    for _i in range(0,16):
        c1 = mlist[_i*4+0]
        c2 = mlist[_i*4+1]
        c3 = mlist[_i*4+2]
        c4 = mlist[_i*4+3]
        temp = []

        if(c1 == 0):
            temp.insert(0, color1)
        elif c1 ==1:
            temp.insert(0, color2)

        if(c2 == 0):
            temp.insert(1, color1)
        elif c2 ==1:
            temp.insert(1, color2)

        if(c3 == 0):
            temp.insert(2, color1)
        elif c3 ==1:
            temp.insert(2, color2)

        if(c4 == 0):
            temp.insert(3, color1)
        elif c4 ==1:
            temp.insert(3, color2)

        _list.insert(_i, temp)

    scratch = empty_64()
    result = empty_32()

    _i = 0
    for _n in _list:
        c1,c2,c3,c4 = _n
        #print str(_n)

        #upper left
        xoff,yoff = 0,0
        i = 0
        j= 0
        for i in range(32):
            for j in range(32): #x is x
                r = c1[i][4*(j)+0]
                g = c1[i][4*(j)+1]
                b = c1[i][4*(j)+2]
                a = c1[i][4*(j)+3]
                scratch[i+yoff][4*(j+xoff)+0] = r
                scratch[i+yoff][4*(j+xoff)+1] = g
                scratch[i+yoff][4*(j+xoff)+2] = b
                scratch[i+yoff][4*(j+xoff)+3] = a
            i += 1

        #upper right
        xoff,yoff = 32,0
        i = 0
        j= 0
        for i in range(32):
            for j in range(32): #x is x
                r = c2[i][4*(j)+0]
                g = c2[i][4*(j)+1]
                b = c2[i][4*(j)+2]
                a = c2[i][4*(j)+3]
                scratch[i+yoff][4*(j+xoff)+0] = r
                scratch[i+yoff][4*(j+xoff)+1] = g
                scratch[i+yoff][4*(j+xoff)+2] = b
                scratch[i+yoff][4*(j+xoff)+3] = a
            i += 1

        #bottom right
        xoff,yoff = 32,32
        i = 0
        j= 0
        for i in range(32):
            for j in range(32): #x is x
                r = c3[i][4*(j)+0]
                g = c3[i][4*(j)+1]
                b = c3[i][4*(j)+2]
                a = c3[i][4*(j)+3]
                scratch[i+yoff][4*(j+xoff)+0] = r
                scratch[i+yoff][4*(j+xoff)+1] = g
                scratch[i+yoff][4*(j+xoff)+2] = b
                scratch[i+yoff][4*(j+xoff)+3] = a
            i += 1

        #bottom left
        xoff,yoff = 0,32
        i = 0
        j= 0
        for row in color2:
            for j in range(32): #x is x
                r = c4[i][4*(j)+0]
                g = c4[i][4*(j)+1]
                b = c4[i][4*(j)+2]
                a = c4[i][4*(j)+3]
                scratch[i+yoff][4*(j+xoff)+0] = r
                scratch[i+yoff][4*(j+xoff)+1] = g
                scratch[i+yoff][4*(j+xoff)+2] = b
                scratch[i+yoff][4*(j+xoff)+3] = a
            i += 1

        xoff,yoff = 16,16
        i = 0
        j= 0
        for row in color2:
            for j in range(32): #x is x
                r = scratch[i+yoff][4*(j+xoff)+0]
                g = scratch[i+yoff][4*(j+xoff)+1]
                b = scratch[i+yoff][4*(j+xoff)+2]
                a = scratch[i+yoff][4*(j+xoff)+3]
                result[i][4*(j)+0] = r
                result[i][4*(j)+1] = g
                result[i][4*(j)+2] = b
                result[i][4*(j)+3] = a
            i += 1

        png_out = png.Writer(width=32, height=32, alpha=True, bitdepth=8, transparent=None)
        with open(tex_dir+'out/_%02d.png' % (_i), 'wb') as f:      # binary mode is important
            png_out.write(f, result)
        '''
        png_out2 = png.Writer(width=64, height=64, alpha=True, bitdepth=8, transparent=None)
        with open(tex_dir+'temp/out_%02d.png' % (_i), 'wb') as f:      # binary mode is important
            png_out2.write(f, scratch)
        '''
        _i += 1

#import postprocess

#postprocess.main()
