#!/usr/bin/python

import ImageFont, ImageDraw, Image
import argparse
import os

def expand_path(path):
    return os.path.abspath(os.path.expanduser(path))

chars = ' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'
fontpath = ''
fontsize = 10
image_size = (256,256)
cfile = ''

def get_args():
    parser = argparse.ArgumentParser(description='Dump ttf font ASCII charset to c file')
    parser.add_argument('font', help='font file')
    parser.add_argument('size', type=int, help='font size')

    args = parser.parse_args()

    global fontpath, cfile, fontsize, outfile

    fontpath = args.font
    fontsize = args.size
    cfile = os.path.basename(fontpath).split('.')[0] + '.c'

class CharData:
    x = 0
    y = 0
    tx = 0
    ty = 0
    char = ''
    index = 0
    
    def __str__(self):
        return repr(self)

    def __repr__(self):
        char = self.char
        if char in ["'", "\\"]:
            char = "\\" + char
        return "{%d,%d,%d,%d,'%s',%d}" % (self.x, self.y, self.tx, self.ty, char, self.index,)
    
def process_conf():
    global fontpath
    fontpath = expand_path(fontpath)
    assert os.path.exists(fontpath)

def write_char_data(data):
    struct_name = """struct CharData
{
    unsigned int x,y;
    unsigned int tx,ty;
    char c;
    int index;
};
"""

    charset_pre = 'const int char_lookup_table[128] = {'
    charset_suf = '};'
    
    n_chars = 'const unsigned int n_font_chars = %d;' % len(data)

    array_start = "const struct CharData char_data[%d] = {" % len(data)
    array_end = "};"

    str_data = [repr(d) for d in data] 
        
    with open(cfile, 'a') as f:
        f.write(charset_pre)
        f.write('\n')

        data_chars = [d.char for d in data]
        data_ords = [ord(c) for c in data_chars]
        data_indices = [d.index for d in data]
        mapping = []
        for i in range(128):
            if i in data_ords:
                index = data_ords.index(i)
                mapping.append(data_indices[index])
            else:
                mapping.append(-1)
                
        f.write(','.join(map(str,mapping)))

        f.write(charset_suf)
        f.write('\n')

        f.write(struct_name)
        f.write('\n')
        f.write(n_chars)
        f.write('\n')
        f.write(array_start)
        f.write('\n')
        f.write(',\n'.join(str_data))
        f.write('\n')
        f.write(array_end)
        f.write('\n')
        f.write('\n')

def write_pixel_data(chars, data, pixels):
    assert len(chars) > 0
    assert len(chars) == len(data)
     
    longest = max(map(lambda d: d.x*d.y, data))
    arr_len = 'const unsigned int n_font_pixels = %d;' % longest
    prefix = 'const unsigned char font_pixels[%d][%d] = {' % (len(chars), longest,)
    suffix = "};"
    
    init = """
        void load_pixel_data()
        {
            for (int i=0; i<n_chars; i++)
            {
                unsigned int size = char_data[i].x*char_data[i].y;
                pixels[i] = (unsigned char*)malloc(size * sizeof(unsigned char));
                for (int j=0; j<size; j++)
                    pixels[j] = 
            }
        }""" % ()

    for p in pixels:
        if len(p) < longest:
            p += [0 for i in range(longest-len(p))]
    
    str_data = ['{%s}' % ','.join(map(str,p)) for p in pixels]
    
    with open(cfile, 'a') as f:
        f.write(prefix)
        f.write('\n')
        f.write(',\n'.join(str_data))
        f.write(suffix)
        f.write('\n')
        f.write('\n')

def write_bmp_data(image):
    
    w = image_size[0]
    h = image_size[1]
    
    pixels = []
    for i in range(w):
        for j in range(h):
            pixels += map(str, list(image.getpixel((i,j))) + [255])
    
    wstr = 'const unsigned int bmp_w = %d;' % w
    hstr = 'const unsigned int bmp_h = %d;' % h
    prefix = 'const unsigned char bmp_data[%d] = {' % (w*h*4)
    suffix = '};'
    
    with open(cfile, 'a') as f:
        f.write(wstr)
        f.write('\n')
        f.write(hstr)
        f.write('\n')
        f.write(prefix)
        f.write('\n')
        f.write(','.join(pixels))
        f.write('\n')
        f.write(suffix)
        f.write('\n')
        f.write('\n')

def main():

    image = Image.new("RGB", image_size)

    draw = ImageDraw.Draw(image)

    # use a truetype font
    font = ImageFont.truetype(fontpath, fontsize)

    char_pixels = []
    data = []

    cursor_x = 0
    cursor_y = 0
    for i, c in enumerate(chars):
        fsize = font.getsize(c)
        if cursor_x + fsize[0] > image_size[0]:
            cursor_y += fsize[1]
            cursor_x = 0

        assert cursor_x < image_size[0]
        assert cursor_y + fsize[1] < image_size[1]
        
        draw.text((cursor_x,cursor_y), c, font=font)

        # dump coordinate data to struct
        d = CharData()
        d.x = fsize[0]
        d.y = fsize[1]
        d.tx = cursor_x
        d.ty = cursor_y
        d.char = c
        d.index = i
        data.append(d)

        # copy pixel data
        size = d.x * d.y
        assert size > 0
        pixels = []
        for j in range(size):
            x = j % d.x
            y = j // d.x
            x += cursor_x
            y += cursor_y
            pix = image.getpixel((x,y))
            if sum(pix) > 0:
                pix = 1
            else:
                pix = 0
            pixels.append(pix)
        char_pixels.append(pixels)
        
        cursor_x += fsize[0]

    with open(cfile, 'w') as f:
        pass
    write_char_data(data)
    #write_pixel_data(chars, data, char_pixels)
    write_bmp_data(image)
    
    image.save('test.bmp')
    
if __name__ == '__main__':
    get_args()
    process_conf()
    main()
