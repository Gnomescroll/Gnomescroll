from block_dat import dat


import pprint


f = open("../../server/dats/cube_dat.py", "w")
pp = pprint.PrettyPrinter(stream=f, indent=4)

f.write("dat = ")
pp.pprint(dat)
