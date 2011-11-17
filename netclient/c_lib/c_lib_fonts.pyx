import os.path

cdef extern from "fonts.h":
    pass

# part .fnt file
# load png as texture
# takes a filename, adds extension and path

class Font:

    font_path = "./media/fonts/"
    font_ext = ".fnt"
    png_ext = "_0.png"
    glyphs = {}
    info = {}

    def __init__(self, fn):
        self.fontfile = fn
        self.process_font_filename(fn)

    def process_font_filename(self):
        fn = self.fontfile
        fn = fn.split('.')[0]
        fn += font_ext
        fn = font_path + fn
        self.fontfile = fn

    def parse(self):
        png = ""
        
        if not os.path.exists(fp):
            print "ERROR c_lib_fonts.pyx :: cannot find font file %s in %s" % (fn, font_path,)
            return

        # parse .fnt
        with open(fp) as f:
            lines = f.readlines()
            print "FONT INFO: %s %s" % (lines[0],lines[1])
            for line in lines:
                tags = line.split()
                name = tags[0]
                tags = dict(map(lambda a: a.split('='), tags[1:]))

                if name == 'page':
                    png = tags['file']
                elif name == 'info':
                    font_info.update(tags)
                    print "Font: %s" % (line,)
                elif name == 'common':
                    font_info.update(tags)
                    print "Font: %s" % (line,)
                elif name == 'chars':
                    print '%s characters in font set' % (tags['count'],)
                elif name == 'char':
                    glyphs[int(tags['id'])] = tags

        # process png filename
        if not png:
            png = self.fontfile + self.png_ext
        fp_png = font_path + png
        if not os.path.exists(fp_png):
            print "ERROR c_lib_fonts.pyx :: cannot find font png file %s in %s" % (fn, font_path,)
            return
        self.pngfile = fp_png

def init():
    if not os.path.exists(Font.font_path):
        print "ERROR c_lib_fonts.pyx :: cannot find font path %s" % (font_path,)
        return

    import opts
    font = Font(opts.opts.font)
    font.parse()

