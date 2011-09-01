
'''
channels = [
    ['client_messages', 0],
]
'''

def run():
    struct_list = []
    f = open('./messages/test', 'r')

    file = f.read()

    l = file.rsplit("---")
    for n0 in l:
        if n0[0] == "*":
            n0 = ""
            continue
        s = Struct()
        struct_list.append(s)
        n1 = n0.rsplit("\n")
        for n2 in n1:
            if len(n2) == 0:
                continue
            s.insert_member(n2)
        #print n1

    x,y,z,z2 = "\n","\n","\n","\n"
    for s in struct_list:
        z = s.print_def()
    for s in struct_list:
        x+=s.struct_size()
    for s in struct_list:
        y+=s.packing_def()
        y+=s.unpacking_def()
    print x
    print y

    size_out = open("./mgen/size.h","w")
    size_out.write(x)
    struct_def = open("./mgen/struct_def.h", "w")
    struct_def.write(z)
    struck_packing = open("./mgen/struct_packing.h", "w")
    struct_def.write(y)

type_list = ["n","un","s","us","b","f"]

def struct_proc(list):
    n = list[1];
    nn = list[2];
    if n == "n":
        return "int32_t %s" % (nn)
    if n == "un":
        return "uint32_t %s" % (nn)
    if n == "s":
        return "int16_t %s" % (nn)
    if n == "us":
        return "uint16_t %s" % (nn)
    if n == "b":
        return "uint8_t %s" % (nn)
    if n == "f":
        return "float %s" % (nn)
    print "ERROR: Type Missing: %s" % (n)
    assert False

def pack_proc(list):
    n = list[1];
    nn = list[2];
    m = ""
    if n == "n":
        m = "int32_t"
    if n == "un":
        m = "uint32_t"
    if n == "s":
        m ="int16_t"
    if n == "us":
        m = "uint16_t"
    if n == "b":
        m = "uint8_t"
    if n == "f":
        m = "float"
    if m != "":
        return "\tPACK_%s(s->%s, buffer, &n);\n" % (m,nn)
    print "ERROR: Type Missing: %s" % (n)
    assert False

def pack_proc(list):
    n = list[1];
    nn = list[2];
    m = ""
    if n == "n":
        m = "int32_t"
    if n == "un":
        m = "uint32_t"
    if n == "s":
        m ="int16_t"
    if n == "us":
        m = "uint16_t"
    if n == "b":
        m = "uint8_t"
    if n == "f":
        m = "float"
    if m != "":
        return "\tUNPACK_%s(s->%s, buffer, &n);\n" % (m,nn)
    print "ERROR: Type Missing: %s" % (n)
    assert False

class Struct:
    def __init__(self):
        self.name = None
        self.n = 0
        self.members = []

    def insert_member(self, string):
        global type_list
        l = string.rsplit(None) #split on whitespace
        if len(l) == 0:
            return
        elif(len(l) == 1):
            print "Error! Single Symbol on Line, %s" % (str(l))
        elif l[0] == "struct":
            self.name = l[1]
            print "struct %s" % (self.name)
        elif not l[0] in type_list:
            print "Invalid type: %s" % (str(l))
        else:
            self.members.insert(self.n, [self.n] + l)
            self.n += 1

    def print_def(self):
        if self.name == None:
            return
        else:
            self.struct_def()
            self.packing_def()
            self.unpacking_def()
            self.send_function()

    def struct_def(self):
        if self.name == None:
            return ''
        p1 = "struct %s { \n" % (self.name)
        p2 = ""
        for i in self.members:
            p2 += "\t%s;\n" % (struct_proc(i))
        p3 = "};\n"
        return p1+p2+p3

    def struct_size(self):
        if self.name == None:
            return ''
        return "printf(" +'"'+ "sizeof(%s)="%(self.name)+'%i' +'", ' +"sizeof(struct %s));" % (self.name)

    def packing_def(self):
        if self.name == None:
            return ''
        s1 = "int pack_struct_%s" %(self.name) + "(void* buffer, struct %s* s) { \n" % (self.name)
        s1 += "\tint n=0;\n"
        s2 = ''
        for i in self.members:
            s2 += pack_proc(i)
        s3 = "\treturn n;\n}\n"
        return s1+s2+s3

    def unpacking_def(self):
        if self.name == None:
            return ''
        s1 = "void unpack_struct_%s" %(self.name) + "(void* buffer, struct %s* s) { \n" % (self.name)
        s1 += "\tint n=0;\n"
        s2 = ''
        for i in self.members:
            s2 += unpack_proc(i)
        s3 = "\treturn n;\n}\n"
        return s1+s2+s3

    def send_function(self):
        pass

run()
