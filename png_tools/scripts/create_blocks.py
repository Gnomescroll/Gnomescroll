#!/usr/bin/python

import os
import sys

def run():
    global block_tex,lblock
    f = open('./dat_blocks', 'r')

    file = f.read()

    l = file.rsplit("===")

    for n0 in l:
        if n0[0] == "*":
            n0 = ""
            continue
        block = lblock.new()
        lines = n0.rsplit("\n")
        for line in lines:
            if len(line) == 0:
                continue
            words = line.rsplit()
            if len(words) == 0:
                continue
            if len(words) == 1:
                continue
            if len(words) == 2:
                block.pinput(words[0], words[1])
            if len(words) == 3:
                block.pinput(words[0],words[1],words[2])
    print "=== texture list ==="
    print str(block_tex.texture_list())
    print "=== cube list ==="
    print str(lblock.block_list())

class Texture_set:
    def __init__(self, error_png="error.png", texture_dir="./block/"):
        self.texture_id_counter = 0
        self.texture_dir = texture_dir
        self.A = {}
        self.B = {}
        self.list = [(error_png,255)]
    def add_texture(self,tex):
        #check if texture file exists
        #if texture file does not exist, then return 255
        if not os.path.exists(self.texture_dir+tex):
            print "Error: texture file does not exist, %s" %(str(tex))
            self.A[tex] = 255
            return 255
        self.A[tex] = self.texture_id_counter
        self.B[self.texture_id_counter] = tex
        id= self.texture_id_counter
        self.list.append((tex,id))
        self.texture_id_counter +=1
        return id
    '''
    def get_texture_by_name(self, tex):
        temp = self.A.get(tex, None)
        if temp == None:
            print "Error: texture does not exist: %s" % (tex)
        return 255
    '''
    def g(self, tex):
        id = self.A.get(tex, None)
        if id == None:
            id = self.add_texture(tex)
        return id
    def texture_list(self):
        return self.list
    def texture_list(self):
        l = []
        for tex,id in self.list:
            l.append(tex)
        return l
    def _texture_list(self):  ##used as input for tile generation
        return self.list

class Block_list:
    def __init__(self):
        self.list = []
    def new(self):
        t = Block_template()
        self.list.append(t)
        return t
    def write_block_dat(self):
        dict= {}
        for t in self.list:
            dict = t
    def block_list(self):
        l = []
        for block in self.list:
            t = block.texture_list()
            if t != None:
                l.append(t)
        return l


hud_c = 65
def get_hud_id():
    global hud_c
    hud_c += 1
    return hud_c -1

class Block_template:
    def __init__(self):
        self.name = None
        self.id = None
        self.side = [255,255,255,255,255,255]
        self.hud_pos = -1
        self.hud_img = -1    #not handling undefined case
    def set_name(self,name):
        self.name = name
    def set_id(self, id):
        self.id = int(id)
    def set_hud(self,s2,s3):
        global hud_tex
        #self.hud_pos = int(s2)
        print "%s, %s" % (s2, s3)
        self.hud_pos = int(s2)
        self.hud_img = hud_tex.g(s3)    #eventually may want to use seperate one for hud
    def pinput(self, s1,s2,s3=None):
        global hud_tex, block_tex
        if s1 == "id":
            self.set_id(s2)
        elif s1 == "name":
            self.set_name(s2)
        elif s1 == "hud":
            self.set_hud(s2,s3)
        elif s1 == "top":
            self.side[0] = block_tex.g(s2)
        elif s1 == "bottom":
            self.side[1] = block_tex.g(s2)
        elif s1 == "west":
            self.side[2] = block_tex.g(s2)
        elif s1 == "east":
            self.side[3] = block_tex.g(s2)
        elif s1 == "north":
            self.side[4] = block_tex.g(s2)
        elif s1 == "south":
            self.side[5] = block_tex.g(s2)
        else:
            print "Invalid keyword: %s, %s, %s" %(str(s1),str(s2),str(s3))
    def texture_list(self):
        if self.name != None and self.id != None:
            return (self.name, self.id, self.side)
        else:
            return None
    def block_dictionary(self):
        pass

block_tex = Texture_set(error_png="error.png", texture_dir="./block/") #block textures
hud_tex = Texture_set(error_png="error.png", texture_dir="./block/")
lblock = Block_list()
run()

import sys
import os

print str(block_tex._texture_list())

print "Writing out block textures"
import spritesheet
sprite = spritesheet.Spritesheet("test", "./block/", block_tex._texture_list())
sprite.verify()
sprite.generate()
sprite.write_out("./blocks_01.png")
sprite.write_out("../netclient/media/texture/blocks_01.png")

print "Writing out block selector hud textures"
import spritesheet
hud_sprite = spritesheet.Spritesheet("test", "./block/", hud_tex._texture_list())
hud_sprite.verify()
hud_sprite.generate()
hud_sprite.write_out("./hud_block_selector.png")
hud_sprite.write_out("../netclient/media/texture/hud_block_selector.png")

### PASS IN USING ARRAY ###
'''
f = open("./conf/block_sprite_conf.py", "w")
f.write("sprites = ")
f.write(str(tex._texture_list()))
f.close
'''

dat = {
    0 : {
        'name': 'empty_space',
        'occludes' : False,
        'active' : False,
        'solid' : False,
        'max_damage': 32,
        'neutron_tolerance' : 2,
        'nuclear': 0,
        'texture_id': [-1]*6,
        'hud_pos' : -1,
        'hud_img' : -1,
        'texture_order': [[0,1,2,3]] * 6,
        'gravity'   :   0,
        'transparent': 0,
    },

}

default =    {
            'name'  :   'default',
            'occludes': True,
            'active':   True,
            'solid':    True,
            'max_damage' : 32,
            'neutron_tolerance' : 2,
            'nuclear' : 0,
            'texture_id': [255]*6,
            'hud_pos' : -1,
            'hud_img' : -1,
            'gravity'   :   0,
            'transparent': 0,
    }

for block in lblock.list:
    if block.name == None or block.id == None:
        continue
    x = {}
    x = default.copy()
    x['name'] = block.name
    x['texture_id'] = block.side
    x['hud_pos'] = block.hud_pos
    x['hud_img'] = block.hud_img
    dat[block.id] = x

#print str(dat)

import pprint
f = open("../server/dats/cube_dat.py", "w")
pp = pprint.PrettyPrinter(stream=f, indent=4)
#pp = pprint.PrettyPrinter(indent=4)
f.write("dat = ")
pp.pprint(dat)
