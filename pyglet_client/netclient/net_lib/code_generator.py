
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

    output_struct_def(struct_list)
    output_packing_def(struct_list)
    output_size_def(struct_list)

def header_include_guard(st, name):
    s1 = "#ifndef %s_h \n#define %s_h \n\n" % (name, name)
    s2 = "#endif\n"
    return s1+st+s2

#include "./type_pack.h"
def output_struct_def(struct_list):
    x= "#include <stdlib.h> \n#include <stdint.h> \n#include \"../type_pack.h\"\n\n"
    for s in struct_list:
        x+= s.struct_def()
    #x += "\n"
    x = header_include_guard(x, "net_lib_mgen_struct_def")
    struct_def = open("./mgen/struct_def.h", "w")
    struct_def.write(x)
    print x

def output_packing_def(struct_list):
    x="#include <stdlib.h> \n#include <stdint.h> \n#include \"../type_pack.h\"\n\n"
    x+= "#include \"./struct_def.h\" \n\n"
    for s in struct_list:
        x+=s.packing_def()
        x+=s.unpacking_def()
    #x += "\n"
    x = header_include_guard(x, "net_lib_mgen_struct_packing")
    struck_packing = open("./mgen/struct_packing.h", "w")
    struck_packing.write(x)
    print x

def output_size_def(struct_list):
    x="#include <stdlib.h> \n#include <stdint.h> \n#include \"../type_pack.h\"\n\n"
    x+= "void DEBUG_net_message_size() { \n"
    x+="\tprintf("+'"'+ "ramsize, netsize" +"\\n\");\n"
    for s in struct_list:
        x+=s.struct_size()
    x += "}\n"
    #x += "\n"
    x = header_include_guard(x, "net_lib_mgen_size")
    struck_size = open("./mgen/size.h", "w")
    struck_size.write(x)
    print x

def print_transmission_template(struct_list):
    x= "\n"
    for s in struct_list:
        x+=s.transmission_template()
    x += "\n"
    #x += "\n"
    x = header_include_guard(x, "net_lib_mgen_size")
    struck_size = open("./mgen/template.h", "w")
    struck_size.write(x)
    print x

type_list = ["n","un","s","us","b","f"]

def type_to_name(n): #take type notation and convert to name
    if n == "n":
        return "int32_t"
    elif n == "un":
        return "uint32_t"
    elif n == "s":
        return "int16_t"
    elif n == "us":
        return "uint16_t"
    elif n == "b":
        return "uint8_t"
    elif n == "f":
        return "float"
    else:
        print "Type not handled, %s" % (type)
        assert False

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
        return "\tPACK_%s(s->%s, buffer, n);\n" % (m,nn)
    print "ERROR: Type Missing: %s" % (n)
    assert False

def struct_template(list):
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
        return "\tPACK_%s(s->%s, buffer, n);\n" % (m,nn)
    print "ERROR: Type Missing: %s" % (n)
    assert False

def unpack_proc(list):
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
        return "\tUNPACK_%s(&s->%s, buffer, n);\n" % (m,nn)
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
        p3 = "};\n\n"
        return p1+p2+p3

    def struct_size(self):
        if self.name == None:
            return ''
        n = ""
        for i in self.members:
            n += "sizeof(%s)+" % (type_to_name(i[1]))
        n += "0"
        s1 ="\tprintf(" +'"'+ "sizeof(%s)="%(self.name)+'%i, %i\\n' +'", ' +"sizeof(struct %s),%s);\n" % (self.name,n)
        return s1 #+s2+s3

    def packing_def(self):
        if self.name == None:
            return ''
        s1 = "static inline void pack_struct_%s" %(self.name) + "(unsigned char* buffer, struct %s* s, int* n) { \n" % (self.name)
        #s1 += "\tint n=0;\n"
        s2 = ''
        for i in self.members:
            s2 += pack_proc(i)
        #s3 = ''
        n = ""
        for i in self.members:
            n += "sizeof(%s)+" % (type_to_name(i[1]))
        n += "0;"
        #s3 = "\t*n+=%s" % (n)
        s3 = "\t//*n += %s;\n}\n\n" %(n)

        return s1+s2+s3

    def unpacking_def(self):
        if self.name == None:
            return ''
        s1 = "static inline void unpack_struct_%s" %(self.name) + "(unsigned char* buffer, struct %s* s, int* n) { \n" % (self.name)
        #s1 += "\tint n=0;\n"
        s2 = ''
        self.members.reverse()
        for i in self.members:
            s2 += unpack_proc(i)
        self.members.reverse()
        s3 = ''
        n = ""
        for i in self.members:
            n += "sizeof(%s)+" % (type_to_name(i[1]))
        n += "0;"
        s3 = "\t//*n += %s;\n}\n\n" %(n)

        #s3 = "\t*n += %s" % (n)
        #s4 = "\n}\n\n"
        return s1+s2+s3 #+s4

    def trasmission_template(self):
        if self.name == None:
            return ''
        s1 = "{ \n"
        s1 = "\tstruct %s s;\n" %(self.name) + "(unsigned char* buffer, struct %s* s, int* n) { \n" % (self.name)
        #s1 += "\tint n=0;\n"
        s2 = ''
        for i in self.members:
            s2 += struct_template(i)

        s3 = "/tpack_struct_%s(buff, &s, &n); \n\n" %(self.name)
        #s3 = "\t*n+=%s" % (n)
        return s1 + s2 + s3

    def send_function(self):
        pass

run()
