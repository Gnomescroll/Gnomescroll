
from gimpfu import *
from commands import getstatusoutput
import os
import time
import tempfile
import logging

#print "%02d" % (1,)

#gimp -i --batch-interpreter python-fu-eval --batch - < postprocess.py

# Set up file paths
testdir = './'

# put test results in a tmp directory (typically in /tmp)

#tmpdir = "out_images/"

indir = "./"
#referencedir = testdir + 'reference_out_images/'

outdir = "./"

def drawable_of_file(filename):
    '''
    Load a file and return the the drawable.
    A call to this can be used as an actual parameter, see instance below.
    '''
    image = pdb.gimp_file_load(filename, filename, run_mode=RUN_NONINTERACTIVE)
    #pdb.gimp_image_flatten(image)
    drawable = pdb.gimp_image_get_active_layer(image)
    return drawable

def runtest(filename, testname, testcommand, testparameters, out):
    infilepath = indir + filename
    # since some tests use the same input file, cat the testname to make an outfilename
    # out and reference have same name, different directories
    outfilepath = outdir + out + '.png'

    # open test file
    image = pdb.gimp_file_load(infilepath, infilepath, run_mode=RUN_NONINTERACTIVE)
    drawable = pdb.gimp_image_get_active_layer(image)

    #SELECTION
    '''
    if is_selection:
        # Make selection    x,y, width, height
        pdb.gimp_rect_select(image, 100, 90, 100, 50, 0, False, 0)
    '''

    num_seg = 10
    seg_array = [16.5,0.0, 0.0,16.5, 16.5,32.0, 32.0,16.5, 16.5,0.0]
    #seg_array = [0.0,0.0, 10.0,0.0, 10.0,10.0, 0.0,10.0, 0.0,0.0]
    antialias = 0
    feather = 0
    feather_radius = 0
    pdb.gimp_free_select(image, num_seg, seg_array, 2, antialias, feather, feather_radius)

    # Build a test string
    teststring = testcommand + "(image, drawable," + testparameters + ",run_mode=RUN_NONINTERACTIVE)"
    logging.info("Test string:" + teststring)

    # Invoke the test
    # Formerly: eval(teststring) but eval only takes expressions, not statements
    start = time.time()
    try:
        exec teststring
    except RuntimeError:
        print "EXCEPTION"

    #logging.info("Processor time: " + str(time.time() - start))
    print "Processor time: %s" % ( str(time.time() - start ))

    #post processing

    # !!! Refresh drawable in case the plugin returned a new image
    drawable = pdb.gimp_image_get_active_drawable(image)

    pdb.gimp_file_save(image, drawable, outfilepath, outfilepath, run_mode=RUN_NONINTERACTIVE)
    pdb.gimp_image_delete(image)


def main():
    # Open the log
    LOG_FILENAME = 'resynth-test.log'
    logging.basicConfig(filename=LOG_FILENAME,level=logging.DEBUG)

    # Make paths to various input files
    grasspath = indir + 'grass-input-alpha.png'

    # Straight resynthesis, from separate image with no selection into selection in drawable
    test = "pdb.plug_in_resynthesizer"
    # !!! Note we want the ID of the drawable

    #parameters = "0,0, True, drawable_of_file('"+ grasspath + "').ID, -1, -1, 0.0, 0.117, 16, 500"

    for _i in range(0,16):
        parameters = "0,0, True, drawable_of_file('"+ "source_texture.png" + "').ID, -1, -1, 0.0, 0.117, 16, 500"
        runtest('./_out_%i.png' % (_i), 'resynthtwoimages', test, parameters, out="img_%i"%(_i))

    #pdb.gimp_quit(True)

main()

def empty_128():
    rows = []
    i2=0
    for i in range(0,128):
        rows.insert(i,[])
        rows[i] = [0,0,0,0] * 128
    return rows

import png

tileset = empty_128()
debugset = empty_128()

for _i in range(0,16):

    x = png.Reader(filename = 'img_%i.png' % (_i))
    width, height, pixels, meta = x.asRGBA8()

    xoff = 32 * (_i%4)
    yoff = 32 * int( (_i-(_i%4))/4)
    i = 0
    j= 0
    for row in pixels:
        for j in range(32): #x is x
            r = row[4*(j)+0]
            g = row[4*(j)+1]
            b = row[4*(j)+2]
            a = row[4*(j)+3]
            tileset[i+yoff][4*(j+xoff)+0] = r
            tileset[i+yoff][4*(j+xoff)+1] = g
            tileset[i+yoff][4*(j+xoff)+2] = b
            tileset[i+yoff][4*(j+xoff)+3] = a
        i += 1

    x = png.Reader(filename = '_out_%i.png' % (_i))
    width, height, pixels, meta = x.asRGBA8()
    xoff = 32 * (_i%4)
    yoff = 32 * int( (_i-(_i%4))/4)
    i = 0
    j= 0
    for row in pixels:
        for j in range(32): #x is x
            r = row[4*(j)+0]
            g = row[4*(j)+1]
            b = row[4*(j)+2]
            a = row[4*(j)+3]
            debugset[i+yoff][4*(j+xoff)+0] = r
            debugset[i+yoff][4*(j+xoff)+1] = g
            debugset[i+yoff][4*(j+xoff)+2] = b
            debugset[i+yoff][4*(j+xoff)+3] = a
        i += 1

png_out = png.Writer(width=128, height=128, alpha=True, bitdepth=8, transparent=None)
print "writing out"
with open('./wang_array.png', 'wb') as f:      # binary mode is important
    png_out.write(f, tileset)
with open('./debug_array.png', 'wb') as f:      # binary mode is important
    png_out.write(f, debugset)


pdb.gimp_quit(True)
