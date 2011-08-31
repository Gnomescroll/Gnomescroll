
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
    for s in struct_list:
        s.print_def()
    print ""
    for s in struct_list:
        s.struct_size()
    print ""
    for s in struct_list:
        s.packing_def()
    print ""
    for s in struct_list:
        s.unpacking_def()

type_list = ["n","un","s","us","b"]

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

    print "Type Missing: %s" % (n)
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
        m ="uint8_t"
    if m != "":
        return "\tPACK_%s(s->%s, buffer, &n);\n" % (m,nn)
    print "Type Missing: %s" % (n)
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
            return
        p1 = "struct %s { \n" % (self.name)
        p2 = ""
        for i in self.members:
            p2 += "\t%s;\n" % (struct_proc(i))
        p3 = "};\n"
        print p1+p2+p3

    def struct_size(self):
        if self.name == None:
            return
        print "printf(" +'"'+ "sizeof(%s)="%(self.name)+'%i' +'", ' +"sizeof(struct %s);" % (self.name)

    def packing_def(self):
        if self.name == None:
            return
        s1 = "int pack_struct_%s" %(self.name) + "(void* buffer, struct %s* s) { \n" % (self.name)
        s1 += "\tint n=0;\n"
        s2 = ''
        for i in self.members:
            s2 += pack_proc(i)
        s3 = "\treturn n;\n}\n"
        print s1+s2+s3

    def unpacking_def(self):
        pass

    def send_function(self):
        pass

run()
